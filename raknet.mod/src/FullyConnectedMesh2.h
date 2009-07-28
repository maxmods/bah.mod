/// \file FullyConnectedMesh2.h
/// \brief Fully connected mesh plugin, revision 2.  
/// \details This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __FULLY_CONNECTED_MESH_2_H
#define __FULLY_CONNECTED_MESH_2_H

class RakPeerInterface;
#include "PluginInterface2.h"
#include "RakMemoryOverride.h"
#include "DS_Multilist.h"
#include "NativeTypes.h"
#include "DS_List.h"

typedef int64_t FCM2Guid;

/// \brief Fully connected mesh plugin, revision 2
/// \details This will connect RakPeer to all connecting peers, and all peers the connecting peer knows about.<BR>
/// It will also calculate which system has been running longest, to find out who should be host, if you need one system to act as a host
/// \pre You must also install the ConnectionGraph2 plugin
/// \ingroup FULLY_CONNECTED_MESH_GROUP
class RAK_DLL_EXPORT FullyConnectedMesh2 : public PluginInterface2
{
public:
	FullyConnectedMesh2();
	virtual ~FullyConnectedMesh2();

	/// \brief The connected host is whichever system we are connected to that has been running the longest.
	/// \details Will return UNASSIGNED_RAKNET_GUID if we are not connected to anyone, or if we are connected and are calculating the host
	/// If includeCalculating is true, will return the estimated calculated host as long as the calculation is nearly complete
	/// includeCalculating should be true if you are taking action based on another system becoming host, because not all host calculations may compelte at the exact same time
	/// \return System address of whichever system is host. 
	RakNetGUID GetConnectedHost(void) const;
	SystemAddress GetConnectedHostAddr(void) const;

	/// \return System address of whichever system is host. Always returns something, even though it may be our own system.
	RakNetGUID GetHostSystem(void) const;

	/// \return If our system is host
	bool IsHostSystem(void) const;

	/// \param[in] includeCalculating If true, and we are currently calculating a new host, return the new host if the calculation is nearly complete
	/// \return If our system is host
	bool IsConnectedHost(void) const;

	/// \brief Automatically add new connections to the fully connected mesh.
	/// \details Defaults to true.
	/// \param[in] b As stated
	void SetAutoparticipateConnections(bool b);

	/// \brief if SetAutoparticipateConnections() is called with false, then you need to use AddParticipant before these systems will be added to the mesh 
	/// \param[in] participant The new participant
	void AddParticipant(SystemAddress participant);

	/// \internal
	RakNetTimeUS GetElapsedRuntime(void);

	/// \internal
	virtual PluginReceiveResult OnReceive(Packet *packet);
	/// \internal
	virtual void OnStartup(void);
	/// \internal
	virtual void OnAttach(void);
	/// \internal
	virtual void OnShutdown(void);
	/// \internal
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	/// \internal
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);

	/// \internal
	struct FCM2Participant
	{
		FCM2Participant() {}
		FCM2Participant(const FCM2Guid &_fcm2Guid, const RakNetGUID &_rakNetGuid, const SystemAddress &_systemAddress) : fcm2Guid(_fcm2Guid), rakNetGuid(_rakNetGuid), systemAddress(_systemAddress) {}
		FCM2Guid fcm2Guid;
		RakNetGUID rakNetGuid;
		SystemAddress systemAddress;
	};

protected:
	void Clear(void);
	void PushNewHost(const SystemAddress &sa, const RakNetGUID &guid);
	void SendOurFCMGuid(SystemAddress addr);
	void SendFCMGuidRequest(SystemAddress addr);
	void SendFCMGuidResponse(SystemAddress addr, unsigned int responseAssignedConnectionCount, unsigned int responseTotalConnectionCount);
	void OnRequestFCMGuid(Packet *packet);
	void OnRespondFCMGuid(Packet *packet);
	void OnInformFCMGuid(Packet *packet);
	void AssignOurFCMGuid(unsigned int responseAssignedConnectionCount);
	void CalculateHost(SystemAddress *systemAddress, RakNetGUID *rakNetGuid, FCM2Guid *fcm2Guid);

	// Used to track how long RakNet has been running. This is so we know who has been running longest
	RakNetTimeUS startupTime;

	// Option for SetAutoparticipateConnections
	bool autoParticipateConnections;

	// totalConnectionCount is roughly maintained across all systems, and increments by 1 each time a new system connects to the mesh
	// It is always kept at the highest known value
	// It is used as the high 4 bytes for new FCMGuids. This causes newer values of FCM2Guid to be higher than lower values. The lowest value is the host.
	unsigned int totalConnectionCount;

	// Our own ourFCMGuid. Starts at unassigned (0). Assigned once we send ID_FCM2_REQUEST_FCMGUID and get back ID_FCM2_RESPOND_FCMGUID
	FCM2Guid ourFCMGuid;

	// Only the host replies to ID_FCM2_REQUEST_FCMGUID
	// Otherwise, the host would rapidly change as new FCM2Guids are downloaded, and if the host was temporarily unresponsive we'd have the wrong host
	// This tracks who we send ID_FCM2_REQUEST_FCMGUID to so that if that system disconnects before we know who the host is, we resend the request
	// This list is cleared and no longer used once we know our own guid
	DataStructures::Multilist<ML_UNORDERED_LIST,SystemAddress> guidRequestRetryList;

	/// List of systems we know the FCM2Guid for
	DataStructures::List<FCM2Participant> participantList;

	// Optimization: Store last calculated host in these variables.
	SystemAddress hostSystemAddress;
	RakNetGUID hostRakNetGuid;
	FCM2Guid hostFCM2Guid;
};

#endif

