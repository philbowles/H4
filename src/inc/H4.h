/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef H4_H
#define H4_H

#define H4_Q_SIZE	20
#define H4_VERSION  "0.1.0"

#if (defined ARDUINO_ARCH_STM32 || defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32)
    #define ARDUINO
#elif defined __unix__
    unsigned long h4GetTick();
    #define millis	h4GetTick
    #include<chrono>
#else // native stm32
    #define CUBEIDE
	#include "main.h"
    #define noInterrupts __disable_irq
    #define interrupts __enable_irq
    #define h4GetTick	HAL_GetTick
	#define millis		HAL_GetTick
#endif

#if defined ARDUINO
 	#include "Arduino.h"
	#define h4GetTick	millis
#else
    #include <memory.h>
    #include<stdio.h>
    #include<string.h>
    #include<unistd.h>
	#include "Print.h"
	#include<cstdarg>
	#include<string>
	extern "C" {
		int _write(int file, char *data, int len);
	}
	class delegateSerial: public Print{
		public:
			delegateSerial(){}

			void begin(uint32_t){}

			size_t write(uint8_t c){

				return _write(1,(char*) &c,1); // 1 is irrlevant
			}

			size_t write(const char *str)
			{
			  if (str == NULL) return 0;
			  return _write(1,(char*)str, strlen(str)); // 1 is irrelevant
			}

			size_t write(const uint8_t *buffer, size_t size)
			{
			  size_t n = 0;
			  while (size--) {
				if (write(*buffer++)) {
				  n++;
				} else {
				  break;
				}
			  }
			  return n;
			}

			size_t  printf(const char* fmt,...){
				va_list arg;
				va_start(arg, fmt);
				size_t rv=vprintf(fmt, arg);
				va_end(arg);
				return rv;
			}
	};
	extern delegateSerial Serial;
#endif

#include<string>
#include<vector>
#include<map>
#include<queue>
#include<algorithm>
#include<functional>

using namespace std;

class   task;

using	H4_TASK_PTR		=task*;
using	H4_TIMER		=H4_TASK_PTR;

using	H4_FN_TIF		=function<bool(task*)>;
using	H4_FN_VOID		=function<void(void)>;
using	H4_FN_COUNT		=function<uint32_t(void)>;

using 	H4_TIMER_MAP	=std::map<uint32_t,H4_TIMER>;

#define CSTR(x) x.c_str()

class H4Countdown {
	public:
		uint32_t 	count;
		H4Countdown(uint32_t start=1) {	count=start; }
		uint32_t operator()() { return --count; }
};

class H4Random: public H4Countdown {
  public:
        H4Random(uint32_t tmin=0,uint32_t tmax=0);
};
//
//		T A S K
//
class task{
			bool  		    harakiri=false;
	static	H4_TIMER_MAP    singles;

		    void            _chain();
		    void            _destruct();

	public:

            H4_FN_VOID     	f;
            uint32_t        rmin=0;
            uint32_t        rmax=0;
            H4_FN_COUNT    	reaper;
            H4_FN_VOID     	chain;
            uint32_t		uid=0;
            bool 			singleton=false;

            size_t			len=0;
            uint32_t        at;
            uint32_t		nrq=0;
            void*			partial=NULL;

			bool            operator()(const task* lhs, const task* rhs) const;
			void            operator()();

		task(){} // only for comparison operator

		task(
			H4_FN_VOID    	_f,
			uint32_t		_m,
			uint32_t		_x,
			H4_FN_COUNT    	_r,
			H4_FN_VOID    	_c,
			uint32_t		_u=0,
			bool 			_s=false
			);

		static 	void 		cancelSingleton(uint32_t);
				uint32_t 	cleardown(uint32_t t);
//		The many ways to die... :)
				uint32_t 	endF(); // finalise: finishEarly
				uint32_t 	endU(); // unconditional finishNow;
				uint32_t	endC(H4_FN_TIF); // conditional
				uint32_t	endK(); // kill, chop etc
//
				size_t 		getPartial(void* d);
				void 		requeue();
				void 		schedule();
				void		storePartial(void* d,size_t l);
		static 	uint32_t	randomRange(uint32_t lo,uint32_t hi);
};
//
//		P R I O R I T Y   Q U E U E (has to be after task)
//
class pq: public priority_queue<task*, vector<task*>, task> {
	protected:
            task*			add(H4_FN_VOID _f,uint32_t _m,uint32_t _x,H4_FN_COUNT _r,H4_FN_VOID _c,uint32_t _u=0,bool _s=false);
            int				capacity() { return c.capacity(); }
            void			clear();
            uint32_t 		gpFramed(task* t,function<uint32_t()> f);
            bool  			has(task* t){ return find(c.begin(),c.end(),t) != c.end(); }
            uint32_t		endF(task* t);
            uint32_t		endU(task* t);
            bool			endC(task* t,H4_FN_TIF f);
            task*  			endK(task* t);
            task* 			next();
            void  			qt(task* t);
            void  			reserve(size_type n){ c.reserve(n); }
            vector<task*> 	select(function<bool(task*)> p);
};

class H4: public pq{
	friend class task;
            void            matchTasks(function<bool(task*)> p,function<void(task*)> f);
    static  vector<H4_FN_VOID> loopChain;
public:
	H4(size_t qSize=H4_Q_SIZE){ reserve(qSize); }

	static  H4_TASK_PTR		context;
    static  void            hookLoop(H4_FN_VOID f){ loopChain.push_back(f); }
    static  void            runHooks(){ for(auto f:loopChain) f(); }
	static	void 		    loop();

            H4_TASK_PTR 	every(uint32_t msec, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	everyRandom(uint32_t Rmin, uint32_t Rmax, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	nTimes(uint32_t n, uint32_t msec, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	nTimesRandom(uint32_t n, uint32_t msec, uint32_t Rmax, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR		once(uint32_t msec, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	onceRandom(uint32_t Rmin, uint32_t Rmax, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR		queueFunction(H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	randomTimes(uint32_t tmin, uint32_t tmax, uint32_t msec, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	randomTimesRandom(uint32_t tmin, uint32_t tmax, uint32_t msec, uint32_t Rmax, H4_FN_VOID fn, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	repeatWhile(H4_FN_COUNT w, uint32_t msec, H4_FN_VOID fn = []() {}, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);
            H4_TASK_PTR 	repeatWhileEver(H4_FN_COUNT w, uint32_t msec, H4_FN_VOID fn = []() {}, H4_FN_VOID fnc = nullptr, uint32_t u = 0,bool s=false);

            H4_TASK_PTR		cancel(H4_TASK_PTR t = context) { return endK(t); } // ? rv ?
            void 			cancelAll(H4_FN_VOID fn = nullptr);
            void 			cancelSingleton(uint32_t s);
            void			cancelSingleton(initializer_list<uint32_t> l){ for(auto i:l) cancelSingleton(i); }
            uint32_t 		finishEarly(H4_TASK_PTR t = context) { return endF(t); }
            uint32_t 		finishNow(H4_TASK_PTR t = context) { return endU(t); }
            bool			finishIf(H4_TASK_PTR t, H4_FN_TIF f) { return endC(t, f); }
//
//      DIAGNOSTIC
//
            void            dumpQ();
};

#endif // H4_H

extern H4 h4;
#ifdef CUBEIDE
extern UART_HandleTypeDef huart3;
#endif
