/**
 * Daniel 2017
 * Make an array of timers available. Each timer can can have a power of 10 divisor up to MAXPOWER. There can be TIMERS_MAXTIMERS. 
 * See below about including. The general concept is to get a timer and set its scaling. Then set a value for the 
 * timer to expire then check to see if its expired - optionally reset it. 
 *
 *  To get a timer:
 * uint8_t tmr;
 * tmr = timers_get_timer(2);
 * 
 * tmr will be an integer that acts as a handle/reference for the timer. 
 * The parameter is the power of 10 that the tick rate is divided by. In this case 10^2 = 100
 * So if its a 1mS base tick rate then tmr will be decremented every 1mS * 100 = 100mS
 * 
 * To set a timer:
 * timers_set_timeout(tmr, 20);
 * 
 * tmr will start ticking down from 20. Each decrement is 100mS (from get_timer) so the timer 
 * will expire in 2seconds. 
 * 
 * To check if timer expired: 
 * if (timers_check_expired(tmr) printf("expired\n");
 * 
 * 
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - with one addition. In one and only one file, just before #include "./timers.h" 
 * also put in #define TIMERS_IMPLEMENTATION 
 * You can #include "./timers.h" in other c files as normal. 
 */

#ifndef TIMERS_H
#define TIMERS_H

    #include <stdint.h>  // uint8_t etc
    
    // Its optional to pass in a function pointer for logging. If a function is provided
    // it will be used otherwise printf will be used if windows/linux - otherwise nothing.      
    // Intention is: level,  __FILE__, __LINE__, __VA_ARGS__ 
    typedef void (*timers_logFnT)(uint8_t, const char *, uint16_t, const char *, ...); 
    
    // Its optional to pass in a function pointer for logging. If a function is provided
    // it will be used otherwise printf will be used and its windows/linux otherwise nothing.    
    void timers_setLogFn(timers_logFnT fn);

    // The absolute max values for the following two is 255. 
    #define TIMERS_MAXTIMERS 10    // maximum number of timers that can be allocated (actually TIMERS_TIMERS_MAXTIMERS-1 because zero isn't allocated)
    #define TIMERS_MAXPOWER10 6    // maximum power of 10 that the base tick rate can be divided by.  
    
    // returns a reference to your timer. You also set the divisor. 0 means the timer is decremented
    // at the rate that update_timers() is called. 1 is decremented a tenth, 2 is decremented a hundreth and so on.  
    uint8_t timers_get_timer(uint8_t div_power10);
    
    // set the timeout value (and start the countdown) tmr_idx is the reference from get_timer. 
    // val is a number from 1 to 255. The time out value is the interval of time between callse to 
    // update_timers() * 10^div_power10 * val
    void timers_set_timeout(uint8_t tmr_idx, uint8_t val);
    
    // returns 1 if timer expired else returns 0. tmr_idx is the reference from get_timer()
    uint8_t timers_check_expired(uint8_t tmr_idx);
    
    // returns 1 if timer expired else returns 0. tmr_idx is the reference from get_timer(). val is a number from 1 to 255.    
    uint8_t timers_check_expired_reset(uint8_t tmr_idx, uint8_t val);
    
    // call this function from an interupt at a set interval of time (eg 1mS). 
    // On windows and linux just call it as often as possible and it will 
    // update the timers if 1mS has passed. 
    void timers_tick(void);
    
