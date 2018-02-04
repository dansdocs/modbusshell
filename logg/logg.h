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

    // function to pass in which will be used to print out messages. The approach is if vaargs is zero then parameters
    // from ... are used.  If vaargs is not zero then it will be used and ... won't be. 
    typedef uint8_t (*Logg_vprintf)(va_list vaargs, const char *fmt, ...);
    
    /// the function used by others to log messages. 
    uint8_t logg(va_list vaargs, uint8_t fid, uint8_t lvl, const char *fmt, ...);
    
    // call once to initialise the logging
    void logg_init(Logg_vprintf vpfn);

#endif // LOGG_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef LOGG_IMPLEMENTATION
#undef LOGG_IMPLEMENTATION

    // Underscores mean its regarded as private and shouldn't be used outside this file. 
    #define _LOGG_MAX_FILES 10
    #define _LOGG_CFID "lob" 
    #define _LOGG_FID ((uint8_t)(('l' << 2) + 'o' + 'b'))
    Logg_vprintf _logg_vprintf;
    uint8_t _logg_lvl; // everything below this will be sent out. 
    uint8_t _logg_fids[_LOGG_MAX_FILES]; // list of fids to exclude from printing.  

    uint8_t logg (va_list vaargs, uint8_t fid, uint8_t lvl, const char *fmt, ...){
        uint8_t i;    
        va_list args;
              
        if (lvl <= _logg_lvl){
            for (i=0; i<_LOGG_MAX_FILES; i++) if (fid == _logg_fids[i]) return 0;
            _logg_vprintf(0, "%2x: ", fid);
            if (vaargs != 0) _logg_vprintf(vaargs, fmt, 0);
            else {
                va_start(args, fmt);
                _logg_vprintf(args, fmt, 0);
                va_end(args);
            }
        }
        return 0;
    }


    void logg_init(Logg_vprintf vpfn){
        uint8_t i;
        
        _logg_vprintf = vpfn;
        for (i=0; i<_LOGG_MAX_FILES; i++) _logg_fids[i] = 0;
        _logg_lvl = 5; 
        logg(0, _LOGG_FID, 1, "FileId: %s\n", _LOGG_CFID);
    }

#endif // LOGG_IMPLEMENTATION

