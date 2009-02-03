/// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "LogCommandParser.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakPeer.h"
#include "ConsoleServer.h"
#include "PacketLogger.h"
#include "RakNetCommandParser.h"
#include "ReplicaManager.h"
#include "RakNetTransport.h"
#include "TelnetTransport.h"
#include "PacketConsoleLogger.h"
#include "PacketFileLogger.h"
#include "Router.h"
#include "ConnectionGraph.h"

RakPeerInterface* RakNetworkFactory::GetRakPeerInterface( void )
{
	return RakNet::OP_NEW<RakPeer>();
}
ConsoleServer* RakNetworkFactory::GetConsoleServer( void )
{
	return RakNet::OP_NEW<ConsoleServer>();
}
ReplicaManager* RakNetworkFactory::GetReplicaManager( void )
{
	return RakNet::OP_NEW<ReplicaManager>();
}
LogCommandParser* RakNetworkFactory::GetLogCommandParser( void )
{
	return RakNet::OP_NEW<LogCommandParser>();
}
PacketLogger* RakNetworkFactory::GetPacketLogger( void )
{
	return RakNet::OP_NEW<PacketLogger>();
}
RakNetCommandParser* RakNetworkFactory::GetRakNetCommandParser( void )
{
	return RakNet::OP_NEW<RakNetCommandParser>();
}
RakNetTransport* RakNetworkFactory::GetRakNetTransport( void )
{
	return RakNet::OP_NEW<RakNetTransport>();
}
TelnetTransport* RakNetworkFactory::GetTelnetTransport( void )
{
	return RakNet::OP_NEW<TelnetTransport>();
}
PacketConsoleLogger* RakNetworkFactory::GetPacketConsoleLogger( void )
{
	return RakNet::OP_NEW<PacketConsoleLogger>();
}
PacketFileLogger* RakNetworkFactory::GetPacketFileLogger( void )
{
	return RakNet::OP_NEW<PacketFileLogger>();
}
Router* RakNetworkFactory::GetRouter( void )
{
	return RakNet::OP_NEW<Router>();
}
ConnectionGraph* RakNetworkFactory::GetConnectionGraph( void )
{
	return RakNet::OP_NEW<ConnectionGraph>();
}
void RakNetworkFactory::DestroyRakPeerInterface( RakPeerInterface* i )
{
	RakNet::OP_DELETE(( RakPeer* ) i);
}
void RakNetworkFactory::DestroyConsoleServer( ConsoleServer* i)
{
	RakNet::OP_DELETE(( ConsoleServer* ) i);
}
void RakNetworkFactory::DestroyReplicaManager( ReplicaManager* i)
{
	RakNet::OP_DELETE(( ReplicaManager* ) i);
}
void RakNetworkFactory::DestroyLogCommandParser( LogCommandParser* i)
{
	RakNet::OP_DELETE(( LogCommandParser* ) i);
}
void RakNetworkFactory::DestroyPacketLogger( PacketLogger* i)
{
	RakNet::OP_DELETE(( PacketLogger* ) i);
}
void RakNetworkFactory::DestroyRakNetCommandParser( RakNetCommandParser* i )
{
	RakNet::OP_DELETE(( RakNetCommandParser* ) i);
}
void RakNetworkFactory::DestroyRakNetTransport(  RakNetTransport* i )
{
	RakNet::OP_DELETE(( RakNetTransport* ) i);
}
void RakNetworkFactory::DestroyTelnetTransport(  TelnetTransport* i )
{
	RakNet::OP_DELETE(( TelnetTransport* ) i);
}
void RakNetworkFactory::DestroyPacketConsoleLogger(  PacketConsoleLogger* i )
{
	RakNet::OP_DELETE(( PacketConsoleLogger* ) i);
}
void RakNetworkFactory::DestroyPacketFileLogger(  PacketFileLogger* i )
{
	RakNet::OP_DELETE(( PacketFileLogger* ) i);
}
void RakNetworkFactory::DestroyRouter(  Router* i )
{
	RakNet::OP_DELETE(( Router* ) i);
}
void RakNetworkFactory::DestroyConnectionGraph(  ConnectionGraph* i )
{
	RakNet::OP_DELETE(( ConnectionGraph* ) i);
}
