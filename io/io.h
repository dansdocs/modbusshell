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

    void io_sendByte_mon(uint8_t b);

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


    void io_sendByte_mon(uint8_t b){
        if (_io_config.mon_sendByte_to_stdio) putchar(b);    
    }

   

    void io_sendByte(){
    
    }


#endif // IO_IMPLEMENTATION

