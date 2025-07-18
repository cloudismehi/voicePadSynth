#include "UIhandling.hpp"

Screen::Screen(Stream &_stream, Audio &_audioInstance, Event &_events, int &_numVoices){
    stream = &_stream; 
    audioInstance = &_audioInstance; 
    events = &_events;
    numVoices = &_numVoices; 
    
    assignVoiceColors(); 
}   

void Screen::update(){
    ClearBackground(color.dark);    
    
    //keep track of frames
    if (++frameCount > 30) frameCount = 0; 

    drawMainWindow(); 
    
    //delete menu
    if (menuInfo.deleteMenu){
        grayScreen(); 
        drawDeleteMenu((width / 2) - 250, (height / 2) - 100); 
        pollDeleteMenu(); 
    }
    else if (menuInfo.saveEvent){
        grayScreen(); 
        drawSaveFile((width / 2) - 250, (height / 2) - 100); 
        pollSaveFile(); 
    } 
    else if (menuInfo.loadFile){
        grayScreen(); 
        drawLoadFile((width / 2) - 250, (height / 2) - 100); 
        pollLoadFile(); 
    } 
    else if (menuInfo.changeScreen){
        grayScreen(); 
        drawChangeScreen((width / 2) - 250, (height / 2) - 100); 
        pollChangeScreen();   
    }
    else if (menuInfo.globalChange){
        grayScreen(); 
        drawGlobalChangeScreen((width / 2) - 250, (height / 2) - 100); 
        pollGlobalChangeScreen(); 
    }
    else {
        pollMainMenu();     
    }
}

void Screen::loadFonts(){
    text.bodyFont = LoadFont("assets/fonts/alpha_beta.png"); 
    text.thickFont = LoadFont("assets/fonts/upheavtt.ttf"); 
    if (IsFontValid(text.thickFont)) std::cout << "[FONT]  thick font loaded\n"; 
    if (IsFontValid(text.bodyFont)) std::cout << "[FONT]  body font loaded\n"; 
}

void Screen::assignVoiceColors(){
    int index = 0; 
    for(int i = 0 ; i < (*stream).info.numVoices; i++){
        color.voiceColors.push_back(color.voiceColorOptions[index]); 
        if (++index >= 4) index = 0; 
    }
}

void Screen::printMouseCoord(){
    DrawText(TextFormat("%d, %d", GetMouseX(), GetMouseY()), 900, 600, 20, BLACK); 
}

void Screen::grayScreen(){
    DrawRectangle(0, 0, width, height, color.grayOutColor); 
}

void Screen::emptyOutQueue(){
    int key = GetKeyPressed(); 
    while (key != 0){
        key = GetKeyPressed(); 
    }
}

void Screen::drawMainWindow(){
    //piano mappings
    DrawTextEx(text.thickFont, "piano mappings", (Vector2){10.f, 10.f}, 
        text.titleFontSize, text.titleSpacing, color.accent); 

    if ((*stream).info.playMode){
        DrawTextEx(text.thickFont, "play mode", (Vector2){20.f, 300.f}, 
            text.titleFontSize, text.titleSpacing, color.red); 
    } else {        
        DrawTextEx(text.thickFont, "edit mode", (Vector2){20.f, 300.f}, 
            text.titleFontSize, text.titleSpacing, color.red); 
    }

    drawPianoRoll(10, 35); 
    //voice info
    drawVoiceInfo(10, 120); 
    //events info
    drawEventInfo(330, 10); 
}

