#include "UDPProxyCoordinator.h"
#include "BitStream.h"
#include "UDPProxyCommon.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "Rand.h"

using namespace RakNet;

bool operator<( const DataStructures::MLKeyRef<UDPProxyCoordinator::SenderAndTargetAddress> &inputKey, const UDPProxyCoordinator::ForwardingRequest *cls )
{
	return inputKey.Get().senderClientAddress < cls->sata.senderClientAddress ||
		(inputKey.Get().senderClientAddress == cls->sata.senderClientAddress && inputKey.Get().targetClientAddress < cls->sata.targetClientAddress);
}
bool operator>( const DataStructures::MLKeyRef<UDPProxyCoordinator::SenderAndTargetAddress> &inputKey, const UDPProxyCoordinator::ForwardingRequest *cls )
{
	return inputKey.Get().senderClientAddress > cls->sata.senderClientAddress ||
		(inputKey.Get().senderClientAddress == cls->sata.senderClientAddress && inputKey.Get().targetClientAddress > cls->sata.targetClientAddress);
}
bool operator==( const DataStructures::MLKeyRef<UDPProxyCoordinator::SenderAndTargetAddress> &inputKey, const UDPProxyCoordinator::ForwardingRequest *cls )
{
	return inputKey.Get().senderClientAddress == cls->sata.senderClientAddress && inputKey.Get().targetClientAddress == cls->sata.targetClientAddress;
}

