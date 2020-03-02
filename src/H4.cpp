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
#include <H4.h>
#include <H4Utils.h>

#ifndef ARDUINO
	delegateSerial Serial;
	extern "C" {	
		int _write(int file, char *data, int len) {
			#ifdef CUBEIDE
			   HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, (uint8_t*)data, len,  0xFFFF);
			   return (status == HAL_OK ? len : 0);
			#else
				return write(1,data,len);
			#endif
		}
	}
#endif

#ifdef __unix__
	
	void nop(){}
	
	unsigned long h4GetTick(){
		using namespace std::chrono;
		long ms=duration_cast<milliseconds>( 
			time_point_cast<milliseconds>(
			steady_clock::now()).time_since_epoch()).count();
		return ms;
	}
	
	void interrupts(){}

	void noInterrupts(){}
#endif
//
//      and ...here we go!
//
void __attribute__((weak)) h4setup(){}
void __attribute__((weak)) h4StartPlugins(){}
void __attribute__((weak)) h4UserLoop(){}
void __attribute__((weak)) onReboot(){}

#define H4CH_TRID_CHNK 99
H4_INT_MAP	        H4::trustedNames={ {H4CH_TRID_CHNK,"CHNK"} };
H4_TIMER 		    H4::context=nullptr;
unordered_map<uint32_t,uint32_t> H4::unloadables;
vector<H4_FN_VOID>  H4::rebootChain={onReboot};

H4_TIMER_MAP	    task::singles={};

void  h4reboot(){ 
    h4.once(1000,[](){
        for(auto &c:H4::rebootChain) c(); 
        h4rebootCore();
    }); 
}    

H4Random::H4Random(uint32_t rmin,uint32_t rmax){ count=task::randomRange(rmin,rmax);	}

task* pq::add(H4_FN_VOID _f,uint32_t _m,uint32_t _x,H4_FN_COUNT _r,H4_FN_VOID _c,uint32_t _u,bool _s){
	task* t=new task(_f,_m,_x,_r,_c,_u,_s);
    taskEvent(t,'C');
	qt(t);
	return t;
}
/*
void pq::clear(){
	noInterrupts();
	for(auto const& qi:c) qi->endK();
	interrupts();
}
*/
uint32_t pq::gpFramed(task* t,H4_FN_RTPTR f){
	uint32_t rv=0;
	if(t){
		noInterrupts();
		if(has(t) || (t==H4::context)) rv=f(); // fix bug where context = 0!
		interrupts();
	}
	return rv;
}

uint32_t pq::endF(task* t){ return gpFramed(t,bind(&task::endF,t)); }

uint32_t pq::endU(task* t){	return gpFramed(t,bind(&task::endU,t)); }

bool 	 pq::endC(task* t,H4_FN_TIF f){ return static_cast<bool>(gpFramed(t,bind(&task::endC,t,f))); }

task* 	 pq::endK(task* t){ return reinterpret_cast<task*>(gpFramed(t,bind(&task::endK,t))); }

task* pq::next(){
	task* t=nullptr;
    //uint32_t now=(uint32_t) h4GetTick(); // can't do inside loop...clocks dont work when noInterrupts!!!
	noInterrupts();
	if(size()){
//	   if(!((int)(top()->at -  now)) > 0) { // .LT. Or .EQ. 
	   if(((int)(top()->at -  h4GetTick())) < 0) {
		t=top();
		pop();
	  }
	}
	interrupts();
	return t;
}

void pq::qt(task* t){
	noInterrupts();
	push(t);
	interrupts();
    taskEvent(t,'S');
}

vector<task*> pq::select(function<bool(task*)> p){
	vector<task*> match;
	noInterrupts();
	for(auto const& qi:c) if(p(qi)) match.push_back(qi);
	interrupts();
	return match;
}
//
//		task
//
task::task(
	H4_FN_VOID     	_f,
	uint32_t		_m,
	uint32_t		_x,
	H4_FN_COUNT    	_r,
	H4_FN_VOID     	_c,
	uint32_t		_u,
	bool 			_s
	):
  f{_f},
  rmin{_m},
  rmax{_x},
  reaper{_r},
  chain{_c},
  uid{_u},
  singleton{_s}
{
    if(_s){
		uint32_t id=_u%100;
		if(singles.count(id)) singles[id]->endK();    
		singles[id]=this;
	}
	schedule();
}

bool task::operator() (const task* lhs, const task* rhs) const { return (lhs->at>rhs->at); }

void task::operator()(){
	if(harakiri) _destruct(); // for clean exits
	else {
		f();
		if(reaper){ // it's finite
		  if(!(reaper())){ // ...and it just ended
			_chain(); // run chain function if there is one
			if((rmin==rmax) && rmin){
				rmin=86400000; // reque in +24 hrs
				rmax=0;
				reaper=nullptr; // and every day after
				requeue();
			} else _destruct();
		  } else requeue();
		} else requeue();
	}
}

void task::_chain(){ if(chain) h4.add(chain,0,0,H4Countdown(1),nullptr,uid); } // prevents tag rescaling during the pass

void task::cancelSingleton(uint32_t s){ if(singles.count(s)) singles[s]->endK(); }

uint32_t task::cleardown(uint32_t pass){
	if(singleton){
		uint32_t id=uid%100;
		singles.erase(id);
	}
	return pass;
}

void task::_destruct(){
    h4.taskEvent(this,'D');
	if(partial) free(partial);
    delete this;
}
//		The many ways to die... :)
uint32_t task::endF(){
	reaper=H4Countdown(1);
	at=0;
	return cleardown(1+nrq);
}

uint32_t task::endU(){
	_chain();
	return nrq+endK();
}