void Screen::pollMainMenu(){
    //main screen 
    bool isThereEventsQueued = !(*events).events.empty(); 
    
    //always poll
    //change piano octave
    if (IsKeyPressed(KEY_Z)){
        if (--piano.octave < 0) piano.octave = 0; 
    } 
    if (IsKeyPressed(KEY_X)){
        if (++piano.octave > 9) piano.octave--; 
    }

    //go into play/edit mode
    if (IsKeyPressed(KEY_P)){
        if ((*stream).info.playMode){
            //enter edit mode
            (*events).enterEditMode(); 
        } else {
            //enter play mode
            (*events).enterPlayMode(); 
        }
        (*stream).info.playMode = !(*stream).info.playMode; 
    }
    //creating new event
    if (IsKeyPressed(KEY_N)){
        printf("trigger n\n"); 
        (*events).newEvent(); 
    }
    pollChange(); //adding new commands or new event 

    //print report for trouble shooting
    if (IsKeyPressed(KEY_W)){
        for (int i = 0; i < (*stream).info.numVoices; i++){
            std::cout << (*stream).info.freqs[i] << ' ' << (*stream).info.inits.at("freq")[i] << '\n'; 
        }
        std::cout << '\n'; 
    }

    if ((*stream).info.playMode){ //play mode
        
        if (isThereEventsQueued){
            //delete event
            if (IsKeyPressed(KEY_D)){
                printf("trigger d\n"); 
                menuInfo.deleteMenu = true; 
            }
            
            //trigger event
            if(IsKeyPressed(KEY_M)){
                if ((*events).events.size() != 0) { (*events).deployEvent(); } 
                else { std::cout << "no events to deploy!\n"; }
            }
            
            //scroll commands
            scrollEvents();    
        }
    } else { //edit mode
        //load events
        if (IsKeyPressed(KEY_L)){
            printf("trigger l\n"); 
            menuInfo.loadFile = true; 
            (*events).getFilenames();  
        }

        if (isThereEventsQueued){
            //delete events
            if (IsKeyPressed(KEY_D)){
                printf("trigger d\n"); 
                menuInfo.deleteMenu = true; 
            }

            scrollEvents(); 

            //save
            if (IsKeyPressed(KEY_S)){
                printf("trigger s\n"); 
                menuInfo.saveEvent = true; 
            }

            //preview (doesn't exist yet)
        }
    }
}

void Screen::scrollEvents(){
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
}

void Screen::pollChange(){
    int key = GetKeyPressed(); 

    if ((key >= 321) and (key <= (320 + *numVoices))){
        change.voice = key - 320; 
        menuInfo.changeScreen = true; 
    }
    else if ((key >= 49) and (key <= (48 + *numVoices))){
        change.voice = key - 48; 
        menuInfo.changeScreen = true; 
    }
    if (IsKeyPressed(KEY_T)){
        change.voice = -1; 
        menuInfo.globalChange = true; 
    }
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
    DrawTextEx(text.bodyFont, TextFormat("octave - %d  [z : down | x : up]", piano.octave),
        {(float)(x + 10), (float)(y + piano.whiteKeyHeight)}, text.bodyFontSize, text.bodySpacing, color.midTone); 
    DrawTextEx(text.bodyFont, "[q] to reset", {(float)(x + 10), (float)(y + piano.whiteKeyHeight + 15)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 
}

bool Screen::pollPianoRoll(){
    bool triggered = false; 
    if (IsKeyPressed(KEY_A)){ piano.note = 24 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_W)){ piano.note = 25 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_S)){ piano.note = 26 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_E)){ piano.note = 27 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_D)){ piano.note = 28 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_F)){ piano.note = 29 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_T)){ piano.note = 30 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_G)){ piano.note = 31 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_Y)){ piano.note = 32 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_H)){ piano.note = 33 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_U)){ piano.note = 34 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_J)){ piano.note = 35 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_K)){ piano.note = 36 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_O)){ piano.note = 37 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_L)){ piano.note = 38 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_P)){ piano.note = 39 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_SEMICOLON)){ piano.note = 40 + (12 * (piano.octave - 1)); triggered = true; }
    else if (IsKeyPressed(KEY_Q)){ piano.note = -1; triggered = true; }
    else if (IsKeyPressed(KEY_Z)){ if (--piano.octave < 0) piano.octave = 0; triggered = true; }
    else if (IsKeyPressed(KEY_X)){ if (++piano.octave > 9) piano.octave--; triggered = true; }

    return triggered; 
}

