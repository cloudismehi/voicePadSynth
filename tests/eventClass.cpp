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

    std::vector<std::function<void(float _newVal, int _voice)> > queue; 
    std::vector<std::pair<float, int> > queueData;

    template<class Obj, class ValueType>
      void addPossibleEvent(Obj& obj, void (Obj::*setter)(ValueType, int), ValueType newVal, std::string id){
        possibleEvents.push_back([&obj, setter](ValueType _newVal, int _voice){ (obj.*setter)(_newVal, _voice); }); 
        glossary[id] = possibleEvents.size() - 1;  
      }

    void addToQueue(std::string id, float _newVal, int _voice){
      queue.push_back(possibleEvents[glossary.at(id)]); 
      queueData.push_back(std::make_pair(_newVal, _voice)); 
    }

    void triggerEvent(){
      if (queue.size() == 0) printf("nothing on queue\n"); 
      else {
        queue.back()(queueData.back().first, queueData.back().second); 
        queue.pop_back(); 
        queueData.pop_back(); 
      }
    }

}; 



std::vector<std::function<void(int _newVal)> > queue; 
std::vector<int> queueData; 


int main(){
	Osc sampleOsc;
  Event events; 

  sampleOsc.print(); 
  events.addPossibleEvent(sampleOsc, &Osc::changeAmplitude, 2.f, "amp"); 
  events.addToQueue("amp", 0.5, 0); 
  events.triggerEvent(); 

  sampleOsc.print(); 

	return 0; 
}

