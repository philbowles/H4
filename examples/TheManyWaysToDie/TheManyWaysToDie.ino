/*
Creative Commons: Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode

You are free to:

Share — copy and redistribute the material in any medium or format
Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms. Under the following terms:

Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions 
under the same license as the original.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others 
from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use is 
permitted by an applicable exception or limitation. To discuss an exception, contact the author:

philbowles2012@gmail.com

No warranties are given. The license may not give you all of the permissions necessary for your intended use. 
For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
*/
#include <H4.h>
H4 h4(115200);
/*
 * Demonstrates the four methods of terminating a task prematurely
 * 
 * 1) Finish: - finishEarly: allow current iteration to complete, them call chain / onComplete function
 * 2) Conditional: finishIf: behave as 1) (clean finish) but only if condition X is true
 * 3) Unconditional: finishNow:  jump straight to chain / onComplete function without allowing current iteration to complete
 * 4) Kill: cancel: just kill it. instantly chop,delete,zap etc
 * 
 */
H4_TIMER f,c,u,k;

uint32_t  rvf=0;
uint32_t  rvu=0;
bool      rvc=false;

void theManyWaysToDie(){
  Serial.println("Choose your weapon");
  rvf=h4.finishEarly(f);  
  rvu=h4.finishNow(u);  
  rvc=h4.finishIf(c,[](H4_TASK_PTR p){ return p->nrq > 15; });

  Serial.print("C will");Serial.print(rvc ? "":" NOT");Serial.println(" finish");
  h4.cancel(k);  
}

void h4setup(){
  Serial.println("\nH4 the many ways to die...");

  f=h4.every(1000,
    [](){ Serial.print("F on iteration #");Serial.println(1+MY(nrq)); },
    [](){ Serial.print("F ran ");Serial.print(rvf);Serial.println(" times"); }
  );
  
  c=h4.every(1000,
    [](){ Serial.print("C on iteration #");Serial.println(1+MY(nrq)); },
    [](){ Serial.println("C (if asked) will only finish after 15 iterations"); }
  );   
  
  u=h4.every(1000,
    [](){ Serial.print("U on iteration #");Serial.println(1+MY(nrq)); },
    [](){ Serial.print("U ran ");Serial.print(rvu);Serial.println(" times"); }
  );    

  k=h4.every(1000,
    [](){ Serial.print("K on iteration #");Serial.println(1+MY(nrq)); },
    [](){ Serial.println("K never got the chance to say goodbye..."); }
  );

  h4.nTimes(2,10000,theManyWaysToDie);
  // 1st time: (after 20 secs)
  // F will run 10 times and send message via its final function
  // C will keep on running as the count has not yet exceeded 15
  // U will run 9 times and send message via its final function
  // K will silently disappear after 9 iterations
  // 
  // 2nd time: (after 20 secs)
  // C will now stop running as the count has exceeded 15
  //
  // no harm will be cause by calling any of the methods again
  // but this time on an invalid H4_TIMER
  //
}
/*

Sample Output:

H4 the many ways to die...
F on iteration #1
C on iteration #1
U on iteration #1
K on iteration #1
F on iteration #2
C on iteration #2
U on iteration #2
K on iteration #2
F on iteration #3
C on iteration #3
U on iteration #3
K on iteration #3
F on iteration #4
C on iteration #4
U on iteration #4
K on iteration #4
F on iteration #5
C on iteration #5
U on iteration #5
K on iteration #5
F on iteration #6
C on iteration #6
U on iteration #6
K on iteration #6
F on iteration #7
C on iteration #7
U on iteration #7
K on iteration #7
F on iteration #8
C on iteration #8
U on iteration #8
K on iteration #8
F on iteration #9
C on iteration #9
U on iteration #9
K on iteration #9
Choose your weapon
C will NOT finish
F on iteration #10
F ran 10 times
U ran 9 times
C on iteration #10
C on iteration #11
C on iteration #12
C on iteration #13
C on iteration #14
C on iteration #15
C on iteration #16
C on iteration #17
C on iteration #18
C on iteration #19
Choose your weapon
C will finish
C on iteration #20
C (if asked) will only finish after 15 iterations
*/