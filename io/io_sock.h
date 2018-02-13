/**
 * Daniel 2018
 *
 *  
**/

#ifndef IO_SOCK_H
#define IO_SOCK_H

        
#endif // IO_SOCK_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_SOCK_IMPLEMENTATION
#undef IO_SOCK_IMPLEMENTATION


#ifdef COMPILE_FOR_WINDOWS


    #include <winsock2.h> 
    #include <ws2tcpip.h>
    #include <stdint.h>
    #include <stdio.h>
    #define platform_bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
  
  
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
        
        //log_set_quiet(1);
        //log_fp = fopen(LOG_FILE_NAME, "a");
        //if (log_fp == NULL) {
        //    printf("error creating log.txt \r");
        //}
        //else log_set_fp(log_fp);
        
        //log_debug("hello from logger");
        
        strcpy(address, adConfig);      
        //if (useSavedConfigIfAvailable) getConfig(&portno, address, MAXBUFSIZE);
        
        if (address[0] == '\0') printf("No address. Act as server listing on port: %i\n", portno);
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
      
        if (connectToRemoteServer) n = recv(sockfd, &rx, 1, 0);
	    else  n = recv(servsockfd, &rx, 1, 0);
         	  
	    if (n == -1) {
            nError = WSAGetLastError(); 
            if (nError == WSAEWOULDBLOCK) return 0;
	    }
	  
        if ((n == 0) || ((n == -1) && (nError != WSAEWOULDBLOCK))) {
            printf("\n%i Disconnected - reconnecting \n", nError); 
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
                closesocket(sockfd);
			    WSACleanup();
                platform_initComs(3000, "", 0);                
		    }
              
            return 0;
        }
        else {
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


#endif // IO_SOCK_IMPLEMENTATION

