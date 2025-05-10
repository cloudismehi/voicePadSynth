#include <iostream>
#include <raylib.h>

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
