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
    for (;;)
    {
       int receivedLength;
       ENetBuffer buffer;

       buffer.data = host -> receivedData;
       buffer.dataLength = sizeof (host -> receivedData);

       receivedLength = enet_socket_receive (host -> socket,
                                             & host -> receivedAddress,
                                             & buffer,
                                             1);

       if (receivedLength < 0)
         return -1;

       if (receivedLength == 0)
         return 0;

       host -> receivedDataLength = receivedLength;
       
       switch (enet_protocol_handle_incoming_commands (host, event))
       {
       case 1:
          return 1;
       
       case -1:
          return -1;

       default:
          break;
       }
    }

    return -1;
}

static void
enet_protocol_send_acknowledgements (ENetHost * host, ENetPeer * peer)
{
    ENetProtocol * command = & host -> commands [host -> commandCount];
    ENetBuffer * buffer = & host -> buffers [host -> bufferCount];
    ENetAcknowledgement * acknowledgement;
    ENetListIterator currentAcknowledgement;
  
    currentAcknowledgement = enet_list_begin (& peer -> acknowledgements);
         
    while (currentAcknowledgement != enet_list_end (& peer -> acknowledgements))
    {
       if (command >= & host -> commands [sizeof (host -> commands) / sizeof (ENetProtocol)] ||
           buffer >= & host -> buffers [sizeof (host -> buffers) / sizeof (ENetBuffer)] ||
           peer -> mtu - host -> packetSize < sizeof (ENetProtocolAcknowledge))
         break;

       acknowledgement = (ENetAcknowledgement *) currentAcknowledgement;
 
       currentAcknowledgement = enet_list_next (currentAcknowledgement);

       buffer -> data = command;
       buffer -> dataLength = sizeof (ENetProtocolAcknowledge);

       host -> packetSize += buffer -> dataLength;
 
       command -> header.command = ENET_PROTOCOL_COMMAND_ACKNOWLEDGE;
       command -> header.channelID = acknowledgement -> command.header.channelID;
       command -> header.flags = 0;
       command -> header.commandLength = ENET_HOST_TO_NET_32 (sizeof (ENetProtocolAcknowledge));
       command -> acknowledge.receivedReliableSequenceNumber = ENET_HOST_TO_NET_32 (acknowledgement -> command.header.reliableSequenceNumber);
       command -> acknowledge.receivedSentTime = ENET_HOST_TO_NET_32 (acknowledgement -> sentTime);
  
       if (acknowledgement -> command.header.command == ENET_PROTOCOL_COMMAND_DISCONNECT)
         peer -> state = ENET_PEER_STATE_ZOMBIE;

       enet_list_remove (& acknowledgement -> acknowledgementList);
       enet_free (acknowledgement);

       ++ command;
       ++ buffer;
    }

    host -> commandCount = command - host -> commands;
    host -> bufferCount = buffer - host -> buffers;
}

