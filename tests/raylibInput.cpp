#include <string>
#include <iostream> 
#include <algorithm>
#include <cctype>

#include "raylib.h"

#define MAX_INPUT_CHARS         70

int main(){
    std::string words; 
    int frameCount = 0; 

    InitWindow(1000, 500, "bear country"); 
    Font font = LoadFont("alpha_beta.png"); 

    SetTargetFPS(30); 

    Color bckg = RAYWHITE; 
    while (!WindowShouldClose()){
        if ((++frameCount) > 30) frameCount = 0; 
        
        BeginDrawing(); 
        ClearBackground(bckg); 
        DrawText("this is a life", 100, 100, 20, GRAY); 
        
        int key = GetKeyPressed(); 
        while (key > 0){
            if ((key >= 39) and (key <= 96)){
                if (words.size() < MAX_INPUT_CHARS){
                    words.push_back(std::tolower((char)key)); 
                }
            }
            else if (key == KEY_SPACE) {
                words.push_back(' ');
            }
            else if (key == KEY_BACKSPACE) {
                words.pop_back();
            }
            else if (key == KEY_ENTER) std::cout << words << '\n'; 
            key =  GetKeyPressed(); 
        }
        
        DrawTextEx(font, words.c_str(), {100.f, 200.f}, 20, 2, BLACK); 
        Color cursorColor = (frameCount < 15) ? BLACK : RAYWHITE; 
        DrawTextEx(font, "_", {100.f + MeasureTextEx(font, words.c_str(), 20, 2).x, 200.f}, 20, 2, cursorColor); 
        
        EndDrawing(); 
    }
    CloseWindow(); 

    return 0; 
}