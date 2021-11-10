/**
 * @stambolk@buffalo.edu_assignment1
 * @author  Shantanu Kumar <skumar39@buffalo.edu>
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


 /*
  * This method Process Input commands
  */
 void ProcessCommands(char **cmd, int count){


     if(ValidateBasicCommnands(cmd) == 1)
     {
     }else if(ValidateToList(cmd,count) == 1){

     }else if(strcmp(cmd[0], "LOGIN") == 0){

         struct addrinfo hints, *servinfo, *p;

         if(role != 0 || count != 3 || !IsAddressAndPortValid(cmd[1], cmd[2]) || logedin){
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);

             		 return;
         }

         memset(&hints, 0, sizeof hints);
         hints.ai_family = AF_INET;
         hints.ai_socktype = SOCK_STREAM;

         if(getaddrinfo(cmd[1], cmd[2], &hints, &servinfo) != 0){
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }
         for(p=servinfo; p!= NULL; p=p->ai_next){
             if((clientsockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
                 continue;
             }
             if(connect(clientsockfd, p->ai_addr, p->ai_addrlen) == -1){
                 close(clientsockfd);
                 continue;
             }
             break;
         }
         if(p == NULL){

			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }
         freeaddrinfo(servinfo);
 		 send(clientsockfd, listenerPort, sizeof listenerPort, 0);

         char buf[BUFLEN];
         recv(clientsockfd, buf, BUFLEN, 0);
         ListUnPack(buf);

         char unread[BUFLEN];
         recv(clientsockfd, unread, BUFLEN, 0);
         char *tmp[BUFLEN];
         int count = 0;
         char *q = strtok(unread, "---");
         while (q!=NULL) {
             tmp[count++] = q;
             q = strtok(NULL, "---");
         }

         for(int i=1;i<count; ){
			 cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
			 fflush(stdout);
			 cse4589_print_and_log("msg from:%s\n[msg]:%s\n", tmp[i], tmp[i+1]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", "RECEIVED");
             fflush(stdout);
             i+=2;
         }

        logedin = 1;
        FD_SET(clientsockfd, &master);
 		maxfd = clientsockfd>maxfd? clientsockfd:maxfd;


		cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
		fflush(stdout);
		cse4589_print_and_log("[%s:END]\n", cmd[0]);
		fflush(stdout);

     }else if(strcmp(cmd[0], "SEND") == 0){
         if(role != 0 || !logedin || !IsAddressAndPortValid(cmd[1], "8888")){
             //fail
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }

         // check if recvier is in local list
         int flag = 0;
         for (int i=0; i<connIndex; i++) {
             if (strcmp(cmd[1], connections[i].remote_addr) == 0) {
                 flag = 1;
                 break;
             }
         }
         if(flag == 0) {
             //fail
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }

         char msg[MSGLEN] = "";
         for (int i=2;i<count;i++){
             strcat(msg, cmd[i]);
             if(i != count-1) strcat(msg, " ");
         }

         char buf[BUFLEN] = "";
 		 strcat(buf, cmd[0]);
         strcat(buf, " ");
         strcat(buf, cmd[1]);
         strcat(buf, " ");
         strcat(buf, msg);
 	     send(clientsockfd, buf, sizeof(buf), 0);// sizeof buf  also works?

         char res[10];
         recv(clientsockfd, res, 10, 0);
		 //debug
		//  printf("--%s--\n", res);
         if (strcmp(res, "FAIL") == 0) {
             //fail
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);

         }else{
             //success
			 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
         }

     }else if(strcmp(cmd[0], "REFRESH") == 0){
         if(role != 0 || !logedin){

			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);

             return;
         }

         send(clientsockfd, "REFRESH", 7, 0);

         char update[BUFLEN];
         recv(clientsockfd, update, BUFLEN, 0);

         ListUnPack(update);

		 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
		 fflush(stdout);

		 cse4589_print_and_log("[%s:END]\n", cmd[0]);
		 fflush(stdout);

     }else if (strcmp(cmd[0], "BROADCAST") == 0){
         if (role != 0 || !logedin) {
             //fail
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);

             return;
         }

         char msg[MSGLEN] = "";
         for (int i=1;i<count;i++){
             strcat(msg, cmd[i]);
             if(i != count-1) strcat(msg, " ");
         }

         char buf[BUFLEN];
         strcpy(buf, cmd[0]);
         strcat(buf, " ");
         strcat(buf, msg);

         send(clientsockfd, buf, BUFLEN, 0);

	 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
	 fflush(stdout);

	 cse4589_print_and_log("[%s:END]\n", cmd[0]);
	 fflush(stdout);

     }else if(strcmp(cmd[0], "UNBLOCK") == 0){
         if(role != 0 || !logedin || !IsAddressAndPortValid(cmd[1], "8888") || count != 2){
             //fail
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }

         int flag = 0;
         for(int i=0;i<connIndex;i++){
             if(strcmp(connections[i].remote_addr, cmd[1]) == 0){
                 flag = 1;
                 break;
             }
         }
         if(flag == 0){
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }

         char buf[BUFLEN];
         strcpy(buf, cmd[0]);
         strcat(buf, " ");
         strcat(buf, cmd[1]);
         send(clientsockfd, buf, BUFLEN, 0);

         char res[10];
         recv(clientsockfd, res, 10, 0);
         if(strcmp(res, "FAIL") == 0){

			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
         }else{

			 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
         }

     }else if(strcmp(cmd[0], "BLOCK") == 0){

         if (role != 0 || !logedin || !IsAddressAndPortValid(cmd[1], "8888") || count != 2) {

			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);

			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);

             return;
         }


         int flag = 0;

         for(int i=0;i<connIndex;i++){

             if(strcmp(connections[i].remote_addr, cmd[1]) == 0){
                 flag = 1;
                 break;
             }

         }

         if(flag == 0){

			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
             return;
         }


         char buf[BUFLEN];
         strcpy(buf, cmd[0]);

         strcat(buf, " ");
         strcat(buf, cmd[1]);

         send(clientsockfd, buf, BUFLEN, 0);

         char res[10];
         recv(clientsockfd, res, 10, 0);
         if(strcmp(res, "FAIL") == 0){
 
			 cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
         }else{

			 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
			 fflush(stdout);
			 cse4589_print_and_log("[%s:END]\n", cmd[0]);
			 fflush(stdout);
         }

     }else if(strcmp(cmd[0], "EXIT") == 0){
         char buf[BUFLEN] = "EXIT";
         send(clientsockfd, buf, BUFLEN, 0);
         logedin = 0;
         close(clientsockfd);
         FD_CLR(clientsockfd, &master);
		 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
		 fflush(stdout);
		 cse4589_print_and_log("[%s:END]\n", cmd[0]);
		 fflush(stdout);
         exit(0);
     }else if(strcmp(cmd[0], "LOGOUT") == 0){
         if(!logedin){

			cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			fflush(stdout);
		 	cse4589_print_and_log("[%s:END]\n", cmd[0]);
			fflush(stdout);
            return;
         }
         char buf[BUFLEN] = "LOGOUT";
         send(clientsockfd, buf, BUFLEN, 0); 
         logedin = 0;
         close(clientsockfd);
         FD_CLR(clientsockfd, &master);
		 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
		 fflush(stdout);
		 cse4589_print_and_log("[%s:END]\n", cmd[0]);
		 fflush(stdout);

     }else if(strcmp(cmd[0], "STATISTICS") == 0){
         if(role != 1){
 
			cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			fflush(stdout);
		  	cse4589_print_and_log("[%s:END]\n", cmd[0]);
			fflush(stdout);
            return;
         }
		 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
		 fflush(stdout);
         for(int i=0;i<connIndex;i++){
             char tmp[20];
             if(connections[i].status == loged_in){
                 strcpy(tmp, "logged-in");
             }else{
                 strcpy(tmp, "logged-out");
             }

			 cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i+1, connections[i].hostname, connections[i].msg_sent, connections[i].msg_received, tmp);
             fflush(stdout);
 
         }
		 cse4589_print_and_log("[%s:END]\n", cmd[0]);
		 fflush(stdout);

     }else if(strcmp(cmd[0], "BLOCKED") == 0){
         if(role != 1 || count != 2 || !IsAddressAndPortValid(cmd[1], "8888")){

			cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			fflush(stdout);
		  	cse4589_print_and_log("[%s:END]\n", cmd[0]);
			fflush(stdout);
            return;
         }
         int flag = 0;
         for(int i=0;i<connIndex;i++){
             if(strcmp(connections[i].remote_addr, cmd[1]) == 0){
                 flag = 1;
				 cse4589_print_and_log("[%s:SUCCESS]\n", cmd[0]);
				 fflush(stdout);
                 for (int j=0; j<connections[i].blockindex; j++) {
  
                     cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", j+1, connections[i].blockedIPs[j]->hostname, connections[i].blockedIPs[j]->remote_addr, connections[i].blockedIPs[j]->portNum);
					 fflush(stdout);
                 }
                 break;
             }
         }

         if(flag == 0){

			cse4589_print_and_log("[%s:ERROR]\n", cmd[0]);
			fflush(stdout);
		  	cse4589_print_and_log("[%s:END]\n", cmd[0]);
			fflush(stdout);
            return;
         }else{
		  	cse4589_print_and_log("[%s:END]\n", cmd[0]);
			fflush(stdout);
		 }
     }

 }




