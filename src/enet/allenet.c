/*
	This is a stub of enet. it does nothing but allocate an
	enet data structure and return it, if that. All of this
	code should be removed when possible
*/

#define ENET_BUILDING_LIB 1
#include "enet/list.h" 
#define ENET_BUILDING_LIB 1
#include "enet/enet.h" 
#include <string.h> 
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
#include <stdio.h>
#include <string.h>
#define ENET_BUILDING_LIB 1
#include "enet/utility.h"
#include "enet/time.h"
#include "enet/enet.h" 

#include <string.h>
#define ENET_BUILDING_LIB 1
#include "enet/enet.h" 
static ENetCallbacks callbacks = { malloc, free, rand };

int
enet_initialize_with_callbacks (ENetVersion version, const ENetCallbacks * inits)
{
   if (version != ENET_VERSION)
     return -1;

   if (inits -> malloc != NULL || inits -> free != NULL)
   {
      if (inits -> malloc == NULL || inits -> free == NULL)
        return -1;

      callbacks.malloc = inits -> malloc;
      callbacks.free = inits -> free;
   }
      
   if (inits -> rand != NULL)
     callbacks.rand = inits -> rand;

   return enet_initialize ();
}
           
void *
enet_malloc (size_t size)
{
   void * memory = callbacks.malloc (size);

   if (memory == NULL)
     abort ();

   return memory;
}

void
enet_free (void * memory)
{
   callbacks.free (memory);
}