void Screen::drawVoiceInfo(int x, int y){
    int maxNote = -2; 
    int minNote = 130; 
    //title
    DrawTextEx(text.thickFont, "voice info", (Vector2){(float)x, (float)y}, 
    text.titleFontSize, text.titleSpacing, color.accent); 

    //note names
    for (int i = 0 ; i < (*stream).info.numVoices; i++){
        DrawTextEx(text.bodyFont, TextFormat("Voice %d", i + 1), 
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
    
    DrawTextEx(text.bodyFont, "total amp", 
        (Vector2){(float)(x), (float)(y + 145)}, text.titleFontSize, 
        text.titleSpacing, color.midTone); 
    
    DrawTextEx(text.bodyFont, TextFormat("%0.2f", (*stream).info.totalAmp), 
        (Vector2){(float)(x + 120), (float)(y + 145)}, text.titleFontSize, 
        text.titleSpacing, color.midToneDark); 
}

void Screen::drawEventInfo(int x, int y){
    Vector2 measured; 
    bool isThereEventsQueued = !(*events).events.empty(); 
    //subtitles
    DrawTextEx(text.thickFont, "events", (Vector2){(float)x, (float)y}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    
    DrawTextEx(text.bodyFont, "new event [n]", (Vector2){(float) (x), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midToneDark); 
    measured = MeasureTextEx(text.bodyFont, "new event[n]", text.bodyFontSize, text.bodySpacing); 
    DrawRectangle(x + 2, y + 40, measured.x, 2, color.midToneDark); 
    
    if ((*stream).info.playMode){   //play mode
        DrawTextEx(text.bodyFont, "goto edit mode[p]", {(float)(x + measured.x + 40), (float)(y + 20)}, 
            text.bodyFontSize, text.bodySpacing, color.midToneDark);         
        DrawRectangle(x + measured.x + 42, y + 40, 132, 2, color.midToneDark);         

        if (isThereEventsQueued){
            DrawTextEx(text.bodyFont, "trigger event [m]", (Vector2){(float) (x), (float)(y + 160)}, 
                text.bodyFontSize, text.bodySpacing, color.midToneDark); 
            DrawRectangle(x + 2, y + 180, 138, 2, color.midToneDark);  
            measured = MeasureTextEx(text.bodyFont, "trigger event [m]", text.bodyFontSize, text.bodySpacing); 

            DrawTextEx(text.bodyFont, "delete [d]", (Vector2){(float) (x + measured.x + 20), (float)(y + 160)}, 
                text.bodyFontSize, text.bodySpacing, color.midToneDark); 
            DrawRectangle(x + measured.x + 22, y + 180, 73, 2, color.midToneDark);  
        }

    } else {                        //edit mode
        DrawTextEx(text.bodyFont, "goto play mode[p]", {(float)(x + measured.x + 20), (float)(y + 20)}, 
            text.bodyFontSize, text.bodySpacing, color.midToneDark);
        DrawRectangle(x + measured.x + 22, y + 40, 138, 2, color.midToneDark);  

        Vector2 foo = MeasureTextEx(text.bodyFont, "goto play mode[p]", text.bodyFontSize, text.bodySpacing); 
        measured.x += foo.x; 
        
        DrawTextEx(text.bodyFont, "load file[l]", {(float)(x + measured.x + 35), (float)(y + 20)}, 
            text.bodyFontSize, text.bodySpacing, color.midToneDark); 
        DrawRectangle(x + measured.x + 37, y + 40, 78, 2, color.midToneDark);  

        if (isThereEventsQueued){
            DrawTextEx(text.bodyFont, "save events [m]", (Vector2){(float) (x), (float)(y + 160)}, 
                text.bodyFontSize, text.bodySpacing, color.midToneDark); 
            DrawRectangle(x + 2, y + 180, 126, 2, color.midToneDark);  
            measured = MeasureTextEx(text.bodyFont, "save events [m]", text.bodyFontSize, text.bodySpacing); 

            DrawTextEx(text.bodyFont, "delete[d]", {(float)(x + measured.x + 20), (float)(y + 160)}, 
                text.bodyFontSize, text.bodySpacing, color.midToneDark); 
            DrawRectangle(x + measured.x + 22, y + 180, 65, 2, color.midToneDark);  
        }
    }

    //put events and info on the screen
    if ((*events).events.size() == 0){
        DrawRectangle(x + 130, y + 50, 350, 100, color.midToneDark); 
        DrawTextEx(text.bodyFont, "no events", (Vector2){(float)(x + 200), (float)(y + 80)}, 
            text.bodyFontSize * 1.4, text.bodySpacing, color.dark); 
    } else {
        int yIndex = 0; 
        for (int i = menuInfo.mainScreenSelection; i < menuInfo.mainScreenSelection + 3; i++){
            if (i < (*events).events.size()){
                if (i == menuInfo.mainScreenSelection){
                    DrawTextEx(text.bodyFont, TextFormat("->event %d", i + 1), 
                        (Vector2){(float)(x + 5), (float)(y + 60 + (20 * (yIndex++)))}, 
                        text.bodyFontSize, text.bodySpacing, color.bright); 
                } else {
                    DrawTextEx(text.bodyFont, TextFormat("->event %d", i + 1), 
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
}

void Screen::drawDeleteMenu(int x, int y){
    //bkg
    DrawRectangle(x, y, 500, 150, color.midTone); 
   
    //little main menu animation
    Color titleColor = (frameCount > 15) ? color.dark : color.red; 

    if (!menuInfo.deleteCommandMenu){
        DrawTextEx(text.thickFont, "what do you want to delete?", 
            (Vector2){(float)(x + 33), (float)(y + 5)}, text.titleFontSize, 
            text.titleSpacing, titleColor); 
        
        DrawTextEx(text.bodyFont, "the event", (Vector2){(float)(x + 20), (float)(y + 70)},
            text.titleFontSize, text.titleSpacing, color.dark); 
        
        DrawTextEx(text.bodyFont, "a command in the event", (Vector2){(float)(x + 170), (float)(y + 70)},
            text.titleFontSize, text.titleSpacing, color.dark); 
        
        if (menuInfo.deleteMenuSelection == 0){
            DrawTextEx(text.thickFont, ">", {(float)(x + 8), (float)(y + 70)}, 
                text.titleFontSize, text.titleSpacing, color.dark); 
        } else if (menuInfo.deleteMenuSelection == 1){
            DrawTextEx(text.thickFont, ">", {(float)(x + 158), (float)(y + 70)}, 
                text.titleFontSize, text.titleSpacing, color.dark); 
        }
    } else {
        DrawTextEx(text.thickFont, "what command?", 
            (Vector2){(float)(x + 130), (float)(y + 5)}, text.titleFontSize, 
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
    emptyOutQueue(); 
    if (IsKeyPressed(KEY_C)){ menuInfo.exitOutOfDeleteMenu = true; }
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
        menuInfo.exitOutOfDeleteMenu = false; 
    }
    
    emptyOutQueue(); 
}

void Screen::drawSaveFile(int x, int y){
    DrawRectangle(x, y, 500, 150, color.midTone); 
    Color titleColor = (frameCount > 15) ? color.dark : color.red; 

    DrawTextEx(text.thickFont, "save events", {(float)(x + 5), (float)(y + 5)}, 
        text.titleFontSize, text.titleSpacing, titleColor); 
    DrawTextEx(text.thickFont, "what name for the file?", {(float)(x + 5), (float)(y + 30)}, 
        text.titleFontSize * 0.8, text.titleSpacing, color.dark); 

    DrawTextEx(text.bodyFont, change.filename.c_str(), {(float)(x + 5), (float)(y + 80)}, 
        text.titleFontSize, text.bodySpacing, color.dark); 
    Color cursorColor = (frameCount < 15) ? color.dark : color.midTone; 
    DrawTextEx(text.bodyFont, "_", 
        {x + 5 + MeasureTextEx(text.bodyFont, change.filename.c_str(), text.titleFontSize, text.bodySpacing).x, (float)(y + 80)}, 
        text.titleFontSize, text.bodySpacing, cursorColor); 
}

void Screen::pollSaveFile(){
    int key = GetKeyPressed(); 
    while (key > 0){
        if ((key >= 39) and (key <= 96)){
            if (change.filename.size() < change.maxFileTitleLength){
                change.filename.push_back(std::tolower((char)key)); 
            }
        }
        else if (key == KEY_SPACE) {
            change.filename.push_back(' ');
        }
        else if (key == KEY_BACKSPACE) {
            change.filename.pop_back();
        }
        if (key == KEY_ENTER){
            menuInfo.saveEvent = false; 
            (*events).saveEvents(change.filename + ".dat"); 

            change.filename = ""; 
        }
        key =  GetKeyPressed(); 
    }
}

void Screen::drawLoadFile(int x, int y){
    DrawRectangle(x, y, 500, 150, color.midTone); 
    Color titleColor = (frameCount > 15) ? color.dark : color.red; 
    DrawTextEx(text.thickFont, "load events", {(float)(x + 5), (float)(y + 5)}, 
        text.titleFontSize, text.titleSpacing, titleColor); 
    
    int printIndex = 0; 
    for (int i = menuInfo.loadFileSelection; i < menuInfo.loadFileSelection + 2; i++){
        if (i < (*events).savedEventFilenames.size()){
            DrawTextEx(text.bodyFont, (*events).savedEventFilenames[i].c_str(), 
                (Vector2){(float)(x + 20), (float)(y + 50 + (printIndex * 30))}, 
                text.bodyFontSize * 1.5, text.bodySpacing, color.dark); 
            
            DrawTextEx(text.thickFont, ">", {(float)(x + 7), (float)(y + 50)}, 
                text.titleFontSize, text.titleSpacing, color.dark); 
            printIndex++; 
        }
    }
}

void Screen::pollLoadFile(){
    if (IsKeyPressed(KEY_DOWN)){
        if (++menuInfo.loadFileSelection >= (*events).savedEventFilenames.size()){
            menuInfo.loadFileSelection--; 
        }
    }
    if (IsKeyPressed(KEY_UP)){
        if (--menuInfo.loadFileSelection < 0) menuInfo.loadFileSelection = 0; 
    }
    if (IsKeyPressed(KEY_C)){
        menuInfo.loadFile = false;
        menuInfo.loadFileSelection = 0; 
    }
    if (IsKeyPressed(KEY_ENTER)){
        (*events).loadEvents((*events).savedEventFilenames[menuInfo.loadFileSelection]);
        menuInfo.loadFile = false;
        menuInfo.loadFileSelection = 0; 
    }
}  

void Screen::drawChangeScreen(int x, int y){
    DrawRectangle(x, y, 550, 250, color.midTone); 
    Color titleColor = (frameCount > 15) ? color.dark : color.red; 
    int _octave; 
    std::string _note; 

    DrawTextEx(text.thickFont, "piano mappings", (Vector2){10.f, 10.f}, 
        text.titleFontSize, text.titleSpacing, color.accent); 
    drawPianoRoll(10, 35); 
    //voice info
    drawVoiceInfo(10, 120);

    midiToName(piano.note, _octave, _note); 
    DrawTextEx(text.thickFont, TextFormat("changing voice %d", change.voice), 
        {(float)(x + 5), (float)(y + 5)}, text.titleFontSize, text.titleSpacing, titleColor); 


    std::string _newNote = (piano.note == -1) ? "new note: ?" : 
        TextFormat("new note: %s%d", _note.c_str(), _octave); 
    DrawTextEx(text.thickFont, _newNote.c_str(), 
        {(float)(x + 5), (float)(y + 30)}, text.titleFontSize, text.titleSpacing, color.dark); 
    
    std::string _newAmp = (change.newAmp == -1) ? "new amp : ?" :  TextFormat("new amp : %0.2f", change.newAmp); 
    DrawTextEx(text.thickFont, _newAmp.c_str(), {(float)(x + 5), (float)(y + 60)}, 
        text.titleFontSize, text.titleSpacing, color.dark); 
    DrawTextEx(text.bodyFont, "[up/down arrow keys : change | shift + up/down : reset]", 
        {(float)(x + 15), (float)(y + 78)}, text.bodyFontSize, text.bodySpacing, color.dark); 
    
    DrawTextEx(text.thickFont, TextFormat("time : %0.2f", change.time), {(float)(x + 5), (float)(y + 100)}, 
        text.titleFontSize, text.titleSpacing, color.dark); 
    DrawTextEx(text.bodyFont, "[left/right arrow keys : change | shift + left/right : reset]", 
        {(float)(x + 15), (float)(y + 118)}, text.bodyFontSize, text.bodySpacing, color.dark); 
    
    DrawTextEx(text.bodyFont, "[use shift + enter to change instantly]", {(float)(x+5), (float)(y + 225)}, 
        text.bodyFontSize, text.bodySpacing, color.dark); 
    
}

void Screen::pollChangeScreen(){
    
    pollPianoRoll(); 
    if (IsKeyPressed(KEY_DOWN)){
        if (change.newAmp == -1) change.newAmp = 0; 
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.newAmp = -1; 
        }
        else {
            if ((change.newAmp -= change.incr) < 0) change.newAmp = 0; 
        }
    }
    if (IsKeyPressed(KEY_UP)){
        if (change.newAmp == -1) change.newAmp = 1; 
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.newAmp = -1; 
        }
        else {
            if ((change.newAmp += change.incr) > 1) change.newAmp = 1; 
        }
    }
    if (IsKeyPressed(KEY_LEFT)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.time = 0; 
        }
        else if (IsKeyDown(KEY_LEFT_ALT) or IsKeyDown(KEY_RIGHT_ALT)){
            if ((change.time -= change.largeTimeIncr) < 0) change.time = 0; 
        }
        else {
            if ((change.time -= change.timeIncr) < 0) change.time = 0; 
        }
    }
    if (IsKeyPressed(KEY_RIGHT)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.time = 0; 
        } 
        else if (IsKeyDown(KEY_LEFT_ALT) or IsKeyDown(KEY_RIGHT_ALT)){
            change.time += change.largeTimeIncr; 
        }
        else {
            change.time += change.timeIncr; 
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)){
        if (piano.note != -1){
            //there was a note change
            
            //instant change
            if (IsKeyDown(KEY_RIGHT_SHIFT) or IsKeyDown(KEY_LEFT_SHIFT)){
                (*events).possibleCommands[0].function(midiToFreq(piano.note), change.voice - 1); 
            } 
            else {
                //make a new event if there are no others 
                if ((*events).events.size() == 0){ (*events).openEvent((*events).newEvent()); }
                //add to current event
                else { (*events).openEvent(menuInfo.mainScreenSelection); }

                (*events).addToEvent((*events).possibleCommands[0].id, midiToFreq(piano.note), 
                    change.time, change.voice - 1); 
                (*events).closeEvent(); 
            }
        }
        if (change.newAmp != -1 ){
            //there was an amp change

            //instant change 
            if (IsKeyDown(KEY_RIGHT_SHIFT) or IsKeyDown(KEY_LEFT_SHIFT)){
                (*events).openEvent((*events).newEvent()); 
                (*events).addToEvent((*events).possibleCommands[1].id, change.newAmp, 0.1, change.voice - 1); 
                (*events).deployEvent((int)(*events).events.size() - 1); 
                (*events).closeEvent(); 
            } else {
                //make a new event if there are no others 
                if ((*events).events.size() == 0){ (*events).openEvent((*events).newEvent()); }
                //add to current event
                else { (*events).openEvent(menuInfo.mainScreenSelection); }

                (*events).addToEvent((*events).possibleCommands[1].id, change.newAmp, change.time, change.voice - 1); 
                (*events).closeEvent(); 
            }
        }
        
        piano.resetPiano(); 
        change.resetChange(); 
        menuInfo.changeScreenSelection = 0; 
        menuInfo.changeScreen = false; 
    }
    if (IsKeyPressed(KEY_C)){
        piano.resetPiano();
        change.resetChange();  
        menuInfo.changeScreen = false; 
        menuInfo.changeScreenSelection = 0; 
    }
}

void Screen::drawGlobalChangeScreen(int x, int y){
    DrawRectangle(x, y, 550, 250, color.midTone); 
    Color titleColor = (frameCount > 15) ? color.dark : color.red;

    DrawTextEx(text.thickFont, "global change", 
        {(float)(x + 5), (float)(y + 5)}, text.titleFontSize, text.titleSpacing, titleColor); 
    
    std::string _newAmp = (change.newAmp == -1) ? "new amp : ?" :  TextFormat("new amp : %0.2f", change.newAmp); 
    DrawTextEx(text.thickFont, _newAmp.c_str(), {(float)(x + 5), (float)(y + 30)}, 
        text.titleFontSize, text.titleSpacing, color.dark); 
    DrawTextEx(text.bodyFont, "[up/down arrow keys : change | shift + up/down : reset]", 
        {(float)(x + 15), (float)(y + 48)}, text.bodyFontSize, text.bodySpacing, color.dark); 
    
    DrawTextEx(text.thickFont, TextFormat("time : %0.2f", change.time), {(float)(x + 5), (float)(y + 70)}, 
        text.titleFontSize, text.titleSpacing, color.dark); 
    DrawTextEx(text.bodyFont, "[left/right arrow keys : change | shift + left/right : reset]", 
        {(float)(x + 15), (float)(y + 88)}, text.bodyFontSize, text.bodySpacing, color.dark); 
    
    DrawTextEx(text.bodyFont, "[use shift + enter to change instantly]", {(float)(x+5), (float)(y + 225)}, 
        text.bodyFontSize, text.bodySpacing, color.dark); 
    
}

void Screen::pollGlobalChangeScreen(){
    if (IsKeyPressed(KEY_DOWN)){
        if (change.newAmp == -1) change.newAmp = 0; 
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.newAmp = -1; 
        }
        else {
            if ((change.newAmp -= change.incr) < 0) change.newAmp = 0; 
        }
    }
    if (IsKeyPressed(KEY_UP)){
        if (change.newAmp == -1) change.newAmp = 1; 
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.newAmp = -1; 
        }
        else {
            if ((change.newAmp += change.incr) > 1) change.newAmp = 1; 
        }
    }
    if (IsKeyPressed(KEY_LEFT)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.time = 0; 
        }
        else if (IsKeyDown(KEY_LEFT_ALT) or IsKeyDown(KEY_RIGHT_ALT)){
            if ((change.time -= change.largeTimeIncr) < 0) change.time = 0; 
        }
        else {
            if ((change.time -= change.timeIncr) < 0) change.time = 0; 
        }
    }
    if (IsKeyPressed(KEY_RIGHT)){
        if (IsKeyDown(KEY_LEFT_SHIFT) or IsKeyDown(KEY_RIGHT_SHIFT)){
            change.time = 0; 
        } 
        else if (IsKeyDown(KEY_LEFT_ALT) or IsKeyDown(KEY_RIGHT_ALT)){
            change.time += change.largeTimeIncr; 
        }
        else {
            change.time += change.timeIncr; 
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)){
        if (change.newAmp != -1 ){
            //there was an amp change

            //instant change 
            if (IsKeyDown(KEY_RIGHT_SHIFT) or IsKeyDown(KEY_LEFT_SHIFT)){
                (*events).openEvent((*events).newEvent()); 
                
                (*events).addToEvent((*events).possibleCommands[1].id, change.newAmp, 0.1, -1); 
                (*events).closeEvent(); 
                (*events).deployEvent((int)(*events).events.size() - 1); 
            } else {
                //make a new event if there are no others 
                if ((*events).events.size() == 0){ (*events).openEvent((*events).newEvent()); }
                //add to current event
                else { (*events).openEvent(menuInfo.mainScreenSelection); }

                (*events).addToEvent((*events).possibleCommands[1].id, change.newAmp, change.time, -1); 
                (*events).closeEvent(); 
            }
        }
        
        piano.resetPiano(); 
        change.resetChange(); 
        menuInfo.globalChange = false; 
    }
    if (IsKeyPressed(KEY_C)){
        piano.resetPiano();
        change.resetChange(); 
        menuInfo.globalChange = false; 
    }
}