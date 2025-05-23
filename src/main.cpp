#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

int numVoices = 3; 


int main(){
	Audio audioInstance; 
	Event events; 
	SineOscillator sineOsc(sampleRate, bitDepth, 3); 

	sineOsc.setFrequencyMidi(0, 60); 
	sineOsc.setFrequencyMidi(1, 64); 
	sineOsc.setFrequencyMidi(2, 67); 
	sineOsc.updateParams(); 

	// audioInstance.setCallback(sineOsc, &SineOscillator::genValue);
	
	//program loop 
	Pa_Initialize(); 	

	audioInstance.init(sampleRate, framesPerBuffer); 
	audioInstance.startAudio(); 
	
	Pa_Sleep(1000); 
	Pa_Sleep(1000); 

	audioInstance.deinit(); 

	return 0; 
}
