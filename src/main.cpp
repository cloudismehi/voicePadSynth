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

	audioStream.info.totalAmp = 1.f; 

	sineOsc.initSynth(); 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "sineOsc_freq", "freq change"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "sineOsc_freqMidi", "freq change, midi input");
	events.addPossibleEvent(sineOsc, &SineOscillator::setAmp, "sineOsc_amp", "amp change"); 
	
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(60), midiToFreq(66), 1, 0); 
	events.addToEvent("sineOsc_freq", midiToFreq(64), midiToFreq(62), 1, 1); 
	events.addToEvent("sineOsc_amp", 1.f, 0.5f, 1, 0); 
	events.closeEvent(); 

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(66), midiToFreq(60), 1, 0); 
	events.closeEvent(); 

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(60), midiToFreq(62), 1, 0); 
	events.addToEvent("sineOsc_freq", midiToFreq(64), midiToFreq(65), 1, 1); 
	events.addToEvent("sineOsc_freq", midiToFreq(67), midiToFreq(69), 1, 2); 
	events.closeEvent(); 

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(71), midiToFreq(72), 1, 3); 
	events.closeEvent(); 

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(72), midiToFreq(71), 1, 3); 
	events.addToEvent("sineOsc_freq", midiToFreq(69), midiToFreq(67), 1, 2); 
	events.closeEvent(); 

	events.openEvent(events.newEvent());
	events.addToEvent("sineOsc_freq", midiToFreq(65), midiToFreq(64), 1, 1); 
	events.addToEvent("sineOsc_freq", midiToFreq(62), midiToFreq(60), 1, 0); 
	events.addToEvent("sineOsc_amp", 0.f, 1.f, 1, -1); 
	events.closeEvent(); 

	InitWindow(screenWidth, screenHeight, "tito"); 
	SetTargetFPS(30); 

	system("clear"); 
	events.listEvents(); 

	while (!WindowShouldClose()){
		BeginDrawing(); 

		screen.update(); 
		if (IsKeyPressed(KEY_M)){
			if (events.events.size() > 0)
				events.deployEvent(); 
			else printf("no events\n"); 
		}	
		
		EndDrawing(); 
	}

	audioInstance.deinit();
	
	CloseWindow(); 

	return 0; 
}
