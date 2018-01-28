/**
 * Daniel 2018
 * 
 * Partial replacement for printf specifically it can print decimal and
 * the conversion is done using lookup table approach rather than division
 * for better performance on a microcontroller. 
 * NOTE: variadic macros promote to int and int on the AVR is a 16bit.
 * Helpful varadic info: https://www.codeproject.com/Articles/4181/Variable-Argument-Functions  
 * 
 *
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - with one addition. In one and only one file, just before #include "./cursor.h" 
 * also put in #define CURSOR_IMPLEMENTATION 
 * You can #include "./cursor.h" in other c files as normal. 
 * 
 * 
 * Supported formats and escape sequences
 * 
 * 
 *  %% --> %
 *  %01u --> uint16_t displayed as decimal with one digit and leading zeros
 *  %02u --> uint16_t displayed as decimal with two digits and leading zeros
 *  %03u --> uint16_t displayed as decimal with three digits and leading zeros
 *  %04u --> uint16_t displayed as decimal with four digits and leading zeros
 *  %05u --> uint16_t displayed as decimal with five digits and leading zeros
 *  %1u --> uint16_t displayed as decimal with one digit and no leading zeros
 *  %2u --> uint16_t displayed as decimal with two digits and no leading zeros
 *  %3u --> uint16_t displayed as decimal with three digits and no leading zeros
 *  %4u --> uint16_t displayed as decimal with four digits and no leading zeros
 *  %5u --> uint16_t displayed as decimal with five digits and no leading zeros
 *  %01x --> uint16_t displayed as hex with one digit and leading zeros
 *  %02x --> uint16_t displayed as hex with two digits and leading zeros
 *  %03x --> uint16_t displayed as hex with three digits and leading zeros
 *  %04x --> uint16_t displayed as hex with four digits and leading zeros
 *  %1x --> uint16_t displayed as hex with one digit and no leading zeros
 *  %2x --> uint16_t displayed as hex with two digits and no leading zeros
 *  %3x --> uint16_t displayed as hex with three digits and no leading zeros
 *  %4x --> uint16_t displayed as hex with four digits and no leading zeros
 *  %c --> a character
 *  %s --> a string (null terminated array of chars)
 *  \\ --> a backslash character
 *  \n --> newline (0x0A)
 *  \r --> carriage return (0x0D)
 *  \t --> tab (0x09)
 *  \" --> " character
 *  \' --> ' character
 *  \033 --> octal escape sequence in this case 0x1B which is the escape character. NOTE - must be exactly 3 characters. 
 *
 *  The sequence \ooo means you can specify any character in the ASCII character set as a three-digit octal character code. The numerical value of the octal integer specifies the value of the desired character.
 *  You can use only the digits 0 through 7 in an octal escape sequence. Octal escape sequences can never be longer than three digits and are terminated by the first character that is not an octal digit. Although you do not need to use all three digits, you must use at least one. For example, the octal representation is \10 for the ASCII backspace character and \101 for the letter A, as given in an ASCII chart. (the numr of hex digits in an escape sequence is unlimited so it hasn't been implemented).
 *
**/

#ifndef UCPRINTF_H
#define UCPRINTF_H

    #include <stdint.h>  // uint8_t etc
    #include <stdarg.h>  // va_start, va_end 
    
    // A typedef for the function that sends out the bytes. 
    typedef uint8_t (*ucprintf_sendByteFnT)(uint8_t);
    
    int ucprintf(ucprintf_sendByteFnT fn, const char *fmt, ...);
    int ucprintf_vprintf(ucprintf_sendByteFnT fn, const char *fmt, va_list args);
         
    // Macro to create a wrapper function which lets tie the function 
    // that sends out the bytes and not have to include it in every call. 
    // Its a convenience feature only. EG say you have a 
    // function of type ucprintf_sendByteFnT called sendByte and you want a 
    // printf function which has the same call signature as normal printf 
    // and uses sendByte to send out the bytes: 
    // ucprintf_GENERATE_FN(examplePrintf, sendByte)
    // You now have a function called examplePrintf which you can call 
    // just like printf (you don't need to include sendByte as you would if 
    // you used ucprintf directly).
    #define ucprintf_CREATE_WRAPPED_FN(fn_name, sendByteFn) \
       int fn_name(char *fmt, ...) {                        \
           va_list args;                                    \
           va_start( args, fmt);                            \
           ucprintf_vprintf(&sendByteFn, fmt, args );       \
           va_end( args );                                  \
           return 0;                                        \
       }
        
