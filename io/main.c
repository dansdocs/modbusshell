/* Example of using io.h  */


// check that we have a target to build for. 
#define COMPILE_FOR_WINDOWS
//#define COMPILE_FOR_LINUX


#define IO_IMPLEMENTATION
#include "./io.h"




int main() {
    uint8_t bb;
    
    platform_initComs(3000, "", 0);
    
    printf("hello main\n");
    while(1){
        if (platform_getByte(&bb)){
            printf("%c", bb);
        }
    }
    
    return 0;
}
