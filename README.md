# Voice Pad Synth

This project is a pad synth with a lot of inspiration taken from string orchestras and the technique of orchestration used in How To Dissapear Completely by Radiohead. 

## Basic architechture

the basic idea for this project is to have a set of voices with a certain pitch and envelope (could be a global or individual envelope), and have them be set up independently through a combination of keyboard input and midi input with hopes to grow into a specialized bit of music software. 

### _the event system_
the main idea is to have the note changes happen in "events", where each event is defined by a note change (and its changing envelope), an amplitude envelope, and a intensity control (piano, forte, rest, etc.). these events can then be dispatched in whatever order the user decides towhich will then trigger all the changes queued for that particular event. 

setting each event will be started with a keybind corresponding to that event, followed by another keybind corresponding to editing said event. after thatwe can start picking each voice and giving a new value for pitch, and all the other things listed above (and maybe more!). after editing is done we can commmit to our event and then dispatch it at our own choosing. these events can be kept after dispatch or deleted to open up room for more events. these can be set live or set beforehand and loaded in. 

### _the voices_
we'll set some max number of voices. the sound source for these can be done through synthesis (like a normal wave generator) or through algorithms to produce things like strings, horns, and so on. 

the voices each will have a pitch envelope which will describe how quickly and with what shape the pitch will change, and a normal amplitude envelope to be retriggered at choosing, regular intervals, with the dispatching of new events, or completely deactivated. all voices will be subject to the final amplitdue envelope with which we can create rythmic ideas on our pads (kinda like a string section, less drone-y).  

### _motivation and project mindset_
i want to keep this from being too mouse-based - a big problem with computer music software is its reliance on keeping true to a physical architecture and ending up being too reliant on clicking things. i want to keep these to keystrokes we can develop a muscle memory to and maybe even get used to to the point of engaging creativity and exploration. i want this used in live performances as well as pre-written programs, so i can't make it too reliant on pre-setup and more so make it adaptable and resilient. 

it'll be modifiable and adaptible, but the options to customize it are not going to be accesible in the same page as all of the perfomance elements will be, i want the user to grow into the experience but not be constantly bombarded with choices by the UI. 

### _dependencies_
**raylib** will handle the visuals, user input, and all UI. **PortAudio** will handle the audio output (and further audio settings). **PortMidi** could be used to handle all midi note input (and maybe output in the future). 

### _as of right now_
nothing is coded :p 
