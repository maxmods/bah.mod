/// \file
/// \brief Forwards UDP datagrams. Independent of RakNet's protocol.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __UDP_FORWARDER_H
#define __UDP_FORWARDER_H

#include "Export.h"
#include "DS_Multilist.h"
#include "RakNetTypes.h"
#include "SocketIncludes.h"
#include "UDPProxyCommon.h"

namespace RakNet
{

/// \brief Forwards UDP datagrams. Independent of RakNet's protocol.
/// \ingroup NAT_PUNCHTHROUGH_GROUP
class RAK_DLL_EXPORT UDPForwarder
{
public:
	UDPForwarder();
	~UDPForwarder();

	/// Starts the system.
	/// Required to call before StartForwarding
	void Startup(void);

	/// Stops the system, and frees all sockets
	void Shutdown(void);

	/// Call on a regular basis. Will call select() on all sockets and forward messages.
	void Update(void);

	/// Sets the maximum number of forwarding entries allowed
	/// Set according to your available bandwidth and the estimated average bandwidth per forwarded address.
	/// A single connection requires 2 entries, as connections are bi-directional.
	/// \param[in] maxEntries The maximum number of simultaneous forwarding entries. Defaults to 64 (32 connections)
	void SetMaxForwardEntries(unsigned short maxEntries);

	/// \return The \a maxEntries parameter passed to SetMaxForwardEntries(), or the default if it was never called
	int GetMaxForwardEntries(void) const;

	/// \note Each call to StartForwarding uses up two forwarding entries, since communications are bidirectional
	/// \return How many entries have been used
	int GetUsedForwardEntries(void) const;

	/// Forwards datagrams from source to destination, and vice-versa
	/// Does nothing if this forward entry already exists via a previous call
	/// \pre Call Startup()
	/// \note RakNet's protocol will ensure a message is sent at least every 5 seconds, so if routing RakNet messages, it is a reasonable value for timeoutOnNoDataMS, plus an extra few seconds for latency
	/// \param[in] source The source IP and port
	/// \param[in] destination Where to forward to (and vice-versa)
	/// \param[in] timeoutOnNoDataMS If no messages are forwarded for this many MS, then automatically remove this entry. Currently hardcoded to UDP_FORWARDER_MAXIMUM_TIMEOUT (else the call fails)
	/// \param[out] srcToDestPort Port to go from source to destination
	/// \param[out] destToSourcePort Port to go from destination to source
	/// \return false on failure (can't bind the sockets). True otherwise, with \a srcToDestPortOut and \a destToSourcePortOut written
	bool StartForwarding(SystemAddress source, SystemAddress destination, RakNetTimeMS timeoutOnNoDataMS,
		unsigned short *srcToDestPort, unsigned short *destToSourcePort);

	/// No longer forward datagrams from source to destination
	/// \param[in] source The source IP and port
	/// \param[in] destination Where to forward to
	void StopForwarding(SystemAddress source, SystemAddress destination);

	struct SrcAndDest
	{
		SystemAddress source;
		SystemAddress destination;
	};

	struct ForwardEntry
	{
		ForwardEntry();
		~ForwardEntry();
		SrcAndDest srcAndDest;
		RakNetTimeMS timeLastDatagramForwarded;
		SOCKET readSocket;
		SOCKET writeSocket;
		RakNetTimeMS timeoutOnNoDataMS;
	};

protected:
	DataStructures::Multilist<ML_ORDERED_LIST, ForwardEntry*, SrcAndDest> forwardList;
	unsigned short maxForwardEntries;

	unsigned short AddForwardingEntry(SrcAndDest srcAndDest, RakNetTimeMS timeoutOnNoDataMS);
};

} // End namespace

#endif
