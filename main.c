/* Cross platform modbus shell. Uses sockets on windows and linux
   serial port on arduino.  */
   
   
#define COMPILE_FOR_WINDOWS

// -------compose logging functionality 
#define LOGG_IMPLEMENTATION
#include "./logg/logg.h"
#define UCPRINTF_IMPLEMENTATION
#include "./ucprintf/ucprintf.h"
#define IO_IMPLEMENTATION
#include "./io/io.h"
    //int ucprintf_vprintf(ucprintf_sendByteFnT fn, const char *fmt, va_list args);

    // an example stub that sends a byte to stdout, if putchar is available.  
uint8_t sendByteExample(uint8_t c) {
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)    
        putchar(c);
    #endif 
    return 1;
}
int op_fn_for_logg(va_list vaargs, const char *fmt, ...){
    va_list args; 

    if (vaargs != 0) ucprintf_vprintf(&sendByteExample, fmt, vaargs);
    else {
        va_start(args, fmt);
        ucprintf_vprintf(&sendByteExample, fmt, args);
        va_end(args);
    }
    return 0;

}

// -------config test
//#define CONFIG_IMPLEMENTATION
//#include "./config.h"

// -------timers test
#define TIMERS_IMPLEMENTATION
#include "./timers/timers.h"




#define _MAI_FID ((uint8_t)(('m' << 2) + 'a' + 'i'))

uint8_t locvprintf(va_list vaargs, const char *fmt, ...){
    va_list args; 
    char buf[200];
    uint8_t i=0;
    buf[0] = '\0';
    
    if (vaargs != 0) vsprintf(buf, fmt, vaargs);
    else {
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
    }
    while (buf[i] != '\0'){
        putchar(buf[i]);
        i++;
    }
    return 0;
}


int main()
{
	// -------config test
	//int i;
    //config_setLogFn(&log_log);
	//for (i=0; i<CONFIG_MAX; i++) printf("%s\n", config_get(i));

    // using logg function directly
    logg(0, _MAI_FID, 1, &locvprintf, "testd %2x\n", 0x73);
	
	// ------- timers test
	uint8_t tmr;
	tmr = timers_get_timer(2);  // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100
	printf("Timer index: %i\n", tmr);
	timers_set_timeout(tmr, 30);
	while(1){
		timers_tick();	
	    if (timers_check_expired(tmr)) {
		    printf("timer expired\n");
		    timers_set_timeout(tmr, 30);
	    }
	}
    return 0; 
}
