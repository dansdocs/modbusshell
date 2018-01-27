
/*********************************************************
* Even out differences between different targets
* Daniel Burke 2017
*********************************************************/

#include "./platform.h"

#if defined(COMPILE_FOR_WINDOWS) || defined(COMPILE_FOR_LINUX) 

  #include "./log.h"

  //----------------------
  //--------------Config File Reader
  
  #define CONFIG_FILE_NAME "config.txt"
  #define LOG_FILE_NAME "log.txt"
  FILE *log_fp;

  // extracts config from a file in a super basic way. Extracts data from between square brackets and 
  // ignores everything else. Key value style. Ignores whitespace. No checking. 
  uint8_t getConfig(int *port, char addr[], uint8_t bufSize){
      FILE *fp;
      char portNum[bufSize];
      char address[bufSize];
      char log[bufSize];
      char c;
      enum st {sBRACKETSTART, sTYPE, sPORTEQ, sADDREQ, sLOGEQ, sPORTDATA, sADDRDATA, sLOGDATA, sBRACKETEND};
      uint8_t state = sBRACKETSTART;
      uint8_t i=0;
      
      portNum[0] = '\0';
      address[0] = '\0';

      fp = fopen(CONFIG_FILE_NAME, "r");
      if (fp == NULL) {
		  printf("config.txt not found. Using defaults.\r");
		  return 0;
	  }
    
      while ((c = fgetc(fp)) != EOF){
		  if ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t')) continue;
		  else if (c == ']') {
			  if (state == sPORTDATA) portNum[i] = '\0';
			  if (state == sADDRDATA) address[i] = '\0';
			  if (state == sLOGDATA ) address[i] = '\0';
			  i = 0;
			  state = sBRACKETSTART;
		  }
		  else if ((state == sBRACKETSTART) && (c == '[')) state = sTYPE;
		  else if ((state == sTYPE) && ((c == 'p') || (c == 'P'))) state = sPORTEQ;
		  else if ((state == sTYPE) && ((c == 'a') || (c == 'A'))) state = sADDREQ;
		  else if ((state == sTYPE) && ((c == 'l') || (c == 'L'))) state = sLOGEQ;		  
		  else if ((state == sPORTEQ) && (c=='=')) state = sPORTDATA;
		  else if ((state == sADDREQ) && (c=='=')) state = sADDRDATA;
		  else if ((state == sLOGEQ) && (c=='=')) state = sLOGDATA;
		  else if ((state == sPORTDATA) && (i < bufSize-1)) portNum[i++] = c;
		  else if ((state == sADDRDATA) && (i < bufSize-1)) address[i++] = c;
		  else if ((state == sLOGDATA) && (i < bufSize-1)) log[i++] = c;		  		  
	  }
   
      if ((portNum[0] == '\0') && (address[0]=='\0') && (log[0]=='\0')){
		  printf("Cant extract information from config.txt\r");
		  return 0;
	  }
      if (portNum[0] == '\0') printf("config.txt doesn't have port data\r");
      else *port = atoi(portNum);
      
      if (address[0] == '\0') printf("config.txt doesn't have address data\r");
      else strcpy(addr, address);
      
      if (log[0] == '\0') printf("config.txt doesn't have log data\r");
	 
      fclose(fp);
      return 1;
  }


#endif


