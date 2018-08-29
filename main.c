/* Cross platform modbus shell. Uses sockets on windows and linux
   serial port on arduino.  */
   
   
   #define COMPILE_FOR_WINDOWS

// -------log test
#define LOGG_IMPLEMENTATION
#include "./logg/logg.h"

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
