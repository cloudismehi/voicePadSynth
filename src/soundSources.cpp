#include "soundSources.hpp"

SineOscillator::SineOscillator(int _sampleRate, int _bitDepth, int _numVoices){
    offset = new float[numVoices]; 
	incr = new float[numVoices]; 

    sampleRate = _sampleRate; 
    bitDepth = _bitDepth; 
    numVoices = _numVoices; 

    frequency = new float[numVoices]; 
    amp = new float[numVoices]; 

    for (int i = 0; i < numVoices; i++){
        frequency[i] = 200.f; 
        amp[i] = 1.f; 
    }

    if (bitDepth == 16){
        maxAmp = pow(2, bitDepth - 1) - 1; 
    } else if (bitDepth == 32){
        maxAmp = 1.f; 
    }
}

void SineOscillator::updateOffsets(){
	for (int i = 0; i < numVoices; i++){
		incr[i] = frequency[i]/sampleRate; 
	}
}

float SineOscillator::genValue(){
	float out = 0.f; 
	for (int i = 0; i < numVoices; i++){
		out += amp[i] * sinf(2 * M_PI * offset[i]); 
		if ((offset[i] += incr[i]) >= 1.f) offset[i] = 0.f; 
	}
	return (totalAmp * out); 
}

void SineOscillator::setFreq(float _freq, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = _freq; 
    }
    updateOffsets(); 
}

void SineOscillator::setFreqMidi(float _note, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = midiToFreq(_note); 
    }
    updateOffsets(); 
}

void SineOscillator::setAmp(float _amp, int _voice){
    if (_voice == -1) totalAmp = _amp; 
    else {
        if (_voice >= numVoices){
            std::cout << "voice number " << _voice << " out of range\n"; 
        } else {
            amp[_voice] = _amp; 
        }
    }
}


SineOscillator::~SineOscillator(){
	delete[] offset; 
	delete[] incr; 
    delete[] frequency; 
    delete[] amp; 
}

float SineOscillator::midiToFreq(int _note){
	return 440.f * exp((log(2) * (_note - 69))/12); 
}


/* ********************************************************************************* */

SineFold::SineFold(int _sampleRate, int _bitDepth, int _numVoices){
    offset = new float[numVoices]; 
	incr = new float[numVoices]; 

    sampleRate = _sampleRate; 
    numVoices = _numVoices; 

    frequency = new float[numVoices]; 
    amp = new float[numVoices]; 

    for (int i = 0; i < numVoices; i++){
        frequency[i] = 200.f; 
        amp[i] = 0.2f; 
    }

}

void SineFold::updateOffsets(){
	for (int i = 0; i < numVoices; i++){
		incr[i] = frequency[i]/sampleRate; 
	}
}

float SineFold::genValue(){
	float out = 0.f; 
	for (int i = 0; i < numVoices; i++){
		out += amp[i] * fold(sinf(2 * M_PI * offset[i])); 
		if ((offset[i] += incr[i]) >= 1.f) offset[i] = 0.f; 
	}
	return out; 
}

void SineFold::setFreq(float _freq, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = _freq; 
    }
    updateOffsets(); 
}

void SineFold::setFreqMidi(float _note, int _voice){
    if (_voice >= numVoices){
        std::cout << "voice number '" << _voice << "' out of range" << std::endl; 
    }
    else {
        frequency[_voice] = midiToFreq(_note); 
    }
    updateOffsets(); 
}


SineFold::~SineFold(){
	delete[] offset; 
	delete[] incr; 
    delete[] frequency; 
    delete[] amp; 
}

float SineFold::midiToFreq(int _note){
	return 440.f * exp((log(2) * (_note - 69))/12); 
}

float SineFold::fold(float _input){
    float output; 

    output = _input * foldAmt; 

    bool goodToGo = false; 
    while (!goodToGo){
        if (output > 1){ output = 2 - output; } 
        else if (output < -1){ output = -2 - output; } 
        else { goodToGo = true; }
    }
    return output; 
}