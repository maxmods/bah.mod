/// \file
/// \brief Router2 plugin. Allows you to connect to a system by routing packets through another system that is connected to both you and the destination. Useful for getting around NATs.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __ROUTER_2_PLUGIN_H
#define __ROUTER_2_PLUGIN_H

class RakPeerInterface;
#include "RakNetTypes.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "Export.h"
#include "UDPForwarder.h"
#include "MessageIdentifiers.h"

namespace RakNet
{

/// \defgroup ROUTER_2_GROUP Router2
/// \brief Part of the NAT punchthrough solution, allowing you to connect to systems by routing through a shared connection.
/// \details
/// \ingroup PLUGINS_GROUP

/// \ingroup ROUTER_2_GROUP
/// \brief Class interface for the Router2 system
/// \details
class RAK_DLL_EXPORT Router2 : public PluginInterface2
{
public:
	Router2();
	virtual ~Router2();

	/// Query all connected systems to connect through them to a third system.
	/// System will return ID_ROUTER_2_FORWARDING_NO_PATH if unable to connect.
	/// Else you will get ID_CONNECTION_REQUEST_ACCEPTED with Packet::guid equal to endpointGuid
	/// \note The SystemAddress for a connection should not be used - always use RakNetGuid as the address can change at any time.
	/// When the address changes, you will get ID_ROUTER_2_REROUTED
	void Connect(RakNetGUID endpointGuid);

	/// Set the maximum number of bidirectional connections this system will support
	/// Defaults to 0
	void SetMaximumForwardingRequests(int max);

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void Update(void);
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnFailedConnectionAttempt(SystemAddress systemAddress, PI2_FailedConnectionAttemptReason failedConnectionAttemptReason);
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);
	virtual void OnShutdown(void);


	enum Router2RequestStates
	{
		R2RS_REQUEST_STATE_QUERY_FORWARDING,
		REQUEST_STATE_REQUEST_FORWARDING,
	};

	struct ConnectionRequestSystem
	{
		RakNetGUID guid;
		int pingToEndpoint;
		unsigned short usedForwardingEntries;
	};

	struct ConnnectRequest
	{
		ConnnectRequest();
		~ConnnectRequest();

		DataStructures::List<ConnectionRequestSystem> connectionRequestSystems;
		Router2RequestStates requestState;
		RakNetTimeMS pingTimeout;
		RakNetGUID endpointGuid;
		RakNetGUID lastRequestedForwardingSystem;
		bool returnConnectionLostOnFailure;
		unsigned int GetGuidIndex(RakNetGUID guid);
	};

	unsigned int GetConnectionRequestIndex(RakNetGUID endpointGuid);

	struct MiniPunchRequest
	{
		RakNetGUID endpointGuid;
		SystemAddress endpointAddress;
		bool gotReplyFromEndpoint;
		RakNetGUID sourceGuid;
		SystemAddress sourceAddress;
		bool gotReplyFromSource;
		RakNetTimeMS timeout;
		RakNetTimeMS nextAction;
		unsigned short srcToDestPort;
		unsigned short destToSourcePort;
		SOCKET srcToDestSocket;
		SOCKET destToSourceSocket;
	};

	struct ForwardedConnection
	{
		RakNetGUID endpointGuid;
		RakNetGUID intermediaryGuid;
		SystemAddress intermediaryAddress;
		bool returnConnectionLostOnFailure;
	};

protected:

	bool UpdateForwarding(unsigned int connectionRequestIndex);
	void RemoveConnectionRequest(unsigned int connectionRequestIndex);
	void RequestForwarding(unsigned int connectionRequestIndex);
	void OnQueryForwarding(Packet *packet);
	void OnQueryForwardingReply(Packet *packet);
	void OnRequestForwarding(Packet *packet);
	void OnMiniPunchReply(Packet *packet);
	void OnForwardingSuccess(Packet *packet);
	int GetLargestPingAmongConnectedSystems(void) const;
	void ReturnToUser(MessageID messageId, RakNetGUID endpointGuid, SystemAddress systemAddress);
	bool ConnectInternal(RakNetGUID endpointGuid, bool returnConnectionLostOnFailure);

	UDPForwarder *udpForwarder;
	int maximumForwardingRequests;
	DataStructures::List<ConnnectRequest*> connectionRequests;
	DataStructures::List<MiniPunchRequest> miniPunchesInProgress;
	DataStructures::List<ForwardedConnection> forwardedConnectionList;

	void ClearConnectionRequests(void);
	void ClearMinipunches(void);
	void ClearForwardedConnections(void);
	void ClearAll(void);
	int ReturnFailureOnCannotForward(RakNetGUID sourceGuid, RakNetGUID endpointGuid);
	void SendFailureOnCannotForward(RakNetGUID sourceGuid, RakNetGUID endpointGuid);
	void SendForwardingSuccess(RakNetGUID sourceGuid, RakNetGUID endpointGuid);
	void SendOOBFromRakNetPort(OutOfBandIdentifiers oob, BitStream *extraData, SystemAddress sa);
	void SendOOBFromSpecifiedSocket(OutOfBandIdentifiers oob, SystemAddress sa, SOCKET socket);
	void SendOOBMessages(MiniPunchRequest *mpr);
};

}

#endif
