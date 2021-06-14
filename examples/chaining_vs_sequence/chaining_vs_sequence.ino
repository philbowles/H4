#include <H4.h>
H4  h4(115200);

H4_SEQ_LIST abc;

void h4setup() {
  H4_ADDSEQUENCE(abc, h4.nTimes(3,500,[]{ Serial.printf("%c ",0x41 + MY(nrq)); }); );
  H4_ADDSEQUENCE(abc,
      h4.once(1000,[]{
        Serial.print("\nIt's easy as:\n");
        h4.nTimes(3,500,[](){ Serial.printf("%d ",MY(nrq)+1); });
      });
  );
  H4_ADDSEQUENCE(abc, 
      h4.once(1500,[]{
        Serial.print("\n...That's how easy it can be!\n");
      });
  );
  Serial.printf("The Jackson 5 sang:\n");
  h4.sequence(abc,[]{ Serial.println("Have a listen to the real thing: https://www.youtube.com/watch?v=ho7796-au8U"); });
}
