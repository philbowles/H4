/*
Creative Commons: Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode

You are free to:

Share — copy and redistribute the material in any medium or format
Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms. Under the following terms:

Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions 
under the same license as the original.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others 
from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use is 
permitted by an applicable exception or limitation. To discuss an exception, contact the author:

philbowles2012@gmail.com

No warranties are given. The license may not give you all of the permissions necessary for your intended use. 
For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
*/
#include <H4.h>
H4 h4(115200);
//
//  Demonstrates the h4Chunker template function
//
//  calls your function with an iterator to the "next" item in a data structure
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
  h4Chunker<vector<string>>(sharks1,chunkShark);
  h4Chunker<vector<string>>(sharks2,chunkShark);
  h4Chunker<vector<string>>(sharks3,chunkShark);
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