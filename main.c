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

int main()
{
	// -------config test
	//int i;
    //config_setLogFn(&log_log);
	//for (i=0; i<CONFIG_MAX; i++) printf("%s\n", config_get(i));

    // using logg function directly
    logg(0, _MAI_FID, 1, &printf, "testd %2x\n", 0x73);
	
	// ------- timers test
	uint8_t tmr;
	tmr = timers_get_timer(2);  // 1mS base tick rate. diviser is 2^10 = 100 so update rate is 1mS * 100
	printf("Timer index: %i\n", tmr);
	if (timers_check_expired(tmr)) printf("timer expired\n");
	timers_set_timeout(tmr, 30);
	printf("\nhere i am before\n");	
	while(1){
		timers_tick();	
	    if (timers_check_expired(tmr)) {
			printf("\nhere i am\n");
		    printf("timer expired\n");
		    timers_set_timeout(tmr, 30);
	    }
	}
	

    return 0; 
}
