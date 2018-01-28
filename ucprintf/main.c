/* Example of using ucprintf.h  */


// check that we have a target to build for. 
#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX) || defined(BUILD_FOR_ARDUINO)
#else
    // No recognised target. Display a message. Manually set a target. 
    #pragma message("No Target defined. Define one of: BUILD_FOR_WINDOWS, BUILD_FOR_LINUX BUILD_FOR_ARDUINO")
    #define BUILD_FOR_LINUX
#endif


#if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)
    // stdio.h is not avaialble for arduino
    #include <stdio.h>   // printf, putchar
#endif

#define UCPRINTF_IMPLEMENTATION
#include "./ucprintf.h"
    
// an example stub that sends a byte to stdout, if putchar is available.  
uint8_t sendByteExample(uint8_t c) {
    #if defined(BUILD_FOR_WINDOWS) || defined(BUILD_FOR_LINUX)    
        putchar(c);
    #endif 
    return 1;
}

// The following are different examples of wrapping the functions.
// This is done if you want to have the same function signature as 
// printf - and have the delegate function you pass in which sends the
// bytes out wrapped so you don't have to carry it arround.  
       
// an example of wrapping the function so it can be used like printf 
// without needing to carry arround the sendByte function. 
int printerf(char *fmt, ...){
    va_list args;
    va_start( args, fmt);
    ucprintf_vprintf(&sendByteExample, fmt, args );
    va_end( args );
    return 0;
}       
   
// using the function creating macro. The function name is the first parameter.   
// This will tie the sendByteExample function to the call. The macro creates the 
// same thing as the version above. 
ucprintf_CREATE_WRAPPED_FN(printf_generated, sendByteExample)  
ucprintf_CREATE_WRAPPED_FN(printf_generated2, sendByteExample) 
    
// A different way of wrapping the function - but its a plain macro so you can't 
// get a function address to pass into other functions like you could with the 
// versions above.
#define PRINTF(...) ucprintf(&sendByteExample, __VA_ARGS__)   

  

int main()
{
    ucprintf_sendByteFnT fnSend;
    fnSend = &sendByteExample;
    
    ucprintf(fnSend, "OctalNull: \0 This Shouldnt appear because the null terminates the string\n");
    ucprintf(fnSend, "Octal three char: \041-->should be an exclamation mark\n");    
    ucprintf(fnSend, "Octal two char: \41-->should be an exclamation mark\n");  
    ucprintf(fnSend, "Octal three char: \123-->should be capital S\n");  
    ucprintf(fnSend, "Percent5: %%, Tab: \t, quote: \', Double Quote: \", Backslash: \\");
    ucprintf(fnSend, "CharacterArg: %c, OctalEscape: \115\n", 'a');
    ucprintf(fnSend, "String eg:  %s  %c     %s",  "stringy", 'j', "With octal escape: \107 --> should be letter G\n");

    ucprintf(fnSend, "Should be 8: %5u\n", 8); 
    ucprintf(fnSend, "Should be 00132: %05u\n", 132);   
    ucprintf(fnSend, "Should be 65532: %05u\n", 65532);    
    ucprintf(fnSend, "Should be 0: %5u\n", 0); 
    ucprintf(fnSend, "Should be 0000: %04u\n", 0);    
             
    ucprintf(fnSend, "Should be BACD: %04x\n", 0xBACD);    
    ucprintf(fnSend, "Should be 001E: %04x\n", 0x001E);   
    ucprintf(fnSend, "Should be 01E: %03x\n", 0x001E);  
    ucprintf(fnSend, "Should be 1E: %02x\n", 0x001E); 
    ucprintf(fnSend, "Should be 0000: %04x\n", 0x0000); 
    ucprintf(fnSend, "Should be 1E: %4x\n", 0x001E); 
    ucprintf(fnSend, "Should be 0: %3x\n", 0x0000); 
    

    
    PRINTF("Wrapping with a macro %s\n", "piggle sniggle");
    
    printerf("wrapping with a function %3u\n", 226);
    
    printf_generated("wrapping with a generated function %3u\n", 52);
    printf_generated2("wrapping with a generated2 function %3u\n", 89);
    
    return 0; 
}
