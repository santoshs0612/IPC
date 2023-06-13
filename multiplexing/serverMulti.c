/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description: server multiplexing  
 *
 *        Version:  1.0
 *        Created:  12/06/23 08:44:43 PM IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Santosh Kumar, 
 *   Organization:  
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

#define MAX_CLIENT_SUPPORTED 32

/*  An array of file descripter which the server
 *   process is maintaing in order to talk with 
 *   the connected client, Master socket fd also a member */

int monitored_fd_set[MAX_CLIENT_SUPPORTED];

/*  Client intermediate result array */

int client_result[MAX_CLENT_SUPPORTED];

/*  Remove or iniatialize the fd set */
static void 
initiaze_monitor_fd_set(){
    int i=0;
    for(;i<MAX_CLIENT_SUPPORTED;i++)
    {
        monitored_fd_set[i]=-1;
    }
}

/*  Add new client ot fd set to array */

static void
add_to_monitored_fd_set(int skt_fd)
{
    int i=0;
    for(;i<MAX_CLIENT_SUPPORTED;i++){
        if(monitored_fd_set[i]!=-1){
            continue;
        }
        monitored_fd_set[i]= skt_fd;
        break;
    }
}

/*  Clone all fds in monitored_fd_set array into
 *  fd_set Data structure */
static void 
refresh_fd_set(fd_set *fd_set_ptr){
    FD_ZERO(fd_set_ptr);
    int i=0;
    for(;i<MAX_CLIENT_SUPPORTED;i++){
        if(monitored_fd_set!=-1){
            FD_SET(monitored_fd_set[i],fd_set_ptr);
        }
    }
}

/*  Get Max value among all fds which server is montoring */

static int
get_max_fd(){
    int i=0;
    int max= -1;
    for(;i<MAX_CLIENT_SUPPORTED;i++){
        if(monitored_fd_set[i]>max){
            max=monitored_fd_set[i];
        }
    }
    return max;
}

int main(int argc, char *argv[]){
    struct sockaddr_un name;

#if 0
    struct sockaddr_un{
        sa_family_t sun_family; /*  F_UNIX */
        char sun_path[108]; /*  path name  */
    };
#endif
    int ret ;
    int connection_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int comm_socket_fd,i;
    initiaze_monitor_fd_set();
    add_to_monitored_fd_set(0);

    /*  If Previous socket exits clear it it happens when 
     *  program exited inadvertently on last run */

    unlink(SOCKET_NAME);

    /* Create the Master Socket */

    connection_socket = socket(AF_UNIX,SOCK_STREAM,0);

    if(connection_socket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Master socket Created");

    /*  initialize */

    memset(&name,0, sizeof( struct sockaddr_un));

    /*  Socket credentialas */
    name.sun_family= AF_UNIX;
    strncpy(name.sun_path,SOCKET_NAME,sizeof(name.sun_pat)-1);

    /* BIND socket to socket name */

    ret= bind(connection_socket,(const struct sockaddr *) &name,
            sizeof(struct sockaddr_un));
    
    if(ret == -1){
        perror("bind");
        exit(EXIT_FAILURE);
        }
    printf("bind() call succeed\n");

    /*  Prepare for accepting the connection */
    /*  the backlog size is set to 20 so while one request being 
     *  processed other requst can be wailting */

    ret= listen(connection_socket,20);
    if(ret ==-1)
    {
        perror("listen");
        exit(EXIT_FAILURE):
    }

    /*  Add master socket to monitored set of fds */

    add_to_monitored_fd_set(connection_socket);

    /* Main loop for handeling the connection */

    for(;;){
        
        referesh_fd_set(&readfds);/* Copy the entire monitored fds to readfds */
        /* Wait for incomming connection */
        printf("Waiting on select() sys call\n");

        /* Call the selest system call, server process blocks here
         * Linux OS keep this process blocked untill the connection in
         * itiation request or data request arrives on any of the fds in readfds */
        select(get_max_fd()+1, &readfds,NULL,NULL);
        
        if(FD_ISSET(connection_socket,&readfds)){
            /* Data arrives on master socket only when new client connection
             * with the server(that is 'connect' call is invloked on client
             * side */
            printf("New connection recieved recvd,accept the connection\n");
            data_socket = accept(connection_socket,NULL,NULL);
            if(data_socket=-1){
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("COnnection accepted form Client\n");
            add_to_monitored_fd_set(data_socket);
        }
        else if(FD_ISSET(0,&readfds)){
            memset(buffer,0,BUFFER_SIZE);
            ret = read(0,buffer,BUFFER_SIZE);
            printf("INput read from console : %s\n", buffer);
        }
        else {/* Data arrived on some other client */
            /*  Find the client which send data */

            i=0,comm_socket_fd=-1;
            for(;i<MAX_CLIENT_SUPPORTED;i++){

                if(FD_ISSET(monitored_fd_set[i],&readfds)){
                    comm_socket_fd= monitored_f_set[i];
                    /* Prepare buffer for data */
                    memset(buffer,0,BUFFER_SIZE);
                    /* Wait for next data packet */
                    /*  Server is blocked here Waiting Waiting ffor the 
                     *  data to arrive from client 'read' is a blocking
                     *  system call */
                    printf("Waiting for data from the client\n");
                    ret =read(comm_socket_fd, buffer, BUUFER_SIZE);
                    if(ret=-1){
                        perror("read");
                        exit(EXIT_FAILURE);
                    }

                   /* Add receive sum and going out logic */
                   memcpy(&data,buffer,sizeof(int));
                   if(data==0){
                       /* Send the result */
                       memset(buffer,0,BUFFER_SIZE);
                       sprintf(buffer,"Result=%d",client_result[i]);
                       printf("Sending final result to client");

                       ret =write(comm_socket_fd, buffer,BUFFER_size);
                       if(ret=-1){
                           perror("Write");
                           exit(EXIT_FAILURE);
                       }
                       /* Close the socket  */
                       close(comm_socket_fd);
                       client_result[i]=0;
                       remove_from_monitored_fd_set(comm_socket_fd);
                       continue;/*  Go to select and block */
                   }
                   client_result[i]+=data;
                }
            }

        }
}/*  Go to select and block */

/*  Close the mster socket  */
close(connection_socket);
remove_from_monitored_fd_set(connection_socket);
printf("Connection Closed");

/*  Server should release the resource before getting terminated
 *  Unlink the socket */
unlink(SOCKET_NAME);
exit(EXIT_SUCCESS);
}
