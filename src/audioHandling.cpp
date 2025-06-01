#include "audioHandling.hpp"

void Envelope::init(float _initValue, float _endValue, float _time){
    offset = _initValue;
    slope = (_endValue - _initValue) / (_time); 
    
    initValue = _initValue; 
    endValue = _endValue; 
    set = true; 
}

float Envelope::nextValue(int _index){
    float out = (float)(slope * _index) + (float)offset; 
    _index++; 

    if ((slope < 0) && (out < endValue)) envDone = true; 
    if ((slope > 0) && (out > endValue)) envDone = true; 

    return out; 
}

/* ******************************************************************************** */
Event::Event(Stream &_stream){
    stream = &_stream; 
    (*stream).initCheck += 1; 
}

int Event::newEvent(){
    events.resize(events.size() + 1); 
    return events.size() - 1; 
}

void Event::listEvents(){
    int eventIndex = 0;
        for (auto event : events){
          std::cout << "event " << eventIndex << ":\n";
          for (int i = 0; i < event.queue.size(); i++){
            std::cout << " " << event.commandNames[i] << " to " << std::get<0>(event.queueData[i]) << " on voice " << std::get<1>(event.queueData[i]) << '\n';
          }
          eventIndex++;
        }
    std::cout << '\n';
}

void Event::addToEvent(int _eventIndex, std::string _id, float _newVal, int _voice){
    events[_eventIndex].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[_eventIndex].queueData.push_back(std::make_tuple(_newVal, _voice, 0));
    events[_eventIndex].commandNames.push_back(_id); 
}

void Event::addToEvent(std::string _id, float _newVal, int _voice){
    if (openedEvent == -1) {
        std::cout << "no event opened\n"; 
        return; 
    }
    events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[openedEvent].queueData.push_back(std::make_tuple(_newVal, _voice, 0));
    events[openedEvent].commandNames.push_back(_id); 
}

void Event::deployEvent(int _eventIndex){
    int commandNumber = events[_eventIndex].queue.size(); 
    for (int i = 0; i < commandNumber; i++){
        (*stream).modifierFunctions.push_back(events[_eventIndex].queue.back()); 
        (*stream).modifierFunctionsValues.push_back(events[_eventIndex].queueData.back()); 

        events[_eventIndex].queue.pop_back(); 
        events[_eventIndex].queueData.pop_back(); 
        events[_eventIndex].commandNames.pop_back();
    }
    events.erase(events.begin() + _eventIndex); 
}

void Event::deployEvent(){
    int commandNumber = events.front().queue.size(); 
    for (int i = 0; i < commandNumber; i++){
        (*stream).modifierFunctions.push_back(events.front().queue.front()); 
        (*stream).modifierFunctionsValues.push_back(events.front().queueData.front()); 
        (*stream).modFunctionNum++; 

        events.front().queue.erase(events.front().queue.begin()); 
        events.front().queueData.erase(events.front().queueData.begin()); 
        events.front().commandNames.erase(events.front().commandNames.begin()); 
    }
    events.erase(events.begin()); 
}


/* ******************************************************************************** */

bool Audio::init(int sampleRate, int framesPerBuffer, Stream &_stream)
{
    Stream* stream = &_stream; 
	paErr = Pa_OpenDefaultStream(&paStream, 0, 1, paFloat32, sampleRate, 
		framesPerBuffer, callback, &_stream); 
	if (checkPaError(paErr)) return false; 
    
    (*stream).initCheck++;

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
		printf("caught pa error: %s\n", Pa_GetErrorText(err));
		Pa_Terminate();
		return true;
	}
	return false; 
}

int Audio::callback(const void* inputBuffer, void* outputBuffer, 
	unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	PaStreamCallbackFlags flags, void* userInfo)
{
	(void) inputBuffer; 
    float *out = (float*) outputBuffer; 
    Stream* audioStream = (Stream*) userInfo; 

    auto modFuncs = audioStream->modifierFunctions; 
    auto modFuncVals = audioStream->modifierFunctionsValues; 
    (*audioStream).modifierFunctions.clear(); 
    (*audioStream).modifierFunctionsValues.clear(); 
    (*audioStream).modFunctionNum = 0; 

    for (int i = 0 ; i < modFuncs.size(); ++i){
        if (std::get<2>(modFuncVals[i]) == 0){
            modFuncs[i](std::get<0>(modFuncVals[i]), std::get<1>(modFuncVals[i])); 
        } else {
            /* handle changes over time */
        }
    }
    
	for (int i = 0; i < framesPerBuffer; i++){
        float outVal = 0; 
    
        for (int audioGenIndex = 0; audioGenIndex < audioStream->audioGenFunctions.size(); audioGenIndex++){
            outVal += (*audioStream).audioGenFunctions[audioGenIndex](); 
        }
		*out++ = outVal; 
	}

	return 0; 
}

/* ******************************************************************************** */

float midiToFreq(int _note){
	return 440.f * exp((log(2) * (_note - 69))/12); 
}