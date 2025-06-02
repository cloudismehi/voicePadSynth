#include <iostream>
#include <math.h>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector> 
#include <string> 
#include <utility>

#include "portaudio.h"


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


    int initCheck = 0; //after events and audio instance initialize, this should be zero 

    template<class Obj>
    void addFunction(Obj &obj, float(Obj::*function)()){
        std::function<float()> func = [&obj, function]() -> float{ return (obj.*function)(); };
        audioGenFunctions.push_back(func); 
    }

};

class Event {
    public: 
    Stream* stream; 
    int sampleRate = 44100; 
    std::unordered_map<std::string, int> glossary; 
    std::vector<std::function<void(float _newVal, int _voice)> > possibleEvents; 
    
    struct Commands{
        std::vector< std::function<void(float _newVal, int _voice)> > queue; 
        std::vector< std::tuple<float, int, float> > queueData; // newVal, voice, time
        std::vector< std::string > commandNames; 
        std::vector< float > curVal; 
    }; 

    std::vector<Commands> events; 
    int openedEvent = -1; 

    Event(Stream &_stream); 
    int newEvent(); 
    void listEvents(); 
    void openEvent(int _eventIndex){ openedEvent = _eventIndex; }
    void closeEvent(){ openedEvent = -1; }

    void addToEvent(int _eventIndex, std::string _id, float _curVal, float _newVal, float _time, int _voice);
    void addToEvent(std::string _id, float _curVal, float _newVal, float _time, int _voice); 
    
    void deployEvent(int _eventIndex);
    void deployEvent();

    template<class Obj>
    void addPossibleEvent(Obj& obj, void (Obj::*setter)(float, int), std::string id){
        possibleEvents.push_back([&obj, setter](float _newVal, int _voice){(obj.*setter)(_newVal, _voice); }); 
        glossary[id] = possibleEvents.size() - 1; 
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