#endif // TIMERS_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef TIMERS_IMPLEMENTATION
#undef TIMERS_IMPLEMENTATION

    uint8_t _timers_tmrs[TIMERS_MAXTIMERS];          // an array of timers set to a value and then decremented to zero. 
    uint8_t _timers_power10div[TIMERS_MAXTIMERS];    // the power of 10 divisor to go along with each timer. Each timer can effectivly be assigend a power of 10 from 0 to TIMERS_MAXPOWER10
    uint8_t _timers_x10count[TIMERS_MAXPOWER10];     // an array of counters, each element increments a tenth the rate of the previous. 
    uint8_t _timers_allocationIndex = 0;      // the index of which timer has been given out. Max is TIMERS_MAXTIMERS. 

    // On windows and linux instead of an interupt calling the timers_tick() to update the timers 
    // every millisecond; instead timers_tick() is just called continuously and the system clock is checked 
    // to see if a mS has passed. The performance of doing this is recorded and loged. 
        
    // use the macro TIMERS_LOG like so:
    // TIMERS_LOG(TIMERS_LOG_WARN, "this is warning number %u", 5);
    // and the macro will either use the function passed in or printf if no function has been provided & win/linux
    // otherwise nothing.    
    timers_logFnT _timers_logFn = 0;
    void timers_setLogFn(timers_logFnT fn){_timers_logFn = fn;}  
    enum { TIMERS_LOG_TRACE, TIMERS_LOG_DEBUG, TIMERS_LOG_INFO, TIMERS_LOG_WARN, TIMERS_LOG_ERROR, TIMERS_LOG_FATAL };    
    
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) 
        #include <sys/timeb.h> //ftime timeb
        #include <stdio.h>     // printf        
        
        uint8_t _timers_enoughTimePassed();
                    
        #define TIMERS_LOG(level, ...) do {if (_timers_logFn == 0){printf("\nLog level %u in %s ", level, __FILE__); printf(__VA_ARGS__); printf("\n");} else _timers_logFn(level,  __FILE__, __LINE__, __VA_ARGS__);}while(0)        
    #else        
        #define _timers_enoughTimePassed() 1
        #define TIMERS_LOG(level, ...) do {if (_timers_logFn != 0) _timers_logFn(level,  __FILE__, __LINE__, __VA_ARGS__);}while(0)        
    #endif // BUILD_FOR_WINDOWS || BUILD_FOR_LINUX         


    
    // Notes about timers: use of uint8_t means that the updates should be atomic on any processor. 
    // this lets use from interupt and main code without gards. It also means that the 
    // Maximum timer value is 255. 
    // Because the divisor counters are free running with respect to when a timer might be initialised
    // the timer accuracy is up to 1 count off becuase the divisor is anywhere through its cycle. 
    // Really if you are looking at a timer value < 10 you probably would be better using the next timer along
    // and using a value of < 100 instead. This means the 1 count error is 1/10 the size. 
    
    
    // return an index for the timer. div_power10 is the power of 10 you
    // want the base tick rate divided by for that timer. Eg. 0 means 10^0=1 so the base 
    // tick rate is divided by 1. 1 means 10^1=10 so a tenth the base tick rate. 
    // 2 means 10^2 so a hundredth the base tick rate. The maximum is TIMERS_MAXPOWER10. 
    // returns timer index. Returns zero if no timers left to allocate. 
    uint8_t timers_get_timer(uint8_t div_power10){
        if (_timers_allocationIndex < TIMERS_MAXTIMERS-1) {
            _timers_allocationIndex++;
            _timers_power10div[_timers_allocationIndex] = div_power10;
            TIMERS_LOG(TIMERS_LOG_INFO, "Timer 0x%02X of 0x%02X allocated.", _timers_allocationIndex, TIMERS_MAXTIMERS-1);      
            return (_timers_allocationIndex);
        }
        return 0;  // signify that no timers are left to provide
    }
    
    void timers_set_timeout(uint8_t tmr_idx, uint8_t val){
        _timers_tmrs[tmr_idx] = val;    
    }
    
    uint8_t timers_check_expired(uint8_t tmr_idx){
        if (!_timers_tmrs[tmr_idx]) return 1;
        return 0;
    }

    uint8_t timers_check_expired_reset(uint8_t tmr_idx, uint8_t val){
        if (!_timers_tmrs[tmr_idx]) {
            timers_set_timeout(tmr_idx, val);
            return 1;
        }
        return 0;
    }    
    
    // call this at a regular rate such as 1mS. The timers are a multiple of 10 
    // of the rate this function is called.  
    void _timers_update(void){
        uint8_t i = 0;
        uint8_t t = 0;
        
        // ripple through all the x10 counters
        for (i=0; i < TIMERS_MAXPOWER10; i++){
            _timers_x10count[i]++; 
            if (_timers_x10count[i] < 10) break;
        }
    
        // reset any x10 counters
        for (i=0; i < TIMERS_MAXPOWER10; i++){
            if (_timers_x10count[i] >= 10) {
    			_timers_x10count[i] = 0;
    		}
        }   
        
        // decrement any registered counters according to the scale factor 
        for (i = 0; i <= _timers_allocationIndex; i++){
            if (_timers_tmrs[i]) {
                if (_timers_power10div[i] == 0) _timers_tmrs[i]--;
                else {
    				for (t = 0; t < _timers_power10div[i]; t++){
    			  	    if (_timers_x10count[t] != 0) break;
    				    else if (t == _timers_power10div[i]-1) _timers_tmrs[i]--; 
    				}
    			}	
            }           
        }
    }
    
    
    void timers_tick(){
    	if (_timers_enoughTimePassed()) _timers_update();	
    }
    
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
        // uses a system timer to see when 1mS has passed and returns 1 otherwise 0.   
        // keeps track of the performance of using the system counter - ie how many times do we meet the 1mS elapsed time
        // compared to how many times we are too slow.                              
        uint8_t _timers_enoughTimePassed(){
	        static uint64_t old_time = 0;
            static uint8_t first_time = 1;
            uint64_t curr_time = 0;
            uint64_t diff_time = 0;
            static uint8_t first_perf_time = 1;    
            struct timeb timebuffer;
            uint8_t timePassed = 1;
            uint32_t errorPercent = 0;
            uint8_t loglvl;
            uint8_t i;

           #define _TIMERS_HIST_MAX 5                       
            struct Perfd {
                uint32_t perf_max_time;
                uint32_t perf_buckets[_TIMERS_HIST_MAX];
                uint16_t performanceLogTime;
                uint32_t toSlowCount;
            };
            static struct Perfd perfd;
 
                                 
            // Get the time from the operating system.           
            ftime(&timebuffer);
            curr_time = (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
             
            if (first_time) {
                first_time = 0;
                old_time = curr_time;
            }
               
            diff_time = curr_time - old_time;
            if (diff_time < 1) timePassed = 0;
            else {
                old_time = curr_time;
                perfd.performanceLogTime++;
            }
              
            // update performance data.		    
            if (first_perf_time){
                first_perf_time = 0;
                perfd.perf_max_time = (uint16_t) diff_time;
                for (i = 0; i<  _TIMERS_HIST_MAX; i++) perfd.perf_buckets[i]=0;
                perfd.performanceLogTime = 0;
                perfd.toSlowCount = 0;                
            }
             
            if (diff_time > 1) perfd.toSlowCount++;         
             
            if (diff_time < _TIMERS_HIST_MAX) perfd.perf_buckets[diff_time]++;
            else  perfd.perf_buckets[_TIMERS_HIST_MAX-1]++;
                
            if ((uint16_t) diff_time > perfd.perf_max_time){
                perfd.perf_max_time = (uint16_t) diff_time;
            }	
            
            // Log the performance data at different rates with different log levels so messages can be filtered
            if (perfd.performanceLogTime % 1000 == 0) {
                if ((perfd.perf_buckets[1] > 0) && (perfd.toSlowCount > 0)) errorPercent = (100/(perfd.perf_buckets[1]/perfd.toSlowCount));
                
                if (perfd.performanceLogTime % 30000 == 0) loglvl = TIMERS_LOG_INFO;
                else if (perfd.performanceLogTime % 5000 == 0) loglvl = TIMERS_LOG_DEBUG;
                else loglvl = TIMERS_LOG_TRACE;   
                             
                if (errorPercent == 0) TIMERS_LOG(loglvl, "Timer performance 1mS: %u, 2mS: %u, 3mS: %u, 4mS+: %u || Max: %umS || Slow: Less than 1 percent", perfd.perf_buckets[1], perfd.perf_buckets[2], perfd.perf_buckets[3], perfd.perf_buckets[4], perfd.perf_max_time);  
                else TIMERS_LOG(loglvl, "Timer performance 1mS: %u, 2mS: %u, 3mS: %u, 4mS+: %u || Max: %umS || Slow: %u percent", perfd.perf_buckets[1], perfd.perf_buckets[2], perfd.perf_buckets[3], perfd.perf_buckets[4], perfd.perf_max_time, errorPercent);                      
                if (perfd.performanceLogTime >= 30000) perfd.performanceLogTime = 0;           
            }            
                                      
            return timePassed;
        }   
    #endif  // BUILD_FOR_WINDOWS                                      
            
#endif // TIMERS_IMPLEMENTATION


