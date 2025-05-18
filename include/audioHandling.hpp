#include <iostream>
#include <math.h>
#include <memory>
#include "portaudio.h"


class Voice{
    protected: 

    float *frequency, *amp; 
    int sampleRate, bitDepth, maxAmp; 

    public: 

    float midiToFreq(int _note); //midi note to frequency
    int numVoices = 1; 
    
    Voice(int _sampleRate, int _bitDepth, int _numVoices);
    
    virtual float genValue() const = 0; //generate output, default is no output
    virtual ~Voice() = default; 
    virtual void updateParams() {}
    void deallocate(); //delete all memory allocated during startup 
    bool setFrequency(int _voice, float _frequency); //set frequency with freq input
    bool setFrequencyMidi(int _voice, int _note); //set frequency with midi note input 
}; 

struct AudioData {
    std::unique_ptr<Voice> oscillator; 
}; 

class Audio{
    public: 
    PaError paErr; 
    PaStream* paStream;

    bool init(int sampleRate, int framesPerBuffer); //initialize audio device
    bool startAudio(); //start outputting sound
    bool deinit(); //deinitialize audio device

    private: 

    bool checkPaError(PaError err); //check for port audio errors
    static int callback(const void* inputBuffer, void* outputBuffer, 
	    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	    PaStreamCallbackFlags flags, void* userInfo); //port audio callback function
};