int
enet_rand (void)
{
   return callbacks.rand ();
} 
ENetHost *
enet_host_create (const ENetAddress * address, size_t peerCount, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
{
} 
void
enet_host_destroy (ENetHost * host)
{
} 
ENetPeer *
enet_host_connect (ENetHost * host, const ENetAddress * address, size_t channelCount)
{
} 
void
enet_host_broadcast (ENetHost * host, enet_uint8 channelID, ENetPacket * packet)
{
} 
void
enet_host_bandwidth_limit (ENetHost * host, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
{
}

void
enet_host_bandwidth_throttle (ENetHost * host)
{ 
} 
void
enet_list_clear (ENetList * list)
{
   list -> sentinel.next = & list -> sentinel;
   list -> sentinel.previous = & list -> sentinel;
}

ENetListIterator
enet_list_insert (ENetListIterator position, void * data)
{
   ENetListIterator result = (ENetListIterator) data;

   result -> previous = position -> previous;
   result -> next = position;

   result -> previous -> next = result;
   position -> previous = result;

   return result;
}

void *
enet_list_remove (ENetListIterator position)
{
   position -> previous -> next = position -> next;
   position -> next -> previous = position -> previous;

   return position;
}

size_t
enet_list_size (ENetList * list)
{
   size_t size = 0;
   ENetListIterator position;

   for (position = enet_list_begin (list);
        position != enet_list_end (list);
        position = enet_list_next (position))
     ++ size;
   
   return size;
} 
ENetPacket *
enet_packet_create (const void * data, size_t dataLength, enet_uint32 flags)
{
    ENetPacket * packet = (ENetPacket *) enet_malloc (sizeof (ENetPacket));

    packet -> data = (enet_uint8 *) enet_malloc (dataLength);

    if (data != NULL)
      memcpy (packet -> data, data, dataLength);

    packet -> referenceCount = 0;
    packet -> flags = flags;
    packet -> dataLength = dataLength;

    return packet;
} 
void
enet_packet_destroy (ENetPacket * packet)
{
    enet_free (packet -> data);
    enet_free (packet);
} 
int
enet_packet_resize (ENetPacket * packet, size_t dataLength)
{
    enet_uint8 * newData;
   
    if (dataLength <= packet -> dataLength)
    {
       packet -> dataLength = dataLength; 
       return 0;
    } 
    newData = (enet_uint8 *) enet_malloc (dataLength);
    memcpy (newData, packet -> data, packet -> dataLength);
    enet_free (packet -> data); 
    packet -> data = newData;
    packet -> dataLength = dataLength; 
    return 0;
} 
void
enet_peer_throttle_configure (ENetPeer * peer, enet_uint32 interval, enet_uint32 acceleration, enet_uint32 deceleration)
{ 
} 
int
enet_peer_throttle (ENetPeer * peer, enet_uint32 rtt)
{ 
    return 0;
} 
int
enet_peer_send (ENetPeer * peer, enet_uint8 channelID, ENetPacket * packet)
{
   return 0;
} 
ENetPacket *
enet_peer_receive (ENetPeer * peer, enet_uint8 channelID)
{ 
} 
static void
enet_peer_reset_outgoing_commands (ENetList * queue)
{
} 
void
enet_peer_reset_queues (ENetPeer * peer)
{ 
}
void
enet_peer_reset (ENetPeer * peer)
{ 
} 
void
enet_peer_ping (ENetPeer * peer)
{ 
} 
void
enet_peer_disconnect_now (ENetPeer * peer)
{
} 
void
enet_peer_disconnect (ENetPeer * peer)
{
} 
ENetOutgoingCommand *
enet_peer_queue_outgoing_command (ENetPeer * peer, const ENetProtocol * command, ENetPacket * packet, enet_uint32 offset, enet_uint16 length)
{ 
}
static enet_uint32 timeCurrent;

static int
enet_protocol_dispatch_incoming_commands (ENetHost * host, ENetEvent * event)
{
    return 0;
} 
static void
enet_protocol_remove_sent_unreliable_commands (ENetPeer * peer)
{
} 
static ENetProtocolCommand
enet_protocol_remove_sent_reliable_command (ENetPeer * peer, enet_uint32 reliableSequenceNumber, enet_uint8 channelID)
{
} 
static ENetPeer *
enet_protocol_handle_connect (ENetHost * host, const ENetProtocolHeader * header, const ENetProtocol * command)
{
} 
static void
enet_protocol_handle_send_reliable (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused variable */
} 
static void
enet_protocol_handle_send_unsequenced (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused var */
} 
static void
enet_protocol_handle_send_unreliable (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	
	host = host; /* unused var */
} 
static void
enet_protocol_handle_send_fragment (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused var */
}
static void
enet_protocol_handle_ping (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused var */
	peer = peer; /* unused var */
    if (command -> header.commandLength < sizeof (ENetProtocolPing))
      return;
} 
static void
enet_protocol_handle_bandwidth_limit (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
} 
static void
enet_protocol_handle_throttle_configure (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
} 
static void
enet_protocol_handle_disconnect (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
} 
static int
enet_protocol_handle_acknowledge (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{ 
} 
static void
enet_protocol_handle_verify_connect (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{
} 
static int
enet_protocol_handle_incoming_commands (ENetHost * host, ENetEvent * event)
{
} 
static int
enet_protocol_receive_incoming_commands (ENetHost * host, ENetEvent * event)
{
} 
static void
enet_protocol_send_acknowledgements (ENetHost * host, ENetPeer * peer)
{
} 
static void
enet_protocol_send_unreliable_outgoing_commands (ENetHost * host, ENetPeer * peer)
{
} 
static int
enet_protocol_check_timeouts (ENetHost * host, ENetPeer * peer, ENetEvent * event)
{
} 
static void
enet_protocol_send_reliable_outgoing_commands (ENetHost * host, ENetPeer * peer)
{
	return;
} 
static int
enet_protocol_send_outgoing_commands (ENetHost * host, ENetEvent * event, int checkForTimeouts)
{ 
    return 0;
} 
void
enet_host_flush (ENetHost * host)
{ 
} 
int
enet_host_service (ENetHost * host, ENetEvent * event, enet_uint32 timeout)
{ 
    return 0; 
} 
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
int enet_socket_send (ENetSocket socket, const ENetAddress * address, const ENetBuffer * buffers, size_t bufferCount)
{
} 
int
enet_socket_receive (ENetSocket socket, ENetAddress * address, ENetBuffer * buffers, size_t bufferCount)
{
} 
int
enet_socket_wait (ENetSocket socket, enet_uint32 * condition, enet_uint32 timeout)
{

} 

