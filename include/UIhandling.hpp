#include <iostream>
#include <string> 
#include <raylib.h>

#include "audioHandling.hpp"

class Screen{
    int width, height; 
    Stream *stream; 
    Audio *audioInstance; 
    Event *events; 

    public: 

    Screen(Stream &_stream, Audio &_audioInstance, Event &_events); 
    void update(); 

    void setDim(int _width, int _height) { width = _width; height = _height; }
    
    
    struct Colors{
        Color background = (Color){56, 134, 89, 255};       //sea green
        
        Color accentsText = (Color){27, 32, 33, 255};       //eerie black
        Color idleText = (Color){64, 76, 79, 255};          //outer space
        Color shortcutText; 
        
        Color accentBox = (Color) {111, 195, 147, 255};     //mint
        Color shortcutBox = (Color){140, 207, 169, 255};    //celadon
    }; 
    Colors color; 
}; 