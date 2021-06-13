#include <H4.h>
//
//  Demonstrates the worker template function
//
//  calls your function with an iterator to the "next" item in a data structure
//
H4 h4(115200);
//
std::vector<std::string> sharks3={"Cookie Cutter","Port Jackson","Dogfish"};// all harmless
std::vector<std::string> sharks2={"Bamboo Shark","Nurse Shark","Angel Shark","Oceanic White Tip"}; // mostly harmless
// cast of killers + a few harmless
std::vector<std::string> sharks1={"Leopard Shark","Whale Shark","Basking Shark","Great White","Bronze Whaler","Tiger","Bull"};

void chunkShark(std::string s){
    if(s=="Great White") {
      Serial.printf("You are about to get eaten by the %s!\n",s.data());
      h4.cancel();
    }
    else {
      if(s=="Oceanic White Tip") {
        Serial.printf("You narrowly escaped death by the %s!\n",s.data());
        h4.cancel();
      }
      else {
        Serial.printf("%s is harmless\n",s.data());
      }
    }
}

void h4setup(){
  h4.nTimes(10,250,[](){ Serial.println("Tick"); });  
  h4.worker<std::vector<std::string>>(sharks1,chunkShark);
  h4.worker<std::vector<std::string>>(sharks2,chunkShark);
  h4.worker<std::vector<std::string>>(sharks3,chunkShark);
}