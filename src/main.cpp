#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "projectSettings.hpp"

int numVoices = 3; 

//TODO: the events class is simplistic
/*
as of right now it only takes functions with one argument for new value and that return void. 
i need to overload this function for situations where i am affecting one voice in particular as opposed to 
all voices at once - keep both for both situations. the void return i won't concern myself with as far as 
queue goes because why would i
*/
//TODO: review Voice / SineOcillator architecture to make sure it works with current project layout.

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
