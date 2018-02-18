/**
 * Daniel 2018
 *
 * a server can only take a single connection - point to point. 
 * while get and send data on an open port is non-blocking the connection/reconnection blocks. 
 *  
**/

#ifndef IO_SOCK_H
#define IO_SOCK_H


    #ifdef COMPILE_FOR_WINDOWS
        #include <winsock2.h> 
        #include <ws2tcpip.h>
    #endif  
    #ifdef COMPILE_FOR_LINUX
        #include <sys/socket.h>
        #include <netinet/in.h>  
        #include <errno.h>     
        #include <string.h>     // strcpy
        #include <fcntl.h>      // fcntl
        #include <unistd.h>     // close   
        #include <arpa/inet.h>  // inet_addr  
    #endif  
    #include <stdint.h>
    #include <stdio.h>

    typedef struct io_sock_s {
      int sockfd;
      int portno;
      int servsockfd; 
      socklen_t clilen;
      struct sockaddr_in serv_addr;
      struct sockaddr_in cli_addr;
      uint8_t connectToRemoteServer;
      char address[20];
      #ifdef COMPILE_FOR_WINDOWS      
          WSADATA wsaData;
          unsigned long on; 
          int iResult;          
      #endif        
    } io_sock_s;
        
#endif // IO_SOCK_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_SOCK_IMPLEMENTATION
#undef IO_SOCK_IMPLEMENTATION


//#ifdef COMPILE_FOR_WINDOWS

    #define io_sock_bzero(b,len) (memset((b), '\0', (len)), (void) 0)  
  
    uint8_t io_sock_initComs(io_sock_s *s, uint16_t chConfig, char *adConfig) {
          
        s-> portno = (int) chConfig;        
        s-> connectToRemoteServer = 0;
        #ifdef COMPILE_FOR_WINDOWS
            s-> on = 1;
        #endif
        #ifdef COMPILE_FOR_LINUX
            int flags;
        #endif
        
        strcpy(s-> address, adConfig);      
        
        if (s-> address[0] == '\0') printf("No address. Act as server listing on port: %i\n", s-> portno);
        else {
            printf("Connecting to server with address %s on port %i", s-> address, s-> portno);
        }
        
        #ifdef COMPILE_FOR_WINDOWS 
            // Initialize Winsock
            s-> iResult = WSAStartup(MAKEWORD(2,2), &(s-> wsaData));
            if (s-> iResult != 0) printf("WSAStartup failed with error: %d\n", s-> iResult);
        #endif  
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
                #ifdef COMPILE_FOR_WINDOWS
                    closesocket(s-> sockfd);
                    WSACleanup();
                #endif  
                #ifdef COMPILE_FOR_LINUX 
                    close(s-> sockfd);
                #endif 
            }  

            // 5 is the maximum number of connections (backlog). 
            if (listen(s-> sockfd, 5) < 0){
	            printf("Socket listen error\n");
                #ifdef COMPILE_FOR_WINDOWS   
                    closesocket(s-> sockfd);                              
                    WSACleanup();
                #endif      
                #ifdef COMPILE_FOR_LINUX 
                    close(s-> sockfd);
                #endif                                
	        }
            s-> clilen = sizeof(s-> cli_addr);
            s-> servsockfd = accept(s-> sockfd, (struct sockaddr *) &(s-> cli_addr), &(s-> clilen));
            if (s-> servsockfd < 0) {
	             printf("ERROR on accept");
                 #ifdef COMPILE_FOR_WINDOWS                  
		             WSACleanup();
                 #endif                     
	        }
	        else {
                // set to non-blocking
                #ifdef COMPILE_FOR_WINDOWS  
                    ioctlsocket(s-> servsockfd, FIONBIO, &(s-> on));  
                #endif   
                #ifdef COMPILE_FOR_LINUX                
                    flags = fcntl(s-> servsockfd, F_GETFL, 0);
                    fcntl(s-> servsockfd, F_SETFL, flags & (~O_NONBLOCK));
                #endif                     
            }	  
	    }
	    else {
		    // we have an ip address for a server to connect to, so connect to it. 
		    s-> connectToRemoteServer = 1;
		    s-> serv_addr.sin_addr.s_addr = inet_addr(s-> address);
		    if (connect(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) != 0 ){
                printf("Connect error when connecting to server\r");
                #ifdef COMPILE_FOR_WINDOWS   
                    closesocket(s-> sockfd);              
                    WSACleanup();
                #endif  
                #ifdef COMPILE_FOR_LINUX 
                    close(s-> sockfd);
                #endif                   
		    }
		    else {
                // set to non-blocking
                #ifdef COMPILE_FOR_WINDOWS  
                    ioctlsocket(s-> sockfd, FIONBIO, &(s-> on));  
                #endif   
                #ifdef COMPILE_FOR_LINUX                
                    flags = fcntl(s-> sockfd, F_GETFL, 0);
                    fcntl(s-> sockfd, F_SETFL, flags & (~O_NONBLOCK));
                #endif                     
            } 	  
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
            #ifdef COMPILE_FOR_WINDOWS 
                nError = WSAGetLastError(); 
                if (nError == WSAEWOULDBLOCK) return 0;
            #endif 
            #ifdef COMPILE_FOR_LINUX
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) return 0;
            #endif 
	    }
	    #ifdef COMPILE_FOR_WINDOWS 
            if (nError != WSAEWOULDBLOCK) nError = 1;
            else nError = 0;
        #endif 
        #ifdef COMPILE_FOR_LINUX 
            if ((errno != EAGAIN) || (errno != EWOULDBLOCK)) nError = 1;
            else nError = 0;
        #endif 
        if ((n == 0) || ((n == -1) && (nError))) {
            printf("\n Disconnected - reconnecting \n"); 
			if (s-> connectToRemoteServer == 0) {
                #ifdef COMPILE_FOR_LINUX 
                    close(s-> servsockfd);
                #endif 
                #ifdef COMPILE_FOR_WINDOWS 
                    closesocket(s-> servsockfd);
                #endif    
            }
            #ifdef COMPILE_FOR_WINDOWS 
                closesocket(s-> sockfd);
                WSACleanup();
            #endif 
            #ifdef COMPILE_FOR_LINUX 
                close(s-> sockfd);
            #endif 
              
            io_sock_initComs(s, s-> portno, s-> address);                
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

//#endif  // COMPILE_FOR_WINDOWS


#endif // IO_SOCK_IMPLEMENTATION

