#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 

#include "audioHandling.hpp"
#include "soundSources.hpp"
#include "projectSettings.hpp"

int numVoices = 2; 

int main(){
	Pa_Initialize(); 	
	Stream audioStream; 

	Audio audioInstance;
	if (!audioInstance.init(sampleRate, framesPerBuffer, audioStream)){
		std::cout << "error initializing audio instance\n"; 
	} 
	
	Event events(audioStream); 
	
	SineOscillator sineOsc(sampleRate, bitDepth, numVoices); 
	SineFold foldOsc(sampleRate, bitDepth, numVoices); 
	
	if (audioStream.initCheck != 2){
		std::cout << "error! audio stream init check failed\n";
		return 1; 
	}
	
	sineOsc.setFreqMidi(60.f, 0); 
	sineOsc.setFreqMidi(64.f, 1); 
	
	foldOsc.setFreqMidi(67.f, 0); 
	foldOsc.setFreqMidi(71.f, 1); 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "freq"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "freqMidi"); 
	
	events.openEvent(events.newEvent()); 
	events.addToEvent("freqMidi", 65.f, 1); 
	events.addToEvent("freqMidi", 62.f, 0); 
	events.closeEvent(); 
	
	audioStream.addFunction(foldOsc, &SineFold::genValue); 
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}
	
	Pa_Sleep(1000); 
	events.deployEvent(); 
	Pa_Sleep(1000); 
	

	audioInstance.deinit(); 

	return 0; 
}
