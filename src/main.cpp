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

const int numVoices = 4; 

int main(){
	Pa_Initialize(); 	
	Stream audioStream(numVoices); 

	Audio audioInstance;
	if (!audioInstance.init(sampleRate, framesPerBuffer, audioStream)){
		std::cout << "error initializing audio instance\n"; 
	} 
	
	Event events(audioStream); 
	SineOscillator sineOsc(audioStream); 

	Screen screen(audioStream, audioInstance, events, numVoices); 

	if (audioStream.initCheck != 2){
		std::cout << "error! audio stream init check failed\n";
		return 1; 
	}
	 
	audioStream.info.notes[0] = 60;  
	audioStream.info.notes[1] = 64;  
	audioStream.info.notes[2] = 67; 
	audioStream.info.notes[3] = 71; 

	audioStream.info.totalAmp = 0.5; 

	sineOsc.initSynth(); 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "sineOsc_freq"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "sineOsc_freqMidi");
	events.addPossibleEvent(sineOsc, &SineOscillator::setAmp, "sineOsc_amp"); 
	
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(60), midiToFreq(66), 1, 0); 
	events.addToEvent("sineOsc_freq", midiToFreq(64), midiToFreq(62), 1, 1); 

	InitWindow(screenWidth, screenHeight, "tito"); 
	SetTargetFPS(30); 

	while (!WindowShouldClose()){
		BeginDrawing(); 

		screen.update(); 
		if (IsKeyPressed(KEY_A)){
			events.deployEvent(); 
		}
	
		EndDrawing(); 
	}

	audioInstance.deinit();
	
	CloseWindow(); 

	return 0; 
}
