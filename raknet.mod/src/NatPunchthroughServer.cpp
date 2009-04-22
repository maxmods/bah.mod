#include "NatPunchthroughServer.h"
#include "SocketLayer.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "MTUSize.h"
#include "GetTime.h"

void NatPunchthroughServer::User::DeleteConnectionAttempt(NatPunchthroughServer::ConnectionAttempt *ca)
{
	unsigned int index = connectionAttempts.GetIndexOf(ca);
	if (index!=-1)
	{
		RakNet::OP_DELETE(ca,__FILE__,__LINE__);
		connectionAttempts.RemoveAtIndex(index);
	}
}
void NatPunchthroughServer::User::DerefConnectionAttempt(NatPunchthroughServer::ConnectionAttempt *ca)
{
	unsigned int index = connectionAttempts.GetIndexOf(ca);
	if (index!=-1)
	{
		connectionAttempts.RemoveAtIndex(index);
	}
}
bool NatPunchthroughServer::User::HasConnectionAttemptToUser(User *user)
{
	unsigned int index;
	for (index=0; index < user->connectionAttempts.Size(); index++)
	{
		if (user->connectionAttempts[index]->recipient==user ||
			user->connectionAttempts[index]->sender==user)
			return true;
	}
	return false;
}

int NatPunchthroughServer::NatPunchthroughUserComp( const RakNetGUID &key, User * const &data )
{
	if (key < data->guid)
		return -1;
	if (key > data->guid)
		return 1;
	return 0;
}

NatPunchthroughServer::NatPunchthroughServer()
{
	lastUpdate=0;
}
NatPunchthroughServer::~NatPunchthroughServer()
{
	User *user, *otherUser;
	ConnectionAttempt *connectionAttempt;
	unsigned int j;
	while(users.Size())
	{
		user = users[0];
		for (j=0; j < user->connectionAttempts.Size(); j++)
		{
			connectionAttempt=user->connectionAttempts[j];
			if (connectionAttempt->sender==user)
				otherUser=connectionAttempt->recipient;
			else
				otherUser=connectionAttempt->sender;
			otherUser->DeleteConnectionAttempt(connectionAttempt);
		}
		RakNet::OP_DELETE(user,__FILE__,__LINE__);
		users[0]=users[users.Size()-1];
		users.RemoveAtIndex(users.Size()-1);
	}
}

