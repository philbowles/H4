#include <H4.h>
H4 h4(115200); // setes Serial to 115200, default to 20 tasks
/*

Optional: Allows you to "tag" your tasks with a name* so that you can find them
    easily in the diagnostic dump

    *keep it to 4 chs or the columns won't line up

*/
const char* giveTaskName(uint32_t n){
  static H4_INT_MAP mydata={
    {1,"Tick"},
    {4,"Rude"},
    {13,"Link"}, // don't have to be in order (but why not?)
    {6,"1Tim"}, // don't have to be used (but why not?)
    {49,"10GB"} // Keep them below 50 - IDs 50-99 are used by the system
    };
  return mydata.count(n) ? mydata[n].c_str():"ANON";
}
// if you don't want task naming, just delete the above

void h4setup() {
    Serial.begin(115200);
    h4.everyRandom(5000,10000,[](){ 
        Serial.print(millis());Serial.println(" RUDE INTERRUPTION");
        h4.dumpQ();
    },nullptr,4);

    h4.hookReboot([](){ 
        Serial.println("USER-DEFINED REBOOT HOOK"); // will run just before reboot
    });

    h4.every(1000,[]{ Serial.print(millis());Serial.println(" PING "); },nullptr,1);  
  
    h4.once(30000,
        []{ Serial.print(millis());Serial.println(" ONE TIME ONLY!!!"); },
        [](){ // chain fn - fires immediately aftery primary completes
        Serial.println("Chained from 1Tim and fire off new task");
        h4.nTimes(11,5000,[](){ // the song is "10 green bottles", but we want to include zero
            uint32_t nBottles=11 - (h4.context->nrq+1);
            Serial.print(nBottles);Serial.println(" green bottles, hangin' on a wall...");
        },nullptr,99);
        },
    13);

    h4.once(60000,h4reboot);    // reboot after 1 minute
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

void onReboot(){
    Serial.println("This always gets called without you needing to 'hook' it");
}
/*

Sample output

52114 PING 
53115 PING 
54116 PING 
6 green bottles, hangin' on a wall...
55117 PING 
56118 PING 
57119 PING 
58120 PING 
58900 RUDE INTERRUPTION
Due @tick UID        Type                     Min       Max       nRQ
000058899 0x3ffefb3c 0404   (evrn/Rude)      5000     10000         7
000059120 0x3ffefbe4 0301   (evry/Tick)      1000         0        58
000060062 0x3ffefccc 0700   (once/ANON)     60000         0         0
000060069 0x3ffefc54 0599   (ntim/CHNK)      5000         0         5
59121 PING 
5 green bottles, hangin' on a wall...
60122 PING 
This always gets called without you needing to 'hook' it
USER-DEFINED REBOOT HOOK

 ets Jan  8 2013,rst cause:2, boot mode:(3,6)

load 0x4010f000, len 1392, room 16 
tail 0
chksum 0xd0
csum 0xd0
v3d128e5c
~ld
 ⸮1063 PING 
2064 PING 
3065 PING 
4066 PING 
5067 PING 
5996 RUDE INTERRUPTION
Due @tick UID        Type                     Min       Max       nRQ
000005995 0x3ffefb3c 0404   (evrn/Rude)      5000     10000         0
000006067 0x3ffefbe4 0301   (evry/Tick)      1000         0         5
000030062 0x3ffefc5c 0713   (once/Link)     30000         0         0
000060062 0x3ffefccc 0700   (once/ANON)     60000         0         0
6068 PING 

*/