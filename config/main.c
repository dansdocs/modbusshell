/* Example of using config.h  */



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


        
#define CONFIG_IMPLEMENTATION
#include "./config.h"

// A simple loging function to pass in to config.h. 
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

#define _MAIN_FID ((uint8_t)(('m' << 2) + 'c' + 'o')) 
enum {MAIN_LOG_TRACE, MAIN_LOG_DEBUG, MAIN_LOG_INFO, MAIN_LOG_WARN, MAIN_LOG_ERROR, MAIN_LOG_FATAL };    



int main()
{
    char *s;

    config_setLogFn(&test_logger);
    test_logger(_MAIN_FID, MAIN_LOG_INFO, "%02x = config/main.c", _MAIN_FID); 
    s = config_get(CONFIG_PORT);
    test_logger(_MAIN_FID, MAIN_LOG_INFO, "Value: %s", s); 
	
    return 0; 
}
