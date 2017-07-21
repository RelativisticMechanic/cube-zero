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
    if (command -> header.commandLength < sizeof (ENetProtocolBandwidthLimit))
      return;

    peer -> incomingBandwidth = ENET_NET_TO_HOST_32 (command -> bandwidthLimit.incomingBandwidth);
    peer -> outgoingBandwidth = ENET_NET_TO_HOST_32 (command -> bandwidthLimit.outgoingBandwidth);

    if (peer -> incomingBandwidth == 0 &&
        host -> outgoingBandwidth == 0)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
    else
      peer -> windowSize = (ENET_MIN (peer -> incomingBandwidth, host -> outgoingBandwidth) /
                             ENET_PEER_WINDOW_SIZE_SCALE) * ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (peer -> windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;
    else
    if (peer -> windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      peer -> windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;
}

static void
enet_protocol_handle_throttle_configure (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused var */
    if (command -> header.commandLength < sizeof (ENetProtocolThrottleConfigure))
      return;

    peer -> packetThrottleInterval = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleInterval);
    peer -> packetThrottleAcceleration = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleAcceleration);
    peer -> packetThrottleDeceleration = ENET_NET_TO_HOST_32 (command -> throttleConfigure.packetThrottleDeceleration);
}

static void
enet_protocol_handle_disconnect (ENetHost * host, ENetPeer * peer, const ENetProtocol * command)
{
	host = host; /* unused var */
    if (command -> header.commandLength < sizeof (ENetProtocolDisconnect))
      return;

    enet_peer_reset_queues (peer);

    if (peer -> state != ENET_PEER_STATE_CONNECTED)
      enet_peer_reset (peer);
    else
    if (command -> header.flags & ENET_PROTOCOL_FLAG_ACKNOWLEDGE)
      peer -> state = ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT;
    else
      peer -> state = ENET_PEER_STATE_ZOMBIE;
}

static int
enet_protocol_handle_acknowledge (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{
    enet_uint32 roundTripTime,
           receivedSentTime,
           receivedReliableSequenceNumber;
    ENetProtocolCommand commandNumber;

    if (command -> header.commandLength < sizeof (ENetProtocolAcknowledge))
      return 0;

    receivedSentTime = ENET_NET_TO_HOST_32 (command -> acknowledge.receivedSentTime);

    if (ENET_TIME_LESS (timeCurrent, receivedSentTime))
      return 0;

    peer -> lastReceiveTime = timeCurrent;
    peer -> earliestTimeout = 0;

    roundTripTime = ENET_TIME_DIFFERENCE (timeCurrent, receivedSentTime);

    enet_peer_throttle (peer, roundTripTime);

    peer -> roundTripTimeVariance -= peer -> roundTripTimeVariance / 4;

    if (roundTripTime >= peer -> roundTripTime)
    {
       peer -> roundTripTime += (roundTripTime - peer -> roundTripTime) / 8;
       peer -> roundTripTimeVariance += (roundTripTime - peer -> roundTripTime) / 4;
    }
    else
    {
       peer -> roundTripTime -= (peer -> roundTripTime - roundTripTime) / 8;
       peer -> roundTripTimeVariance += (peer -> roundTripTime - roundTripTime) / 4;
    }

    if (peer -> roundTripTime < peer -> lowestRoundTripTime)
      peer -> lowestRoundTripTime = peer -> roundTripTime;

    if (peer -> roundTripTimeVariance > peer -> highestRoundTripTimeVariance) 
      peer -> highestRoundTripTimeVariance = peer -> roundTripTimeVariance;

    if (peer -> packetThrottleEpoch == 0 ||
        ENET_TIME_DIFFERENCE(timeCurrent, peer -> packetThrottleEpoch) >= peer -> packetThrottleInterval)
    {
        peer -> lastRoundTripTime = peer -> lowestRoundTripTime;
        peer -> lastRoundTripTimeVariance = peer -> highestRoundTripTimeVariance;
        peer -> lowestRoundTripTime = peer -> roundTripTime;
        peer -> highestRoundTripTimeVariance = peer -> roundTripTimeVariance;
        peer -> packetThrottleEpoch = timeCurrent;
    }

    receivedReliableSequenceNumber = ENET_NET_TO_HOST_32 (command -> acknowledge.receivedReliableSequenceNumber);

    commandNumber = enet_protocol_remove_sent_reliable_command (peer, receivedReliableSequenceNumber, command -> header.channelID);

    switch (peer -> state)
    {
    case ENET_PEER_STATE_ACKNOWLEDGING_CONNECT:
       if (commandNumber != ENET_PROTOCOL_COMMAND_VERIFY_CONNECT)
         return 0;

       host -> recalculateBandwidthLimits = 1;

       peer -> state = ENET_PEER_STATE_CONNECTED;

       event -> type = ENET_EVENT_TYPE_CONNECT;
       event -> peer = peer;

       return 1;

    case ENET_PEER_STATE_DISCONNECTING:
       if (commandNumber != ENET_PROTOCOL_COMMAND_DISCONNECT)
         return 0;

       host -> recalculateBandwidthLimits = 1;

       event -> type = ENET_EVENT_TYPE_DISCONNECT;
       event -> peer = peer;

       enet_peer_reset (peer);

       return 1;

    default:
       break;
    }
   
    return 0;
}

static void
enet_protocol_handle_verify_connect (ENetHost * host, ENetEvent * event, ENetPeer * peer, const ENetProtocol * command)
{
    enet_uint16 mtu;
    enet_uint32 windowSize;

    if (command -> header.commandLength < sizeof (ENetProtocolVerifyConnect) ||
        peer -> state != ENET_PEER_STATE_CONNECTING)
      return;

    if (ENET_NET_TO_HOST_32 (command -> verifyConnect.channelCount) != peer -> channelCount ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleInterval) != peer -> packetThrottleInterval ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleAcceleration) != peer -> packetThrottleAcceleration ||
        ENET_NET_TO_HOST_32 (command -> verifyConnect.packetThrottleDeceleration) != peer -> packetThrottleDeceleration)
    {
        peer -> state = ENET_PEER_STATE_ZOMBIE;

        return;
    }

    peer -> outgoingPeerID = ENET_NET_TO_HOST_16 (command -> verifyConnect.outgoingPeerID);

    mtu = ENET_NET_TO_HOST_16 (command -> verifyConnect.mtu);

    if (mtu < ENET_PROTOCOL_MINIMUM_MTU)
      mtu = ENET_PROTOCOL_MINIMUM_MTU;
    else 
    if (mtu > ENET_PROTOCOL_MAXIMUM_MTU)
      mtu = ENET_PROTOCOL_MAXIMUM_MTU;

    if (mtu < peer -> mtu)
      peer -> mtu = mtu;

    windowSize = ENET_NET_TO_HOST_32 (command -> verifyConnect.windowSize);

    if (windowSize < ENET_PROTOCOL_MINIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MINIMUM_WINDOW_SIZE;

    if (windowSize > ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE)
      windowSize = ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE;

    if (windowSize < peer -> windowSize)
      peer -> windowSize = windowSize;

    peer -> incomingBandwidth = ENET_NET_TO_HOST_32 (command -> verifyConnect.incomingBandwidth);
    peer -> outgoingBandwidth = ENET_NET_TO_HOST_32 (command -> verifyConnect.outgoingBandwidth);

    host -> recalculateBandwidthLimits = 1;

    peer -> state = ENET_PEER_STATE_CONNECTED;

    event -> type = ENET_EVENT_TYPE_CONNECT;
    event -> peer = peer;
}

