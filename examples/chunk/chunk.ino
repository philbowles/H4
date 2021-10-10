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
//  Demonstrates the important technique of "worker threads"
//
//  Shows the use of repeatWhile to "chunk up" a task, i.e. to run it incrementally (a "chunk" at a time)
//  in the background without negatively affecting other tasks. Often called a "worker" task or thread
//
//  five vectors are processed simultaneously by five parallel tasks, all "interleaving" with each other
//  The "sharks" tasks also show the difference betwen the use of "cancel" and "finishEarly" to end a task
//  (introduced in "TheManyWaysToDie" example)
//
//   *    NB THERE ARE MUCH BETTER WAYS TO DO MUCH OF THIS - THIS IS   *D E M O*   CODE!!
//   *    ===============================================================================
//
//   See the "chunkier" example for a much easier faster and more elegant way to do the same thing
//
vector<string> monkeys={"macacque","spider","mandrill","capuchin","colobus","howler","rhesus","squirrel"};
vector<string> apes={"chimp","bonobo","gorilla","my neighbour"};
vector<string> sharks3={"Cookie Cutter","Port Jackson","Dogfish"};// all harmless
vector<string> sharks2={"Oceanic White Tip","ExitWater","Bamboo Shark","Nurse Shark","Angel Shark"}; // mostly harmless
// cast of killers + a few harmless
vector<string> sharks1={"Bronze Whaler","Tiger","Bull","Great White","StayIn","Leopard Shark","Whale Shark","Basking Shark"};

void workerThread1(){
  Serial.print("T=");Serial.print(millis());Serial.print(" Type of Monkey: ");Serial.println(CSTR(monkeys.back()));
  monkeys.pop_back();
}

void workerThread2(){
  static int i=0;
  Serial.print("T=");Serial.print(millis());Serial.print(" Type of Ape: ");Serial.println(CSTR(apes.back()));
  apes.pop_back();
}

void refSharks(vector<string>& r){
  h4.repeatWhile(
    bind([](vector<string>& r){ return r.size(); },ref(r)),
    1000,
    bind([](vector<string>& r){ 
      string shark=r.back();  
      if(shark=="ExitWater") h4.cancel();  // exits immediately you don't get eaten 
      else {
        if(shark=="StayIn") h4.finishEarly();  // jumps to end, you get eaten
        else {
          Serial.print(CSTR(shark));Serial.println(" is harmless");
        }
      }
      r.pop_back();            
    },ref(r)),
    bind([](vector<string>& r){ 
        if(r.size()) { Serial.print("You got nibbled by a ");Serial.println(CSTR(r.back())); }
      },ref(r))
  );  
}

void h4setup(){
  h4.repeatWhile([](){ return monkeys.size(); },1000,workerThread1); // always once!
  h4.repeatWhile([](){ return apes.size(); },1000,workerThread2,[](){ Serial.println("cut off in my prime...ate"); }); // always once!
  refSharks(sharks3);
  refSharks(sharks2);
  refSharks(sharks1);
}
/*
 Sample Output:

T=1058 Type of Monkey: squirrel
Dogfish is harmless
Basking Shark is harmless
T=1058 Type of ape No. 1: my neighbour
Angel Shark is harmless
T=2059 Type of Monkey: rhesus
T=2059 Type of ape No. 2: gorilla
Port Jackson is harmless
Whale Shark is harmless
Nurse Shark is harmless
Cookie Cutter is harmless
Leopard Shark is harmless
T=3060 Type of Monkey: howler
T=3060 Type of ape No. 3: bonobo
Bamboo Shark is harmless
T=4061 Type of Monkey: colobus
T=4061 Type of ape No. 4: chimp
You got nibbled by a Great White
cut off in my prime...ate
T=5062 Type of Monkey: capuchin
T=6063 Type of Monkey: mandrill
T=7064 Type of Monkey: spider
T=8065 Type of Monkey: macacque

 */