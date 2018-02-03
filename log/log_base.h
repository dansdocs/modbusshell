/**
 * 
 * Daniel 2018
 * 
 * Simple logging - redirect the output by providing a function with a printf style signature. 
 * 
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - but one and only onec file, just before #include "./log_base.h" also put in #define LOG_BASE_IMPLEMENTATION 
 * You can #include "./log_base.h" in other c files as normal. 
 * 
 */

#ifndef LOG_BASE_H
#define LOG_BASE_H

    #include <stdarg.h>  // va_list

    typedef uint8_t (*Log_base_vprintf)(va_list args, const char *fmt, ...);

    // Underscores mean its regarded as private and shouldn't be used outside this file. 
    #define _LOG_BASE_NUMFILES 10

    typedef struct Log_base {
        uint8_t _fid;                      // numeric   file identifier for this file
        char *_cfid;                       // character file identifier for this file
        uint8_t _lvl;                      // everything below this will be sent out. 
        uint8_t _fids[_LOG_BASE_NUMFILES]; // list of fids to exclude from printing. 
        Log_base_vprintf _vprintf;         // a function with similar to combined printf and vprintf 
        uint8_t (*log)(struct Log_base*, va_list, uint8_t, uint8_t, const char*, ...);  // function external things will use to log things.         
    } Log_base;


#endif // LOG_BASE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef LOG_BASE_IMPLEMENTATION
#undef LOG_BASE_IMPLEMENTATION

    uint8_t _log_log (struct Log_base *s, va_list vaargs, uint8_t fid, uint8_t lvl, const char *fmt, ...){
        uint8_t i;    
        va_list args;
              
        if (lvl <= s-> _lvl){
            for (i=0; i<_LOG_BASE_NUMFILES; i++) if (fid == s-> _fids[i]) return 0;
            s-> _vprintf(0, "%2x: ", fid);
            if (vaargs != 0) s-> _vprintf(vaargs, fmt, 0);
            else {
                va_start(args, fmt);
                s-> _vprintf(args, fmt, 0);
                va_end(args);
            }
        }
        return 0;
    }


    void log_base_init(Log_base *s, Log_base_vprintf vpfn){
        uint8_t i;
        
        s-> _cfid = "lob";  
        s-> _fid = (s-> _cfid[0] << 2) + s-> _cfid[1] + s-> _cfid[2];  
        s-> _vprintf = vpfn;
        s -> log = &_log_log;
        for (i=0; i<10; i++) s-> _fids[i] = 0;
        s-> _lvl = 5; 
        s-> log(s, 0, s-> _fid, 1, "FileId: %s\n", s-> _cfid);
    }

#endif // LOG_BASE_IMPLEMENTATION

