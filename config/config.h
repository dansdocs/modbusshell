/**
 * Daniel 2017
 * Super basic config file reader. Extracts data from between square brackets and 
 * ignores everything else. Key value style. Ignores whitespace. No checking.
 * 
 * Example config.txt contents:
 * [port=3000]
 * 
 * Example usage:
 * printf("%s", config_get(CONFIG_PORT));
 * 
 * Notes: To add/remove configuration keys, update both enum and array. Keep them in sync and follow pattern. 
 * Use only lowercase in array (the key in configuration file is converted to lowercase if upper is used). 
 * Leave CONFIG_MAX in place so that the array size is ok. 
 * CONFIG_BUFFERSIZE is the maximum individual string length. 
 * This applies to the key and value. Minus 1 for the null termination. 
 * 
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - but one and only onec file, just before #include "./config.h" also put in #define CONFIG_IMPLEMENTATION 
 * You can #include "./config.h" in other c files as normal. 
 */

#ifndef CONFIG_H
#define CONFIG_H

    // returns the value related to the key requested from the config file. 
    // The key is as per CONFIG_KEYS. For example, if config.txt has the following line
    // [port=3000]
    // then the function call:
    // config_get(CONFIG_PORT);
    // would return the string "3000"
    char* config_get(uint8_t config);

    // Its optional to pass in a function pointer for logging. 
    typedef void (*config_logFnT)(uint8_t, uint8_t, const char *, ...); 
    void config_setLogFn(config_logFnT fn);
    
    #define CONFIG_FILE_NAME "config.txt"
    
    // This is the maximum number of characters 
    // that a key or the data can have. Actually 1 less because of null terminator     
    #define CONFIG_BUFFERSIZE 20

    // A list of recognised values that will be extracted from the config file. 
    // Add and remove following the same pattern. 
    // x-macro approach to associate an enum and string together and then 
    // create the enum and string array from them.  
    // To see the output of the preprocessor: gcc -E config.h
    
    #define CONFIG_KEYS                  \
    X(CONFIG_PORT,        "port")        \
    X(CONFIG_ADDRESS,     "address")     \
    X(CONFIG_LOGSTDERROR, "logstderror") \
    X(CONFIG_LOGFILE,     "logfile")     \
    X(CONFIG_LOGLEVEL,    "loglevel")    
    
    #define X(a, b) a,
        enum {CONFIG_KEYS CONFIG_MAX};
    #undef X

    #define X(a, b) {b},
        static const char config_keys[CONFIG_MAX][CONFIG_BUFFERSIZE] = {CONFIG_KEYS};
    #undef X

#endif // CONFIG_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef CONFIG_IMPLEMENTATION
#undef CONFIG_IMPLEMENTATION

    #include <stdint.h> // for uint8_t etc. 
    #include <ctype.h>  // for tolower()
    #include <stdio.h>  // for fopen, fgetc, fclose
    #include <string.h> // for strcmp
  
    // Infrustructure for logging.  
    #define _CONFIG_FID ((uint8_t)(('c' << 2) + 'o' + 'n'))
    enum { CONFIG_LOG_TRACE, CONFIG_LOG_DEBUG, CONFIG_LOG_INFO, CONFIG_LOG_WARN, CONFIG_LOG_ERROR, CONFIG_LOG_FATAL };    
    void _config_dummylogFn (uint8_t fid, uint8_t lvl, const char *fmt, ...){;}    
    config_logFnT config_logFn = &_config_dummylogFn;
    void config_setLogFn(config_logFnT fn){
        config_logFn = fn;
        config_logFn(_CONFIG_FID, CONFIG_LOG_INFO, "%02x = config/config.h", _CONFIG_FID);    
    } 
 
  void _config_readFile(char cfg[][CONFIG_BUFFERSIZE]){
      int i;
      int j;
      int cfgIndex;

      FILE *fp;
      char c;
      enum {sBRACKETSTART, sKEY, sDATA};
      uint8_t state = sBRACKETSTART;
      
      // temporarily hold a string as its being read from file. 
      char st[CONFIG_BUFFERSIZE];   
      for (j = 0; j < CONFIG_BUFFERSIZE; j++) st[j] = '\0';
		  
      fp = fopen(CONFIG_FILE_NAME, "r");
      if (fp == NULL)  config_logFn(_CONFIG_FID, CONFIG_LOG_INFO,  "%s not found", CONFIG_FILE_NAME);
      else {
          while ((c = fgetc(fp)) != EOF){				  
              // skip whitespace & reset statemachine if we get a close bracket.
		      if ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t')) continue;
		      else if (c == ']') state = sBRACKETSTART;
		          		          
		      switch(state) {
                  case sBRACKETSTART :
                      if (c == '[') { 
		                  state = sKEY;
						  i = 0;
		              }
                      break;                           
                  case sKEY :
                      if (i < CONFIG_BUFFERSIZE - 2) { 
                          if (c != '=') st[i++] = tolower(c);
                          else {
					          i = 0;
						      cfgIndex = -1;
						      for (j=0; j < CONFIG_MAX; j++) if (strcmp(st, config_keys[j]) == 0) cfgIndex = j;
						      for (j=0; j < CONFIG_BUFFERSIZE; j++) st[j] = '\0';
						      if (cfgIndex < 0) state = sBRACKETSTART;
						      else state = sDATA;
						  }
				      }                          
                      break;                          
                  case sDATA :
                      if (i < CONFIG_BUFFERSIZE - 2) cfg[cfgIndex][i++] = c;						  
                      break; 
              }
	      }   
          fclose(fp);	
      }      	          
  }
    
  char* config_get(uint8_t key){
      
      int i;
      int j;
      static int fileNotRead = 1;
       
      // Each row corresponds to the key read from the file. [row][column]
      static char cfg[CONFIG_MAX][CONFIG_BUFFERSIZE];        
      
      if (fileNotRead){
		  fileNotRead = 0;
          for (i = 0; i < CONFIG_MAX; i++){
              for (j = 0; j < CONFIG_BUFFERSIZE; j++) cfg[i][j] = '\0'; 
	      }		  
		  _config_readFile(cfg);
	  }         
      return (cfg[key]);
  }


#endif // CONFIG_IMPLEMENTATION



