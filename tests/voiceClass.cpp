#include <iostream> 
#include <functional> 


class Voice{
  public: 
    int foo = 0; 
    float genValue(){ 
      std::cout << "this is a life\n";
      foo++; 
      return 1; 
    }
}; 

void callback(void* data){
  std::function<float()>* func = (std::function<float()>*) data; 

  (*func)(); 
}

int main(){
  Voice voice; 
  auto setter = &Voice::genValue; 
  std::function<float()> func = [&voice, setter]() -> float { return (voice.*setter)();  }; 
  
  callback(&func);
  std::cout << voice.foo << '\n'; 

  return 0;
}
