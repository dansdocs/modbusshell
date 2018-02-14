/* Example of using io.h  */


// check that we have a target to build for. 
#define COMPILE_FOR_WINDOWS
//#define COMPILE_FOR_LINUX


#define IO_IMPLEMENTATION
#include "./io.h"




int main() {
    uint8_t bb;
    io_sock_s sockst;
    
    platform_initComs(&sockst, 3000, "127.0.0.1", 0);
    
    while(1){
        if (io_sock_getByte(&sockst, &bb)){
            printf("%c", bb);
        }
        io_sock_sendByte(&sockst, 'p');
    }
    
    return 0;
}
