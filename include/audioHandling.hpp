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
    

    std::function<void()> initSynth; 
    bool setInitSynth = false; 

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
        float* pannings; 
        float* bucket; 
        float* bucket2;
        
        std::unordered_map<std::string, std::vector<float>> inits; 

        int numVoices; 
        int sampleRate = 44100; 
        int bitDepth = 32; 
        bool playMode = false; 
    }; 
    StreamInfo info; 

    int initCheck = 0; //after events and audio instance initialize, this should be zero 

    template<class Obj>
    void addFunction(Obj &obj, float(Obj::*function)()){
        std::function<float()> func = [&obj, function]() -> float{ return (obj.*function)(); };
        audioGenFunctions.push_back(func); 
    }

    template<class Obj>
    void addInitSynth(Obj& obj, void(Obj::*function)()){
        std::function<void()> func = [&obj, function]() { return (obj.*function)(); };
        initSynth = func; 
        setInitSynth = true; 
    }

    Stream(const int _numVoices); 
    ~Stream(); 
    
    void changePanning(float _newVal, int _voice); 
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
        bool isGlobal; 
    }; 
    
    struct Commands{
        std::vector< std::function<void(float _newVal, int _voice)> > function; 
        
        std::vector< float          > newVal, time; 
        std::vector< int            > voice; 
        std::vector< bool           > isFreq; 
        std::vector< bool           > isGlobal; 
        
        std::vector< std::string    > commandNames; 
        std::vector< std::string    > commandDescriptor; 
    }; 
    
    std::vector<PossibleCommand> possibleCommands; 
    std::vector<Commands> events; 
    std::string savedEventsPath = "assets/events/"; 
    
    //name of the file that contains the edit information for the current session, 
    //used when toggling back and forth between edit and play mode.  
    std::string openedStreamName = "openedStream.dat";   
    std::vector<std::string> savedEventFilenames; 


    Event(Stream &_stream); 
    //create new event
    int newEvent(); 
    void listEvents(); 
    void openEvent(int _eventIndex){ openedEvent = _eventIndex; }
    void closeEvent(){ openedEvent = -1; }

    void addToEvent(int _eventIndex, std::string _id, float _newVal, float _time, int _voice);
    void addToEvent(std::string _id, float _newVal, float _time, int _voice); 
    
    void deployEvent(int _eventIndex);
    void deployEvent();

    void deleteEvent(int _eventIndex); 
    void deleteCommandFromEvent(int _eventIndex, int _commandIndex); 
    void clearQueue(); 

    /*
    order: id/cur/new/time/voice
    */
    bool saveEvents(std::string _filename);
    bool loadEvents(std::string _filname); 
    bool getFilenames(); 
    void enterPlayMode(); 
    void enterEditMode(); 

    std::string formatDescriptor(std::string _id, float _newVal, int _voice); 
    
    template<class Obj>
    void addPossibleEvent(Obj& obj, void (Obj::*setter)(float, int), 
            std::string _id, std::string _descriptor, bool _isFreq = false, bool _isGlobal = false){
        
        glossary[_id] = possibleCommands.size(); 

        PossibleCommand command; 
        command.function = [&obj, setter](float _newVal, int _voice){
            (obj.*setter)(_newVal, _voice); 
        };
        command.descriptor = _descriptor; 
        command.isFreq = _isFreq; 
        command.id = _id; 
        command.isGlobal = _isGlobal; 

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