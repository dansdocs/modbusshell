/**
 * Daniel 2018
 *
 * file i/o 
 *  
**/

#ifndef IO_FILE_H
#define IO_FILE_H


    #ifdef COMPILE_FOR_WINDOWS
    #endif  
    #ifdef COMPILE_FOR_LINUX
    #endif  
    #include <stdint.h>
    #include <stdio.h>
    
    // type of function to pass in for log messages. 
    //uint8_t fid, uint8_t lvl, const char *fmt, ...
    typedef void (*io_file_loggfnt)(uint8_t, uint8_t, const char *, ...);
    
    void io_file_set_loggfn(io_file_loggfnt fn);
        
#endif // IO_FILE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_FILE_IMPLEMENTATION
#undef IO_FILE_IMPLEMENTATION

    io_file_loggfnt _io_file_logfn;
    #define _IO_FILE_CFID "ios" 
    uint8_t _io_file_fid = ((uint8_t)(('i' << 2) + 'o' + 's'));
  
    void io_file_set_loggfn(io_file_loggfnt fn){
        _io_file_logfn = fn;
        _io_file_logfn(_io_file_fid, 4, "FileId %s\n", _IO_FILE_CFID);
    }
  
    uint8_t io_file_getByte(io_file_s *s, uint8_t *rxByte){

        static uint8_t first = 1;
        
        if (first) {
            _io_file_logfn(_io_file_fid, 4, "Connected\n");
            first = 0;
        }
      

    }
  
    uint8_t io_file_sendByte(io_file_s *s, uint8_t txByte){

    }

#endif // IO_FILE_IMPLEMENTATION

