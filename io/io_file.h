/**
 * Daniel 2018
 *
 * file i/o 
 * 
 * Todo: take in config filename.
 *  
**/

#ifndef IO_FILE_H
#define IO_FILE_H


    #ifdef BUILD_FOR_WINDOWS
    #endif  
    #ifdef BUILD_FOR_LINUX
    #endif  
    
    // type of function to pass in for log messages. 
    //uint8_t fid, uint8_t lvl, const char *fmt, ...
    typedef void (*io_file_logFnT)(uint8_t, uint8_t, const char *, ...);    
    void io_file_setLogFn(io_file_logFnT fn);

    uint8_t io_file_sendByte(uint8_t txByte);
        
#endif // IO_FILE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_FILE_IMPLEMENTATION
#undef IO_FILE_IMPLEMENTATION

    #include <stdio.h>

    // Infrustructure for logging.  
    #define _IO_FILE_FID ((uint8_t)(('i' << 2) + 'o' + 'f'))
    enum { IO_FILE_LOG_TRACE, IO_FILE_LOG_DEBUG, IO_FILE_LOG_INFO, IO_FILE_LOG_WARN, IO_FILE_LOG_ERROR, IO_FILE_LOG_FATAL };    
    void _io_file_dummylogFn (uint8_t fid, uint8_t lvl, const char *fmt, ...){;}    
    io_file_logFnT io_file_logFn = &_io_file_dummylogFn;
    void io_file_setLogFn(io_file_logFnT fn){
        io_file_logFn = fn;
        io_file_logFn(_IO_FILE_FID, IO_FILE_LOG_INFO, "%02x = io/io_file.h", _IO_FILE_FID);    
    }  

    FILE *_io_file_fp;
    
    uint8_t io_file_sendByte(uint8_t txByte){
        static uint8_t first = 1;
        
        if (first) {
            _io_file_fp = fopen("./log.txt", "a");
            first = 0;
        }

        fputc(txByte, _io_file_fp);
        fflush (_io_file_fp);

        return 0;
    }

#endif // IO_FILE_IMPLEMENTATION