#ifdef COMPILE_FOR_WINDOWS
      
  struct platform_Perfd platform_perfd;
  #define STIMEOUTMS 2000
  //uint16_t socketTimeout = STIMEOUTMS;

  // getter which returns the array of millisecond buckets (histogram) 
  // kept up to date by the platform_enoughTimePassed function.   
  struct platform_Perfd platform_timers_get_performance_data(){
    return platform_perfd;
  }                           
  
  // uses a system timer to see when 1mS has passed and returns 1 otherwise 0.                                
  uint8_t platform_enoughTimePassed(){
	    static uint64_t old_time = 0;
	    static uint8_t first_time = 1;
	    uint64_t curr_time = 0;
	    uint64_t diff_time = 0;
	    static uint8_t first_perf_time = 1;    
        struct _timeb timebuffer;
        uint8_t timePassed = 1;
        
        _ftime(&timebuffer);
        curr_time = (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
    
	    if (first_time) {
			first_time = 0;
			old_time = curr_time;
		}
		
	    diff_time = curr_time - old_time;
	    if (diff_time < 1) timePassed = 0;
	    else old_time = curr_time;

        // update performance data.		    
        if (first_perf_time){
	      	first_perf_time = 0;
	      	platform_perfd.perf_max_time = (uint16_t) diff_time;
	    }

        
	    if (diff_time < platform_MAX_BUCKETS) platform_perfd.perf_buckets[diff_time]++;
        else  platform_perfd.perf_buckets[platform_MAX_BUCKETS-1]++;
	      
	    if ((uint16_t) diff_time > platform_perfd.perf_max_time){
	      	platform_perfd.perf_max_time = (uint16_t) diff_time;
	    }	
	    
	    // update timeout for socket receiving data. If no data recieved, timeout and close it. 
	    //if ((timePassed) && (socketTimeout)) socketTimeout--;
	    
	    return timePassed;
  }                                 

  // -------------  sockets 

  int sockfd, servsockfd; 
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  unsigned long on = 1;
  uint8_t connectToRemoteServer = 0;
  WSADATA wsaData;

  uint8_t platform_initComs(uint16_t chConfig, char *adConfig, uint8_t useSavedConfigIfAvailable) {

      int iResult, portno;
      portno = (int) chConfig;
      #define MAXBUFSIZE 20
      char address[MAXBUFSIZE];
      
      log_set_quiet(1);
      log_fp = fopen(LOG_FILE_NAME, "a");
      if (log_fp == NULL) {
		  printf("error creating log.txt \r");
	  }
	  else log_set_fp(log_fp);
      
      log_debug("hello from logger");
      
      strcpy(address, adConfig);      
      if (useSavedConfigIfAvailable) getConfig(&portno, address, MAXBUFSIZE);
      
      if (address[0] == '\0') printf("No address. Act as server listing on port: %i", portno);
      else {
		  printf("Connecting to server with address %s on port %i", address, portno);
      }
		  

      // Initialize Winsock
      iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
      if (iResult != 0) printf("WSAStartup failed with error: %d\n", iResult);
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) printf("ERROR opening socket");
      platform_bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(portno);

      // if address is a null string then we don't have server ip address to connect to 
      // so we are the server and need to listen for an incoming connection.       
      if (address[0] == '\0'){
		  connectToRemoteServer = 0;
          serv_addr.sin_addr.s_addr = INADDR_ANY;
          if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
 	          printf("ERROR on binding\n");
              closesocket(sockfd);
              WSACleanup();
          }  

          // 5 is the maximum number of connections (backlog). 
          if (listen(sockfd, 5) < 0){
	          printf("Socket listen error\n");
              closesocket(sockfd);
              WSACleanup();
	      }
          clilen = sizeof(cli_addr);
          servsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
          if (servsockfd < 0) {
	           printf("ERROR on accept");
		       WSACleanup();
	      }
	      else ioctlsocket(servsockfd, FIONBIO, &on);  // set to non-blocking	  
	  }
	  else {
		  // we have an ip address for a server to connect to, so connect to it. 
		  connectToRemoteServer = 1;
		  serv_addr.sin_addr.s_addr = inet_addr(address);
		  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0 ){
              printf("Connect error when connecting to server\r");
              closesocket(sockfd);
              WSACleanup();
		  }
		  else ioctlsocket(sockfd, FIONBIO, &on);  // set to non-blocking	  
      }

	  return 0;
  }
  
  uint8_t platform_getByte(uint8_t *rxByte){

      int n = 0;
      int err;
      char rx;
      int nError;

      //if (socketTimeout == 0) {
		//  printf("No data recieved timeout, close socket...\r");	
		//  if (connectToRemoteServer) closesocket(sockfd);
	    //  else closesocket(servsockfd);
	  //}
      
      if (connectToRemoteServer) n = recv(sockfd, &rx, 1, 0);
	  else  n = recv(servsockfd, &rx, 1, 0);
	  	  	  
	  if (n == -1) {
	      nError = WSAGetLastError(); 
          if (nError == WSAEWOULDBLOCK) return 0;
	  }
	  
      if ((n == 0) || ((n == -1) && (nError != WSAEWOULDBLOCK))) {
          printf("Disconnected - reconnecting \n"); 
          if (connectToRemoteServer) {
			  closesocket(sockfd);
			  WSACleanup();
			  WSAStartup(MAKEWORD(2,2), &wsaData);
              sockfd = socket(AF_INET, SOCK_STREAM, 0);
			  err = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		      if (err != 0 ){
				  nError = WSAGetLastError();
                  printf("ERROR on trying to reconnect3 - exit program and restart %i, %i\r", err, nError);
		      }
		      else {
				  ioctlsocket(sockfd, FIONBIO, &on);  // set to non-blocking	 
		     	  printf("Reconnected"); 
			 }
		  }
		  else {
			  closesocket(servsockfd);
			  WSACleanup();
              servsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
              if (servsockfd < 0) {
				  printf("ERROR on trying to reconnect - exit program and restart\r");
			  }
              else {
				  ioctlsocket(servsockfd, FIONBIO, &on);
				  printf("Reconnected");
			  }
		  }

          return 0;
      }
      else {
		  //socketTimeout = STIMEOUTMS;  // recieved some data so reset the socket close timer. 
		  *rxByte = (uint8_t)rx;
		  return 1;
	  }
	  return 0;
  }
  
  uint8_t platform_sendByte(uint8_t txByte){
      int n = 0;
      char tx = txByte;
      
      // this assumes that a closed connection will be addressed by the getByte function being called often
      // it also assumes that data that isn't sent will only be due to a closed connection, not because too
      // much was being sent in one go. 
      if (connectToRemoteServer) n = send(sockfd, &tx, 1, 0);
	  else  n = send(servsockfd, &tx, 1, 0);

	  return (uint8_t) n;
  }

