raylib_inc = -I/Users/fernandosanchez/development/third-libs/raylib/src
raylib_lib = -L/Users/fernandosanchez/development/third-libs/raylib/src -l raylib
raylib_dep = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

voicePadSynth: voicePadSynth.o
	@g++ -std=c++20  ${raylib_dep} ${raylib_lib} src/obj/voicePadSynth.o -o voicePadSynth
	@mv voicePadSynth execs
	@echo "built project!"
	
voicePadSynth.o: src/main.cpp
	@g++ -std=c++20 ${raylib_inc} src/main.cpp -c -o voicePadSynth.o
	@mv voicePadSynth.o src/obj
