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


//io
//printf
//log

void composed_log(uint8_t init, const char *fmt){
    static Io_file iof;
    static FILE *f;
    uint8_t i;
    
    if (init){
        f = fopen("test.txt", "a");  
        io_file_init(f, &iof);  
    }
    else {
        for(i=0; fmt[i] != '\0'; i++){
            iof.pushByte(fmt[i], &iof);
        }      
        fclose(f);
    }
}


int main() {
    composed_log(1, "pp");
    composed_log(0, "its alive\n");
   return 0;
}
