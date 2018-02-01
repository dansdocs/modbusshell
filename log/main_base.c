/* Example of using log_base.h  */



// check that we have a target to build for. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) || defined(BUILD_FOR_ARDUINO)
#else
    // No recognised target. Display a message. Manually set a target. 
    #pragma message("No Target defined. Define one of: BUILD_FOR_WINDOWS, BUILD_FOR_LINUX BUILD_FOR_ARDUINO")
    #define BUILD_FOR_WINDOWS
#endif


#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
    // stdio.h is not avaialble for arduino
    #include <stdio.h>   // printf, putchar
    #include <stdint.h>  // uint8_t
    #include <stdarg.h>  // va_list
#endif


uint8_t test_printf(const char *fmt, va_list args){
    vprintf(fmt, args);
    return 0;
}


#define LOG_BASE_IMPLEMENTATION
#include "./log_base.h"       



int main()
{
    Log_base logb;
    
    log_base_init(&logb, &test_printf);
    
    logb.log(&logb, 33, 1, "%2x: blah blah\n", 22);


    return 0; 
}
