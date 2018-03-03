/**
 * Daniel 2018
 *
 *  
**/

#ifndef IO_H
#define IO_H

 
    #include "./io_sock.h" 
    #include "./io_file.h"  
    
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

#endif // IO_IMPLEMENTATION

