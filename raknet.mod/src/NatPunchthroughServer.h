/// \file
/// \brief Contains the NAT-punchthrough plugin for the server.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __NAT_PUNCHTHROUGH_SERVER_H
#define __NAT_PUNCHTHROUGH_SERVER_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "SocketIncludes.h"
#include "DS_OrderedList.h"

class RakPeerInterface;
struct Packet;

/// \defgroup NAT_PUNCHTHROUGH_GROUP NatPunchthrough
/// \ingroup PLUGINS_GROUP

/// Maintain connection to NatPunchthroughServer to process incoming connection attempts through NatPunchthroughClient
/// Server maintains two sockets clients can connect to so as to estimate the next port choice
/// Server tells other client about port estimate, current public port to the server, and a time to start connection attempts
/// \ingroup NAT_PUNCHTHROUGH_GROUP
class RAK_DLL_EXPORT NatPunchthroughServer : public PluginInterface2
{
public:
	/// Constructor
	NatPunchthroughServer();

	/// Destructor
	virtual ~NatPunchthroughServer();

	/// \internal For plugin handling
	virtual void Update(void);

	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);

	/// \internal For plugin handling
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);

	// Each connected user has a ready state. Ready means ready for nat punchthrough.
	struct User;
	struct ConnectionAttempt
	{
		ConnectionAttempt() {sender=0; recipient=0; startTime=0; inProgress=false; attemptPhase=NAT_ATTEMPT_PHASE_NOT_STARTED;}
		User *sender, *recipient;
		RakNetTime startTime;
		bool inProgress;
		enum
		{
			NAT_ATTEMPT_PHASE_NOT_STARTED,
			NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS,
		} attemptPhase;
	};
	struct User
	{
		RakNetGUID guid;
		SystemAddress systemAddress;
		unsigned short mostRecentPort;
		bool isReady;

		DataStructures::List<ConnectionAttempt *> connectionAttempts;
		bool HasConnectionAttemptToUser(User *user);
		void DerefConnectionAttempt(ConnectionAttempt *ca);
		void DeleteConnectionAttempt(ConnectionAttempt *ca);
	};
	RakNetTime lastUpdate;
	static int NatPunchthroughUserComp( const RakNetGUID &key, User * const &data );
protected:
	void OnNATPunchthroughRequest(Packet *packet);
	DataStructures::OrderedList<RakNetGUID, User*, NatPunchthroughServer::NatPunchthroughUserComp> users;

	void OnGetMostRecentPort(Packet *packet);
	void OnClientReady(Packet *packet);

	void SendTimestamps(void);
	void StartPendingPunchthrough(void);
	void StartPunchthroughForUser(User*user);

};

#endif
