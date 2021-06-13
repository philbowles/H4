#include<H4.h>

H4 h4(115200); // Automatically starts Serial for you if speed provided

class simple{
  public:
    void says(const char* x){
        Serial.print("T=");Serial.print(millis());
        Serial.print(" Simple Simon says: ");Serial.println(x);  
    }
};

simple simpleSimon;

void bareFunction(){ simpleSimon.says("Bare your soul"); }

void h4setup() {

  h4.queueFunction(std::bind(&simple::says,simpleSimon,"\nThis will run ONCE"));       // will run once with no delay as soon as you exit setup
  
  // USING BARE FUNCTION
  h4.nTimes(5,3000,bareFunction);

  // USING STD::BIND
  h4.everyRandom(10000,30000,
    std::bind(&simple::says,simpleSimon,"It CAN be done often too!")                 // and (rather annoyingly) again every 10 - 30 seconds
    );
  // USING LAMBDA     
  h4.randomTimesRandom(5,10,5000,15000,
    [](){ simpleSimon.says("It's never a bind!"); }  
    );
}