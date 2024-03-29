#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */

#define SNDBUFSIZE 976   /* Size of sending buffer */
#include <string.h> // for memset
#include <time.h>

//for UDP connection, probably neccessary
#include <sys/types.h>
#include <netdb.h>
#include "Practical.h"


void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket, char *trigAcptPort, int PrintStatus)
{
  int recvMsgSize;                    /* Size of received message */
  unsigned char sndBuffer[SNDBUFSIZE];/* Buffer for data string to be sent */
  memset(sndBuffer,73,sizeof(sndBuffer));
  /***************************************/
  //  Definition of signal receiver port 
  /***************************************/
  // Construct the server address structure
  //char *service = trigAcptPort; // First arg:  local port/service
  //printf("Trigger will be accepted with the port # %s\n",service);
  printf("Trigger receive port is set to # %s\n",trigAcptPort);
  
  struct addrinfo addrCriteria;                   // Criteria for address
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram socket
  addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP socket
  struct addrinfo *servAddr;                      // List of server addresses
  //int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
  int rtnVal = getaddrinfo(NULL, trigAcptPort, &addrCriteria, &servAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  /***************************************/
  //  Creation of signal receiver port 
  /***************************************/
  // Create socket for incoming connections
  int sock = socket(servAddr->ai_family, servAddr->ai_socktype,
      servAddr->ai_protocol);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  // Bind to the local address
  if (bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
    DieWithSystemMessage("bind() failed");
  
  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  
  /***************************************/
  //  Handling process
  /***************************************/
  for(;;)
    {
      /***preparation for trigger acception***/
      // Client address
      struct sockaddr_storage clntAddr; 
      // Set Length of client address structure (in-out parameter)
      socklen_t clntAddrLen = sizeof(clntAddr);
      // Size of trigger signal message
      char buffer[MAXSTRINGLENGTH]; // I/O buffer

      /***trigger acception***/
      // (Block until receive message from a client)
      ssize_t numBytesRcvd = recvfrom(sock, buffer, MAXSTRINGLENGTH, 0,
				      (struct sockaddr *) &clntAddr, &clntAddrLen);
      if (numBytesRcvd < 0)
	DieWithSystemMessage("recvfrom() failed");
      
      /***trigger acception message***/
      if(PrintStatus==1)
	{
	  fputs("Trigger signal is accepted from ", stdout);
	  PrintSocketAddress((struct sockaddr *) &clntAddr, stdout);
	  fputc('\n', stdout);
	}
      /***sending data***/   
      if(send(clntSocket, sndBuffer, sizeof(sndBuffer), 0)<0)
	break;//DieWithError("Send error \n");
    }
	  
  close(clntSocket);    /* Close client socket */
}
