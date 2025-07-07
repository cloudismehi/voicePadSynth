#include <iostream> 
#include <fstream> 
#include <string> 
#include <sstream> 
#include <filesystem>
#include <vector>

namespace fs = std::filesystem; 

class EventInfo{
    public: 

    std::string id; 
    std::vector<std::string> filenames; 

    float cur, newVal, time; 
    int voice; 

    bool loadData(std::string _filename); 
    bool storeData(std::string _filename); 
    void printReport(); 
    bool getFilenames(); 
}; 

int main(){
    EventInfo event; 

    // if (!event.loadData("testData.dat")) return 1; 
    event.getFilenames(); 
    for (auto e : event.filenames){
        std::cout << e << '\n'; 
    }

    return 0; 
}

bool EventInfo::loadData(std::string _filename){
    std::fstream file; 
    file.open(_filename, std::ios::in); 
    if (!file) {
        std::cout << "error opening file\n"; 
        return false; 
    }
    
    std::string inputString; 
    int _index = 0; 
    while (std::getline(file, inputString)){
        if (inputString == "new") {
            printf("new event (%d)\n", _index); 
            _index++; 
        }
        else {
            std::stringstream stream(inputString); 
            std::string element; 

            int _subindex = 0; 
            while (getline(stream, element, '/')){
                switch(_subindex){
                    case 0: 
                        id = element; 
                        break; 
                    case 1: 
                        cur = std::stof(element); 
                        break; 
                    case 2: 
                        newVal = std::stof(element); 
                        break; 
                    case 3: 
                        time = std::stof(element); 
                        break; 
                    case 4: 
                        voice = std::stoi(element); 
                        break; 
                }
                _subindex++; 
            }
            printReport(); 
        }
    }
    file.close(); 
    return true; 
}

bool EventInfo::storeData(std::string _filename){
    std::fstream file; 
    file.open(_filename, std::ios::out); 
    if (!file){
        std::cout << "error creating file\n"; 
        return false; 
    }
    
    //new event
    file << "new" << '\n'; 

    //add to recently opened event 
    //  id/cur/new/time/voice 
    file << "sineOsc_freq" << '/' << "0" << '/' << "1" << '/' << "0.5" << '/' << "0" << '\n'; 
    file << "sineOsc_freq" << '/' << "1" << '/' << "4" << '/' << "0.5" << '/' << "1" << '\n'; 

    //new event 
    file << "new" << '\n'; 
    //add to recently opened event
    //  id/cur/new/time/voice 
    file << "sineOsc_freq" << '/' << "2" << '/' << "5" << '/' << "0.5" << '/' << "0" << '\n'; 
    file << "sineOsc_freq" << '/' << "4" << '/' << "6" << '/' << "0.5" << '/' << "1" << '\n'; 
    

    file.close(); 
    return true; 
}

void EventInfo::printReport(){
    printf("id: %s\nold val: %0.2f\nnew val: %0.2f\ntime: %0.2f\nvoice %d\n\n", id.c_str(), cur, newVal, time, voice); 
}

bool EventInfo::getFilenames(){
    try {
        for (const auto& entry : fs::directory_iterator(".")){
            std::string name = entry.path().filename().string(); 
            if (fs::is_regular_file(entry.status())){
                if (name.substr(name.length() - 4, 4) == ".dat"){
                    filenames.push_back(entry.path().filename().string()); 
                }
            }
        }
    } catch (const fs::filesystem_error& e){
        std::cerr << "filesystem error: " << e.what() << std::endl; 
        return false; 
    }
    printf("got all available filenames!\n"); 
    return true; 
}