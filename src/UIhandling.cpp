#include "UIhandling.hpp"

Screen::Screen(Stream &_stream, Audio &_audioInstance, Event &_events, const int &_numVoices){
    stream = &_stream; 
    audioInstance = &_audioInstance; 
    events = &_events;
    numVoices = &_numVoices; 
    
    loadFonts(); 
    assignVoiceColors(); 
}   

void Screen::update(){
    ClearBackground(color.dark);    
    
    //keep track of frames
    if (++frameCount > 30) frameCount = 0; 

    pollEvents();     
    //piano mappings
    drawPianoRoll(10, 10); 
    //voice info
    drawVoiceInfo(10, 90); 
    //events info
    drawEventInfo(330, 10); 
    
    
    if (grayOutMainScreen){
        DrawRectangle(0, 0, width, height, color.grayOutColor); 
    }

    if (menuInfo.deleteMenu){
        drawDeleteMenu((width / 2) - 250, (height / 2) - 100); 
        
    }
}

void Screen::loadFonts(){
    text.bodyFont = LoadFont("assets/fonts/upheavtt.ttf"); 
    text.thickFont = LoadFont("assets/fonts/upheavtt.ttf"); 
}

void Screen::assignVoiceColors(){
    
    int index = 0; 
    for(int i = 0 ; i < (*stream).info.numVoices; i++){
        color.voiceColors.push_back(color.voiceColorOptions[index]); 
        if (++index >= 4) index = 0; 
    }
}

void Screen::pollEvents(){
    //event menu input
    if (menuInfo.eventMenu){
        if (IsKeyPressed(KEY_DOWN)) {
            menuInfo.mainScreenSelection++; 
            printf("trigger down\n"); 
        }
        if ((menuInfo.mainScreenSelection) >= (*events).events.size()) menuInfo.mainScreenSelection--; 
        if (IsKeyPressed(KEY_UP)) {
            menuInfo.mainScreenSelection--;     
            printf("trigger up\n"); 
        }
        if ((menuInfo.mainScreenSelection) < 0) menuInfo.mainScreenSelection++; 
        
        //open delete menu
        if (IsKeyPressed(KEY_D)){
            printf("trigger d\n"); 
            grayOutMainScreen = true; 
            menuInfo.eventMenu = false; 
            menuInfo.deleteMenu = true; 
        }

        //trigger next event
        if (IsKeyPressed(KEY_M)){
            printf("trigger m\n"); 
            if ((*events).events.size() > 0)
                (*events).deployEvent(); 
            else printf("no events\n"); 
        }	
    }

    //delete menu 
    if (menuInfo.deleteMenu){
        if (menuInfo.deleteCommandMenu){
            //delete a command
            int _max = (*events).events[menuInfo.mainScreenSelection].commandDescriptor.size(); 
            if (IsKeyPressed(KEY_DOWN)){
                printf("tigger down\n"); 
                menuInfo.deleteCommandSelection++; 
                if (menuInfo.deleteCommandSelection >= _max) menuInfo.deleteCommandSelection--; 
            }
            if (IsKeyPressed(KEY_UP)){
                printf("trigger up\n"); 
                menuInfo.deleteCommandSelection--; 
                if (menuInfo.deleteCommandSelection < 0) menuInfo.deleteCommandSelection++; 
            }
            if (IsKeyPressed(KEY_ENTER)){
                printf("trigger enter\n"); 
                (*events).deleteCommandFromEvent(menuInfo.mainScreenSelection, menuInfo.deleteCommandSelection);
                menuInfo.exitOutOfDeleteMenu = true;  
            }
        } else {
            //delete menu
            if (IsKeyPressed(KEY_LEFT)){
                printf("trigger left\n"); 
                if (--menuInfo.deleteMenuSelection < 0) menuInfo.deleteMenuSelection = 1;
            }
            if (IsKeyPressed(KEY_RIGHT)){
                printf("trigger right\n"); 
                if (++menuInfo.deleteMenuSelection > 1) menuInfo.deleteMenuSelection = 0;
            }
            if (IsKeyPressed(KEY_ENTER)){
                printf("trigger enter\n"); 
                if (menuInfo.deleteMenuSelection == 0){
                    (*events).deleteEvent(menuInfo.mainScreenSelection); 
                    menuInfo.exitOutOfDeleteMenu = true; 
                } else if (menuInfo.deleteMenuSelection == 1){
                    menuInfo.deleteCommandMenu = true; 
                }
                
            }
        }
        //exit out of menu
        if (menuInfo.exitOutOfDeleteMenu){
            menuInfo.deleteMenuSelection = 0; 
            menuInfo.deleteMenu = false; 
            menuInfo.deleteCommandSelection = 0; 
            menuInfo.deleteCommandMenu = false; 
            menuInfo.eventMenu = true; 
            menuInfo.exitOutOfDeleteMenu = false; 

            grayOutMainScreen = false; 
        }
    }
}

void Screen::printMouseCoord(){
    DrawText(TextFormat("%d, %d", GetMouseX(), GetMouseY()), 900, 600, 20, BLACK); 
}

