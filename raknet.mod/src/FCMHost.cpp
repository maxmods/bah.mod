#include "FCMHost.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakPeerInterface.h"


FCMHost::FCMHost()
{
	autoAddConnections=false;
	autoAddConnectionsTargetGroup=0;
	DataStructures::Map<FCMHostGroupID, FCMGroup* >::IMPLEMENT_DEFAULT_COMPARISON();
	DataStructures::Map<FCMHostGroupID, int >::IMPLEMENT_DEFAULT_COMPARISON();
}
FCMHost::~FCMHost()
{
	Clear();
}
void FCMHost::AddParticipant(RakNetGUID guid, FCMHostGroupID groupId)
{
	FCMGroup *fcmGroup;
	Participant *participant;
	RakNetGUID myGuid = rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
	if (groupList.Has(groupId)==false)
	{
		// Create new group
		// Send ID_HOST_QUERY to new group member
		fcmGroup = RakNet::OP_NEW<FCMGroup>(__FILE__, __LINE__);
		groupList.SetNew(groupId,fcmGroup);

		participant=AddParticipantToGroup(fcmGroup,guid);
		participant->SendHostQuery(groupId, rakPeerInterface);
	}
	else
	{
		FCMHostState hostState = GetHostState(groupId);
		unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
		fcmGroup=groupList[groupListIndex];
		if (fcmGroup->GetParticipantByGuid(guid))
			return;
		participant = AddParticipantToGroup(fcmGroup,guid);
		if (participant==0)
			return;
		if (hostState==FCMHS_WE_ARE_HOST)
		{
			SendHostNotification(groupId, GetLocalPriority(groupId), rakPeerInterface->GetSystemAddressFromGuid(participant->guid));
		}
		else if (hostState==FCMHS_CALCULATING_NEW_HOST)
		{
			// Adding someone during the host calculation process restarts it
			fcmGroup->SendParticipantList(groupId, rakPeerInterface, GetLocalPriority(groupId));

			// If all participants list now matches all remote participant list, set them to PARTICIPANT_LIST_REPLIED and send our own list
			fcmGroup->ReplyToParticipantListReceived(groupId, rakPeerInterface, GetLocalPriority(groupId));
		}
		else
		{
			participant->SendHostQuery(groupId, rakPeerInterface);
		}
	}
}
void FCMHost::RemoveParticipantFromAllGroups(RakNetGUID guid)
{
	FCMHostGroupID groupId;
	unsigned int i=0;
	while ( i < groupList.Size())
	{
		groupId=groupList.GetKeyAtIndex(i);
		if (RemoveParticipant(guid, groupId)==false)
			i++;
	}
}
bool FCMHost::RemoveParticipant(RakNetGUID guid, FCMHostGroupID groupId)
{
	FCMGroup *fcmGroup;
	unsigned int groupListIndex, participantIndex;
	unsigned int i;
	Participant *participant;
	if (groupList.Has(groupId)==false)
	{
		// Unknown participant since unknown group
		return false;
	}
	groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];
	for (participantIndex=0; participantIndex<groupList[groupListIndex]->participants.Size(); participantIndex++)
	{
		if (groupList[groupListIndex]->participants[participantIndex]->guid==guid)
			break;
	}
	

	if (participantIndex==groupList[groupListIndex]->participants.Size())
	{
		// unknown participant
		return false;
	}
	// Remove this participant from the list
	RakNet::OP_DELETE(groupList[groupListIndex]->participants[participantIndex], __FILE__, __LINE__);
	groupList[groupListIndex]->participants.RemoveAtIndexFast(participantIndex);

	if (groupList[groupListIndex]->participants.Size()==0)
	{
		RakNet::OP_DELETE(groupList[groupListIndex], __FILE__, __LINE__);
		groupList.RemoveAtIndex(groupListIndex);
		// Last member of a group has dropped
		return true;
	}

	if (fcmGroup->host==guid)
	{
		// The host has dropped
		// Restart host calculation process for all systems that are not already doing so
		for (i=0; i < fcmGroup->participants.Size(); i++)
		{
			participant=fcmGroup->participants[i];
			if (participant->participantState!=FCMHost::Participant::QUERYING_HOST)
			{
				participant->SendHostQuery(groupId, rakPeerInterface);

				// Other systems will reply either with the host, or with HOST_UNKNOWN
			}			
		}
	}
	else if (fcmGroup->host==UNASSIGNED_RAKNET_GUID)
	{
		// We are either waiting for replies from QUERYING_HOST to be HOST_UNKNOWN, or are in progress calculating the host
		// Start/restart the host calculation process as long as we are no longer waiting for QUERYING_HOST
		if (fcmGroup->AreAnyParticipantsAtState(FCMHost::Participant::QUERYING_HOST)==false)
		{
			fcmGroup->SendParticipantList(groupId, rakPeerInterface, GetLocalPriority(groupId));
		}
	}	
	return true;
}
void FCMHost::SetAutoAddNewConnections(bool autoAdd, FCMHostGroupID groupId)
{
	autoAddConnections=autoAdd;
	autoAddConnectionsTargetGroup=groupId;
}
FCMHostState FCMHost::GetHostState(FCMHostGroupID groupId)
{
	if (groupList.Has(groupId)==false)
		return FCMHS_SOLO;
	unsigned int index = groupList.GetIndexAtKey(groupId);
	FCMGroup *group = groupList[index];

	if (group->host==rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
		return FCMHS_WE_ARE_HOST;
	if (group->host!=UNASSIGNED_RAKNET_GUID)
		return FCMHS_REMOTE_SYSTEM_IS_HOST;
	unsigned int i;
	for (i=0; i < group->participants.Size(); i++)
	{
		if (group->participants[i]->participantState==Participant::QUERYING_HOST)
			return FCMHS_QUERYING_FOR_HOST;
	}
	return FCMHS_CALCULATING_NEW_HOST;
}
bool FCMHost::HasHost(unsigned int groupListIndex)
{
	FCMGroup *group = groupList[groupListIndex];
	return group->host!=UNASSIGNED_RAKNET_GUID;
}
bool FCMHost::HasHost(FCMHostGroupID groupId)
{
	if (groupList.Has(groupId)==false)
		return false;
	unsigned int index = groupList.GetIndexAtKey(groupId);
	return HasHost(index);
}
RakNetGUID FCMHost::GetHost(FCMHostGroupID groupId)
{
	if (groupList.Has(groupId)==false)
		return UNASSIGNED_RAKNET_GUID;
	unsigned int index = groupList.GetIndexAtKey(groupId);
	FCMGroup *group = groupList[index];
	return group->host;
}

void FCMHost::SetHostPriority(int priority, FCMHostGroupID groupId)
{
	localPriorityList.Set(groupId,priority);

	if (groupList.Has(groupId)==false)
		return;
	// Changing the host priority restarts the host calculation process if we are currently calculating the host
	if (GetHostState(groupId)==FCMHS_CALCULATING_NEW_HOST)
	{
		unsigned int index = groupList.GetIndexAtKey(groupId);
		FCMGroup *fcmGroup = groupList[index];
		fcmGroup->SendParticipantList(groupId, rakPeerInterface, GetLocalPriority(groupId));
	}
}
void FCMHost::GetParticipants(DataStructures::List<RakNetGUID> &participants, FCMHostGroupID groupId)
{
	if (groupList.Has(groupId)==false)
		return;
	unsigned int index = groupList.GetIndexAtKey(groupId);
	FCMGroup *group = groupList[index];

	unsigned int i;
	for (i=0; i < group->participants.Size(); i++)
	{
		participants.Push(group->participants[i]->guid);
	}
	return;
}
PluginReceiveResult FCMHost::OnReceive(Packet *packet)
{
	switch (packet->data[0])
	{
	case ID_FCM_HOST_QUERY:
		OnHostQuery(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FCM_HOST_UNKNOWN:
		OnHostUnknown(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FCM_HOST_NOTIFICATION:
		OnHostNotification(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FCM_HOST_PARTICIPANT_LIST_TRANSMIT:
		OnHostParticipantListTransmit(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FCM_HOST_PARTICIPANT_LIST_MATCH:
		OnHostParticipantListMatch(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	
	}

	return RR_CONTINUE_PROCESSING;
}
void FCMHost::OnShutdown(void)
{
	Clear();
}
void FCMHost::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) systemAddress;
	(void) rakNetGUID;
	(void) lostConnectionReason;
	RemoveParticipantFromAllGroups(rakPeerInterface->GetGuidFromSystemAddress(systemAddress));
}
void FCMHost::OnHostQuery(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(1);
	FCMHostGroupID groupId;
	incomingBs.Read(groupId);
	FCMGroup *fcmGroup;
	if (groupList.Has(groupId)==false)
		return;
	unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];

	// If we are host, reply with host info
	if (fcmGroup->host==rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
	{
		SendHostNotification(groupId, GetLocalPriority(groupId), packet->systemAddress);
	}
	else
	{
		RakNet::BitStream bs;
		bs.Write((MessageID)ID_FCM_HOST_UNKNOWN);
		bs.Write(groupId);
		SendUnified(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	}
}
void FCMHost::OnHostUnknown(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(1);
	FCMHostGroupID groupId;
	incomingBs.Read(groupId);
	FCMGroup *fcmGroup;
	if (groupList.Has(groupId)==false)
		return;
	unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];
	Participant *participant = fcmGroup->GetParticipantByGuid(packet->guid);
	if (participant==0)
		return;
	participant->participantState=FCMHost::Participant::HOST_UNKNOWN;

	// If no systems are querying host anymore, then start the host determination process
	if (fcmGroup->AreAnyParticipantsAtState(FCMHost::Participant::QUERYING_HOST)==false)
	{
		fcmGroup->SendParticipantList(groupId, rakPeerInterface, GetLocalPriority(groupId));
	}
}
void FCMHost::OnHostNotification(Packet *packet)
{
	// Packet telling us who the host is
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(1);
	FCMHostGroupID groupId;
	incomingBs.Read(groupId);
	int priority;
	incomingBs.Read(priority);
	FCMGroup *fcmGroup;
	if (groupList.Has(groupId)==false)
		return;
	unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];

	// If we don't have a host, set it
	// If we already have a host, pick the better of the two. It shouldn't happen very often
	// Better hosts have higher priority, or equal priorities and lower guids
	if (fcmGroup->host==UNASSIGNED_RAKNET_GUID ||
		fcmGroup->hostPriority < priority ||
		(fcmGroup->hostPriority==priority && fcmGroup->host > packet->guid))
	{
		fcmGroup->SetHost(packet->guid,priority);
		NotifyStateChange(groupId, packet->guid);
	}
}
void FCMHost::OnHostParticipantListTransmit(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(1);
	FCMHostGroupID groupId;
	incomingBs.Read(groupId);
	FCMGroup *fcmGroup;
	if (groupList.Has(groupId)==false)
		return;
	unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];

	// Store priorityA, participantListA, state PARTICIPANT_LIST_RECEIVED
	FCMHost::Participant* participant = fcmGroup->GetParticipantByGuid(packet->guid);
	if (participant==0)
		return;

	incomingBs.Read(participant->remotePriority);
	fcmGroup->ReadParticipantGUIDsFromBitstream(&incomingBs, participant->remoteGuids);
	participant->participantState=Participant::PARTICIPANT_LIST_RECEIVED;

	// If all participants list now matches all remote participant list, set them to PARTICIPANT_LIST_REPLIED and send our own list
	fcmGroup->ReplyToParticipantListReceived(groupId, rakPeerInterface, GetLocalPriority(groupId));
}
void FCMHost::OnHostParticipantListMatch(Packet *packet)
{
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(1);
	FCMHostGroupID groupId;
	incomingBs.Read(groupId);
	FCMGroup *fcmGroup;
	if (groupList.Has(groupId)==false)
		return;
	unsigned int groupListIndex = groupList.GetIndexAtKey(groupId);
	fcmGroup=groupList[groupListIndex];
	// Store priorityA, participantListA, state PARTICIPANT_LIST_RECEIVED
	FCMHost::Participant* participant = fcmGroup->GetParticipantByGuid(packet->guid);
	if (participant==0)
		return;
	int ourInitialPriority;
	incomingBs.Read(participant->remotePriority);
	incomingBs.Read(ourInitialPriority);

	// Our priority has changed, and the system is recalculating the host
	if (ourInitialPriority!=GetLocalPriority(groupId))
		return;


	RakNetGUID myGuid = rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
	DataStructures::List<RakNetGUID> theirGuids;
	fcmGroup->ReadParticipantGUIDsFromBitstream(&incomingBs, theirGuids);

	if (fcmGroup->ParticipantListsMatch(theirGuids, myGuid, participant->guid))
	{
		participant->participantState=Participant::PARTICIPANT_LIST_MATCHED;

		// If all systems are state PARTICIPANT_LIST_MATCHED, set host by highest priority, lowest guid.
		bool allMatched=true;
		unsigned int i;
		for (i=0; i < fcmGroup->participants.Size(); i++)
		{
			if (fcmGroup->participants[i]->participantState!=Participant::PARTICIPANT_LIST_MATCHED)
			{
				allMatched=false;
				break;
			}
		}
		if (allMatched)
		{
			// Determine host by highest priority, lowest guid.
			Participant *bestParticipant = fcmGroup->participants[0];
			for (i=1; i < fcmGroup->participants.Size(); i++)
			{
				if (fcmGroup->participants[i]->remotePriority > bestParticipant->remotePriority)
					bestParticipant=fcmGroup->participants[i];
				else if (fcmGroup->participants[i]->remotePriority == bestParticipant->remotePriority && 
					fcmGroup->participants[i]->guid < bestParticipant->guid)
					bestParticipant=fcmGroup->participants[i];
			}

			if (bestParticipant->remotePriority < GetLocalPriority(groupId) ||
				(bestParticipant->remotePriority == GetLocalPriority(groupId) && bestParticipant->guid > myGuid))
			{
				// I am host
				fcmGroup->SetHost(myGuid, fcmGroup->hostPriority);

				// Broadcast host determination
				unsigned int j;
				for (j=0; j < fcmGroup->participants.Size(); j++)
					SendHostNotification(groupId, fcmGroup->hostPriority, rakPeerInterface->GetSystemAddressFromGuid(fcmGroup->participants[j]->guid));

				// Tell game
				NotifyStateChange(groupId, packet->guid);
			}
			else
			{
				// bestParticipant is host
				// fcmGroup->SetHost(bestParticipant->guid, bestParticipant->remotePriority);
				// Do nothing, rely on host to tell us
			}
		}
	}
}
void FCMHost::NotifyStateChange(FCMHostGroupID groupId, RakNetGUID guid)
{
	Packet *p = rakPeerInterface->AllocatePacket(sizeof(MessageID)+sizeof(FCMHostGroupID));
	RakNet::BitStream bs2(p->data, sizeof(MessageID)+sizeof(FCMHostGroupID), false);
	bs2.SetWriteOffset(0);
	bs2.Write((MessageID)ID_FCM_HOST_STATE_CHANGE_NOTIFICATION);
	bs2.Write(groupId);
	p->systemAddress=rakPeerInterface->GetSystemAddressFromGuid(guid);
	p->systemIndex=(SystemIndex)-1;
	p->guid=guid;
	// Push at head, to replace the incoming notification message.
	// Otherwise the notification message is effectively moved to the end of the queue
	rakPeerInterface->PushBackPacket(p, true);
}
void FCMHost::Clear(void)
{
	ClearGroupList();
}
void FCMHost::ClearGroupList(void)
{
	unsigned int i;
	for (i=0; i < groupList.Size(); i++)
		RakNet::OP_DELETE(groupList[i], __FILE__, __LINE__);
	groupList.Clear();
}
void FCMHost::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) systemAddress;
	(void) isIncoming;

	if (autoAddConnections)
		AddParticipant(rakNetGUID, autoAddConnectionsTargetGroup);
}
void FCMHost::SendHostNotification(FCMHostGroupID groupId, int localPriority, SystemAddress systemAddress)
{
	// Send to the participant that we are the host
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_FCM_HOST_NOTIFICATION);
	bs.Write(groupId);
	bs.Write(localPriority);
	SendUnified(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

int FCMHost::GetLocalPriority(FCMHostGroupID groupId)
{
	if (localPriorityList.Has(groupId)==false)
		return 0;
	return localPriorityList.Get(groupId);
}
FCMHost::Participant* FCMHost::AddParticipantToGroup(FCMGroup* group,RakNetGUID guid)
{
//	if (rakPeer->GetInternalID(UNASSIGNED_SYSTEM_ADDRESS,0).port==60002)
//		int a=5;
	for (unsigned int i=0; i < group->participants.Size(); i++)
		if (group->participants[i]->guid==guid)
			return 0;

	Participant *participant = RakNet::OP_NEW<Participant>(__FILE__,__LINE__);
	participant->guid=guid;
	group->participants.Push(participant, __FILE__, __LINE__ );
	return participant;
}
FCMHost::Participant::Participant()
{
	participantState=UNDEFINED;
	remotePriority=0;
	guid=UNASSIGNED_RAKNET_GUID;
}
FCMHost::Participant::~Participant()
{

}
void FCMHost::Participant::SendHostQuery(FCMHostGroupID groupId, RakPeerInterface *rakPeer)
{
	if (rakPeer->GetInternalID(UNASSIGNED_SYSTEM_ADDRESS,0).port==60002)
	{
		printf("Setting participantState to QUERYING_HOST for %s\n", rakPeer->GetSystemAddressFromGuid(guid).ToString(true));
	}

	participantState=FCMHost::Participant::QUERYING_HOST;
	// Clear state for this participant
	remoteGuids.Clear();

	// Adding someone while querying for host queries that guy too
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_FCM_HOST_QUERY);
	bs.Write(groupId);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakPeer->GetSystemAddressFromGuid(guid), false);
}
FCMHost::FCMGroup::FCMGroup()
{
	host=UNASSIGNED_RAKNET_GUID;
	hostPriority=0;
}
FCMHost::FCMGroup::~FCMGroup()
{
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
		RakNet::OP_DELETE(participants[i], __FILE__, __LINE__ );
}
void FCMHost::FCMGroup::WriteParticipantGUIDsToBitstream(RakNet::BitStream *bs)
{
	unsigned short count = (unsigned short) participants.Size();
	bs->Write(count);
	for (unsigned int i=0; i < participants.Size(); i++)
		bs->Write(participants[i]->guid);
}
void FCMHost::FCMGroup::ReadParticipantGUIDsFromBitstream(RakNet::BitStream *bs, DataStructures::List<RakNetGUID> &guids)
{
	guids.Clear();
	unsigned short count;
	bs->Read(count);
	Participant participant;
	RakNetGUID guid;
	for (unsigned short i=0; i < count;  i++)
	{
		bs->Read(guid);
		guids.Insert(guid);
	}
}
bool FCMHost::FCMGroup::AllParticipantsAreAtState(FCMHost::Participant::ParticipantState ps)
{
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
		if (participants[i]->participantState!=ps)
			return false;
	return true;
}
bool FCMHost::FCMGroup::AreAnyParticipantsAtState(FCMHost::Participant::ParticipantState ps)
{
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
		if (participants[i]->participantState==ps)
			return true;
	return false;
}

