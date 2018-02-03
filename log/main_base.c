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


#define LOG_BASE_IMPLEMENTATION
#include "./log_base.h"    

uint8_t test_output_fn(uint8_t b){
    putchar(b);
    putchar(' ');
    return 0;
}

uint8_t test_vprintf(va_list vaargs, const char *fmt, ...){
    va_list args; 
    char buf[200];
    uint8_t i=0;
    buf[0] = '\0';
    
    if (vaargs != 0) vsprintf(buf, fmt, vaargs);
    else {
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
    }
    while (buf[i] != '\0'){
        test_output_fn(buf[i]);
        i++;
    }
    return 0;
}


void logit(const char *fmt, ...){
    static uint8_t firstRun = 1;
    static Log_base logstruct;
    va_list args; 
    
    if (firstRun) {
        log_base_init(&logstruct, &test_vprintf);
        firstRun = 0;
    }
    else {
        va_start(args, fmt);
        logstruct.log(&logstruct, args, 0xab, 1, fmt, 0);
        va_end(args);
    }
}
   



int main()
{

    //logb.log(&logb, 0xab, 1, "%2x blah %2x blah\n", 0x11, 0x22 );
    
    logit("init");
    logit("testd");
    
    
    // example macro to reduce boilerplate
    //#define LOG_WARN(msg, ...) logb.log(&logb, 0xab, 1, msg, __VA_ARGS__)
    //LOG_WARN("piggle %2x \n", 0xa2);
    

    return 0; 
}
