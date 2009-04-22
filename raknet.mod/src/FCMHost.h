/// \file
/// \brief In a fully connected mesh, determines which system can act as a host. This generally is the system that has been connected to the mesh the longest.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


/*

On events:
- HOST DROPS
- ADD OR DROP PARTICIPANT WHEN NO HOST
- CHANGE PRIORITY WHEN NO HOST:

1. Clear all state information, except participant list
2. System A: (Host drops or add participant only) Send ID_FCM_HOST_QUERY to all remote participants. Set state to HOST_QUERYING for each remote participant
3. System A: If all systems are at or become state HOST_UNKNOWN (due to packet or participant drop), go to 4.
4. System A: For all remote participants, (referred to as system B), send ID_PARTICIPANT_LIST_UPDATE, priorityA, participantListA. set state of B to PARTICIPANT_LIST_TRANSMITTED.
5. System B: On ID_PARTICIPANT_LIST_UPDATE, Store priorityA, participantListA, state PARTICIPANT_LIST_RECEIVED for A
6. System B: If due to ID_PARTICIPANT_LIST_UPDATE, participant add, or participant drop, all stored participant lists now match our current list goto 7.
7. System B: Send ID_PARTICIPANT_LIST_MATCHED to all remote participants. Include priorityB, participantListB
8. System A: On ID_PARTICIPANT_LIST_MATCHED, If participantListB==participantListA, and priorityA==priorityAFromPacket, set state PARTICIPANT_LIST_MATCHED
9. System A: If all systems are state PARTICIPANT_LIST_MATCHED, set host by highest priority, lowest guid.
10. If you are now host, send ID_HOST,guid,priority to all participants.
11. If got ID_HOST, but think the host is different, then send ID_HOST,guid,priority to all systems. Set host to higher priority host.

ADD OR DROP PARTICIPANT WHEN WE ARE HOST

1. Send to participant ID_HOST,guid,priority

*/

#ifndef __FCM_HOST_H
#define __FCM_HOST_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "DS_List.h"
#include "DS_Map.h"

class RakPeerInterface;
struct Packet;

typedef unsigned char FCMHostGroupID;

/*
On events:
- HOST DROPS
- ADD OR DROP PARTICIPANT WHEN NO HOST
- CHANGE PRIORITY WHEN NO HOST:

1. Clear all state information, except participant list
2. System A: (Host drops or add participant only) Send ID_HOST_QUERY to all remote participants. Set state to QUERYING_HOST for each remote participant
3. System A: If all systems are at or become state HOST_UNKNOWN (due to packet or participant drop), go to 4.
4. System A: For all remote participants, (referred to as system B), send ID_FCM_HOST_PARTICIPANT_LIST_TRANSMIT, priorityA, participantListA. set state of B to PARTICIPANT_LIST_TRANSMITTED.
5. System B: On  ID_FCM_HOST_PARTICIPANT_LIST_TRANSMIT, Store priorityA, participantListA, state PARTICIPANT_LIST_RECEIVED for A
6. System B: If due to ID_PARTICIPANT_LIST_UPDATE, participant add, or participant drop, all stored participant lists now match our current list goto 7.
7. System B: Send ID_PARTICIPANT_LIST_MATCHED to all remote participants. Include priorityB, participantListB
8. System A: On ID_PARTICIPANT_LIST_MATCHED, If participantListB==participantListA, and priorityA==priorityAFromPacket, set state PARTICIPANT_LIST_MATCHED
9. System A: If all systems are state PARTICIPANT_LIST_MATCHED, set host by highest priority, lowest guid.
10. If you are now host, send ID_HOST,guid,priority to all participants.
11. If got ID_HOST, but think the host is different, then send ID_HOST,guid,priority to all systems. Set host to higher priority host.

ADD PARTICIPANT WHEN WE ARE HOST

1. Send to participant ID_HOST,guid,priority
*/

/// \defgroup FCM_HOST_GROUP FullyConnectedMeshHost
/// \ingroup PLUGINS_GROUP

/// \brief In a fully connected mesh of peers, allows all peers to agree that another peer is hot.
///
/// In peer to peer games it is often still useful to have one system act as a host / server
/// For example, if the game has AI, only the host / server should control the AI

enum FCMHostState
{
	/// No participants, so we are the default host
	FCMHS_SOLO,
	/// Querying to see if another system is the host
	FCMHS_QUERYING_FOR_HOST,
	/// Calculating a new host
	FCMHS_CALCULATING_NEW_HOST,
	/// This system is the host
	FCMHS_WE_ARE_HOST,
	/// Another system is the host
	FCMHS_REMOTE_SYSTEM_IS_HOST
};


/// \pre A fully connected mesh is required among the participants - e.g. all participants must eventually connect to all other participants. Otherwise the algorithm will not return the same host among all systems.
/// \note ID_FCM_HOST_CHANGED is returned to RakPeerInterface::Receive() every time the host changes.
/// \ingroup FCM_HOST_GROUP
class RAK_DLL_EXPORT FCMHost : public PluginInterface2
{
public:
	/// Constructor
	FCMHost();

	/// Destructor
	virtual ~FCMHost();

	/// Add a connected system to the list of systems to manage host tracking
	/// \param[in] guid The system to add
	/// \param[in] groupId Which group to assign this system to. Groups are used if you want to manage more than one list at a time. Use 0 if you don't care.
	void AddParticipant(RakNetGUID guid, FCMHostGroupID groupId=0);

