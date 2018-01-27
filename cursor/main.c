/* Example of using cursor.h  */


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

       
#define CURSOR_IMPLEMENTATION
#include "./cursor.h"

// Something to pass into cursor.h and print the data. Its intended that ucprintf would be used. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) 
    #define PRINTFN printf
#else
    #pragma message("No log Fn for BUILD_FOR_ARDUINO target... yet")
#endif




int main()
{
    cursor_hide();
    cursor_clearScreen();
    cursor_gotoXY(1,1);
    printf("hello");
    cursor_gotoXY(1,5);
    printf("world");
    cursor_setSendCmdFn(&PRINTFN);
    cursor_gotoXY(10,1);
    printf("super");
    cursor_gotoXY(20,10);
    printf("position");
    cursor_show();
    return 0; 
}
