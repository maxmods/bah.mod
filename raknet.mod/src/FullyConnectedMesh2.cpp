/// \file
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "FullyConnectedMesh2.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakAssert.h"
#include "GetTime.h"
#include "Rand.h"

FullyConnectedMesh2::FullyConnectedMesh2()
{
	startupTime=0;
	autoParticipateConnections=true;
	totalConnectionCount=0;
	ourFCMGuid=0;
}
FullyConnectedMesh2::~FullyConnectedMesh2()
{
	Clear();
}
RakNetGUID FullyConnectedMesh2::GetConnectedHost(void) const
{
	if (ourFCMGuid==0)
		return UNASSIGNED_RAKNET_GUID;
	return hostRakNetGuid;
}
SystemAddress FullyConnectedMesh2::GetConnectedHostAddr(void) const
{
	if (ourFCMGuid==0)
		return UNASSIGNED_SYSTEM_ADDRESS;
	return hostSystemAddress;
}
RakNetGUID FullyConnectedMesh2::GetHostSystem(void) const
{
	if (ourFCMGuid==0)
		return rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);

	return hostRakNetGuid;
}
bool FullyConnectedMesh2::IsHostSystem(void) const
{
	return GetHostSystem()==rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
}
bool FullyConnectedMesh2::IsConnectedHost(void) const
{
	return GetConnectedHost()==rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
}
void FullyConnectedMesh2::SetAutoparticipateConnections(bool b)
{
	autoParticipateConnections=b;
}
void FullyConnectedMesh2::AddParticipant(SystemAddress participant)
{
	if (rakPeerInterface->IsConnected(participant,false,false)==false)
		return;

	if (ourFCMGuid==0)
	{
		if (guidRequestRetryList.GetIndexOf(participant)==(DataStructures::DefaultIndexType)-1)
		{
			SendFCMGuidRequest(participant);
			guidRequestRetryList.Push(participant,__FILE__,__LINE__);
		}
	}
	else
	{
		SendOurFCMGuid(participant);
	}
}

