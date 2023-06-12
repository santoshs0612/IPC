/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description: Writing the server side code  
 *
 *        Version:  1.0
 *        Created:  10/06/23 08:37:22 PM IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME Santosh Kumar, 
 *   Organization:  Open
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
    struct sockaddr_un name;
#if 0
    struct sockaddr_un {
        sa_family_t sun family; /* AF_UNIX */
        char sun_path[108]; /*  pathname */
    };

#endif
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];
/*  if main socket running remove it first  */

    unlink(SOCKET_NAME);
/*  Creaing the masteer socket  */

   connection_socket= socket(AF_UNIX,SOCK_STREAM,0);

   if(connection_socket==-1)
   {
       perror("socket");
       exit(EXIT_FAILURE);
   }
   printf("Master socket created\n");

/*  Initialization */
    memset(&name,0,sizeof(struct sockaddr_un));
/*  socket details */
    name.sun_family= AF_UNIX;
    strncpy(name.sun_path,SOCKET_NAME,sizeof(name.sun_path)-1);

/*  BINDing the socket to system */
    ret = bind(connection_socket,(const struct sockaddr *) &name,sizeof(struct sockaddr_un));
    if(ret==-1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("bind() call succeed\n");
/*  Preparing for connection accepting */
    ret= listen(connection_socket,20);
    if(ret ==-1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
/*  main infi loop for connection handeling */

for(; ;){
/*  Wait for incomming connection */
    printf("Waiting on accept() sys call\n");
    data_socket=accept(connection_socket,NULL,NULL);
    if(data_socket==-1){
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("connection accepted from client\n");
    result=0;
    for(;;)
    {
       /*  prepare buffer for recv */
       memset(buffer,0,BUFFER_SIZE);
       /*  waitng for data  */
       /*  server blockched here if only takae command
        *   from connected client */
       printf("Waiting for data from the client\n");
       ret = read(data_socket,buffer,BUFFER_SIZE);

       if(ret==-1){
           perror("read");
           exit(EXIT_FAILURE);
       }

       memcpy(&data,buffer,sizeof(int));
       if(data==0)
       {
           break;
       }
       result+=data;
    }
/*  Send result */
    memset(buffer,0,BUFFER_SIZE);
    sprintf(buffer,"Result=%d",result);
    printf("Sending final result to client\n");
    ret = write(data_socket,buffer,BUFFER_SIZE);
    if(ret==-1)
    {
        perror("Write");
        exit(EXIT_FAILURE);
    }
/*  Close the data socket  */
    close(data_socket);

}

/*  Close the Master Socket  */
close(connection_socket);
printf("connection closed\n");
/*  Release the resource and unlink */
unlink(SOCKET_NAME);
exit(EXIT_SUCCESS);
}

