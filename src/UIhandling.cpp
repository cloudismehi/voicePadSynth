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
    
    //piano mappings
    drawPianoRoll(10, 10); 

    //voice info
    drawVoiceInfo(10, 90); 

    //events info
    drawEventInfo(330, 10); 
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
    
    DrawTextEx(text.bodyFont, "open event [j]", (Vector2){(float) (x), (float)(y + 40)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 
    
    DrawTextEx(text.bodyFont, "deploy event (first) [m]", 
        (Vector2){(float) (x + 125), (float)(y + 20)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 
    
    DrawTextEx(text.bodyFont, "close event [k]", 
        (Vector2){(float) (x + 125), (float)(y + 40)}, 
        text.bodyFontSize, text.bodySpacing, color.midTone); 

    int yIndex = 0; 
    for (int i = eventInfo.eventSelected; i < eventInfo.eventSelected + 3; i++){
        if (i < (*events).events.size()){
            if (i == eventInfo.eventSelected){
                DrawTextEx(text.bodyFont, TextFormat("->event %d", i), 
                    (Vector2){(float)(x + 5), (float)(y + 70 + (20 * (yIndex++)))}, 
                    text.bodyFontSize, text.bodySpacing, color.bright); 
            } else {
                DrawTextEx(text.bodyFont, TextFormat("->event %d", i), 
                    (Vector2){(float)(x + 5), (float)(y + 80 + (20 * (yIndex++)))}, 
                    text.bodyFontSize, text.bodySpacing, color.bright); 
                }
        }
    }
    DrawRectangleLinesEx((Rectangle){(float)(x), (float)(y + 65), 90, 25}, 2, color.midTone); 
    
    DrawRectangle(x + 130, y + 80, 350, 100, color.midToneDark); 
    int _index = 0; 
    for (auto command : (*events).events[eventInfo.eventSelected].commandDescriptor){
        DrawTextEx(text.bodyFont, command.c_str(), 
            (Vector2){(float)(x + 135), (float)(y + 85 + (_index * 20))}, text.bodyFontSize, 
            text.bodySpacing, color.bright); 
        _index++; 
    }

    //poll menu input
    if (IsKeyPressed(KEY_DOWN)) eventInfo.eventSelected++; 
    if ((eventInfo.eventSelected) >= (*events).events.size()) eventInfo.eventSelected--; 
    if (IsKeyPressed(KEY_UP)) eventInfo.eventSelected--;     
    if ((eventInfo.eventSelected) < 0) eventInfo.eventSelected++; 

}
