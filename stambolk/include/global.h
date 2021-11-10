#ifndef GLOBAL_H_
#define GLOBAL_H_

//****************************************************** INCLUDE FILES ***********************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>


//*************************************************** PREPROCESSORS ***************************************************************************************
 #define CMD_SIZE 100

 #define BUFLEN 1024

 #define MSGLEN 256

 #define BACKLOG 4

 #define STD_IN 0

 #define HOSTNAMESTRLEN 50

 #define PORTSTRLEN 10

 #define loged_in 1

 #define loged_out 0

#define HOSTNAME_LEN 128

#define PATH_LEN 256


//*********************************************** STRUCTURES ***********************************************************************************************
struct connection {

     char hostname[HOSTNAMESTRLEN];

     char remote_addr[INET_ADDRSTRLEN];

     int portNum;

     int msg_received;

     int msg_sent;

     int status;

     int blockindex;

     struct connection *blockedIPs[3];

     int connsockfd;
 };


//******************************************** GLOBAL VARIABLES ********************************************************************************************

 char listenerPort[PORTSTRLEN];

 char bufferedmsg[BUFLEN];

 int role;

 struct connection connections[4];

 int localsockfd;

 int clientsockfd;

 fd_set master, read_fds;

 int connIndex;

 int logedin; 

 int maxfd;


//************************************************* Function Declarations ***********************************************************************************

int StringToNumber(const char* InputString);

int CreateSocketProc(const char *port);

int IsAddressAndPortValid(char *Address, char *Port_Number);

void ListPack(char *list);

int get_localIP(char *res);

void ListUnPack(char *list);

int CheckSenderBlocked(int Sender, char *Receiver);

int ValidateBasicCommnands(char **cmd);

int ValidateToList(char **command, int count);

void ProcessCommands(char **cmd, int count);

void GiveResponseToClients(char **arguments, int count, int caller);

void ReadCommandsAndStartProcessing(void);

#endif
