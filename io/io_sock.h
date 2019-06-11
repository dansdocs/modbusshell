/**
 * Daniel 2018
 *
 * a server can only take a single connection - point to point. 
 * while get and send data on an open port is non-blocking the connection/reconnection blocks. 
 *  
**/

#ifndef IO_SOCK_H
#define IO_SOCK_H


    #ifdef BUILD_FOR_WINDOWS
        #include <winsock2.h> 
        #include <ws2tcpip.h>
    #endif  
    #ifdef BUILD_FOR_LINUX
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
      uint8_t state; 
      socklen_t clilen;
      struct sockaddr_in serv_addr;
      struct sockaddr_in cli_addr;
      uint8_t actAsServer;
      char address[20];
      #ifdef BUILD_FOR_WINDOWS      
          WSADATA wsaData;
          unsigned long on; 
          int iResult; 
          int nError;         
      #endif        
    } io_sock_s;
    
    // type of function to pass in for log messages. 
    //uint8_t fid, uint8_t lvl, const char *fmt, ...
    typedef void (*io_sock_logFnT)(uint8_t, uint8_t, const char *, ...);
    
    void io_sock_setLogFn(io_sock_logFnT fn);
        
#endif // IO_SOCK_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef IO_SOCK_IMPLEMENTATION
#undef IO_SOCK_IMPLEMENTATION

    enum io_sock_CONNECTSTATES {
        io_sock_UNINITIALISED, 
        io_sock_INITSERVER, 
        io_sock_SERVERCONNECTWAIT, 
        io_sock_SERVERCONNECTED,
        io_sock_NULL
    };

   // Infrustructure for logging.  
    #define _IO_SOCK_FID ((uint8_t)(('i' << 2) + 'o' + 's'))
    enum { IO_SOCK_LOG_TRACE, IO_SOCK_LOG_DEBUG, IO_SOCK_LOG_INFO, IO_SOCK_LOG_WARN, IO_SOCK_LOG_ERROR, IO_SOCK_LOG_FATAL };    
    void _io_sock_dummylogFn (uint8_t fid, uint8_t lvl, const char *fmt, ...){;}    
    io_sock_logFnT io_sock_logFn = &_io_sock_dummylogFn;
    void io_sock_setLogFn(io_sock_logFnT fn){
        io_sock_logFn = fn;
        io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_INFO, "%02x = io/io_sock.h", _IO_SOCK_FID);    
    }     
    
    #define io_sock_bzero(b,len) (memset((b), '\0', (len)), (void) 0) 

    uint8_t _io_sock_initServer(io_sock_s *s) {

        if(s->state == io_sock_INITSERVER){
            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_TRACE, "Initialising server");
             
            #ifdef BUILD_FOR_WINDOWS 
                // Initialize Winsock
                s-> iResult = WSAStartup(MAKEWORD(2,2), &(s-> wsaData));
                if (s-> iResult != 0) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "WSAStartup failed with error: %d", s-> iResult);
            #endif  
            s-> sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (s-> sockfd < 0) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR opening socket");
            io_sock_bzero((char *) &(s-> serv_addr), sizeof(s-> serv_addr));
            s-> serv_addr.sin_family = AF_INET;
            s-> serv_addr.sin_port = htons(s-> portno);
            
            // we are the server and need to listen for an incoming connection.       
		    s-> actAsServer = 1;
            s-> serv_addr.sin_addr.s_addr = INADDR_ANY;
            if (bind(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) < 0) {
 	            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR on binding");
                #ifdef BUILD_FOR_WINDOWS
                    closesocket(s-> sockfd);
                    WSACleanup();
                #endif  
                #ifdef BUILD_FOR_LINUX 
                    close(s-> sockfd);
                #endif 
            }  
             
             // 5 is the maximum number of connections (backlog). 
            if (listen(s-> sockfd, 5) < 0){
	            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Socket listen error");
                #ifdef BUILD_FOR_WINDOWS   
                    closesocket(s-> sockfd);                              
                    WSACleanup();
                #endif      
                #ifdef BUILD_FOR_LINUX 
                    close(s-> sockfd);
                #endif                                
	        }
            else {
                s->state = io_sock_SERVERCONNECTWAIT;
                s-> clilen = sizeof(s-> cli_addr);
                // set to non-blocking
                #ifdef BUILD_FOR_WINDOWS  
                    ioctlsocket(s-> sockfd, FIONBIO, &(s-> on));  
                #endif   
                #ifdef BUILD_FOR_LINUX                
                    fcntl(s-> sockfd, F_SETFL, O_NONBLOCK);
                #endif 
            }
        }     

        if(s->state == io_sock_SERVERCONNECTWAIT){
            s->nError = 0;
            s-> servsockfd = accept(s-> sockfd, (struct sockaddr *) &(s-> cli_addr), &(s-> clilen));
            if (s-> servsockfd < 0) {
	            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR on accept");
                s->nError = WSAGetLastError(); 
                if (s->nError == WSAEWOULDBLOCK) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Would block");
                else {
                    #ifdef BUILD_FOR_WINDOWS                  
		                WSACleanup();
                    #endif
                }                     
	        } else {
                // set to non-blocking
                #ifdef BUILD_FOR_WINDOWS  
                    ioctlsocket(s-> servsockfd, FIONBIO, &(s-> on));  
                #endif   
                #ifdef BUILD_FOR_LINUX                
                    fcntl(s-> servsockfd, F_SETFL, O_NONBLOCK);
                #endif                     
            }
            if (s->nError != WSAEWOULDBLOCK) {
                s->state = io_sock_SERVERCONNECTED;
                io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_INFO, "CONECTED %d", s->portno);
            }
        }	  
	    return 0;
    }


    uint8_t io_sock_initComs(io_sock_s *s, uint16_t chConfig, char *adConfig) {
          
        s-> portno = (int) chConfig;        
        #ifdef BUILD_FOR_WINDOWS
            s-> on = 1;
        #endif
        
        strcpy(s-> address, adConfig);      
        
        if (s-> address[0] == '\0') {
            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_INFO, "No address, will act as server listing on port: %d", s-> portno);
            s-> state = io_sock_INITSERVER;
            s-> actAsServer = 1;            
            _io_sock_initServer(s);
        }
        else {
            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_INFO, "Connecting to server with address %s on port %d", s-> address, s-> portno);
            s-> actAsServer = 0;
        }
        
    //    #ifdef BUILD_FOR_WINDOWS 
    //        // Initialize Winsock
    //        s-> iResult = WSAStartup(MAKEWORD(2,2), &(s-> wsaData));
    //        if (s-> iResult != 0) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "WSAStartup failed with error: %d", s-> iResult);
    //    #endif  
    //    s-> sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //    if (s-> sockfd < 0) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR opening socket");
    //    io_sock_bzero((char *) &(s-> serv_addr), sizeof(s-> serv_addr));
    //    s-> serv_addr.sin_family = AF_INET;
    //    s-> serv_addr.sin_port = htons(s-> portno);
    //
    //    // if address is a null string then we don't have server ip address to connect to 
    //    // so we are the server and need to listen for an incoming connection.       
    //    if (s-> address[0] == '\0'){
	//	    s-> connectToRemoteServer = 0;
    //        s-> serv_addr.sin_addr.s_addr = INADDR_ANY;
    //        if (bind(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) < 0) {
 	//            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR on binding");
    //            #ifdef BUILD_FOR_WINDOWS
    //                closesocket(s-> sockfd);
    //                WSACleanup();
    //            #endif  
    //            #ifdef BUILD_FOR_LINUX 
    //                close(s-> sockfd);
    //            #endif 
    //        }  
    //
    //        // 5 is the maximum number of connections (backlog). 
    //        if (listen(s-> sockfd, 5) < 0){
	//            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Socket listen error");
    //            #ifdef BUILD_FOR_WINDOWS   
    //                closesocket(s-> sockfd);                              
    //                WSACleanup();
    //            #endif      
    //            #ifdef BUILD_FOR_LINUX 
    //                close(s-> sockfd);
    //            #endif                                
	//        }
    //        else {
    //            // set to non-blocking
    //            #ifdef BUILD_FOR_WINDOWS  
    //                ioctlsocket(s-> sockfd, FIONBIO, &(s-> on));  
    //            #endif   
    //            #ifdef BUILD_FOR_LINUX                
    //                fcntl(s-> sockfd, F_SETFL, O_NONBLOCK);
    //            #endif 
    //        }
    //
    //        io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_TRACE, "Before blocking");
    //        s-> clilen = sizeof(s-> cli_addr);
    //        nError = WSAEWOULDBLOCK;
    //        while (nError == WSAEWOULDBLOCK){
    //            nError = 0;
    //            s-> servsockfd = accept(s-> sockfd, (struct sockaddr *) &(s-> cli_addr), &(s-> clilen));
    //            if (s-> servsockfd < 0) {
	//                io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "ERROR on accept");
    //                nError = WSAGetLastError(); 
    //                if (nError == WSAEWOULDBLOCK) io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Would block");
    //                else {
    //                     #ifdef BUILD_FOR_WINDOWS                  
	//	                    WSACleanup();
    //                     #endif
    //                }                     
	//            }
	//            else {
    //                // set to non-blocking
    //                #ifdef BUILD_FOR_WINDOWS  
    //                    ioctlsocket(s-> servsockfd, FIONBIO, &(s-> on));  
    //                #endif   
    //                #ifdef BUILD_FOR_LINUX                
    //                    fcntl(s-> servsockfd, F_SETFL, O_NONBLOCK);
    //                #endif                     
    //            }
    //            Sleep(800);
    //        }
    //        io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_TRACE, "AFTER blocking");	  
	//    }
	//    else {
	//	    // we have an ip address for a server to connect to, so connect to it. 
	//	    s-> connectToRemoteServer = 1;
	//	    s-> serv_addr.sin_addr.s_addr = inet_addr(s-> address);
	//	    if (connect(s-> sockfd, (struct sockaddr *) &(s-> serv_addr), sizeof(s-> serv_addr)) != 0 ){
    //            io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Connect error when connecting to server");
    //            #ifdef BUILD_FOR_WINDOWS   
    //                closesocket(s-> sockfd);              
    //                WSACleanup();
    //            #endif  
    //            #ifdef BUILD_FOR_LINUX 
    //                close(s-> sockfd);
    //            #endif                   
	//	    }
	//	    else {
    //            // set to non-blocking
    //            #ifdef BUILD_FOR_WINDOWS  
    //                ioctlsocket(s-> sockfd, FIONBIO, &(s-> on));  
    //            #endif   
    //            #ifdef BUILD_FOR_LINUX                
    //                fcntl(s-> sockfd, F_SETFL, O_NONBLOCK);
    //            #endif                     
    //        } 	  
    //    }

	    return 0;
    }
  
    uint8_t io_sock_getByte(io_sock_s *s, uint8_t *rxByte){

        int n = 0;
        char rx;
        int nError;
     
        if (s->state == io_sock_SERVERCONNECTWAIT){
            _io_sock_initServer(s); 
        } 

        if (s->state == io_sock_SERVERCONNECTED){
            if (s-> actAsServer) n = recv(s-> servsockfd, &rx, 1, 0);
	        else n = recv(s-> sockfd, &rx, 1, 0);
             	  
	        if (n == -1) {
                #ifdef BUILD_FOR_WINDOWS 
                    nError = WSAGetLastError(); 
                    if (nError == WSAEWOULDBLOCK) return 0;
                #endif 
                #ifdef BUILD_FOR_LINUX
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) return 0;
                #endif 
	        }
	        #ifdef BUILD_FOR_WINDOWS 
                if (nError != WSAEWOULDBLOCK) nError = 1;
                else nError = 0;
            #endif 
            #ifdef BUILD_FOR_LINUX 
                if ((errno != EAGAIN) || (errno != EWOULDBLOCK)) nError = 1;
                else nError = 0;
            #endif 
            if ((n == 0) || ((n == -1) && (nError))) {
                io_sock_logFn(_IO_SOCK_FID, IO_SOCK_LOG_ERROR, "Disconnected - reconnecting"); 
	    		if (s-> actAsServer) {
                    #ifdef BUILD_FOR_LINUX 
                        close(s-> servsockfd);
                    #endif 
                    #ifdef BUILD_FOR_WINDOWS 
                        closesocket(s-> servsockfd);
                    #endif    
                }
                #ifdef BUILD_FOR_WINDOWS 
                    closesocket(s-> sockfd);
                    WSACleanup();
                #endif 
                #ifdef BUILD_FOR_LINUX 
                    close(s-> sockfd);
                #endif 
                  
                io_sock_initComs(s, s-> portno, s-> address);                
                return 0;
            }
            else {
	    	    *rxByte = (uint8_t)rx;
	    	    return 1;
	        }
        }
	    return 0;
    }
  
    uint8_t io_sock_sendByte(io_sock_s *s, uint8_t txByte){
        int n = 0;
        char tx = txByte; 
      
        // this assumes that a closed connection will be addressed by the getByte function being called often
        // it also assumes that data that isn't sent will only be due to a closed connection, not because too
        // much was being sent in one go. 
        if (s-> actAsServer) n = send(s-> servsockfd, &tx, 1, 0);
	    else n = send(s-> sockfd, &tx, 1, 0);

	    return (uint8_t) n;
    }

#endif // IO_SOCK_IMPLEMENTATION