void FCMHost::FCMGroup::SendParticipantList(FCMHostGroupID groupId, RakPeerInterface *rakPeer, int localPriority)
{
	// This restarts the host negotiation process

	Participant *participant;
	unsigned int i;
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_FCM_HOST_PARTICIPANT_LIST_TRANSMIT);
	bs.Write(groupId);
	bs.Write(localPriority);
	WriteParticipantGUIDsToBitstream(&bs);

	// Restart the process by clearing the participants states and transmitting the participant list
	for (i=0; i < participants.Size(); i++)
	{
		participant=participants[i];
		participant->participantState=FCMHost::Participant::PARTICIPANT_LIST_TRANSMITTED;
		participant->remoteGuids.Clear();

		rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakPeer->GetSystemAddressFromGuid(participant->guid), false);
	}
}

void FCMHost::FCMGroup::SetHost(RakNetGUID guid, int priority)
{
	host=guid;
	hostPriority=priority;

	// Fix up participant list - stop all processing and set to final state
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
	{
		if (host!=participants[i]->guid)
			participants[i]->participantState=Participant::PARTICIPANT_LIST_MATCHED;
		else
			participants[i]->participantState=Participant::IS_HOST;
		participants[i]->remoteGuids.Clear();
	}
}
void FCMHost::FCMGroup::ReplyToParticipantListReceived(FCMHostGroupID groupId, RakPeerInterface *rakPeer, int localPriority)
{
	unsigned int i;
	RakNet::BitStream bs;
	Participant *participant;
	for (i=0; i < participants.Size(); i++)
	{
		participant=participants[i];
		if (participant->participantState==Participant::PARTICIPANT_LIST_RECEIVED &&
			ParticipantListsMatch(participant->remoteGuids, rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS), participant->guid))
		{
			// System is a perfect match
		}
		else
		{
			return;
		}
	}

	// All systems are perfect matches. Send ID_FCM_HOST_PARTICIPANT_LIST_MATCH to all systems
	for (i=0; i < participants.Size(); i++)
	{
		participant=participants[i];

		bs.Reset();
		bs.Write((MessageID)ID_FCM_HOST_PARTICIPANT_LIST_MATCH);
		bs.Write(groupId);
		bs.Write(localPriority);
		bs.Write(participant->remotePriority);
		WriteParticipantGUIDsToBitstream(&bs);
		rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, rakPeer->GetSystemAddressFromGuid(participant->guid), false);
	}
}
bool FCMHost::FCMGroup::ParticipantListsMatch(DataStructures::List<RakNetGUID> &otherParticipantGuids, RakNetGUID excludeThisGuid, RakNetGUID excludeThisGuid2)
{
	DataStructures::OrderedList<RakNetGUID, RakNetGUID> myGuidList, theirGuidList;
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
	{
		if (excludeThisGuid!=participants[i]->guid &&
			excludeThisGuid2!=participants[i]->guid)
			myGuidList.Insert(participants[i]->guid,participants[i]->guid, true);
	}
	for (i=0; i < otherParticipantGuids.Size(); i++)
	{
		if (excludeThisGuid!=otherParticipantGuids[i] &&
			excludeThisGuid2!=otherParticipantGuids[i])
			theirGuidList.Insert(otherParticipantGuids[i],otherParticipantGuids[i], true);
	}
	if (myGuidList.Size()!=theirGuidList.Size())
		return false;
	for (i=0; i < myGuidList.Size(); i++)
	{
		if (myGuidList[i]!=theirGuidList[i])
			return false;
	}
	return true;
}
FCMHost::Participant* FCMHost::FCMGroup::GetParticipantByGuid(RakNetGUID guid)
{
	unsigned int i;
	for (i=0; i < participants.Size(); i++)
	{
		if (participants[i]->guid==guid)
			return participants[i];
	}
	return 0;
}
