// server.cpp: little more than enhanced multicaster
// runs dedicated or as client coroutine

#include "cube.h" 

enum { ST_EMPTY, ST_LOCAL, ST_TCPIP }; 
struct client				   // server side version of "dynent" type
{
	int type;
	ENetPeer *peer;
	string hostname;
	string mapvote;
	string name;
	int modevote;
};

vector<client> clients; 
int maxclients = 8;
string smapname;
struct server_entity			// server side version of "entity" type
{
	bool spawned;
	int spawnsecs;
};

vector<server_entity> sents;

bool notgotitems = true;		// true when map has changed and waiting for clients to send item
int mode = 0;

void restoreserverstate(vector<entity> &ents)   // hack: called from savegame code, only works in SP
{
	loopv(sents)
	{
		sents[i].spawned = ents[i].spawned;
		sents[i].spawnsecs = 0;
	}; 
};

int interm = 0, minremain = 0, mapend = 0;
bool mapreload = false; 
bool isdedicated;
ENetHost * serverhost = NULL;
int bsend = 0, brec = 0, laststatus = 0, lastsec = 0; 
#define MAXOBUF 100000 
void process(ENetPacket *packet, int sender);
void multicast(ENetPacket *packet, int sender);
void disconnect_client(int n, const char *reason);

void send(int n, ENetPacket *packet)
{
	if(!packet) return;
	switch(clients[n].type)
	{
		case ST_TCPIP:
		{
			enet_peer_send(clients[n].peer, 0, packet);
			bsend += packet->dataLength;
			break;
		};
		case ST_LOCAL:
			localservertoclient(packet->data, packet->dataLength);
			break;
	};
};

void send2(bool rel, int cn, int a, int b)
{
	ENetPacket *packet = enet_packet_create(NULL, 32, rel ? ENET_PACKET_FLAG_RELIABLE : 0);
	uchar *start = packet->data;
	uchar *p = start+2;
	putint(p, a);
	putint(p, b);
	*(ushort *)start = ENET_HOST_TO_NET_16(p-start);
	enet_packet_resize(packet, p-start);
	if(cn<0) process(packet, -1);
	else send(cn, packet);
	if(packet->referenceCount==0) enet_packet_destroy(packet);
};

void sendservmsg(char *msg)
{
	ENetPacket *packet = enet_packet_create(NULL, _MAXDEFSTR+10, ENET_PACKET_FLAG_RELIABLE);
	uchar *start = packet->data;
	uchar *p = start+2;
	putint(p, SV_SERVMSG);
	sendstring(msg, p);
	*(ushort *)start = ENET_HOST_TO_NET_16(p-start);
	enet_packet_resize(packet, p-start);
	multicast(packet, -1);
	if(packet->referenceCount==0) enet_packet_destroy(packet);
};

void disconnect_client(int n, const char *reason)
{
	printf("disconnecting client (%s) [%s]\n", clients[n].hostname, reason);
	enet_peer_disconnect(clients[n].peer);
	clients[n].type = ST_EMPTY;
	send2(true, -1, SV_CDIS, n);
};

void resetitems() { sents.setsize(0); notgotitems = true; };

void pickup(uint i, int sec, int sender)		 // server side item pickup, acknowledge first client that gets it
{
	if(i>=(uint)sents.length()) return;
	if(sents[i].spawned)
	{
		sents[i].spawned = false;
		sents[i].spawnsecs = sec;
		send2(true, sender, SV_ITEMACC, i);
	};
};

void resetvotes()
{
	loopv(clients) clients[i].mapvote[0] = 0;
}; 

// server side processing of updates: does very little and most state is tracked client only
// could be extended to move more gameplay to server (at expense of lag)

void process(ENetPacket * packet, int sender)   // sender may be -1
{
	;// does nothing
};

