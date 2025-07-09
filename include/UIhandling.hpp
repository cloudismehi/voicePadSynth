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

    int frameCount = 0; 
    
    public: 

    Screen(Stream &_stream, Audio &_audioInstance, Event &_events, int &_numVoices); 
    void update(); 

    void setDim(int _width, int _height) { width = _width; height = _height; }
    void loadFonts();
    void assignVoiceColors();  
    
    struct Colors{
        Color dark = (Color){27, 32, 33, 255};              //eerie black
        Color bright = (Color){229, 235, 234, 255};         //anti-flash white
        Color midTone = (Color){157, 128, 143, 255};        //mountbatten pink
        Color midToneDark = (Color){115, 89, 102, 255};     //eggplant
        Color accent = (Color){74, 124, 89, 255};           //viridian (green looking one)
        Color grayOutColor = (Color){18, 22, 25, 200};      //night
        Color red = (Color){184, 0, 52, 255};               //red (ncs)

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

        int note = -1; 
        int octave = 4; 

        void resetPiano(){
            note = -1; 
            octave = 4;  
        }
    }; 
    PianoSettings piano; 

    struct MenuInfo{
        int mainScreenSelection = 0; 
        int deleteMenuSelection = 0; 
        int deleteCommandSelection = 0; 
        int loadFileSelection = 0; 
        int changeScreenSelection = 0; 

        bool mainMenu = true; 
        bool deleteMenu = false; 
        bool deleteCommandMenu = false; 
        bool exitOutOfDeleteMenu = false; 
        bool saveEvent = false; 
        bool loadFile = false; 
        bool changeScreen = false; 
        bool globalChange = false; 
    }; 
    MenuInfo menuInfo; 

    struct Change{
        int voice = 0; 
        float newAmp = -1; 
        float time = 0; 
        float incr = 0.1; 
        float timeIncr = 0.1; 
        float largeTimeIncr = 1; 

        std::string filename = ""; 
        int maxFileTitleLength = 20; 

        void resetChange(){
            voice = 0; 
            newAmp = -1; 
            time = 0; 
            incr = 0.1; 
            timeIncr = 0.1; 
        }
    }; 
    Change change; 
    
    private:
    
    void printMouseCoord(); 
    void grayScreen(); 
    void emptyOutQueue(); 
    
    //main menu elements 
    void drawMainWindow(); 
    void pollMainMenu(); 
    void pollChange(); 
    void drawPianoRoll(int x, int y); 
    bool pollPianoRoll(); 
    void drawVoiceInfo(int x, int y); 
    void drawEventInfo(int x, int y);
    
    //delete menu optionts
    void drawDeleteMenu(int x, int y); 
    void pollDeleteMenu(); 
    
    //save file
    void drawSaveFile(int x, int y); 
    void pollSaveFile(); 

    //load file
    void drawLoadFile(int x, int y); 
    void pollLoadFile(); 

    //change screen
    void drawChangeScreen(int x, int y); 
    void pollChangeScreen(); 
    
    //global change screen
    void drawGlobalChangeScreen(int x, int y); 
    void pollGlobalChangeScreen(); 
}; 