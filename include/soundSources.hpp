#include <iostream> 
#include <cmath>

class SineOscillator{
	public:

	float *offset, *incr, *frequency, *amp; 
    int sampleRate, bitDepth, maxAmp, numVoices; 
	float totalAmp = 1.f; 

    SineOscillator(int _sampleRate, int _bitDepth, int _numVoices); 

	float genValue(); 
	void updateOffsets(); 

    void setFreq(float _freq, int _voice); 
    void setFreqMidi(float _note, int _voice);
    void setAmp(float _amp, int _voice); 

	~SineOscillator(); 

    private: 
    float midiToFreq(int _note);    

};

class SineFold{
    public: 

    float *offset, *incr, *frequency, *amp; 
    int sampleRate, numVoices; 

    float foldAmt = 2; 

    SineFold(int _sampleRate, int _bitDepth, int _numVoices);

    float genValue(); 
	void updateOffsets(); 

    void setFreq(float _freq, int _voice); 
    void setFreqMidi(float _note, int _voice);
    
    ~SineFold(); 

    private: 
    float midiToFreq(int _note); 
    float fold(float _input); 
}; 