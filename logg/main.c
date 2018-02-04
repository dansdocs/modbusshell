/* Example of using logg.h  */

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


#define LOGG_IMPLEMENTATION
#include "./logg.h"    

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

int main()
{    
    logg_init(&test_vprintf);
    logg(0, 0x2a, 1, "testd %2x\n", 0x73);
        
    // example macro to reduce boilerplate for a particular file.
    #define LOGG_WARN(msg, ...) logg(0, 0x2a, 1, msg, __VA_ARGS__)

    LOGG_WARN("piggle poggle %2x\n", 0x4f);

    return 0; 
}