void NatPunchthroughServer::Update(void)
{
	ConnectionAttempt *connectionAttempt;
	User *user, *recipient;
	unsigned int i,j;
	RakNetTime time = RakNet::GetTime();
	if (time > lastUpdate+250)
	{
		lastUpdate=time;

		for (i=0; i < users.Size(); i++)
		{
			user=users[i];
			for (j=0; j < user->connectionAttempts.Size(); j++)
			{
				connectionAttempt=user->connectionAttempts[j];
				if (connectionAttempt->sender==user)
				{
					if (connectionAttempt->attemptPhase!=ConnectionAttempt::NAT_ATTEMPT_PHASE_NOT_STARTED &&
						time > connectionAttempt->startTime &&
						time - connectionAttempt->startTime > 5000)
					{
						RakNet::BitStream outgoingBs;
						// that other system might not be running the plugin
						outgoingBs.Write((MessageID)ID_NAT_TARGET_UNRESPONSIVE);
						outgoingBs.Write(connectionAttempt->recipient->guid);
						rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,connectionAttempt->sender->systemAddress,false);
						connectionAttempt->sender->isReady=true;
						connectionAttempt->recipient->isReady=true;
						recipient=connectionAttempt->recipient;
						connectionAttempt->sender->DerefConnectionAttempt(connectionAttempt);
						connectionAttempt->recipient->DeleteConnectionAttempt(connectionAttempt);

						StartPunchthroughForUser(user);
						StartPunchthroughForUser(recipient);

						break;
					}
				}
			}
		}
	}
}
PluginReceiveResult NatPunchthroughServer::OnReceive(Packet *packet)
{
	switch (packet->data[0])
	{
	case ID_NAT_PUNCHTHROUGH_REQUEST:
		OnNATPunchthroughRequest(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_NAT_GET_MOST_RECENT_PORT:
		OnGetMostRecentPort(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_NAT_CLIENT_READY:
		OnClientReady(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	return RR_CONTINUE_PROCESSING;
}
void NatPunchthroughServer::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) systemAddress;

	unsigned int i=0;
	bool objectExists;
	i = users.GetIndexFromKey(rakNetGUID, &objectExists);
	if (objectExists)
	{
		RakNet::BitStream outgoingBs;
		DataStructures::List<User *> freedUpInProgressUsers;
		User *user = users[i];
		User *otherUser;
		unsigned int connectionAttemptIndex;
		ConnectionAttempt *connectionAttempt;
		for (connectionAttemptIndex=0; connectionAttemptIndex < user->connectionAttempts.Size(); connectionAttemptIndex++)
		{
			connectionAttempt=user->connectionAttempts[connectionAttemptIndex];
			if (connectionAttempt->recipient==user)
			{
				// Recipient dropped, tell sender
				outgoingBs.Reset();
				otherUser=connectionAttempt->sender;
				outgoingBs.Write((MessageID)ID_NAT_CONNECTION_TO_TARGET_LOST);
				outgoingBs.Write(rakNetGUID);
				rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,otherUser->systemAddress,false);
			}
			else
			{
				// Sender dropped
				otherUser=connectionAttempt->recipient;
			}

			if (connectionAttempt->inProgress)
			{
				otherUser->isReady=true;
				freedUpInProgressUsers.Insert(otherUser);
			}

			otherUser->DeleteConnectionAttempt(connectionAttempt);
		}

		RakNet::OP_DELETE(users[i], __FILE__, __LINE__);
		users.RemoveAtIndex(i);

		for (i=0; i < freedUpInProgressUsers.Size(); i++)
		{
			StartPunchthroughForUser(freedUpInProgressUsers[i]);
		}
	}
}

void NatPunchthroughServer::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) systemAddress;
	(void) isIncoming;

	User *user = RakNet::OP_NEW<User>(__FILE__,__LINE__);
	user->guid=rakNetGUID;
	user->mostRecentPort=0;
	user->systemAddress=systemAddress;
	users.Insert(rakNetGUID, user, true);
}
void NatPunchthroughServer::OnNATPunchthroughRequest(Packet *packet)
{
	RakNet::BitStream outgoingBs;
	RakNet::BitStream incomingBs(packet->data, packet->length, false);
	incomingBs.IgnoreBytes(sizeof(MessageID));
	RakNetGUID recipientGuid, senderGuid;
	incomingBs.Read(recipientGuid);
	senderGuid=packet->guid;
	unsigned int i;
	bool objectExists;
	i = users.GetIndexFromKey(senderGuid, &objectExists);
	RakAssert(objectExists);

	ConnectionAttempt *ca = RakNet::OP_NEW<ConnectionAttempt>(__FILE__,__LINE__);
	ca->sender=users[i];
	i = users.GetIndexFromKey(recipientGuid, &objectExists);
	if (objectExists==false)
	{
		outgoingBs.Write((MessageID)ID_NAT_TARGET_NOT_CONNECTED);
		outgoingBs.Write(recipientGuid);
		rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
		RakNet::OP_DELETE(ca,__FILE__,__LINE__);
		return;
	}
	ca->recipient=users[i];
	if (ca->sender->HasConnectionAttemptToUser(ca->recipient))
	{
		outgoingBs.Write((MessageID)ID_NAT_ALREADY_IN_PROGRESS);
		outgoingBs.Write(recipientGuid);
		rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
		RakNet::OP_DELETE(ca,__FILE__,__LINE__);
		return;
	}

	ca->sender->connectionAttempts.Insert(ca);
	ca->recipient->connectionAttempts.Insert(ca);

	StartPunchthroughForUser(ca->sender);
}
void NatPunchthroughServer::OnClientReady(Packet *packet)
{
	unsigned int i;
	bool objectExists;
	i = users.GetIndexFromKey(packet->guid, &objectExists);
	if (objectExists)
	{
		users[i]->isReady=true;
		StartPunchthroughForUser(users[i]);
	}
}

