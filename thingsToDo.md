# Things to do

## UI

- (!) **need to add UI adaptability to different voices**
- support for more events that aren't frequency or amplitude - possibly through a sub-menu or something like that

## events system

- determine a way to keep multiple events from targeting the same voice at the same time - as of right now the behavior is undetermined when it comes to this.

## the voices

- i want to find a way to get multiple voices to react to the same functions - like have the voices linked in some way so that the user can layer sounds. the only way i can think to do this is to have the same syntax functions for all the voice functions.
- i need more voice engines but that's last on the priority list.

## the envelopes

- the only envelope determined now is linear, this shouldn't be the only option so i must code another one. i need to implement volume envelopes

## implement more synth things

- more oscillator types, filters, lfos, so on and so forth

## global tempo and clock

- i want to implement a global click that we could sync our event triggering with.

## panning

- voices should be able to be panned around to fully embrace the orchestra feeling.

## refactor

## functionality

**what goes where?**

- **play mode** will have controls for triggering next events, adding new commands and events, and erasing commands or events
- **edit mode** will have controls for adding new commands and events, erasing commands and events, loading and saving commands and events, and previewing current events
  - **command preview** anything that is being set currently will be changed - everything else will be previewed with default values
