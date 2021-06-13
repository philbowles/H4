#include <H4.h>
//
//  Demonstrates the important technique of "worker threads"
//
//  Shows the use of repeatWhile to "chunk up" a task, i.e. to run it incrementally (a "chunk" at a time)
//  in the background without negatively affecting other tasks. Often called a "worker" task or thread
//
//  five std::vectors are processed simultaneously by five parallel tasks, all "interleaving" with each other
//  The "sharks" tasks also show the difference betwen the use of "cancel" and "finishEarly" to end a task
//  (introduced in "TheManyWaysToDie" example)
//
//   *    NB THERE ARE MUCH BETTER WAYS TO DO MUCH OF THIS - THIS IS   *D E M O*   CODE!!
//   *    ===============================================================================
//
//   See the "chunkier" example for a much easier faster and more elegant way to do the same thing
//
H4 h4(115200);
//
std::vector<std::string> monkeys={"macacque","spider","mandrill","capuchin","colobus","howler","rhesus","squirrel"};
std::vector<std::string> apes={"chimp","bonobo","gorilla","my neighbour"};
std::vector<std::string> sharks3={"Cookie Cutter","Port Jackson","Dogfish"};// all harmless
std::vector<std::string> sharks2={"Oceanic White Tip","ExitWater","Bamboo Shark","Nurse Shark","Angel Shark"}; // mostly harmless
// cast of killers + a few harmless
std::vector<std::string> sharks1={"Bronze Whaler","Tiger","Bull","Great White","StayIn","Leopard Shark","Whale Shark","Basking Shark"};

void workerThread1(){
  Serial.print("T=");Serial.print(millis());Serial.print(" Type of Monkey: ");Serial.println(CSTR(monkeys.back()));
  monkeys.pop_back();
}

void workerThread2(){
  static int i=0;
  Serial.print("T=");Serial.print(millis());Serial.print(" Type of Ape: ");Serial.println(CSTR(apes.back()));
  apes.pop_back();
}

void refSharks(std::vector<std::string>& r){
  h4.repeatWhile(
    bind([](std::vector<std::string>& r){ return r.size(); },ref(r)),
    1000,
    bind([](std::vector<std::string>& r){ 
      std::string shark=r.back();  
      if(shark=="ExitWater") h4.cancel();  // exits immediately you don't get eaten 
      else {
        if(shark=="StayIn") h4.finishEarly();  // jumps to end, you get eaten
        else {
          Serial.print(CSTR(shark));Serial.println(" is harmless");
        }
      }
      r.pop_back();            
    },ref(r)),
    bind([](std::vector<std::string>& r){ 
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