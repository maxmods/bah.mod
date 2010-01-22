/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "PluginInterface2.h"
#include "PacketizedTCP.h"
#include "RakPeerInterface.h"
#include "BitStream.h"

PluginInterface2::PluginInterface2()
{
	rakPeerInterface=0;
	packetizedTCP=0;
}
PluginInterface2::~PluginInterface2()
{

}
void PluginInterface2::SendUnified( const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
{
	if (rakPeerInterface)
		rakPeerInterface->Send(bitStream,priority,reliability,orderingChannel,systemIdentifier,broadcast);
	else
		packetizedTCP->Send((const char*) bitStream->GetData(), bitStream->GetNumberOfBytesUsed(), systemIdentifier.systemAddress, broadcast);
}
Packet *PluginInterface2::AllocatePacketUnified(unsigned dataSize)
{
	if (rakPeerInterface)
		return rakPeerInterface->AllocatePacket(dataSize);
	else
		return packetizedTCP->AllocatePacket(dataSize);
		
}
void PluginInterface2::PushBackPacketUnified(Packet *packet, bool pushAtHead)
{
	if (rakPeerInterface)
		return rakPeerInterface->PushBackPacket(packet,pushAtHead);
	else
		return packetizedTCP->PushBackPacket(packet,pushAtHead);
}
void PluginInterface2::DeallocPacketUnified(Packet *packet)
{
	if (rakPeerInterface)
		return rakPeerInterface->DeallocatePacket(packet);
	else
		return packetizedTCP->DeallocatePacket(packet);
}
bool PluginInterface2::SendListUnified( const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
{
	if (rakPeerInterface)
	{
		return rakPeerInterface->SendList(data,lengths,numParameters,priority,reliability,orderingChannel,systemIdentifier,broadcast);
	}
	else
	{
		return packetizedTCP->SendList(data,lengths,numParameters,systemIdentifier.systemAddress,broadcast );
	}
}
void PluginInterface2::SetRakPeerInterface( RakPeerInterface *ptr )
{
	rakPeerInterface=ptr;
}
void PluginInterface2::SetPacketizedTCP( PacketizedTCP *ptr )
{
	packetizedTCP=ptr;
}
