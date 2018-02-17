/* Example of using io.h  */


// check that we have a target to build for. 
#define COMPILE_FOR_WINDOWS
//#define COMPILE_FOR_LINUX


#define IO_IMPLEMENTATION
#include "./io.h"

#ifdef COMPILE_FOR_WINDOWS
    #include <windows.h>
#else
    #include <unistd.h>
#endif

void wait( int seconds )
{   // Pretty crossplatform, both ALL POSIX compliant systems AND Windows
    #ifdef COMPILE_FOR_WINDOWS
        Sleep( 1000 * seconds );
    #else
        sleep( seconds );
    #endif
}




int main() {
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;
    io_sock_s sock1;
    io_sock_s sock2;
    io_sock_s sock3;
    io_sock_s sock4;
    
    platform_initComs(&sock1, 3000, "", 0);
    platform_initComs(&sock2, 3001, "", 0);
    platform_initComs(&sock3, 3002, "", 0);
    platform_initComs(&sock4, 3003, "", 0);

    while(1){
        if (io_sock_getByte(&sock1, &b1)){
            printf("%c", b1);
        }
 
        if (io_sock_getByte(&sock2, &b2)){
            printf("%c", b2);
        }
        
        if (io_sock_getByte(&sock3, &b3)){
            printf("%c", b3);
        }
     
        if (io_sock_getByte(&sock4, &b4)){
            printf("%c", b4);
        }
                        
        io_sock_sendByte(&sock1, '1');
        io_sock_sendByte(&sock2, '2');   
        io_sock_sendByte(&sock3, '3');
        io_sock_sendByte(&sock4, '4');  
        wait(1);  
        
          

    }
    
    return 0;
}
