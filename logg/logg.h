/**
 * 
 * Daniel 2018
 * 
 * Simple logging - redirect the output by providing a function with a printf style-ish signature. 
 * 
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - but one and only onec file, just before #include "./logg.h" also put in #define LOG_IMPLEMENTATION 
 * You can #include "./logg.h" in other c files as normal. 
 * 
 */

#ifndef LOGG_H
#define LOGG_H

    #include <stdarg.h>  // va_list
    #include <stdint.h>  // uint8_t etc

    // function to pass in which will be used to print out messages. The approach is if vaargs is zero then parameters
    // from ... are used.  If vaargs is not zero then it will be used and ... won't be. 
    typedef uint8_t (*Logg_vprintf)(va_list vaargs, const char *fmt, ...);
    
    // the function used by others to log messages (or wrap the function) 
    uint8_t logg(va_list vaargs, uint8_t fid, uint8_t lvl, Logg_vprintf pfn, const char *fmt, ...);
        
    // Adding an ID will mean logg messages with that ID won't come through.
    void logg_blockId(uint8_t id);
    
    // Only messages with a priority level lower than lvl will get through. Max 5.
    void logg_setLevel(uint8_t lvl);

#endif // LOGG_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef LOGG_IMPLEMENTATION
#undef LOGG_IMPLEMENTATION

    // Underscores mean its regarded as private and shouldn't be used outside this file. 
    #define _LOGG_MAX_FILES 10
    #define _LOGG_CFID "log" 
    #define _LOGG_FID ((uint8_t)(('l' << 2) + 'o' + 'g'))
    #define _LOGGMAXFN 5
    uint8_t _logg_i = 0;
    uint8_t _logg_lvl = 5; // everything below this will be sent out. 
    uint8_t _logg_fids[_LOGG_MAX_FILES]; // list of fids to exclude from printing.  
    uint8_t _logg_fid_index = 0;


    uint8_t logg (va_list vaargs, uint8_t fid, uint8_t lvl, Logg_vprintf pfn, const char *fmt, ...){
        uint8_t i;    
        va_list args;
              
        if (lvl <= _logg_lvl){
            for (i=0; i<_LOGG_MAX_FILES; i++) if (fid == _logg_fids[i]) return 0;
            pfn(0, "[%02x, %02x] : ", fid, lvl);
            if (vaargs == 0){
                va_start(args, fmt);
                pfn(args, fmt);
                va_end(args);
            }
            else pfn(vaargs, fmt);
            
        }
        return 0;
    }
    
    void logg_blockId(uint8_t id){
        if (_logg_fid_index < _LOGG_MAX_FILES) {
            _logg_fids[_logg_fid_index] = id;
            _logg_fid_index++;
        }
    }
    
    void logg_setLevel(uint8_t lvl){
        _logg_lvl = lvl;
    }    
    

#endif // LOGG_IMPLEMENTATION

