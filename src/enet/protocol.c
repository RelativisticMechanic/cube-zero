/** 
 @file  protocol.c
 @brief ENet protocol functions
*/
#include <stdio.h>
#include <string.h>
#define ENET_BUILDING_LIB 1
#include "enet/utility.h"
#include "enet/time.h"
#include "enet/enet.h"

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

