/* Example of using timers.h  */

// TODO: All logging variables to be HEX. Use the h and hh modifiers to provide the type info. 
// working on linux
// working on arduio.

// check that we have a target to build for. 

#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) || defined(BUILD_FOR_ARDUINO)
#else
    // No recognised target. Display a message. Manually set a target. 
    #pragma message("No Target defined. Define one of: BUILD_FOR_WINDOWS, BUILD_FOR_LINUX BUILD_FOR_ARDUINO")
    #define BUILD_FOR_LINUX
#endif


#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
    // stdio.h is not avaialble for arduino
    #include <stdio.h>   // printf
#endif

#include <stdarg.h>  // va_start, va_end 
#include <stdint.h>  // uint8_t etc


        
#define TIMERS_IMPLEMENTATION
#include "./timers.h"

// A simple loging function to pass in to timers.h. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) 
    //void composed_logg(uint8_t fid, uint8_t lvl, const char *fmt, ...);
    //typedef void (*timers_logFnT)(uint8_t, uint8_t, const char *, ...); 
    void test_logger(uint8_t fid, uint8_t lvl, const char *fmt, ...) {    
        va_list args;
        printf("[%02x, %02x] : ", fid, lvl);
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args); 
        putchar('\n'); 
    }
#else
    #pragma message("No log Fn for BUILD_FOR_ARDUINO target... yet")
    void test_logger(uint8_t fid, uint8_t lvl, const char *fmt, ...) {        
    }
#endif

// Infrustructure for logging.  

#define _MAIN_FID ((uint8_t)(('m' << 2) + 't' + 'i')) 
enum {MAIN_LOG_TRACE, MAIN_LOG_DEBUG, MAIN_LOG_INFO, MAIN_LOG_WARN, MAIN_LOG_ERROR, MAIN_LOG_FATAL };    



int main()
{
	uint8_t tmr1;
    uint8_t tmr2;

    test_logger(_MAIN_FID, MAIN_LOG_INFO, "%02x = main.c", _MAIN_FID); 

    // get a timer, should not cause any log output because log fn isn't set. 
	tmr1 = timers_get_timer(2);      
      
    // Pass in a function to use as a logging function.   
    timers_setLogFn(&test_logger);
    
    // get another timer, now that the logFn has been set timers.h should see some output. 
	tmr2 = timers_get_timer(2);    // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100 = 100mS
	timers_set_timeout(tmr1, 10);  // 100mS * 10 = 1 second
    timers_set_timeout(tmr2, 20);  // 100mS * 10 = 2 second

	while(1){

        // two different ways to do the same thing. The first checks if the timer is expired then resets it as a dedicated action. 
        // the second checks and resets. 
	    if (timers_check_expired(tmr1)) { 
             test_logger(_MAIN_FID, MAIN_LOG_INFO, "timer %i expired", tmr1); 
            timers_set_timeout(tmr1, 10);
        }
	    if (timers_check_expired_reset(tmr2, 20)) test_logger(_MAIN_FID, MAIN_LOG_INFO, "timer %i expired", tmr2);  

		timers_tick();  // needs to be called at least every 1mS        
	}
	
    return 0; 
}
