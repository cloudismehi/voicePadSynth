portaudio_inc = -I/Users/fernandosanchez/development/third-libs/audio/portaudio/include
portaudio_lib = -L/Users/fernandosanchez/development/third-libs/audio/portaudio/build -lportaudio
portaudio_dep = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices

default: objectReference

eventClass: eventClass.cpp
	@g++ -std=c++20 eventClass.cpp ${portaudio_inc} ${portaudio_dep} ${portaudio_lib} -o eventClass

voiceClass: voiceClass.cpp
	@g++ -std=c++20 voiceClass.cpp -o voiceClass

envelope: envelope.cpp
	@g++ -std=c++20 envelope.cpp ${portaudio_inc} ${portaudio_dep} ${portaudio_lib} -o envelope

objectReference: objectReference.cpp
	@g++ -std=c++20 objectReference.cpp -o objectReference
