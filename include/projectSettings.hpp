#ifndef SETTINGS
#define SETTINGS

#include <raylib.h>
#define MAX_SAMPLES_PER_UPDATE	4096

int sampleRate = 44100; 
int bitDepth = 32;

int framesPerBuffer = 64; 

//raylib settings
int screenWidth = 1000; 
int screenHeight = 1000; 
float backgroundColor[] = {0.f, 30.f, 0.f, 1.f}; 

#endif