UDPProxyCoordinator::UDPProxyCoordinator()
{

}
UDPProxyCoordinator::~UDPProxyCoordinator()
{
	Clear();
}
void UDPProxyCoordinator::SetRemoteLoginPassword(RakNet::RakString password)
{
	remoteLoginPassword=password;
}
void UDPProxyCoordinator::Update(void)
{

}
PluginReceiveResult UDPProxyCoordinator::OnReceive(Packet *packet)
{
	if (packet->data[0]==ID_UDP_PROXY_GENERAL && packet->length>1)
	{
		switch (packet->data[1])
		{
		case ID_UDP_PROXY_FORWARDING_REQUEST_FROM_CLIENT_TO_COORDINATOR:
			OnForwardingRequestFromClientToCoordinator(packet);
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		case ID_UDP_PROXY_LOGIN_REQUEST_FROM_SERVER_TO_COORDINATOR:
			OnLoginRequestFromServerToCoordinator(packet);
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		case ID_UDP_PROXY_FORWARDING_REPLY_FROM_SERVER_TO_COORDINATOR:
			OnForwardingReplyFromServerToCoordinator(packet);
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}
	}
	return RR_CONTINUE_PROCESSING;
}
void UDPProxyCoordinator::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) rakNetGUID;

	DataStructures::DefaultIndexType idx, idx2;

	idx=0;
	while (idx < forwardingRequestList.GetSize())
	{
		if (forwardingRequestList[idx]->requestingAddress==systemAddress)
		{
			// Guy disconnected before the attempt completed
			RakNet::OP_DELETE(forwardingRequestList[idx], __FILE__, __LINE__);
			forwardingRequestList.RemoveAtIndex(idx);
		}
		else
			idx++;
	}

	idx = serverList.GetIndexOf(systemAddress);
	if (idx!=-1)
	{
		ForwardingRequest *fw;
		// For each pending client for this server, choose from remaining servers.
		for (idx2=0; idx2 < forwardingRequestList.GetSize(); idx2++)
		{
			fw = forwardingRequestList[idx2];
			if (fw->currentlyAttemptedServerAddress==systemAddress)
			{
				// Try the next server
				TryNextServer(fw->sata, fw);
			}
		}

		// Remove dead server
		serverList.RemoveAtIndex(idx);
	}
}
void UDPProxyCoordinator::OnForwardingRequestFromClientToCoordinator(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(2);
	SystemAddress sourceAddress;
	incomingBs.Read(sourceAddress);
	if (sourceAddress==UNASSIGNED_SYSTEM_ADDRESS)
		sourceAddress=packet->systemAddress;
	SystemAddress targetAddress;
	incomingBs.Read(targetAddress);
	RakNetTimeMS timeoutOnNoDataMS;
	incomingBs.Read(timeoutOnNoDataMS);
	bool hasServerSelectionBitstream;
	incomingBs.Read(hasServerSelectionBitstream);
	ForwardingRequest *fw = RakNet::OP_NEW<ForwardingRequest>(__FILE__,__LINE__);
	if (hasServerSelectionBitstream)
		incomingBs.Read(&(fw->serverSelectionBitstream));


	RakNet::BitStream outgoingBs;
	SenderAndTargetAddress sata;
	sata.senderClientAddress=sourceAddress;
	sata.targetClientAddress=targetAddress;
	DataStructures::DefaultIndexType insertionIndex;
	insertionIndex = forwardingRequestList.GetInsertionIndex(sata);
	if (insertionIndex==-1)
	{
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_IN_PROGRESS);
		outgoingBs.Write(sata.senderClientAddress);
		outgoingBs.Write(targetAddress);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		RakNet::OP_DELETE(fw, __FILE__, __LINE__);
		return;
	}

	if (serverList.GetSize()==0)
	{
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_NO_SERVERS_ONLINE);
		outgoingBs.Write(sata.senderClientAddress);
		outgoingBs.Write(targetAddress);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		RakNet::OP_DELETE(fw, __FILE__, __LINE__);
		return;
	}

	fw->sata=sata;
	fw->timeoutOnNoDataMS=timeoutOnNoDataMS;
	fw->requestingAddress=packet->systemAddress;

	if (serverList.GetSize()>1)
	{
		DataStructures::DefaultIndexType bestServerIndex = GetBestServer(serverList, sourceAddress, targetAddress, &fw->serverSelectionBitstream);
		if (bestServerIndex<0 || bestServerIndex>serverList.GetSize())
		{
			RakAssert("GetBestServer() returned invalid index" && false);
			bestServerIndex=0;
		}
		fw->currentlyAttemptedServerAddress=serverList[bestServerIndex];
		fw->remainingServersToTry=serverList;
		fw->remainingServersToTry.RemoveAtIndex(bestServerIndex);
	}
	else
	{
		fw->currentlyAttemptedServerAddress=serverList[0];
	}

	forwardingRequestList.InsertAtIndex(fw, insertionIndex);

	SendForwardingRequest(sourceAddress, targetAddress, fw->currentlyAttemptedServerAddress, timeoutOnNoDataMS);
}
void UDPProxyCoordinator::SendForwardingRequest(SystemAddress sourceAddress, SystemAddress targetAddress, SystemAddress serverAddress, RakNetTimeMS timeoutOnNoDataMS)
{
	RakNet::BitStream outgoingBs;
	// Send request to desired server
	outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
	outgoingBs.Write((MessageID)ID_UDP_PROXY_FORWARDING_REQUEST_FROM_COORDINATOR_TO_SERVER);
	outgoingBs.Write(sourceAddress);
	outgoingBs.Write(targetAddress);
	outgoingBs.Write(timeoutOnNoDataMS);
	rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
}
DataStructures::DefaultIndexType UDPProxyCoordinator::GetBestServer(const DataStructures::Multilist<ML_UNORDERED_LIST, SystemAddress> &potentialServers, SystemAddress senderAddress, SystemAddress targetAddress, BitStream *serverSelectionBitstream)
{
	(void) senderAddress;
	(void) targetAddress;
	(void) serverSelectionBitstream;

	// Just pick randomly by default
	return randomMT() % potentialServers.GetSize();
}
void UDPProxyCoordinator::OnLoginRequestFromServerToCoordinator(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(2);
	RakNet::RakString password;
	incomingBs.Read(password);
	RakNet::BitStream outgoingBs;

	if (remoteLoginPassword.IsEmpty())
	{
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_NO_PASSWORD_SET_FROM_COORDINATOR_TO_SERVER);
		outgoingBs.Write(password);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		return;
	}

	if (remoteLoginPassword!=password)
	{
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_WRONG_PASSWORD_FROM_COORDINATOR_TO_SERVER);
		outgoingBs.Write(password);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		return;
	}

	DataStructures::DefaultIndexType insertionIndex;
	insertionIndex=serverList.GetInsertionIndex(packet->systemAddress);
	if (insertionIndex==-1)
	{
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_ALREADY_LOGGED_IN_FROM_COORDINATOR_TO_SERVER);
		outgoingBs.Write(password);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		return;
	}
	serverList.InsertAtIndex(packet->systemAddress, insertionIndex);
	outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
	outgoingBs.Write((MessageID)ID_UDP_PROXY_LOGIN_SUCCESS_FROM_COORDINATOR_TO_SERVER);
	outgoingBs.Write(password);
	rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}
