#include "UIhandling.hpp"

Screen::Screen(Stream &_stream, Audio &_audioInstance, Event &_events, int &_numVoices){
    stream = &_stream; 
    audioInstance = &_audioInstance; 
    events = &_events;
    numVoices = &_numVoices; 
    
    loadFonts(); 
}   

void Screen::update(){
    ClearBackground(color.midTone);    
    
    drawPianoRoll(100, 100); 

}

void Screen::loadFonts(){
    fonts.bodyFont = LoadFont("assets/fonts/VCR_OSD_MONO_1.001.ttf"); 
}

void Screen::printMouseCoord(){
    DrawText(TextFormat("%d, %d", GetMouseX(), GetMouseY()), 900, 600, 20, BLACK); 
}

void Screen::drawPianoRoll(int x, int y){
    int keyWidth = 15; 
    int whiteKeyHeight = 30;
    int blackKeyHeight = 10; 
    int keyOffset = 2; 

    for (int i = 0; i < 14; i++){
        DrawRectangle(x + (i * (keyWidth + keyOffset)), y, keyWidth, whiteKeyHeight, color.bright); 
    }
    for (int i = 0; i < 14; i++){
        if ((i == 2) or (i == 6) or (i == 9) or (i == 13)); 
        else 
            DrawRectangle(x + (keyWidth/2) + (i * (keyWidth + keyOffset)), 100, keyWidth, blackKeyHeight, color.dark); 
    }
}