/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description: Client File  
 *
 *        Version:  1.0
 *        Created:  11/06/23 11:24:09 AM IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME Santosh Kumar, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int 
main(int argc,char *argv[])
{
    struct sockaddr_un addr;
    int i;
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE];
    /*  Creating the data socket */
    data_socket= socket(AF_UNIX,SOCK_STREAM,0);
    if(data_socket==-1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    /*  Clear the stuct addr because the some
     *  implimentation have some exta fields */
    memset(&addr,0,sizeof(struct sockaddr_un));

    /*  connect the socket to the address */
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,SOCKET_NAME,sizeof(addr.sun_path)-1);
    ret = connect(data_socket,(const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
   if(ret==-1){
       fprintf(stderr,"The server is down.\n");
       exit(EXIT_FAILURE);
   }
   /*   send data */
   do{
       printf("Enter number to send to server :\n");
       scanf("%d",&i);
       ret = write(data_socket,&i, sizeof(int));
       if(ret==-1){
           perror("write");
           break;
       }
       printf("No of bytes sent = %d, data sent = %d\n", ret,i);
   }
   while(i);

   /*   Request Result */

   memset(buffer,0,BUFFER_SIZE);
   strncpy(buffer,"RES",strlen("RES"));
   buffer[strlen(buffer)]= '\0';
   printf("buffer = %s\n", buffer);

   ret = write(data_socket, buffer,strlen(buffer));
   if(ret==-1){
       perror("write");
       exit(EXIT_FAILURE);
   }
   /*   Receive the result */

   memset(buffer,0,BUFFER_SIZE);
   ret= read(data_socket,buffer,BUFFER_SIZE);
   if(ret==-1){
       perror("read");
       exit(EXIT_FAILURE);
   }
   /*  Ensure buffer is zero terminated  */

   buffer[BUFFER_SIZE-1]=0;
   printf("Result = %s\n", buffer);

   /*   Close The socket  */
   close(data_socket);
   exit(EXIT_SUCCESS);

}

