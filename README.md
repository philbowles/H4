# H4 vn 0.1.0

## Universal functional scheduler / timer with rich API for asynchronous one-off, periodic and random events with event chaining. Allows identical code on multiple platforms: ArduinoIDE ESP8266, ESP32, STM32NUCLEO-* / STM32CubeIDE HAL F0/1/3/4 / Ubuntu Linux / Raspberry Pi (jessie) 

![H4 Flyer](/assets/H4flyer.jpg)

## Change log

Read recent changes [here](/changelog.txt)

## Introduction

Serialises Ascyhronous events (buttons. sensors etc) into a serial queue running on "the main loop"

Invokes user-defined callback on event. Callback can be:

* Bare statically-linked function
* Class function
* Lambda function
* Any C++ functor with operator()() override

All callbacks (type `H4_FN_VOID`) can take numerous parameters using C++ stdlib bind.

All callbacks take a "chain" function (also type `H4_FN_VOID`) which is called on completion. "Completion" can occur one of three ways:

*   Naturally-expiring (e.g. "`once`") task exits
*   Free-running (e.g. "`everyXXX`") task is cancelled by user code
*   Task can terminate itself arbitrarily

Tasks which only "make sense" if they are unique (e.g. a system "ticker") can declare themselves on creation as a "singleton". Any existing task with the same type and ID will be cancelled and replaced by the new instance, ensuring only one copy is ever in the queue.

All task creators return a "handle" (type `H4_TASK_PTR`) which can be used to subsequently cancel the task, or ignored if not required

**N.B.** with one exception (`queueFunction`) all tasks start _after_ the first specifiied time interval. Using the infinite task "`every(1000...`" will invoke the first instance of user callback at Tstart + 1sec (1000 mS).

## API

### Event callbacks (all timed in milliseconds)

```cpp
every   // run task every t milliseconds
everyRandom // run task continuously rescheduling at random time nMin < t < nMax milliseconds
nTimes // run task n times at intervals of t milliseconds 
nTimesRandom // run task n times rescheduling at random time nMin < t < nMax milliseconds
once // have a guess
onceRandom // have another guess
queueFunction // run task NOW* - no initial interval t.[* on next schedule: MCU-dependent, but ~uSec]
randomTimes // run task any number of times nMin < n < nMax at intervals of t
randomTimesRandom // run task random number of time nMin < n < nMax at random intervals of nMin < t < nMax milliseconds
repeatWhile // run task until user-defined "countdown" function (type H4_FN_COUNT) returns zero then cancel
repeatWhileEver // run task until user-defined "countdown" function (type H4_FN_COUNT) returns zero then rescehule [ Note 1]
```
_Note 1_ 

The user _*MUST*_ reset, cancel, stop < whatever > the condition that causes the countdown expiry, otherwise an infinite loop will occur and probably crash the MCU. Since `repeatWhile` cancels itself after the first occurrence of countdown expiry this is not an issue, but `repeatWhileEver` reschedules itself, so if the countdown function has not reset itself, it will still be "expired" and so `repeatWhileEver` reschedules itself, so if the countdown function has not reset itself, it will still be "expired" and so `repeatWhileEver`... You get the picture?

### Task cancellation:

```cpp
cancel // with immediate effect, do not run current instance or chain function, pass "GO" or collect $200
cancelAll // You really don't ever want to do this, but it's there...
cancelSingleton // have a guess
cancelSingleton(initializer_list<uint32_t> l) // have several guesses in one call
finishEarly // jump to chain function after current schedule then quit
finishNow // quit after current schedule, do not run chain function
finishIf // "finishEarly" if user-supplied termination function (type H4_FN_TIF) returns true
```

### Globals

User should instantiate an H4 object at global scope, naming it h4

```cpp
H4  h4; // can also provide a value for lengthe of Q, default=20, e.g. H4 h4(13);

h4.context // contains the H4_TASK_PTR of the currently scheduled task or `nullptr` if Q empty

H4::loop() // must be called in a `while(true)` loop for non-arduinoIDE implementations. 
// In Arduino IDE, do NOT include a "normal" loop() funtion!!!
```

## Installation

### Arduino IDE

Simply download the zip of this repostory and install as an Arduino library: Sketch/Include Library/Add .ZIP Library...

### STM32CubeIDE

This is a little more involved and has some limitations:

#### Serial Limitations

