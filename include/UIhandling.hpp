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
    const int *numVoices; 
    
    public: 

    Screen(Stream &_stream, Audio &_audioInstance, Event &_events, const int &_numVoices); 
    void update(); 

    void setDim(int _width, int _height) { width = _width; height = _height; }
    void loadFonts();
    void assignVoiceColors();  
    
    
    struct Colors{
        Color dark = (Color){27, 32, 33, 255};          //eerie black
        Color bright = (Color){229, 235, 234, 255};     //anti-flash white
        Color midTone = (Color){157, 128, 143, 255};    //mountbatten pink
        Color midToneDark = (Color){115, 89, 102, 255};      //eggplant
        Color accent = (Color){74, 124, 89, 255};       //viridian (green looking one)
        
        Color voiceColorOptions[4] = {
            (Color){248, 243, 43, 255}, 
            (Color){162, 37, 34, 255}, 
            (Color){41, 51, 92, 255}, 
            (Color){255, 166, 43, 255}, 
        }; 

        std::vector<Color> voiceColors; 
    }; 
    Colors color; 

    struct TextSettings{
        int titleFontSize = 20; 
        int titleSpacing = 3;  
        
        int bodyFontSize = 15; 
        int smallBodyFontSize = 10; 
        int bodySpacing = 2; 

        Font bodyFont; 
        Font thickFont; 
    }; 
    TextSettings text; 

    struct PianoSettings{
        int keyWidth = 25; 
        int whiteKeyHeight = 40;
        int blackKeyHeight = 20; 
        int keyOffset = 2; 

        int off = keyWidth + keyOffset; 
        int qKeyWidth = keyWidth / 4;
        int hKeyWidth = keyWidth / 2; 
        int hWHeight = whiteKeyHeight / 2; 

        int whiteFontSize = 20; 
        int blackFontSize = 15; 

    }; 
    PianoSettings piano; 

    private:
    
    void printMouseCoord(); 
    void drawPianoRoll(int x, int y); 
    void drawVoiceInfo(int x, int y); 
}; 