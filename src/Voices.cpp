#include "Voices.hpp"

void Voice::calculateIncr(){
    incr = frequency/sampleRate; 
}

Voice::Voice(int _sampleRate, int _bitDepth, float _initFreq){
    sampleRate = _sampleRate; 
    bitDepth = _bitDepth; 
    frequency = _initFreq; 
    if (bitDepth == 16){
        maxAmp = pow(2, bitDepth - 1) - 1; 
    } else if (bitDepth == 32){
        maxAmp = 1.f; 
    }

    calculateIncr(); 
}

float Voice::genValue(){
    float valToReturn = maxAmp * amp * sinf(2 * M_PI * idx); 
    if ((idx += incr) > 1.f) idx -= 1.f; 
    return valToReturn; 
}