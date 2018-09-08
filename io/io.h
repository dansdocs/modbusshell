/**
 * Daniel 2018
 *
 *  
**/

#ifndef IO_H
#define IO_H

 
    #include "./io_sock.h" 
    #include "./io_file.h"  
    #ifdef BUILD_FOR_WINDOWS
        #include <stdio.h>
    #endif 
    #ifdef BUILD_FOR_LINUX
        #include <stdio.h>
    #endif 


   typedef struct _io_config_s {
      uint8_t mon_sendByte_to_stdio;      
    } _io_config_s;    
    
    _io_config_s _io_config = {.mon_sendByte_to_stdio = 1};

    uint8_t io_sendByte_mon(uint8_t b);

    // type of function to pass in for log messages. 
    //uint8_t fid, uint8_t lvl, const char *fmt, ...
    typedef void (*io_logFnT)(uint8_t, uint8_t, const char *, ...);    
    void io_setLogFn(io_logFnT fn);

     // type of function to pass in which is then used to get config details
    typedef char* (*io_ConfigFnT)(uint8_t);    
    void io_setConfigFn(io_ConfigFnT fn);

#endif // IO_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_IMPLEMENTATION
#undef IO_IMPLEMENTATION

    #include <stdint.h>  // uint8_t etc
    #include <stdio.h>   // FILE   

    #define IO_SOCK_IMPLEMENTATION
    #include "./io_sock.h"   
    #define IO_FILE_IMPLEMENTATION
    #include "./io_file.h"   

    // Infrustructure for logging.  
    #define _IO_FID ((uint8_t)(('i' << 2) + 'o' + ' '))
    enum {IO_LOG_TRACE, IO_LOG_DEBUG, IO_LOG_INFO, IO_LOG_WARN, IO_LOG_ERROR, IO_LOG_FATAL};    
    void _io_dummylogFn (uint8_t fid, uint8_t lvl, const char *fmt, ...){;}    
    io_logFnT io_logFn = &_io_dummylogFn;
    void io_setLogFn(io_logFnT fn){
        io_logFn = fn;
        io_logFn(_IO_FID, IO_LOG_INFO, "%02x = io/io.h", _IO_FID);         
        io_file_setLogFn(fn);
        io_sock_setLogFn(fn);   
    }     

    // Infrustructure for config
    char* _io_dummyConfigFn (uint8_t in){return "0";}
    io_ConfigFnT io_ConfigFn = &_io_dummyConfigFn;
    void io_setConfigFn(io_ConfigFnT fn){
        io_ConfigFn = fn;
        io_logFn(_IO_FID, IO_LOG_INFO, "Config function passed into io.h");         
    }         

    uint8_t io_sendByte_mon(uint8_t b){
        if (_io_config.mon_sendByte_to_stdio) {
            putchar(b);
        }
        return 0;
    }

   

    void io_sendByte(){
    
    }


#endif // IO_IMPLEMENTATION

