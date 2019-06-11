/* Cross platform modbus shell. Uses sockets on windows and linux
   serial port on arduino.  */
   
   
#define COMPILE_FOR_WINDOWS

// -------compose the logging functionality from the parts
#define LOGG_IMPLEMENTATION
#include "./logg/logg.h"
#define UCPRINTF_IMPLEMENTATION
#include "./ucprintf/ucprintf.h"
#define IO_IMPLEMENTATION
#include "./io/io.h"

// compose vprinf by wrapping it with the relevent io
uint8_t composed_vprintf(va_list vaargs, const char *fmt, ...){
    va_list args; 
    if (vaargs != 0) ucprintf_vprintf(&io_sendByte_mon, fmt, vaargs);
    else { va_start(args, fmt); ucprintf_vprintf(&io_sendByte_mon, fmt, args); va_end(args);}
    return 0;
}

// compose logg by wrapping it with the wrapped vprintf this is the logg function provided to 
// everything to use. Changing io_sendByte_mon lets you redirect all monitoring to one or more places.    
void composed_logg(uint8_t fid, uint8_t lvl, const char *fmt, ...){
    va_list args; 
    va_start(args, fmt); logg(args, fid, lvl, &composed_vprintf, fmt); va_end(args);
    io_sendByte_mon('\n');
}


// -------config 
#define CONFIG_IMPLEMENTATION
#include "./config/config.h"

// -------timers 
#define TIMERS_IMPLEMENTATION
#include "./timers/timers.h"

#define _MAIN_FID ((uint8_t)(('m' << 2) + 'a' + 'i'))
enum {MAIN_LOG_TRACE, MAIN_LOG_DEBUG, MAIN_LOG_INFO, MAIN_LOG_WARN, MAIN_LOG_ERROR, MAIN_LOG_FATAL };    

int main()
{
	int i;

    config_setLogFn(&composed_logg);
    composed_logg(_MAIN_FID, MAIN_LOG_INFO, "%02x = main.c", _MAIN_FID);    
    timers_setLogFn(&composed_logg);
    io_setLogFn(&composed_logg);
    io_setConfigFn(&config_get);

    // ------- write out the config
	for (i=0; i<CONFIG_MAX; i++) composed_logg(_MAIN_FID, MAIN_LOG_TRACE, "Config Entry: %s", config_get(i));

	// ------- timers test
	uint8_t tmr;
    composed_logg(_MAIN_FID, MAIN_LOG_INFO, "Requesting timer");
	tmr = timers_get_timer(2);  // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100
	composed_logg(_MAIN_FID, MAIN_LOG_INFO, "Timer, index: %2u", tmr);
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
