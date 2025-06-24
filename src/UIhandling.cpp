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

    //main window
    //piano mappings
    DrawTextEx(text.thickFont, "piano mappings", (Vector2){10.f, 10.f}, 
    text.titleFontSize, text.titleSpacing, color.accent); 
    drawPianoRoll(10, 35); 
    //voice info
    drawVoiceInfo(10, 90); 
    //events info
    drawEventInfo(330, 10); 
    //commands info
    drawCommandOptions(10, 280); 
    
    //gray out flag (kinda useless rn)
    if (grayOutMainScreen){
        grayScreen(); 
    }
    
    //delete menu
    if (menuInfo.deleteMenu){
        grayScreen(); 
        drawDeleteMenu((width / 2) - 250, (height / 2) - 100); 
        pollDeleteMenu(); 
    }

    //freq change menu
    if (menuInfo.freqChangeMenu){
        // grayScreen(); 
        drawFreqChange(10, 420); 
        pollFreqChange(); 
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
    //main screen 
    if (menuInfo.mainMenu){

        //event menu
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
            menuInfo.mainMenu = false; 
            menuInfo.deleteMenu = true; 
        }

        //trigger next event
        if (IsKeyPressed(KEY_M)){
            printf("trigger m\n"); 
            if ((*events).events.size() > 0)
                (*events).deployEvent(); 
            else printf("no events\n"); 
        }	
        if (IsKeyPressed(KEY_N)){
            printf("trigger n\n"); 
            (*events).newEvent(); 
        }
        
        //poll command list scroll
        if (IsKeyPressed(KEY_C)){
            if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
                printf("trigger shift + c\n"); 
                menuInfo.commandSelection -= 1; 
                if (menuInfo.commandSelection < 0) menuInfo.commandSelection++; 
            } else {
                printf("trigger c\n"); 
                menuInfo.commandSelection++; 
                if (menuInfo.commandSelection >= (*events).glossary.size()) menuInfo.commandSelection--; 
            }
        }
        //poll new command to event
        if (IsKeyPressed(KEY_ONE) and ((*events).glossary.size() > menuInfo.commandSelection)){
            printf("trigger 1\n");
            menuInfo.highlightCommand = menuInfo.commandSelection; 

            //new freq
            if ((*events).isFreq[menuInfo.commandSelection]){
                printf("freq change\n"); 
                menuInfo.mainMenu = false; 
                menuInfo.freqChangeMenu = true; 
            }
        }
        if (IsKeyPressed(KEY_TWO) and ((*events).glossary.size() > menuInfo.commandSelection + 1)){
            printf("trigger 2\n");
            menuInfo.highlightCommand = menuInfo.commandSelection + 1; 
        }
        if (IsKeyPressed(KEY_THREE) and ((*events).glossary.size() > menuInfo.commandSelection + 2)){
            printf("trigger 3\n");
            menuInfo.highlightCommand = menuInfo.commandSelection + 2;
            std::cout << (*events).descriptor[menuInfo.commandSelection + 2] << '\n'; 
        }

    }   
}

void Screen::printMouseCoord(){
    DrawText(TextFormat("%d, %d", GetMouseX(), GetMouseY()), 900, 600, 20, BLACK); 
}

void Screen::grayScreen(){
    DrawRectangle(0, 0, width, height, color.grayOutColor); 
}