**H4** includes a heavily-modified port of the Arduino Serial class to enable a "straight lift" of Arduino code that includes `Serial.printX` statments. (these are also used by the diagnostic routine `h4.dumpQ()` which will probably be removed in a later release or at least made `#define`-able).

The port is write-only: printing is supported, but no reading or checking if available() etc. On the plus side, pritning includes support for formatted output (via `printf`) and `std::string`

1. It is hobbled by a cheap-and-cheesy hardcoded reliance on USART3 until I work out a fancy automatic method.
2. The `begin(baud_rate);` method is a stub which does nothing, the speed used is that set by the HW configuration in CubeMX. It is included to allow "straight lift" but need not be called. /* TODO: fix this! */

#### Timing limitation

**H4** Is predicated upon `HAL_GetTick()` returning milliseconds. Any other timing strategy will break it.

#### Installation steps

1. Generate you project as a C++ project, selecting USART3 (*see above)
2. Rename main.c to .cpp
3. Download and unzip this repository
4. Import H4.cpp, Print.cpp into <project>/Core/Src
5. Import H4.h, Print.h into <project>/Core/Inc
6. In main.cpp add the following lines to the relevant "safe" code blocks:

```cpp
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "H4.h"
/* USER CODE END Includes */

...

/* USER CODE BEGIN PV */
H4 h4(25); // define inital Q size: H4 h4; defaults to 20
/* USER CODE END PV */

...

/* USER CODE BEGIN 2 */
// put all your H4 setup code here, e.g. :
// NB the Arduino millis() function is mapped to HAL_GetTick()
Serial.print("H4 running natively on STM32F429ZI\n");
h4.every(1000,[]{
    Serial.print(millis());Serial.println(" PING ");
});
/* USER CODE END 2 */

...
/* Infinite loop */
/* USER CODE BEGIN WHILE */
while (1)
{
/* USER CODE END WHILE */
h4.loop();
/* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */
```

## Advanced Topics

