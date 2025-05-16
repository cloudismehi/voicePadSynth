#include "audioHandling.hpp"

void Voice::calculateOffset(){
    for (int i = 0; i < numVoices; i++){
        offset[i] = (float)frequency[i] / sampleRate; 
    }
}

float Voice::midiToFreq(int _note){
    return 440.f * exp((log(2) * (_note - 69))/12); 
}  

Voice::Voice(int _sampleRate, int _bitDepth, int _numVoices){
    sampleRate = _sampleRate; 
    bitDepth = _bitDepth; 
    numVoices = _numVoices; 

    offset = new float[numVoices]; 
    idx = new float[numVoices]; 
    frequency = new float[numVoices]; 
    amp = new float[numVoices]; 

    for (int i = 0; i < numVoices; i++){
        frequency[i] = 200.f; 
        amp[i] = 0.2f; 
        idx[i] = 0.f; 
    }

    calculateOffset(); 
    
    if (bitDepth == 16){
        maxAmp = pow(2, bitDepth - 1) - 1; 
    } else if (bitDepth == 32){
        maxAmp = 1.f; 
    }
    
    // for (int i = 0; i < numVoices; i++){
    //     std::cout << "frequency " << frequency[i] << "(" << frequency << ")" << " amp " << amp[i] << "(" << amp << ")" << '\n'; 
    //     std::cout << "offset " << offset[i] << "(" << offset << ")"  << " idx " << idx[i] << "(" << idx << ")"  << "\n\n"; 
    // }
}

float Voice::genValue(){
    float valToReturn = 0; 
    
    for (int i = 0; i < numVoices; i++){
        valToReturn += amp[i] * sinf(2 * M_PI * idx[i]); 
        if ((idx[i] += offset[i]) > 1.f) idx[i] -= 1.f;    
    }

    return valToReturn; 
}

void Voice::deallocate(){
    delete[] offset;    
    delete[] idx; 
    delete[] frequency; 
    delete[] amp; 
}

bool Voice::setFrequency(int _voice, float _frequency){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
        return false; 
    }
    else {
        frequency[_voice] = _frequency; 
        calculateOffset(); 
    }
    return true; 
}

bool Voice::setFrequencyMidi(int _voice, int _note){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
        return false; 
    }
    else {
        frequency[_voice] = midiToFreq(_note); 
        calculateOffset(); 
    }
    return true; 
}

/* ******************************************************************************** */

bool Audio::init(int sampleRate, int framesPerBuffer, Voice voices)
{
	paErr = Pa_OpenDefaultStream(&paStream, 0, 1, paFloat32, sampleRate, 
		framesPerBuffer, callback, &voices); 
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
	Voice* data = (Voice*) userInfo; 
	(void) inputBuffer; 
    
    float bucket = 0; 

	for (int i = 0; i < framesPerBuffer; i++){
		float outVal = data->genValue();
		*out++ = outVal; 
	}

	return 0; 
}