#include<H4.h>
H4 h4(115200);

std::vector<std::string> stumble={"one","two","three","four","five"};
std::vector<uint32_t> bounce={5000,500,2500};

void h4setup(){
  h4.worker<std::vector<std::string>>(stumble,[](std::string s){
    Serial.printf("STUMBLE T=%d iter %d=%s\n",millis(),MY(nrq),s.data());
  });
  h4.worker<std::vector<uint32_t>>(bounce,[](uint32_t u){
    Serial.printf("BOUNCE  T=%d iter %d=%d\n",millis(),MY(nrq),u);
  });
  h4.nTimes(15,100,[]{ Serial.printf("T=%lu Interleaved TICK\n",millis()); });
  Serial.printf("\nT=%d Start your engines\n",millis());
}
