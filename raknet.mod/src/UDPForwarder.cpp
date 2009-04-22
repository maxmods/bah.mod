#include "UDPForwarder.h"
#include "GetTime.h"
#include "MTUSize.h"
#include "SocketLayer.h"
#include "WSAStartupSingleton.h"

using namespace RakNet;
static const unsigned short DEFAULT_MAX_FORWARD_ENTRIES=64;

bool operator<( const DataStructures::MLKeyRef<UDPForwarder::SrcAndDest> &inputKey, const UDPForwarder::ForwardEntry *cls )
{
	return inputKey.Get().source < cls->srcAndDest.source ||
		(inputKey.Get().source == cls->srcAndDest.source && inputKey.Get().destination < cls->srcAndDest.destination);
}
bool operator>( const DataStructures::MLKeyRef<UDPForwarder::SrcAndDest> &inputKey, const UDPForwarder::ForwardEntry *cls )
{
	return inputKey.Get().source > cls->srcAndDest.source ||
		(inputKey.Get().source == cls->srcAndDest.source && inputKey.Get().destination > cls->srcAndDest.destination);
}
bool operator==( const DataStructures::MLKeyRef<UDPForwarder::SrcAndDest> &inputKey, const UDPForwarder::ForwardEntry *cls )
{
	return inputKey.Get().source == cls->srcAndDest.source && inputKey.Get().destination == cls->srcAndDest.destination;
}


UDPForwarder::ForwardEntry::ForwardEntry() {readSocket=INVALID_SOCKET; timeLastDatagramForwarded=RakNet::GetTime();}
UDPForwarder::ForwardEntry::~ForwardEntry() {if (readSocket!=INVALID_SOCKET) closesocket(readSocket);}

UDPForwarder::UDPForwarder()
{
#ifdef _WIN32
	WSAStartupSingleton::AddRef();
#endif

	maxForwardEntries=DEFAULT_MAX_FORWARD_ENTRIES;
}
UDPForwarder::~UDPForwarder()
{
	Shutdown();

#ifdef _WIN32
	WSAStartupSingleton::Deref();
#endif
}
void UDPForwarder::Startup(void)
{
}
void UDPForwarder::Shutdown(void)
{
	forwardList.ClearPointers(true,__FILE__,__LINE__);
}

