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

Stream::Stream(const int _numVoices){
    info.numVoices = _numVoices; 
    info.amps = new float[_numVoices]; 
    info.notes = new int[_numVoices]; 
    info.freqs = new float[_numVoices]; 
    info.bucket = new float[_numVoices]; 
    info.bucket2 = new float[_numVoices]; 

    for (int i = 0; i < _numVoices; i++){
        info.amps[i] = 1.f; 
    }
}  

Stream::~Stream(){
    delete[] info.amps; 
    delete[] info.notes; 
    delete[] info.freqs; 
    delete[] info.bucket; 
    delete[] info.bucket2; 
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
          for (int i = 0; i < event.function.size(); i++){
            std::cout << " " << event.commandDescriptor[i] << '\n'; 
          }
          eventIndex++;
        }
    std::cout << '\n';
}

void Event::addToEvent(int _eventIndex, std::string _id, float &_curVal, 
    float _newVal, float _time, int _voice)
{
    auto *command = &possibleCommands[glossary.at(_id)]; 
    
    events[_eventIndex].function.push_back((*command).function); 
    events[_eventIndex].newVal.push_back(_newVal); 
    events[_eventIndex].time.push_back(_time); 
    events[_eventIndex].voice.push_back(_voice); 
    events[_eventIndex].commandNames.push_back(_id); 
    
    std::string descriptorFormated = formatDescriptor(_id, _newVal, _voice); 

    events[_eventIndex].commandDescriptor.push_back(descriptorFormated); 
    events[_eventIndex].isFreq.push_back((*command).isFreq); 
    events[_eventIndex].isGlobal.push_back((*command).isGlobal); 
    events[_eventIndex].curVal.push_back(&_curVal); 
}

void Event::addToEvent(std::string _id, float &_curVal, float _newVal, float _time, int _voice){
    if (openedEvent == -1) {
        std::cout << "no event opened\n"; 
        return; 
    }
    
    addToEvent(openedEvent, _id, _curVal, _newVal, _time, _voice); 
}

void Event::deployEvent(int _eventIndex){
    int commandNumber = events[_eventIndex].function.size(); 
    for (int i = 0; i < commandNumber; i++){

        Stream::ModifierFunc func; 
        func.func = events[_eventIndex].function.back(); 
        func.newVal = events[_eventIndex].newVal.back(); 
        func.voice = events[_eventIndex].voice.back(); 
        func.changeDur = events[_eventIndex].time.back(); 
        func.curVal = *(events[_eventIndex].curVal.back()); 
        
        (*stream).modFuncs.push_back(func); 
        
        events[_eventIndex].function.pop_back(); 
        events[_eventIndex].newVal.pop_back(); 
        events[_eventIndex].voice.pop_back(); 
        events[_eventIndex].time.pop_back(); 
        events[_eventIndex].commandNames.pop_back();
        events[_eventIndex].curVal.pop_back(); 
        events[_eventIndex].isGlobal.pop_back(); 
        events[_eventIndex].isFreq.pop_back(); 
    }
    events.erase(events.begin() + _eventIndex); 
}

void Event::deployEvent(){
    int commandNumber = events.front().function.size(); 
    for (int i = 0; i < commandNumber; i++){
        Stream::ModifierFunc func; 
        func.func = events.front().function.front(); 
        func.newVal = events.front().newVal.front(); 
        func.voice = events.front().voice.front(); 
        func.changeDur = events.front().time.front(); 
        func.curVal = (*events.front().curVal.front()); 
        std::cout << (*events.front().curVal.front()) << '\n';
        
        (*stream).modFuncs.push_back(func); 

        events.front().curVal.erase(events.front().curVal.begin()); 
        events.front().function.erase(events.front().function.begin()); 
        events.front().newVal.erase(events.front().newVal.begin()); 
        events.front().voice.erase(events.front().voice.begin()); 
        events.front().time.erase(events.front().time.begin()); 
        events.front().commandNames.erase(events.front().commandNames.begin()); 
        events.front().isGlobal.erase(events.front().isGlobal.begin()); 
        events.front().isFreq.erase(events.front().isFreq.begin()); 
    }
    events.erase(events.begin()); 
}

void Event::deleteEvent(int _eventIndex){
    events.erase(events.begin() + _eventIndex); 
}

void Event::deleteCommandFromEvent(int _eventIndex, int _commandIndex){
    if (events[_eventIndex].function.size() == 1) events.erase(events.begin() + _eventIndex); 
    else {
        events[_eventIndex].function.erase(events[_eventIndex].function.begin() + _commandIndex); 
        events[_eventIndex].newVal.erase(events[_eventIndex].newVal.begin() + _commandIndex); 
        events[_eventIndex].voice.erase(events[_eventIndex].voice.begin() + _commandIndex); 
        events[_eventIndex].time.erase(events[_eventIndex].time.begin() + _commandIndex); 
        events[_eventIndex].commandNames.erase(events[_eventIndex].commandNames.begin() + _commandIndex); 
        events[_eventIndex].commandDescriptor.erase(events[_eventIndex].commandDescriptor.begin() + _commandIndex); 
        events[_eventIndex].isFreq.erase(events[_eventIndex].isFreq.begin() + _commandIndex); 
        events[_eventIndex].curVal.erase(events[_eventIndex].curVal.begin() + _commandIndex); 
        events[_eventIndex].isGlobal.erase(events[_eventIndex].isGlobal.begin() + _commandIndex); 
    }
}

