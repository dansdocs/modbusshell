/**************************************************
Make an array of timers available. Each timer can 
can have a power of 10 divisor up to MAXPOWER. 
There can be MAXTIMERS. 
**************************************************/

#include "./platform.h"
#include "./timers.h"


uint8_t tmrs[MAXTIMERS];          // an array of timers set to a value and then decremented to zero. 
uint8_t power10div[MAXTIMERS];    // the power of 10 divisor to go along with each timer. Each timer can effectivly be assigend a power of 10 from 0 to MAXPOWER10
uint8_t x10count[MAXPOWER10];     // an array of counters, each element increments a tenth the rate of the previous. 
uint8_t allocationIndex = 0;      // the index of which timer has been given out. Max is MAXTIMERS. 


// Notes about timers: use of uint8_t means that the updates should be atomic on any processor. 
// this lets use from interupt and main code without gards. That means that the 
// Maximum timer value is 255. 
// Because the divisor counters are free running with respect to when a timer might be initialised
// the timer accuracy is up to 1 count off becuase the divisor is anywhere through its cycle. 
// Really if you are looking at a timer value < 10 you probably would be better using the next timer along
// and using a value of < 100 instead. This means the 1 count error is 1/10 the size. 


// return an index for the timer. div_power10 is the power of 10 you
// want the base tick rate divided by for that timer. Eg. 0 means 10^0=1 so the base 
// tick rate is divided by 1. 1 means 10^1=10 so a tenth the base tick rate. 
// 2 means 10^2 so a hundredth the base tick rate. The maximum is MAXPOWER10. 
// returns timer index. Returns zero if no timers left to allocate. 
uint8_t timers_get_timer(uint8_t div_power10){
    if (allocationIndex < MAXTIMERS-1) {
        allocationIndex++;
        power10div[allocationIndex] = div_power10;      
        return (allocationIndex);
    }
    return 0;  // signify that no timers are left to provide
}

void timers_set_timeout(uint8_t tmr_idx, uint8_t val){
    tmrs[tmr_idx] = val;    
}

uint8_t timers_check_expired(uint8_t tmr_idx){
    if (!tmrs[tmr_idx]) return 1;
    return 0;
}


// call this at a regular rate such as 1mS. The timers are a multiple of 10 
// of the rate this function is called.  
void _update_timers(void){
    uint8_t i = 0;
    uint8_t t = 0;
    //static uint32_t cntr = 1;
    
    // ripple through all the x10 counters
    for (i=0; i < MAXPOWER10; i++){
        x10count[i]++; 
        if (x10count[i] < 10) break;
    }

    // reset any x10 counters
    for (i=0; i < MAXPOWER10; i++){
        if (x10count[i] >= 10) {
			x10count[i] = 0;
		}
    }   
    
    // decrement any registered counters according to the scale factor 
    for (i = 0; i <= allocationIndex; i++){
        if (tmrs[i]) {
            if (power10div[i] == 0) tmrs[i]--;
            else {
				for (t = 0; t < power10div[i]; t++){
			  	    if (x10count[t] != 0) break;
				    else if (t == power10div[i]-1) tmrs[i]--; 
				}
			}	
        }           
    }
}

void timers_tick(){
	if (platform_enoughTimePassed()) _update_timers();	
}






