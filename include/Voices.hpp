#include <math.h> 
#include <iostream> 

class Voice{
    //sine wave generator
    private: 
    float idx = 0; 
    float incr; 
    int sampleRate, bitDepth, maxAmp; 

    void calculateIncr(); 

    public: 
    bool voiceActive = false;     
    float frequency, amp = 0.2f;

    Voice(int _sampleRate, int _bitDepth, float _initFreq = 200); 
    float genValue(); 
}; 