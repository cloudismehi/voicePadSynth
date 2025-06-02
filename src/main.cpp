#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 
#include <chrono>

#include "audioHandling.hpp"
#include "soundSources.hpp"
#include "projectSettings.hpp"

int numVoices = 3; 

int main(){
	
	Pa_Initialize(); 	
	Stream audioStream; 

	Audio audioInstance;
	if (!audioInstance.init(sampleRate, framesPerBuffer, audioStream)){
		std::cout << "error initializing audio instance\n"; 
	} 
	
	Event events(audioStream); 
	
	SineOscillator sineOsc(sampleRate, bitDepth, numVoices); 
	
	if (audioStream.initCheck != 2){
		std::cout << "error! audio stream init check failed\n";
		return 1; 
	}
	
	sineOsc.setFreqMidi(60.f, 0); 
	sineOsc.setFreqMidi(64.f, 1); 
	sineOsc.setFreqMidi(67.f, 2); 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "freq"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "freqMidi"); 
	
	events.openEvent(events.newEvent()); 

	events.addToEvent("freq", midiToFreq(60.f), midiToFreq(62.f), 0, 0); 
	events.addToEvent("freq", midiToFreq(64.f), midiToFreq(65.f), 0, 1); 
	events.addToEvent("freq", midiToFreq(67.f), midiToFreq(69.f), 0, 2); 

	events.closeEvent(); 
	
	events.openEvent(events.newEvent()); 

	events.addToEvent("freq", midiToFreq(62.f), midiToFreq(64.f), 0.5, 0); 
	events.addToEvent("freq", midiToFreq(65.f), midiToFreq(67.f), 0.5, 1); 
	events.addToEvent("freq", midiToFreq(69.f), midiToFreq(71.f), 0.5, 2); 

	events.closeEvent(); 
	
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}
	
	auto startTime = std::chrono::high_resolution_clock::now(); 
	Pa_Sleep(1000); 
	events.deployEvent(); 
	Pa_Sleep(2000); 
	events.deployEvent(); 
	Pa_Sleep(2000); 
	auto endTime = std::chrono::high_resolution_clock::now(); 

	audioInstance.deinit(); 

	auto dur = duration_cast<std::chrono::milliseconds>(endTime - startTime); 
	std::cout << "program finished in  " << dur.count()/1000.f << "ms\n"; 

	return 0; 
}
