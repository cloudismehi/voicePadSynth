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
        Color background = (Color){64, 76, 79, 255};        //outer space
        
        Color accentText = (Color){27, 32, 33, 255};        //eerie black
        Color idleText = RAYWHITE;         
        Color shortcutText; 
        
        Color accentBox = (Color) {111, 195, 147, 255};     //mint
        Color shortcutBox = (Color){140, 207, 169, 255};    //celadon
    }; 
    Colors color; 

    struct FontStruct{
        Font mainFont; 
        Font bodyFont; 
    }; 
    FontStruct fonts; 

    private:
    
    void eventsList(float _x, float _y); 
    void voiceInfo(float _x, float _y); 
    
    void printMouseCoord(); 
}; 