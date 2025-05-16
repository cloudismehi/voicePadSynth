#include <iostream>
#include <math.h> 

#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"


int main(){
	Audio audioInstance; 
	
	Voice voice(sampleRate, bitDepth, 3); 	
	voice.setFrequencyMidi(0, 60); 
	voice.setFrequencyMidi(1, 64); 
	voice.setFrequencyMidi(2, 67); 

	Pa_Initialize(); 
	
	audioInstance.init(sampleRate, framesPerBuffer, voice); 
	audioInstance.startAudio(); 
	Pa_Sleep(1000); 
	audioInstance.deinit(); 

	voice.deallocate(); 

	return 0; 
}

