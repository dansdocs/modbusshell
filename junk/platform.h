
/*********************************************************
* Even out differences between different targets
* Daniel Burke 2017
*********************************************************/

#ifndef platform_h
#define platform_h

// uncomment only one of the following - or pass 
// the -D flag into gcc to define which arch to compile for. 
// gcc ./main.c ./timers.c ./screen.c -DCOMPILE_FOR_WINDOWS=1 -Wall -s -o2 -o test_timer.exe 

#if defined(COMPILE_FOR_WINDOWS) || defined(COMPILE_FOR_LINUX) || defined(COMPILE_FOR_ARDUINO)

#else
    //#define COMPILE_FOR_WINDOWS
    #define COMPILE_FOR_LINUX
    //#define COMPILE_FOR_ARDUINO
#endif

#ifdef COMPILE_FOR_WINDOWS
  #include <winsock2.h> 
  #include <ws2tcpip.h>
  #include <windows.h> // screen functions
  #include <sys/timeb.h> //timeb
  #include <stdint.h>
  #include <stdio.h>
  
  #define platform_bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
  
  #define platform_printf(...) printf( __VA_ARGS__)
  
  #define platform_screen_clear() system("cls")
  #define platform_screen_cursorOff() do{                                               \
	                             HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);\
	                             CONSOLE_CURSOR_INFO info;                              \
	                             info.dwSize = 100;                                     \
	                             info.bVisible = FALSE;                                 \
	                             SetConsoleCursorInfo(consoleHandle, &info);            \
	                           }while(0)  

  #define platform_screen_cursorOn()  do{                                               \
	                             HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);\
	                             CONSOLE_CURSOR_INFO info;                              \
	                             info.dwSize = 100;                                     \
	                             info.bVisible = TRUE;                                  \
	                             SetConsoleCursorInfo(consoleHandle, &info);            \
	                           }while(0)  


  
  #define  platform_screen_gotoXY(x, y) do{                                             \
	                               COORD Coord;                                         \
                                   Coord.X = x;                                         \
                                   Coord.Y = y;                                         \
                                   if (y>0) Coord.Y--;                                  \
                                   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);\
                                 }while(0) 
                                 
  // -------------timers.c
  // performance information data for  timers.c. Store the maximum mS delay between calls to tick()
  // and bucket/histogram by counting the number of  mS delays from 0mS up. The final 
  // one in the array is that or greater.   
                                 
  #define platform_MAX_BUCKETS 20
  struct platform_Perfd {
        uint16_t perf_max_time;
        uint16_t perf_buckets[platform_MAX_BUCKETS];
  };
        
  struct platform_Perfd platform_timers_get_performance_data();                           
                                 
  uint8_t platform_enoughTimePassed();
  
  // communication via sockets or serial
  // platform_initiComs(3000, ""); this will set up a server that listens for an incoming connection on port 3000
  // platform_initiComs(3000, "192.168.0.22"); this will connect to a server at address 192.168.0.22 on port 3000
  uint8_t platform_initComs(uint16_t chConfig, char *adConfig, uint8_t useSavedConfigIfAvailable);  
  uint8_t platform_getByte(uint8_t* rxByte);
  uint8_t platform_sendByte(uint8_t txByte);
  

#endif  // COMPILE_FOR_WINDOWS

#ifdef COMPILE_FOR_LINUX
  #include <sys/socket.h> // rcev bind etc
  #include <fcntl.h>      // fcntl (nonblocking)
  #include <unistd.h>     // for close()
  #include <netinet/in.h> //struct sockaddr_in
  #include <sys/timeb.h> //timeb
  #include <stdint.h>
  #include <stdio.h>  
  
  #define platform_bzero(b,len) (memset((b), '\0', (len)), (void) 0)
  #define platform_printf(...) printf( __VA_ARGS__)
    
  // vt100 commands to control the screen
  #define platform_screen_clear()  printf("\033[2J")
  #define platform_screen_gotoXY(x, y) printf("\033[%d;%dH",y,x)
  #define platform_screen_cursorOff() printf("\033[?25l")
  #define platform_screen_cursorOn() printf("\033[?25h")
  
  // -------------timers.c
  // performance information data for  timers.c. Store the maximum mS delay between calls to tick()
  // and bucket/histogram by counting the number of  mS delays from 0mS up. The final 
  // one in the array is that or greater.   
                                 
  #define platform_MAX_BUCKETS 20
  struct platform_Perfd {
        uint16_t perf_max_time;
        uint16_t perf_buckets[platform_MAX_BUCKETS];
  };
        
  struct platform_Perfd platform_timers_get_performance_data();                           
                                 
  uint8_t platform_enoughTimePassed();

  // ------------- communication
  uint8_t platform_coms_init(uint16_t);
  //uint8_t platform_coms_getByte(uint8_t*);
  //uint8_t platform_coms_sendByte(uint8_t);
  


#endif



#endif //platform_h
