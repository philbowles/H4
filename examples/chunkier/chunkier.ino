#include <H4.h>
//
//  Demonstrates the chunker template function
//
//  calls your function with an iterator to the "next" item in a data structure
//
H4 h4(115200);
//
vector<string> sharks3={"Cookie Cutter","Port Jackson","Dogfish"};// all harmless
vector<string> sharks2={"Bamboo Shark","Nurse Shark","Angel Shark","Get Out!","Oceanic White Tip"}; // mostly harmless
// cast of killers + a few harmless
vector<string> sharks1={"Leopard Shark","Whale Shark","Basking Shark","Stay in and die","Great White","Bronze Whaler","Tiger","Bull"};

void chunkShark(vector<string>::const_iterator i){
    string shark=(*i); 
    if(shark=="Stay in and die") {
      Serial.print("You are about to get eaten by a ");Serial.println(CSTR((*++i)));
      h4.cancel();
    }
    else {
      if(shark=="Get Out!") {
        Serial.print("You narrowly escaped death by a ");Serial.println(CSTR((*++i)));
        h4.cancel();
      }
      else {
        Serial.print(CSTR(shark));Serial.println(" is harmless");
      }
    }
}

void h4setup(){
  h4.nTimes(10,250,[](){ Serial.println("Tick"); });  
  chunker<vector<string>>(sharks1,chunkShark);
  chunker<vector<string>>(sharks2,chunkShark);
  chunker<vector<string>>(sharks3,chunkShark);
}
/*
 Sample Output:
Bamboo Shark is harmless
Leopard Shark is harmless
Tick
Cookie Cutter is harmless
Nurse Shark is harmless
Whale Shark is harmless
Tick
Port Jackson is harmless
Angel Shark is harmless
Basking Shark is harmless
Tick
You narrowly escaped death by a Oceanic White Tip
Dogfish is harmless
You are about to get eaten by a Great White
Tick
Tick
Tick
Tick
Tick
Tick
Tick

 */