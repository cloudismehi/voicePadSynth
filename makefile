raylib_inc = -I/Users/fernandosanchez/development/third-libs/raylib/src
raylib_lib = -L/Users/fernandosanchez/development/third-libs/raylib/src -l raylib
raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

portaudio_inc = -I/Users/fernandosanchez/development/third-libs/audio/portaudio/include
portaudio_lib = -L/Users/fernandosanchez/development/third-libs/audio/portaudio/build -lportaudio
portaudio_dep = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices

voicePadSynth: audioHandling.o soundSources.o voicePadSynth.o
	@g++ -std=c++20  ${raylib_dep} ${raylib_lib} ${portaudio_dep} ${portaudio_lib} src/obj/audioHandling.o src/obj/voicePadSynth.o src/obj/soundSources.o -o voicePadSynth
	@mv voicePadSynth execs
	@echo "built project!"
	
voicePadSynth.o: src/main.cpp
	@g++ -std=c++20 ${raylib_inc} ${portaudio_inc} -I ./include src/main.cpp -c -o voicePadSynth.o
	@mv voicePadSynth.o src/obj

audioHandling.o: src/audioHandling.cpp
	@g++ -std=c++20 src/audioHandling.cpp ${portaudio_inc} -I ./include -c -o audioHandling.o 
	@mv audioHandling.o src/obj

soundSources.o: src/soundSources.cpp
	@g++ -std=c++20 src/soundSources.cpp -I ./include -c -o soundSources.o
	@mv soundSources.o src/obj