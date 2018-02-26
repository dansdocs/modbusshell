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

//------------------------------------------------

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

//------------------------------------------------

uint8_t test_output_fn2(uint8_t b){
    putchar(b);
    if  (b != '\n') putchar('.');
    return 0;
}

uint8_t test_vprintf2(va_list vaargs, const char *fmt, ...){
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
        test_output_fn2(buf[i]);
        i++;
    }
    return 0;
}

//------------------------------------------------

// example macro for very minor boilerplate reduction here 
#define LOGG(lvl, msg, ...) logg(0, _MLO_FID, lvl, test_vprintf, msg, __VA_ARGS__)

//------------------------------------------------


// this example bonds (wraps) the output function to the logg, similar to the macro 
// but you have a function so can pass on a function pointer about.  
void wrapped_logg_example(uint8_t fid, uint8_t lvl, const char *fmt, ...){
    va_list args;

    va_start(args, fmt);     
    logg(args, fid, lvl, &test_vprintf, fmt);
    va_end(args);
}

int main()
{        
    #define _MLO_CFID "mlo" 
    #define _MLO_FID ((uint8_t)(('m' << 2) + 'l' + 'o'))
    logg(0, _MLO_FID, 4, &test_vprintf, "FileId %s\n", _MLO_CFID);
        
    // using logg function directly
    logg(0, _MLO_FID, 1, &test_vprintf, "testd %2x\n", 0x73);
    
    // using logg function directly with the other test_vprintf2
    logg(0, _MLO_FID, 1, &test_vprintf2, "testd other test_vprintf2 %2x\n", 0x71); 
            
    LOGG(4, "piggle poggle33 %2x\n", 0x4f);
    
    //block level 4 and 5 messages. 
    logg_setLevel(3);
    
    LOGG(4, "attempt poggle %2x\n", 0x4e);
    LOGG(3, "attempt poggle %2x\n", 0x4c);
    
    //block messages from ID _MLO_FID
    logg_blockId(_MLO_FID);
    LOGG(3, "attempt poggle %2x\n", 0x4b);
    
    wrapped_logg_example(0x03, 2, "wrapped test - test param: %2x\n", 0x6B);
    
    return 0; 
}