static void
enet_protocol_send_unreliable_outgoing_commands (ENetHost * host, ENetPeer * peer)
{
    ENetProtocol * command = & host -> commands [host -> commandCount];
    ENetBuffer * buffer = & host -> buffers [host -> bufferCount];
    ENetOutgoingCommand * outgoingCommand;
    ENetListIterator currentCommand;

    currentCommand = enet_list_begin (& peer -> outgoingUnreliableCommands);
    
    while (currentCommand != enet_list_end (& peer -> outgoingUnreliableCommands))
    {
       outgoingCommand = (ENetOutgoingCommand *) currentCommand;

       if (command >= & host -> commands [sizeof (host -> commands) / sizeof (ENetProtocol)] ||
           buffer + 1 >= & host -> buffers [sizeof (host -> buffers) / sizeof (ENetBuffer)] ||
           peer -> mtu - host -> packetSize < outgoingCommand -> command.header.commandLength ||
           (outgoingCommand -> packet != NULL &&
             peer -> mtu - host -> packetSize < outgoingCommand -> command.header.commandLength + 
                                                         outgoingCommand -> packet -> dataLength))
         break;

       currentCommand = enet_list_next (currentCommand);

       if (outgoingCommand -> packet != NULL)
       {
          peer -> packetThrottleCounter += ENET_PEER_PACKET_THROTTLE_COUNTER;
          peer -> packetThrottleCounter %= ENET_PEER_PACKET_THROTTLE_SCALE;
          
          if (peer -> packetThrottleCounter > peer -> packetThrottle)
          {
             -- outgoingCommand -> packet -> referenceCount;

             if (outgoingCommand -> packet -> referenceCount == 0)
               enet_packet_destroy (outgoingCommand -> packet);
         
             enet_list_remove (& outgoingCommand -> outgoingCommandList);
             enet_free (outgoingCommand);
           
             continue;
          }
       }

       buffer -> data = command;
       buffer -> dataLength = outgoingCommand -> command.header.commandLength;
      
       host -> packetSize += buffer -> dataLength;

       * command = outgoingCommand -> command;
       
       enet_list_remove (& outgoingCommand -> outgoingCommandList);

       if (outgoingCommand -> packet != NULL)
       {
          ++ buffer;
          
          buffer -> data = outgoingCommand -> packet -> data;
          buffer -> dataLength = outgoingCommand -> packet -> dataLength;

          command -> header.commandLength += buffer -> dataLength;

          host -> packetSize += buffer -> dataLength;

          enet_list_insert (enet_list_end (& peer -> sentUnreliableCommands), outgoingCommand);
       }
       else
         enet_free (outgoingCommand);

       command -> header.commandLength = ENET_HOST_TO_NET_32 (command -> header.commandLength);

       ++ command;
       ++ buffer;
    } 

    host -> commandCount = command - host -> commands;
    host -> bufferCount = buffer - host -> buffers;
}

static int
enet_protocol_check_timeouts (ENetHost * host, ENetPeer * peer, ENetEvent * event)
{
	host = host; /* unused var */
    ENetOutgoingCommand * outgoingCommand;
    ENetListIterator currentCommand;

    currentCommand = enet_list_begin (& peer -> sentReliableCommands);

    while (currentCommand != enet_list_end (& peer -> sentReliableCommands))
    {
       outgoingCommand = (ENetOutgoingCommand *) currentCommand;

       currentCommand = enet_list_next (currentCommand);

       if (ENET_TIME_DIFFERENCE (timeCurrent, outgoingCommand -> sentTime) < outgoingCommand -> roundTripTimeout)
         continue;

       if(peer -> earliestTimeout == 0 ||
          ENET_TIME_LESS(outgoingCommand -> sentTime, peer -> earliestTimeout))
           peer -> earliestTimeout = outgoingCommand -> sentTime;

       if (peer -> earliestTimeout != 0 &&
             (ENET_TIME_DIFFERENCE(timeCurrent, peer -> earliestTimeout) >= ENET_PEER_TIMEOUT_MAXIMUM ||
               (outgoingCommand -> roundTripTimeout >= outgoingCommand -> roundTripTimeoutLimit &&
                 ENET_TIME_DIFFERENCE(timeCurrent, peer -> earliestTimeout) >= ENET_PEER_TIMEOUT_MINIMUM)))
       {
          event -> type = ENET_EVENT_TYPE_DISCONNECT;
          event -> peer = peer;

          enet_peer_reset (peer);

          return 1;
       }

       if (outgoingCommand -> packet != NULL)
         peer -> reliableDataInTransit -= outgoingCommand -> fragmentLength;
          
       ++ peer -> packetsLost;

       outgoingCommand -> roundTripTimeout *= 2;

       enet_list_insert (enet_list_begin (& peer -> outgoingReliableCommands),
                         enet_list_remove (& outgoingCommand -> outgoingCommandList));

       if (currentCommand == enet_list_begin (& peer -> sentReliableCommands) &&
           enet_list_empty (& peer -> sentReliableCommands) == 0)
       {
          outgoingCommand = (ENetOutgoingCommand *) currentCommand;

          peer -> nextTimeout = outgoingCommand -> sentTime + outgoingCommand -> roundTripTimeout;
       }
    }
    
    return 0;
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
    timeCurrent = enet_time_get ();

    enet_protocol_send_outgoing_commands (host, NULL, 0);
} 
int
enet_host_service (ENetHost * host, ENetEvent * event, enet_uint32 timeout)
{ 
    return 0; 
}

