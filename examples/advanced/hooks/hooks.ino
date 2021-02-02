#include <H4.h>
H4 h4(115200); // sets Serial to 115200, default to 20 tasks

void h4setup() {
    h4.everyRandom(5000,10000,[](){ 
        Serial.print(millis());Serial.println(" RUDE INTERRUPTION");
    },nullptr,4);

    h4.hookReboot([](){ Serial.println("USER-DEFINED REBOOT HOOK 1 "); });
    h4.hookReboot([](){ Serial.println("USER-DEFINED REBOOT HOOK 2 "); });
    h4.hookReboot([](){ Serial.println("USER-DEFINED REBOOT HOOK 3"); });

    h4.every(1000,[]{ Serial.print(millis());Serial.println(" PING "); },nullptr,1);  
  
    h4.once(30000,
        []{ Serial.print(millis());Serial.println(" ONE TIME ONLY!!!"); },
        [](){ // chain fn - fires immediately aftery primary completes
        Serial.println("Chained from 1Tim and fire off new task");
        h4.nTimes(11,5000,[](){ // the song is "10 green bottles", but we want to include zero
            uint32_t nBottles=11 - (h4.context->nrq+1);
            Serial.print(nBottles);Serial.println(" green bottles, hangin' on a wall...");
        });
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
void h4UserLoop(){
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
    Serial.println("This always gets called LAST and without you needing to 'hook' it");
}
