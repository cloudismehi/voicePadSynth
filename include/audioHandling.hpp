#include <iostream>
#include <math.h>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector> 
#include <string> 
#include <utility>

#include "portaudio.h"


class Voice{
    protected: 

    float *frequency, *amp; 
    int sampleRate, bitDepth, maxAmp; 

    public: 

    float midiToFreq(int _note); //midi note to frequency
    int numVoices = 1; 
    
    Voice(int _sampleRate, int _bitDepth, int _numVoices);
    
    virtual float genValue() { return 0; } //generate output, default is no output
    virtual ~Voice() = default; 
    virtual void updateParams() {}
    void deallocate(); //delete all memory allocated during startup 
    void setFrequency(float _frequency, int _voice); //set frequency with freq input
    void setFrequencyMidi(float _note, int _voice); //set frequency with midi note input 
}; 

class SineOscillator : public Voice{
	public:
	float *offset, *incr; 
	
	SineOscillator(int _sampleRate, int _bitDepth, int _numVoices) : Voice(_sampleRate, _bitDepth, _numVoices) {
		init(); 
	}
	void init(); 
	float genValue() override; 
	~SineOscillator() override; 
	void updateParams() override; 
    void freqChange(float _note, int _voice) { 
        setFrequencyMidi(_note, _voice); 
        updateParams(); 
    }
}; 

class Event {
    public: 
    std::unordered_map<std::string, int> glossary; 
    std::vector<std::function<void(float _newVal, int _voice)> > possibleEvents; 
    
    std::vector< std::function<void(float _newVal, int _voice)> > queue; 
    std::vector< std::pair<float, int> > queueData; 


    template<class Obj>
    void addPossibleEvent(Obj& obj, void (Obj::*setter)(float, int), std::string id){
        possibleEvents.push_back([&obj, setter](float _newVal, int _voice){(obj.*setter)(_newVal, _voice); }); 
        glossary[id] = possibleEvents.size() - 1; 
    } 

    void addToQueue(std::string id, float _newVal, int _voice); 
    void triggerEvent(); 
}; 

class Audio{
    public: 
    PaError paErr; 
    PaStream* paStream;
    std::function<float()> audioCallback = []() -> float{ return 0.f; }; 

    bool init(int sampleRate, int framesPerBuffer); //initialize audio device
    bool startAudio(); //start outputting sound
    bool deinit(); //deinitialize audio device

    template<class Obj> 
    void setCallback(Obj& obj, float (Obj::*setter)()){
        audioCallback = [&obj, setter]() -> float{ return (obj.*setter)(); }; 
    }; 

    private: 

    bool checkPaError(PaError err); //check for port audio errors
    static int callback(const void* inputBuffer, void* outputBuffer, 
	    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	    PaStreamCallbackFlags flags, void* userInfo); //port audio callback function
};

