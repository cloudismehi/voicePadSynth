#include <iostream> 
#include <cmath>
#include <vector>
#include <unordered_map>
#include <functional>
#include "portaudio.h"

class LinearEnvelope{
  public: 
  float slope, offset = 0; 
  int index = 0, sampleRate = 44100;
  float max = 1, min = 0; 
  float initValue, endValue; 

  bool envDone = false, set = false; 

  void setSlope(float _initValue, float _endValue, float _time){
    slope = (_endValue - _initValue) / (_time * sampleRate); 
    initValue = _initValue; 
    endValue = _endValue; 
    set = true; 
  }
  void setOffset(float _newOffset) { offset = _newOffset; }
  void resetEnvelope(){ index = 0; }
  void setMinMax(float _min, float _max){ min = _min ; max = _max;  }
  
  float nextValue(){
    float out = (set) ? ((slope * (index++)) + offset) : 1; 
    // std::cout << out; 
    if (out > max) out = max; 
    if (out < min) out = min; 
    if ((slope < 0) && (out < endValue)) envDone = true; 
    if ((slope > 0) && (out > endValue)) envDone = true; 

    return out; 
  }

}; 

class Event{
public:
  int sampleRate = 44100; 
  std::unordered_map<std::string, int> glossary; 
  std::vector<std::function<void(float _newVal, int _voice)> > possibleEvents;

  struct IndividualEvents{
    std::vector<std::function<void(float _newVal, int _voice)> > queue; 
    std::vector<std::pair<float, int> > queueData;
    std::vector<std::string> commandNames; 
    std::vector<float> slopes; 
  };

  std::vector<IndividualEvents> events; 
  int openedEvent = -1; 

  int newEvent();
  void openEvent(int _eventIndex){ openedEvent = _eventIndex; }
  void closeEvent(){ openedEvent = -1; }
  void listEvents();

  void addToEvent(int _eventIndex, std::string _id, float _newVal, int _voice);
  void addToEvent(std::string _id, float _newVal, int _voice);
  void addToEvent(std::string _id, float _newVal, int _voice, float _curValue, float _time);
  
  void deployEvent(int _eventIndex);
  void deployEvent();
  float deployEventEnv(); 
  void removeEvent(); 

  template<class Obj>
    void addPossibleEvent(Obj& obj, void (Obj::*setter)(float, int), std::string id){
      possibleEvents.push_back([&obj, setter](float _newVal, int _voice){ (obj.*setter)(_newVal, _voice); }); 
      glossary[id] = possibleEvents.size() - 1;  
    }

}; 

class UserData{
public: 
  float freq = 200; 

  float incr = freq/44100; 
  float offset = 0; 
  float amp = 1; 

  float genValue(){
    float out = 0.f; 
    out += sinf(2 * M_PI * offset); 
    if ((offset += incr) >= 1.f) offset = 0.f; 
    return out; 
  }
  void changeFreq(float _newFreq, int foo){
    freq = _newFreq; 
    incr = freq/44100; 
  }
}; 

int checkPaError(PaError _err){
  if (_err != paNoError){
    std::cout << Pa_GetErrorText(_err); 
    return 1; 
  }
  return 0; 
}

int callback(const void* input, void *output, unsigned long frameCount, 
  const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* data); 

int main(){
  Event events; 
  
  PaError err; 
  PaStream* stream; 
  UserData user; 

  events.addPossibleEvent(user, &UserData::changeFreq, "freq"); 
  events.newEvent(); 
  events.addToEvent(0, "freq", 300, 0); 

  err = Pa_Initialize(); 
  checkPaError(err); 
  err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, 44100, 256, callback, &user); 
  checkPaError(err); 
  err = Pa_StartStream(stream); 
  checkPaError(err); 

  Pa_Sleep(1000); 
  events.deployEvent(); 
  Pa_Sleep(1000); 

  err = Pa_StopStream(stream); 
  checkPaError(err); 
  err = Pa_CloseStream(stream); 
  checkPaError(err); 
  err = Pa_Terminate(); 
  checkPaError(err); 

  return 0; 
}

int callback(const void* input, void *output, unsigned long frameCount, 
  const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* data)
{
  (void) input; 
  float* outputBuffer = (float*) output; 
  UserData* userInput = (UserData*) data; 

  for (int i = 0; i < frameCount; i++){
    float out = userInput->genValue(); 
    *outputBuffer++ = out; 
  }
  return 0; 
}

int Event::newEvent(){
  events.resize(events.size() + 1);
  return events.size() - 1; 
}

void Event::listEvents(){
  int eventIndex = 0; 
  for (auto event : events){
    std::cout << "event " << eventIndex << ":\n"; 
    for (int i = 0; i < event.queue.size(); i++){
      std::cout << " " << event.commandNames[i] << " to " << event.queueData[i].first << " on voice " << event.queueData[i].second << '\n';
    }
    eventIndex++; 
  }
  std::cout << '\n'; 
}

void Event::addToEvent(int _eventIndex, std::string _id, float _newVal, int _voice){
  events[_eventIndex].queue.push_back(possibleEvents[glossary.at(_id)]); 
  events[_eventIndex].queueData.push_back(std::make_pair(_newVal, _voice));
  events[_eventIndex].commandNames.push_back(_id); 

}

void Event::addToEvent(std::string _id, float _newVal, int _voice){
  if (openedEvent == -1) {
    std::cout << "no event opened\n"; 
    return; 
  }
  events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
  events[openedEvent].queueData.push_back(std::make_pair(_newVal, _voice));
  events[openedEvent].commandNames.push_back(_id); 
}

void Event::addToEvent(std::string _id, float _newVal, int _voice, float _curValue, float _time){
  if (openedEvent == -1) {
    std::cout << "no event opened\n"; 
    return; 
  }
  events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
  events[openedEvent].queueData.push_back(std::make_pair(_newVal, _voice));
  events[openedEvent].commandNames.push_back(_id); 
  events[openedEvent].slopes.push_back((_newVal - _curValue)/(_time * sampleRate)); 
}


void Event::deployEvent(int _eventIndex){
  int commandNumber = events[_eventIndex].queue.size(); 
  for (int i = 0; i < commandNumber; i++){
    events[_eventIndex].queue.back()(events[_eventIndex].queueData.back().first, events[_eventIndex].queueData.back().second);
    events[_eventIndex].queue.pop_back(); 
    events[_eventIndex].queueData.pop_back(); 
    events[_eventIndex].commandNames.pop_back();
  }
  events.erase(events.begin() + _eventIndex);
}

void Event::deployEvent(){
  int commandNumber = events.front().queue.size(); 
  for (int i = 0; i < commandNumber; i++){
    events.front().queue.back()(events.front().queueData.back().first, events.front().queueData.back().second); 
    events.front().queue.erase(events.front().queue.begin()); 
    events.front().queueData.erase(events.front().queueData.begin()); 
    events.front().commandNames.erase(events.front().commandNames.begin());
  }
  events.erase(events.begin()); 
}