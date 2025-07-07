# Things to do

### UI
(!) need to add UI adaptability to different voices

### events system
- the basics of the events system is basically written and implemented, i need to determine a way to keep multiple events from targeting the same voice at the same time - as of right now the behavior is undetermined when it comes to this. 
- simplify note changes - it takes too many clicks no one could ever use this live. 

### the voices
- i want to find a way to get multiple voices to react to the same functions - like have the voices linked in some way so that the user can layer sounds. the only way i can think to do this is to have the same syntax functions for all the voice functions. 
- i need more voice engines but that's last on the priority list. 

### the envelopes
- the only envelope determined now is linear, this shouldn't be the only option so i must code another one. i need to implement volume envelopes 

### implement more synth things 
- (!) more oscillator types, filters, lfos, so on and so forth

### live mode? 
- (!) a simplified version of the ui where one could move the voices with a lot more ease but less control

### global tempo and clock
- i want to implement a global click that we could sync our event triggering with. 

### troubleshooting
- saving events doesn't work for either saving the first event or saving manually from main (kinda solved?)

## refactor
- the whole UI menu system and stuff may or may not be too complicated for its own sake. i feel like it can be rewritten as each menu function calling its polling function in some internal while loop that returns whenever the option is selected - may reduce redundancies and maybe make the code more flexible. will take a bit to implement though. 

## functionality
- i want to make a "play mode" in the synth, in this mode it runs through the events and deletes them as they get triggered - the switch can happen automatically or manually - after the "playback" is done, we can exit play mode and see all the events as we had them - this will be the main method for modifying and creating events, the edit mode. play mode will also allow you to add and modify events on the fly but will probably not be the main way to do so. 