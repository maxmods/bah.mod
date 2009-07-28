/*
  Copyright (c) 2007-2009 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 

#include "glue.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxSocketDescriptor::MaxSocketDescriptor(const SocketDescriptor d)
	: descriptor(d)
{
}

MaxSocketDescriptor::MaxSocketDescriptor(unsigned short port, const char *hostAddress)
	: descriptor(port, strdup(hostAddress))
{
	
}

MaxSocketDescriptor::~MaxSocketDescriptor() {
}

SocketDescriptor & MaxSocketDescriptor::Descriptor() {
	return descriptor;
}

MaxNetworkID::MaxNetworkID(const NetworkID & n)
	: networkID(n)
{
}

MaxNetworkID::~MaxNetworkID() {
}

NetworkID & MaxNetworkID::ID() {
	return networkID;
}

MaxSystemAddress::MaxSystemAddress()
{
}

MaxSystemAddress::MaxSystemAddress(const SystemAddress & a)
	: systemAddress(a)
{
}

MaxSystemAddress::~MaxSystemAddress() {
}

SystemAddress & MaxSystemAddress::Address() {
	return systemAddress;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_raknet_seedMT(unsigned int seed) {
	seedMT(seed);
}

unsigned int bmx_raknet_randomMT() {
	return randomMT();
}

float bmx_raknet_frandomMT() {
	return frandomMT();
}

void bmx_raknet_fillBufferMT(void * buffer, unsigned int size) {
	fillBufferMT(buffer, size);
}

void bmx_raknet_gettime(BBInt64 * v) {
#ifdef __GET_TIME_64BIT
	*v = RakNet::GetTime();
#else
	*v = static_cast<BBInt64>(RakNet::GetTime());
#endif
}

void bmx_raknet_gettimens(RakNetTimeUS * v) {
	*v = RakNet::GetTimeUS();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_raknet_SizeOfRakNetTime() {
	return sizeof(RakNetTime);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RakPeerInterface * bmx_RakNetworkFactory_GetRakPeerInterface() {
	return RakNetworkFactory::GetRakPeerInterface();
}

ConsoleServer * bmx_RakNetworkFactory_GetConsoleServer() {
	return RakNetworkFactory::GetConsoleServer();
}

ReplicaManager * bmx_RakNetworkFactory_GetReplicaManager() {
	return RakNetworkFactory::GetReplicaManager();
}

LogCommandParser * bmx_RakNetworkFactory_GetLogCommandParser() {
	return RakNetworkFactory::GetLogCommandParser();
}

PacketLogger * bmx_RakNetworkFactory_GetPacketLogger() {
	RakNetworkFactory::GetPacketLogger();
}

RakNetCommandParser * bmx_RakNetworkFactory_GetRakNetCommandParser() {
	return RakNetworkFactory::GetRakNetCommandParser();
}

RakNetTransport * bmx_RakNetworkFactory_GetRakNetTransport() {
	return RakNetworkFactory::GetRakNetTransport();
}

TelnetTransport * bmx_RakNetworkFactory_GetTelnetTransport() {
	return RakNetworkFactory::GetTelnetTransport();
}

PacketConsoleLogger * bmx_RakNetworkFactory_GetPacketConsoleLogger() {
	return RakNetworkFactory::GetPacketConsoleLogger();
}

PacketFileLogger * bmx_RakNetworkFactory_GetPacketFileLogger() {
	return RakNetworkFactory::GetPacketFileLogger();
}

Router * bmx_RakNetworkFactory_GetRouter() {
	return RakNetworkFactory::GetRouter();
}

ConnectionGraph * bmx_RakNetworkFactory_GetConnectionGraph() {
	return RakNetworkFactory::GetConnectionGraph();
}

void bmx_RakNetworkFactory_DestroyRakPeerInterface(RakPeerInterface * peer) {
	RakNetworkFactory::DestroyRakPeerInterface(peer);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxSocketDescriptor * bmx_SocketDescriptor_new(int port, BBString * hostAddress) {
	char * s = 0;
	if (hostAddress != &bbEmptyString) {
		s = bbStringToCString(hostAddress);
	}
	MaxSocketDescriptor * descriptor = new MaxSocketDescriptor(static_cast<unsigned short>(port), s);
	if (s) bbMemFree(s);
	return descriptor;
}

void bmx_SocketDescriptor_delete(MaxSocketDescriptor * descriptor) {
	delete descriptor;
}

void bmx_SocketDescriptor_setport(MaxSocketDescriptor * desc, int port) {
	desc->Descriptor().port = static_cast<unsigned short>(port);
}

int bmx_SocketDescriptor_getport(MaxSocketDescriptor * desc) {
	return static_cast<int>(desc->Descriptor().port);
}

void bmx_SocketDescriptor_sethostaddress(MaxSocketDescriptor * desc, BBString * hostAddress) {
	char * a = 0;
	if (hostAddress != &bbEmptyString) {
		a = bbStringToCString(hostAddress);
	}

	if (a) {
		strcpy(desc->Descriptor().hostAddress, a);
		bbMemFree(a);
	} else {
		desc->Descriptor().hostAddress[0] = 0;
	}
	
}

BBString * bmx_SocketDescriptor_gethostaddress(MaxSocketDescriptor * desc) {
	return bbStringFromCString(desc->Descriptor().hostAddress);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool bmx_RakPeer_Startup(RakPeerInterface * peer, int maxConnections, int threadSleepTimer, MaxSocketDescriptor * descriptor) {
	if (descriptor) {
		return peer->Startup(static_cast<unsigned short>(maxConnections), threadSleepTimer, &descriptor->Descriptor(), 1);
	} else {
		return peer->Startup(static_cast<unsigned short>(maxConnections), threadSleepTimer, &SocketDescriptor(), 1);
	}
}

Packet * bmx_RakPeer_Receive(RakPeerInterface * peer) {
	return peer->Receive();
}

void bmx_RakPeer_DeallocatePacket(RakPeerInterface * peer, Packet * packet) {
	peer->DeallocatePacket(packet);
}

void bmx_RakPeer_SetMaximumIncomingConnections(RakPeerInterface * peer, int numberAllowed) {
	peer->SetMaximumIncomingConnections(static_cast<unsigned short>(numberAllowed));
}

bool bmx_RakPeer_Connect(RakPeerInterface * peer, BBString * host, int remotePort, BBString * passwordData) {
	char * h = 0;
	char * p = 0;
	int len = 0;
	
	if (host != &bbEmptyString) {
		h = bbStringToCString(host);
	}
	if (passwordData != &bbEmptyString) {
		p = bbStringToCString(passwordData);
		len = passwordData->length;
	}
	bool ret = peer->Connect(h, static_cast<unsigned short>(remotePort), p, len);
	if (h) bbMemFree(h);
	if (p) bbMemFree(p);
	return ret;
}

void bmx_RakPeer_RegisterAsRemoteProcedureCall(RakPeerInterface * peer, BBString * id, void(*params)(RPCParameters *rpcParms)) {
	char * i = 0;
	i = bbStringToCString(id);
	
	peer->RegisterAsRemoteProcedureCall(i, params);

	bbMemFree(i);
}


void bmx_RakPeer_DisableSecurity(RakPeerInterface * peer) {
	peer->DisableSecurity();
}

int bmx_RakPeer_GetMaximumIncomingConnections(RakPeerInterface * peer) {
	return static_cast<int>(peer->GetMaximumIncomingConnections());
}

int bmx_RakPeer_NumberOfConnections(RakPeerInterface * peer) {
	return static_cast<int>(peer->NumberOfConnections());
}

void bmx_RakPeer_SetIncomingPassword(RakPeerInterface * peer, BBString * passwordData) {
	char * p = 0;
	int len = 0;
	
	if (passwordData != &bbEmptyString) {
		p = bbStringToCString(passwordData);
		len = passwordData->length;
	}
	peer->SetIncomingPassword(p, len);
	if (p) bbMemFree(p);
}

BBString * bmx_RakPeer_GetIncomingPassword(RakPeerInterface * peer) {
	char p[2048];
	int len = 2048;
	peer->GetIncomingPassword(p, &len);
	return bbStringFromCString(p);
}

void bmx_RakPeer_Shutdown(RakPeerInterface * peer, int blockDuration, int orderingChannel) {
	peer->Shutdown(blockDuration, static_cast<unsigned char>(orderingChannel));
}

bool bmx_RakPeer_IsActive(RakPeerInterface * peer) {
	return peer->IsActive();
}

void bmx_RakPeer_UnregisterAsRemoteProcedureCall(RakPeerInterface * peer, BBString * name) {
	char * n = 0;

	if (name != &bbEmptyString) {
		n = bbStringToCString(name);
	}
	peer->UnregisterAsRemoteProcedureCall(n);
	if (n) bbMemFree(n);
}

bool bmx_RakPeer_RPC(RakPeerInterface * peer, const char * id, const char * data, unsigned int bitLength, 
		PacketPriority priority, PacketReliability reliability, int 		orderingChannel,
		MaxSystemAddress * addr, bool broadcast, BBInt64 * includedTimestamp, 
		RakNet::BitStream * replyFromTarget) {

	bool ret;
#ifndef __GET_TIME_64BIT
	RakNetTime timestamp = static_cast<RakNetTime>(*includedTimestamp);
#endif
	if (addr) {
		ret = peer->RPC(id, data, bitLength, priority, reliability, static_cast<char>(orderingChannel),
				addr->Address(), broadcast, 
#ifdef __GET_TIME_64BIT
				(RakNetTime*)includedTimestamp
#else
				&timestamp
#endif
				,UNASSIGNED_NETWORK_ID, replyFromTarget);
	} else {
		ret = peer->RPC(id, data, bitLength, priority, reliability, static_cast<char>(orderingChannel),
				UNASSIGNED_SYSTEM_ADDRESS, broadcast,
#ifdef __GET_TIME_64BIT
				(RakNetTime*)includedTimestamp
#else
				&timestamp
#endif
				,UNASSIGNED_NETWORK_ID, replyFromTarget);
	}
	
	return ret;
}

void bmx_RakPeer_AddToSecurityExceptionList(RakPeerInterface * peer, BBString * ip) {
	char * p = bbStringToCString(ip);
	peer->AddToSecurityExceptionList(p);
	bbMemFree(p);
}

void bmx_RakPeer_RemoveFromSecurityExceptionList(RakPeerInterface * peer, BBString * ip) {
	char * p = 0;
	
	if (ip != &bbEmptyString) {
		p = bbStringToCString(ip);
	}
	peer->RemoveFromSecurityExceptionList(p);
	if (p) bbMemFree(p);
}

bool bmx_RakPeer_IsInSecurityExceptionList(RakPeerInterface * peer, BBString * ip) {
	char * p = bbStringToCString(ip);
	bool ret = peer->IsInSecurityExceptionList(p);
	bbMemFree(p);
	return ret;
}

int bmx_RakPeer_GetMaximumNumberOfPeers(RakPeerInterface * peer) {
	return static_cast<int>(peer->GetMaximumNumberOfPeers());
}

void bmx_RakPeer_InitializeSecurity(RakPeerInterface * peer, uint32_t * pubKeyE, const char * pubKeyN,
		const char * privKeyP, const char * privKeyQ) {
	
	peer->InitializeSecurity((char*)pubKeyE, pubKeyN, privKeyP, privKeyQ);
}

bool bmx_RakPeer_SendBitStream(RakPeerInterface * peer, RakNet::BitStream * stream, PacketPriority priority, 
		PacketReliability reliability, int orderingChannel, MaxSystemAddress * addr, bool broadcast) {

	bool ret;

	if (addr) {
		ret = peer->Send(stream, priority, reliability, static_cast<char>(orderingChannel),
				addr->Address(), broadcast);
	} else {
		ret = peer->Send(stream, priority, reliability, static_cast<char>(orderingChannel),
				UNASSIGNED_SYSTEM_ADDRESS, broadcast);
	}
	
	return ret;

}

void bmx_RakPeer_SetOccasionalPing(RakPeerInterface * peer, bool doPing) {
	peer->SetOccasionalPing(doPing);
}

MaxSystemAddress * bmx_RakPeer_GetSystemAddressFromIndex(RakPeerInterface * peer, int index) {
	return new MaxSystemAddress(peer->GetSystemAddressFromIndex(index));
}

void bmx_RakPeer_CloseConnection(RakPeerInterface * peer, MaxSystemAddress * target, bool sendDisconnectionNotification, bool orderingChannel) {
	peer->CloseConnection(target->Address(), sendDisconnectionNotification, orderingChannel);
}

bool bmx_RakPeer_IsConnected(RakPeerInterface * peer, MaxSystemAddress * addr, bool includeInProgress, bool includeDisconnecting) {
	return peer->IsConnected(addr->Address(), includeInProgress, includeDisconnecting);
}

int bmx_RakPeer_GetIndexFromSystemAddress(RakPeerInterface * peer, MaxSystemAddress * addr) {
	return peer->GetIndexFromSystemAddress(addr->Address());
}

void bmx_RakPeer_AddToBanList(RakPeerInterface * peer, BBString * ip, BBInt64 milliseconds) {
	char * p = bbStringToCString(ip);
#ifdef __GET_TIME_64BIT
	peer->AddToBanList(p, milliseconds);
#else
	peer->AddToBanList(p, static_cast<int>(milliseconds));
#endif
	bbMemFree(p);
}

void bmx_RakPeer_RemoveFromBanList(RakPeerInterface * peer, BBString * ip) {
	char * p = bbStringToCString(ip);
	peer->RemoveFromBanList(p);
	bbMemFree(p);
}

void bmx_RakPeer_ClearBanList(RakPeerInterface * peer) {
	peer->ClearBanList();
}

bool bmx_RakPeer_IsBanned(RakPeerInterface * peer, BBString * ip) {
	char * p = bbStringToCString(ip);
	bool ret = peer->IsBanned(p);
	bbMemFree(p);
}

void bmx_RakPeer_Ping(RakPeerInterface * peer, MaxSystemAddress * addr) {
	peer->Ping(addr->Address());
}

int bmx_RakPeer_GetAveragePing(RakPeerInterface * peer, MaxSystemAddress * addr) {
	return peer->GetAveragePing(addr->Address());
}

int bmx_RakPeer_GetLastPing(RakPeerInterface * peer, MaxSystemAddress * addr) {
	return peer->GetLastPing(addr->Address());
}

int bmx_RakPeer_GetLowestPing(RakPeerInterface * peer, MaxSystemAddress * addr) {
	return peer->GetLowestPing(addr->Address());
}

void bmx_RakPeer_SetOfflinePingResponse(RakPeerInterface * peer, BBString * data) {
	char * p = 0;
	int n = 0;
	
	if (data != &bbEmptyString) {
		p = bbStringToCString(data);
		n = strlen(p);
	}
	
	peer->SetOfflinePingResponse(p, n);
	if (p) bbMemFree(p);
}

BBString * bmx_RakPeer_GetOfflinePingResponse(RakPeerInterface * peer) {
	char * p;
	unsigned int length;
	peer->GetOfflinePingResponse(&p, &length);
	return bbStringFromBytes(p, length);
}

void bmx_RakPeer_SetCompileFrequencyTable(RakPeerInterface * peer, bool doCompile) {
	peer->SetCompileFrequencyTable(doCompile);
}

void bmx_RakPeer_AttachPlugin(RakPeerInterface * peer, PluginInterface2 * 	plugin	) {
	peer->AttachPlugin(plugin);
}

void bmx_RakPeer_DetachPlugin(RakPeerInterface * peer, PluginInterface2 * 	plugin	) {
	peer->DetachPlugin(plugin);
}

bool bmx_RakPeer_Send(RakPeerInterface * peer, const char * data, const int length, PacketPriority priority, 
		PacketReliability reliability, int orderingChannel, MaxSystemAddress * addr, bool broadcast) {

	bool ret;

	if (addr) {
		ret = peer->Send(data, length, priority, reliability, static_cast<char>(orderingChannel),
				addr->Address(), broadcast);
	} else {
		ret = peer->Send(data, length, priority, reliability, static_cast<char>(orderingChannel),
				UNASSIGNED_SYSTEM_ADDRESS, broadcast);
	}
	
	return ret;
}

BBString * bmx_RakPeer_GetLocalIP(RakPeerInterface * peer, int index) {
	return bbStringFromCString(peer->GetLocalIP(index));
}

bool bmx_RakPeer_IsLocalIP(RakPeerInterface * peer, BBString * ip) {
	char * p = bbStringToCString(ip);
	bool ret = peer->IsLocalIP(p);
	bbMemFree(p);
	return ret;
}

RakNetGUID * bmx_RakPeer_GetGuidFromSystemAddress(RakPeerInterface * peer, MaxSystemAddress * systemAddress) {
	return new RakNetGUID(peer->GetGuidFromSystemAddress(systemAddress->Address()));
}

RakNetStatistics * bmx_RakPeer_GetStatistics(RakPeerInterface * peer, MaxSystemAddress * systemAddress) {
	return peer->GetStatistics(systemAddress->Address());
}

MaxSystemAddress * bmx_RakPeer_GetInternalID(RakPeerInterface * peer, MaxSystemAddress * systemAddress) {
	return new MaxSystemAddress(peer->GetInternalID(systemAddress->Address()));
}

MaxSystemAddress * bmx_RakPeer_GetExternalID(RakPeerInterface * peer, MaxSystemAddress * systemAddress) {
	return new MaxSystemAddress(peer->GetExternalID(systemAddress->Address()));
}

void bmx_RakPeer_SetTimeoutTime(RakPeerInterface * peer, BBInt64 timeMS, MaxSystemAddress * target) {
#ifdef __GET_TIME_64BIT
	peer->SetTimeoutTime(timeMS, target->Address());
#else
	peer->SetTimeoutTime(static_cast<RakNetTime>(timeMS), target->Address());
#endif
}

bool bmx_RakPeer_SetMTUSize(RakPeerInterface * peer, int size) {
	return peer->SetMTUSize(size);
}

int bmx_RakPeer_GetMTUSize(RakPeerInterface * peer, MaxSystemAddress * target) {
	return peer->GetMTUSize(target->Address());
}

int bmx_RakPeer_GetNumberOfAddresses(RakPeerInterface * peer) {
	return static_cast<int>(peer->GetNumberOfAddresses());
}

bool bmx_RakPeer_DeleteCompressionLayer(RakPeerInterface * peer, bool inputLayer) {
	return peer->DeleteCompressionLayer(inputLayer);
}

float bmx_RakPeer_GetCompressionRatio(RakPeerInterface * peer) {
	return peer->GetCompressionRatio();
}

float bmx_RakPeer_GetDecompressionRatio(RakPeerInterface * peer) {
	return peer->GetDecompressionRatio();
}

void bmx_RakPeer_ApplyNetworkSimulator(RakPeerInterface * peer, double maxSendBPS, int minExtraPing, int extraPingVariance) {
	peer->ApplyNetworkSimulator(maxSendBPS, static_cast<unsigned short>(minExtraPing), static_cast<unsigned short>(extraPingVariance));
}

void bmx_RakPeer_SetPerConnectionOutgoingBandwidthLimit(RakPeerInterface * peer, int maxBitsPerSecond) {
	peer->SetPerConnectionOutgoingBandwidthLimit(static_cast<unsigned>(maxBitsPerSecond));
}

bool bmx_RakPeer_IsNetworkSimulatorActive(RakPeerInterface * peer) {
	return peer->IsNetworkSimulatorActive();
}

void bmx_RakPeer_AllowConnectionResponseIPMigration(RakPeerInterface * peer, bool allow) {
	peer->AllowConnectionResponseIPMigration(allow);
}

bool bmx_RakPeer_AdvertiseSystem(RakPeerInterface * peer, BBString * host, int remotePort, const char * data, int dataLength, int connectionSocketIndex) {
	char * p = bbStringToCString(host);
	bool ret = peer->AdvertiseSystem(p, static_cast<unsigned short>(remotePort), data, dataLength, static_cast<unsigned>(connectionSocketIndex));
	bbMemFree(p);
	return ret;
}

void bmx_RakPeer_SetSplitMessageProgressInterval(RakPeerInterface * peer, int interval) {
	peer->SetSplitMessageProgressInterval(interval);
}

void bmx_RakPeer_SetUnreliableTimeout(RakPeerInterface * peer, BBInt64 timeoutMS) {
#ifdef __GET_TIME_64BIT
	peer->SetUnreliableTimeout(timeoutMS);
#else
	peer->SetUnreliableTimeout(static_cast<RakNetTime>(timeoutMS));
#endif
}

void bmx_RakPeer_SendTTL(RakPeerInterface * peer, BBString * host, int remotePort, int ttl, int connectionSocketIndex) {
	char * p = bbStringToCString(host);
	peer->SendTTL(p, static_cast<unsigned short>(remotePort), ttl, static_cast<unsigned>(connectionSocketIndex));
	bbMemFree(p);
}

void bmx_RakPeer_PushBackPacket(RakPeerInterface * peer, Packet * packet, bool pushAtHead) {
	peer->PushBackPacket(packet, pushAtHead);
}

Packet * bmx_RakPeer_AllocatePacket(RakPeerInterface * peer, int dataSize) {
	return peer->AllocatePacket(static_cast<unsigned>(dataSize));
}

void bmx_RakPeer_PingHost(RakPeerInterface * peer, BBString * host, int remotePort, bool onlyReplyOnAcceptingConnections, int connectionSocketIndex) {
	char * p = bbStringToCString(host);
	peer->Ping(p, remotePort, onlyReplyOnAcceptingConnections, static_cast<unsigned>(connectionSocketIndex));
	bbMemFree(p);
}

void bmx_RakPeer_SetNetworkIDManager(RakPeerInterface * peer, NetworkIDManager * manager) {
	peer->SetNetworkIDManager(manager);
}

NetworkIDManager * bmx_RakPeer_GetNetworkIDManager(RakPeerInterface * peer) {
	return peer->GetNetworkIDManager();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned char * bmx_Packet_GetData(Packet * packet) {
	return packet->data;
}

unsigned int bmx_Packet_GetBitSize(Packet * packet) {
	return packet->bitSize;
}

MaxSystemAddress * bmx_Packet_GetSystemAddress(Packet * packet) {
	return new MaxSystemAddress(packet->systemAddress);
}

int bmx_Packet_GetPacketIdentifier(Packet * packet) {
	unsigned char id = 255;

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		assert(packet->length > sizeof(unsigned char) + sizeof(RakNetTime));
		id = (unsigned char) packet->data[sizeof(unsigned char) + sizeof(RakNetTime)];
	}
	else {
		id = (unsigned char) packet->data[0];
	}
	return static_cast<int>(id);

}

RakNetGUID * bmx_Packet_GetGuid(Packet * packet) {
	return new RakNetGUID(packet->guid);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_RPCParameters_GetFunctionName(RPCParameters * params) {
	return bbStringFromCString(params->functionName);
}

unsigned char * bmx_RPCParameters_GetInput(RPCParameters * params) {
	return params->input;
}

unsigned int bmx_RPCParameters_GetNumberOfBitsOfData(RPCParameters * params) {
	return params->numberOfBitsOfData;
}

MaxSystemAddress * bmx_RPCParameters_GetSender(RPCParameters * params) {
	return new MaxSystemAddress(params->sender);
}

RakPeerInterface * bmx_RPCParameters_GetRecipient(RPCParameters * params) {
	return params->recipient;
}

RakNet::BitStream * bmx_RPCParameters_GetReplyToSender(RPCParameters * params) {
	return params->replyToSender;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_NetworkID_delete(MaxNetworkID * networkId) {
	delete networkId;
}

MaxNetworkID * bmx_NetworkID_unassigned() {
	return new MaxNetworkID(UNASSIGNED_NETWORK_ID);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_SystemAddress_delete(MaxSystemAddress * address) {
	delete address;
}

MaxSystemAddress * bmx_SystemAddress_unassigned() {
	return new MaxSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
}

bool bmx_SystemAddress_Equals(MaxSystemAddress * address, MaxSystemAddress * other) {
	return address->Address().binaryAddress == other->Address().binaryAddress &&
		address->Address().port == other->Address().port;
}

BBString * bmx_SystemAddress_ToString(MaxSystemAddress * address) {
	return bbStringFromCString(address->Address().ToString(false));
}

int bmx_SystemAddress_GetBinaryAddress(MaxSystemAddress * address) {
	return static_cast<int>(address->Address().binaryAddress);
}

int bmx_SystemAddress_GetPort(MaxSystemAddress * address) {
	return static_cast<int>(address->Address().port);
}

MaxSystemAddress * bmx_SystemAddress_create() {
	return new MaxSystemAddress();
}

void bmx_SystemAddress_SetBinaryAddress(MaxSystemAddress * address, BBString * addr) {
	char * p = bbStringToCString(addr);
	address->Address().SetBinaryAddress(p);
	bbMemFree(p);
}

void bmx_SystemAddress_SetPort(MaxSystemAddress * address, int port) {
	address->Address().port = static_cast<unsigned short>(port);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RakNet::BitStream * bmx_BitStream_Create() {
	return new RakNet::BitStream();
}

RakNet::BitStream * bmx_BitStream_CreateFromData(unsigned char * data, unsigned int size, bool copy) {
	return new RakNet::BitStream(data, size, copy);
}

void bmx_BitStream_Delete(RakNet::BitStream * stream) {
	delete stream;
}

void bmx_BitStream_Reset(RakNet::BitStream * stream) {
	stream->Reset();
}

bool bmx_BitStream_SerializeByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeShort(RakNet::BitStream * stream, bool writeToBitstream, short * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeInt(RakNet::BitStream * stream, bool writeToBitstream, int * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value) {
	return stream->Serialize(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaLastByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue, unsigned char lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue, short lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue, int lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue, unsigned short lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue, unsigned int lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue, float lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaLastDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue, double lastValue) {
	return stream->SerializeDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeDeltaByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaShort(RakNet::BitStream * stream, bool writeToBitstream, short * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaInt(RakNet::BitStream * stream, bool writeToBitstream, int * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeDeltaDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value) {
	return stream->SerializeDelta(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedShort(RakNet::BitStream * stream, bool writeToBitstream, short * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedInt(RakNet::BitStream * stream, bool writeToBitstream, int * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value) {
	return stream->SerializeCompressed(writeToBitstream, value);
}

bool bmx_BitStream_SerializeCompressedDeltaLastByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue, unsigned char lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue, short lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue, int lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue, unsigned short lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue, unsigned int lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue, float lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaLastDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue, double lastValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, *currentValue, lastValue);
}

bool bmx_BitStream_SerializeCompressedDeltaByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_SerializeCompressedDeltaDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue) {
	return stream->SerializeCompressedDelta(writeToBitstream, currentValue);
}

bool bmx_BitStream_ReadByte(RakNet::BitStream * stream, unsigned char * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadShort(RakNet::BitStream * stream, short * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadInt(RakNet::BitStream * stream, int * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadUShort(RakNet::BitStream * stream, unsigned short * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadUInt(RakNet::BitStream * stream, unsigned int * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadFloat(RakNet::BitStream * stream, float * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadDouble(RakNet::BitStream * stream, double * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadLong(RakNet::BitStream * stream, BBInt64 * value) {
	return stream->Read(*value);
}

bool bmx_BitStream_ReadTime(RakNet::BitStream * stream, BBInt64 * value) {
#ifdef __GET_TIME_64BIT
	return stream->Read(value);
#else
	RakNetTime time;
	bool ret = stream->Read(time);
	*value = static_cast<BBInt64>(time);
	return ret;
#endif
}

BBString * bmx_BitStream_ReadString(RakNet::BitStream * stream, int length) {
	char data[length];
	bool ret = stream->Read(&data[0], length);
	if (!ret) {
		return &bbEmptyString;
	} else {
		return bbStringFromCString(&data[0]);
	}
}

bool bmx_BitStream_ReadDeltaByte(RakNet::BitStream * stream, unsigned char * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaShort(RakNet::BitStream * stream, short * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaInt(RakNet::BitStream * stream, int * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaUShort(RakNet::BitStream * stream, unsigned short * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaUInt(RakNet::BitStream * stream, unsigned int * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaFloat(RakNet::BitStream * stream, float * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadDeltaDouble(RakNet::BitStream * stream, double * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadCompressedByte(RakNet::BitStream * stream, unsigned char * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedShort(RakNet::BitStream * stream, short * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedInt(RakNet::BitStream * stream, int * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedUShort(RakNet::BitStream * stream, unsigned short * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedUInt(RakNet::BitStream * stream, unsigned int * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedFloat(RakNet::BitStream * stream, float * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedDouble(RakNet::BitStream * stream, double * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedDeltaByte(RakNet::BitStream * stream, unsigned char * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaShort(RakNet::BitStream * stream, short * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaInt(RakNet::BitStream * stream, int * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaUShort(RakNet::BitStream * stream, unsigned short * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaUInt(RakNet::BitStream * stream, unsigned int * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaFloat(RakNet::BitStream * stream, float * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadCompressedDeltaDouble(RakNet::BitStream * stream, double * value) {
	return stream->ReadCompressedDelta(*value);
}

bool bmx_BitStream_ReadBit(RakNet::BitStream * stream) {
	return stream->ReadBit();
}

bool bmx_BitStream_ReadDeltaBool(RakNet::BitStream * stream, bool * value) {
	return stream->ReadDelta(*value);
}

bool bmx_BitStream_ReadCompressedBool(RakNet::BitStream * stream, bool * value) {
	return stream->ReadCompressed(*value);
}

bool bmx_BitStream_ReadCompressedDeltaBool(RakNet::BitStream * stream, bool * value) {
	return stream->ReadCompressedDelta(*value);
}

void bmx_BitStream_WriteByte(RakNet::BitStream * stream, unsigned char * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteShort(RakNet::BitStream * stream, short * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteInt(RakNet::BitStream * stream, int * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteFloat(RakNet::BitStream * stream, float * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteDouble(RakNet::BitStream * stream, double * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteLong(RakNet::BitStream * stream, BBInt64 * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteTime(RakNet::BitStream * stream, BBInt64 * value) {
#ifdef __GET_TIME_64BIT
	stream->Write(*value);
#else
	stream->Write(static_cast<RakNetTime>(*value));
#endif
}

void bmx_BitStream_Write0(RakNet::BitStream * stream) {
	stream->Write0();
}

void bmx_BitStream_Write1(RakNet::BitStream * stream) {
	stream->Write1();
}
	
void bmx_BitStream_WriteUShort(RakNet::BitStream * stream, unsigned short * value) {
	stream->Write(*value);
}

void bmx_BitStream_WriteUInt(RakNet::BitStream * stream, unsigned int * value) {
	stream->Write(*value);
}


void bmx_BitStream_AssertStreamEmpty(RakNet::BitStream * stream) {
	stream->AssertStreamEmpty();
}

void bmx_BitStream_IgnoreBits(RakNet::BitStream * stream, int numberOfBits) {
	stream->IgnoreBits(numberOfBits);
}

void bmx_BitStream_IgnoreBytes(RakNet::BitStream * stream, int numberOfBytes) {
	stream->IgnoreBytes(numberOfBytes);
}

void bmx_BitStream_SetWriteOffset(RakNet::BitStream * stream, int offset) {
	stream->SetWriteOffset(offset);
}

int bmx_BitStream_GetNumberOfBitsUsed(RakNet::BitStream * stream) {
	return static_cast<int>(stream->GetNumberOfBitsUsed());
}

int bmx_BitStream_GetWriteOffset(RakNet::BitStream * stream) {
	return static_cast<int>(stream->GetWriteOffset());
}

int bmx_BitStream_GetNumberOfBytesUsed(RakNet::BitStream * stream) {
	return static_cast<int>(stream->GetNumberOfBytesUsed());
}

int bmx_BitStream_GetReadOffset(RakNet::BitStream * stream) {
	return static_cast<int>(stream->GetReadOffset());
}

void bmx_BitStream_SetReadOffset(RakNet::BitStream * stream, int offset) {
	stream->SetReadOffset(static_cast<BitSize_t>(offset));
}

int bmx_BitStream_GetNumberOfUnreadBits(RakNet::BitStream * stream) {
	return static_cast<int>(stream->GetNumberOfUnreadBits());
}

void bmx_BitStream_WriteBits(RakNet::BitStream * stream, const unsigned char * data, int numberOfBitsToWrite, int rightAlignedBits) {
	stream->WriteBits(data, static_cast<int>(numberOfBitsToWrite), static_cast<int>(rightAlignedBits));
}

void bmx_BitStream_WriteCompressedByte(RakNet::BitStream * stream, char * b) {
	stream->WriteCompressed(*b);
}

void bmx_BitStream_WriteCompressedShort(RakNet::BitStream * stream, short * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedUShort(RakNet::BitStream * stream, short * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedInt(RakNet::BitStream * stream, int * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedUInt(RakNet::BitStream * stream, int * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedFloat(RakNet::BitStream * stream, float * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedDouble(RakNet::BitStream * stream, double * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteCompressedLong(RakNet::BitStream * stream, BBInt64 * value) {
	stream->WriteCompressed(*value);
}

void bmx_BitStream_WriteDeltaByte(RakNet::BitStream * stream, char * b, char lastValue) {
	stream->WriteDelta(*b, lastValue);
}

void bmx_BitStream_WriteDeltaShort(RakNet::BitStream * stream, short * currentValue, short lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaUShort(RakNet::BitStream * stream, short * currentValue, short lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaInt(RakNet::BitStream * stream, int * currentValue, int lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaUInt(RakNet::BitStream * stream, int * currentValue, int lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaFloat(RakNet::BitStream * stream, float * currentValue, float lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaDouble(RakNet::BitStream * stream, double * currentValue, double lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteDeltaLong(RakNet::BitStream * stream, BBInt64 * currentValue, BBInt64 lastValue) {
	stream->WriteDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaByte(RakNet::BitStream * stream, char * b, char lastValue) {
	stream->WriteCompressedDelta(*b, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaShort(RakNet::BitStream * stream, short * currentValue, short lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaUShort(RakNet::BitStream * stream, short * currentValue, short lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaInt(RakNet::BitStream * stream, int * currentValue, int lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaUInt(RakNet::BitStream * stream, int * currentValue, int lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaFloat(RakNet::BitStream * stream, float * currentValue, float lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaDouble(RakNet::BitStream * stream, double * currentValue, double lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}

void bmx_BitStream_WriteCompressedDeltaLong(RakNet::BitStream * stream, BBInt64 * currentValue, BBInt64 lastValue) {
	stream->WriteCompressedDelta(*currentValue, lastValue);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RSACrypt * bmx_RSACrypt_new() {
	return new RSACrypt();
}

void bmx_RSACrypt_delete(RSACrypt * rsa) {
	delete rsa;
}

bool bmx_RSACrypt_generatePrivateKey(RSACrypt * rsa, uint32_t limbs) {
	return rsa->generatePrivateKey(limbs);
}

uint32_t bmx_RSACrypt_getFactorLimbs(RSACrypt * rsa) {
	return rsa->getFactorLimbs();
}

void bmx_RSACrypt_getPrivateP(RSACrypt * rsa, uint32_t * p) {
	uint32_t pt[RAKNET_RSA_FACTOR_LIMBS/2];
	rsa->getPrivateP(pt);
	memcpy(p, &pt, sizeof(pt));
}

void bmx_RSACrypt_getPrivateQ(RSACrypt * rsa, uint32_t * q) {
	uint32_t qt[RAKNET_RSA_FACTOR_LIMBS/2];
	rsa->getPrivateQ(qt);
	memcpy(q, &qt, sizeof(qt));
}

uint32_t bmx_RSACrypt_getModLimbs(RSACrypt * rsa) {
	return rsa->getModLimbs();
}

void bmx_RSACrypt_getPublicModulus(RSACrypt * rsa, uint32_t * modulus) {
	uint32_t mt[RAKNET_RSA_FACTOR_LIMBS];
	rsa->getPublicModulus(mt);
	memcpy(modulus, &mt, sizeof(mt));
}

uint32_t bmx_RSACrypt_getPublicExponent(RSACrypt * rsa) {
	return rsa->getPublicExponent();
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FileListTransfer * bmx_FileListTransfer_new() {
	return new FileListTransfer();
}

int bmx_FileListTransfer_SetupReceive(FileListTransfer * transfer, FileListTransferCBInterface * handler, bool deleteHandler, MaxSystemAddress * allowedSender) {
	return static_cast<int>(transfer->SetupReceive(handler, deleteHandler, allowedSender->Address()));
}

void bmx_FileListTransfer_Send(FileListTransfer * transfer, FileList * fileList, RakPeerInterface * rakPeer, MaxSystemAddress * recipient,
		int setID, PacketPriority priority, int orderingChannel, bool compressData) {
	transfer->Send(fileList, rakPeer, recipient->Address(), static_cast<unsigned short>(setID), priority, orderingChannel, compressData);
}

void bmx_FileListTransfer_CancelReceive(FileListTransfer * transfer, int setID) {
	transfer->CancelReceive(static_cast<unsigned short>(setID));
}

void bmx_FileListTransfer_RemoveReceiver(FileListTransfer * transfer, MaxSystemAddress * systemAddress) {
	transfer->RemoveReceiver(systemAddress->Address());
}

bool bmx_FileListTransfer_IsHandlerActive(FileListTransfer * transfer, int setID) {
	return transfer->IsHandlerActive(static_cast<unsigned short>(setID));
}

void bmx_FileListTransfer_delete(FileListTransfer * transfer) {
	delete transfer;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MessageFilter * bmx_MessageFilter_new() {
	return new MessageFilter();
}

void bmx_MessageFilter_delete(MessageFilter * filter) {
	delete filter;
}

void bmx_MessageFilter_SetAutoAddNewConnectionsToFilter(MessageFilter * filter, int filterSetID) {
	filter->SetAutoAddNewConnectionsToFilter(filterSetID);
}

void bmx_MessageFilter_SetAllowMessageID(MessageFilter * filter, bool allow, int messageIDStart, int messageIDEnd, int filterSetID) {
	filter->SetAllowMessageID(allow, messageIDStart, messageIDEnd, filterSetID);
}

void bmx_MessageFilter_SetAllowRPC(MessageFilter * filter, bool allow, BBString * functionName, int filterSetID) {
	char * p = bbStringToCString(functionName);
	filter->SetAllowRPC(allow, p, filterSetID);
	bbMemFree(p);
}

void bmx_MessageFilter_SetSystemFilterSet(MessageFilter * filter, MaxSystemAddress * systemAddress, int filterSetID) {
	filter->SetSystemFilterSet(systemAddress->Address(), filterSetID);
}

void bmx_MessageFilter_SetActionOnDisallowedMessage(MessageFilter * filter, bool kickOnDisallowed, bool banOnDisallowed, BBInt64 banTimeMS, int filterSetID) {
	filter->SetActionOnDisallowedMessage(kickOnDisallowed, banOnDisallowed,
#ifdef __GET_TIME_64BIT

	banTimeMS
#else
	static_cast<RakNetTime>(banTimeMS)
#endif
	,filterSetID);
}

void bmx_MessageFilter_SetFilterMaxTime(MessageFilter * filter, int allowedTimeMS, bool banOnExceed, BBInt64 banTimeMS, int filterSetID) {
	filter->SetFilterMaxTime(allowedTimeMS, banOnExceed,
#ifdef __GET_TIME_64BIT

	banTimeMS
#else
	static_cast<RakNetTime>(banTimeMS)
#endif
	,filterSetID);
}

int bmx_MessageFilter_GetSystemFilterSet(MessageFilter * filter, MaxSystemAddress * systemAddress) {
	return filter->GetSystemFilterSet(systemAddress->Address());
}

int bmx_MessageFilter_GetSystemCount(MessageFilter * filter, int filterSetID) {
	return static_cast<int>(filter->GetSystemCount(filterSetID));
}

MaxSystemAddress * bmx_MessageFilter_GetSystemByIndex(MessageFilter * filter, int filterSetID, int index) {
	return new MaxSystemAddress(filter->GetSystemByIndex(filterSetID, static_cast<unsigned>(index)));
}

int bmx_MessageFilter_GetFilterSetCount(MessageFilter * filter) {
	return static_cast<int>(filter->GetFilterSetCount());
}

int bmx_MessageFilter_GetFilterSetIDByIndex(MessageFilter * filter, int index) {
	return filter->GetFilterSetIDByIndex(static_cast<unsigned>(index));
}

void bmx_MessageFilter_DeleteFilterSet(MessageFilter * filter, int filterSetID) {
	filter->DeleteFilterSet(filterSetID);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_RakNetGUID_ToString(RakNetGUID * guid) {
	return bbStringFromCString(guid->ToString());
}

void bmx_RakNetGUID_delete(RakNetGUID * guid) {
	delete guid;
}

RakNetGUID * bmx_RakNetGUID_unassigned() {
	return new RakNetGUID(UNASSIGNED_RAKNET_GUID);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_RakNetStatistics_ToStringLevel(RakNetStatistics * stats, int verbosityLevel) {
	char buffer[4096];
	StatisticsToString(stats, buffer, verbosityLevel);
	return bbStringFromCString(buffer);
}

int bmx_RakNetStatistics_messageSendBuffer(RakNetStatistics * stats, PacketPriority priority) {
	return static_cast<int>(stats->messageSendBuffer[priority]);
}

int bmx_RakNetStatistics_messagesSent(RakNetStatistics * stats, PacketPriority priority) {
	return static_cast<int>(stats->messagesSent[priority]);
}

void bmx_RakNetStatistics_messageDataBitsSent(RakNetStatistics * stats, PacketPriority priority, BBInt64 * v) {
	*v = stats->messageDataBitsSent[priority];
}

void bmx_RakNetStatistics_messageTotalBitsSent(RakNetStatistics * stats, PacketPriority priority, BBInt64 * v) {
	*v = stats->messageTotalBitsSent[priority];
}

int bmx_RakNetStatistics_packetsContainingOnlyAcknowlegements(RakNetStatistics * stats) {
	return static_cast<int>(stats->packetsContainingOnlyAcknowlegements);
}

int bmx_RakNetStatistics_acknowlegementsSent(RakNetStatistics * stats) {
	return static_cast<int>(stats->acknowlegementsSent);
}

int bmx_RakNetStatistics_acknowlegementsPending(RakNetStatistics * stats) {
	return static_cast<int>(stats->acknowlegementsPending);
}

void bmx_RakNetStatistics_acknowlegementBitsSent(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->acknowlegementBitsSent;
}

int bmx_RakNetStatistics_packetsContainingOnlyAcknowlegementsAndResends(RakNetStatistics * stats) {
	return static_cast<int>(stats->packetsContainingOnlyAcknowlegementsAndResends);
}

int bmx_RakNetStatistics_messageResends(RakNetStatistics * stats) {
	return static_cast<int>(stats->messageResends);
}

void bmx_RakNetStatistics_messageDataBitsResent(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->messageDataBitsResent;
}

void bmx_RakNetStatistics_messagesTotalBitsResent(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->messagesTotalBitsResent;
}

int bmx_RakNetStatistics_messagesOnResendQueue(RakNetStatistics * stats) {
	return static_cast<int>(stats->messagesOnResendQueue);
}

int bmx_RakNetStatistics_numberOfUnsplitMessages(RakNetStatistics * stats) {
	return static_cast<int>(stats->numberOfUnsplitMessages);
}

int bmx_RakNetStatistics_numberOfSplitMessages(RakNetStatistics * stats) {
	return static_cast<int>(stats->numberOfSplitMessages);
}

int bmx_RakNetStatistics_totalSplits(RakNetStatistics * stats) {
	return static_cast<int>(stats->totalSplits);
}

int bmx_RakNetStatistics_packetsSent(RakNetStatistics * stats) {
	return static_cast<int>(stats->packetsSent);
}

void bmx_RakNetStatistics_encryptionBitsSent(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->encryptionBitsSent;
}

void bmx_RakNetStatistics_totalBitsSent(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->totalBitsSent;
}

int bmx_RakNetStatistics_sequencedMessagesOutOfOrder(RakNetStatistics * stats) {
	return static_cast<int>(stats->sequencedMessagesOutOfOrder);
}

int bmx_RakNetStatistics_sequencedMessagesInOrder(RakNetStatistics * stats) {
	return static_cast<int>(stats->sequencedMessagesInOrder);
}

int bmx_RakNetStatistics_orderedMessagesOutOfOrder(RakNetStatistics * stats) {
	return static_cast<int>(stats->orderedMessagesOutOfOrder);
}

int bmx_RakNetStatistics_orderedMessagesInOrder(RakNetStatistics * stats) {
	return static_cast<int>(stats->orderedMessagesInOrder);
}

int bmx_RakNetStatistics_packetsReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->packetsReceived);
}

int bmx_RakNetStatistics_packetsWithBadCRCReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->packetsWithBadCRCReceived);
}

void bmx_RakNetStatistics_bitsReceived(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->bitsReceived;
}

void bmx_RakNetStatistics_bitsWithBadCRCReceived(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->bitsWithBadCRCReceived;
}

int bmx_RakNetStatistics_acknowlegementsReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->acknowlegementsReceived);
}

int bmx_RakNetStatistics_duplicateAcknowlegementsReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->duplicateAcknowlegementsReceived);
}

int bmx_RakNetStatistics_messagesReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->messagesReceived);
}

int bmx_RakNetStatistics_invalidMessagesReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->invalidMessagesReceived);
}

int bmx_RakNetStatistics_duplicateMessagesReceived(RakNetStatistics * stats) {
	return static_cast<int>(stats->duplicateMessagesReceived);
}

int bmx_RakNetStatistics_messagesWaitingForReassembly(RakNetStatistics * stats) {
	return static_cast<int>(stats->messagesWaitingForReassembly);
}

int bmx_RakNetStatistics_internalOutputQueueSize(RakNetStatistics * stats) {
	return static_cast<int>(stats->internalOutputQueueSize);
}

double bmx_RakNetStatistics_bitsPerSecondSent(RakNetStatistics * stats) {
	return stats->bitsPerSecondSent;
}

double bmx_RakNetStatistics_bitsPerSecondReceived(RakNetStatistics * stats) {
	return stats->bitsPerSecondReceived;
}

void bmx_RakNetStatistics_connectionStartTime(RakNetStatistics * stats, BBInt64 * v) {
	*v = stats->connectionStartTime;
}

int bmx_RakNetStatistics_bandwidthExceeded(RakNetStatistics * stats) {
	return static_cast<int>(stats->bandwidthExceeded);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FileList * bmx_FileList_new() {
	return new FileList();
}

void bmx_FileList_AddFilesFromDirectory(FileList * list, BBString * applicationDirectory, BBString * subDirectory, bool writeHash, bool writeData, bool recursive, FileListNodeContext * context) {
	char * d = bbStringToCString(applicationDirectory);
	char * s = bbStringToCString(subDirectory);
	list->AddFilesFromDirectory(d, s, writeHash, writeData, recursive, (context)?*context : FileListNodeContext(0,0));
	bbMemFree(d);
	bbMemFree(s);
}

void bmx_FileList_Clear(FileList * list) {
	list->Clear();
}

bool bmx_FileList_Deserialize(FileList * list, RakNet::BitStream * bitStream) {
	return list->Deserialize(bitStream);
}

void bmx_FileList_ListMissingOrChangedFiles(FileList * list, BBString * applicationDirectory, FileList * missingOrChangedFiles, bool alwaysWriteHash, bool neverWriteHash) {
	char * p = bbStringToCString(applicationDirectory);
	list->ListMissingOrChangedFiles(p, missingOrChangedFiles, alwaysWriteHash, neverWriteHash);
	bbMemFree(p);
}

void bmx_FileList_GetDeltaToCurrent(FileList * list, FileList * inputList, FileList * outputList, BBString * dirSubset, BBString * remoteSubdir) {
	char * s = bbStringToCString(dirSubset);
	char * r = bbStringToCString(remoteSubdir);
	list->GetDeltaToCurrent(inputList, outputList, s, r);
	bbMemFree(s);
	bbMemFree(r);
}

void bmx_FileList_PopulateDataFromDisk(FileList * list, BBString * applicationDirectory, bool writeFileData, bool writeFileHash, bool removeUnknownFiles) {
	char * p = bbStringToCString(applicationDirectory);
	list->PopulateDataFromDisk(p, writeFileData, writeFileHash, removeUnknownFiles);
	bbMemFree(p);
}

void bmx_FileList_WriteDataToDisk(FileList * list, BBString * applicationDirectory) {
	char * p = bbStringToCString(applicationDirectory);
	list->WriteDataToDisk(p);
	bbMemFree(p);
}

void bmx_FileList_AddFileMemory(FileList * list,BBString *  filename, BBString * fullpathToFile, const char * data, const unsigned dataLength, const unsigned fileLength, FileListNodeContext * context, bool isAReference) {
	char * f = bbStringToCString(filename);
	char * p = bbStringToCString(fullpathToFile);
	list->AddFile(f, p, data, dataLength, fileLength, (context)?*context : FileListNodeContext(0,0), isAReference);
	bbMemFree(f);
	bbMemFree(p);
}

void bmx_FileList_AddFile(FileList * list, BBString * filepath, BBString * filename, FileListNodeContext * context) {
	char * p = bbStringToCString(filepath);
	char * n = bbStringToCString(filename);
	list->AddFile(p, n, (context)?*context : FileListNodeContext(0,0));
	bbMemFree(p);
	bbMemFree(n);
}

void bmx_FileList_DeleteFiles(FileList * list, BBString * applicationDirectory) {
	char * p = bbStringToCString(applicationDirectory);
	list->DeleteFiles(p);
	bbMemFree(p);
}

void bmx_FileList_delete(FileList * list) {
	delete list;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FileListNodeContext * bmx_FileListNodeContext_new() {
	return new FileListNodeContext();
}

void bmx_FileListNodeContext_delete(FileListNodeContext * context) {
	delete context;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ReadyEvent * bmx_ReadyEvent_new() {
	return new ReadyEvent();
}

bool bmx_ReadyEvent_SetEvent(ReadyEvent * readyEvent, int eventId, bool isReady) {
	return readyEvent->SetEvent(eventId, isReady);
}

bool bmx_ReadyEvent_DeleteEvent(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->DeleteEvent(eventId);
}

bool bmx_ReadyEvent_IsEventSet(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->IsEventSet(eventId);
}

bool bmx_ReadyEvent_IsEventCompletionProcessing(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->IsEventCompletionProcessing(eventId);
}

bool bmx_ReadyEvent_IsEventCompleted(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->IsEventCompleted(eventId);
}

bool bmx_ReadyEvent_HasEvent(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->HasEvent(eventId);
}

int bmx_ReadyEvent_GetEventListSize(ReadyEvent * readyEvent) {
	return static_cast<int>(readyEvent->GetEventListSize());
}

int bmx_ReadyEvent_GetEventAtIndex(ReadyEvent * readyEvent, int index) {
	return readyEvent->GetEventAtIndex(static_cast<unsigned>(index));
}

bool bmx_ReadyEvent_AddToWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address) {
	return readyEvent->AddToWaitList(eventId, address->Address());
}

bool bmx_ReadyEvent_RemoveFromWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address) {
	return readyEvent->RemoveFromWaitList(eventId, address->Address());
}

bool bmx_ReadyEvent_IsInWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address) {
	return readyEvent->IsInWaitList(eventId, address->Address());
}

int bmx_ReadyEvent_GetRemoteWaitListSize(ReadyEvent * readyEvent, int eventId) {
	return static_cast<int>(readyEvent->GetRemoteWaitListSize(eventId));
}

MaxSystemAddress * bmx_ReadyEvent_GetFromWaitListAtIndex(ReadyEvent * readyEvent, int eventId, int index) {
	return new MaxSystemAddress(readyEvent->GetFromWaitListAtIndex(eventId, static_cast<unsigned>(index)));
}

ReadyEventSystemStatus bmx_ReadyEvent_GetReadyStatus(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address) {
	return readyEvent->GetReadyStatus(eventId, address->Address());
}

void bmx_ReadyEvent_SetSendChannel(ReadyEvent * readyEvent, int newChannel) {
	readyEvent->SetSendChannel(static_cast< unsigned char >(newChannel));
}

void bmx_ReadyEvent_delete(ReadyEvent * readyEvent) {
	delete readyEvent;
}

bool bmx_ReadyEvent_ForceCompletion(ReadyEvent * readyEvent, int eventId) {
	return readyEvent->ForceCompletion(eventId);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NetworkIDManager * bmx_NetworkIDManager_create() {
	return new NetworkIDManager;
}

void bmk_NetworkIDManager_SetIsNetworkIDAuthority(NetworkIDManager * manager, int isAuthority) {
	manager->SetIsNetworkIDAuthority(static_cast<bool>(isAuthority));
}

int bmk_NetworkIDManager_IsNetworkIDAuthority(NetworkIDManager * manager) {
	return static_cast<int>(manager->IsNetworkIDAuthority());
}

void bmk_NetworkIDManager_SetGuid(NetworkIDManager * manager, RakNetGUID * guid) {
	manager->SetGuid(*guid);
}

int bmk_NetworkIDManager_GetSharedNetworkID(NetworkIDManager * manager) {
	return static_cast<int>(manager->GetSharedNetworkID());
}

void bmk_NetworkIDManager_SetSharedNetworkID(NetworkIDManager * manager, int id) {
	manager->SetSharedNetworkID(static_cast<unsigned short>(id));
}



