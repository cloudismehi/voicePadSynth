#pragma once

#include <iostream> 
#include <cmath>

#include "audioHandling.hpp"

class SineOscillator{
	public:

	float *offset, *incr, *frequency, *amp; 
    int sampleRate, bitDepth, maxAmp, numVoices; 
	float totalAmp = 1.f; 
    bool init = false; 
    Stream* stream; 

    SineOscillator(Stream &_stream); 
    void initSynth(); 

	float genValue(); 
	void updateOffsets(); 

    void setFreq(float _freq, int _voice); 
    void setFreqMidi(float _note, int _voice);
    
    /*
    if voice is set to -1, it modifies the overall volume of all voices 
    (independent of individual) amplitudes
    */
    void setAmp(float _amp, int _voice); 

	~SineOscillator(); 

    private: 
};


float fold(float _input, float _foldAmt); 