PluginReceiveResult FullyConnectedMesh2::OnReceive(Packet *packet)
{
	switch (packet->data[0])
	{
	case ID_REMOTE_NEW_INCOMING_CONNECTION:
		{
			unsigned int count;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(count);
			SystemAddress remoteAddress;
			RakNetGUID remoteGuid;
			char str[64];
			for (unsigned int i=0; i < count; i++)
			{
				bsIn.Read(remoteAddress);
				bsIn.Read(remoteGuid);
				remoteAddress.ToString(false,str);
				rakPeerInterface->Connect(str,remoteAddress.port,0,0);
			}
		}
		break;
	case ID_FCM2_REQUEST_FCMGUID:
		OnRequestFCMGuid(packet);
		break;
	case ID_FCM2_RESPOND_FCMGUID:
		OnRespondFCMGuid(packet);
		break;
	case ID_FCM2_INFORM_FCMGUID:
		OnInformFCMGuid(packet);
		break;
	}
	return RR_CONTINUE_PROCESSING;
}
void FullyConnectedMesh2::OnStartup(void)
{
	startupTime=RakNet::GetTimeUS();
}
void FullyConnectedMesh2::OnAttach(void)
{
	// In case Startup() was called first
	if (rakPeerInterface->IsActive())
		startupTime=RakNet::GetTimeUS();
}
void FullyConnectedMesh2::OnShutdown(void)
{
	Clear();
	startupTime=0;
}
void FullyConnectedMesh2::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) systemAddress;
	(void) rakNetGUID;

	DataStructures::DefaultIndexType idx;
	idx = guidRequestRetryList.GetIndexOf(systemAddress);
	if (idx!=-1)
	{
		guidRequestRetryList.RemoveAtIndex(idx,__FILE__,__LINE__);
		// If ourFCMGuid is UNASSIGNED, SendFCMGuidRequest to all in guidRequestRetryList
		if (ourFCMGuid==0)
		{
			for (idx=0; idx < guidRequestRetryList.GetSize(); idx++)
			{
				SendFCMGuidRequest(guidRequestRetryList[idx]);
			}
		}
	}

	for (idx=0; idx < participantList.Size(); idx++)
	{
		if (participantList[idx].systemAddress==systemAddress)
		{
			participantList[idx]=participantList[participantList.Size()-1];
			participantList.Pop();
			if (systemAddress==hostSystemAddress)
			{
				if (participantList.Size()==0)
				{
					hostSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
					hostRakNetGuid=rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
					hostFCM2Guid=ourFCMGuid;
				}
				else
				{
					CalculateHost(&hostSystemAddress, &hostRakNetGuid, &hostFCM2Guid);
				}
				PushNewHost(hostSystemAddress,hostRakNetGuid);
			}
			return;
		}
	}

}
RakNetTimeUS FullyConnectedMesh2::GetElapsedRuntime(void)
{
	RakNetTimeUS curTime=RakNet::GetTimeUS();
	if (curTime>startupTime)
		return curTime-startupTime;
	else
		return 0;
}
void FullyConnectedMesh2::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) isIncoming;
	(void) rakNetGUID;
	(void) systemAddress;

	if (autoParticipateConnections)
		AddParticipant(systemAddress);
}
void FullyConnectedMesh2::Clear(void)
{
	guidRequestRetryList.Clear(false,__FILE__,__LINE__);
	participantList.Clear(false);

	autoParticipateConnections=true;
	totalConnectionCount=0;
	ourFCMGuid=0;
}
void FullyConnectedMesh2::PushNewHost(const SystemAddress &sa, const RakNetGUID &guid)
{
	Packet *p = rakPeerInterface->AllocatePacket(sizeof(MessageID));
	p->data[0]=ID_FCM2_NEW_HOST;
	p->systemAddress=sa;
	p->systemIndex=(SystemIndex)-1;
	p->guid=guid;
	rakPeerInterface->PushBackPacket(p, true);
}
void FullyConnectedMesh2::SendOurFCMGuid(SystemAddress addr)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_FCM2_INFORM_FCMGUID);
	RakAssert(ourFCMGuid!=0); // Can't inform others of our FCM2Guid if it's unset!
	bsOut.Write(ourFCMGuid);
	bsOut.Write(totalConnectionCount);
	rakPeerInterface->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,addr,false);
}
void FullyConnectedMesh2::SendFCMGuidRequest(SystemAddress addr)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_FCM2_REQUEST_FCMGUID);
	bsOut.Write(GetElapsedRuntime());
	rakPeerInterface->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,addr,false);
}
void FullyConnectedMesh2::SendFCMGuidResponse(SystemAddress addr, unsigned int responseAssignedConnectionCount, unsigned int responseTotalConnectionCount)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_FCM2_RESPOND_FCMGUID);
	bsOut.Write(responseAssignedConnectionCount);
	bsOut.Write(responseTotalConnectionCount);
	rakPeerInterface->Send(&bsOut,HIGH_PRIORITY,RELIABLE,0,addr,false);
}
void FullyConnectedMesh2::AssignOurFCMGuid(unsigned int responseAssignedConnectionCount)
{
	// Only assigned once ever
	RakAssert(ourFCMGuid==0);
	unsigned int randomNumber = randomMT();
	ourFCMGuid |= randomNumber;
	uint64_t reponse64 = responseAssignedConnectionCount;
	ourFCMGuid |= reponse64<<32;
}
void FullyConnectedMesh2::CalculateHost(SystemAddress *systemAddress, RakNetGUID *rakNetGuid, FCM2Guid *fcm2Guid)
{
	// Can't calculate host without knowing our own
	RakAssert(ourFCMGuid!=0);

	// Can't calculate host without being connected to anyone else
	RakAssert(participantList.Size()>0);

	// Return the lowest value of all FCM2Guid
	FCM2Guid lowestFCMGuid=ourFCMGuid;
	SystemAddress associatedSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	RakNetGUID associatedRakNetGuid=rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);

	DataStructures::DefaultIndexType idx;
	for (idx=0; idx < participantList.Size(); idx++)
	{
		if (participantList[idx].fcm2Guid<lowestFCMGuid)
		{
			lowestFCMGuid=participantList[idx].fcm2Guid;
			associatedSystemAddress=participantList[idx].systemAddress;
			associatedRakNetGuid=participantList[idx].rakNetGuid;
		}
	}

	*systemAddress=associatedSystemAddress;
	*rakNetGuid=associatedRakNetGuid;
	*fcm2Guid=lowestFCMGuid;
}
void FullyConnectedMesh2::OnRequestFCMGuid(Packet *packet)
{
	RakNet::BitStream bsIn(packet->data,packet->length,false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	// If ourFCMGuid is set, and we are not host, return
	if (ourFCMGuid!=0 && IsHostSystem()==false)
		return;

	RakNetTimeUS senderElapsedRuntime;
	if (ourFCMGuid==0)
	{
		bsIn.Read(senderElapsedRuntime);
		RakNetTimeUS ourElapsedRuntime = GetElapsedRuntime();

		// Even if both systems disagree on who is host by time, the problem is avoid because of the random number for the low 4 bytes of FCM2Guid
		if (ourElapsedRuntime>senderElapsedRuntime)
		{
			// We are probably host
			// 2 is for 2 total connections, since neither of us are host
			SendFCMGuidResponse(packet->systemAddress, totalConnectionCount+1, 2);
		}
		else
		{
			// They are probably host
			SendFCMGuidResponse(packet->systemAddress, totalConnectionCount, 2);
		}
	}
	else
	{
		// totalConnectionCount is set to the value of 2 passed to SendFCMGuidResponse when our own guid is first assigned
		RakAssert(totalConnectionCount!=0);
		SendFCMGuidResponse(packet->systemAddress, totalConnectionCount+1, totalConnectionCount+1);
	}
}
void FullyConnectedMesh2::OnRespondFCMGuid(Packet *packet)
{
	RakNet::BitStream bsIn(packet->data,packet->length,false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	unsigned int responseAssignedConnectionCount;
	unsigned int responseTotalConnectionCount;
	bsIn.Read(responseAssignedConnectionCount);
	bsIn.Read(responseTotalConnectionCount);
	if (ourFCMGuid==0)
	{
		AssignOurFCMGuid(responseAssignedConnectionCount);
		guidRequestRetryList.Clear(true,__FILE__,__LINE__);
		printf("Total connection count updated from %i to %i\n", totalConnectionCount, responseTotalConnectionCount);
		totalConnectionCount=responseTotalConnectionCount;
		if (participantList.Size()!=0)
		{
			CalculateHost(&hostSystemAddress, &hostRakNetGuid, &hostFCM2Guid);
			PushNewHost(hostSystemAddress, hostRakNetGuid);
		}
	}
	SendOurFCMGuid(packet->systemAddress);
	DataStructures::DefaultIndexType idx;
	for (idx=0; idx < participantList.Size(); idx++)
		SendOurFCMGuid(participantList[idx].systemAddress);
}
void FullyConnectedMesh2::OnInformFCMGuid(Packet *packet)
{
	RakNet::BitStream bsIn(packet->data,packet->length,false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	DataStructures::DefaultIndexType idx;
	for (idx=0; idx < participantList.Size(); idx++)
	{
		if (participantList[idx].systemAddress==packet->systemAddress)
		{
			// Duplicate message, just ignore since they are already in the participantList
			return;
		}
	}


	FCM2Guid theirFCMGuid;
	unsigned int theirTotalConnectionCount;
	bsIn.Read(theirFCMGuid);
	bsIn.Read(theirTotalConnectionCount);
	participantList.Push(FCM2Participant(theirFCMGuid, packet->guid, packet->systemAddress), __FILE__,__LINE__);
	if (theirTotalConnectionCount>totalConnectionCount)
	{
		printf("Total connection count IS  updated from %i to %i\n", totalConnectionCount, theirTotalConnectionCount);
		totalConnectionCount=theirTotalConnectionCount;
	}
	else
	{
		printf("Total connection count NOT updated from %i to %i\n", totalConnectionCount, theirTotalConnectionCount);
	}

	if (ourFCMGuid!=0)
	{
		SystemAddress systemAddress;
		RakNetGUID rakNetGuid;
		FCM2Guid fcm2Guid;
		CalculateHost(&systemAddress, &rakNetGuid, &fcm2Guid);
		if (rakNetGuid!=hostRakNetGuid)
		{
			hostSystemAddress=systemAddress;
			hostRakNetGuid=rakNetGuid;
			hostFCM2Guid=fcm2Guid;
			PushNewHost(hostSystemAddress, hostRakNetGuid);
		}
	}
}
