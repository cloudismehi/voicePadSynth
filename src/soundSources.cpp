#include "soundSources.hpp"

SineOscillator::SineOscillator(Stream &_stream){
    stream = &_stream; 

    offset = new float[(*stream).info.numVoices]; 
	incr = new float[(*stream).info.numVoices]; 

    sampleRate = (*stream).info.sampleRate; 
    bitDepth = (*stream).info.bitDepth; 
    numVoices = (*stream).info.numVoices; 

    frequency = new float[numVoices]; 
    amp = new float[numVoices]; 

    for (int i = 0; i < numVoices; i++){
        frequency[i] = 200.f; 
        amp[i] = 1.f; 
        offset[i] = 0.f; 
    }

    //max amp 
    if (bitDepth == 16){
        maxAmp = pow(2, bitDepth - 1) - 1; 
    } else if (bitDepth == 32){
        maxAmp = 1.f; 
    }
}

void SineOscillator::initSynth(){
    for (int i = 0; i < numVoices; i++){
        setFreq(midiToFreq((*stream).info.notes[i]), i); 
        setAmp((*stream).info.amps[i], i); 
        (*stream).info.freqs[i] = midiToFreq((*stream).info.notes[i]); 
    }
    setAmp((*stream).info.totalAmp, -1); 
    init = true; 
}

void SineOscillator::updateOffsets(){
	for (int i = 0; i < numVoices; i++){
		incr[i] = frequency[i]/sampleRate; 
	}
}

float SineOscillator::genValue(){
    float out = 0.f; 
    if (init){
        for (int i = 0; i < numVoices; i++){
            out += amp[i] * sinf(2 * M_PI * offset[i]); 
            if ((offset[i] += incr[i]) >= 1.f) offset[i] = 0.f; 
            (*stream).info.bucket[i] = offset[i]; 
            (*stream).info.bucket2[i] = incr[i]; 
        }
    }
    return (totalAmp * out); 
}

void SineOscillator::setFreq(float _freq, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = _freq; 
        (*stream).info.notes[_voice] = freqToMidi(_freq); 
        (*stream).info.freqs[_voice] = _freq; 
    }
    updateOffsets(); 
}

void SineOscillator::setFreqMidi(float _note, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = midiToFreq(_note); 
        (*stream).info.notes[_voice] = _note; 
        (*stream).info.freqs[_voice] = midiToFreq(_note); 
    }
    updateOffsets();   
}

void SineOscillator::setAmp(float _amp, int _voice){
    if (_voice == -1) {
        totalAmp = _amp; 
        (*stream).info.totalAmp = _amp; 
    }
    else {
        if (_voice >= numVoices){
            std::cout << "voice number " << _voice << " out of range\n"; 
        } else {
            amp[_voice] = _amp; 
            (*stream).info.amps[_voice] = _amp; 
        }
    }
}


SineOscillator::~SineOscillator(){
	delete[] offset; 
	delete[] incr; 
    delete[] frequency; 
    delete[] amp; 
}

float fold(float _input, float _foldAmt){
    float output; 

    output = _input * _foldAmt; 

    bool goodToGo = false; 
    while (!goodToGo){
        if (output > 1){ output = 2 - output; } 
        else if (output < -1){ output = -2 - output; } 
        else { goodToGo = true; }
    }
    return output; 
}