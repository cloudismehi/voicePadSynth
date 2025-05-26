#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

int numVoices = 2; 

//TODO: review Voice / SineOcillator architecture to make sure it works with current project layout.

int main(){
	Audio audioInstance; 
	Event events; 
	SineOscillator sineOsc(sampleRate, bitDepth, numVoices); 

	sineOsc.setFrequencyMidi(60.f, 0); 
	sineOsc.setFrequencyMidi(64.f, 1); 
	sineOsc.updateParams(); 

	events.addPossibleEvent(sineOsc, &SineOscillator::freqChange, "freq"); 
	events.newEvent(); 
	events.openEvent(0); 
	events.addToEvent("freq", 62.f, 1); 
	events.addToEvent("freq", 59.f, 0); 
	events.closeEvent(); 
	

	audioInstance.setCallback(sineOsc, &SineOscillator::genValue);
	
	//program loop 
	Pa_Initialize(); 	

	audioInstance.init(sampleRate, framesPerBuffer); 
	audioInstance.startAudio(); 
	
	Pa_Sleep(1000); 
	events.deployEvent(); 
	Pa_Sleep(1000); 
	

	audioInstance.deinit(); 

	return 0; 
}
