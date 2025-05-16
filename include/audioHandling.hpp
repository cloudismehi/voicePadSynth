#include <iostream>
#include <math.h>
#include <vector>
#include "portaudio.h"

class Voice{
    //sine wave generator
private: 
    
    float *offset, *idx; 
    int sampleRate, bitDepth, maxAmp; 
    
    void calculateOffset(); //calculate offset for sine function
    float midiToFreq(int _note); //midi note to frequency
public:     
    int numVoices = 1; 

    float *frequency, *amp; 

    Voice(int _sampleRate, int _bitDepth, int _numVoices);

    float genValue(); //generate output
    void deallocate(); //delete all memory allocate during startup 
    bool setFrequency(int _voice, float _frequency); //set frequency with freq input
    bool setFrequencyMidi(int _voice, int _note); //set frequency with midi note input 
}; 

class Audio{
    public: 
    PaError paErr; 
    PaStream* paStream;

    bool init(int sampleRate, int framesPerBuffer, Voice voices); //initialize audio device
    bool startAudio(); //start outputting sound
    bool deinit(); //deinitialize audio device

    private: 
    int numberOfActiveVoices; 

    bool checkPaError(PaError err); //check for port audio errors
    static int callback(const void* inputBuffer, void* outputBuffer, 
	    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, 
	    PaStreamCallbackFlags flags, void* userInfo); //port audio callback function
}; 