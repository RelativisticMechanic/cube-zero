#include <string.h> /* CM Graff -- clean up a compiler wanring */ 
#define ENET_BUILDING_LIB 1
#include "enet/enet.h" 
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

/** @} */
