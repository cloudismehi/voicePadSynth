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
	screen.setDim(screenWidth, screenHeight); 

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
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "sineOsc_freq", "freq change", true); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setAmp, "sineOsc_amp", "amp change"); 

	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	events.openEvent(events.newEvent()); 
	events.addToEvent("sineOsc_freq", audioStream.info.freqs[0], midiToFreq(62), 1, 0); 
	events.closeEvent(); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}

	InitWindow(screenWidth, screenHeight, "tito"); 
	screen.loadFonts(); 
	SetTargetFPS(30); 

	// system("clear"); 
	
	while (!WindowShouldClose()){
		BeginDrawing(); 

		screen.update(); //also polls events
				
		EndDrawing(); 
	}

	audioInstance.deinit();
	
	CloseWindow(); 

	std::cout << "program ended!\n"; 

	return 0; 
}