void NatPunchthroughServer::OnGetMostRecentPort(Packet *packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	unsigned short mostRecentPort;
	bsIn.Read(mostRecentPort);
	unsigned int i,j;
	User *user;
	ConnectionAttempt *connectionAttempt;
	bool objectExists;
	i = users.GetIndexFromKey(packet->guid, &objectExists);
	if (objectExists)
	{
		user=users[i];
		user->mostRecentPort=mostRecentPort;
		RakNetTime time = RakNet::GetTime();

		for (j=0; j < user->connectionAttempts.Size(); j++)
		{
			connectionAttempt=user->connectionAttempts[j];
			if (connectionAttempt->attemptPhase==ConnectionAttempt::NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS &&
				connectionAttempt->sender->mostRecentPort!=0 &&
				connectionAttempt->recipient->mostRecentPort!=0)
			{
				SystemAddress senderSystemAddress = connectionAttempt->sender->systemAddress;
				SystemAddress recipientSystemAddress = connectionAttempt->recipient->systemAddress;
				SystemAddress recipientTargetAddress = recipientSystemAddress;
				SystemAddress senderTargetAddress = senderSystemAddress;
				recipientTargetAddress.port=connectionAttempt->recipient->mostRecentPort;
				senderTargetAddress.port=connectionAttempt->sender->mostRecentPort;

				// Pick a time far enough in the future that both systems will have gotten the message
				int targetPing = rakPeerInterface->GetAveragePing(recipientTargetAddress);
				int senderPing = rakPeerInterface->GetAveragePing(senderSystemAddress);
				RakNetTime simultaneousAttemptTime;
				if (targetPing==-1 || senderPing==-1)
					simultaneousAttemptTime = time + 1500;
				else
				{
					int largerPing = targetPing > senderPing ? targetPing : senderPing;
					if (largerPing * 4 < 100)
						simultaneousAttemptTime = time + 100;
					else
						simultaneousAttemptTime = time + (largerPing * 4);
				}

				// Send to recipient timestamped message to connect at time
				RakNet::BitStream bsOut;
				bsOut.Write((MessageID)ID_TIMESTAMP);
				bsOut.Write(simultaneousAttemptTime);
				bsOut.Write((MessageID)ID_NAT_CONNECT_AT_TIME);
				bsOut.Write(senderTargetAddress); // Public IP, using most recent port
				for (j=0; j < MAXIMUM_NUMBER_OF_INTERNAL_IDS; j++) // Internal IP
					bsOut.Write(rakPeerInterface->GetInternalID(senderSystemAddress,j));
				bsOut.Write(connectionAttempt->sender->guid);
				bsOut.Write(false);
				rakPeerInterface->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,recipientSystemAddress,false);

				// Same for sender
				bsOut.Reset();
				bsOut.Write((MessageID)ID_TIMESTAMP);
				bsOut.Write(simultaneousAttemptTime);
				bsOut.Write((MessageID)ID_NAT_CONNECT_AT_TIME);
				bsOut.Write(recipientTargetAddress); // Public IP, using most recent port
				for (j=0; j < MAXIMUM_NUMBER_OF_INTERNAL_IDS; j++) // Internal IP
					bsOut.Write(rakPeerInterface->GetInternalID(recipientSystemAddress,j));						
				bsOut.Write(connectionAttempt->recipient->guid);
				bsOut.Write(true);
				rakPeerInterface->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,senderSystemAddress,false);

				connectionAttempt->recipient->DerefConnectionAttempt(connectionAttempt);
				connectionAttempt->sender->DeleteConnectionAttempt(connectionAttempt);
			}
		}
	}
}
void NatPunchthroughServer::StartPunchthroughForUser(User *user)
{
	if (user->isReady==false)
		return;

	ConnectionAttempt *connectionAttempt;
	User *sender,*recipient,*otherUser;
	unsigned int i;
	for (i=0; i < user->connectionAttempts.Size(); i++)
	{
		connectionAttempt=user->connectionAttempts[i];
		if (connectionAttempt->sender==user)
		{
			otherUser=connectionAttempt->recipient;
			sender=user;
			recipient=otherUser;
		}
		else
		{
			otherUser=connectionAttempt->sender;
			recipient=user;
			sender=otherUser;
		}

		if (otherUser->isReady)
		{
			sender->isReady=false;
			recipient->isReady=false;
			connectionAttempt->attemptPhase=ConnectionAttempt::NAT_ATTEMPT_PHASE_GETTING_RECENT_PORTS;
			connectionAttempt->startTime=RakNet::GetTime();

			sender->mostRecentPort=0;
			recipient->mostRecentPort=0;

			RakNet::BitStream outgoingBs;
			outgoingBs.Write((MessageID)ID_NAT_GET_MOST_RECENT_PORT);
			rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,sender->systemAddress,false);
			rakPeerInterface->Send(&outgoingBs,HIGH_PRIORITY,RELIABLE_ORDERED,0,recipient->systemAddress,false);
		}
	}
}
