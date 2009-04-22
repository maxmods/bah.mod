#include "ReplicaManager3.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "NetworkIDManager.h"

using namespace RakNet;

bool operator<( const DataStructures::MLKeyRef<Replica3*> &inputKey, const LastSerializationResult *cls ) {return (void*) inputKey.Get() < (void*) cls->replica;} \
bool operator>( const DataStructures::MLKeyRef<Replica3*> &inputKey, const LastSerializationResult *cls ) {return (void*) inputKey.Get() > (void*) cls->replica;} \
bool operator==( const DataStructures::MLKeyRef<Replica3*> &inputKey, const LastSerializationResult *cls ) {return (void*) inputKey.Get() == (void*) cls->replica;}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LastSerializationResult::LastSerializationResult()
{
	replica=0;
	lastSerializationSent=0;
	neverSerialize=false;
}
LastSerializationResult::~LastSerializationResult()
{
	if (lastSerializationSent)
		RakNet::OP_DELETE(lastSerializationSent,__FILE__,__LINE__);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ReplicaManager3::ReplicaManager3()
{
	autoConstructByQuery=true;
	defaultSendParameters.orderingChannel=0;
	defaultSendParameters.priority=HIGH_PRIORITY;
	defaultSendParameters.reliability=RELIABLE_ORDERED;
	autoSerializeInterval=30;
	lastAutoSerializeOccurance=0;
	worldId=0;
	autoCreateConnections=true;
	autoDestroyConnections=true;
	networkIDManager=0;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ReplicaManager3::~ReplicaManager3()
{
	Clear();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetAutoManageConnections(bool autoCreate, bool autoDestroy)
{
	autoCreateConnections=autoCreate;
	autoDestroyConnections=autoDestroy;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ReplicaManager3::PushConnection(RakNet::Connection_RM3 *newConnection)
{
	if (newConnection==0)
		return false;
	if (GetConnectionByGUID(newConnection->GetRakNetGUID()))
		return false;
	DataStructures::DefaultIndexType index = connectionList.GetInsertionIndex(newConnection);
	if (index!=-1)
		connectionList.InsertAtIndex(newConnection,index,__FILE__,__LINE__);
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RakNet::Connection_RM3 * ReplicaManager3::PopConnection(RakNetGUID guid, bool alsoDeleteCreatedReplicas)
{
	DataStructures::Multilist<ML_STACK, Replica3*> replicaList;
	RakNet::Connection_RM3 *connection;
	DataStructures::DefaultIndexType index;
	for (index=0; index < connectionList.GetSize(); index++)
	{
		if (connectionList[index]->GetRakNetGUID()==guid)
		{
			connection=connectionList[index];
			if (alsoDeleteCreatedReplicas)
			{
				GetReplicasCreatedByGuid(guid, replicaList);
				BroadcastDestructionList(replicaList, connection->GetSystemAddress());
				DataStructures::DefaultIndexType index2;
				for (index2=0; index2 < replicaList.GetSize(); index2++)
				{
					replicaList[index2]->PreDestruction(connection);
					replicaList[index2]->DeallocReplica(connection);
				}
			}
			connectionList.RemoveAtIndex(index,__FILE__,__LINE__);
			return connection;
		}
	}




	return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::Reference(RakNet::Replica3 *replica3)
{
	DataStructures::DefaultIndexType index;
	index = userReplicaList.GetInsertionIndex(replica3);
	if (index!=-1)
	{
		if (networkIDManager==0)
			networkIDManager=rakPeerInterface->GetNetworkIDManager();
		RakAssert(networkIDManager);
		replica3->SetNetworkIDManager(networkIDManager);
		if (replica3->creatingSystemGUID==UNASSIGNED_RAKNET_GUID)
			replica3->creatingSystemGUID=rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
		replica3->replicaManager=this;
		userReplicaList.InsertAtIndex(replica3,index,__FILE__,__LINE__);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::Dereference(RakNet::Replica3 *replica3)
{
	DataStructures::DefaultIndexType index, index2;
	for (index=0; index < userReplicaList.GetSize(); index++)
	{
		if (userReplicaList[index]==replica3)
		{
			// Remove from all connections
			for (index2=0; index2 < connectionList.GetSize(); index2++)
			{
				connectionList[index2]->Dereference(replica3);
			}

			userReplicaList.RemoveAtIndex(index,__FILE__,__LINE__);
			return;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::DereferenceList(DataStructures::Multilist<ML_STACK, Replica3*> &replicaListIn)
{
	DataStructures::DefaultIndexType index;
	for (index=0; index < userReplicaList.GetSize(); index++)
		Dereference(replicaListIn[index]);
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::GetReplicasCreatedByMe(DataStructures::Multilist<ML_STACK, Replica3*> &replicaListOut)
{
	RakNetGUID myGuid = rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
	GetReplicasCreatedByGuid(rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS), replicaListOut);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::GetReferencedReplicaList(DataStructures::Multilist<ML_STACK, Replica3*> &replicaListOut)
{
	replicaListOut=userReplicaList;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::GetReplicasCreatedByGuid(RakNetGUID guid, DataStructures::Multilist<ML_STACK, Replica3*> &replicaListOut)
{
	replicaListOut.Clear(false,__FILE__,__LINE__);
	DataStructures::DefaultIndexType index;
	for (index=0; index < userReplicaList.GetSize(); index++)
	{
		if (userReplicaList[index]->creatingSystemGUID==guid)
			replicaListOut.Push(userReplicaList[index],__FILE__,__LINE__);
	}
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned ReplicaManager3::GetReplicaCount(void) const
{
	return userReplicaList.GetSize();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Replica3 *ReplicaManager3::GetReplicaAtIndex(unsigned index)
{
	return userReplicaList[index];
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DataStructures::DefaultIndexType ReplicaManager3::GetConnectionCount(void) const
{
	return connectionList.GetSize();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Connection_RM3* ReplicaManager3::GetConnectionAtIndex(unsigned index) const
{
	return connectionList[index];
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Connection_RM3* ReplicaManager3::GetConnectionBySystemAddress(SystemAddress sa) const
{
	DataStructures::DefaultIndexType index;
	for (index=0; index < connectionList.GetSize(); index++)
	{
		if (connectionList[index]->GetSystemAddress()==sa)
		{
			return connectionList[index];
		}
	}
	return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Connection_RM3* ReplicaManager3::GetConnectionByGUID(RakNetGUID guid) const
{
	DataStructures::DefaultIndexType index;
	for (index=0; index < connectionList.GetSize(); index++)
	{
		if (connectionList[index]->GetRakNetGUID()==guid)
		{
			return connectionList[index];
		}
	}
	return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetDefaultOrderingChannel(char def)
{
	defaultSendParameters.orderingChannel=def;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetDefaultPacketPriority(PacketPriority def)
{
	defaultSendParameters.priority=def;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetDefaultPacketReliability(PacketReliability def)
{
	defaultSendParameters.reliability=def;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetAutoConstructByQuery(bool autoConstruct)
{
	autoConstructByQuery=autoConstruct;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetAutoSerializeInterval(RakNetTime intervalMS)
{
	autoSerializeInterval=intervalMS;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::GetConnectionsThatHaveReplicaConstructed(Replica3 *replica, DataStructures::Multilist<ML_STACK, Connection_RM3*> &connectionsThatHaveConstructedThisReplica)
{
	connectionsThatHaveConstructedThisReplica.Clear(false,__FILE__,__LINE__);
	DataStructures::DefaultIndexType index;
	for (index=0; index < connectionList.GetSize(); index++)
	{
		if (connectionList[index]->HasReplicaConstructed(replica))
			connectionsThatHaveConstructedThisReplica.Push(connectionList[index],__FILE__,__LINE__);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::Clear(void)
{
	connectionList.ClearPointers(true,__FILE__,__LINE__);
	userReplicaList.Clear(true,__FILE__,__LINE__);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PRO ReplicaManager3::GetDefaultSendParameters(void) const
{
	return defaultSendParameters;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetWorldID(unsigned char id)
{
	worldId=id;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char ReplicaManager3::GetWorldID(void) const
{
	return worldId;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NetworkIDManager *ReplicaManager3::GetNetworkIDManager(void) const
{
	return networkIDManager;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::SetNetworkIDManager(NetworkIDManager *_networkIDManager)
{
	networkIDManager=_networkIDManager;
	if (networkIDManager)
		networkIDManager->SetGuid(rakPeerInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS));
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PluginReceiveResult ReplicaManager3::OnReceive(Packet *packet)
{
	unsigned char incomingWorldId;
	RakNetTime timestamp=0;
	unsigned char packetIdentifier, packetDataOffset;
	if ( ( unsigned char ) packet->data[ 0 ] == ID_TIMESTAMP )
	{
		if ( packet->length > sizeof( unsigned char ) + sizeof( RakNetTime ) )
		{
			packetIdentifier = ( unsigned char ) packet->data[ sizeof( unsigned char ) + sizeof( RakNetTime ) ];
			// Required for proper endian swapping
			RakNet::BitStream tsBs(packet->data+sizeof(MessageID),packet->length-1,false);
			tsBs.Read(timestamp);
			incomingWorldId=packet->data[sizeof( unsigned char )*2 + sizeof( RakNetTime )];
			packetDataOffset=sizeof( unsigned char )*3 + sizeof( RakNetTime );
		}
		else
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	else
	{
		packetIdentifier = ( unsigned char ) packet->data[ 0 ];
		incomingWorldId=packet->data[sizeof( unsigned char )];
		packetDataOffset=sizeof( unsigned char )*2;
	}
	if (incomingWorldId!=worldId)
		return RR_CONTINUE_PROCESSING;

	switch (packetIdentifier)
	{
	case ID_REPLICA_MANAGER_CONSTRUCTION:
		OnConstruction(packet->data, packet->length, packet->guid, packetDataOffset);
		break;
	case ID_REPLICA_MANAGER_SERIALIZE:
		OnSerialize(packet->data, packet->length, packet->guid, timestamp, packetDataOffset);
		break;
	case ID_REPLICA_MANAGER_DOWNLOAD_STARTED:
		OnLocalConstructionRejected(packet->data, packet->length, packet->guid, packetDataOffset);
		break;
	case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
		OnLocalConstructionAccepted(packet->data, packet->length, packet->guid, packetDataOffset);
		break;
	}

	return RR_CONTINUE_PROCESSING;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::Update(void)
{
	DataStructures::DefaultIndexType index,index2;

	if (autoConstructByQuery)
	{
		RM3ConstructionState constructionState;
		DataStructures::Multilist<ML_STACK, Replica3*> constructedReplicas, destroyedReplicas, constructedReplicasCulled, destroyedReplicasCulled;
		for (index=0; index < connectionList.GetSize(); index++)
		{
			constructedReplicas.Clear(true,__FILE__,__LINE__);
			destroyedReplicas.Clear(true,__FILE__,__LINE__);
			for (index2=0; index2 < userReplicaList.GetSize(); index2++)
			{
				if (userReplicaList[index2]->creatingSystemGUID==connectionList[index]->GetRakNetGUID())
					continue;

				constructionState=userReplicaList[index2]->QueryConstruction(connectionList[index],this);
				if (constructionState==RM3CS_ALREADY_EXISTS_REMOTELY)
				{
					connectionList[index]->Reference(userReplicaList[index2], true);
				}
				else if (constructionState==RM3CS_SEND_CONSTRUCTION)
				{
					constructedReplicas.Push(userReplicaList[index2],__FILE__,__LINE__);
				}
				else if (constructionState==RM3CS_NO_ACTION)
				{
					// Do nothing
				}
				else if (constructionState==RM3CS_SEND_DESTRUCTION)
				{
					destroyedReplicas.Push(userReplicaList[index2],__FILE__,__LINE__);
				}
			}

			connectionList[index]->CullUniqueNewAndDeletedObjects(constructedReplicas,destroyedReplicas,
				constructedReplicasCulled,
				destroyedReplicasCulled);
			connectionList[index]->SendConstruction(constructedReplicasCulled,destroyedReplicasCulled,defaultSendParameters,rakPeerInterface,worldId);
			connectionList[index]->SetDownloadWasSent(true);
		}
	}

	if (autoSerializeInterval>0)
	{
		RakNetTime time = RakNet::GetTime();

		if (time - lastAutoSerializeOccurance > autoSerializeInterval)
		{
			// Reset all per-tick flags held in the replica
			for (index=0; index < userReplicaList.GetSize(); index++)
			{
				Replica3 *replica = userReplicaList[index];
				replica->serializesIdenticallyThisTick=false;
				replica->serializedNothingIdenticallyThisTick=false;
			}

			for (index=0; index < connectionList.GetSize(); index++)
				connectionList[index]->OnAutoserializeInterval(defaultSendParameters, this);

			lastAutoSerializeOccurance=time;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) systemAddress;
	if (autoDestroyConnections)
	{
		Connection_RM3 *connection = PopConnection(rakNetGUID, true);
		if (connection)
			DeallocConnection(connection);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming)
{
	(void) isIncoming;
	if (autoCreateConnections)
	{
		Connection_RM3 *connection = AllocConnection(systemAddress, rakNetGUID);
		if (connection)
			PushConnection(connection);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnConstruction(unsigned char *packetData, int packetDataLength, RakNetGUID senderGuid, unsigned char packetDataOffset)
{
	Connection_RM3 *connection = GetConnectionByGUID(senderGuid);
	if (connection==0)
		return;

	RakNet::BitStream bsIn(packetData,packetDataLength,false);
	bsIn.IgnoreBytes(packetDataOffset);
	DataStructures::DefaultIndexType objectListSize, index;
	BitSize_t bitOffset;
	Replica3 *replica;
	uint32_t allocationNumber=0;
	NetworkID networkId;
	RakNetGUID creatingSystemGuid;

	if (networkIDManager==0)
		networkIDManager=rakPeerInterface->GetNetworkIDManager();
	RakAssert(networkIDManager);

	bsIn.Read(objectListSize);
	for (index=0; index < objectListSize; index++)
	{
		bsIn.Read(bitOffset);
		replica = connection->AllocReplica(&bsIn);
		if (replica==0)
		{
			bsIn.SetReadOffset(bitOffset);
			continue;
		}

		replica->SetNetworkIDManager(networkIDManager);

		bsIn.Read(networkId);
		if (networkId==UNASSIGNED_NETWORK_ID)
		{
			if (networkIDManager->IsNetworkIDAuthority()==false)
			{
				// Can't assign network ID
				replica->replicaManager=0;
				replica->DeallocReplica(connection);
				bsIn.SetReadOffset(bitOffset);
				continue;
			}

			bsIn.Read(allocationNumber);
		}
		else
		{
			if (networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId))
			{
				// Network ID already in use
				if (GetReplicaByNetworkID(networkId))
					connection->Reference(replica,false);
				
				replica->replicaManager=0;
				replica->DeallocReplica(connection);
				bsIn.SetReadOffset(bitOffset);
				continue;
			}

			replica->SetNetworkID(networkId);
		}

		bsIn.Read(creatingSystemGuid);
		replica->creatingSystemGUID=creatingSystemGuid;
		replica->replicaManager=this;

		if (!replica->QueryRemoteConstruction(connection) ||
			!replica->DeserializeConstruction(&bsIn, connection))
		{
			// Overtake this message to mean construction rejected
			if (networkId==UNASSIGNED_NETWORK_ID)
			{
				RakNet::BitStream bsOut;
				bsOut.Write((MessageID)ID_REPLICA_MANAGER_DOWNLOAD_STARTED);
				bsOut.Write(worldId);
				bsOut.Write(allocationNumber);
				replica->SerializeConstructionRequestRejected(&bsOut,connection);
				rakPeerInterface->Send(&bsOut, defaultSendParameters.priority, defaultSendParameters.reliability, defaultSendParameters.orderingChannel, connection->GetSystemAddress(), false);
			}

			replica->replicaManager=0;
			replica->DeallocReplica(connection);
			bsIn.SetReadOffset(bitOffset);
			continue;
		}

		bsIn.SetReadOffset(bitOffset);
		replica->PostDeserializeConstruction(connection);

		if (networkId==UNASSIGNED_NETWORK_ID)
		{
			// Overtake this message to mean construction accepted
			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE);
			bsOut.Write(worldId);
			bsOut.Write(allocationNumber);
			bsOut.Write(replica->GetNetworkID());
			replica->SerializeConstructionRequestAccepted(&bsOut,connection);
			rakPeerInterface->Send(&bsOut, defaultSendParameters.priority, defaultSendParameters.reliability, defaultSendParameters.orderingChannel, connection->GetSystemAddress(), false);
		}
		bsIn.AlignReadToByteBoundary();

		// Register the replica
		Reference(replica);

		// Tell the connection(s) that this object exists since they just sent it to us
		connection->Reference(replica,false);
	}

	// Destructions
	bool b = bsIn.Read(objectListSize);
	RakAssert(b);
	for (index=0; index < objectListSize; index++)
	{
		bsIn.Read(networkId);
		bsIn.Read(bitOffset);
		replica = networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId);
		if (replica==0)
		{
			// Unknown object
			bsIn.SetReadOffset(bitOffset);
			continue;
		}
		bsIn.Read(replica->deletingSystemGUID);
		if (replica->DeserializeDestruction(&bsIn,connection))
		{
			// Make sure it wasn't deleted in DeserializeDestruction
			if (networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId))
			{
				replica->PreDestruction(connection);

				// Forward deletion by remote system
				BroadcastDestruction(replica,connection->GetSystemAddress());
				Dereference(replica);
				replica->replicaManager=0; // Prevent BroadcastDestruction from being called again
				replica->DeallocReplica(connection);
			}			
		}
		else
		{
			replica->PreDestruction(connection);
			connection->Dereference(replica);
		}
	
		bsIn.AlignReadToByteBoundary();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnSerialize(unsigned char *packetData, int packetDataLength, RakNetGUID senderGuid, RakNetTime timestamp, unsigned char packetDataOffset)
{
	Connection_RM3 *connection = GetConnectionByGUID(senderGuid);
	if (connection==0)
		return;
	if (networkIDManager==0)
		networkIDManager=rakPeerInterface->GetNetworkIDManager();
	RakAssert(networkIDManager);
	RakNet::BitStream bsIn(packetData,packetDataLength,false);
	bsIn.IgnoreBytes(packetDataOffset);

	Replica3 *replica;
	NetworkID networkId;
	bsIn.Read(networkId);
	replica = networkIDManager->GET_OBJECT_FROM_ID<Replica3*>(networkId);
	bsIn.AlignReadToByteBoundary();

	if (replica!=0)
		replica->Deserialize(&bsIn,timestamp,connection);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnLocalConstructionRejected(unsigned char *packetData, int packetDataLength, RakNetGUID senderGuid, unsigned char packetDataOffset)
{
	Connection_RM3 *connection = GetConnectionByGUID(senderGuid);
	if (connection==0)
		return;
	RakNet::BitStream bsIn(packetData,packetDataLength,false);
	bsIn.IgnoreBytes(packetDataOffset);
	uint32_t allocationNumber;
	bsIn.Read(allocationNumber);
	DataStructures::DefaultIndexType index;
	for (index=0; index < userReplicaList.GetSize(); index++)
	{
		if (userReplicaList[index]->GetAllocationNumber()==allocationNumber)
		{
			userReplicaList[index]->DeserializeConstructionRequestRejected(&bsIn,connection);
			break;
		}
	}
	
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ReplicaManager3::OnLocalConstructionAccepted(unsigned char *packetData, int packetDataLength, RakNetGUID senderGuid, unsigned char packetDataOffset)
{
	Connection_RM3 *connection = GetConnectionByGUID(senderGuid);
	if (connection==0)
		return;
	RakNet::BitStream bsIn(packetData,packetDataLength,false);
	bsIn.IgnoreBytes(packetDataOffset);
	uint32_t allocationNumber;
	bsIn.Read(allocationNumber);
	NetworkID assignedNetworkId;
	bsIn.Read(assignedNetworkId);
	DataStructures::DefaultIndexType index;
	for (index=0; index < userReplicaList.GetSize(); index++)
	{
		if (userReplicaList[index]->GetAllocationNumber()==allocationNumber)
		{
			userReplicaList[index]->SetNetworkID(assignedNetworkId);
			userReplicaList[index]->DeserializeConstructionRequestAccepted(&bsIn,connection);
			break;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Replica3* ReplicaManager3::GetReplicaByNetworkID(NetworkID networkId)
{
	DataStructures::DefaultIndexType i;
	for (i=0; i < userReplicaList.GetSize(); i++)
	{
		if (userReplicaList[i]->GetNetworkID()==networkId)
			return userReplicaList[i];
	}
	return 0;
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void ReplicaManager3::BroadcastDestructionList(DataStructures::Multilist<ML_STACK, Replica3*> &replicaList, SystemAddress exclusionAddress)
{
	RakNet::BitStream bsOut;
	DataStructures::DefaultIndexType i,j;

	for (i=0; i < replicaList.GetSize(); i++)
	{

		if (replicaList[i]->deletingSystemGUID==UNASSIGNED_RAKNET_GUID)
			replicaList[i]->deletingSystemGUID=GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
	}

	for (j=0; j < connectionList.GetSize(); j++)
	{
		if (connectionList[j]->GetSystemAddress()==exclusionAddress)
			continue;

		bsOut.Reset();
		bsOut.Write((MessageID)ID_REPLICA_MANAGER_CONSTRUCTION);
		bsOut.Write(worldId);
		DataStructures::DefaultIndexType cnt=0;
		bsOut.Write(cnt); // No construction
		cnt=replicaList.GetSize();
		bsOut.Write(cnt);

		for (i=0; i < replicaList.GetSize(); i++)
		{
			if (connectionList[j]->HasReplicaConstructed(replicaList[i])==false)
				continue;

			NetworkID networkId;
			networkId=replicaList[i]->GetNetworkID();
			bsOut.Write(networkId);
			BitSize_t offsetStart, offsetEnd;
			offsetStart=bsOut.GetWriteOffset();
			bsOut.Write(offsetStart);
			bsOut.Write(replicaList[i]->deletingSystemGUID);
			replicaList[i]->SerializeDestruction(&bsOut, connectionList[j]);
			bsOut.AlignWriteToByteBoundary();
			offsetEnd=bsOut.GetWriteOffset();
			bsOut.SetWriteOffset(offsetStart);
			bsOut.Write(offsetEnd);
			bsOut.SetWriteOffset(offsetEnd);
		}

		rakPeerInterface->Send(&bsOut,defaultSendParameters.priority,defaultSendParameters.reliability,defaultSendParameters.orderingChannel,connectionList[j]->GetSystemAddress(),false);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void ReplicaManager3::BroadcastDestruction(Replica3 *replica, SystemAddress exclusionAddress)
{
	DataStructures::Multilist<ML_STACK, Replica3*> replicaList;
	replicaList.Push(replica);
	BroadcastDestructionList(replicaList,exclusionAddress);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Connection_RM3::Connection_RM3(SystemAddress _systemAddress, RakNetGUID _guid)
	: systemAddress(_systemAddress), guid(_guid), downloadWasSent(false)
{
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Connection_RM3::~Connection_RM3()
{
	constructedReplicas.ClearPointers(true,__FILE__,__LINE__);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::GetConstructedReplicas(DataStructures::Multilist<ML_ORDERED_LIST, LastSerializationResult*, Replica3*> &objectsTheyDoHave)
{
	objectsTheyDoHave=constructedReplicas;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Connection_RM3::HasReplicaConstructed(RakNet::Replica3 *replica)
{
	return constructedReplicas.GetIndexOf(replica)!=-1;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::OnAutoserializeInterval(PRO defaultSendParameters, ReplicaManager3 *rm3)
{
	SerializeParameters sp;
	sp.bitsWrittenSoFar=0;
	DataStructures::DefaultIndexType index;
	for (index=0; index < constructedReplicas.GetSize(); index++)
	{
		sp.destinationConnection=this;
		sp.lastSerializationSent=constructedReplicas[index]->lastSerializationSent;
		sp.messageTimestamp=0;
		sp.pro=defaultSendParameters;
		RakAssert( !( sp.pro.reliability > RELIABLE_SEQUENCED || sp.pro.reliability < 0 ) );
		RakAssert( !( sp.pro.priority > NUMBER_OF_PRIORITIES || sp.pro.priority < 0 ) );
		SendSerializeIfChanged(constructedReplicas[index], &sp, rm3->GetRakPeerInterface(), rm3->GetWorldID(), false);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::SendSerialize(RakNet::Replica3 *replica, RakNet::BitStream *serializationData, RakNetTime timestamp, PRO sendParameters, RakPeerInterface *rakPeer, unsigned char worldId)
{
	RakAssert(replica->GetNetworkID()!=UNASSIGNED_NETWORK_ID);
	RakNet::BitStream out;
	if (timestamp!=0)
	{
		out.Write((MessageID)ID_TIMESTAMP);
		out.Write(timestamp);
	}
	out.Write((MessageID)ID_REPLICA_MANAGER_SERIALIZE);
	out.Write(worldId);
	out.Write(replica->GetNetworkID());
	out.AlignWriteToByteBoundary();
	out.Write(serializationData);
	rakPeer->Send(&out,sendParameters.priority,sendParameters.reliability,sendParameters.orderingChannel,systemAddress,false);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::SendSerializeIfChanged(LastSerializationResult *lastSerializationResult, SerializeParameters *sp, RakPeerInterface *rakPeer, unsigned char worldId, bool forceForNewConnection)
{
	RakNet::Replica3 *replica = lastSerializationResult->replica;
	RM3SerializationResult serializationResult;
	RakNet::BitStream *bsSentThisTick, *bsSentLastTick;

	if (lastSerializationResult->neverSerialize)
		return;

	if (replica->GetNetworkID()==UNASSIGNED_NETWORK_ID)
		return;

	if (replica->QuerySerialization(this)==false)
		return;

	sp->outputBitstream.Reset();
	if (sp->lastSerializationSent)
		sp->lastSerializationSent->ResetReadPointer();
	replica->identicalSerializationLastTick->ResetReadPointer();

	// serializesIdentically is reset to false each tick, if set to true, won't call Serialize() again for any following connection
	if (replica->serializesIdenticallyThisTick==false)
	{
		serializationResult = replica->Serialize(sp);
	

		if (serializationResult==RM3SR_NEVER_SERIALIZE_FOR_THIS_CONNECTION)
		{
			// Never again for this connection and replica pair
			lastSerializationResult->neverSerialize=true;
			return;
		}

		if (serializationResult==RM3SR_DO_NOT_SERIALIZE)
		{
			// Don't serialize this tick only
			return;
		}
		
		// Serialize. User wrote to sp->outputBitstream
		bsSentThisTick=&sp->outputBitstream;

		if (serializationResult==RM3SR_SERIALIZED_IDENTICALLY)
		{
			if (bsSentThisTick->GetNumberOfBitsUsed()==0)
			{
				// User wrote nothing for all remaining connections. This flag remains true until iterated through all remaining connections
				replica->serializedNothingIdenticallyThisTick=true;
				return;
			}

			replica->identicalSerializationTimestamp=sp->messageTimestamp;

			// Swap last write/read pointers, reset new write
			replica->SwapIdenticalSerializationPointers();

			// Tag not to call Serialize() again for all remaining connections this tick
			replica->serializesIdenticallyThisTick=true;

			// Copy what was written for remaining connections to compare against, and for next tick
			replica->identicalSerializationThisTick->Reset();
			replica->identicalSerializationThisTick->Write(&sp->outputBitstream);

			replica->identicalSerializationPRO=sp->pro;
			sp->outputBitstream.ResetReadPointer();
		}
		else
		{
			RakAssert(serializationResult==RM3SR_SERIALIZED_UNIQUELY);
			if (bsSentThisTick->GetNumberOfBitsUsed()==0)
				return;

			// Uniquely store what was serialized for this connection / replica pair
			if (lastSerializationResult->lastSerializationSent==0)
				lastSerializationResult->lastSerializationSent=RakNet::OP_NEW<RakNet::BitStream>(__FILE__,__LINE__);
		}
	}
	else
	{
		if (replica->serializedNothingIdenticallyThisTick)
			return;

		// We are sending whatever was tagged as identical in a prior iteration
		bsSentThisTick=replica->identicalSerializationThisTick;
		sp->messageTimestamp=replica->identicalSerializationTimestamp;
		sp->pro=replica->identicalSerializationPRO;
		RakAssert( !( sp->pro.reliability > RELIABLE_SEQUENCED || sp->pro.reliability < 0 ) );
		RakAssert( !( sp->pro.priority > NUMBER_OF_PRIORITIES || sp->pro.priority < 0 ) );
		serializationResult=RM3SR_SERIALIZED_IDENTICALLY;
	}

	// Point to whatever we sent last tick, either per-connection/replica data, or the identical shared data
	if (lastSerializationResult->lastSerializationSent) 
		bsSentLastTick=lastSerializationResult->lastSerializationSent;
	else
		bsSentLastTick=replica->identicalSerializationLastTick;

	if (forceForNewConnection ||
		bsSentThisTick->GetNumberOfBitsUsed() != bsSentLastTick->GetNumberOfBitsUsed() ||
		memcmp(bsSentThisTick->GetData(), bsSentLastTick->GetData(), bsSentThisTick->GetNumberOfBytesUsed())!=0)
	{
		sp->bitsWrittenSoFar+=bsSentThisTick->GetNumberOfBitsUsed();
		SendSerialize(replica, bsSentThisTick, sp->messageTimestamp, sp->pro, rakPeer, worldId);

		if (lastSerializationResult->lastSerializationSent)
		{
			lastSerializationResult->lastSerializationSent->Reset();
			sp->outputBitstream.ResetReadPointer();
			lastSerializationResult->lastSerializationSent->Write(&sp->outputBitstream);
		}
	}

	if (serializationResult==RM3SR_SERIALIZED_IDENTICALLY)
	{
		// If using identical data, but had per-connection/replica serialization before, can free that memory now.
		RakNet::OP_DELETE(lastSerializationResult->lastSerializationSent,__FILE__,__LINE__);
		lastSerializationResult->lastSerializationSent=0;
	}
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LastSerializationResult * Connection_RM3::Reference(Replica3* replica, bool isStatic)
{
	if (constructedReplicas.GetIndexOf(replica)!=-1)
		return 0;

	LastSerializationResult *lsr = RakNet::OP_NEW<LastSerializationResult>(__FILE__,__LINE__);
	lsr->replica=replica;
	lsr->isStatic=isStatic;
	constructedReplicas.Push(lsr,replica,__FILE__,__LINE__);
	return lsr;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::Dereference(Replica3* replica)
{
	constructedReplicas.RemoveAtKey(replica,false);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::SetConstructionByList(DataStructures::Multilist<ML_STACK, Replica3*> objectsThatExist, ReplicaManager3 *replicaManager)
{
	DataStructures::Multilist<ML_STACK, Replica3*> intersection, uniqueToSource1, uniqueToSource2;
	
	DataStructures::Multilist<ML_STACK, Replica3*> constructedReplicasStack;
	DataStructures::DefaultIndexType i;
	for (i=0; i < constructedReplicas.GetSize(); i++)
		constructedReplicasStack.Push(constructedReplicas[i]->replica);
	constructedReplicasStack.TagSorted();

	DataStructures::Multilist<ML_STACK, Replica3*>::FindIntersection(objectsThatExist,
		constructedReplicasStack,
		intersection,
		uniqueToSource1,
		uniqueToSource2);	
	SendConstruction(uniqueToSource1,uniqueToSource2,replicaManager->GetDefaultSendParameters(),replicaManager->GetRakPeerInterface(),replicaManager->GetWorldID());
	downloadWasSent=true;
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::CullUniqueNewAndDeletedObjects(
	DataStructures::Multilist<ML_STACK, Replica3*> &newObjectsIn,
	DataStructures::Multilist<ML_STACK, Replica3*> &deletedObjectsIn,
	DataStructures::Multilist<ML_STACK, Replica3*> &newObjectsOut,
	DataStructures::Multilist<ML_STACK, Replica3*> &deletedObjectsOut)
{
	DataStructures::DefaultIndexType index;
	DataStructures::Multilist<ML_STACK, Replica3*> newObjectsSortedCopy=newObjectsIn;
	DataStructures::Multilist<ML_STACK, Replica3*> deletedObjectsSortedCopy=deletedObjectsIn;
	DataStructures::Multilist<ML_STACK, Replica3*> intersection, uniqueToSource1, uniqueToSource2;
	DataStructures::Multilist<ML_STACK, Replica3*> _constructedReplicas;
	newObjectsOut.Clear();
	deletedObjectsOut.Clear();
	for (index=0; index < constructedReplicas.GetSize(); index++)
		_constructedReplicas.Push(constructedReplicas[index]->replica);
	_constructedReplicas.TagSorted();
	DataStructures::Multilist<ML_STACK, Replica3*>::FindIntersection(
		newObjectsSortedCopy,
		_constructedReplicas, 
		intersection,
		uniqueToSource1,
		uniqueToSource2);

	// Remove from newObjects objects that already exist on the remote system
	// uniqueToSource1;
	for (index=0; index < newObjectsIn.GetSize(); index++)
	{
		if (uniqueToSource1.GetIndexOf(newObjectsIn[index])!=-1)
			newObjectsOut.Push(newObjectsIn[index]);
	}

	DataStructures::Multilist<ML_STACK, Replica3*>::FindIntersection(
		deletedObjectsSortedCopy,
		_constructedReplicas, 
		intersection,
		uniqueToSource1,
		uniqueToSource2);

	// Remove from deletedObjects objects that do not already exist on the remote system
	// intersection;
	for (index=0; index < deletedObjectsIn.GetSize(); index++)
	{
		if (intersection.GetIndexOf(deletedObjectsIn[index])!=-1)
			deletedObjectsOut.Push(deletedObjectsIn[index]);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::SendConstruction(DataStructures::Multilist<ML_STACK, Replica3*> &newObjects, DataStructures::Multilist<ML_STACK, Replica3*> &deletedObjects, PRO sendParameters, RakPeerInterface *rakPeer, unsigned char worldId)
{
	if (newObjects.GetSize()==0 && deletedObjects.GetSize()==0)
		return;

	// All construction and destruction takes place in the same network message
	// Otherwise, if objects rely on each other being created the same tick to be valid, this won't always be true
	DataStructures::Multilist<ML_STACK, LastSerializationResult*> serializedObjects;
	BitSize_t offsetStart, offsetEnd;
	DataStructures::DefaultIndexType newListIndex, oldListIndex;
	RakNet::BitStream bsOut;
	NetworkID networkId;
	LastSerializationResult *lsr;
	bsOut.Write((MessageID)ID_REPLICA_MANAGER_CONSTRUCTION);
	bsOut.Write(worldId);
	bsOut.Write(newObjects.GetSize());
	// Construction
	for (newListIndex=0; newListIndex < newObjects.GetSize(); newListIndex++)
	{
		offsetStart=bsOut.GetWriteOffset();
		bsOut.Write(offsetStart);
		newObjects[newListIndex]->WriteAllocationID(&bsOut);
		networkId=newObjects[newListIndex]->GetNetworkID();
		bsOut.Write(networkId);
		if (networkId==UNASSIGNED_NETWORK_ID)
			bsOut.Write(newObjects[newListIndex]->GetAllocationNumber());
		bsOut.Write(newObjects[newListIndex]->creatingSystemGUID);
		newObjects[newListIndex]->SerializeConstruction(&bsOut, this);
		bsOut.AlignWriteToByteBoundary();
		offsetEnd=bsOut.GetWriteOffset();
		bsOut.SetWriteOffset(offsetStart);
		bsOut.Write(offsetEnd);
		bsOut.SetWriteOffset(offsetEnd);
		lsr = Reference(newObjects[newListIndex],false);
		if (lsr)
			serializedObjects.Push(lsr);
	}

	// Destruction
	DataStructures::DefaultIndexType listSize=deletedObjects.GetSize();
	bsOut.Write(listSize);
	for (oldListIndex=0; oldListIndex < deletedObjects.GetSize(); oldListIndex++)
	{
		networkId=deletedObjects[oldListIndex]->GetNetworkID();
		bsOut.Write(networkId);
		offsetStart=bsOut.GetWriteOffset();
		bsOut.Write(offsetStart);
		deletedObjects[oldListIndex]->deletingSystemGUID=rakPeer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
		bsOut.Write(deletedObjects[oldListIndex]->deletingSystemGUID);
		deletedObjects[oldListIndex]->SerializeDestruction(&bsOut, this);
		bsOut.AlignWriteToByteBoundary();
		offsetEnd=bsOut.GetWriteOffset();
		bsOut.SetWriteOffset(offsetStart);
		bsOut.Write(offsetEnd);
		bsOut.SetWriteOffset(offsetEnd);
	}
	rakPeer->Send(&bsOut,sendParameters.priority,sendParameters.reliability,sendParameters.orderingChannel,systemAddress,false);

	// Immediately send serialize after construction.
	// If the object was serialized identically, and does not change later on, then the new connection never gets the data
	SerializeParameters sp;
	sp.bitsWrittenSoFar=0;
	for (newListIndex=0; newListIndex < serializedObjects.GetSize(); newListIndex++)
	{
		sp.destinationConnection=this;
		sp.lastSerializationSent=serializedObjects[newListIndex]->lastSerializationSent;
		sp.messageTimestamp=0;
		sp.pro=sendParameters;
		RakAssert( !( sp.pro.reliability > RELIABLE_SEQUENCED || sp.pro.reliability < 0 ) );
		RakAssert( !( sp.pro.priority > NUMBER_OF_PRIORITIES || sp.pro.priority < 0 ) );
		SendSerializeIfChanged(serializedObjects[newListIndex], &sp, rakPeer, worldId, true);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Connection_RM3::IsInitialDownload(void) const
{
	return downloadWasSent==false;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connection_RM3::SetDownloadWasSent(bool b)
{
	downloadWasSent=b;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Replica3::Replica3()
{
	identicalSerializationThisTick=&identicalSerializationTick1;
	identicalSerializationLastTick=&identicalSerializationTick2;
	creatingSystemGUID=UNASSIGNED_RAKNET_GUID;
	deletingSystemGUID=UNASSIGNED_RAKNET_GUID;
	replicaManager=0;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Replica3::~Replica3()
{
	if (replicaManager)
	{
		replicaManager->Dereference(this);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Replica3::BroadcastDestruction(void)
{
	if (replicaManager)
		replicaManager->BroadcastDestruction(this,UNASSIGNED_SYSTEM_ADDRESS);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Replica3::SwapIdenticalSerializationPointers(void)
{

	RakNet::BitStream *swap;
	swap=identicalSerializationThisTick;
	identicalSerializationThisTick=identicalSerializationLastTick;
	identicalSerializationLastTick=swap;
	identicalSerializationThisTick->Reset();

}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RakNetGUID Replica3::GetCreatingSystemGUID(void) const
{
	return creatingSystemGUID;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RM3ConstructionState Replica3::QueryConstruction_ClientConstruction(RakNet::Connection_RM3 *destinationConnection)
{
	(void) destinationConnection;
	if (creatingSystemGUID==replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
		return RM3CS_SEND_CONSTRUCTION;
	// Send back to the owner client too, because they couldn't assign the network ID
	if (networkIDManager->IsNetworkIDAuthority())
		return RM3CS_SEND_CONSTRUCTION;
	return RM3CS_NO_ACTION;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QueryRemoteConstruction_ClientConstruction(RakNet::Connection_RM3 *sourceConnection)
{
	(void) sourceConnection;

	// OK to create
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RM3ConstructionState Replica3::QueryConstruction_ServerConstruction(RakNet::Connection_RM3 *destinationConnection)
{
	(void) destinationConnection;

	if (networkIDManager->IsNetworkIDAuthority())
		return RM3CS_SEND_CONSTRUCTION;
	return RM3CS_NO_ACTION;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QueryRemoteConstruction_ServerConstruction(RakNet::Connection_RM3 *sourceConnection)
{
	(void) sourceConnection;
	if (networkIDManager->IsNetworkIDAuthority())
		return false;
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RM3ConstructionState Replica3::QueryConstruction_PeerToPeer(RakNet::Connection_RM3 *destinationConnection)
{
	(void) destinationConnection;

	// We send to all, others do nothing
	if (creatingSystemGUID==replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
		return RM3CS_SEND_CONSTRUCTION;
	return RM3CS_ALREADY_EXISTS_REMOTELY;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QueryRemoteConstruction_PeerToPeer(RakNet::Connection_RM3 *sourceConnection)
{
	(void) sourceConnection;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QuerySerialization_ClientSerializable(RakNet::Connection_RM3 *destinationConnection)
{
	// Owner client sends to all
	if (creatingSystemGUID==replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
		return true;
	// Server sends to all but owner client
	if (networkIDManager->IsNetworkIDAuthority() && destinationConnection->GetRakNetGUID()!=creatingSystemGUID)
		return true;
	// Remote clients do not send
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QuerySerialization_ServerSerializable(RakNet::Connection_RM3 *destinationConnection)
{
	(void) destinationConnection;
	// Server sends to all
	if (networkIDManager->IsNetworkIDAuthority())
		return true;

	// Clients do not send
	return false;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool Replica3::QuerySerialization_PeerToPeer(RakNet::Connection_RM3 *destinationConnection)
{
	(void) destinationConnection;

	// Owner peer sends to all
	if (creatingSystemGUID==replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS))
		return true;

	// Remote peers do not send
	return false;
}
