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
    
    
    uint8_t io_file_sendByte(uint8_t txByte);
        
#endif // IO_FILE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_FILE_IMPLEMENTATION
#undef IO_FILE_IMPLEMENTATION

    #include <stdio.h>
    #define _IO_FILE_CFID "iof" 
    uint8_t _io_file_fid = ((uint8_t)(('i' << 2) + 'o' + 'f'));

    FILE *_io_file_fp;

    
    uint8_t io_file_sendByte(uint8_t txByte){
        static uint8_t first = 1;
        printf("here i am\r\n");
        
        if (first) {
            _io_file_fp = fopen("./log.txt", "a");
            fprintf(_io_file_fp, "%2x: io_file.h\n", _io_file_fid);
            //fclose(_io_file_fp);
            first = 0;
        }

        fputc(txByte, _io_file_fp);
        fflush (_io_file_fp);

        return 0;
    }

#endif // IO_FILE_IMPLEMENTATION

