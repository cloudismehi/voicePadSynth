#include <iostream>
#include <functional>
#include <vector> 
#include <unordered_map>
#include <utility> 

class Osc{
	public: 
    std::vector<float> freq; 
    std::vector<float> damping; 
    float amplitude = 1.f; 

		void changeDamping(float _newVal, int _voice){ 
      damping[_voice] = _newVal; 
    }
		void changeFreq(float _newVal, int _voice){ 
      freq[_voice] = _newVal; 
    }
    void changeAmplitude(float _newVal, int _voice = -1){
      amplitude = _newVal; 
    }
		void print(){
		  std::cout << "frequencies are: \n"; 
      for (auto val : freq) std::cout << val << ' '; 
      std::cout << '\n'; 
      
      std::cout << "dampings are: \n"; 
      for (auto val : damping) std::cout << val << ' '; 
      std::cout << '\n'; 
      
      std::cout << "amplitude is " << amplitude << '\n'; 
    }

    Osc(){
      freq.push_back(200.f); 
      freq.push_back(300.f);

      damping.push_back(10.f); 
      damping.push_back(15.f); 
    }
}; 

class Event{
  public:
    std::unordered_map<std::string, int> glossary; 
    std::vector<std::function<void(float _newVal, int _voice)> > possibleEvents;

    struct IndividualEvents{
      std::vector<std::function<void(float _newVal, int _voice)> > queue; 
      std::vector<std::pair<float, int> > queueData;
      std::vector<std::string> commandNames; 
    };

    std::vector<IndividualEvents> events; 
    int openedEvent = -1; 
  
    int newEvent(){
      events.resize(events.size() + 1);
      return events.size() - 1; 
    }

    void openEvent(int _eventIndex){ openedEvent = _eventIndex; }
    void closeEvent(){ openedEvent = -1; }

    void listEvents(){
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

    void addToEvent(int _eventIndex, std::string _id, float _newVal, int _voice){
      events[_eventIndex].queue.push_back(possibleEvents[glossary.at(_id)]); 
      events[_eventIndex].queueData.push_back(std::make_pair(_newVal, _voice));
      events[_eventIndex].commandNames.push_back(_id); 
    }

    void addToEvent(std::string _id, float _newVal, int _voice){
      if (openedEvent == -1) {
        std::cout << "no event opened\n"; 
        return; 
      }
      events[openedEvent].queue.push_back(possibleEvents[glossary.at(_id)]); 
      events[openedEvent].queueData.push_back(std::make_pair(_newVal, _voice));
      events[openedEvent].commandNames.push_back(_id); 
    }
    

    void deployEvent(int _eventIndex){
      int commandNumber = events[_eventIndex].queue.size(); 
      for (int i = 0; i < commandNumber; i++){
        events[_eventIndex].queue.back()(events[_eventIndex].queueData.back().first, events[_eventIndex].queueData.back().second);
        events[_eventIndex].queue.pop_back(); 
        events[_eventIndex].queueData.pop_back(); 
        events[_eventIndex].commandNames.pop_back();
      }
      events.erase(events.begin() + _eventIndex); 
    }
    void deployEvent(){
      int commandNumber = events.front().queue.size(); 
      for (int i = 0; i < commandNumber; i++){
        events.front().queue.back()(events.front().queueData.back().first, events.front().queueData.back().second); 
        events.front().queue.erase(events.front().queue.begin()); 
        events.front().queueData.erase(events.front().queueData.begin()); 
        events.front().commandNames.erase(events.front().commandNames.begin()); 
      }
      events.erase(events.begin()); 
    }

    template<class Obj, class ValueType>
      void addPossibleEvent(Obj& obj, void (Obj::*setter)(ValueType, int), ValueType newVal, std::string id){
        possibleEvents.push_back([&obj, setter](ValueType _newVal, int _voice){ (obj.*setter)(_newVal, _voice); }); 
        glossary[id] = possibleEvents.size() - 1;  
      }

 }; 

int main(){
	Osc sampleOsc;
  Event events; 

  sampleOsc.print();
  std::cout << '\n'; 

  events.addPossibleEvent(sampleOsc, &Osc::changeAmplitude, 2.f, "amp"); 
  events.addPossibleEvent(sampleOsc, &Osc::changeFreq, 2.f, "freq"); 

  std::cout << "new event "<< events.newEvent() << " was added!\n\n"; 
  
  events.openEvent(0); 
  events.addToEvent("freq", 500, 0); 
  events.closeEvent();

  std::cout << "new event "<< events.newEvent() << " was added!\n\n"; 
  events.addToEvent(1, "freq", 200, 1); 

  // events.listEvents(); 
  
  events.deployEvent();

  // events.listEvents();

  sampleOsc.print(); 

	return 0; 
}

