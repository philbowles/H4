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
H4  h4(115200);
/*
 *    Demonstrates H4's ability to "chain" timer calls. Chained-in functions can call further
 *    functions, leading to complex sequences
 *    
 *    A function is created that runs 3 times and then "chains" in another function
 *    The next function also runs 3 times and chains in yet another function
 *    this next function chain in a single final function 10 seconds after it ends
 *    
 *    The whole of the above is itself "wrapped" in a function that can then be started randomly
 *      
 *    
 *    NB THERE ARE MUCH BETTER WAYS TO DO MUCH OF THIS - THIS IS   *D E M O*   CODE!!
 *    ===============================================================================
 *    
 */

H4_FN_VOID  jackson5=[](){                          // h4_FN_VOID defines a function object we can use later
    Serial.print("T=");Serial.print(millis());
    Serial.println(" The Jackson 5 sang: ");
    
    h4.nTimes(3,1500,[]()
      {
      static char c=0x41;
      Serial.print(c++);       // run 3 times...A B C
      },
      []()                            // and then... chain function
        {
        Serial.print("\nIt's easy as: ");
        h4.nTimes(3,1500,[](){
          static int n=1;
          Serial.print(n++);     // 1 2 3
          },                          // and then... chain function
          []()
            {
            Serial.print("\n...That's how easy it can be!\n");
            h4.once(10000,[](){ Serial.println("have a listen to the real thing: https://www.youtube.com/watch?v=ho7796-au8U"); });
            } // end 123 chain function
        ); // end 123 function
        } // end abc chain function
      ); // end "ABC" function
}; // end fn declaration

void h4setup() {
  h4.onceRandom(15000,20000,jackson5);
}
/*
 * Sample output
 T=15988 The Jackson 5 sang: 
ABC
It's easy as: 123
...That's how easy it can be!
have a listen to the real thing: https://www.youtube.com/watch?v=ho7796-au8U

 */