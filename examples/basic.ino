/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <H4.h>
H4 h4; // default to 20 tasks

/*

Optional: Allows you to "tag" your tasks with a name* so that you can find them
    easily in the diagnostic dump

    *keep it to 4 chs or the columns won't line up

*/
const char* getTaskName(uint32_t n){
  static std::map<uint32_t,string> mydata={
    {1,"Tick"},
    {4,"Rude"},
    {13,"Link"},
    {66,"1Tim"},
    {99,"10GB"}
    };
  return mydata.count(n) ? mydata[n].c_str():"NONE";
}
// if you don't want task naming, just delete the above

void setup() {
  Serial.begin(115200);
    h4.everyRandom(5000,10000,[](){ 
    Serial.print(millis());Serial.println(" RUDE INTERRUPTION");
    h4.dumpQ();
  },nullptr,4);

  h4.every(1000,[]{ Serial.print(millis());Serial.println(" PING "); },nullptr,1);  
  
  h4.once(30000,
    []{ Serial.print(millis());Serial.println(" ONE TIME ONLY!!!"); },
    [](){ // chain fn - fires immediately aftery primary completes
      Serial.println("Chained from 1Tim and fire off new task");
      h4.nTimes(11,5000,[](){ // the song is "10 green bottles", but we want to include zero
        uint32_t nBottles=11 - (h4.context->nrq+1);
        Serial.print(nBottles);Serial.println(" green bottles, hangin' on a wall...");
      },nullptr,99);
    },13);
}
/*
    Optional:

        Used ONLY for other libraries that have "loop" / "handle" / "keepalive" functions
        that must be called regularly.

        If you dont have any, delete the whole function
*/
void userLoop(){
  static long prev=0;
  if(millis() - prev > 5000) {
      // e.g. http.handleClient();
      // e.g. mqtt.loop();
      // etc
    Serial.print("USER LOOP\n");
    prev=millis();
  }
}