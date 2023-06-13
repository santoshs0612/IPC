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


