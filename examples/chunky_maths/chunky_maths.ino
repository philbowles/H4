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
#include<H4.h>
H4 h4(115200);

#define CALIBRATION_STOP_GT   500
#define MAX_ATTEMPTS          10
/*
 * Controlling function decrements MAX_ATTEMPTS directly and thus
 * exits automatically when counted down to zero and does NOT save partials globally
 * 
 * "Calibration" function saves partials to global copy with "good" results
 * and finishes early, thus "chain" function ("onFinish" if you like)
 * can easily tell if calibration succeeded
 */
struct S {
  int   n;
  int   sigma;
  int   avg;  
} mydata = {MAX_ATTEMPTS,0,0};


void h4setup(){
    Serial.printf("Calibration will stop when avg > %d (or %d attempts)\n",CALIBRATION_STOP_GT,MAX_ATTEMPTS);
    H4_TIMER p=h4.repeatWhile(
        []{
            struct S temp; // temp copy of struct
            ME->getPartial(&temp); // fill temp struct from partial
            int rv=--temp.n;
            ME->putPartial((void *)&temp); // save modified struct for next iteration
            return rv;
        },
//        task::randomRange(H4_JITTER_LO,H4_JITTER_HI), // arbitrary
        1000,
        [](){ 
            struct S temp; // temp copy of struct
            ME->getPartial(&temp); // fill temp struct from partial
            int inst=random(0,1000);  // do some operation(s)
            temp.sigma+=inst;
            temp.avg=temp.sigma / (MAX_ATTEMPTS+1 - temp.n); 
            Serial.printf("N=%d inst=%d sigma=%d avg=%d\n",temp.n,inst,temp.sigma,temp.avg);
            // OR return true in controlling function and h4.cancel() here
            // OR - whenever you want:
            if(temp.avg > CALIBRATION_STOP_GT) {
              memcpy(&mydata,&temp,sizeof(struct S)); // save it cos its just gonna get destroyed...
              h4.finishEarly(); 
            }
            ME->putPartial((void *)&temp); // save modified struct for next iteration
        },
        [](){
            if(mydata.avg) Serial.printf("Calibration Complete N=%d sigma=%d avg=%d\n",mydata.n,mydata.sigma,mydata.avg);
            else Serial.printf("Calibration failed after %d attempts\n",MAX_ATTEMPTS);
        });
    // reserve space for struct
    p->createPartial(&mydata, sizeof(struct S));
}