bool Event::saveEvents(std::string _filename){
    std::fstream file; 
    file.open(savedEventsPath + _filename, std::ios::out); 
    if (!file) {
        std::cout << "error making file\n"; 
        return false; 
    }

    for (int i = 0; i < events.size(); i++){
        file << "new\n"; 
        for (int q = 0; q < events[i].function.size(); q++){
            file << events[i].commandNames[q] << '/' << events[i].curVal[q] << '/';
            file << events[i].newVal[q] << '/' << events[i].time[q] << '/' << events[i].voice[q] << '\n'; 
        }
    }

    file.close(); 
    return true; 
}

bool Event::loadEvents(std::string _filename){
    std::fstream file; 
    file.open(savedEventsPath + _filename); 
    if (!file){
        std::cout << "error opening file\n"; 
        return false; 
    }

    std::string inputString; 
    int _voice; 
    float _curVal, _newVal, _time; 
    std::string _id; 
    while(std::getline(file, inputString)){
        int numElementsSet = 0; 
        if (inputString == "new"){
            openEvent(newEvent()); 
        } else {
            std::stringstream stream(inputString); 
            std::string element; 
            int _index = 0; 
            while (getline(stream, element, '/')){
                switch(_index){
                    case 0: 
                        //id
                        _id = element; 
                        numElementsSet++;
                        break; 
                    case 1: 
                        //current value
                        _curVal = std::stof(element); 
                        numElementsSet++;
                        break; 
                    case 2: 
                        //new value
                        _newVal = std::stof(element); 
                        numElementsSet++;
                        break; 
                    case 3: 
                        //time
                        _time = std::stof(element); 
                        numElementsSet++;
                        break; 
                    case 4: 
                        //voice
                        _voice = std::stoi(element); 
                        numElementsSet++;
                        break; 
                }
                _index++; 
            }
        }
        if (numElementsSet == 5) { addToEvent(_id, _curVal, _newVal, _time, _voice); } 
    }
    closeEvent(); 
    file.close(); 
    return true; 
}

bool Event::getFilenames(){
    savedEventFilenames.clear(); 
    try{
        for (const auto& entry : fs::directory_iterator(savedEventsPath)){
            std::string name = entry.path().filename().string(); 
            if (fs::is_regular_file(entry.status())){
                if (name.substr(name.length() - 4, 4) == ".dat"){
                    savedEventFilenames.push_back(name); 
                }
            }
        }
    } catch (const fs::filesystem_error &e){
        std::cout << "error reading files: " << e.what() << std::endl; 
        return false; 
    }
    for (auto name : savedEventFilenames){
        std::cout << name << '\n'; 
    }
    return true; 
}

std::string Event::formatDescriptor(std::string _id, float _newVal, int _voice){
    std::string name = ""; 
    
    auto* command = &possibleCommands[glossary.at(_id)]; 

    if ((*command).isFreq){
        int _oct = 0; 
        std::string _name = ""; 
        midiToName(freqToMidi(_newVal), _oct, _name); 
        
        name += TextFormat("%s to %s%d on voice %d", (*command).descriptor.c_str(), 
            _name.c_str(), _oct, _voice);    
    } else if (_voice == -1){
        name += TextFormat("global %s to %0.2f", (*command).descriptor.c_str(), _newVal); 
    } else {
        name += TextFormat("%s to %0.2f on voice %d", (*command).descriptor.c_str(), 
            _newVal, _voice);
    }

    return name; 
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

    //trigger instant events in the queue 
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
                //set envelope for new events
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
		*out++ = outVal * 0.2; 
	}

	return 0; 
}

/* ******************************************************************************** */

float midiToFreq(int _note){
	return 440.f * exp((log(2) * (_note - 69))/12); 
}

int freqToMidi(int _freq){
    return static_cast<int>(std::round(69 + 12 * std::log2(_freq / 440.0)));
}

void midiToName(int _note, int& octave, std::string &note){
    octave = ((_note / 12) - 2) + 1; 
    int remaind = _note % 12; 

    switch (remaind){
        case 0: 
            note = "C"; 
            break; 
        case 1: 
            note = "C#"; 
            break; 
        case 2: 
            note = "D"; 
            break; 
        case 3: 
            note = "D#"; 
            break; 
        case 4: 
            note = "E"; 
            break; 
        case 5: 
            note = "F"; 
            break; 
        case 6: 
            note = "F#"; 
            break; 
        case 7: 
            note = "G"; 
            break; 
        case 8: 
            note = "G#"; 
            break; 
        case 9: 
            note = "A"; 
            break; 
        case 10: 
            note = "A#"; 
            break; 
        case 11: 
            note = "B"; 
            break; 
    }
}