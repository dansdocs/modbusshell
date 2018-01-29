/* Example of using io.h  */


// check that we have a target to build for. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) || defined(BUILD_FOR_ARDUINO)
#else
    // No recognised target. Display a message. Manually set a target. 
    #pragma message("No Target defined. Define one of: BUILD_FOR_WINDOWS, BUILD_FOR_LINUX BUILD_FOR_ARDUINO")
    #define BUILD_FOR_LINUX
#endif


#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
    // stdio.h is not avaialble for arduino
    #include <stdio.h>   // printf, putchar
#endif

#define IO_IMPLEMENTATION
#include "./io.h"




int main() {
   FILE *f;
   f = fopen("test.txt", "a");    
   
   Io_file s;
   io_file_init(f, &s);
   
   s.pushByte('H', &s);
   
   fclose(f);
   return 0;
}