#endif // UCPRINTF_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef UCPRINTF_IMPLEMENTATION
#undef UCPRINTF_IMPLEMENTATION
    
    void _ucprintf_itoa_base16(ucprintf_sendByteFnT fn, uint16_t val, uint8_t hideLeadingZeros, uint8_t numDisplayDigits) {
        char dig[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        #define _ucprintf_MAXHEXDIG 4
        char result[_ucprintf_MAXHEXDIG] = {'0', '0', '0', '0'};
        uint8_t dignum;
        
        result[0] = dig[(val>>12) & 0x000F];
        result[1] = dig[(val>>8) & 0x000F];
        result[2] = dig[(val>>4) & 0x000F];    
        result[3] = dig[val & 0x000F];    
        
        dignum = _ucprintf_MAXHEXDIG - numDisplayDigits;
        
        if (hideLeadingZeros){
            for(; (result[dignum] == '0') && (dignum < _ucprintf_MAXHEXDIG-1); ++dignum)
                ;
        }
        
        for(; dignum < _ucprintf_MAXHEXDIG; ++dignum) {
            fn(result[dignum]);
        }    
    }
    
    
    // In the lookup table, using the strings is just a way of populating
    // a two dimensional array of numbers increasing in powers of 2 with each
    // number an array of each number as a power of 10. 
    // i.e. at the top level the array is base 10 digits increasing as power of 2: 
    // 0001, 0002, 0004, 0008, 0016, 0032, 0064, 0128, 0256, 0512... 
    // The corect combination of these is then summed based on the incoming
    // value you want converted to decimal. 
    void _ucprintf_itoa_base10(ucprintf_sendByteFnT fn, uint16_t val, uint8_t hideLeadingZeros, uint8_t numDisplayDigits) {
        #define _ucprintf_LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))
        #define _ucprintf_NUMDIGITS 5
        uint16_t bitnum, dignum;
        char sum, carry, result[_ucprintf_NUMDIGITS] = "\x00\x00\x00\x00\x00";
        const char *lookup;
        static const char itoa_lookup[][_ucprintf_NUMDIGITS] = {
            "\x00\x00\x00\x00\x01", "\x00\x00\x00\x00\x02", "\x00\x00\x00\x00\x04",
            "\x00\x00\x00\x00\x08", "\x00\x00\x00\x01\x06", "\x00\x00\x00\x03\x02", 
            "\x00\x00\x00\x06\x04", "\x00\x00\x01\x02\x08", "\x00\x00\x02\x05\x06", 
            "\x00\x00\x05\x01\x02", "\x00\x01\x00\x02\x04", "\x00\x02\x00\x04\x08",
            "\x00\x04\x00\x09\x06", "\x00\x08\x01\x09\x02", "\x01\x06\x03\x08\x04",
            "\x03\x02\x07\x06\x08"};         
    
        for(bitnum = 0; bitnum < _ucprintf_LEN(itoa_lookup); ++bitnum) {
            if(val & (1 << bitnum)) {
                carry = 0;
                lookup = itoa_lookup[bitnum];
                for(dignum = _ucprintf_NUMDIGITS-1; dignum != 0xFFFF; --dignum) {
                    sum = result[dignum] + lookup[dignum] + carry;
                    if(sum < 10) {
                        carry = 0;
                    } else {
                        carry = 1;
                        sum -= 10;
                    }
                    result[dignum] = sum;
                }
            }
        }
        
        dignum = _ucprintf_NUMDIGITS - numDisplayDigits;
    
        if (hideLeadingZeros){
            for(; !result[dignum] && dignum < _ucprintf_NUMDIGITS-1; ++dignum)
                ;
        }
    
        for(; dignum < _ucprintf_NUMDIGITS; ++dignum) {
            fn(result[dignum] + '0');
        }
    }    
    
    // Takes in a pointer to the character array with index pointing to the start of the 
    // escape sequence (the backslash). It returns with binary value byte represented by the escape sequence.
    // The index (i) incremented to point to the last part of the escape sequence 
    // which is just one further along for all except in the case certain octal sequences. 
    //  The most detailed is the octal escape sequence. 
    // \ddd is the octal escape sequence where d can be 0 - 7 inclusive. 
    // there can be one two or three characters after the \. Its terminated by the first
    // character that isn't 0 - 7 or once there have been three characters.  
    // If no supported escape sequence then 0x15 is returned (zero can't be because its NULL). 
    // 0x15 is ASCII Negative Ack so not likley to appear in a string. 
    uint8_t _ucprintf_escape(const char *fmt, uint8_t *i){
        
        uint8_t temp = 0;
        uint8_t a = 0;
        uint8_t b = 0;
        uint8_t c = 0;
        
        if (fmt[*i] == '\\') {*i = *i + 1; return '\\';}
        else if ((fmt[*i] == '\\') && (fmt[*i+1] == '\'')) {*i = *i + 1; return '\'';}
        else if ((fmt[*i] == '\\') && (fmt[*i+1] == '\"')) {*i = *i + 1; return '\"';}     
        else if ((fmt[*i] == '\\') && (fmt[*i+1] == 't'))  {*i = *i + 1; return '\t';} 
        else if ((fmt[*i] == '\\') && (fmt[*i+1] == 'r'))  {*i = *i + 1; return '\r';}
        else if ((fmt[*i] == '\\') && (fmt[*i+1] == 'n'))  {*i = *i + 1; return '\n';}        
        else if ((fmt[*i] == '\\') && ((fmt[*i+1] >= '0') && (fmt[*i+1] <= '9'))) {
            *i = *i + 1;            
            if ((fmt[*i+1] != 0) && ((fmt[*i+1] >= '0') && (fmt[*i+1] <= '9'))) {
                if ((fmt[*i+2] != 0) && ((fmt[*i+2] >= '0') && (fmt[*i+2] <= '9'))) {
                    c = fmt[*i+2]; b = fmt[*i+1]; a = fmt[*i];
                    *i = *i + 2;
                }
                else {
                    c = fmt[*i+1]; b = fmt[*i];
                    *i = *i + 1;
                }
            }
            else c = fmt[*i];
            
            temp = (a-'0') << 6;
            temp = (b-'0') << 3;
            temp = (c-'0');
            return temp;
        }
        else return 0x15;
    }
    
    void _ucprintf_processStr(ucprintf_sendByteFnT fn, char *s){
        
        uint8_t b = s[0];
        uint8_t i = 0; // index of the charater in the format string
        uint8_t skip = 0;
        
        while (b != '\0'){
        
            if (b == '\\') {
                b = _ucprintf_escape(s, &i);              
                if (b < 0x20) skip = 1;
                i++;
            } 
            if (!(skip)) fn(b);
            else  skip = 0;            
            i++;
            b=s[i];
        }
    }        
    
    int ucprintf_vprintf(ucprintf_sendByteFnT fn, const char *fmt, va_list args){
        uint8_t b = fmt[0];
        uint8_t i = 0; // index of the charater in the format string
        char *s;
        uint8_t skip = 0;       

        while (b != '\0'){

            if (b == '\\') {
                b = _ucprintf_escape(fmt, &i);
            }
            else if ((b == '%') && (fmt[i+1] == '%')) i++;
            else if ((b == '%') && (fmt[i+1] == 'c')) {
                b = (uint8_t) va_arg(args, int); 
                if (b < 0x20) skip = 1;
                i++;
            } 
            else if ((b == '%') && (fmt[i+1] == 's')) {
                s = va_arg(args, char*); 
                _ucprintf_processStr(fn, s);
                i++;
                skip = 1;                
            }
            else if ((b == '%') && ((fmt[i+1] > '0') && (fmt[i+1] <= '5'))){
              if (fmt[i+2] == 'u') {
                  _ucprintf_itoa_base10(fn, va_arg(args, int), 1, fmt[i+1]-'0');    
                  i = i+2;
                  skip = 1;
              }
              else if ((fmt[i+2] == 'x') && (fmt[i+1] != '5')){
                  _ucprintf_itoa_base16(fn, va_arg(args, int), 1, fmt[i+1]-'0');    
                  i = i+2;
                  skip = 1;                  
              }
            }
            else if ((b == '%') && (fmt[i+1] == '0')) {
                if ((fmt[i+2] > '0') && (fmt[i+2] <= '5')){
                    if (fmt[i+3] == 'u') {
                        _ucprintf_itoa_base10(fn, va_arg(args, int), 0, fmt[i+2]-'0');    
                        i = i+3;
                        skip = 1;
                    }
                    else if ((fmt[i+3] == 'x') && (fmt[i+2] != '5')){
                        _ucprintf_itoa_base16(fn, va_arg(args, int), 0, fmt[i+2]-'0');    
                        i = i+3;
                        skip = 1;                  
                    }
                }
            }            

            if (!(skip)) fn(b);
            else skip = 0;
            i++;
            b=fmt[i];
        }
        return 0;
   }  
    
   int ucprintf(ucprintf_sendByteFnT fn, const char *fmt, ...) {
        va_list args;
      
        va_start(args, fmt);
        ucprintf_vprintf(fn, fmt, args);
        va_end(args);
        return 0;
    }
       

#endif // UCPRINTF_IMPLEMENTATION

