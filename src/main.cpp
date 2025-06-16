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
int notes[4]; 
float amps[4] = {1.f, 1.f, 1.f, 1.f}; 
float totalAmp = 1.f; 

int main(){
	std::cout << "hello! welcome to the string synth! enjoy\n"; 

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
	notes[0] = 60; 
	sineOsc.setFreqMidi(64.f, 1); 
	notes[1] = 64; 
	sineOsc.setFreqMidi(67.f, 2); 
	notes[2] = 67; 
	sineOsc.setFreqMidi(71.f, 3); 
	notes[3] = 67; 

	sineOsc.totalAmp = 0.5; 
	totalAmp = 0.5; 
	
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreq, "sineOsc_freq"); 
	events.addPossibleEvent(sineOsc, &SineOscillator::setFreqMidi, "sineOsc_freqMidi");
	events.addPossibleEvent(sineOsc, &SineOscillator::setAmp, "sineOsc_amp"); 
	
	audioStream.addFunction(sineOsc, &SineOscillator::genValue); 

	if(!audioInstance.startAudio()){
		std::cout << "error with starting audio!\n"; 
	}
	
	system("clear"); 

	std::cout << "everything is set up for you, you can start inputing commands, for help type 'help'\n"; 
	bool run = true; 
	while (run){
		std::string input; 
		std::getline(std::cin, input); 

		//quit out of program
		if (input == "quit")
			run = false; 
		if (input == "new"){
			events.openEvent(events.newEvent());
			system("clear"); 
			std::cout << "opened new event!\n"; 
		}
		if (input == "deploy"){
			system("clear"); 
			std::cout << "deployed event\n"; 
			events.deployEvent(); 
		}
		if (input == "list"){
			events.listEvents(); 
		}
		if (input == "frequency"){
			system("clear"); 
			if (events.openedEvent == -1){
				std::cout << "you need to open an event before adding commands\n"; 

			} else {
				int voice, newNote, time; 
				
				std::cout << "   frequency change! on what voice? "; 
				std::getline(std::cin, input); 
				voice = std::stoi(input); 
	
				std::cout << "   what new note? (the current note is " << notes[voice] << ") "; 
				std::getline(std::cin, input); 
				newNote = std::stoi(input); 

				std::cout << "   over how much time? "; 
				std::getline(std::cin, input); 
				time = std::stoi(input); 

				events.addToEvent("sineOsc_freq", midiToFreq(notes[voice]), midiToFreq(newNote), time, voice); 

				std::cout << "added a frequency change to the opened event\n"; 
			}
			
		}
		if (input == "amp"){
			std::cout << "amp\n"; 
		}
		if (input == "help"){
			std::cout << "help\n"; 
		}

	}

	audioInstance.deinit();
	

	return 0; 
}
