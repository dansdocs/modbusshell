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
    
    // the function used by others to log messages. 
    uint8_t logg(uint8_t fid, uint8_t lvl, uint8_t handle, const char *fmt, ...);
    
    // call once to initialise the logging and pass in the call back function
    // returns a handle which is used in the logg function. This lets you
    // send data to different places. 
    uint8_t logg_init(Logg_vprintf vpfn, uint8_t lvl);
    
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
    Logg_vprintf _logg_vprintf[_LOGGMAXFN];
    uint8_t _logg_i = 0;
    uint8_t _logg_lvl; // everything below this will be sent out. 
    uint8_t _logg_fids[_LOGG_MAX_FILES]; // list of fids to exclude from printing.  
    uint8_t _logg_fid_index;


    uint8_t logg (uint8_t fid, uint8_t lvl, uint8_t handle, const char *fmt, ...){
        uint8_t i;    
        va_list args;
              
        if (lvl <= _logg_lvl){
            for (i=0; i<_LOGG_MAX_FILES; i++) if (fid == _logg_fids[i]) return 0;
            _logg_vprintf[handle](0, "%2x: ", fid);

            va_start(args, fmt);
            _logg_vprintf[handle](args, fmt);
            va_end(args);
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
    

    uint8_t logg_init(Logg_vprintf vpfn, uint8_t lvl){
        _logg_fid_index = 0;
        _logg_vprintf[_logg_i] = vpfn;
        if (_logg_i+1 < _LOGGMAXFN) _logg_i++;
        _logg_lvl = lvl; 
        if (_logg_i == 1) logg(_LOGG_FID, 4, 0, "FileId %s\n", _LOGG_CFID);
        return _logg_i-1;
    }

#endif // LOGG_IMPLEMENTATION

