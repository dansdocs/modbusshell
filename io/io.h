/**
 * Daniel 2018
 *
 *  
**/

#ifndef IO_H
#define IO_H

    #include <stdint.h>  // uint8_t etc
    #include <stdio.h>   // FILE    
    
    typedef struct Io_file {
        FILE *_f;
        uint8_t (*pushByte)(uint8_t, struct Io_file*);
    } Io_file;
    
        
#endif // IO_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_IMPLEMENTATION
#undef IO_IMPLEMENTATION

    uint8_t _io_file_pushByte(uint8_t b, Io_file *self){
        fputc (b, self->_f);
        return 1;
    }

    void io_file_init(FILE *f, Io_file *o){
        o->pushByte = &_io_file_pushByte;
        o->_f = f;
    }  

#endif // IO_IMPLEMENTATION

