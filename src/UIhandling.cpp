#include "UIhandling.hpp"

Screen::Screen(Stream &_stream, Audio &_audioInstance, Event &_events, int &_numVoices){
    stream = &_stream; 
    audioInstance = &_audioInstance; 
    events = &_events;
    numVoices = &_numVoices; 
    
    loadFonts(); 
}   

void Screen::update(){
    ClearBackground(color.background);    
    
    eventsList(10, 10); 
}

void Screen::loadFonts(){
    fonts.mainFont = LoadFont("assets/fonts/TT Interphases Pro Mono Trial Bold.ttf"); 
    fonts.bodyFont = LoadFont("assets/fonts/TT Interphases Pro Mono Trial Regular.ttf"); 
}

void Screen::eventsList(float _x, float _y){
    int fontSize; 
    Vector2 box; 
    int maxSize = 0; 
    std::string bucketText; 
    
    //box for the background
    for (auto event : events->glossary){
        //calculate the max size of event titles
        if (event.first.size() > maxSize) {
            for (int i = maxSize; i < event.first.size(); i++){
                bucketText.append("g"); 
            }
            maxSize = event.first.size(); 
        }
    }
    box = MeasureTextEx(fonts.bodyFont, bucketText.c_str(), 20, 0); 
    DrawRectangleRounded((Rectangle){_x - 2.5f, _y, box.x + 10.f, box.y + 5.f + (events->glossary.size() * 20)}, 0.30, 4, color.shortcutBox); 

    //box for the title
    box = MeasureTextEx(fonts.mainFont, "list of events", 20, 0); 
    DrawRectangleRounded((Rectangle){_x - 2.5f, _y, box.x + 8.f, box.y}, 0.60, 4, color.accentBox); 
    
    //title 
    DrawTextEx(fonts.mainFont, "list of events", (Vector2){_x, _y}, 20, 0, color.accentText); 

    //events
    int _index = 0; 
    for (auto event : events->glossary){
        DrawTextEx(fonts.bodyFont, event.first.c_str(), (Vector2){_x, _y + 20 + (20 * _index++)}, 20, 0, color.idleText); 
    }
}

void Screen::voiceInfo(float _x, float _y){
    int fontSize; 
    Vector2 box; 
    int maxSize = 0; 
    std::string bucketText; 
    
    //box for the background
    for (auto event : events->glossary){
        //calculate the max size of event titles
        if (event.first.size() > maxSize) {
            for (int i = maxSize; i < event.first.size(); i++){
                bucketText.append("g"); 
            }
            maxSize = event.first.size(); 
        }
    }
    box = MeasureTextEx(fonts.bodyFont, bucketText.c_str(), 20, 0); 
    DrawRectangleRounded((Rectangle){_x - 2.5f, _y, box.x + 10.f, box.y + 5.f + (events->glossary.size() * 20)}, 0.30, 4, color.shortcutBox); 

    //box for the title
    box = MeasureTextEx(fonts.mainFont, "list of events", 20, 0); 
    DrawRectangleRounded((Rectangle){_x - 2.5f, _y, box.x + 8.f, box.y}, 0.60, 4, color.accentBox); 
    
    //title 
    DrawTextEx(fonts.mainFont, "list of events", (Vector2){_x, _y}, 20, 0, color.accentText); 

    //events
    int _index = 0; 
    for (auto event : events->glossary){
        DrawTextEx(fonts.bodyFont, event.first.c_str(), (Vector2){_x, _y + 20 + (20 * _index++)}, 20, 0, color.idleText); 
    }
}

void Screen::printMouseCoord(){
    DrawText(TextFormat("%d, %d", GetMouseX(), GetMouseY()), 900, 600, 20, BLACK); 
}