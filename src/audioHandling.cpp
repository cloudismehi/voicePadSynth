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
int Event::newEvent(){
    events.resize(events.size() + 1); 
    return events.size() - 1; 
}

void Event::listEvents(){
    int eventIndex = 0;
        for (auto event : events){
          std::cout << "event " << eventIndex << ":\n";
          for (int i = 0; i < event.queue.size(); i++){
            std::cout << " " << event.commandNames[i] << " to " << event.queueData[i].first << " on voice " << event.queueData[i].second << '\n';
          }
          eventIndex++;
        }
    std::cout << '\n';
}

void Event::addToEvent(int _eventIndex, std::string _id, float _newVal, int _voice){
    events[_eventIndex].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[_eventIndex].queueData.push_back(std::make_pair(_newVal, _voice));
    events[_eventIndex].commandNames.push_back(_id); 
}

void Event::addToEvent(std::string _id, float _newVal, int _voice){
    if (openedEvent == -1) {
        std::cout << "no event opened\n"; 
        return; 
    }
    events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[openedEvent].queueData.push_back(std::make_pair(_newVal, _voice));
    events[openedEvent].commandNames.push_back(_id); 
}

void Event::deployEvent(int _eventIndex){
    int commandNumber = events[_eventIndex].queue.size(); 
    for (int i = 0; i < commandNumber; i++){
        events[_eventIndex].queue.back()(events[_eventIndex].queueData.back().first, events[_eventIndex].queueData.back().second);
        events[_eventIndex].queue.pop_back(); 
        events[_eventIndex].queueData.pop_back(); 
        events[_eventIndex].commandNames.pop_back();
    }
    events.erase(events.begin() + _eventIndex); 
}

void Event::deployEvent(){
    int commandNumber = events.front().queue.size(); 
    for (int i = 0; i < commandNumber; i++){
        events.front().queue.back()(events.front().queueData.front().first, events.front().queueData.front().second); 
        events.front().queue.erase(events.front().queue.begin()); 
        events.front().queueData.erase(events.front().queueData.begin()); 
        events.front().commandNames.erase(events.front().commandNames.begin()); 
    }
    events.erase(events.begin()); 
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
