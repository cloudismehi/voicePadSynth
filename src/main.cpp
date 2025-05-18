#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

extern AudioData audioData; 

class SineOscillator : public Voice{
	void calculateIncr(); 
	public:
	float *offset, *incr; 
	
	SineOscillator(int _sampleRate, int _bitDepth, int _numVoices) : Voice(_sampleRate, _bitDepth, _numVoices) {
		init(); 
	}
	void init(); 
	float genValue() const override; 
	~SineOscillator() override; 
	void updateParams() override; 
}; 

int main(){
	PaError paErr; 
    PaStream* paStream;

	Audio audioInstance; 
	SineOscillator voice(sampleRate, bitDepth, 3); 
	
	audioData.oscillator = std::make_unique<SineOscillator>(sampleRate, bitDepth, 3); 

	audioData.oscillator->setFrequencyMidi(0, 60); 
	audioData.oscillator->setFrequencyMidi(1, 64); 
	audioData.oscillator->setFrequencyMidi(2, 67); 
	audioData.oscillator->updateParams(); 

	Pa_Initialize(); 	
	audioInstance.init(sampleRate, framesPerBuffer); 
	audioInstance.startAudio(); 
	
	Pa_Sleep(1000); 

	audioInstance.deinit(); 

	voice.deallocate(); 

	return 0; 
}

void SineOscillator::init(){
	offset = new float[numVoices]; 
	incr = new float[numVoices]; 
}

void SineOscillator::updateParams(){
	for (int i = 0; i < numVoices; i++){
		incr[i] = frequency[i]/sampleRate; 
	}
}

float SineOscillator::genValue() const{
	float out = 0.f; 
	for (int i = 0; i < numVoices; i++){
		out += sinf(2 * M_PI * offset[i]); 
		if ((offset[i] += incr[i]) >= 1.f) offset[i] = 0.f; 
	}
	return out; 
}

SineOscillator::~SineOscillator(){
	delete[] offset; 
	delete[] incr; 
}