/*
* This method gives response to incoming client messages
*/

void GiveResponseToClients(char **arguments, int count, int caller){
     if(strcmp(arguments[0], "SEND") == 0){

         char msg[BUFLEN] = "";
         char senderaddr[INET_ADDRSTRLEN];
         int sender;
         for(sender=0;sender<connIndex;sender++){
             if(connections[sender].connsockfd == caller){
                 strcat(msg, connections[sender].remote_addr);
                 strcat(msg, " ");
                 strcat(msg, arguments[2]);
                 strcat(msg, " ");

                 strcpy(senderaddr, connections[sender].remote_addr);
                 break;
             }
         }

         int flag = 0;

         for(int i=0;i<connIndex;i++){
             if(strcmp(arguments[1], connections[i].remote_addr) == 0){
                 flag = 1;
                 if(CheckSenderBlocked(caller, arguments[1])){
					 send(caller, "BLOCKED", 7, 0);
                     return;
                 }

                 if(connections[i].status == loged_in){
                     send(connections[i].connsockfd, msg, BUFLEN, 0);

                     connections[i].msg_received++;

		     cse4589_print_and_log("[%s:SUCCESS]\n" , "RELAYED");
		     fflush(stdout);

		     cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", connections[sender].remote_addr, connections[i].remote_addr, arguments[2]);
		     fflush(stdout);

		     cse4589_print_and_log("[%s:END]\n", "RELAYED");
		     fflush(stdout);

                 }else{
                     strcat(bufferedmsg, connections[i].remote_addr);
                     strcat(bufferedmsg, "---");

                     strcat(bufferedmsg, senderaddr);
                     strcat(bufferedmsg, "---");

                     strcat(bufferedmsg, arguments[2]);
                     strcat(bufferedmsg, "---");

                     connections[i].msg_received++;
                 }

                 break;
             }
         }
         if(flag==0){

             send(caller, "FAIL", 4, 0);
         }else{

             send(caller, "SUCCESS", 7, 0);

             connections[sender].msg_sent++;
         }

     }else if(strcmp(arguments[0], "REFRESH") == 0){
         char list[BUFLEN] = "";
         ListPack(list);
         send(caller, list, BUFLEN, 0);
     }else if(strcmp(arguments[0], "BROADCAST") == 0){

         char sender[INET_ADDRSTRLEN];
         char msg[BUFLEN];
         for(int i=0;i<connIndex;i++){
             if(connections[i].connsockfd == caller){
                 connections[i].msg_sent++;
                 strcpy(sender, connections[i].remote_addr);

                 strcpy(msg, connections[i].remote_addr);
                 strcat(msg, " ");

                 strcat(msg, arguments[1]);
                 strcat(msg, " ");
             }
         }

         for (int i=0; i<connIndex; i++) {
             if(!CheckSenderBlocked(caller, connections[i].remote_addr) && connections[i].connsockfd != caller){
                 if(connections[i].status == loged_in){
                     send(connections[i].connsockfd, msg, BUFLEN, 0);
                     connections[i].msg_received++;
                     //triger event
					 cse4589_print_and_log("[%s:SUCCESS]\n" , "RELAYED");
					 fflush(stdout);

					 cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sender, "255.255.255.255", msg);
					 fflush(stdout);

					 cse4589_print_and_log("[%s:END]\n", "RELAYED");
					 fflush(stdout);
                 }else{
 //                    strcat(bufferedmsg, "255.255.255.255"); mark
                     strcat(bufferedmsg, connections[i].remote_addr);
                     strcat(bufferedmsg, "---");
                     strcat(bufferedmsg, sender);
                     strcat(bufferedmsg, "---");
                     strcat(bufferedmsg, arguments[1]);
                     strcat(bufferedmsg, "---");
                     connections[i].msg_received++;
                 }
             }
         }
     }else if(strcmp(arguments[0], "BLOCK") == 0){

         int target;
         for(target=0;target<connIndex;target++){
             if(connections[target].connsockfd == caller) break;
         }

         int found = 0;
         int blocking;
         for (blocking = 0; blocking< connIndex; blocking++) {
             if(strcmp(connections[blocking].remote_addr, arguments[1])== 0) {
                 found = 1;
                 break;
             }
         }

         if(found == 0){
             send(caller, "FAIL", 4, 0);
             return;
         }

         if (connections[target].blockindex == 0) {

             connections[target].blockedIPs[connections[target].blockindex++] = &connections[blocking];
         }else{
             for(int i=0;i<connections[target].blockindex; i++){
                 if(strcmp(connections[target].blockedIPs[i]->remote_addr, arguments[1]) == 0){
                     send(caller, "FAIL", 4, 0);
                     return;

                 }
             }
             connections[target].blockedIPs[connections[target].blockindex++] = &connections[blocking];


             if(connections[target].blockindex > 1){
                 for(int i=0;i<connections[target].blockindex-1;i++){
                     for (int j=i; j<connections[target].blockindex; j++) {
                         if(connections[target].blockedIPs[i]->portNum > connections[target].blockedIPs[j]->portNum){
                             struct connection *tmp = connections[target].blockedIPs[i];
                             connections[target].blockedIPs[i] = connections[target].blockedIPs[j];
                             connections[target].blockedIPs[j] = tmp;
                         }
                     }
                 }
             }
         }
         send(caller, "SUCCESS", 7, 0);

     }else if(strcmp(arguments[0], "UNBLOCK") == 0){
         int target = 0;

         for(target = 0; target<connIndex; target++){
             if(connections[target].connsockfd == caller) 
		break;
         }

         if(connections[target].blockindex == 0){

             send(caller, "FAIL", 4, 0);
             return;
         }

         int flag = 0;

         for(int i=0;i<connections[target].blockindex;i++){

             if(strcmp(connections[target].blockedIPs[i]->remote_addr, arguments[1]) ==  0){

                 if(i == 2){
                     connections[target].blockindex--;
                     flag = 1;
                     break;
                 }

                 for(int j=i+1;j<connections[target].blockindex;j++){

                     connections[target].blockedIPs[j-1] =  connections[target].blockedIPs[j];
                 }

                 flag = 1;

                 connections[target].blockindex--;

                 break;
             }
         }
         if(flag == 0){
             send(caller, "FAIL", 4, 0);
             return;
         }
         send(caller, "SUCCESS", 7, 0);

     }else if(strcmp(arguments[0], "EXIT") == 0){

         int target;
         for(target=0;target<connIndex;target++){

             if(connections[target].connsockfd == caller){
                 close(connections[target].connsockfd);

                 FD_CLR(connections[target].connsockfd, &master);

                 if(target == 3){
                     connIndex--;
                     break;
                 }

                 for(int j=target+1;j<connIndex;j++){
                     connections[j-1] = connections[j];
                 }
                 connIndex--;

                 break;
             }
         }
     }else if(strcmp(arguments[0], "LOGOUT") == 0){

         for(int i=0;i<connIndex;i++){

             if(connections[i].connsockfd == caller){

                 close(connections[i].connsockfd);

                 FD_CLR(connections[i].connsockfd, &master);

                 connections[i].status = loged_out;

                 break;
             }
         }
     }


 }


