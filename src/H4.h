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

#ifndef H4_H
#define H4_H

#define H4_VERSION  "4.0.0"

#define H4_USERLOOP       0 // improves performance
#define H4_COUNT_LOOPS    0 // DIAGNOSTICS
#define H4_HOOK_TASKS     0

#define H4_JITTER_LO    100 // Entropy lower bound
#define H4_JITTER_HI    350 // Entropy upper bound
#define H4_Q_CAPACITY	 10 // Default Q capacity
#define H4_Q_ABS_MIN      6 // Absolute minimum Q capacity

enum {
    H4_CHUNKER_ID=90,
    H4AT_SCAVENGER_ID,
    H4AS_SSE_KA_ID,
    H4AS_WS_KA_ID,
    H4AMC_RCX_ID,
    H4AMC_KA_ID
}; // must not grow past 99!

#include <Arduino.h>

#define h4rebootCore ESP.restart 
#define H4_BOARD ARDUINO_BOARD

void h4reboot();

void HAL_enableInterrupts();
void HAL_disableInterrupts();

#include<string>
#include<vector>
#include<unordered_map>
#include<queue>
#include<algorithm>
#include<functional>

class   task;
using	H4_TASK_PTR		=task*;
using	H4_TIMER		=H4_TASK_PTR;

using	H4_FN_COUNT		=std::function<uint32_t(void)>;
using	H4_FN_TASK		=std::function<void(H4_TASK_PTR,uint32_t)>;
using	H4_FN_TIF		=std::function<bool(H4_TASK_PTR)>;
using	H4_FN_VOID		=std::function<void(void)>;
using   H4_FN_RTPTR     =H4_FN_COUNT;
//
using   H4_INT_MAP      =std::unordered_map<uint32_t,std::string>;
using 	H4_TIMER_MAP	=std::unordered_map<uint32_t,H4_TIMER>;
//

#define CSTR(x) x.c_str()
#define ME H4::context
#define MY(x) H4::context->x
#define TAG(x) (u+((x)*100))

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
            H4_FN_VOID      lastRites=[]{};
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

        ~task(){ }//Serial.printf("T=%u TASK DTOR %p\n",millis(),this); }

		static	void 		cancelSingleton(uint32_t id);
				uint32_t 	cleardown(uint32_t t);
//		The many ways to die... :)
				uint32_t 	endF(); // finalise: finishEarly
				uint32_t 	endU(); // unconditional finishNow;
				uint32_t	endC(H4_FN_TIF); // conditional
				uint32_t	endK(); // kill, chop etc
//
				void		createPartial(void* d,size_t l);
				void 		getPartial(void* d){ memcpy(d, partial, len); }
                void        putPartial(void *d){ memcpy(partial, d, len); }
                void 		requeue();
				void 		schedule();
		static 	uint32_t	randomRange(uint32_t lo,uint32_t hi); // move to h4
};
//
//      H 4
//
extern void h4StartPlugins();

class H4: public std::priority_queue<task*, std::vector<task*>, task>{ // H4P 35500 - 35700
	friend class task;
                std::vector<H4_FN_VOID> loopChain;
    public:       
        static  std::unordered_map<uint32_t,uint32_t> unloadables;
	    static  H4_TASK_PTR		context;

	    	    void 		    loop();
                void            setup();

                H4(uint32_t baud=0,size_t qSize=H4_Q_CAPACITY){ 
                    reserve(qSize);
                    if(baud) { 
                        Serial.begin(baud);
                        Serial.printf("\nH4 RUNNING %s\n",H4_VERSION);
                    }
                }

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
                void			cancel(std::initializer_list<H4_TASK_PTR> l){ for(auto const t:l) cancel(t); }
                void 			cancelAll(H4_FN_VOID fn = nullptr);
                void 			cancelSingleton(uint32_t s){ task::cancelSingleton(s); }
                void			cancelSingleton(std::initializer_list<uint32_t> l){ for(auto const i:l) cancelSingleton(i); }
                uint32_t 		finishEarly(H4_TASK_PTR t = context) { return endF(t); }
                uint32_t 		finishNow(H4_TASK_PTR t = context) { return endU(t); }
                bool			finishIf(H4_TASK_PTR t, H4_FN_TIF f) { return endC(t, f); }
//              syscall only
                size_t          _capacity(){ return c.capacity(); } 
                std::vector<task*>   _copyQ();
                void            _hookLoop(H4_FN_VOID f,uint32_t subid);
                bool            _unHook(uint32_t token);

//	protected:
                uint32_t 		gpFramed(task* t,std::function<uint32_t()> f);
                bool  			has(task* t){ return find(c.begin(),c.end(),t) != c.end(); }
                uint32_t		endF(task* t);
                uint32_t		endU(task* t);
                bool			endC(task* t,H4_FN_TIF f);
                task*  			endK(task* t);
                void  			qt(task* t);
                void  			reserve(size_t n){ c.reserve(n); }
                H4_FN_TASK      taskEvent=[](task*,uint32_t){};
//
#if H4_HOOK_TASKS
        static  H4_FN_TASK      taskHook;

                void            _hookTask(H4_FN_TASK f){ taskHook=f; }
        static  std::string     dumpTask(task* t,uint32_t faze);
        static  void            addTaskNames(H4_INT_MAP names);
        static  std::string     getTaskType(uint32_t t);
        static  const char*     getTaskName(uint32_t t);
#else
        static  void            addTaskNames(H4_INT_MAP names){}
#endif
        static  void            dumpQ();    
//    public:
                task*			add(H4_FN_VOID _f,uint32_t _m,uint32_t _x,H4_FN_COUNT _r,H4_FN_VOID _c,uint32_t _u=0,bool _s=false);
};

template<typename T>
class pr{
        size_t   size=sizeof(T);

        template<typename T2>
        T2  put(T2 v){ 
            memcpy(MY(partial),reinterpret_cast<void*>(&v),size);
            return get<T2>();
            }
        template<typename T2>
        T2  get(){ return (*(reinterpret_cast<T2*>(MY(partial)))); }

    public:
        pr(T v){
            if(!MY(partial)){ 
                MY(partial)=reinterpret_cast<T*>(malloc(size));
                put<T>(v);
            }
        }

        pr operator=( const T other ) { return put(other);  }

        operator T() { return get<T>(); }

        T operator +(T v) { return get<T>()+v; }

        T operator +=(T v) { return put<T>(get<T>()+v); }

        T* operator->() const {
        return reinterpret_cast<T*>(MY(partial));
      }
};

extern H4 h4;

template<typename T>
static void h4Chunker(T &x,std::function<void(typename T::iterator)> fn,uint32_t lo=H4_JITTER_LO,uint32_t hi=H4_JITTER_HI,H4_FN_VOID final=nullptr){
    H4_TIMER p=h4.repeatWhile(
        H4Countdown(x.size()),
        task::randomRange(lo,hi), // arbitrary
        [=](){ 
            typename T::iterator thunk;
            ME->getPartial(&thunk);
            fn(thunk++);
            ME->putPartial((void *)&thunk);
            yield();
            },
        final,
        H4_CHUNKER_ID);
    typename T::iterator chunkIt=x.begin();
    p->createPartial((void *)&chunkIt, sizeof(typename T::iterator));
    p->lastRites=[=]{
        free(p->partial);
        p->partial=nullptr;
    };
}
#endif // H4_H