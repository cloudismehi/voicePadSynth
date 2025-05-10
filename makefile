raylib_inc = -I/Users/fernandosanchez/development/third-libs/raylib/src
raylib_lib = -L/Users/fernandosanchez/development/third-libs/raylib/src -l raylib
raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

portaudio_inc = -I/Users/fernandosanchez/development/third-libs/audio/portaudio/include
portaudio_lib = -L/Users/fernandosanchez/development/third-libs/audio/portaudio/build -l portaudio

voicePadSynth: Voices.o voicePadSynth.o
	@g++ -std=c++20  ${raylib_dep} ${raylib_lib} ${portaudio_lib} src/obj/Voices.o src/obj/voicePadSynth.o -o voicePadSynth
	@mv voicePadSynth execs
	@echo "built project!"
	
voicePadSynth.o: src/main.cpp
	@g++ -std=c++20 ${raylib_inc} ${portaudio_inc} -I ./include src/main.cpp -c -o voicePadSynth.o
	@mv voicePadSynth.o src/obj

Voices.o: src/Voices.cpp
	@g++ -std=c++20 src/Voices.cpp -I ./include -c -o Voices.o 
	@mv Voices.o src/obj