void Screen::drawPianoRoll(int x, int y){
    std::string mappingLetter; 

    //white keys
    for (int i = 0; i < 10; i++){
        DrawRectangle(x + (i * piano.off), y, piano.keyWidth, piano.whiteKeyHeight, color.bright); 
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
                (Vector2){(float)(x + (i * (piano.off)) + piano.qKeyWidth), (float)(y + piano.hWHeight)}, 
                piano.whiteFontSize, 0, color.midTone); 
    }
    //black keys
    for (int i = 0; i < 10; i++){
        if ((i == 2) or (i == 6) or (i == 9)); 
        else{
            DrawRectangle(x + (piano.hKeyWidth) + (i * piano.off), y, 
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
                (Vector2){(float)(x + (i * piano.off) + piano.qKeyWidth + piano.hKeyWidth), (float)(y)}, 
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
        text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    
    DrawTextEx(text.bodyFont, "deploy event (first) [m]", 
        (Vector2){(float) (x + 125), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midToneDark); 

    DrawTextEx(text.bodyFont, "delete [d]", 
        (Vector2){(float) (x + 335), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midToneDark); 

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

void Screen::drawCommandOptions(int x, int y){
    DrawTextEx(text.thickFont, "commands", (Vector2){(float)x, (float)y}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    DrawTextEx(text.thickFont, "scroll down events  [c]", (Vector2){(float)(x + 5), (float)(y + 25)},
        text.bodyFontSize, text.titleSpacing, color.midToneDark); 
    DrawTextEx(text.thickFont, "scroll up events    [cmd + c]", (Vector2){(float)(x + 5), (float)(y + 45)},
        text.bodyFontSize, text.titleSpacing, color.midToneDark); 

    int printIndex = 0; 
    for (int i = menuInfo.commandSelection; i < menuInfo.commandSelection + 3; i++){
        Color commandColor = color.midTone; 
        Color shortcutColor = color.bright; 
        if (menuInfo.highlightCommand != -1){
            if (menuInfo.highlightCommand - menuInfo.commandSelection == i){
                commandColor = (frameCount > 15) ? color.midTone : color.red; 
                shortcutColor = (frameCount > 15) ? color.bright : color.red; 
            } 
        }
        if (i < (*events).descriptor.size()){
            DrawTextEx(text.thickFont, (*events).descriptor[i].c_str(), 
                (Vector2){(float)(x + 10), (float)(y + 75 + (printIndex * 20))}, 
                text.titleFontSize, text.bodySpacing, commandColor);
                
            DrawTextEx(text.bodyFont, TextFormat("[%d]", printIndex + 1), 
                (Vector2){(float)(x + 200), (float)(y + 75 + (printIndex * 20))}, 
                text.titleFontSize, text.bodySpacing, shortcutColor); 
            printIndex++;  
        }

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

void Screen::pollDeleteMenu(){
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
        menuInfo.mainMenu = true; 
        menuInfo.exitOutOfDeleteMenu = false; 
    }
}

void Screen::drawFreqChange(int x, int y){
    //cover up event menu
    DrawTextEx(text.thickFont, "freq change", (Vector2){(float)(x + 5), (float)(y + 5)}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    DrawTextEx(text.bodyFont, "[c] to cancel", (Vector2){(float)(x + 25), (float)(y + 25)}, 
        text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    DrawTextEx(text.bodyFont, "change octave [up/down]", 
        (Vector2){(float)(x + 155), (float)(y + 10)}, text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    DrawTextEx(text.bodyFont, "change time [left/right]", 
        (Vector2){(float)(x + 155), (float)(y + 25)}, text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    DrawTextEx(text.bodyFont, "change time incr [shift + left/shift + right]", 
        (Vector2){(float)(x + 370), (float)(y + 25)}, text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    DrawTextEx(text.bodyFont, "change voice [shift + up/shift + down]", 
        (Vector2){(float)(x + 370), (float)(y + 10)}, text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    
    std::string note = ""; 
    int octave = 0;
    midiToName(piano.note, octave, note); 
    DrawTextEx(text.bodyFont, TextFormat("change to: %s%d", note.c_str(), octave), 
        (Vector2){(float)(x + 15), (float)(y + 55)}, text.titleFontSize, text.titleSpacing, color.bright); 
    DrawTextEx(text.bodyFont, TextFormat("over %0.2f seconds", piano.time), 
        (Vector2){(float)(x + 15), (float)(y + 75)}, text.titleFontSize, text.titleSpacing, color.bright); 
    DrawTextEx(text.bodyFont, TextFormat("(increments of %0.2f)", piano.incr), 
        (Vector2){(float)(x + 250), (float)(y + 79)}, text.bodyFontSize, text.bodySpacing, color.bright); 
    DrawTextEx(text.bodyFont, TextFormat("on voice %d", piano.voice), 
        (Vector2){(float)(x + 15), (float)(y + 92)}, text.titleFontSize, text.titleSpacing, color.bright); 
}

void Screen::pollFreqChange(){
    if (IsKeyPressed(KEY_UP)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            piano.voice++; 
            if (piano.voice >= (*stream).info.numVoices){ piano.voice--; }
        } else {
            piano.note += 12; 
            piano.octave++; 
            if (piano.note > 108){
                piano.note -= 12; 
                piano.octave--; 
            }
        }
    }
    if (IsKeyPressed(KEY_DOWN)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            piano.voice--; 
            if (piano.voice < 0) piano.voice = 0; 
        } else {
            piano.note -= 12; 
            piano.octave--; 
            if (piano.note < 24){
                piano.note += 12; 
                piano.octave++; 
            }
        }
    }
    
    if (IsKeyPressed(KEY_A)){ piano.note = 24 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_W)){ piano.note = 25 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_S)){ piano.note = 26 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_E)){ piano.note = 27 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_D)){ piano.note = 28 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_F)){ piano.note = 29 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_T)){ piano.note = 30 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_G)){ piano.note = 31 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_Y)){ piano.note = 32 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_H)){ piano.note = 33 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_U)){ piano.note = 34 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_J)){ piano.note = 35 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_K)){ piano.note = 36 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_O)){ piano.note = 37 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_L)){ piano.note = 38 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_P)){ piano.note = 39 + (12 * (piano.octave - 1)); }
    else if (IsKeyPressed(KEY_SEMICOLON)){ piano.note = 40 + (12 * (piano.octave - 1)); }

    if (IsKeyPressed(KEY_RIGHT)) {
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            piano.incr += 0.25; 
        } else {
            piano.time += piano.incr; 
        }
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            piano.incr -= 0.25; 
            if (piano.incr < 0.25){ piano.incr = 0.25; }
        } else {
            piano.time -= piano.incr; 
            if (piano.time < 0) {piano.time = 0; }
        }
    }

    if (IsKeyPressed(KEY_C)){
        menuInfo.mainMenu = true; 
        menuInfo.freqChangeMenu = false; 
        piano.voice = 0; 
        menuInfo.highlightCommand = -1; 
        piano.note = 60;
        piano.octave = 4;
        piano.time = 0; 
        piano.incr = 0.5; 
    }

    if (IsKeyPressed(KEY_ENTER)){
        (*events).addToEvent(menuInfo.mainScreenSelection, "sineOsc_freq" ,
            midiToFreq((*stream).info.notes[piano.voice]), midiToFreq(piano.note), piano.time, piano.voice); 
        
        menuInfo.mainMenu = true; 
        menuInfo.freqChangeMenu = false; 
        piano.voice = 0; 
        menuInfo.highlightCommand = -1; 
        piano.note = 60;
        piano.octave = 4;
        piano.time = 0; 
        piano.incr = 0.5; 
    }
}