void UDPForwarder::Update(void)
{
	fd_set      readFD;
	//fd_set exceptionFD;
	FD_ZERO(&readFD);
//	FD_ZERO(&exceptionFD);
	timeval tv;
	int selectResult;
	tv.tv_sec=0;
	tv.tv_usec=0;

	RakNetTime curTime = RakNet::GetTime();

	SOCKET largestDescriptor=0;
	DataStructures::DefaultIndexType i;

	// Remove unused entries
	i=0;
	while (i < forwardList.GetSize())
	{
		if (curTime > forwardList[i]->timeLastDatagramForwarded && // Account for timestamp wrap
			curTime > forwardList[i]->timeLastDatagramForwarded+forwardList[i]->timeoutOnNoDataMS)
		{
			RakNet::OP_DELETE(forwardList[i],__FILE__,__LINE__);
			forwardList.RemoveAtIndex(i,__FILE__,__LINE__);
		}
		else
			i++;
	}

	for (i=0; i < forwardList.GetSize(); i++)
	{
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
		FD_SET(forwardList[i]->readSocket, &readFD);
//		FD_SET(forwardList[i]->readSocket, &exceptionFD);
		largestDescriptor = forwardList[i]->readSocket;
	}

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
	selectResult=socketselect(largestDescriptor+1, &readFD, 0, 0, &tv);
#else
	selectResult=(int) select((int) largestDescriptor+1, &readFD, 0, 0, &tv);		
#endif

	if (selectResult > 0)
	{
		for (i=0; i < forwardList.GetSize(); i++)
		{
			if (FD_ISSET(forwardList[i]->readSocket, &readFD))
			{
				const int flag=0;
				int receivedDataLen, len=0;
				char data[ MAXIMUM_MTU_SIZE ];
				sockaddr_in sa;
				socklen_t len2;
				unsigned short portnum=0;
				len2 = sizeof( sa );
				sa.sin_family = AF_INET;
				receivedDataLen = recvfrom( forwardList[i]->readSocket, data, MAXIMUM_MTU_SIZE, flag, ( sockaddr* ) & sa, ( socklen_t* ) & len2 );
				portnum = ntohs( sa.sin_port );
				if (forwardList[i]->srcAndDest.source.binaryAddress==sa.sin_addr.s_addr &&
					forwardList[i]->srcAndDest.source.port==portnum)
				{
					// Forward to destination
					len=0;
					sockaddr_in saOut;
					saOut.sin_port = htons( forwardList[i]->srcAndDest.destination.port ); // User port
					saOut.sin_addr.s_addr = forwardList[i]->srcAndDest.destination.binaryAddress;
					saOut.sin_family = AF_INET;
					do
					{
						len = sendto( forwardList[i]->writeSocket, data, receivedDataLen, 0, ( const sockaddr* ) & saOut, sizeof( saOut ) );
					}
					while ( len == 0 );

					forwardList[i]->timeLastDatagramForwarded=curTime;
				}
			}
		}
	}
}
void UDPForwarder::SetMaxForwardEntries(unsigned short maxEntries)
{
	RakAssert(maxEntries>0 && maxEntries<65535/2);
	maxForwardEntries=maxEntries;
}
int UDPForwarder::GetMaxForwardEntries(void) const
{
	return maxForwardEntries;
}
int UDPForwarder::GetUsedForwardEntries(void) const
{
	return (int) forwardList.GetSize();
}
unsigned short UDPForwarder::AddForwardingEntry(SrcAndDest srcAndDest, RakNetTimeMS timeoutOnNoDataMS)
{
	DataStructures::DefaultIndexType insertionIndex;
	insertionIndex = forwardList.GetInsertionIndex(srcAndDest);
	if (insertionIndex!=(DataStructures::DefaultIndexType)-1)
	{
		int sock_opt;
		sockaddr_in listenerSocketAddress;
		listenerSocketAddress.sin_port = 0;
		ForwardEntry *fe = RakNet::OP_NEW<UDPForwarder::ForwardEntry>(__FILE__,__LINE__);
		fe->srcAndDest=srcAndDest;
		fe->timeoutOnNoDataMS=timeoutOnNoDataMS;
		fe->readSocket = socket( AF_INET, SOCK_DGRAM, 0 );

		// This doubles the max throughput rate
		sock_opt=1024*256;
		setsockopt(fe->readSocket, SOL_SOCKET, SO_RCVBUF, ( char * ) & sock_opt, sizeof ( sock_opt ) );

		// Immediate hard close. Don't linger the readSocket, or recreating the readSocket quickly on Vista fails.
		sock_opt=0;
		setsockopt(fe->readSocket, SOL_SOCKET, SO_LINGER, ( char * ) & sock_opt, sizeof ( sock_opt ) );

		listenerSocketAddress.sin_family = AF_INET;
		listenerSocketAddress.sin_addr.s_addr = INADDR_ANY;

		int ret = bind( fe->readSocket, ( struct sockaddr * ) & listenerSocketAddress, sizeof( listenerSocketAddress ) );
		if (ret==-1)
		{
			RakNet::OP_DELETE(fe,__FILE__,__LINE__);
			return 0;
		}

		DataStructures::DefaultIndexType oldSize = forwardList.GetSize();
		forwardList.InsertAtIndex(fe,insertionIndex,__FILE__,__LINE__);
		RakAssert(forwardList.GetSize()==oldSize+1);
		return SocketLayer::GetLocalPort ( fe->readSocket );
	}
	return 0;
}
bool UDPForwarder::StartForwarding(SystemAddress source, SystemAddress destination, RakNetTimeMS timeoutOnNoDataMS,
								   unsigned short *srcToDestPort, unsigned short *destToSourcePort)
{
	// Invalid parameters?
	if (timeoutOnNoDataMS == 0 || timeoutOnNoDataMS > UDP_FORWARDER_MAXIMUM_TIMEOUT || source==UNASSIGNED_SYSTEM_ADDRESS || destination==UNASSIGNED_SYSTEM_ADDRESS)
		return false;

	SrcAndDest srcAndDest;
	srcAndDest.destination=destination;
	srcAndDest.source=source;
	SrcAndDest destAndSrc;
	destAndSrc.destination=source;
	destAndSrc.source=destination;

	*srcToDestPort = AddForwardingEntry(srcAndDest, timeoutOnNoDataMS);

	if (*srcToDestPort==0)
		return false;

	*destToSourcePort = AddForwardingEntry(destAndSrc, timeoutOnNoDataMS);

	if (*destToSourcePort==0)
		return false;

	DataStructures::DefaultIndexType idxSrcAndDest, idxDestAndSrc;
	idxSrcAndDest = forwardList.GetIndexOf(srcAndDest);
	idxDestAndSrc = forwardList.GetIndexOf(destAndSrc);
	forwardList[idxSrcAndDest]->writeSocket=forwardList[idxDestAndSrc]->readSocket;
	forwardList[idxDestAndSrc]->writeSocket=forwardList[idxSrcAndDest]->readSocket;

	return true;

}
void UDPForwarder::StopForwarding(SystemAddress source, SystemAddress destination)
{
	SrcAndDest srcAndDest;
	srcAndDest.destination=destination;
	srcAndDest.source=source;
	DataStructures::DefaultIndexType idx = forwardList.GetIndexOf(srcAndDest);
	if (idx!=(DataStructures::DefaultIndexType)-1)
	{
		RakNet::OP_DELETE(forwardList[idx],__FILE__,__LINE__);
		forwardList.RemoveAtIndex(idx,__FILE__,__LINE__);
	}

	srcAndDest.destination=source;
	srcAndDest.source=destination;

	idx = forwardList.GetIndexOf(srcAndDest);
	if (idx!=(DataStructures::DefaultIndexType)-1)
	{
		RakNet::OP_DELETE(forwardList[idx],__FILE__,__LINE__);
		forwardList.RemoveAtIndex(idx,__FILE__,__LINE__);
	}
}
