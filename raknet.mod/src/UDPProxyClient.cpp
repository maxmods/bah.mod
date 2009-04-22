#include "UDPProxyClient.h"
#include "BitStream.h"
#include "UDPProxyCommon.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

using namespace RakNet;

UDPProxyClient::UDPProxyClient()
{
	resultHandler=0;
}
UDPProxyClient::~UDPProxyClient()
{

}
void UDPProxyClient::SetResultHandler(UDPProxyClientResultHandler *rh)
{
	resultHandler=rh;
}
bool UDPProxyClient::RequestForwarding(SystemAddress proxyCoordinator, SystemAddress sourceAddress, SystemAddress targetAddress, RakNetTimeMS timeoutOnNoDataMS, RakNet::BitStream *serverSelectionBitstream)
{
	if (rakPeerInterface->IsConnected(proxyCoordinator,false,false)==false)
		return false;

	// Pretty much a bug not to set the result handler, as otherwise you won't know if the operation succeeed or not
	RakAssert(resultHandler!=0);
	if (resultHandler==0)
		return false;

	BitStream outgoingBs;
	outgoingBs.Write((MessageID)ID_UDP_PROXY_GENERAL);
	outgoingBs.Write((MessageID)ID_UDP_PROXY_FORWARDING_REQUEST_FROM_CLIENT_TO_COORDINATOR);
	outgoingBs.Write(sourceAddress);
	outgoingBs.Write(targetAddress);
	outgoingBs.Write(timeoutOnNoDataMS);
	if (serverSelectionBitstream && serverSelectionBitstream->GetNumberOfBitsUsed()>0)
	{
		outgoingBs.Write(true);
		outgoingBs.Write(serverSelectionBitstream);
	}
	else
	{
		outgoingBs.Write(false);
	}
	rakPeerInterface->Send(&outgoingBs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, proxyCoordinator, false);

	return true;
}
void UDPProxyClient::Update(void)
{

}
PluginReceiveResult UDPProxyClient::OnReceive(Packet *packet)
{
	if (packet->data[0]==ID_UDP_PROXY_GENERAL && packet->length>1)
	{
		switch (packet->data[1])
		{
		case ID_UDP_PROXY_FORWARDING_SUCCEEDED:
		case ID_UDP_PROXY_ALL_SERVERS_BUSY:
		case ID_UDP_PROXY_IN_PROGRESS:
		case ID_UDP_PROXY_NO_SERVERS_ONLINE:
		case ID_UDP_PROXY_FORWARDING_NOTIFICATION:
			{
				SystemAddress senderAddress, targetAddress;
				RakNet::BitStream incomingBs(packet->data, packet->length, false);
				incomingBs.IgnoreBytes(sizeof(MessageID)*2);
				incomingBs.Read(senderAddress);
				incomingBs.Read(targetAddress);

				switch (packet->data[1])
				{
				case ID_UDP_PROXY_FORWARDING_NOTIFICATION:
				case ID_UDP_PROXY_FORWARDING_SUCCEEDED:
					{
						unsigned short srcToDestPort;
						unsigned short destToSourcePort;
						RakNet::RakString serverIP;
						incomingBs.Read(serverIP);
						incomingBs.Read(srcToDestPort);
						incomingBs.Read(destToSourcePort);
						if (packet->data[1]==ID_UDP_PROXY_FORWARDING_SUCCEEDED)
						{
							if (resultHandler)
								resultHandler->OnForwardingSuccess(serverIP.C_String(), srcToDestPort, destToSourcePort, packet->systemAddress, senderAddress, targetAddress, this);
						}
						else
						{
							// Send a datagram to the proxy, so if we are behind a router, that router adds an entry to the routing table.
							// Otherwise the router would block the incoming datagrams from source
							// It doesn't matter if the message actually arrives as long as it goes through the router
							rakPeerInterface->Ping(serverIP.C_String(), destToSourcePort, false);

							if (resultHandler)
								resultHandler->OnForwardingNotification(serverIP.C_String(), srcToDestPort, destToSourcePort, packet->systemAddress, senderAddress, targetAddress, this);
						}
					}
					break;
				case ID_UDP_PROXY_ALL_SERVERS_BUSY:
					if (resultHandler)
						resultHandler->OnAllServersBusy(packet->systemAddress, senderAddress, targetAddress, this);
					break;
				case ID_UDP_PROXY_IN_PROGRESS:
					if (resultHandler)
						resultHandler->OnForwardingInProgress(packet->systemAddress, senderAddress, targetAddress, this);
					break;
				case ID_UDP_PROXY_NO_SERVERS_ONLINE:
					if (resultHandler)
						resultHandler->OnNoServersOnline(packet->systemAddress, senderAddress, targetAddress, this);
					break;
				}
			
			}
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
		}
	}
	return RR_CONTINUE_PROCESSING;
}


