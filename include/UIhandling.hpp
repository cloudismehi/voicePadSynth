#include <iostream>
#include <string> 
#include <vector>
#include <raylib.h>

#include "audioHandling.hpp"

class Screen{
    int width, height; 
    Stream *stream; 
    Audio *audioInstance; 
    Event *events; 
    int *numVoices; 

    public: 

    Screen(Stream &_stream, Audio &_audioInstance, Event &_events, int &_numVoices); 
    void update(); 

    void setDim(int _width, int _height) { width = _width; height = _height; }
    void loadFonts(); 
    
    
    struct Colors{
        Color dark = (Color){27, 32, 33, 255};          //eerie black
        Color bright = (Color){229, 235, 234, 255};     //anti-flash white
        Color midTone = (Color){157, 128, 143, 255};    //rose quartz (more like gray); 
        Color accent = (Color){74, 124, 89, 255};       //viridian (green looking one)
    }; 
    Colors color; 

    struct FontStruct{
        Font mainFont; 
        Font bodyFont; 
    }; 
    FontStruct fonts; 

    private:
    
    void printMouseCoord(); 
    void drawPianoRoll(int x, int y); 
}; 