/* Example of using cursor.h  */


// Have only one of the following uncommented. TEST_STAND_ALONE only uses std libs
// outside of cursor.h. TEST_UCPRINTF_INTEGRATION brings in ucprinf to test basic interation
//#define TEST_STAND_ALONE
#define TEST_UCPRINTF_INTEGRATION


// check that we have a target to build for. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) || defined(BUILD_FOR_ARDUINO)
#else
    // No recognised target. Display a message. Manually set a target. 
    #pragma message("No Target defined. Define one of: BUILD_FOR_WINDOWS, BUILD_FOR_LINUX BUILD_FOR_ARDUINO")
    #define BUILD_FOR_LINUX
#endif


#ifdef TEST_STAND_ALONE
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
        #include <stdio.h>   // printf
    #else
        #pragma message("stdio.h is not avaialble for arduino, so no standalone arduino test. ")        
    #endif

    // Provide the standard printf function to cursor.h    
    #define print_it printf
#endif

#ifdef TEST_UCPRINTF_INTEGRATION

    #define UCPRINTF_IMPLEMENTATION
    #include "../ucprintf/ucprintf.h"
    
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
        #include <stdio.h>   // putchar
        
        // an example stub that sends a byte to stdout  
        uint8_t sendByteExample(uint8_t c) {
                putchar(c);
            return 1;
        }
        
        // Create a wrapper function that uses sendByteExample and has the 
        // same signature as printf. 
        ucprintf_CREATE_WRAPPED_FN(print_it, sendByteExample)        
        
    #else
        #pragma message("stdio.h is not avaialble for arduino, so no arduino test yet. ")        
    #endif

#endif

       
#define CURSOR_IMPLEMENTATION
#include "./cursor.h"



int main()
{
    cursor_hide();
    cursor_clearScreen();
    cursor_gotoXY(1,1);
    printf("hello");
    cursor_gotoXY(1,5);
    printf("world");
    cursor_setSendCmdFn(&print_it);
    cursor_gotoXY(10,1);
    printf("super");
    cursor_gotoXY(20,10);
    printf("position");
    cursor_show();
    return 0; 
}
