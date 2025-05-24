#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

int numVoices = 3; 

//TODO: review Voice / SineOcillator architecture to make sure it works with current project layout.
//TODO: make it so that multiple function calls can happen per event. 

int main(){
	Audio audioInstance; 
	Event events; 
	SineOscillator sineOsc(sampleRate, bitDepth, 3); 

	sineOsc.setFrequencyMidi(60.f, 0); 
	sineOsc.setFrequencyMidi(64.f, 1); 
	sineOsc.setFrequencyMidi(67.f, 2); 
	sineOsc.updateParams(); 

	events.addPossibleEvent(sineOsc, &SineOscillator::freqChange, "freq"); 
	
	events.addToQueue("freq", 55.f, 0); 
	events.addToQueue("freq", 59.f, 1); 
	events.addToQueue("freq", 62.f, 2); 

	events.addToQueue("freq", 63.f, 2); 

	events.addToQueue("freq", 55.f, 0); 
	events.addToQueue("freq", 60.f, 1); 
	events.addToQueue("freq", 64.f, 2); 

	events.addToQueue("freq", 57.f, 0); 
	events.addToQueue("freq", 61.f, 1); 

	events.addToQueue("freq", 59.f, 0); 
	events.addToQueue("freq", 62.f, 1); 

	events.addToQueue("freq", 63.f, 1); 
	
	audioInstance.setCallback(sineOsc, &SineOscillator::genValue);
	
	//program loop 
	Pa_Initialize(); 	

	audioInstance.init(sampleRate, framesPerBuffer); 
	audioInstance.startAudio(); 
	
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	Pa_Sleep(1000); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	events.triggerEvent(); 
	Pa_Sleep(3000); 

	audioInstance.deinit(); 

	return 0; 
}
