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


}
