#include<H4.h>

H4 h4(115200); // Automatically starts Serial for you if speed provided

#define LED1  LED_GREEN // these values will depend on your specific board
#define LED2  LED_RED // Do not use the same values without knowing why!
#define LED3  LED_BLUE

void myCallback(uint8_t pin){
    digitalWrite(pin, !digitalRead(pin)); // invert pin state
}

void h4setup(){ // do the same type of thing as the standard setup
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);

    // using "Lambda" functions to pass value of pin to myCallback when the event occurs
    h4.every(1000,[](){ myCallback(LED1); }); // All times are milliseconds, 1000=1 second
    h4.every(2000,[](){ myCallback(LED2); }); // All times are milliseconds, 1000=1 second
    h4.every(3000,[](){ myCallback(LED3); }); // All times are milliseconds, 1000=1 second
}