void UDPProxyCoordinator::OnForwardingReplyFromServerToCoordinator(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(2);
	SenderAndTargetAddress sata;
	incomingBs.Read(sata.senderClientAddress);
	incomingBs.Read(sata.targetClientAddress);
	DataStructures::DefaultIndexType index = forwardingRequestList.GetIndexOf(sata);
	if (index==-1)
	{
		// The guy disconnected before the request finished
		return;
	}
	ForwardingRequest *fw = forwardingRequestList[index];

	bool success;
	incomingBs.Read(success);

	RakNet::BitStream outgoingBs;
	if (success)
	{
		char serverIP[64];
		packet->systemAddress.ToString(false,serverIP);
		unsigned short srcToDestPort;
		unsigned short destToSourcePort;
		incomingBs.Read(srcToDestPort);
		incomingBs.Read(destToSourcePort);

		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_FORWARDING_SUCCEEDED);
		outgoingBs.Write(sata.senderClientAddress);
		outgoingBs.Write(sata.targetClientAddress);
		outgoingBs.Write(RakNet::RakString(serverIP));
		outgoingBs.Write(srcToDestPort);
		outgoingBs.Write(destToSourcePort);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, fw->requestingAddress, false);

		outgoingBs.Reset();
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_FORWARDING_NOTIFICATION);
		outgoingBs.Write(sata.senderClientAddress);
		outgoingBs.Write(sata.targetClientAddress);
		outgoingBs.Write(RakNet::RakString(serverIP));
		outgoingBs.Write(srcToDestPort);
		outgoingBs.Write(destToSourcePort);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, sata.targetClientAddress, false);

		return;
	}

	// Try next server
	TryNextServer(sata, fw);
}
void UDPProxyCoordinator::TryNextServer(SenderAndTargetAddress sata, ForwardingRequest *fw)
{
	if (fw->remainingServersToTry.GetSize()==0)
	{
		RakNet::BitStream outgoingBs;
		outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
		outgoingBs.Write((MessageID)ID_UDP_PROXY_ALL_SERVERS_BUSY);
		outgoingBs.Write(sata.senderClientAddress);
		outgoingBs.Write(sata.targetClientAddress);
		rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, fw->requestingAddress, false);
		return;
	}

	DataStructures::DefaultIndexType bestServerIndex = GetBestServer(fw->remainingServersToTry, sata.senderClientAddress,sata.targetClientAddress, & fw->serverSelectionBitstream);
	if (bestServerIndex<0 || bestServerIndex>fw->remainingServersToTry.GetSize())
	{
		RakAssert("GetBestServer() returned invalid index" && false);
		bestServerIndex=0;
	}
	fw->currentlyAttemptedServerAddress=fw->remainingServersToTry[bestServerIndex];
	fw->remainingServersToTry.RemoveAtIndex(bestServerIndex);

	SendForwardingRequest(sata.senderClientAddress, sata.targetClientAddress, fw->currentlyAttemptedServerAddress, fw->timeoutOnNoDataMS);
}
void UDPProxyCoordinator::Clear(void)
{
	serverList.Clear(true, __FILE__, __LINE__);
	forwardingRequestList.ClearPointers(true, __FILE__, __LINE__);
}