void send_welcome(int n)
{ 
	ENetPacket * packet = enet_packet_create (NULL, MAXTRANS, ENET_PACKET_FLAG_RELIABLE); 
	
	uchar *start = packet->data;
	uchar *p = start+2;
	putint(p, SV_INITS2C);
	putint(p, n);
	putint(p, PROTOCOL_VERSION);
	/*
	putint(p, smapname[0]);
	//sendstring(serverpassword, p);

	putint(p, clients.length()>maxclients);
	if(smapname[0])
	{
		putint(p, SV_MAPCHANGE);
		sendstring(smapname, p);
		putint(p, mode);
		putint(p, SV_ITEMLIST);
		loopv(sents) if(sents[i].spawned) putint(p, i);
		putint(p, -1);
	};
	
	*(ushort *)start = ENET_HOST_TO_NET_16(p-start);

	enet_packet_resize(packet, p-start);
	*/
	send(n, packet);


};

void multicast(ENetPacket *packet, int sender)
{
	loopv(clients)
	{
		if(i==sender) continue;
		send(i, packet);
	};
};

void localclienttoserver(ENetPacket *packet)
{
	process(packet, 0);
	if(!packet->referenceCount) enet_packet_destroy (packet);
};

client &addclient()
{
	loopv(clients) if(clients[i].type==ST_EMPTY) return clients[i];
	return clients.add();
};

void checkintermission()
{
	if(!minremain)
	{
		interm = lastsec+10;
		mapend = lastsec+1000;
	};
	send2(true, -1, SV_TIMEUP, minremain--);
};

void startintermission() { minremain = 0; checkintermission(); }; 

void cleanupserver()
{
	if(serverhost) enet_host_destroy(serverhost);
};

void localdisconnect()
{
	loopv(clients) if(clients[i].type==ST_LOCAL) clients[i].type = ST_EMPTY;
};

void localconnect()
{
	client &c = addclient();
	c.type = ST_LOCAL;
	strcpy_s(c.hostname, "local");
	send_welcome(&c-&clients[0]); 
}; 

// all network traffic is in 32bit ints, which are then compressed using the following simple scheme (assumes that most values are small).

void putint(uchar *&p, int n)
{
	if(n<128 && n>-127) { *p++ = n; }
	else if(n<0x8000 && n>=-0x8000) { *p++ = 0x80; *p++ = n; *p++ = n>>8;  }
	else { *p++ = 0x81; *p++ = n; *p++ = n>>8; *p++ = n>>16; *p++ = n>>24; };
};

int getint(uchar *&p)
{
	int c = *((char *)p);
	p++;
	if(c==-128) { int n = *p++; n |= *((char *)p)<<8; p++; return n;}
	else if(c==-127) { int n = *p++; n |= *p++<<8; n |= *p++<<16; return n|(*p++<<24); } 
	else return c;
};

void sendstring(char *t, uchar *&p)
{
	while(*t) putint(p, *t++);
	putint(p, 0);
};

const char *modenames[] =
{
	"SP", "DMSP", "ffa/default", "coopedit", "ffa/duel", "teamplay",
	"instagib", "instagib team", "efficiency", "efficiency team",
	"insta arena", "insta clan arena", "tactics arena", "tactics clan arena",
};
	  
const char *modestr(int n) { return (n>=-2 && n<12) ? modenames[n+2] : "unknown"; };

char msgsizesl[] =  // size inclusive message token, 0 for variable or not-checked sizes
{ 
	SV_INITS2C, 4, SV_INITC2S, 0, SV_POS, 12, SV_TEXT, 0, SV_SOUND, 2, SV_CDIS, 2,
	SV_EDITH, 7, SV_EDITT, 7, SV_EDITS, 6, SV_EDITD, 6, SV_EDITE, 6,
	SV_DIED, 2, SV_DAMAGE, 4, SV_SHOT, 8, SV_FRAGS, 2,
	SV_MAPCHANGE, 0, SV_ITEMSPAWN, 2, SV_ITEMPICKUP, 3, SV_DENIED, 2,
	SV_PING, 2, SV_PONG, 2, SV_CLIENTPING, 2, SV_GAMEMODE, 2,
	SV_TIMEUP, 2, SV_EDITENT, 10, SV_MAPRELOAD, 2, SV_ITEMACC, 2,
	SV_SENDMAP, 0, SV_RECVMAP, 1, SV_SERVMSG, 0, SV_ITEMLIST, 0,
	SV_EXT, 0,
	-1
};

char msgsizelookup(int msg)
{
	for(char *p = msgsizesl; *p>=0; p += 2) if(*p==msg) return p[1];
	return -1;
}; 

