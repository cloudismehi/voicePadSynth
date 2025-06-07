#include "UIhandling.hpp"

Screen::Screen(Stream &_stream, Audio &_audioInstance, Event &_events){
    stream = &_stream; 
    audioInstance = &_audioInstance; 
    events = &_events; 
}   

void Screen::update(){
    ClearBackground(color.background);    

    int w = GetMouseX() - 20; 
    int h = GetMouseY() - 25; 

    // DrawText(std::to_string(w).c_str(), 30, 800, 20, BLACK); 
    // DrawText(std::to_string(h).c_str(), 30, 900, 20, BLACK); 

    DrawRectangle(20, 25, 260, 40, color.accentBox); 
    DrawRectangle(37, 58, 187, 65, color.shortcutBox); 

    DrawText("possible events", 30, 30, 30, color.accentsText); 

    int _index = 0; 
    for (auto possible : events->glossary){
        std::string possibleEvents = possible.first; 
        DrawText(possibleEvents.c_str(), 40, 60 + (20 * (_index++)), 20, color.idleText); 
    }
    
}
