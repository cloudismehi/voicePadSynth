#include <iostream>
#include <functional>
#include <vector> 
#include <unordered_map>

class Osc{
	public: 
		int damping = 1; 
		int freq = 200; 

		void changeDamping(int newVal){ damping = newVal; }
		void changeFreq(int newVal){ freq = newVal; }
		void print(){
			std::cout << "freq: " << freq << " damping " << damping << '\n'; 
		}
}; 

class Synth{
  public: 
    int dist = 0; 

    void changeDist(int _newDist){ dist = _newDist; }
    void print(){ std::cout << "dist= " << dist << '\n'; }
};

class Event{
  public:
    std::unordered_map<std::string, int> glossary; 
    std::vector<std::function<void(int _newVal)> > possibleEvents;
    template<class Obj, class ValueType>
      void addPossibleEvent(Obj& obj, void (Obj::*setter)(ValueType), ValueType newVal, std::string id){
        possibleEvents.push_back([&obj, setter](ValueType _newVal){ (obj.*setter)(_newVal); }); 
        glossary[id] = possibleEvents.size() - 1;  
      }

    std::vector<std::function<void(int _newVal)> > queue; 
    std::vector<int> queueData;

    void addToQueue(std::string id, int _newVal){
      queue.push_back(possibleEvents[glossary.at(id)]); 
      queueData.push_back(_newVal); 
    }
    void triggerEvent(){
      if (queue.size() == 0) printf("nothing on queue\n"); 
      else {
        queue.back()(queueData.back()); 
        queue.pop_back(); 
        queueData.pop_back(); 
      }
    }
}; 



std::vector<std::function<void(int _newVal)> > queue; 
std::vector<int> queueData; 


int main(){
	Osc sampleOsc;
  Synth sampleSynth; 
  Event events; 

  events.addPossibleEvent(sampleOsc, &Osc::changeFreq, 2, "freq"); 
  events.addPossibleEvent(sampleOsc, &Osc::changeDamping, 2, "damping"); 
  events.addPossibleEvent(sampleSynth, &Synth::changeDist, 3, "dist"); 

  events.addToQueue("freq", 100); 
  events.addToQueue("dist", 30); 

  events.triggerEvent();
  sampleSynth.print(); 
  sampleOsc.print(); 

  events.triggerEvent(); 
  sampleSynth.print(); 
  sampleOsc.print(); 

	return 0; 
}

