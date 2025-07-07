#pragma once 

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector> 
#include <string> 
#include <utility>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "portaudio.h"
#include "raylib.h"

namespace fs = std::filesystem; 

class Envelope{
  public: 
  float slope, offset = 0; 
  int index = 0, sampleRate = 44100;
  float initValue, endValue; 

  bool envDone = false, set = false; 

  void init(float _initValue, float _endValue, float _time);
  
  void resetEnvelope(){ index = 0; }
  
  float nextValue();
};

class Stream{
    public:
    std::vector< std::function<float()> > audioGenFunctions; 

    struct ModifierFunc{
        std::function< void(float, int) > func; 
        Envelope envelope; 
        float newVal, curVal, changeDur; 
        int voice; 
    }; 
    std::vector<ModifierFunc> modFuncs;
    
    struct StreamInfo{
        float* amps; 
        float totalAmp; 
        int* notes; 
        float* freqs; 
        float* bucket; 
        float* bucket2; 

        int numVoices; 
        int sampleRate = 44100; 
        int bitDepth = 32; 
    }; 
    StreamInfo info; 

    int initCheck = 0; //after events and audio instance initialize, this should be zero 

    template<class Obj>
    void addFunction(Obj &obj, float(Obj::*function)()){
        std::function<float()> func = [&obj, function]() -> float{ return (obj.*function)(); };
        audioGenFunctions.push_back(func); 
    }

    Stream(const int _numVoices); 
    ~Stream(); 
};

class Event {
    public: 
    Stream* stream; 
    
    int openedEvent = -1; 

    //converts ids to an index where data is stored 
    std::unordered_map<std::string, int> glossary; 
    struct PossibleCommand{
        std::function<void(float _newVal, int _voice)> function; 
        std::string id; 
        std::string descriptor; 
        bool isFreq; 
    }; 
    
    struct Commands{
        std::vector< std::function<void(float _newVal, int _voice)> > function; 
        
        std::vector< float          > newVal, time; 
        std::vector< float*         > curVal; 
        std::vector< int            > voice; 
        std::vector< bool           > isFreq; 
        
        std::vector< std::string    > commandNames; 
        std::vector< std::string    > commandDescriptor; 
    }; 
    
    std::vector<PossibleCommand> possibleCommands; 
    std::vector<Commands> events; 
    std::string savedEventsPath = "assets/events/"; 
    std::vector<std::string> savedEventFilenames; 


    Event(Stream &_stream); 
    //create new event
    int newEvent(); 
    void listEvents(); 
    void openEvent(int _eventIndex){ openedEvent = _eventIndex; }
    void closeEvent(){ openedEvent = -1; }

    void addToEvent(int _eventIndex, std::string _id, float &_curVal, float _newVal, float _time, int _voice);
    void addToEvent(std::string _id, float &_curVal, float _newVal, float _time, int _voice); 
    
    void deployEvent(int _eventIndex);
    void deployEvent();

    void deleteEvent(int _eventIndex); 
    void deleteCommandFromEvent(int _eventIndex, int _commandIndex); 

    /*
    order: id/cur/new/time/voice
    */
    bool saveEvents(std::string _filename);
    bool loadEvents(std::string _filname); 
    bool getFilenames(); 

    std::string formatDescriptor(std::string _id, float _newVal, int _voice); 
    

    template<class Obj>
    void addPossibleEvent(Obj& obj, void (Obj::*setter)(float, int), 
            std::string _id, std::string _descriptor, bool _isFreq = false){
        
        glossary[_id] = possibleCommands.size(); 

        PossibleCommand command; 
        command.function = [&obj, setter](float _newVal, int _voice){
            (obj.*setter)(_newVal, _voice); 
        };
        command.descriptor = _descriptor; 
        command.isFreq = _isFreq; 
        command.id = _id; 

        possibleCommands.push_back(command); 
    } 
};

class Audio{
    public: 
    PaError paErr; 
    PaStream* paStream;

    bool init(int sampleRate, int framesPerBuffer, Stream &_stream); //initialize audio device
    bool startAudio(); //start outputting sound
    bool deinit(); //deinitialize audio device

    private: 

    bool checkPaError(PaError err); //check for port audio errors
    static int callback(const void* inputBuffer, void* outputBuffer, 
	    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	    PaStreamCallbackFlags flags, void* userInfo); //port audio callback function
};

//helper functions
float midiToFreq(int _note);
int freqToMidi(int _freq); 
void midiToName(int _note, int& octave, std::string &note); 