void Screen::drawPianoRoll(int x, int y){
    std::string mappingLetter; 

    int pianoY = y + 25; 

    DrawTextEx(text.thickFont, "piano mappings", (Vector2){(float)x, (float)y}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    //white keys
    for (int i = 0; i < 10; i++){
        DrawRectangle(x + (i * piano.off), pianoY, piano.keyWidth, piano.whiteKeyHeight, color.bright); 
        switch (i){
            case 1: 
                mappingLetter = 'A'; 
                break; 
            case 0: 
                mappingLetter = 'S'; 
                break; 
            case 2: 
                mappingLetter = 'D'; 
                break; 
            case 3: 
                mappingLetter = 'F'; 
                break; 
            case 4: 
                mappingLetter = 'G'; 
                break;
            case 5: 
                mappingLetter = 'H'; 
                break;
            case 6: 
                mappingLetter = 'J'; 
                break;
            case 7:
                mappingLetter = 'K'; 
                break;
            case 8: 
                mappingLetter = 'L'; 
                break;
            case 9: 
                mappingLetter = ';'; 
                break;
            default: 
                mappingLetter = ' '; 
                break; 
        }
        DrawTextEx(text.thickFont, mappingLetter.c_str(), 
                (Vector2){(float)(x + (i * (piano.off)) + piano.qKeyWidth), (float)(pianoY + piano.hWHeight)}, 
                piano.whiteFontSize, 0, color.midTone); 
    }
    //black keys
    for (int i = 0; i < 10; i++){
        if ((i == 2) or (i == 6) or (i == 9)); 
        else{
            DrawRectangle(x + (piano.hKeyWidth) + (i * piano.off), pianoY, 
                piano.keyWidth, piano.blackKeyHeight, color.dark); 
            
            switch(i){
                case 0: 
                    mappingLetter = 'W'; 
                    break; 
                case 1: 
                    mappingLetter = 'E'; 
                    break; 
                case 3: 
                    mappingLetter = 'T'; 
                    break; 
                case 4: 
                    mappingLetter = 'Y'; 
                    break; 
                case 5: 
                    mappingLetter = 'U'; 
                    break; 
                case 7: 
                    mappingLetter = 'O'; 
                    break; 
                case 8: 
                    mappingLetter = 'P'; 
                    break; 
                default: 
                    mappingLetter = ' '; 
                    break; 
            }
            DrawTextEx(text.thickFont, mappingLetter.c_str(), 
                (Vector2){(float)(x + (i * piano.off) + piano.qKeyWidth + piano.hKeyWidth), (float)(pianoY)}, 
                piano.blackFontSize, 0, color.midTone); 
        } 
    }
}

void Screen::drawVoiceInfo(int x, int y){
    int maxNote = -2; 
    int minNote = 130; 
    //title
    DrawTextEx(text.thickFont, "voice info", (Vector2){(float)x, (float)y}, 
    text.titleFontSize, text.titleSpacing, color.accent); 

    //note names
    for (int i = 0 ; i < (*stream).info.numVoices; i++){
        DrawTextEx(text.thickFont, TextFormat("Voice %d", i), 
            (Vector2){(float)(x + 10), (float)(y + 25 + (i * 25))}, 
            text.titleFontSize, text.bodySpacing, color.midToneDark);

        std::string name; 
        int oct; 
        midiToName((*stream).info.notes[i], oct, name); 
        DrawTextEx(text.bodyFont, TextFormat("%s%d", name.c_str(), oct), 
            (Vector2){(float)(x + 100), (float)(y + 28 + (i * 25))}, 
            text.bodyFontSize, text.bodySpacing, color.bright); 
        
        DrawTextEx(text.bodyFont, TextFormat("%0.2f", (*stream).info.amps[i]), 
            (Vector2){(float)(x + 160), (float)(y + 28 + (i * 25))}, 
            text.bodyFontSize, text.bodySpacing, color.bright); 
        
        if ((*stream).info.notes[i] > maxNote) maxNote = (*stream).info.notes[i]; 
        if ((*stream).info.notes[i] < minNote) minNote = (*stream).info.notes[i]; 
    }
    //labels
    DrawTextEx(text.bodyFont, "notes", 
        (Vector2){(float)(x + 90), (float)(y + 23 + ((*stream).info.numVoices * 25))}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 

    DrawTextEx(text.bodyFont, "amp", 
        (Vector2){(float)(x + 160), (float)(y + 23 + ((*stream).info.numVoices * 25))}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 

    DrawRectangle((x + 200), (y + 20), 50, 100, color.midToneDark); 
    
    //draw little circles and rectangles
    for (int i = 0 ; i < (*stream).info.numVoices; i++){
        float normNote = ((float)(*stream).info.notes[i] - minNote) / (maxNote - minNote); 
        Color circleColor = color.voiceColors[i]; 
        circleColor.a = 255 * (*stream).info.amps[i]; 
        DrawCircle((x + 225), (y + 30 + ((1 - normNote) * 80)), 5, circleColor); 

        DrawRectangle((float)(x + 135), (float)(y + 35 + (i * 25)), 10, 4, circleColor); 
    }
    
    DrawTextEx(text.thickFont, "total amp", 
        (Vector2){(float)(x), (float)(y + 145)}, text.titleFontSize, 
        text.titleSpacing, color.midTone); 
    
    DrawTextEx(text.thickFont, TextFormat("%0.2f", (*stream).info.totalAmp), 
        (Vector2){(float)(x + 120), (float)(y + 145)}, text.titleFontSize, 
        text.titleSpacing, color.midToneDark); 
}

void Screen::drawEventInfo(int x, int y){
    //subtitles
    DrawTextEx(text.thickFont, "events", (Vector2){(float)x, (float)y}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    
    DrawTextEx(text.bodyFont, "new event [n]", (Vector2){(float) (x), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 
    
    DrawTextEx(text.bodyFont, "deploy event (first) [m]", 
        (Vector2){(float) (x + 125), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 
    
    DrawTextEx(text.bodyFont, "add to event [a]", 
        (Vector2){(float) (x + 335), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 

    DrawTextEx(text.bodyFont, "delete [d]", 
        (Vector2){(float) (x + 490), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 

    int yIndex = 0; 
    for (int i = menuInfo.mainScreenSelection; i < menuInfo.mainScreenSelection + 3; i++){
        if (i < (*events).events.size()){
            if (i == menuInfo.mainScreenSelection){
                DrawTextEx(text.bodyFont, TextFormat("->event %d", i), 
                    (Vector2){(float)(x + 5), (float)(y + 60 + (20 * (yIndex++)))}, 
                    text.bodyFontSize, text.bodySpacing, color.bright); 
            } else {
                DrawTextEx(text.bodyFont, TextFormat("->event %d", i), 
                    (Vector2){(float)(x + 5), (float)(y + 70 + (20 * (yIndex++)))}, 
                    text.bodyFontSize, text.bodySpacing, color.bright); 
                }
        }
    }
    DrawRectangleLinesEx((Rectangle){(float)(x), (float)(y + 55), 90, 25}, 2, color.midTone); 
    
    //descriptors
    DrawRectangle(x + 130, y + 50, 350, 100, color.midToneDark); 
    int _index = 0; 
    for (auto command : (*events).events[menuInfo.mainScreenSelection].commandDescriptor){
        DrawTextEx(text.bodyFont, command.c_str(), 
            (Vector2){(float)(x + 135), (float)(y + 55 + (_index * 20))}, text.bodyFontSize, 
            text.bodySpacing, color.bright); 
        _index++; 
    }
}

void Screen::drawDeleteMenu(int x, int y){
    //bkg
    DrawRectangle(x, y, 500, 150, color.midTone); 
   
    //little main menu animation
    Color titleColor = (frameCount > 15) ? color.dark : color.red; 

    if (!menuInfo.deleteCommandMenu){
        DrawTextEx(text.thickFont, "what do you want to delete?", 
            (Vector2){(float)(x + 33), (float)(y + 5)}, text.titleFontSize * 1.5, 
            text.titleSpacing, titleColor); 
        
        DrawTextEx(text.thickFont, "the event", (Vector2){(float)(x + 20), (float)(y + 70)},
            text.titleFontSize * 1.2, text.titleSpacing, color.dark); 
        
        DrawTextEx(text.thickFont, "a command in the event", (Vector2){(float)(x + 170), (float)(y + 70)},
            text.titleFontSize * 1.2, text.titleSpacing, color.dark); 
        
        if (menuInfo.deleteMenuSelection == 0){
            DrawRectangleLinesEx((Rectangle){(float)(x + 15), (float)(y + 65), 135, 30}, 
                2, color.midToneDark); 
        } else if (menuInfo.deleteMenuSelection == 1){
            DrawRectangleLinesEx((Rectangle){(float)(x + 165), (float)(y + 65), 305, 30}, 
                2, color.midToneDark); 
        }
    } else {
        DrawTextEx(text.thickFont, "what command?", 
            (Vector2){(float)(x + 130), (float)(y + 5)}, text.titleFontSize * 1.5, 
            text.titleSpacing, titleColor); 
        
        int printIndex = 0; //for orienting the commands
        for (int i = menuInfo.deleteCommandSelection; i < menuInfo.deleteCommandSelection + 2; i++){
            if (i < (*events).events[menuInfo.mainScreenSelection].commandDescriptor.size()){
                DrawTextEx(text.thickFont, 
                    (*events).events[menuInfo.mainScreenSelection].commandDescriptor[i].c_str(), 
                    (Vector2){(float)(x + 5), (float)(y + 70 + (25 * printIndex))},
                    text.bodyFontSize * 1.3, text.bodySpacing, color.dark); 
            }
            printIndex++; 
        }
        Color boxColor = (frameCount > 15) ? color.midToneDark : color.red; 
        DrawRectangleLinesEx((Rectangle){(float)(x + 1),(float)(y + 65), 400, 30}, 2, boxColor); 
    }

}