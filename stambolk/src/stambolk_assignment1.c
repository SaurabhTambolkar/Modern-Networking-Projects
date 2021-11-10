/**
 * @stambolk@buffalo.edu_assignment1
 * @author  Saurabh Tambolkar <stambolk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */

#include "../include/global.h"
#include "../include/logger.h"

//********************************************* FUNCTIONS DEFINATION ******************************************************************************************

/*
 * This Method Converts String to Number
*/
 int StringToNumber(const char* InputString){
     int RetVal = 0;

     for (int i=0;i<strlen(InputString);i++){
         int t = InputString[i]-'0';

         if  (t<0 || t >9)
		 return -1;

         RetVal = RetVal * 10 + t;
     }
     return RetVal;
 }


 /*
  * This method checks whehter Sender is blocked
  * return 0 ,if it's not blocked
  * return 1 ,if it's blocked
  */
 int CheckSenderBlocked(int Sender, char *Receiver){
     int RetVal = 0;

     char Sender_Address[INET_ADDRSTRLEN] = "";

     int i=0;
     while(i < connIndex){
         if(connections[i].connsockfd == Sender){
             strcpy(Sender_Address, connections[i].remote_addr);
             break;
         }
	 i++;
     }


     i = 0;
     while(i < connIndex){

         if(strcmp(connections[i].remote_addr, Receiver) == 0){
             for(int j=0; j < connections[i].blockindex; j++)
	     {
                 if(strcmp(connections[i].blockedIPs[j]->remote_addr, Sender_Address) == 0)
		 {
                     RetVal = 1;
                     break;
                 }
             }
             break;
         }
	 i++;
     }

     return RetVal;
 }


