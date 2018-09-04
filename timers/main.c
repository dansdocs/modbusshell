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
    void test_logger(uint8_t level, const char *file, uint16_t line, const char *fmt, ...) {    
        va_list args;
        printf("\n-------------------------\r\n");
        printf("Log level %i in file %s at line number %i with message: \n", level, file, line);
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
        printf("-------------------------\r\n");    
    }
#else
    #pragma message("No log Fn for BUILD_FOR_ARDUINO target... yet")
    void test_logger(uint8_t level, const char *file, uint16_t line, const char *fmt, ...) {        
    }
#endif





int main()
{
	uint8_t tmr1;
    uint8_t tmr2;


    // get a timer, should cause timers.h to log something to its own logger. 
	tmr1 = timers_get_timer(2);      
      
    // Pass in a function to use as a logging function.   
    timers_setLogFn(&test_logger);
    
    // get another timer, now that the logFn has been set timers.h should use that instead. 
	tmr2 = timers_get_timer(2);    // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100 = 100mS
	timers_set_timeout(tmr1, 10);  // 100mS * 10 = 1 second
    timers_set_timeout(tmr2, 20);  // 100mS * 10 = 2 second

	while(1){

        // two different ways to do the same thing. The first checks if the timer is expired then resets it as a dedicated action. 
        // the second checks and resets. 
	    if (timers_check_expired(tmr1)) { 
            printf("timer %i expired\r\n", tmr1); 
            timers_set_timeout(tmr1, 10);
        }
	    if (timers_check_expired_reset(tmr2, 20)) printf("timer %i expired\r\n", tmr2);  
        //printf(".");
		timers_tick();  // needs to be called at least every 1mS
        //for (sleepcycles = 0; sleepcycles<1; sleepcycles++) Sleep(0);  
        //Sleep(0);        
	}
	
    return 0; 
}
