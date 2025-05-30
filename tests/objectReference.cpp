#include <iostream> 

class Type{
  public:
  int foo = 0; 
};

class Voice{
  public: 

  int voice = 10; 
}; 

class RefClass{
  public: 

  int *ref; 
  Type *refType; 

  RefClass(int &voice){
    ref = &voice; 
  }
  RefClass(Type &_ref){
    refType = &_ref; 
    (*refType).foo = 19; 
  }
  void randomWork(){
    *ref = 90; 
    
  }
}; 

int main(){
  Voice voice; 
  Type foo; 

  RefClass ref(foo); 
  ref.randomWork(); 

  std::cout << foo.foo << '\n'; 
}