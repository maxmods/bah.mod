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

#include "assert.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RSACrypt.h"
#include "Rand.h"
#include "GetTime.h"
#include "FileListTransfer.h"
#include "MessageFilter.h"
#include "RakNetStatistics.h"
#include "ReadyEvent.h"
#include "NetworkIDManager.h"

class MaxSocketDescriptor;
class MaxNetworkID;
class MaxSystemAddress;

extern "C" {

#include "blitz.h"

	RakPeerInterface * bmx_RakNetworkFactory_GetRakPeerInterface();
	ConsoleServer * bmx_RakNetworkFactory_GetConsoleServer();
	ReplicaManager * bmx_RakNetworkFactory_GetReplicaManager();
	LogCommandParser * bmx_RakNetworkFactory_GetLogCommandParser();
	PacketLogger * bmx_RakNetworkFactory_GetPacketLogger();
	RakNetCommandParser * bmx_RakNetworkFactory_GetRakNetCommandParser();
	RakNetTransport * bmx_RakNetworkFactory_GetRakNetTransport();
	TelnetTransport * bmx_RakNetworkFactory_GetTelnetTransport();
	PacketConsoleLogger * bmx_RakNetworkFactory_GetPacketConsoleLogger();
	PacketFileLogger * bmx_RakNetworkFactory_GetPacketFileLogger();
	Router * bmx_RakNetworkFactory_GetRouter();
	ConnectionGraph * bmx_RakNetworkFactory_GetConnectionGraph();
	void bmx_RakNetworkFactory_DestroyRakPeerInterface(RakPeerInterface * peer);

	MaxSocketDescriptor * bmx_SocketDescriptor_new(int port, BBString * hostAddress);
	void bmx_SocketDescriptor_delete(MaxSocketDescriptor * desc);
	void bmx_SocketDescriptor_setport(MaxSocketDescriptor * desc, int port);
	int bmx_SocketDescriptor_getport(MaxSocketDescriptor * desc);
	void bmx_SocketDescriptor_sethostaddress(MaxSocketDescriptor * desc, BBString * hostAddress);
	BBString * bmx_SocketDescriptor_gethostaddress(MaxSocketDescriptor * desc);

	bool bmx_RakPeer_Startup(RakPeerInterface * peer, int maxConnections, int threadSleepTimer, MaxSocketDescriptor * descriptor);
	Packet * bmx_RakPeer_Receive(RakPeerInterface * peer);
	void bmx_RakPeer_DeallocatePacket(RakPeerInterface * peer, Packet * packet);
	void bmx_RakPeer_SetMaximumIncomingConnections(RakPeerInterface * peer, int numberAllowed);
	bool bmx_RakPeer_Connect(RakPeerInterface * peer, BBString * host, int remotePort, BBString * passwordData);
	void bmx_RakPeer_RegisterAsRemoteProcedureCall(RakPeerInterface * peer, BBString * id, void(*)(RPCParameters *rpcParms));
	void bmx_RakPeer_DisableSecurity(RakPeerInterface * peer);
	int bmx_RakPeer_GetMaximumIncomingConnections(RakPeerInterface * peer);
	int bmx_RakPeer_NumberOfConnections(RakPeerInterface * peer);
	void bmx_RakPeer_SetIncomingPassword(RakPeerInterface * peer, BBString * passwordData);
	BBString * bmx_RakPeer_GetIncomingPassword(RakPeerInterface * peer);
	void bmx_RakPeer_Shutdown(RakPeerInterface * peer, int blockDuration, int orderingChannel);
	bool bmx_RakPeer_IsActive(RakPeerInterface * peer);
	void bmx_RakPeer_UnregisterAsRemoteProcedureCall(RakPeerInterface * peer, BBString * name);
	bool bmx_RakPeer_RPC(RakPeerInterface * peer, const char * id, const char * data, unsigned int bitLength, 
		PacketPriority priority, PacketReliability reliability, int 		orderingChannel,
		MaxSystemAddress * addr, bool broadcast, BBInt64 * includedTimestamp, 
		RakNet::BitStream * replyFromTarget);
	void bmx_RakPeer_AddToSecurityExceptionList(RakPeerInterface * peer, BBString * ip);
	void bmx_RakPeer_RemoveFromSecurityExceptionList(RakPeerInterface * peer, BBString * ip);
	bool bmx_RakPeer_IsInSecurityExceptionList(RakPeerInterface * peer, BBString * ip);
	int bmx_RakPeer_GetMaximumNumberOfPeers(RakPeerInterface * peer);
	void bmx_RakPeer_InitializeSecurity(RakPeerInterface * peer, uint32_t * pubKeyE, const char * pubKeyN,
		const char * privKeyP, const char * privKeyQ);
	bool bmx_RakPeer_SendBitStream(RakPeerInterface * peer, RakNet::BitStream * stream, PacketPriority priority, 
		PacketReliability reliability, int orderingChannel, MaxSystemAddress * addr, bool broadcast);
	void bmx_RakPeer_SetOccasionalPing(RakPeerInterface * peer, bool doPing);
	MaxSystemAddress * bmx_RakPeer_GetSystemAddressFromIndex(RakPeerInterface * peer, int index);
	void bmx_RakPeer_CloseConnection(RakPeerInterface * peer, MaxSystemAddress * target, bool sendDisconnectionNotification, bool orderingChannel);
	bool bmx_RakPeer_IsConnected(RakPeerInterface * peer, MaxSystemAddress * addr, bool includeInProgress, bool includeDisconnecting);
	int bmx_RakPeer_GetIndexFromSystemAddress(RakPeerInterface * peer, MaxSystemAddress * addr);
	void bmx_RakPeer_AddToBanList(RakPeerInterface * peer, BBString * ip, BBInt64 milliseconds);
	void bmx_RakPeer_RemoveFromBanList(RakPeerInterface * peer, BBString * ip);
	void bmx_RakPeer_ClearBanList(RakPeerInterface * peer);
	bool bmx_RakPeer_IsBanned(RakPeerInterface * peer, BBString * ip);
	void bmx_RakPeer_Ping(RakPeerInterface * peer, MaxSystemAddress * addr);
	int bmx_RakPeer_GetAveragePing(RakPeerInterface * peer, MaxSystemAddress * addr);
	int bmx_RakPeer_GetLastPing(RakPeerInterface * peer, MaxSystemAddress * addr);
	int bmx_RakPeer_GetLowestPing(RakPeerInterface * peer, MaxSystemAddress * addr);
	void bmx_RakPeer_SetOfflinePingResponse(RakPeerInterface * peer, BBString * data);
	BBString * bmx_RakPeer_GetOfflinePingResponse(RakPeerInterface * peer);
	void bmx_RakPeer_SetCompileFrequencyTable(RakPeerInterface * peer, bool doCompile);
	void bmx_RakPeer_AttachPlugin(RakPeerInterface * peer, PluginInterface2 * 	plugin	);
	void bmx_RakPeer_DetachPlugin(RakPeerInterface * peer, PluginInterface2 * 	plugin	);
	bool bmx_RakPeer_Send(RakPeerInterface * peer, const char * data, const int length, PacketPriority priority, 
		PacketReliability reliability, int orderingChannel, MaxSystemAddress * addr, bool broadcast);
	BBString * bmx_RakPeer_GetLocalIP(RakPeerInterface * peer, int index);
	bool bmx_RakPeer_IsLocalIP(RakPeerInterface * peer, BBString * ip);
	RakNetGUID * bmx_RakPeer_GetGuidFromSystemAddress(RakPeerInterface * peer, MaxSystemAddress * systemAddress);
	RakNetStatistics * bmx_RakPeer_GetStatistics(RakPeerInterface * peer, MaxSystemAddress * systemAddress);
	MaxSystemAddress * bmx_RakPeer_GetInternalID(RakPeerInterface * peer, MaxSystemAddress * systemAddress);
	MaxSystemAddress * bmx_RakPeer_GetExternalID(RakPeerInterface * peer, MaxSystemAddress * systemAddress);
	void bmx_RakPeer_SetTimeoutTime(RakPeerInterface * peer, BBInt64 timeMS, MaxSystemAddress * target);
	bool bmx_RakPeer_SetMTUSize(RakPeerInterface * peer, int size);
	int bmx_RakPeer_GetMTUSize(RakPeerInterface * peer, MaxSystemAddress * target);
	int  bmx_RakPeer_GetNumberOfAddresses(RakPeerInterface * peer);
	bool bmx_RakPeer_DeleteCompressionLayer(RakPeerInterface * peer, bool inputLayer);
	float bmx_RakPeer_GetCompressionRatio(RakPeerInterface * peer);
	float bmx_RakPeer_GetDecompressionRatio(RakPeerInterface * peer);
	void bmx_RakPeer_ApplyNetworkSimulator(RakPeerInterface * peer, double maxSendBPS, int minExtraPing, int extraPingVariance);
	void bmx_RakPeer_SetPerConnectionOutgoingBandwidthLimit(RakPeerInterface * peer, int maxBitsPerSecond);
	bool bmx_RakPeer_IsNetworkSimulatorActive(RakPeerInterface * peer);
	void bmx_RakPeer_AllowConnectionResponseIPMigration(RakPeerInterface * peer, bool allow);
	bool bmx_RakPeer_AdvertiseSystem(RakPeerInterface * peer, BBString * host, int remotePort, const char * data, int dataLength, int connectionSocketIndex);
	void bmx_RakPeer_SetSplitMessageProgressInterval(RakPeerInterface * peer, int interval);
	void bmx_RakPeer_SetUnreliableTimeout(RakPeerInterface * peer, BBInt64 timeoutMS);
	void bmx_RakPeer_SendTTL(RakPeerInterface * peer, BBString * host, int remotePort, int ttl, int connectionSocketIndex);
	void bmx_RakPeer_PushBackPacket(RakPeerInterface * peer, Packet * packet, bool pushAtHead);
	Packet * bmx_RakPeer_AllocatePacket(RakPeerInterface * peer, int dataSize);
	void bmx_RakPeer_PingHost(RakPeerInterface * peer, BBString * host, int remotePort, bool onlyReplyOnAcceptingConnections, int connectionSocketIndex);
	void bmx_RakPeer_SetNetworkIDManager(RakPeerInterface * peer, NetworkIDManager * manager);
	NetworkIDManager * bmx_RakPeer_GetNetworkIDManager(RakPeerInterface * peer);

	unsigned char * bmx_Packet_GetData(Packet * packet);
	unsigned int bmx_Packet_GetBitSize(Packet * packet);
	MaxSystemAddress * bmx_Packet_GetSystemAddress(Packet * packet);
	int bmx_Packet_GetPacketIdentifier(Packet * packet);
	RakNetGUID * bmx_Packet_GetGuid(Packet * packet);


	BBString * bmx_RPCParameters_GetFunctionName(RPCParameters * params);
	unsigned char * bmx_RPCParameters_GetInput(RPCParameters * params);
	unsigned int bmx_RPCParameters_GetNumberOfBitsOfData(RPCParameters * params);
	MaxSystemAddress * bmx_RPCParameters_GetSender(RPCParameters * params);
	RakPeerInterface * bmx_RPCParameters_GetRecipient(RPCParameters * params);
	RakNet::BitStream * bmx_RPCParameters_GetReplyToSender(RPCParameters * params);

	void bmx_NetworkID_delete(MaxNetworkID * networkId);
	MaxNetworkID * bmx_NetworkID_unassigned();

	void bmx_SystemAddress_delete(MaxSystemAddress * address);
	MaxSystemAddress * bmx_SystemAddress_unassigned();
	bool bmx_SystemAddress_Equals(MaxSystemAddress * address, MaxSystemAddress * other);
	BBString * bmx_SystemAddress_ToString(MaxSystemAddress * address);
	int bmx_SystemAddress_GetBinaryAddress(MaxSystemAddress * address);
	int bmx_SystemAddress_GetPort(MaxSystemAddress * address);
	MaxSystemAddress * bmx_SystemAddress_create();
	void bmx_SystemAddress_SetBinaryAddress(MaxSystemAddress * address, BBString * addr);
	void bmx_SystemAddress_SetPort(MaxSystemAddress * address, int port);

	RakNet::BitStream * bmx_BitStream_Create();
	RakNet::BitStream * bmx_BitStream_CreateFromData(unsigned char * data, unsigned int size, bool copy);
	void bmx_BitStream_Delete(RakNet::BitStream * stream);
	void bmx_BitStream_Reset(RakNet::BitStream * stream);
	bool bmx_BitStream_SerializeByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value);
	bool bmx_BitStream_SerializeShort(RakNet::BitStream * stream, bool writeToBitstream, short * value);
	bool bmx_BitStream_SerializeInt(RakNet::BitStream * stream, bool writeToBitstream, int * value);
	bool bmx_BitStream_SerializeUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value);
	bool bmx_BitStream_SerializeUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value);
	bool bmx_BitStream_SerializeFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value);
	bool bmx_BitStream_SerializeDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value);
	bool bmx_BitStream_SerializeDeltaLastByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue, unsigned char lastValue);
	bool bmx_BitStream_SerializeDeltaLastShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue, short lastValue);
	bool bmx_BitStream_SerializeDeltaLastInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue, int lastValue);
	bool bmx_BitStream_SerializeDeltaLastUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue, unsigned short lastValue);
	bool bmx_BitStream_SerializeDeltaLastUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue, unsigned int lastValue);
	bool bmx_BitStream_SerializeDeltaLastFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue, float lastValue);
	bool bmx_BitStream_SerializeDeltaLastDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue, double lastValue);
	bool bmx_BitStream_SerializeDeltaByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value);
	bool bmx_BitStream_SerializeDeltaShort(RakNet::BitStream * stream, bool writeToBitstream, short * value);
	bool bmx_BitStream_SerializeDeltaInt(RakNet::BitStream * stream, bool writeToBitstream, int * value);
	bool bmx_BitStream_SerializeDeltaUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value);
	bool bmx_BitStream_SerializeDeltaUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value);
	bool bmx_BitStream_SerializeDeltaFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value);
	bool bmx_BitStream_SerializeDeltaDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value);

	bool bmx_BitStream_SerializeCompressedByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * value);
	bool bmx_BitStream_SerializeCompressedShort(RakNet::BitStream * stream, bool writeToBitstream, short * value);
	bool bmx_BitStream_SerializeCompressedInt(RakNet::BitStream * stream, bool writeToBitstream, int * value);
	bool bmx_BitStream_SerializeCompressedUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * value);
	bool bmx_BitStream_SerializeCompressedUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * value);
	bool bmx_BitStream_SerializeCompressedFloat(RakNet::BitStream * stream, bool writeToBitstream, float * value);
	bool bmx_BitStream_SerializeCompressedDouble(RakNet::BitStream * stream, bool writeToBitstream, double * value);
	bool bmx_BitStream_SerializeCompressedDeltaLastByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue, unsigned char lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue, short lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue, int lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue, unsigned short lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue, unsigned int lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue, float lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaLastDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue, double lastValue);
	bool bmx_BitStream_SerializeCompressedDeltaByte(RakNet::BitStream * stream, bool writeToBitstream, unsigned char * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaShort(RakNet::BitStream * stream, bool writeToBitstream, short * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaInt(RakNet::BitStream * stream, bool writeToBitstream, int * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaUShort(RakNet::BitStream * stream, bool writeToBitstream, unsigned short * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaUInt(RakNet::BitStream * stream, bool writeToBitstream, unsigned int * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaFloat(RakNet::BitStream * stream, bool writeToBitstream, float * currentValue);
	bool bmx_BitStream_SerializeCompressedDeltaDouble(RakNet::BitStream * stream, bool writeToBitstream, double * currentValue);

	bool bmx_BitStream_ReadBit(RakNet::BitStream * stream);
	bool bmx_BitStream_ReadByte(RakNet::BitStream * stream, unsigned char * value);
	bool bmx_BitStream_ReadShort(RakNet::BitStream * stream, short * value);
	bool bmx_BitStream_ReadInt(RakNet::BitStream * stream, int * value);
	bool bmx_BitStream_ReadUShort(RakNet::BitStream * stream, unsigned short * value);
	bool bmx_BitStream_ReadUInt(RakNet::BitStream * stream, unsigned int * value);
	bool bmx_BitStream_ReadFloat(RakNet::BitStream * stream, float * value);
	bool bmx_BitStream_ReadDouble(RakNet::BitStream * stream, double * value);
	bool bmx_BitStream_ReadLong(RakNet::BitStream * stream, BBInt64 * value);
	bool bmx_BitStream_ReadTime(RakNet::BitStream * stream, BBInt64 * value);
	BBString * bmx_BitStream_ReadString(RakNet::BitStream * stream, int length);

	bool bmx_BitStream_ReadDeltaByte(RakNet::BitStream * stream, unsigned char * value);
	bool bmx_BitStream_ReadDeltaShort(RakNet::BitStream * stream, short * value);
	bool bmx_BitStream_ReadDeltaInt(RakNet::BitStream * stream, int * value);
	bool bmx_BitStream_ReadDeltaUShort(RakNet::BitStream * stream, unsigned short * value);
	bool bmx_BitStream_ReadDeltaUInt(RakNet::BitStream * stream, unsigned int * value);
	bool bmx_BitStream_ReadDeltaFloat(RakNet::BitStream * stream, float * value);
	bool bmx_BitStream_ReadDeltaDouble(RakNet::BitStream * stream, double * value);
	bool bmx_BitStream_ReadCompressedByte(RakNet::BitStream * stream, unsigned char * value);
	bool bmx_BitStream_ReadCompressedShort(RakNet::BitStream * stream, short * value);
	bool bmx_BitStream_ReadCompressedInt(RakNet::BitStream * stream, int * value);
	bool bmx_BitStream_ReadCompressedUShort(RakNet::BitStream * stream, unsigned short * value);
	bool bmx_BitStream_ReadCompressedUInt(RakNet::BitStream * stream, unsigned int * value);
	bool bmx_BitStream_ReadCompressedFloat(RakNet::BitStream * stream, float * value);
	bool bmx_BitStream_ReadCompressedDouble(RakNet::BitStream * stream, double * value);
	bool bmx_BitStream_ReadCompressedDeltaByte(RakNet::BitStream * stream, unsigned char * value);
	bool bmx_BitStream_ReadCompressedDeltaShort(RakNet::BitStream * stream, short * value);
	bool bmx_BitStream_ReadCompressedDeltaInt(RakNet::BitStream * stream, int * value);
	bool bmx_BitStream_ReadCompressedDeltaUShort(RakNet::BitStream * stream, unsigned short * value);
	bool bmx_BitStream_ReadCompressedDeltaUInt(RakNet::BitStream * stream, unsigned int * value);
	bool bmx_BitStream_ReadCompressedDeltaFloat(RakNet::BitStream * stream, float * value);
	bool bmx_BitStream_ReadCompressedDeltaDouble(RakNet::BitStream * stream, double * value);

	bool bmx_BitStream_ReadDeltaBool(RakNet::BitStream * stream, bool * value);
	bool bmx_BitStream_ReadCompressedBool(RakNet::BitStream * stream, bool * value);
	bool bmx_BitStream_ReadCompressedDeltaBool(RakNet::BitStream * stream, bool * value);

	void bmx_BitStream_WriteByte(RakNet::BitStream * stream, unsigned char * value);
	void bmx_BitStream_WriteShort(RakNet::BitStream * stream, short * value);
	void bmx_BitStream_WriteInt(RakNet::BitStream * stream, int * value);
	void bmx_BitStream_WriteFloat(RakNet::BitStream * stream, float * value);
	void bmx_BitStream_WriteDouble(RakNet::BitStream * stream, double * value);
	void bmx_BitStream_WriteLong(RakNet::BitStream * stream, BBInt64 * value);
	void bmx_BitStream_WriteTime(RakNet::BitStream * stream, BBInt64 * value);
	void bmx_BitStream_AssertStreamEmpty(RakNet::BitStream * stream);
	void bmx_BitStream_IgnoreBits(RakNet::BitStream * stream, int numberOfBits);
	void bmx_BitStream_IgnoreBytes(RakNet::BitStream * stream, int numberOfBytes);
	void bmx_BitStream_SetWriteOffset(RakNet::BitStream * stream, int offset);
	void bmx_BitStream_Write0(RakNet::BitStream * stream);
	void bmx_BitStream_Write1(RakNet::BitStream * stream);
	void bmx_BitStream_WriteUShort(RakNet::BitStream * stream, unsigned short * value);
	void bmx_BitStream_WriteUInt(RakNet::BitStream * stream, unsigned int * value);

	int bmx_BitStream_GetNumberOfBitsUsed(RakNet::BitStream * stream);
	int bmx_BitStream_GetWriteOffset(RakNet::BitStream * stream);
	int bmx_BitStream_GetNumberOfBytesUsed(RakNet::BitStream * stream);
	int bmx_BitStream_GetReadOffset(RakNet::BitStream * stream);
	void bmx_BitStream_SetReadOffset(RakNet::BitStream * stream, int offset);
	int bmx_BitStream_GetNumberOfUnreadBits(RakNet::BitStream * stream);
	void bmx_BitStream_WriteBits(RakNet::BitStream * stream, const unsigned char * data, int numberOfBitsToWrite, int rightAlignedBits);
	void bmx_BitStream_WriteCompressedByte(RakNet::BitStream * stream, char * b);
	void bmx_BitStream_WriteCompressedShort(RakNet::BitStream * stream, short * value);
	void bmx_BitStream_WriteCompressedUShort(RakNet::BitStream * stream, short * value);
	void bmx_BitStream_WriteCompressedInt(RakNet::BitStream * stream, int * value);
	void bmx_BitStream_WriteCompressedUInt(RakNet::BitStream * stream, int * value);
	void bmx_BitStream_WriteCompressedFloat(RakNet::BitStream * stream, float * value);
	void bmx_BitStream_WriteCompressedDouble(RakNet::BitStream * stream, double * value);
	void bmx_BitStream_WriteCompressedLong(RakNet::BitStream * stream, BBInt64 * value);
	void bmx_BitStream_WriteDeltaByte(RakNet::BitStream * stream, char * b, char lastValue);
	void bmx_BitStream_WriteDeltaShort(RakNet::BitStream * stream, short * currentValue, short lastValue);
	void bmx_BitStream_WriteDeltaUShort(RakNet::BitStream * stream, short * currentValue, short lastValue);
	void bmx_BitStream_WriteDeltaInt(RakNet::BitStream * stream, int * currentValue, int lastValue);
	void bmx_BitStream_WriteDeltaUInt(RakNet::BitStream * stream, int * currentValue, int lastValue);
	void bmx_BitStream_WriteDeltaFloat(RakNet::BitStream * stream, float * currentValue, float lastValue);
	void bmx_BitStream_WriteDeltaDouble(RakNet::BitStream * stream, double * currentValue, double lastValue);
	void bmx_BitStream_WriteDeltaLong(RakNet::BitStream * stream, BBInt64 * currentValue, BBInt64 lastValue);
	void bmx_BitStream_WriteCompressedDeltaByte(RakNet::BitStream * stream, char * b, char lastValue);
	void bmx_BitStream_WriteCompressedDeltaShort(RakNet::BitStream * stream, short * currentValue, short lastValue);
	void bmx_BitStream_WriteCompressedDeltaUShort(RakNet::BitStream * stream, short * currentValue, short lastValue);
	void bmx_BitStream_WriteCompressedDeltaInt(RakNet::BitStream * stream, int * currentValue, int lastValue);
	void bmx_BitStream_WriteCompressedDeltaUInt(RakNet::BitStream * stream, int * currentValue, int lastValue);
	void bmx_BitStream_WriteCompressedDeltaFloat(RakNet::BitStream * stream, float * currentValue, float lastValue);
	void bmx_BitStream_WriteCompressedDeltaDouble(RakNet::BitStream * stream, double * currentValue, double lastValue);
	void bmx_BitStream_WriteCompressedDeltaLong(RakNet::BitStream * stream, BBInt64 * currentValue, BBInt64 lastValue);

	
	RSACrypt * bmx_RSACrypt_new();
	void bmx_RSACrypt_delete(RSACrypt * rsa);
	bool bmx_RSACrypt_generatePrivateKey(RSACrypt * rsa, uint32_t limbs);
	uint32_t bmx_RSACrypt_getFactorLimbs(RSACrypt * rsa);
	void bmx_RSACrypt_getPrivateP(RSACrypt * rsa, uint32_t * p);
	void bmx_RSACrypt_getPrivateQ(RSACrypt * rsa, uint32_t * q);
	uint32_t bmx_RSACrypt_getModLimbs(RSACrypt * rsa);
	void bmx_RSACrypt_getPublicModulus(RSACrypt * rsa, uint32_t * modulus);
	uint32_t bmx_RSACrypt_getPublicExponent(RSACrypt * rsa);

	void bmx_raknet_seedMT(unsigned int seed);
	void bmx_raknet_gettime(BBInt64 * v);
	void bmx_raknet_gettimens(RakNetTimeUS * v);
	unsigned int bmx_raknet_randomMT();
	float bmx_raknet_frandomMT();
	void bmx_raknet_fillBufferMT(void * buffer, unsigned int size);

	FileListTransfer * bmx_FileListTransfer_new();
	int bmx_FileListTransfer_SetupReceive(FileListTransfer * transfer, FileListTransferCBInterface * handler, bool deleteHandler, MaxSystemAddress * allowedSender);
	void bmx_FileListTransfer_Send(FileListTransfer * transfer, FileList * fileList, RakPeerInterface * rakPeer, MaxSystemAddress * recipient,
		int setID, PacketPriority priority, int orderingChannel, bool compressData);
	void bmx_FileListTransfer_CancelReceive(FileListTransfer * transfer, int setID);
	void bmx_FileListTransfer_RemoveReceiver(FileListTransfer * transfer, MaxSystemAddress * systemAddress);
	bool bmx_FileListTransfer_IsHandlerActive(FileListTransfer * transfer, int setID);
	void bmx_FileListTransfer_delete(FileListTransfer * transfer);

	MessageFilter * bmx_MessageFilter_new();
	void bmx_MessageFilter_delete(MessageFilter * filter);
	void bmx_MessageFilter_SetAutoAddNewConnectionsToFilter(MessageFilter * filter, int filterSetID);
	void bmx_MessageFilter_SetAllowMessageID(MessageFilter * filter, bool allow, int messageIDStart, int messageIDEnd, int filterSetID);
	void bmx_MessageFilter_SetAllowRPC(MessageFilter * filter, bool allow, BBString * functionName, int filterSetID);
	void bmx_MessageFilter_SetSystemFilterSet(MessageFilter * filter, MaxSystemAddress * systemAddress, int filterSetID);
	void bmx_MessageFilter_SetActionOnDisallowedMessage(MessageFilter * filter, bool kickOnDisallowed, bool banOnDisallowed, BBInt64 banTimeMS, int filterSetID);
	void bmx_MessageFilter_SetFilterMaxTime(MessageFilter * filter, int allowedTimeMS, bool banOnExceed, BBInt64 banTimeMS, int filterSetID);
	int bmx_MessageFilter_GetSystemFilterSet(MessageFilter * filter, MaxSystemAddress * systemAddress);
	int bmx_MessageFilter_GetSystemCount(MessageFilter * filter, int filterSetID);
	MaxSystemAddress * bmx_MessageFilter_GetSystemByIndex(MessageFilter * filter, int filterSetID, int index);
	int bmx_MessageFilter_GetFilterSetCount(MessageFilter * filter);
	int bmx_MessageFilter_GetFilterSetIDByIndex(MessageFilter * filter, int index);
	void bmx_MessageFilter_DeleteFilterSet(MessageFilter * filter, int filterSetID);

	BBString * bmx_RakNetGUID_ToString(RakNetGUID * guid);
	void bmx_RakNetGUID_delete(RakNetGUID * guid);
	RakNetGUID * bmx_RakNetGUID_unassigned();

	BBString * bmx_RakNetStatistics_ToStringLevel(RakNetStatistics * stats, int verbosityLevel);
	int bmx_RakNetStatistics_messageSendBuffer(RakNetStatistics * stats, PacketPriority priority);
	int bmx_RakNetStatistics_messagesSent(RakNetStatistics * stats, PacketPriority priority);
	void bmx_RakNetStatistics_messageDataBitsSent(RakNetStatistics * stats, PacketPriority priority, BBInt64 * v);
	void bmx_RakNetStatistics_messageTotalBitsSent(RakNetStatistics * stats, PacketPriority priority, BBInt64 * v);
	int bmx_RakNetStatistics_packetsContainingOnlyAcknowlegements(RakNetStatistics * stats);
	int bmx_RakNetStatistics_acknowlegementsSent(RakNetStatistics * stats);
	int bmx_RakNetStatistics_acknowlegementsPending(RakNetStatistics * stats);
	void bmx_RakNetStatistics_acknowlegementBitsSent(RakNetStatistics * stats, BBInt64 * v);
	int bmx_RakNetStatistics_packetsContainingOnlyAcknowlegementsAndResends(RakNetStatistics * stats);
	int bmx_RakNetStatistics_messageResends(RakNetStatistics * stats);
	void bmx_RakNetStatistics_messageDataBitsResent(RakNetStatistics * stats, BBInt64 * v);
	void bmx_RakNetStatistics_messagesTotalBitsResent(RakNetStatistics * stats, BBInt64 * v);
	int bmx_RakNetStatistics_messagesOnResendQueue(RakNetStatistics * stats);
	int bmx_RakNetStatistics_numberOfUnsplitMessages(RakNetStatistics * stats);
	int bmx_RakNetStatistics_numberOfSplitMessages(RakNetStatistics * stats);
	int bmx_RakNetStatistics_totalSplits(RakNetStatistics * stats);
	int bmx_RakNetStatistics_packetsSent(RakNetStatistics * stats);
	void bmx_RakNetStatistics_encryptionBitsSent(RakNetStatistics * stats, BBInt64 * v);
	void bmx_RakNetStatistics_totalBitsSent(RakNetStatistics * stats, BBInt64 * v);
	int bmx_RakNetStatistics_sequencedMessagesOutOfOrder(RakNetStatistics * stats);
	int bmx_RakNetStatistics_sequencedMessagesInOrder(RakNetStatistics * stats);
	int bmx_RakNetStatistics_orderedMessagesOutOfOrder(RakNetStatistics * stats);
	int bmx_RakNetStatistics_orderedMessagesInOrder(RakNetStatistics * stats);
	int bmx_RakNetStatistics_packetsReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_packetsWithBadCRCReceived(RakNetStatistics * stats);
	void bmx_RakNetStatistics_bitsReceived(RakNetStatistics * stats, BBInt64 * v);
	void bmx_RakNetStatistics_bitsWithBadCRCReceived(RakNetStatistics * stats, BBInt64 * v);
	int bmx_RakNetStatistics_acknowlegementsReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_duplicateAcknowlegementsReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_messagesReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_invalidMessagesReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_duplicateMessagesReceived(RakNetStatistics * stats);
	int bmx_RakNetStatistics_messagesWaitingForReassembly(RakNetStatistics * stats);
	int bmx_RakNetStatistics_internalOutputQueueSize(RakNetStatistics * stats);
	double bmx_RakNetStatistics_bitsPerSecondSent(RakNetStatistics * stats);
	double bmx_RakNetStatistics_bitsPerSecondReceived(RakNetStatistics * stats);
	void bmx_RakNetStatistics_connectionStartTime(RakNetStatistics * stats, BBInt64 * v);
	int bmx_RakNetStatistics_bandwidthExceeded(RakNetStatistics * stats);

	int bmx_raknet_SizeOfRakNetTime();

	FileList * bmx_FileList_new();
	void bmx_FileList_AddFilesFromDirectory(FileList * list, BBString * applicationDirectory, BBString * subDirectory, bool writeHash, bool writeData, bool recursive, FileListNodeContext * context);
	void bmx_FileList_Clear(FileList * list);
	bool bmx_FileList_Deserialize(FileList * list, RakNet::BitStream * bitStream);
	void bmx_FileList_ListMissingOrChangedFiles(FileList * list, BBString * applicationDirectory, FileList * missingOrChangedFiles, bool alwaysWriteHash, bool neverWriteHash);
	void bmx_FileList_GetDeltaToCurrent(FileList * list, FileList * inputList, FileList * outputList, BBString * dirSubset, BBString * remoteSubdir);
	void bmx_FileList_PopulateDataFromDisk(FileList * list, BBString * applicationDirectory, bool writeFileData, bool writeFileHash, bool removeUnknownFiles);
	void bmx_FileList_WriteDataToDisk(FileList * list, BBString * applicationDirectory);
	void bmx_FileList_AddFileMemory(FileList * list,BBString *  filename, BBString * fullpathToFile, const char * data, const unsigned dataLength, const unsigned fileLength, FileListNodeContext * context, bool isAReference);
	void bmx_FileList_AddFile(FileList * list, BBString * filepath, BBString * filename, FileListNodeContext * context);
	void bmx_FileList_DeleteFiles(FileList * list, BBString * applicationDirectory);
	void bmx_FileList_delete(FileList * list);

	FileListNodeContext * bmx_FileListNodeContext_new();
	void bmx_FileListNodeContext_delete(FileListNodeContext * context);

	ReadyEvent * bmx_ReadyEvent_new();
	bool bmx_ReadyEvent_SetEvent(ReadyEvent * readyEvent, int eventId, bool isReady);
	bool bmx_ReadyEvent_DeleteEvent(ReadyEvent * readyEvent, int eventId);
	bool bmx_ReadyEvent_IsEventSet(ReadyEvent * readyEvent, int eventId);
	bool bmx_ReadyEvent_IsEventCompletionProcessing(ReadyEvent * readyEvent, int eventId);
	bool bmx_ReadyEvent_IsEventCompleted(ReadyEvent * readyEvent, int eventId);
	bool bmx_ReadyEvent_HasEvent(ReadyEvent * readyEvent, int eventId);
	int bmx_ReadyEvent_GetEventListSize(ReadyEvent * readyEvent);
	int bmx_ReadyEvent_GetEventAtIndex(ReadyEvent * readyEvent, int index);
	bool bmx_ReadyEvent_AddToWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address);
	bool bmx_ReadyEvent_RemoveFromWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address);
	bool bmx_ReadyEvent_IsInWaitList(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address);
	int bmx_ReadyEvent_GetRemoteWaitListSize(ReadyEvent * readyEvent, int eventId);
	MaxSystemAddress * bmx_ReadyEvent_GetFromWaitListAtIndex(ReadyEvent * readyEvent, int eventId, int index);
	ReadyEventSystemStatus bmx_ReadyEvent_GetReadyStatus(ReadyEvent * readyEvent, int eventId, MaxSystemAddress * address);
	void bmx_ReadyEvent_SetSendChannel(ReadyEvent * readyEvent, int newChannel);
	void bmx_ReadyEvent_delete(ReadyEvent * readyEvent);
	bool bmx_ReadyEvent_ForceCompletion(ReadyEvent * readyEvent, int eventId);

	NetworkIDManager * bmx_NetworkIDManager_create();
	void bmk_NetworkIDManager_SetIsNetworkIDAuthority(NetworkIDManager * manager, int isAuthority);
	int bmk_NetworkIDManager_IsNetworkIDAuthority(NetworkIDManager * manager);
	void bmk_NetworkIDManager_SetGuid(NetworkIDManager * manager, RakNetGUID * guid);
	int bmk_NetworkIDManager_GetSharedNetworkID(NetworkIDManager * manager);
	void bmk_NetworkIDManager_SetSharedNetworkID(NetworkIDManager * manager, int id);

}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MaxSocketDescriptor
{
public:
	MaxSocketDescriptor(const SocketDescriptor d);
	MaxSocketDescriptor(unsigned short port, const char *hostAddress);
	~MaxSocketDescriptor();
	SocketDescriptor & Descriptor();

private:
	SocketDescriptor descriptor;

};

class MaxNetworkID
{
public:
	MaxNetworkID(const NetworkID & n);
	~MaxNetworkID();
	NetworkID & ID();

private:
	NetworkID networkID;

};

class MaxSystemAddress
{
public:
	MaxSystemAddress(const SystemAddress & a);
	MaxSystemAddress();
	~ MaxSystemAddress();
	SystemAddress & Address();

private:
	SystemAddress systemAddress;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