static int
enet_protocol_handle_incoming_commands (ENetHost * host, ENetEvent * event)
{
    ENetProtocolHeader * header;
    ENetProtocol * command;
    ENetPeer * peer;
    enet_uint8 * currentData;
    size_t commandCount;

    if (host -> receivedDataLength < sizeof (ENetProtocolHeader))
      return 0;

    header = (ENetProtocolHeader *) host -> receivedData;

    header -> peerID = ENET_NET_TO_HOST_16 (header -> peerID);
    header -> sentTime = ENET_NET_TO_HOST_32 (header -> sentTime);

    if (header -> peerID == 0xFFFF)
      peer = NULL;
    else
    if (header -> peerID >= host -> peerCount)
      return 0;
    else
    {
       peer = & host -> peers [header -> peerID];

       if (peer -> state == ENET_PEER_STATE_DISCONNECTED ||
           peer -> state == ENET_PEER_STATE_ZOMBIE || 
           (host -> receivedAddress.host != peer -> address.host &&
             peer -> address.host != ENET_HOST_BROADCAST) ||
           header -> challenge != peer -> challenge)
         return 0;
       else
       {
           peer -> address.host = host -> receivedAddress.host;
           peer -> address.port = host -> receivedAddress.port;
       }
    }

    if (peer != NULL)
      peer -> incomingDataTotal += host -> receivedDataLength;

    commandCount = header -> commandCount;
    currentData = host -> receivedData + sizeof (ENetProtocolHeader);
  
    while (commandCount > 0 &&
           currentData < & host -> receivedData [host -> receivedDataLength])
    {
       command = (ENetProtocol *) currentData;

       if (currentData + sizeof (ENetProtocolCommandHeader) > & host -> receivedData [host -> receivedDataLength])
         return 0;

       command -> header.commandLength = ENET_NET_TO_HOST_32 (command -> header.commandLength);

       if (currentData + command -> header.commandLength > & host -> receivedData [host -> receivedDataLength])
         return 0;

       -- commandCount;
       currentData += command -> header.commandLength;

       if (peer == NULL)
       {
          if (command -> header.command != ENET_PROTOCOL_COMMAND_CONNECT)
            return 0;
       }
         
       command -> header.reliableSequenceNumber = ENET_NET_TO_HOST_32 (command -> header.reliableSequenceNumber);

       switch (command -> header.command)
       {
       case ENET_PROTOCOL_COMMAND_ACKNOWLEDGE:
          enet_protocol_handle_acknowledge (host, event, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_CONNECT:
          peer = enet_protocol_handle_connect (host, header, command);

          break;

       case ENET_PROTOCOL_COMMAND_VERIFY_CONNECT:
          enet_protocol_handle_verify_connect (host, event, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_DISCONNECT:
          enet_protocol_handle_disconnect (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_PING:
          enet_protocol_handle_ping (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_SEND_RELIABLE:
          enet_protocol_handle_send_reliable (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE:
          enet_protocol_handle_send_unreliable (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED:
          enet_protocol_handle_send_unsequenced (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_SEND_FRAGMENT:
          enet_protocol_handle_send_fragment (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT:
          enet_protocol_handle_bandwidth_limit (host, peer, command);

          break;

       case ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE:
          enet_protocol_handle_throttle_configure (host, peer, command);

          break;

       default:
          break;
       }

       if (peer != NULL &&
           (command -> header.flags & ENET_PROTOCOL_FLAG_ACKNOWLEDGE) != 0)
       {
           switch (peer -> state)
           {
           case ENET_PEER_STATE_DISCONNECTING:
              break;

           case ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT:
              if (command -> header.command != ENET_PROTOCOL_COMMAND_DISCONNECT)
                break;

           default:   
              enet_peer_queue_acknowledgement (peer, command, header -> sentTime);        

              break;
           }
       }
    }

    if (event -> type != ENET_EVENT_TYPE_NONE)
      return 1;

    return 0;
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

