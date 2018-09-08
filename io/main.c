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

  
// A simple loging function to pass in to io.h. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) 
    //void composed_logg(uint8_t fid, uint8_t lvl, const char *fmt, ...);
    //typedef void (*timers_logFnT)(uint8_t, uint8_t, const char *, ...); 
    void test_logger(uint8_t fid, uint8_t lvl, const char *fmt, ...) {    
        va_list args;
        printf("[%02x, %02x] : ", fid, lvl);
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args); 
        putchar('\n'); 
    }
#else
    #pragma message("No log Fn for BUILD_FOR_ARDUINO target... yet")
    void test_logger(uint8_t fid, uint8_t lvl, const char *fmt, ...) {        
    }
#endif

// Infrustructure for logging.  

#define _MAIN_FID ((uint8_t)(('m' << 2) + 'i' + 'o')) 
enum {MAIN_LOG_TRACE, MAIN_LOG_DEBUG, MAIN_LOG_INFO, MAIN_LOG_WARN, MAIN_LOG_ERROR, MAIN_LOG_FATAL };    

enum {CONFIG_PORT, CONFIG_ADDRESS, CONFIG_LOGSTDERROR, CONFIG_LOGFILE, CONFIG_LOGLEVEL};
char* test_config(uint8_t idx){
    if (idx == CONFIG_PORT) return "3000";
    else return "0";
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

    io_file_sendByte('c');
    io_sendByte_mon('a'); io_sendByte_mon('\n');

     test_logger(_MAIN_FID, MAIN_LOG_INFO, "%02x = io/main.c", _MAIN_FID); 
     io_setLogFn(&test_logger);
     io_setConfigFn(&test_config);
     
    io_sock_initComs(&sock1, 3000, "");
    io_sock_initComs(&sock2, 3001, "");
  //  io_sock_initComs(&sock3, 3002, "");
  //  io_sock_initComs(&sock4, 3003, "");

  
    while(1){
        if (io_sock_getByte(&sock1, &b1)){
            test_logger(_MAIN_FID, MAIN_LOG_INFO, "Socket1 received character: %c", b1);
        }
 
        if (io_sock_getByte(&sock2, &b2)){
            test_logger(_MAIN_FID, MAIN_LOG_INFO, "Socket2 received character: %c", b2);
        }
        
   //     if (io_sock_getByte(&sock3, &b3)){
   //         test_logger(_MAIN_FID, MAIN_LOG_INFO, "Socket3 received character: %c", b3);
   //     }
   //  
   //     if (io_sock_getByte(&sock4, &b4)){
   //         test_logger(_MAIN_FID, MAIN_LOG_INFO, "Socket4 received character: %c", b4);
   //     }
                        
        io_sock_sendByte(&sock1, '1');
        io_sock_sendByte(&sock2, '2');   
   //     io_sock_sendByte(&sock3, '3');
   //     io_sock_sendByte(&sock4, '4');  
   
        
        wait(1);  
        
          

    }
    
    return 0;
}