This is an advanced topic for experts only. If you are a beginner, skip to the [Sample Code](/README.md#example-code-arduino-ide) at the end.

### Things you can do with a context H4_TASK_PTR

Experts: All usage should be performed with care and _only_ if you really know what you are doing!

#### "Partial Results"

"Worker threads" or tasks that have a big job to do "in the background" can "chunk up" the job by saving intermediate values in an area managed by H4 and preserved between schedules. A simple example might be to do something with a very large array. The user might create an "every" task. On the first pass ,create an iterator, do(X) store the iterator in "partial". On subsequent passes, retirieve the oterator from "partials", increment it, do(X) and store it etc. When the iterator "runs off the end", the task cancel itself. H4 will do any cleanup.

```cpp
h4.context->storePartial(void* d,size_t l); // save a lump of data d (of length l) in partial results
h4.getPartial(void* d); // fill an l-length block of data (user-defined) into d from partials results
// If d is created by malloc etc, you MUST MUST MUST deallocate it at some point. static data is safer
// if you can afford the memory. OR if you like living dangerously...
````

As you have guessed by now, `h4.context` is simply a task pointer to an item in the Queue. Most methods and members are public, so TAKE CARE.

`h4.context->partial` is a pointer to the partial results and can be used *IN READ ONLY MODE* in preference to `getPartial()`. *DO NOT* run off the end of it: `h4.context->len` contains its size in byes saved from `storePartial()`. *DO NOT CHANGE THE VALUE OF len!!!*

#### The many ways to die

The public cancellation methods map onto the following task functions. Since we are talking about code inside a task, you can save a function call by calling these directly on the context pointer, e.g. `h4.context->endK();` will do the same as h4.cancel(); Make this the last call, you cannot rely on anything after it has been called, just get out while you can!

```cpp
uint32_t 	endF(); // finalise: finishEarly
uint32_t 	endU(); // unconditional finishNow;
uint32_t	endC(H4_FN_TIF); // conditional
uint32_t	endK(); // kill, chop etc
```

There are other functions, but here's the golden rule: If I haven't mentioned it and explained it here: **DON'T CALL IT!**

#### Member Variables

These control H4's scheduling so **DO NOT CHANGE THEM!**. A _lot_ of things will break if you do, including probably your whole app. They are public and writeable for two reasons:

* They allow some "clever" functionality: its good to know _which_ iteration you are on in an nTimes task (see the "10 Green Bottles" example)
* I'm lazy

Don't use them to "cheat" the system, use the proper public H4 calls for safety.

```cpp
H4_FN_VOID     	f; // "It". The task. Your function that gets called on each schedule
uint32_t        rmin=0; // (usually) the controlling time in mS OR the minimum random time [ see Note 2 ]
uint32_t        rmax=0; // the maximum random time [ see Note 2 ] 
H4_FN_COUNT     reaper; // when this function retrunes zero, the Grim Reaper calls and the task dies a horrible death
H4_FN_VOID      chain; // you guessed it...
uint32_t        uid=0; // unique ID of task
bool            singleton=false; // sigh...
size_t          len=0; // length of partial data
uint32_t        at;  // "due" time [ see note 3 ]
uint32_t        nrq=0; // numbr of times this task has been RE-QUEUED. Note the "RE-" [ See Note 4 ]
void*           partial=NULL; // ptr -> Your partial results
```

_Note 2_

`rmin` and `rmax` work together. If it's not a random task, `rmin` is functionally equivalent to mSec which controls the timer, e.g. `once(30000,...` will have `rmin` = 30000 and `rmax`=0. For randomly-timed tasks  they have the min and max values of the randomness. In `queueFunction`, they are both zero, since it never has to wait for any time before it runs f(). There is a special case: when they are equal (but not both zero) then the "due" time is set to T=100*60*60*24 or a whole day's worth of milliseconds, causing the task to be reschedeled at the exact same time tomorrow... allowing a "`daily(...`" task  - which will be coming soon

_Note 3_

The core of the scheduler runs on an architecture-dependent clock which returns a number of milliseconds T. Whatever T is, H4 adds `rmin` to it and sets that as the "due" tme, i.e. run this "at" time T+`rmin`. On every loop, the task at the head of the queue is checked and if `at` is > T+rmin, i.e. the time at which is was due to run has passed, `f()` is called and the task then makes its reschedule decision.

If the reaper returns zero the task cleans up, calls the chain (if any) and deletes itself. If it needs to reschedule, it adds `rmin` (or `randomRange(rmin,rmax)`) to the due time `at` and copies itself back into the queue.

_Note 4_

At the end of a task that has run once, it has been scheduled but _it has not been RE-queued_ hence `nrq==0`. For all task types except `queueFunction` the value of `nrq+1` is the number of times that `f()` has been run, being 1 schedule + `nrq` RE-schedules. 

## Example code (Arduino IDE)

```cpp
#include <H4.h>
H4 h4;
/*
 Optional: allows you to "tag" your own task so that you can see them in the Queue dump
 */
const char* getTaskName(uint32_t n){
  static std::map<uint32_t,string> mydata={
    {1,"Tick"},
    {4,"Rude"},
    {13,"Link"},
    {66,"1Tim"},
    {99,"10GB"}
    };
  return mydata.count(n) ? mydata[n].c_str():"NONE";
}

void setup() {
  Serial.begin(115200);
  h4.everyRandom(5000,10000,[](){ 
    Serial.print(millis());Serial.println(" RUDE INTERRUPTION");
    h4.dumpQ();
  },nullptr,4);

  h4.every(1000,[]{ Serial.print(millis());Serial.println(" PING "); },nullptr,1);  
  
  h4.once(30000,
    []{ Serial.print(millis());Serial.println(" ONE TIME ONLY!!!"); },
    [](){ // chain fn - fires immediately aftery primary completes
      Serial.println("Chained from 1Tim and fire off new task");
      h4.nTimes(11,5000,[](){ // the song is "10 green bottles", but we want to include zero
        uint32_t nBottles=11 - (h4.context->nrq+1);
        Serial.print(nBottles);Serial.println(" green bottles, hangin' on a wall...");
      },nullptr,99);
    },13);
}
/* 
 Optional: for linking into other libraries that have a "loop" or "handle" or "keepalive" function
 that must be called regularly...usually in the "main loop" - which isn't there anymore!

 If you don't need it, omit it completely
*/
void userLoop(){
    // e.g. http.handleClient();
    // e.g. mqtt.loop();
  static long prev=0;
  if(millis() - prev > 5000) {
    Serial.print("USER LOOP\n");
    prev=millis();
  }
}
```

## Output from above (Running on ESp32)

![Output ESp32](/assets/output.jpg)
