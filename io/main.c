/* Example of using io.h  */


// check that we have a target to build for. 
//#define BUILD_FOR_WINDOWS
//#define BUILD_FOR_LINUX


#define IO_IMPLEMENTATION
#include "./io.h"

#ifdef BUILD_FOR_WINDOWS
    #include <windows.h>
    #include <stdio.h>
#else
    #include <unistd.h>
    #include <stdarg.h>  // va_start, va_end     
    #include <stdio.h>    
#endif

void wait( int seconds )
{   // Pretty crossplatform, both ALL POSIX compliant systems AND Windows
    #ifdef BUILD_FOR_WINDOWS
        Sleep( 1000 * seconds );
    #else
        sleep( seconds );
    #endif
}

  
void testlogfn(uint8_t fid, uint8_t lvl, const char *fmt, ...){
    va_list args; 
    char buf[200];
    uint8_t i=0;
    buf[0] = '\0';
        
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    printf("%2x: ", fid);
    while (buf[i] != '\0'){      
         putchar(buf[i]);
         i++;
    }
}



int main() {
    uint8_t b1;
    uint8_t b2;
  //  uint8_t b3;
  //  uint8_t b4;
    io_sock_s sock1;
    io_sock_s sock2;
   // io_sock_s sock3;
   // io_sock_s sock4;
   
    io_sock_set_loggfn(&testlogfn);
     
    io_sock_initComs(&sock1, 3000, "");
    io_sock_initComs(&sock2, 3001, "");
  //  io_sock_initComs(&sock3, 3002, "");
  //  io_sock_initComs(&sock4, 3003, "");
  
   io_file_sendByte('c');

    while(1){
        if (io_sock_getByte(&sock1, &b1)){
            printf("%c", b1);
        }
 
        if (io_sock_getByte(&sock2, &b2)){
            printf("%c", b2);
        }
        
   //     if (io_sock_getByte(&sock3, &b3)){
   //         printf("%c", b3);
   //     }
   //  
   //     if (io_sock_getByte(&sock4, &b4)){
   //         printf("%c", b4);
   //     }
                        
        io_sock_sendByte(&sock1, '1');
        io_sock_sendByte(&sock2, '2');   
   //     io_sock_sendByte(&sock3, '3');
   //     io_sock_sendByte(&sock4, '4');  
        wait(1);  
        
          

    }
    
    return 0;
}
