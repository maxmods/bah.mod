#include "Router2.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "RakNetTime.h"
#include "GetTime.h"
#include "DS_OrderedList.h"
#include "SocketLayer.h"

using namespace RakNet;

enum Router2MessageIdentifiers
{
	ID_ROUTER_2_QUERY_FORWARDING,
	ID_ROUTER_2_REPLY_FORWARDING,
	ID_ROUTER_2_REQUEST_FORWARDING,
	ID_ROUTER_2_FORWARDING_SUCCESS,
};
Router2::ConnnectRequest::ConnnectRequest()
{

}
Router2::ConnnectRequest::~ConnnectRequest()
{

}
Router2::Router2()
{
	udpForwarder=0;
	maximumForwardingRequests=0;
}
Router2::~Router2()
{
	ClearAll();

	if (udpForwarder)
	{
		udpForwarder->Shutdown();
		RakNet::OP_DELETE(udpForwarder,__FILE__,__LINE__);
	}
}
void Router2::ClearMinipunches(void)
{
	miniPunchesInProgress.Clear(false,__FILE__,__LINE__);
}
void Router2::ClearConnectionRequests(void)
{
	for (unsigned int i=0; i < connectionRequests.Size(); i++)
	{
		RakNet::OP_DELETE(connectionRequests[i],__FILE__,__LINE__);
	}
	connectionRequests.Clear(false,__FILE__,__LINE__);
}
bool Router2::ConnectInternal(RakNetGUID endpointGuid, bool returnConnectionLostOnFailure)
{
	// if (alreadyConnected to endpointGuid) Return false
	if (rakPeerInterface->IsConnected(endpointGuid,false,false)==true)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return false;
	}

	int largestPing = GetLargestPingAmongConnectedSystems();
	if (largestPing<0)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);

		// Not connected to anyone
		return false;
	}

	// ALready in progress?
	if (GetConnectionRequestIndex(endpointGuid)!=(unsigned int)-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);

		return false;
	}

	// StoreRequest(endpointGuid, Largest(ping*2), systemsSentTo). Set state REQUEST_STATE_QUERY_FORWARDING
	Router2::ConnnectRequest *cr = RakNet::OP_NEW<Router2::ConnnectRequest>(__FILE__,__LINE__);
	DataStructures::List<SystemAddress> addresses;
	DataStructures::List<RakNetGUID> guids;
	rakPeerInterface->GetSystemList(addresses, guids);
	cr->requestState=R2RS_REQUEST_STATE_QUERY_FORWARDING;
	cr->pingTimeout=RakNet::GetTimeMS()+largestPing*2;
	cr->endpointGuid=endpointGuid;
	cr->returnConnectionLostOnFailure=returnConnectionLostOnFailure;
	for (unsigned int i=0; i < guids.Size(); i++)
	{
		ConnectionRequestSystem crs;
		crs.guid=guids[i];
		crs.pingToEndpoint=-1;
		cr->connectionRequestSystems.Push(crs,__FILE__,__LINE__);
	}
	connectionRequests.Push(cr,__FILE__,__LINE__);

	// Broadcast(ID_ROUTER_2_QUERY_FORWARDING, endpointGuid);
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_ROUTER_2_INTERNAL);
	bsOut.Write((unsigned char) ID_ROUTER_2_QUERY_FORWARDING);
	bsOut.Write(endpointGuid);
	rakPeerInterface->Send(&bsOut,MEDIUM_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
	return true;
}
void Router2::Connect(RakNetGUID endpointGuid)
{
	ConnectInternal(endpointGuid,false);
}
void Router2::SetMaximumForwardingRequests(int max)
{
	if (max>0 && maximumForwardingRequests<=0)
	{
		udpForwarder = RakNet::OP_NEW<UDPForwarder>(__FILE__,__LINE__);
		udpForwarder->Startup();
	}
	else if (max<=0 && maximumForwardingRequests>0)
	{
		udpForwarder->Shutdown();
		RakNet::OP_DELETE(udpForwarder,__FILE__,__LINE__);
		udpForwarder=0;
	}

	maximumForwardingRequests=max;
}
PluginReceiveResult Router2::OnReceive(Packet *packet)
{
	SystemAddress sa;
	RakNet::BitStream bs(packet->data,packet->length,false);
	if (packet->data[0]==ID_ROUTER_2_INTERNAL)
	{
		switch (packet->data[1])
		{
		case ID_ROUTER_2_QUERY_FORWARDING:
			{
				OnQueryForwarding(packet);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
			break;
		case ID_ROUTER_2_REPLY_FORWARDING:
			{
				OnQueryForwardingReply(packet);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
			break;
		case ID_ROUTER_2_REQUEST_FORWARDING:
			{
				OnRequestForwarding(packet);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
			break;
		case ID_ROUTER_2_FORWARDING_SUCCESS:
			{
				OnForwardingSuccess(packet);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
			break;
		}
	}
	else if (packet->data[0]==ID_OUT_OF_BAND_INTERNAL && packet->length>=2)
	{
		switch (packet->data[1])
		{
			case ID_ROUTER_2_REPLY_TO_SENDER_PORT:
				SendOOBFromRakNetPort(ID_ROUTER_2_MINI_PUNCH_REPLY, 0, packet->systemAddress);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			case ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT:
				bs.IgnoreBytes(2);
				sa.binaryAddress=packet->systemAddress.binaryAddress;
				bs.Read(sa.port);
				SendOOBFromRakNetPort(ID_ROUTER_2_MINI_PUNCH_REPLY, 0, sa);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			case ID_ROUTER_2_MINI_PUNCH_REPLY:
				// TODO:
				// Problem: This message itself was forwarded. We want to create the socket but not forward it until reply success
				// We also need to then read on the forwarding socket until forwarding begins
				OnMiniPunchReply(packet);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}
	}
	else if (packet->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
	{
		unsigned int forwardedConnectionIndex=0;
		while (forwardedConnectionIndex<forwardedConnectionList.Size())
		{
			if (forwardedConnectionList[forwardedConnectionIndex].intermediaryAddress==packet->systemAddress)
			{
				packet->guid=forwardedConnectionList[forwardedConnectionIndex].endpointGuid;
				forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
			}
			else
				forwardedConnectionIndex++;
		}
	}

	return RR_CONTINUE_PROCESSING;
}
void Router2::Update(void)
{
	RakNetTimeMS curTime = RakNet::GetTimeMS();
	unsigned int connectionRequestIndex=0;
	while (connectionRequestIndex < connectionRequests.Size())
	{
		ConnnectRequest* connectionRequest = connectionRequests[connectionRequestIndex];
		// pingTimeout is only used with R2RS_REQUEST_STATE_QUERY_FORWARDING
		if (connectionRequest->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING &&
			connectionRequest->pingTimeout < curTime)
		{
			bool anyRemoved=false;
			unsigned int connectionRequestGuidIndex=0;
			while (connectionRequestGuidIndex < connectionRequest->connectionRequestSystems.Size())
			{
				if (connectionRequest->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint<0)
				{
					anyRemoved=true;
					connectionRequest->connectionRequestSystems.RemoveAtIndexFast(connectionRequestGuidIndex);
				}
				else
				{
					connectionRequestGuidIndex++;
				}
			}

			if (anyRemoved)
			{
				if (UpdateForwarding(connectionRequestIndex)==false)
				{
					RemoveConnectionRequest(connectionRequestIndex);
				}
				else
				{
					connectionRequestIndex++;
				}
			}
			else
			{
				connectionRequestIndex++;
			}
		}
		else
		{
			connectionRequestIndex++;
		}
	}

	unsigned int i=0;
	while (i < miniPunchesInProgress.Size())
	{
		if (miniPunchesInProgress[i].timeout<curTime)
		{
			printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);

			ReturnFailureOnCannotForward(miniPunchesInProgress[i].sourceGuid, miniPunchesInProgress[i].endpointGuid);
			miniPunchesInProgress.RemoveAtIndexFast(i);
		}
		else if (curTime>miniPunchesInProgress[i].nextAction)
		{
			miniPunchesInProgress[i].nextAction=curTime+100;
			SendOOBMessages(&miniPunchesInProgress[i]);
		}
		else
			i++;
	}

}
void Router2::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) systemAddress;


	unsigned int forwardedConnectionIndex=0;
	while (forwardedConnectionIndex<forwardedConnectionList.Size())
	{
		if (forwardedConnectionList[forwardedConnectionIndex].endpointGuid==rakNetGUID)
		{
			// No longer need forwarding
			forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
		}
		else if (forwardedConnectionList[forwardedConnectionIndex].intermediaryGuid==rakNetGUID)
		{
			// Lost connection to intermediary. Reconnect to endpoint. If failed, push ID_CONNECTION_LOST
			ConnectInternal(forwardedConnectionList[forwardedConnectionIndex].endpointGuid, true);
			forwardedConnectionList.RemoveAtIndexFast(forwardedConnectionIndex);
		}
		else
			forwardedConnectionIndex++;
	}

	unsigned int connectionRequestIndex=0;
	while (connectionRequestIndex < connectionRequests.Size())
	{
		unsigned int connectionRequestGuidIndex = connectionRequests[connectionRequestIndex]->GetGuidIndex(rakNetGUID);
		if (connectionRequestGuidIndex>=0)
		{
			connectionRequests[connectionRequestIndex]->connectionRequestSystems.RemoveAtIndexFast(connectionRequestGuidIndex);
			if (UpdateForwarding(connectionRequestIndex)==false)
			{
				RemoveConnectionRequest(connectionRequestIndex);
			}
			else
			{
				connectionRequestIndex++;
			}
		}
		else
		{
			connectionRequestIndex++;
		}
	}


	unsigned int i=0;
	while (i < miniPunchesInProgress.Size())
	{
		if (miniPunchesInProgress[i].sourceGuid==rakNetGUID || miniPunchesInProgress[i].endpointGuid==rakNetGUID)
		{
			if (miniPunchesInProgress[i].sourceGuid!=rakNetGUID)
				ReturnFailureOnCannotForward(miniPunchesInProgress[i].sourceGuid, miniPunchesInProgress[i].endpointGuid);
			miniPunchesInProgress.RemoveAtIndexFast(i);
		}
		else
			i++;
	}
}
void Router2::OnFailedConnectionAttempt(SystemAddress systemAddress, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason)
{
	(void) failedConnectionAttemptReason;
	(void) systemAddress;
}
void Router2::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) isIncoming;

	unsigned int forwardedConnectionIndex=0;
	while (forwardedConnectionIndex<forwardedConnectionList.Size())
	{
		// Never timeout forwarded connections. They fail if we lose the connection and can't reestablish
		if (forwardedConnectionList[forwardedConnectionIndex].endpointGuid==rakNetGUID)
			rakPeerInterface->SetTimeoutTime(1000000, systemAddress);
		forwardedConnectionIndex++;
	}
}
void Router2::OnShutdown(void)
{
	ClearAll();
}
bool Router2::UpdateForwarding(unsigned int connectionRequestIndex)
{
	ConnnectRequest* connectionRequest = connectionRequests[connectionRequestIndex];
	if (connectionRequest->connectionRequestSystems.Size()==0)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		if (connectionRequest->returnConnectionLostOnFailure)
			ReturnToUser(ID_CONNECTION_LOST, connectionRequest->endpointGuid, UNASSIGNED_SYSTEM_ADDRESS);
		else
			ReturnToUser(ID_ROUTER_2_FORWARDING_NO_PATH, connectionRequest->endpointGuid, UNASSIGNED_SYSTEM_ADDRESS);
		return false;
	}

	if (connectionRequest->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING)
	{
		for (unsigned int i=0; i < connectionRequest->connectionRequestSystems.Size(); i++)
		{
			if (connectionRequest->connectionRequestSystems[i].pingToEndpoint<=0)
				break;
		}

		RequestForwarding(connectionRequestIndex);
	}
	else if (connectionRequest->requestState==REQUEST_STATE_REQUEST_FORWARDING)
	{
		RequestForwarding(connectionRequestIndex);
	}

	return true;
}
void Router2::RemoveConnectionRequest(unsigned int connectionRequestIndex)
{
	RakNet::OP_DELETE(connectionRequests[connectionRequestIndex],__FILE__,__LINE__);
	connectionRequests.RemoveAtIndexFast(connectionRequestIndex);
}
int ConnectionRequestSystemComp( const Router2::ConnectionRequestSystem & key, const Router2::ConnectionRequestSystem &data )
{
	if (key.pingToEndpoint * (key.usedForwardingEntries+1) < data.pingToEndpoint * (data.usedForwardingEntries+1))
		return -1;
	if (key.pingToEndpoint * (key.usedForwardingEntries+1) == data.pingToEndpoint * (data.usedForwardingEntries+1))
		return 0;
	return 1;
}
void Router2::RequestForwarding(unsigned int connectionRequestIndex)
{
	ConnnectRequest* connectionRequest = connectionRequests[connectionRequestIndex];
	RakAssert(connectionRequest->requestState==R2RS_REQUEST_STATE_QUERY_FORWARDING);
	connectionRequest->requestState=REQUEST_STATE_REQUEST_FORWARDING;

	if (connectionRequest->GetGuidIndex(connectionRequest->lastRequestedForwardingSystem)!=(unsigned int)-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return;
	}

	// Prioritize systems to request forwarding
	DataStructures::OrderedList<ConnectionRequestSystem, ConnectionRequestSystem, ConnectionRequestSystemComp> commandList;
	unsigned int connectionRequestGuidIndex;
	for (connectionRequestGuidIndex=0; connectionRequestGuidIndex < connectionRequest->connectionRequestSystems.Size(); connectionRequestGuidIndex++)
	{
		RakAssert(connectionRequest->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint>=0);
		commandList.Insert(connectionRequest->connectionRequestSystems[connectionRequestGuidIndex],
			connectionRequest->connectionRequestSystems[connectionRequestGuidIndex],
			true,
			__FILE__,
			__LINE__);
	}

	connectionRequest->lastRequestedForwardingSystem=commandList[0].guid;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_ROUTER_2_INTERNAL);
	bsOut.Write((unsigned char) ID_ROUTER_2_REQUEST_FORWARDING);
	bsOut.Write(connectionRequest->endpointGuid);
	rakPeerInterface->Send(&bsOut,MEDIUM_PRIORITY,RELIABLE_ORDERED,0,connectionRequest->lastRequestedForwardingSystem,false);
}
void Router2::SendFailureOnCannotForward(RakNetGUID sourceGuid, RakNetGUID endpointGuid)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_ROUTER_2_INTERNAL);
	bsOut.Write((unsigned char) ID_ROUTER_2_REPLY_FORWARDING);
	bsOut.Write(endpointGuid);
	bsOut.Write(false);
	rakPeerInterface->Send(&bsOut,MEDIUM_PRIORITY,RELIABLE_ORDERED,0,sourceGuid,false);
}
int Router2::ReturnFailureOnCannotForward(RakNetGUID sourceGuid, RakNetGUID endpointGuid)
{
	// If the number of systems we are currently forwarding>=maxForwarding, return ID_ROUTER_2_REPLY_FORWARDING,endpointGuid,false
	if (udpForwarder==0 || udpForwarder->GetUsedForwardEntries()/2>maximumForwardingRequests)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		SendFailureOnCannotForward(sourceGuid,endpointGuid);
		return -1;
	}

	int pingToEndpoint;
	pingToEndpoint = rakPeerInterface->GetAveragePing(endpointGuid);
	if (pingToEndpoint==-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		SendFailureOnCannotForward(sourceGuid,endpointGuid);
		return -1;
	}
	return pingToEndpoint;
}
void Router2::OnQueryForwarding(Packet *packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(MessageID) + sizeof(unsigned char));
	RakNetGUID endpointGuid;
	// Read endpointGuid
	bs.Read(endpointGuid);

	int pingToEndpoint = ReturnFailureOnCannotForward(packet->guid, endpointGuid);
	if (pingToEndpoint==-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return;
	}

	// If we are connected to endpointGuid, reply ID_ROUTER_2_REPLY_FORWARDING,endpointGuid,true,ping,numCurrentlyForwarding
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_ROUTER_2_INTERNAL);
	bsOut.Write((unsigned char) ID_ROUTER_2_REPLY_FORWARDING);
	bsOut.Write(endpointGuid);
	bsOut.Write(true);
	bsOut.Write((unsigned short) pingToEndpoint);
	bsOut.Write((unsigned short) udpForwarder->GetUsedForwardEntries()/2);
	rakPeerInterface->Send(&bsOut,MEDIUM_PRIORITY,RELIABLE_ORDERED,0,packet->guid,false);
}
void Router2::OnQueryForwardingReply(Packet *packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(MessageID) + sizeof(unsigned char));
	RakNetGUID endpointGuid;
	bs.Read(endpointGuid);
	// Find endpointGuid among stored requests

	unsigned int connectionRequestIndex = GetConnectionRequestIndex(endpointGuid);
	if (connectionRequestIndex==(unsigned int)-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return;
	}
	unsigned int connectionRequestGuidIndex = connectionRequests[connectionRequestIndex]->GetGuidIndex(packet->guid);
	if (connectionRequestGuidIndex==(unsigned int)-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return;
	}

	bool canForward;
	bs.Read(canForward);
	if (canForward)
	{
		unsigned short pingToEndpoint;
		unsigned short usedEntries;
		bs.Read(pingToEndpoint);
		bs.Read(usedEntries);
		connectionRequests[connectionRequestIndex]->connectionRequestSystems[connectionRequestGuidIndex].usedForwardingEntries=usedEntries;
		connectionRequests[connectionRequestIndex]->connectionRequestSystems[connectionRequestGuidIndex].pingToEndpoint=rakPeerInterface->GetAveragePing(packet->guid)+pingToEndpoint;
	}
	else
	{
		connectionRequests[connectionRequestIndex]->connectionRequestSystems.RemoveAtIndex(connectionRequestGuidIndex);
	}

	if (UpdateForwarding(connectionRequestIndex)==false)
	{
		RemoveConnectionRequest(connectionRequestIndex);
	}
}
void Router2::SendForwardingSuccess(RakNetGUID sourceGuid, RakNetGUID endpointGuid)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)ID_ROUTER_2_INTERNAL);
	bsOut.Write((unsigned char) ID_ROUTER_2_FORWARDING_SUCCESS);
	bsOut.Write(endpointGuid);
	rakPeerInterface->Send(&bsOut,MEDIUM_PRIORITY,RELIABLE_ORDERED,0,sourceGuid,false);
}
void Router2::SendOOBFromRakNetPort(OutOfBandIdentifiers oob, BitStream *extraData, SystemAddress sa)
{
	RakNet::BitStream oobBs;
	oobBs.Write((unsigned char)oob);
	if (extraData)
	{
		extraData->ResetReadPointer();
		oobBs.Write(*extraData);
	}
	char ipAddressString[32];
	sa.ToString(false, ipAddressString);
	rakPeerInterface->SendOutOfBand((const char*) ipAddressString,sa.port,(MessageID) ID_OUT_OF_BAND_INTERNAL,(const char*) oobBs.GetData(),oobBs.GetNumberOfBytesUsed());
}
void Router2::SendOOBFromSpecifiedSocket(OutOfBandIdentifiers oob, SystemAddress sa, SOCKET socket)
{
	RakNet::BitStream bs;
	rakPeerInterface->WriteOutOfBandHeader(&bs, ID_OUT_OF_BAND_INTERNAL);
	bs.Write((unsigned char) oob);
	SocketLayer::Instance()->SendTo_PC( socket, (const char*) bs.GetData(), bs.GetNumberOfBytesUsed(), sa.binaryAddress, sa.port );
}
void Router2::SendOOBMessages(Router2::MiniPunchRequest *mpr)
{
	// Mini NAT punch
	// Send from srcToDestPort to packet->systemAddress (source). If the message arrives, the remote system should reply.
	SendOOBFromSpecifiedSocket(ID_ROUTER_2_REPLY_TO_SENDER_PORT, mpr->sourceAddress, mpr->destToSourceSocket);

	// Send from destToSourcePort to endpointSystemAddress (destination). If the message arrives, the remote system should reply.
	SendOOBFromSpecifiedSocket(ID_ROUTER_2_REPLY_TO_SENDER_PORT, mpr->endpointAddress, mpr->srcToDestSocket);

	// Tell source to send to srcToDestPort
	RakNet::BitStream extraData;
	extraData.Write(mpr->srcToDestPort);
	SendOOBFromRakNetPort(ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT, &extraData, mpr->sourceAddress);

	// Tell destination to send to destToSourcePort
	extraData.Reset();
	extraData.Write(mpr->destToSourcePort);
	SendOOBFromRakNetPort(ID_ROUTER_2_REPLY_TO_SPECIFIED_PORT, &extraData, mpr->endpointAddress);
}
void Router2::OnRequestForwarding(Packet *packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(MessageID) + sizeof(unsigned char));
	RakNetGUID endpointGuid;
	bs.Read(endpointGuid);

	int pingToEndpoint = ReturnFailureOnCannotForward(packet->guid, endpointGuid);
	if (pingToEndpoint==-1)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		return;
	}

	unsigned short srcToDestPort;
	unsigned short destToSourcePort;
	SOCKET srcToDestSocket;
	SOCKET destToSourceSocket;
	SystemAddress endpointSystemAddress = rakPeerInterface->GetSystemAddressFromGuid(endpointGuid);
	UDPForwarderResult result = udpForwarder->StartForwarding(
		packet->systemAddress, endpointSystemAddress, 10000, 0,
		&srcToDestPort, &destToSourcePort, &srcToDestSocket, &destToSourceSocket);

	if (result==UDPFORWARDER_FORWARDING_ALREADY_EXISTS)
	{
		SendForwardingSuccess(packet->guid, endpointGuid);
	}
	else if (result==UDPFORWARDER_NO_SOCKETS || result==UDPFORWARDER_INVALID_PARAMETERS)
	{
		printf("Router2 failed at %s:%i\n", __FILE__, __LINE__);
		RakAssert(result!=UDPFORWARDER_INVALID_PARAMETERS);
		SendFailureOnCannotForward(packet->guid, endpointGuid);
	}
	else
	{

		// Store the punch request
		MiniPunchRequest miniPunchRequest;
		miniPunchRequest.endpointAddress=endpointSystemAddress;
		miniPunchRequest.endpointGuid=endpointGuid;
		miniPunchRequest.gotReplyFromEndpoint=false;
		miniPunchRequest.gotReplyFromSource=false;
		miniPunchRequest.sourceGuid=packet->guid;
		miniPunchRequest.sourceAddress=packet->systemAddress;
		miniPunchRequest.srcToDestPort=srcToDestPort;
		miniPunchRequest.destToSourcePort=destToSourcePort;
		miniPunchRequest.srcToDestSocket=srcToDestSocket;
		miniPunchRequest.destToSourceSocket=destToSourceSocket;
		int ping1 = rakPeerInterface->GetAveragePing(packet->guid);
		int ping2 = rakPeerInterface->GetAveragePing(endpointGuid);
		if (ping1>ping2)
			miniPunchRequest.timeout=RakNet::GetTimeMS()+ping1*3+300;
		else
			miniPunchRequest.timeout=RakNet::GetTimeMS()+ping2*3+300;
		miniPunchRequest.nextAction=RakNet::GetTimeMS()+100;
		SendOOBMessages(&miniPunchRequest);
		miniPunchesInProgress.Push(miniPunchRequest,__FILE__,__LINE__);
	}
}
void Router2::OnMiniPunchReply(Packet *packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(MessageID) + sizeof(unsigned char));

	// Find stored punch request
	unsigned int i=0;
	while (i < miniPunchesInProgress.Size())
	{
		if (miniPunchesInProgress[i].sourceGuid==packet->guid || miniPunchesInProgress[i].endpointGuid==packet->guid)
		{
			if (miniPunchesInProgress[i].sourceGuid==packet->guid)
				miniPunchesInProgress[i].gotReplyFromSource=true;
			if (miniPunchesInProgress[i].endpointGuid==packet->guid)
				miniPunchesInProgress[i].gotReplyFromEndpoint=true;
			if (miniPunchesInProgress[i].gotReplyFromEndpoint==true &&
				miniPunchesInProgress[i].gotReplyFromSource==true)
			{
				SendForwardingSuccess(miniPunchesInProgress[i].sourceGuid, miniPunchesInProgress[i].endpointGuid);
				miniPunchesInProgress.RemoveAtIndexFast(i);
			}
			else
			{
				i++;
			}
		}
		else
			i++;
	}
}
void Router2::OnForwardingSuccess(Packet *packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(MessageID) + sizeof(unsigned char));
	RakNetGUID endpointGuid;
	bs.Read(endpointGuid);

	unsigned int forwardingIndex;
	for (forwardingIndex=0; forwardingIndex < forwardedConnectionList.Size(); forwardingIndex++)
	{
		if (forwardedConnectionList[forwardingIndex].endpointGuid==endpointGuid)
			break;
	}

	if (forwardingIndex<forwardedConnectionList.Size())
	{
		// Return rerouted notice
		rakPeerInterface->ChangeSystemAddress(endpointGuid, packet->systemAddress);
		ReturnToUser(ID_ROUTER_2_REROUTED, endpointGuid, packet->systemAddress);		
	}
	else
	{
		char ipAddressString[32];
		packet->systemAddress.ToString(false, ipAddressString);
		rakPeerInterface->Connect(ipAddressString, packet->systemAddress.port, 0,0);

		// removeFrom connectionRequests;
		unsigned int connectionRequestIndex = GetConnectionRequestIndex(endpointGuid);
		ForwardedConnection fc;
		fc.endpointGuid=endpointGuid;
		fc.intermediaryAddress=packet->systemAddress;
		fc.intermediaryGuid=packet->guid;
		fc.returnConnectionLostOnFailure=connectionRequests[connectionRequestIndex]->returnConnectionLostOnFailure;
		// add to forwarding list
		forwardedConnectionList.Push(fc,__FILE__,__LINE__);
		connectionRequests.RemoveAtIndexFast(connectionRequestIndex);
	}
}
int Router2::GetLargestPingAmongConnectedSystems(void) const
{
	int avePing;
	int largestPing=-1;
	unsigned short maxPeers = rakPeerInterface->GetMaximumNumberOfPeers();
	if (maxPeers==0)
		return 9999;
	unsigned short index;
	for (index=0; index < rakPeerInterface->GetMaximumNumberOfPeers(); index++)
	{
		RakNetGUID g = rakPeerInterface->GetGUIDFromIndex(index);
		if (g!=UNASSIGNED_RAKNET_GUID)
		{
			avePing=rakPeerInterface->GetAveragePing(rakPeerInterface->GetGUIDFromIndex(index));
			if (avePing>largestPing)
				largestPing=avePing;
		}
	}
	return largestPing;
}

