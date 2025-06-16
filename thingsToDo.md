# Things to do

**The whole purpose of this file is to list all the things that need implementing**

### design the ui / program loop and interactability 
the current project program loop happens over commands in the terminal. i want to prioritize having a functional piece of software over designing the UI, so i may do the program loop with ncurses or something to keep everything as simple as possible and so that i can get cracking with the things that i find more important to the program flow at this stage. this is not set in stone. 

### events system
the basics of the events system is basically written and implemented, i need to determine a way to keep multiple events from targeting the same voice at the same time - as of right now the behavior is undetermined when it comes to this. 


### the voices
i want to find a way to get multiple voices to react to the same functions - like have the voices linked in some way so that the user can layer sounds. the only way i can think to do this is to have the same syntax functions for all the voice functions. 
i need more voice engines but that's last on the priority list. 

### the envelopes
the only envelope determined now is linear, this shouldn't be the only option so i must code another one. i need to implement volume envelopes - not just for all voices but for each individual voice. 