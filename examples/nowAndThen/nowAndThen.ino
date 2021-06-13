#include<H4.h>
H4 h4(115200);

std::vector<uint32_t> stutter={6,7,4,3};
int32_t T;

void h4setup(){
    Serial.printf("Intervals: 0 ");
    for(auto const& s:stutter) Serial.printf("%d ",s);
    Serial.printf("\nT=%u\n",T=millis());
    h4.nowAndThen(stutter,[&]{
      uint32_t delta=millis()-T;
      T=millis();
      Serial.printf("+%u %u iter=%u DO STUFF\n",MY(rmin),delta,MY(nrq));
    });
}