/* Cross platform modbus shell. Uses sockets on windows and linux
   serial port on arduino.  */
   
   
#define COMPILE_FOR_WINDOWS

// -------compose the logging functionality 
#define LOGG_IMPLEMENTATION
#include "./logg/logg.h"
#define UCPRINTF_IMPLEMENTATION
#include "./ucprintf/ucprintf.h"
#define IO_IMPLEMENTATION
#include "./io/io.h"

// componse vprinf by wrapping it with the relevent io
uint8_t composed_vprintf(va_list vaargs, const char *fmt, ...){
    va_list args; 
    if (vaargs != 0) ucprintf_vprintf(&io_sendByte_mon, fmt, vaargs);
    else { va_start(args, fmt); ucprintf_vprintf(&io_sendByte_mon, fmt, args); va_end(args);}
    return 0;
}

// compose logg by wrapping it with the wrapped vprintf   
void composed_logg(uint8_t fid, uint8_t lvl, const char *fmt, ...){
    va_list args; 
    va_start(args, fmt); logg(args, fid, lvl, &composed_vprintf, fmt); va_end(args);
    io_sendByte_mon('\n');
}


// -------config test
//#define CONFIG_IMPLEMENTATION
//#include "./config.h"

// -------timers test
#define TIMERS_IMPLEMENTATION
#include "./timers/timers.h"




#define _MAIN_FID ((uint8_t)(('m' << 2) + 'a' + 'i'))
enum {MAIN_LOG_TRACE, MAIN_LOG_DEBUG, MAIN_LOG_INFO, MAIN_LOG_WARN, MAIN_LOG_ERROR, MAIN_LOG_FATAL };    



int main()
{
	// -------config test
	//int i;
    //config_setLogFn(&log_log);
	//for (i=0; i<CONFIG_MAX; i++) printf("%s\n", config_get(i));

    composed_logg(_MAIN_FID, MAIN_LOG_INFO, "%02x = main.c", _MAIN_FID);    
    timers_setLogFn(&composed_logg);


	// ------- timers test
	uint8_t tmr;
	tmr = timers_get_timer(2);  // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100
	composed_logg(_MAIN_FID, MAIN_LOG_INFO, "Timer index: %2u", tmr);
	timers_set_timeout(tmr, 30);
	while(1){
		timers_tick();	
	    if (timers_check_expired(tmr)) {
		    composed_logg(_MAIN_FID, MAIN_LOG_INFO, "timer expired");
		    timers_set_timeout(tmr, 30);
	    }
	}
    return 0; 
}