uint32_t task::endC(H4_FN_TIF f){
	bool rv=f(this);
	if(rv) return endF();
	return rv;
}

uint32_t task::endK(){
	harakiri=true;
	return cleardown(at=0);
}

uint32_t task::randomRange(uint32_t rmin,uint32_t rmax){ return rmax > rmin ? (rand() % (rmax-rmin)) + rmin:rmin; }

void task::requeue(){
	nrq++;
	schedule();
	h4.qt(this);
}

void task::schedule(){ at=(uint32_t) h4GetTick() + randomRange(rmin,rmax); }

void task::createPartial(void* d,size_t l){
	partial=malloc(l);
	memcpy(partial,d,l);
    len = l;
}
//
//      H4
//
extern  void h4setup();

void H4::_hookLoop(H4_FN_VOID f,H4_INT_MAP names,uint32_t subid){
    if(f) {
        unloadables[subid]=loopChain.size();
        loopChain.push_back(f);
    }
    trustedNames.insert(names.begin(),names.end());
}  
#ifdef ARDUINO
        void setup(){
            h4.startup();
            h4setup();
        }        
        
        void loop(){ h4.loop(); }

#endif

#define TAG(x) (u+((x)*100))

H4_TASK_PTR H4::every(uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,msec,0,nullptr,fnc,TAG(3),s); }

H4_TASK_PTR H4::everyRandom(uint32_t Rmin,uint32_t Rmax,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,Rmin,Rmax,nullptr,fnc,TAG(4),s); }

H4_TASK_PTR H4::nTimes(uint32_t n,uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,msec,0,H4Countdown(n),fnc,TAG(5),s); }

H4_TASK_PTR H4::nTimesRandom(uint32_t n,uint32_t Rmin,uint32_t Rmax,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,Rmin,Rmax,H4Countdown(n),fnc,TAG(6),s); }

H4_TASK_PTR H4::once(uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,msec,0,H4Countdown(1),fnc,TAG(7),s); }

H4_TASK_PTR H4::onceRandom(uint32_t Rmin,uint32_t Rmax,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,Rmin,Rmax,H4Countdown(1),fnc,TAG(8),s); }

H4_TASK_PTR H4::queueFunction(H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,0,0,H4Countdown(1),fnc,TAG(9),s); }

H4_TASK_PTR H4::randomTimes(uint32_t tmin,uint32_t tmax,uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,msec,0,H4Random(tmin,tmax),fnc,TAG(10),s); }

H4_TASK_PTR H4::randomTimesRandom(uint32_t tmin,uint32_t tmax,uint32_t Rmin,uint32_t Rmax,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,Rmin,Rmax,H4Random(tmin,tmax),fnc,TAG(11),s); }

H4_TASK_PTR H4::repeatWhile(H4_FN_COUNT fncd,uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){ return add(fn,msec,0,fncd,fnc,TAG(12),s); }

H4_TASK_PTR H4::repeatWhileEver(H4_FN_COUNT fncd,uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){
  return add(fn,msec,0,fncd,
				bind([this](H4_FN_COUNT fncd,uint32_t msec,H4_FN_VOID fn,H4_FN_VOID fnc,uint32_t u,bool s){
					fnc();
					repeatWhileEver(fncd,msec,fn,fnc,u,s);
				},fncd,msec,fn,fnc,u,s),
			TAG(13),s);
}

#ifndef __cplusplus
extern "C" {
#endif
	void H4::loop(){
		if(context=h4.next()){
            (*context)();
 			context=nullptr;
		}
        for(auto f:loopChain) f();
#ifndef H4_NO_USERLOOP
		h4UserLoop();
#endif
	}
#ifndef __cplusplus
}
#endif
//
//  DIAGNOSTIC
//
void H4::_matchTasks(function<bool(task*)> p,function<void(task*)> f){
    vector<task*> vesta=select(p);
    sort(vesta.begin(),vesta.end(),[](const task* a, const task* b){ return a->at < b->at; });
    for(auto const& m:vesta) if(has(m)) f(m);
}

    
H4_INT_MAP tasktypes={
    {3,"evry"}, // 3
    {4,"evrn"}, // 4
    {5,"ntim"}, // 5
    {6,"ntrn"}, // 6
    {7,"once"}, // 7
    {8,"1xrn"}, // 8
    {9,"qfun"}, // 9
    {10,"rntx"}, // 10
    {11,"rnrn"}, // 11
    {12,"rptw"}, // 12
    {13,"rpwe"}  // 13
};

const char* __attribute__((weak)) giveTaskName(uint32_t id){ return "ANON"; }

void H4::_dumpTask(task* t){
    char buf[256];
    uint32_t type=t->uid/100;
    uint32_t id=t->uid%100;
    sprintf(buf,"%09lu 0x%08lx %04d %s (%s/%s) %9d %9d %9d\n",
        t->at,
        (unsigned long) t,
        t->uid,
        t->singleton ? "S":" ",
        tasktypes.count(type) ? CSTR(tasktypes[type]):"NDEF",
        (trustedNames.count(id) ? CSTR(trustedNames[id]):giveTaskName(id)),
        t->rmin,
        t->rmax,
        t->nrq);
    Serial.print(buf);
}

using namespace std::placeholders;

void H4::dumpQ(){
	Serial.print("Due @tick UID        Type                     Min       Max       nRQ\n");  
    if(context) _dumpTask(context);     
	_matchTasks(
		[](task* t){ return true; },
        bind(&H4::_dumpTask,this,_1)
	);     
}
/*
void  H4::postMsg(string s){
    long startTime=micros();
    h4.queueFunction(bind([](string s,long startTime){ 
        Serial.print(CSTR(s));Serial.print(" ran @ ");Serial.println(startTime);
    },s,startTime));    
}
*/