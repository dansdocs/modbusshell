/**
 * Daniel 2017
 * 
 * Basic Cursor location for windows command or ANSI/VT100 style terminals 
 * 
 * There are two relevent compile time defines:
 * If BUILD_FOR_WINDOWS is defined then the windows api will be used to locate the cursor
 * otherwise ANSI/VT100 commands will be used. 
 * If a function is passed in to send the data then ANSI/VT100 commands will be used
 * even if BUILD_FOR_WINDOWS is defined.  
 *
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - with one addition. In one and only one file, just before #include "./cursor.h" 
 * also put in #define CURSOR_IMPLEMENTATION 
 * You can #include "./cursor.h" in other c files as normal. 
 * 
 */

#ifndef CURSOR_H
#define CURSOR_H

    #include <stdint.h>  // uint8_t etc
    
    // Used to optionally provide a function which will be used when sending ANSI/VT100 commands
    typedef int (*cursor_sendCmdFnT)(const char *fmt, ...); 
    void cursor_setSendCmdFn(cursor_sendCmdFnT fn);    

    void cursor_clearScreen();
    void cursor_hide();
    void cursor_show();
    void cursor_gotoXY(uint8_t x, uint8_t y);
    
#endif // CURSOR_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef CURSOR_IMPLEMENTATION
#undef CURSOR_IMPLEMENTATION

    cursor_sendCmdFnT _cursor_sendCommmandFn = 0;
    void cursor_setSendCmdFn(cursor_sendCmdFnT fn){_cursor_sendCommmandFn = fn;} 

    // use passed in function to send VT100 commands
    void _cursor_clearScreen() {
        _cursor_sendCommmandFn("\033[2J");
    }
    
    void _cursor_gotoXY(uint8_t x, uint8_t y) {
        _cursor_sendCommmandFn("\033[%d;%dH", y, x);
    }
    
    void _cursor_hide() {
        _cursor_sendCommmandFn("\033[?25l");
    }
    
    void _cursor_show(){
        _cursor_sendCommmandFn("\033[?25h");
    }


    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)

        #include <stdio.h>   // printf
        
        #ifdef BUILD_FOR_WINDOWS
    
            #include <windows.h> 
    
            void cursor_clearScreen() {
                if (_cursor_sendCommmandFn) _cursor_clearScreen();
                else system("cls");
            }
            
            void cursor_hide() {   
                if (_cursor_sendCommmandFn) _cursor_hide();  
                else {                    
                    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
                    CONSOLE_CURSOR_INFO info;                              
                    info.dwSize = 100;                                     
                    info.bVisible = FALSE;                                 
                    SetConsoleCursorInfo(consoleHandle, &info);    
                }        
            }  
    
            void cursor_show() {    
                if (_cursor_sendCommmandFn) _cursor_show();  
                else {                                                           
    	            HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    	            CONSOLE_CURSOR_INFO info;                              
    	            info.dwSize = 100;                                     
    	            info.bVisible = TRUE;                                  
    	            SetConsoleCursorInfo(consoleHandle, &info);   
                }         
    	    }  
    
            void cursor_gotoXY(uint8_t x, uint8_t y) {      
                if (_cursor_sendCommmandFn) _cursor_gotoXY(x, y);  
                else {                                                                                                     
    	            COORD Coord;                                         
                    Coord.X = x;                                         
                    Coord.Y = y;                                         
                    if (y>0) Coord.Y--;   
                    if (x>0) Coord.X--;                                  
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);
                }
            } 
        #endif // BUILD_FOR_WINDOWS

        #ifdef BUILD_FOR_LINUX
            void cursor_clearScreen() {
                if (_cursor_sendCommmandFn) _cursor_clearScreen(); 
                else printf("\033[2J");
            }
            
            void cursor_gotoXY(uint8_t x, uint8_t y) {
                if (_cursor_sendCommmandFn)  _cursor_gotoXY(x, y); 
                else printf("\033[%d;%dH", y, x);
            }
            
            void cursor_hide() {
                if (_cursor_sendCommmandFn) _cursor_hide();  
                else printf("\033[?25l");
            }
            
            void cursor_show(){
                if (_cursor_sendCommmandFn) _cursor_show();  
                else printf("\033[?25h");
            }
        #endif // BUILD_FOR_LINUX
    #else 
        void cursor_clearScreen() {
            if (_cursor_sendCommmandFn) _cursor_clearScreen(); 
        }
        
        void cursor_gotoXY(uint8_t x, uint8_t y) {
            if (_cursor_sendCommmandFn)  _cursor_gotoXY(x, y); 
        }
        
        void cursor_hide() {
            if (_cursor_sendCommmandFn) _cursor_hide();  
        }
        
        void cursor_show(){
            if (_cursor_sendCommmandFn) _cursor_show();  
        }    
    #endif //(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)

#endif // CURSOR_IMPLEMENTATION

