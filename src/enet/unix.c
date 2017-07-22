/** 
 @file  unix.c
 @brief ENet Unix system specific functions
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define ENET_BUILDING_LIB 1
#include "enet/enet.h"

#ifdef HAS_FCNTL
#include <fcntl.h>
#endif

#ifdef HAS_POLL
#include <sys/poll.h>
#endif

#ifndef HAS_SOCKLEN_T
//typedef int socklen_t;
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

static enet_uint32 timeBase = 0;

int
enet_initialize (void)
{
    return 0;
}

void
enet_deinitialize (void)
{
}

enet_uint32
enet_time_get (void)
{
}

void
enet_time_set (enet_uint32 newTimeBase)
{
}

int
enet_address_set_host (ENetAddress * address, const char * name)
{


}

int
enet_address_get_host (const ENetAddress * address, char * name, size_t nameLength)
{
    
}

ENetSocket
enet_socket_create (ENetSocketType type, const ENetAddress * address)
{
}

int
enet_socket_connect (ENetSocket socket, const ENetAddress * address)
{
    
}

ENetSocket
enet_socket_accept (ENetSocket socket, ENetAddress * address)
{
} 
    
void
enet_socket_destroy (ENetSocket socket)
{

}

int
enet_socket_send (ENetSocket socket,
                  const ENetAddress * address,
                  const ENetBuffer * buffers,
                  size_t bufferCount)
{
}

int
enet_socket_receive (ENetSocket socket,
                     ENetAddress * address,
                     ENetBuffer * buffers,
                     size_t bufferCount)
{
}

int
enet_socket_wait (ENetSocket socket, enet_uint32 * condition, enet_uint32 timeout)
{

}