unsigned int Router2::GetConnectionRequestIndex(RakNetGUID endpointGuid)
{
	unsigned int i;
	for (i=0; i < connectionRequests.Size(); i++)
	{
		if (connectionRequests[i]->endpointGuid==endpointGuid)
			return i;
	}
	return (unsigned int) -1;
}
unsigned int Router2::ConnnectRequest::GetGuidIndex(RakNetGUID guid)
{
	unsigned int i;
	for (i=0; i < connectionRequestSystems.Size(); i++)
	{
		if (connectionRequestSystems[i].guid==guid)
			return i;
	}
	return (unsigned int) -1;
}
void Router2::ReturnToUser(MessageID messageId, RakNetGUID endpointGuid, SystemAddress systemAddress)
{
	Packet *p = rakPeerInterface->AllocatePacket(sizeof(MessageID)+sizeof(unsigned char));
	p->data[0]=messageId;
	p->systemAddress=systemAddress;
	p->systemAddress.systemIndex=(SystemIndex)-1;
	p->guid=endpointGuid;
	rakPeerInterface->PushBackPacket(p, true);
}
void Router2::ClearForwardedConnections(void)
{
	forwardedConnectionList.Clear(false,__FILE__,__LINE__);
}
void Router2::ClearAll(void)
{
	ClearConnectionRequests();
	ClearMinipunches();
	ClearForwardedConnections();
}