/*
 * This Method Creates Socket and tryto listen to it
*/

 int CreateSocketProc(const char *port){
     struct addrinfo hints, *servinfo, *p;

     memset(&hints, 0, sizeof hints);
     hints.ai_family = AF_INET;
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_flags = AI_PASSIVE;

     if(getaddrinfo(NULL, port, &hints, &servinfo) == -1){
         return -1;
     }

     for(p = servinfo; p != NULL; p=p->ai_next){
         if((localsockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
             continue;
         }

         if(bind(localsockfd, p->ai_addr, p->ai_addrlen) == -1){
             close(localsockfd);
             continue;
         }
         break;
     }

     if(p == NULL) return -1;
     freeaddrinfo(servinfo);

     if(listen(localsockfd, BACKLOG) == -1){
         return -1;
     }

     return 0;
 }


 /*
  * This Method Pack the list,
  * Input Should be an empty String,
  */
 void ListPack(char *List_to_output){

     for (int i=0; i<connIndex; i++) {

         if(connections[i].status == loged_in){

	     char temp_status[5];
             char temp_port[PORTSTRLEN];

             strcat(List_to_output, connections[i].hostname);
             strcat(List_to_output, "---");

             strcat(List_to_output, connections[i].remote_addr);
             strcat(List_to_output, "---");

             sprintf(temp_port, "%d", connections[i].portNum);
	     strcat(List_to_output, temp_port);
             strcat(List_to_output, "---");

             sprintf(temp_status, "%d", connections[i].status);             
	     strcat(List_to_output, temp_status);
             strcat(List_to_output, "---");

         }
     }
 }

 /*
  * The method checks if given addr and port is valid
  * return 1 if it's valid
  * return 0 if it's invalid
  */
 int IsAddressAndPortValid(char *Address, char *Port_Number){

     int RetVal = 1;
     // Checking for IP Address
     for(int i=0; i < CMD_SIZE; i++){

         if(*(Address+i) == '\0')
		 break;

         if(*(Address+i) == '.')
		 continue;

         int c = *(Address+i) - '0';

         if( c <0 || c >9)
	 {
             RetVal = 0;
             break;
         }
     }

     // Checking for PORT Number
     for(int i=0; i < CMD_SIZE; i++)
     {
         if(*(Port_Number+i) == '\0') 
		break;

         int c = *(Port_Number+i) - '0';

         if( c < 0 || c > 9) {
             RetVal = 0;
             break;
         }

     }

     return RetVal;
 }

// This function gives local ip address
int get_localIP(char *res){

     struct addrinfo hints, *servinfo, *p;

     int Socket_FD = 0;

     struct sockaddr myip;

     socklen_t len = sizeof(myip);

     memset(&hints, 0, sizeof(hints));

     hints.ai_family = AF_UNSPEC;

     hints.ai_socktype = SOCK_DGRAM;

     if(getaddrinfo("8.8.8.8", "53", &hints, &servinfo) != 0)
     {
         return -1;
     }

     for(p=servinfo; p!= NULL; p=p->ai_next)
     {	
         if((Socket_FD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
             continue;
         }

         if(connect(Socket_FD, p->ai_addr, p->ai_addrlen) == -1){
             close(Socket_FD);
             continue;
         }
         break;
     }

     if(p == NULL){
         return -1;
     }

     freeaddrinfo(servinfo);

     getsockname(Socket_FD, &myip, &len);

     inet_ntop(AF_INET, &(((struct sockaddr_in *)&myip)->sin_addr), res, INET6_ADDRSTRLEN);

     close(Socket_FD);

     return 0;
 }


 void ListUnPack(char *list){

     char *Segment[20];
     char *p;
     int count = 0;

     p = strtok(list, "---");

     while (p != NULL) {

         Segment[count++] = p;
         p = strtok(NULL, "---");

     }

     if (connIndex != 0)
	 connIndex = 0;
    
     int i = 0;
     while( i<count )
     {

         strcpy(connections[connIndex].hostname, Segment[i++]);
         strcpy(connections[connIndex].remote_addr, Segment[i++]);

         int Temp = StringToNumber(Segment[i++]);

         connections[connIndex].portNum = Temp;

         Temp = StringToNumber(Segment[i++]);

         connections[connIndex++].status = Temp;
     }

 }



int ValidateToList(char **command, int count)
{
   if(strcmp(command[0], "LIST") == 0){
         	 int count = 1;

	  	 cse4589_print_and_log("[%s:SUCCESS]\n", command[0]);
		 fflush(stdout);

         int i = 0;
         while(i<connIndex){
             if(connections[i].status == loged_in){

		 cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", count++, connections[i].hostname, connections[i].remote_addr, connections[i].portNum);
		 fflush(stdout);

             }
	     i++;
         }
		 cse4589_print_and_log("[%s:END]\n", command[0]);
         	 fflush(stdout);
       return 1;
     }
     return 0;
}



int ValidateBasicCommnands(char **command)
{
   if(strcmp(command[0], "IP") == 0){

         char localip[INET6_ADDRSTRLEN];

         get_localIP(localip);

		 cse4589_print_and_log("[%s:SUCCESS]\n", command[0]);
		 fflush(stdout);

		 cse4589_print_and_log("IP:%s\n", localip);
		 fflush(stdout);

		 cse4589_print_and_log("[%s:END]\n", command[0]);
		 fflush(stdout);
		 return 1;

     }     
     else if (strcmp(command[0], "AUTHOR") == 0) {

		 cse4589_print_and_log("[%s:SUCCESS]\n", command[0]);
		 fflush(stdout);

		 cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "stambolk");
		 fflush(stdout);

		 cse4589_print_and_log("[%s:END]\n", command[0]);
		 fflush(stdout);
		 return 1;

     }else if(strcmp(command[0], "PORT") == 0){

		 cse4589_print_and_log("[%s:SUCCESS]\n", command[0]);
		 fflush(stdout);

		 cse4589_print_and_log("PORT:%d\n", StringToNumber(listenerPort));
		 fflush(stdout);

		 cse4589_print_and_log("[%s:END]\n", command[0]);
		 fflush(stdout);
		 return 1;
    }
    return 0;
}





/*
  * Start select for cmd and connecting.
  */
 void ReadCommandsAndStartProcessing(void){
     char *argm[5];

     FD_ZERO(&master);
     FD_ZERO(&read_fds);
     FD_SET(STD_IN, &master);
     FD_SET(localsockfd, &master);
     maxfd = localsockfd;

     while (1) {
         read_fds = master;

         if(select(maxfd+1, &read_fds, NULL, NULL, NULL) == -1){
             return;
         }

         for(int i=0 ;i < maxfd+1; i++){
             if(FD_ISSET(i, &read_fds)){
                 // collect input
                 if(i == STD_IN){

                     char *cmd = (char *)malloc(sizeof(char)*CMD_SIZE);
                     memset(cmd, '\0', CMD_SIZE);

                     fgets(cmd, CMD_SIZE-1, stdin);

                     for(int j =0; j<CMD_SIZE; j++){
                         if(cmd[j] == '\n'){
                             cmd[j] = '\0';
                             break;
                         }
                     }

                     int count = 0;
                     char *tmp = strtok(cmd, " ");
                     while(tmp != NULL){
                         argm[count++] = tmp;
                         tmp = strtok(NULL, " ");
                     }

                     ProcessCommands(argm, count);
                 }else if(i == localsockfd && role == 1){
                     // process new connections, use a data structure to store info
                     struct sockaddr_storage remoteaddr;
                     socklen_t len = sizeof(remoteaddr);
                     int newfd = accept(localsockfd, (struct sockaddr *)&remoteaddr, &len);
                     if(newfd == -1){
 //                        flag = -1;
                         continue;
                     }
                     FD_SET(newfd, &master);
                     maxfd = maxfd > newfd? maxfd: newfd;

					 char clientPort[PORTSTRLEN];
					 // bug: different length between client and server
					 recv(newfd, clientPort, PORTSTRLEN, 0);
                     char tmp[INET_ADDRSTRLEN];
                     inet_ntop(AF_INET, &(((struct sockaddr_in *)&remoteaddr)->sin_addr), tmp, INET_ADDRSTRLEN);
                     struct hostent *he;
                     struct in_addr ipv4addr;
                     inet_pton(AF_INET, tmp, &ipv4addr);
                     he = gethostbyaddr(&ipv4addr, sizeof(struct in_addr), AF_INET);
                     int exist = 0;
                     for(int i=0;i<connIndex;i++){
                         if(strcmp(connections[i].remote_addr, tmp) == 0){
                             exist = 1;
                             connections[i].status = loged_in;
                             break;
                         }
                     }
                     if(!exist){
                         struct connection newConnection;
                         newConnection.connsockfd = newfd;
                         strcpy(newConnection.remote_addr, tmp);
                         //newConnection.portNum = ((struct sockaddr_in *)&remoteaddr)->sin_port;
						 newConnection.portNum = StringToNumber(clientPort);
                         strcpy(newConnection.hostname, he->h_name);
                         newConnection.msg_sent = 0;
                         newConnection.msg_received = 0;
                         newConnection.status = loged_in;
                         newConnection.blockindex = 0;
                         connections[connIndex++] = newConnection;
                     }
                     // sort the array
                     if(connIndex > 1){
                         for(int cur = 0; cur< connIndex-1; cur++){
                             for(int fast = cur+1; fast<connIndex; fast++){
                                 if(connections[cur].portNum > connections[fast].portNum){
                                     struct connection tmp = connections[cur];
                                     connections[cur] = connections[fast];
                                     connections[fast] = tmp;
                                 }
                             }
                         }
                     }
                     // afterwards redirect the array to newly connected client
                     char list[BUFLEN] = "";
                     ListPack(list);
                     send(newfd, list, BUFLEN, 0);


                     //prepare buffered msg for that client
                     int count = 0;
                     char *bufmsg[BUFLEN];
                     char *p;
                     p = strtok(bufferedmsg, "---");
                     while (p!=NULL) {
                         bufmsg[count++] = p;
                         p = strtok(NULL, "---");
                     }

                     char newBufferedmsg[BUFLEN]="";
                     char sendingmsg[BUFLEN]="";

					 int flag = 0;
                     for (int i=0; i<count; ) {
                         if (strcmp(tmp, bufmsg[i]) == 0) {
							 if(flag == 0){
								 strcat(sendingmsg, "BROADCAST,");
								 strcat(sendingmsg, "---");
								 flag = 1;
							 }
                             strcat(sendingmsg, bufmsg[i+1]);
                             strcat(sendingmsg, "---");
                             strcat(sendingmsg, bufmsg[i+2]);
                             strcat(sendingmsg, "---");
							 cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
							 fflush(stdout);
							 cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", bufmsg[i+1], bufmsg[i], bufmsg[i+2]);
							 fflush(stdout);
							 cse4589_print_and_log("[%s:END]\n", "RELAYED");
							 fflush(stdout);
							 i+=3;
                         } else if(strcmp(bufmsg[i], "255.255.255.255") == 0){
							 if(flag == 0){
								 strcat(sendingmsg, "BROADCAST,");
								 strcat(sendingmsg, "---");
								 flag = 1;
							 }
                             strcat(newBufferedmsg, "255.255.255.255");
                             strcat(newBufferedmsg, "---");

                             strcat(newBufferedmsg, bufmsg[i+1]);
                             strcat(newBufferedmsg, "---");

                             strcat(newBufferedmsg, bufmsg[i+2]);
                             strcat(newBufferedmsg, "---");

                             strcat(sendingmsg, bufmsg[i+1]);
                             strcat(sendingmsg, "---");

                             strcat(sendingmsg, bufmsg[i+2]);
                             strcat(sendingmsg, "---");

			     cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
			     fflush(stdout);

			     cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", bufmsg[i+1], bufmsg[i], bufmsg[i+2]);
			     fflush(stdout);

			     cse4589_print_and_log("[%s:END]\n", "RELAYED");
			     fflush(stdout);

			     i+=3;

                         } else{
                             strcat(newBufferedmsg, bufmsg[i++]);
                             strcat(newBufferedmsg, "---");

                             strcat(newBufferedmsg, bufmsg[i++]);
                             strcat(newBufferedmsg, "---");

                             strcat(newBufferedmsg, bufmsg[i++]);
                             strcat(newBufferedmsg, "---");
                         }
                     }
                     strcpy(bufferedmsg, newBufferedmsg);
				
                     send(newfd, sendingmsg, BUFLEN, 0);

                 }else if(role == 0 && i == clientsockfd){
 			         char buf[BUFLEN];
                     int nbytes = recv(i, buf, sizeof buf, 0);
					 
					 char flag[10]="";
					if(nbytes > 1){
						strncpy(flag, buf, 10);
					 }

					 if(strcmp(flag, "BROADCAST,") == 0){
						char *tmp;
	 					int count = 0;
	 					char *msgset[BUFLEN];
	 					tmp = strtok(buf, "---");
	 					while (tmp != NULL) {
	 						msgset[count++] = tmp;
	 						tmp = strtok(NULL, "---");
	 					}
					

	 					for(int i=1;i<count;){
	 						cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
	 						fflush(stdout);
	 						cse4589_print_and_log("msg from:%s\n[msg]:%s\n", msgset[i], msgset[i+1]);
	 						fflush(stdout);
	 						cse4589_print_and_log("[%s:END]\n", "RECEIVED");
	 						fflush(stdout);
	 						i+=2;
	 					}


					}else{
	                     char *tmp;
	                     int count = 0;
	                     char *msgset[BUFLEN];
	                     tmp = strtok(buf, " ");
	                     while (tmp != NULL) {
	                         msgset[count++] = tmp;
	                         tmp = strtok(NULL, " ");
	                     }

						 char recvmsg[MSGLEN] = "";
						 for(int n = 1; n< count; n++){
							strcat(recvmsg, msgset[n]);
							if(n<count-1) strcat(recvmsg, " ");
						 }

						 cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
						 fflush(stdout);
						 cse4589_print_and_log("msg from:%s\n[msg]:%s\n", msgset[0], recvmsg);
						 fflush(stdout);
						 cse4589_print_and_log("[%s:END]\n", "RECEIVED");
						 fflush(stdout);
						 }
                     // triger event cse4589
                     // recv from server to receive msg from others
                 }else{
                     char buf[BUFLEN];
                     ssize_t nbytes = recv(i, buf, sizeof buf, 0);
                     buf[nbytes] = '\0';

                     int count = 0;
                     char *p;
                     char *arguments[BUFLEN];
                     p = strtok(buf, " ");
                     while (p != NULL) {
                         arguments[count++] = p;
                         p = strtok(NULL, " ");
                     }

					 char msg[MSGLEN] = "";
					 if(strcmp(arguments[0], "SEND") == 0){
						 for(int i=2; i<count; i++){
							strcat(msg, arguments[i]);
							if(i < count-1) strcat(msg, " ");
						 }
						 count = 3;
						 arguments[count-1] = msg;
					 }

					 if(strcmp(arguments[0], "BROADCAST") == 0){
						for(int i=1; i<count; i++){
							strcat(msg, arguments[i]);
							if(i < count-1) strcat(msg, " ");
						 }
						 count = 2;
						 arguments[count-1] = msg;
					 }

 		            GiveResponseToClients(arguments, count, i);
                     // recv from connected socket, also need to consider argument,send, refresh or block.

                 }
             }
         }
     }
 }
 

 

 /**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{

	cse4589_init_log(argv[2]);


	fclose(fopen(LOGFILE, "w"));

	strcpy(bufferedmsg,"");
	role = 0;
	connIndex = 0;
	logedin = 0;

	if (argc != 3) {
		printf("usage: ./chat_app s/c #port\n");
		return 1;
	}


	if(strcmp(argv[1], "s")==0) 
		role = 1;
	else if(strcmp(argv[1], "c")==0) 
		role = 0;
	else 
		return -1;


	strcpy(listenerPort, argv[2]);

	CreateSocketProc(argv[2]);

	ReadCommandsAndStartProcessing();

	return 0;
}
