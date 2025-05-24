#include "audioHandling.hpp"

float Voice::midiToFreq(int _note){
    return 440.f * exp((log(2) * (_note - 69))/12); 
}  

Voice::Voice(int _sampleRate, int _bitDepth, int _numVoices){
    sampleRate = _sampleRate; 
    bitDepth = _bitDepth; 
    numVoices = _numVoices; 

    frequency = new float[numVoices]; 
    amp = new float[numVoices]; 

    for (int i = 0; i < numVoices; i++){
        frequency[i] = 200.f; 
        amp[i] = 0.2f; 
    }

    if (bitDepth == 16){
        maxAmp = pow(2, bitDepth - 1) - 1; 
    } else if (bitDepth == 32){
        maxAmp = 1.f; 
    }
}

void Voice::deallocate(){
    delete[] frequency; 
    delete[] amp; 
}

void Voice::setFrequency(float _frequency, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = _frequency; 
    }
}

void Voice::setFrequencyMidi(float _note, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = midiToFreq(_note); 
    }
}

/* ******************************************************************************** */

void SineOscillator::init(){
	offset = new float[numVoices]; 
	incr = new float[numVoices]; 
}

void SineOscillator::updateParams(){
	for (int i = 0; i < numVoices; i++){
		incr[i] = frequency[i]/sampleRate; 
	}
}

float SineOscillator::genValue(){
	float out = 0.f; 
	for (int i = 0; i < numVoices; i++){
		out += amp[i] * sinf(2 * M_PI * offset[i]); 
		if ((offset[i] += incr[i]) >= 1.f) offset[i] = 0.f; 
	}
	return out; 
}

SineOscillator::~SineOscillator(){
	delete[] offset; 
	delete[] incr; 
}

/* ******************************************************************************** */
void Event::addToQueue(std::string id, float _newVal, int _voice){
    queue.push_back(possibleEvents[glossary.at(id)]); 
    queueData.push_back(std::make_pair(_newVal, _voice)); 
}

void Event::triggerEvent(){
    if (queue.size() ==0) std::cout << "nothing added to queue\n"; 
    else {
        queue.back()(queueData.back().first, queueData.back().second); 
        queue.pop_back(); 
        queueData.pop_back(); 
    }
}

/* ******************************************************************************** */

bool Audio::init(int sampleRate, int framesPerBuffer)
{
	paErr = Pa_OpenDefaultStream(&paStream, 0, 1, paFloat32, sampleRate, 
		framesPerBuffer, callback, &audioCallback); 
	if (checkPaError(paErr)) return false; 

    return true;   
}

bool Audio::startAudio(){
    paErr = Pa_StartStream(paStream); 
	if (checkPaError(paErr)) return false; 

    return true;
}

bool Audio::deinit(){
    paErr = Pa_StopStream(paStream); 
	if (checkPaError(paErr)) return false; 
	paErr = Pa_CloseStream(paStream); 
	if (checkPaError(paErr)) return false; 
	paErr = Pa_Terminate(); 
	if (checkPaError(paErr)) return false; 

    return true; 
}

bool Audio::checkPaError(PaError err){
    if (err != paNoError){
		printf("caught pa error: %s", Pa_GetErrorText(err));
		Pa_Terminate();
		return false;
	}
	return true; 
}

int Audio::callback(const void* inputBuffer, void* outputBuffer, 
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	PaStreamCallbackFlags flags, void* userInfo)
{
    float *out = (float*) outputBuffer; 

	std::function<float()>* audioGenFunc = (std::function<float()>*) userInfo; 
	(void) inputBuffer; 

	for (int i = 0; i < framesPerBuffer; i++){
		float outVal = (*audioGenFunc)(); 
		*out++ = outVal; 
	}

	return 0; 
}

/* ******************************************************************************** */