	/// Remove a participant added with AddParticipant(), or automatically added with SetAutoAddNewConnections(), from all groups
	/// \param[in] guid The system to remove
	void RemoveParticipantFromAllGroups(RakNetGUID guid);

	/// Remove a participant added with AddParticipant(), or automatically added with SetAutoAddNewConnections()
	/// \param[in] guid The system to remove
	/// \param[in] groupId Which group to remove this system from. Groups are used if you want to manage more than one list at a time. Use 0 if you don't care.
	/// \return True if done, false if not
	bool RemoveParticipant(RakNetGUID guid, FCMHostGroupID groupId=0);

	/// If set to true, all subsequent new connections will be added to \a groupId.
	/// Defaults to false
	/// \param[in] autoAdd If true, removes the need to call AddParticipant() on ID_NEW_INCOMING_CONNECTION or ID_CONNECTION_REQUEST_ACCEPTED
	/// Which group to automatically add systems to. Unused parameter if \a autoAdd is false
	void SetAutoAddNewConnections(bool autoAdd, FCMHostGroupID groupId=0);

	/// Get the current state of the system for determing the host
	/// \param[in] groupId Which group we are referring to.
	FCMHostState GetHostState(FCMHostGroupID groupId=0);

	/// Returns if we are connected to other systems, and one of those systems (possibly ourselves) is a host
	/// If we are not connected to other systems, then our own system is always the host, and in a sense the host is undetermined
	/// This function could also be named HasValidHost, or IsHostDetermined
	/// \param[in] groupId Which group we are referring to.
	/// \return true If there is a networked host
	bool HasHost(FCMHostGroupID groupId=0);

	/// Get the current host.
	/// Only meaningful when GetHostState is FCMHS_WE_ARE_HOST or FCMHS_REMOTE_SYSTEM_IS_HOST
	/// \param[in] groupId Which group we are referring to.
	/// \return The Guid of the host
	RakNetGUID GetHost(FCMHostGroupID groupId=0);

	/// While calculating the host, priority gives a boost to which system will be host
	/// If two systems have the same priority, the system with the lower GUID will be host.
	/// priority defaults to 0
	/// This won't change the host after the fact, only before adding that system with AddParticipant
	/// \param[in] priority The priority level to set. Just a number for comparison, set to whatever you want.
	/// \param[in] groupId Which group we are referring to.
	void SetHostPriority(int priority, FCMHostGroupID groupId=0);

	/// Get all participants in a group
	void GetParticipants(DataStructures::List<RakNetGUID> &participants, FCMHostGroupID groupId=0);

	/// Reset the system, clearing all data
	void Clear(void);

	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(void);
	/// \internal For plugin handling
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);
protected:

	void OnHostQuery(Packet *packet);
	void OnHostUnknown(Packet *packet);
	void OnHostNotification(Packet *packet);
	void OnHostParticipantListTransmit(Packet *packet);
	void OnHostParticipantListMatch(Packet *packet);
	void NotifyStateChange(FCMHostGroupID groupId, RakNetGUID guid);

	struct Participant
	{
		Participant();
		~Participant();
		enum ParticipantState
		{
			UNDEFINED,
			QUERYING_HOST,
			HOST_UNKNOWN,
			PARTICIPANT_LIST_TRANSMITTED,
			PARTICIPANT_LIST_RECEIVED,
			PARTICIPANT_LIST_MATCHED,
			IS_HOST
		} participantState;

		int remotePriority;
		RakNetGUID guid;
		// Systems this remote system knows about
		DataStructures::List<RakNetGUID> remoteGuids;

		void SendHostQuery(FCMHostGroupID groupId, RakPeerInterface *rakPeer);
	};

	struct FCMGroup
	{
		FCMGroup();
		~FCMGroup();

		DataStructures::List<Participant*> participants;
		RakNetGUID host;
		int hostPriority;

		void WriteParticipantGUIDsToBitstream(RakNet::BitStream *bs);
		void ReadParticipantGUIDsFromBitstream(RakNet::BitStream *bs, DataStructures::List<RakNetGUID> &guids);
		bool AllParticipantsAreAtState(Participant::ParticipantState ps);
		bool AreAnyParticipantsAtState(Participant::ParticipantState ps);
		void SendParticipantList(FCMHostGroupID groupId, RakPeerInterface *rakPeer, int localPriority);
		void SetHost(RakNetGUID guid, int priority);
		Participant* GetParticipantByGuid(RakNetGUID guid);
		// If our participant list now matches a remote participant list, set them to PARTICIPANT_LIST_REPLIED and send our own list
		void ReplyToParticipantListReceived(FCMHostGroupID groupId, RakPeerInterface *rakPeer, int localPriority);
		bool ParticipantListsMatch(DataStructures::List<RakNetGUID> &otherParticipantGuids, RakNetGUID excludeThisGuid, RakNetGUID excludeThisGuid2);
	};


	void OnHostDrop(unsigned int groupListIndex, RakNetGUID guid);
	void ClearGroupList(void);
	void SendHostNotification(FCMHostGroupID groupId, int localPriority, SystemAddress systemAddress);
	Participant* AddParticipantToGroup(FCMGroup* group,RakNetGUID guid);
	bool HasHost(unsigned int groupListIndex);
	int GetLocalPriority(FCMHostGroupID groupId);
	
	bool autoAddConnections;
	FCMHostGroupID autoAddConnectionsTargetGroup;

	DataStructures::Map<FCMHostGroupID, FCMGroup* > groupList;

	DataStructures::Map<FCMHostGroupID, int > localPriorityList;
};

#endif
