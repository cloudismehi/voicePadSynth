#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

int numVoices = 3; 
extern AudioData audioData; 

int main(){
	PaError paErr; 
    PaStream* paStream;

	Audio audioInstance; 
	useDefaultOsc(sampleRate, bitDepth, numVoices); 
	SineOscillator voice(sampleRate, bitDepth, numVoices); 

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
