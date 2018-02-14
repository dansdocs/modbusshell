/**
 * Daniel 2018
 *
 *  
**/

#ifndef IO_SOCK_H
#define IO_SOCK_H

    #include <winsock2.h> 
    #include <ws2tcpip.h>
    #include <stdint.h>
    #include <stdio.h>

    typedef struct io_sock_s {
      int sockfd;
      int portno;
      int iResult;
      int servsockfd; 
      socklen_t clilen;
      struct sockaddr_in serv_addr;
      struct sockaddr_in cli_addr;
      unsigned long on; 
      uint8_t connectToRemoteServer;
      WSADATA wsaData;
      char address[20];
    } io_sock_s;
        
#endif // IO_SOCK_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_SOCK_IMPLEMENTATION
#undef IO_SOCK_IMPLEMENTATION


#ifdef COMPILE_FOR_WINDOWS

    #define io_sock_bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
  
    uint8_t platform_initComs(io_sock_s *s, uint16_t chConfig, char *adConfig, uint8_t useSavedConfigIfAvailable) {
          
        s-> portno = (int) chConfig;        
        s-> on = 1;
        s-> connectToRemoteServer = 0;
        
        strcpy(s-> address, adConfig);      
        //if (useSavedConfigIfAvailable) getConfig(&portno, s-> address, MAXBUFSIZE);
        
        if (s-> address[0] == '\0') printf("No address. Act as server listing on port: %i\n", s-> portno);
        else {
            printf("Connecting to server with address %s on port %i", s-> address, s-> portno);
        }
        
        
        // Initialize Winsock
        s-> iResult = WSAStartup(MAKEWORD(2,2), &(s-> wsaData));
        if (s-> iResult != 0) printf("WSAStartup failed with error: %d\n", s-> iResult);
        s-> sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (s-> sockfd < 0) printf("ERROR opening socket");
        io_sock_bzero((char *) &(s-> serv_addr), sizeof(s-> serv_addr));
        s-> serv_addr.sin_family = AF_INET;
        s-> serv_addr.sin_port = htons(s-> portno);

        // if address is a null string then we don't have server ip address to connect to 
        // so we are the server and need to listen for an incoming connection.       
        if (s-> address[0] == '\0'){
		    s-> connectToRemoteServer = 0;
            s-> serv_addr.sin_addr.s_addr = INADDR_ANY;
            if (bind(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) < 0) {
 	            printf("ERROR on binding\n");
                closesocket(s-> sockfd);
                WSACleanup();
            }  

            // 5 is the maximum number of connections (backlog). 
            if (listen(s-> sockfd, 5) < 0){
	            printf("Socket listen error\n");
                closesocket(s-> sockfd);
                WSACleanup();
	        }
            s-> clilen = sizeof(s-> cli_addr);
            s-> servsockfd = accept(s-> sockfd, (struct sockaddr *) &(s-> cli_addr), &(s-> clilen));
            if (s-> servsockfd < 0) {
	             printf("ERROR on accept");
		         WSACleanup();
	        }
	        else ioctlsocket(s-> servsockfd, FIONBIO, &(s-> on));  // set to non-blocking	  
	    }
	    else {
		    // we have an ip address for a server to connect to, so connect to it. 
		    s-> connectToRemoteServer = 1;
		    s-> serv_addr.sin_addr.s_addr = inet_addr(s-> address);
		    if (connect(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) != 0 ){
                printf("Connect error when connecting to server\r");
                closesocket(s-> sockfd);
                WSACleanup();
		    }
		    else ioctlsocket(s-> sockfd, FIONBIO, &(s-> on));  // set to non-blocking	  
        }

	    return 0;
    }
  
    uint8_t io_sock_getByte(io_sock_s *s, uint8_t *rxByte){

        int n = 0;
        char rx;
        int nError;
      
        if (s-> connectToRemoteServer) n = recv(s-> sockfd, &rx, 1, 0);
	    else  n = recv(s-> servsockfd, &rx, 1, 0);
         	  
	    if (n == -1) {
            nError = WSAGetLastError(); 
            if (nError == WSAEWOULDBLOCK) return 0;
	    }
	  
        if ((n == 0) || ((n == -1) && (nError != WSAEWOULDBLOCK))) {
            printf("\n%i Disconnected - reconnecting \n", nError); 
			if (s-> connectToRemoteServer == 0) closesocket(s-> servsockfd);
            closesocket(s-> sockfd);
			WSACleanup();
            platform_initComs(s, s-> portno, s-> address, s-> connectToRemoteServer);                
            return 0;
        }
        else {
		    *rxByte = (uint8_t)rx;
		    return 1;
	    }
	    return 0;
    }
  
    uint8_t io_sock_sendByte(io_sock_s *s, uint8_t txByte){
        int n = 0;
        char tx = txByte; 
      
        // this assumes that a closed connection will be addressed by the getByte function being called often
        // it also assumes that data that isn't sent will only be due to a closed connection, not because too
        // much was being sent in one go. 
        if (s-> connectToRemoteServer) n = send(s-> sockfd, &tx, 1, 0);
	    else  n = send(s-> servsockfd, &tx, 1, 0);

	    return (uint8_t) n;
    }

#endif  // COMPILE_FOR_WINDOWS


#endif // IO_SOCK_IMPLEMENTATION