#endif  // COMPILE_FOR_WINDOWS


#ifdef COMPILE_FOR_LINUX

    struct platform_Perfd platform_perfd;

    // getter which returns the array of millisecond buckets (histogram) 
    // kept up to date by the platform_enoughTimePassed function.   
    struct platform_Perfd platform_timers_get_performance_data(){
        return platform_perfd;
    }                           
	
    // uses a system timer to see when 1mS has passed and returns 1 otherwise 0.                                
    uint8_t platform_enoughTimePassed(){
	    static uint64_t old_time = 0;
	    static uint8_t first_time = 1;
	    uint64_t curr_time = 0;
	    uint64_t diff_time = 0;
	    static uint8_t first_perf_time = 1;
        uint8_t timePassed = 1;
        
        struct timeb timebuffer;
        ftime(&timebuffer);
        curr_time = (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
         
   	    if (first_time) {
			first_time = 0;
			old_time = curr_time;
		}
	    diff_time = curr_time - old_time;
	    if (diff_time < 1) timePassed = 0;
	    else old_time = curr_time;

        // update performance data.		    
        if (first_perf_time){
	      	first_perf_time = 0;
	      	platform_perfd.perf_max_time = (uint16_t) diff_time;
	    }
        
	    if (diff_time < platform_MAX_BUCKETS) platform_perfd.perf_buckets[diff_time]++;
        else  platform_perfd.perf_buckets[platform_MAX_BUCKETS-1]++;
	      
	    if ((uint16_t) diff_time > platform_perfd.perf_max_time){
	      	platform_perfd.perf_max_time = (uint16_t) diff_time;
	    }   
	    return timePassed;
	}
	
	uint8_t platform_coms_init(uint16_t portNo){
	}

#endif  // COMPILE_FOR_LINUX

