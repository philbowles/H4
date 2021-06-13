#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) 

H4_SEQ_LIST fnList;

void h4setup() {
  H4_ADDSEQUENCE(fnList, h4.nTimes(5,1000,[]{ Serial.printf("0x%08x T=%d nTimes %d\n",ME,millis(),MY(nrq)); }); );
  H4_ADDSEQUENCE(fnList, h4.queueFunction([]{ Serial.printf("0x%08x This will run ONCE\n",ME); },[]{ Serial.printf("0x%08x and then finish\n",ME); }); );
  H4_ADDSEQUENCE(fnList, h4.randomTimesRandom(4,7,1000,3000,[](){ Serial.printf("0x%08x T=%d RTR n=%d\n",millis(),ME,MY(nrq)); }); );

  h4.sequence(fnList,[]{ Serial.printf("'TIME FOR BED', SAID ZEBEDEE\n"); },H4P_TRID_BTTO);
}
