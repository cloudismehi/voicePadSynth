raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
portaudio_dep = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices

voicePadSynth: UIhandling.o audioHandling.o soundSources.o voicePadSynth.o 
	@g++ -std=c++20  ${raylib_dep} -lraylib ${portaudio_dep} -lportaudio src/obj/audioHandling.o src/obj/voicePadSynth.o src/obj/soundSources.o src/obj/UIHandling.o -o voicePadSynth
	@mv voicePadSynth execs
	@echo "built project!"
	
voicePadSynth.o: src/main.cpp
	@g++ -std=c++20 -I ./include src/main.cpp -c -o voicePadSynth.o
	@mv voicePadSynth.o src/obj

audioHandling.o: src/audioHandling.cpp
	@g++ -std=c++20 src/audioHandling.cpp -I ./include -c -o audioHandling.o 
	@mv audioHandling.o src/obj

soundSources.o: src/soundSources.cpp
	@g++ -std=c++20 src/soundSources.cpp -I ./include -c -o soundSources.o
	@mv soundSources.o src/obj

UIhandling.o: src/UIhandling.cpp
	@g++ -std=c++20 src/UIhandling.cpp -I ./include -c -o UIHandling.o 
	@mv UIhandling.o src/obj