#include <iostream>
#include <memory>
#include <math.h> 
#include <raylib.h>
#include <portaudio.h> 
#include <chrono>

#include "audioHandling.hpp"
#include "soundSources.hpp"
#include "projectSettings.hpp"
#include "UIhandling.hpp"

int numVoices = 4; 

int main(){
	
	Pa_Initialize(); 	
	Stream audioStream(numVoices); 

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
	sineOsc.setFreqMidi(71.f, 3); 
	sineOsc.totalAmp = 0.5; 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "sineOsc_freq"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "sineOsc_freqMidi");
	events.addPossibleEvent(sineOsc, &SineOscillator::setAmp, "sineOsc_amp"); 
	
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}

	//raylib init
	InitWindow(screenWidth, screenHeight, "this is a life"); 
	Screen screen(audioStream, audioInstance, events, numVoices); 
	
	while (!WindowShouldClose()){
		BeginDrawing(); 
	
		screen.update(); 

		EndDrawing(); 
	}

	audioInstance.deinit();
	CloseWindow(); 

	return 0; 
}
