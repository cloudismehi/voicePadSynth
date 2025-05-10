#include <iostream>

#include <raylib.h>
#include <portaudio.h> 
#include "Voices.hpp"

int main(){
	InitWindow(1000, 1000, "this is a life"); 
	SetTargetFPS(30); 

	while(!WindowShouldClose()){
		BeginDrawing(); 
		ClearBackground(WHITE); 
		EndDrawing(); 
	}

	return 0; 
}
