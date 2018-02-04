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
    //putchar(' ');
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
    logg_init(&test_vprintf, 5);
    
    #define _MLO_CFID "mlo" 
    #define _MLO_FID ((uint8_t)(('m' << 2) + 'l' + 'o'))
    logg(_MLO_FID, 4, "FileId %s\n", _MLO_CFID);
    
    
    // using logg function directly
    logg(_MLO_FID, 1, "testd %2x\n", 0x73);
        
    // example macro for very minor boilerplate reduction
    #define LOGG(lvl, msg, ...) logg( _MLO_FID, lvl, msg, __VA_ARGS__)

    LOGG(4, "piggle poggle %2x\n", 0x4f);
    
    //block level 4 and 5 messages. 
    logg_setLevel(3);
    
    LOGG(4, "attempt poggle %2x\n", 0x4e);
    LOGG(3, "attempt poggle %2x\n", 0x4c);
    
    //block messages from ID _MLO_FID
    logg_blockId(_MLO_FID);
    LOGG(3, "attempt poggle %2x\n", 0x4b);
    
    

    return 0; 
}
