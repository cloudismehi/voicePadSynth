#include "audioHandling.hpp"

void Envelope::init(float _initValue, float _endValue, float _time){
    offset = _initValue;
    slope = (_endValue - _initValue) / (_time * sampleRate); 

    initValue = _initValue; 
    endValue = _endValue; 
    set = true; 
}

float Envelope::nextValue(){
    //linear envelope
    float out = (float)(slope * index) + (float)offset; 

    // check is envelope is done 
    if ((slope < 0) && (out < endValue)) envDone = true; 
    if ((slope > 0) && (out > endValue)) envDone = true; 

    return out; 
}

/* ******************************************************************************** */
Event::Event(Stream &_stream){
    stream = &_stream; 
    (*stream).initCheck += 1; // just set for checks - might delete later 
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

void Event::addToEvent(int _eventIndex, std::string _id, float _curVal, float _newVal, float _time, int _voice){
    events[_eventIndex].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[_eventIndex].queueData.push_back(std::make_tuple(_newVal, _voice, _time));
    events[_eventIndex].commandNames.push_back(_id); 
    events[_eventIndex].curVal.push_back(_curVal); 
}

void Event::addToEvent(std::string _id, float _curVal, float _newVal, float _time, int _voice){
    if (openedEvent == -1) {
        std::cout << "no event opened\n"; 
        return; 
    }
    events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
    events[openedEvent].queueData.push_back(std::make_tuple(_newVal, _voice, _time));
    events[openedEvent].commandNames.push_back(_id); 
    events[openedEvent].curVal.push_back(_curVal); 
}

void Event::deployEvent(int _eventIndex){
    int commandNumber = events[_eventIndex].queue.size(); 
    for (int i = 0; i < commandNumber; i++){

        Stream::ModifierFunc func; 
        func.func = events[_eventIndex].queue.back(); 
        func.newVal = std::get<0>(events[_eventIndex].queueData.back()); 
        func.voice = std::get<1>(events[_eventIndex].queueData.back()); 
        func.changeDur = std::get<2>(events[_eventIndex].queueData.back()); 
        func.curVal = events[_eventIndex].curVal.back(); 
        
        (*stream).modFuncs.push_back(func); 
        

        events[_eventIndex].queue.pop_back(); 
        events[_eventIndex].queueData.pop_back(); 
        events[_eventIndex].commandNames.pop_back();
        events[_eventIndex].curVal.pop_back(); 
    }
    events.erase(events.begin() + _eventIndex); 
}

void Event::deployEvent(){
    int commandNumber = events.front().queue.size(); 
    for (int i = 0; i < commandNumber; i++){
        Stream::ModifierFunc func; 
        func.func = events.front().queue.front(); 
        func.newVal = std::get<0>(events.front().queueData.front()); 
        func.voice = std::get<1>(events.front().queueData.front()); 
        func.changeDur = std::get<2>(events.front().queueData.front()); 
        func.curVal = events.front().curVal.front(); 
        
        (*stream).modFuncs.push_back(func); 

        events.front().curVal.erase(events.front().curVal.begin()); 
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

    auto modifierFunc = audioStream->modFuncs; 

    for (int i = 0; i < modifierFunc.size(); ++i){
        if (modifierFunc[i].changeDur == 0){
            modifierFunc[i].func(modifierFunc[i].newVal, modifierFunc[i].voice); 
            (*audioStream).modFuncs.erase((*audioStream).modFuncs.begin() + i); 
            modifierFunc = audioStream->modFuncs; 
        } 
        
    }
    
	for (int i = 0; i < framesPerBuffer; i++){
        float outVal = 0; 
        
        for (int p = 0; p < modifierFunc.size(); ++p){
            if (modifierFunc[p].changeDur != 0){
                if (!modifierFunc[p].envelope.set){
                    (*audioStream).modFuncs[p].envelope.init(modifierFunc[p].curVal, 
                        modifierFunc[p].newVal, modifierFunc[p].changeDur); 
                    modifierFunc[p] = audioStream->modFuncs[p]; 
                }
                
                modifierFunc[p].func(modifierFunc[p].envelope.nextValue(), modifierFunc[p].voice);
                (*audioStream).modFuncs[p].envelope.index++; 
                if (modifierFunc[p].envelope.envDone){
                    // std::cout << "envelope done!\n"; 
                    modifierFunc[p].func(modifierFunc[p].newVal, modifierFunc[p].voice); 
                    (*audioStream).modFuncs.erase((*audioStream).modFuncs.begin() + p);
                    modifierFunc = audioStream->modFuncs; 
                }
            }
        }

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