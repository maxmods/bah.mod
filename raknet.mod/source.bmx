' Copyright (c) 2007-2009 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import BRL.Blitz

?win32
Import "include/*.h"
?
Import "src/*.h"

Import "src/_FindFirst.cpp"
Import "src/WSAStartupSingleton.cpp"
Import "src/AsynchronousFileIO.cpp"
Import "src/AutoRPC.cpp"
Import "src/BigInt.cpp"
Import "src/BitStream_NoTemplate.cpp"
Import "src/BitStream.cpp"
Import "src/CheckSum.cpp"
Import "src/CommandParserInterface.cpp"
Import "src/ConnectionGraph.cpp"
Import "src/ConnectionGraph2.cpp"
Import "src/ConsoleServer.cpp"
Import "src/DataBlockEncryptor.cpp"
Import "src/DataCompressor.cpp"
Import "src/DirectoryDeltaTransfer.cpp"
Import "src/DS_BytePool.cpp"
Import "src/DS_ByteQueue.cpp"
Import "src/DS_HuffmanEncodingTree.cpp"
Import "src/DS_Table.cpp"
Import "src/EmailSender.cpp"
Import "src/EncodeClassName.cpp"
Import "src/EpochTimeToString.cpp"
Import "src/ExtendedOverlappedPool.cpp"
Import "src/FileList.cpp"
Import "src/FileListTransfer.cpp"
Import "src/FileOperations.cpp"
Import "src/FormatString.cpp"
Import "src/FullyConnectedMesh.cpp"
Import "src/FullyConnectedMesh2.cpp"
Import "src/FunctionThread.cpp"
Import "src/Gen_RPC8.cpp"
Import "src/GetTime.cpp"
Import "src/gettimeofday.cpp"
Import "src/GridSectorizer.cpp"
Import "src/HTTPConnection.cpp"
Import "src/IncrementalReadInterface.cpp"
Import "src/InlineFunctor.cpp"
Import "src/Itoa.cpp"
Import "src/LightweightDatabaseClient.cpp"
Import "src/LightweightDatabaseCommon.cpp"
Import "src/LightweightDatabaseServer.cpp"
Import "src/LinuxStrings.cpp"
Import "src/LogCommandParser.cpp"
Import "src/MessageFilter.cpp"
Import "src/NatPunchthroughServer.cpp"
Import "src/NatPunchthroughClient.cpp"
Import "src/NetworkIDManager.cpp"
Import "src/NetworkIDObject.cpp"
Import "src/PacketConsoleLogger.cpp"
Import "src/PacketFileLogger.cpp"
Import "src/PacketizedTCP.cpp"
Import "src/PacketLogger.cpp"
Import "src/PluginInterface.cpp"
Import "src/PluginInterface2.cpp"
Import "src/RakMemoryOverride.cpp"
Import "src/RakNetCommandParser.cpp"
Import "src/RakNetSocket.cpp"
Import "src/RakNetStatistics.cpp"
Import "src/RakNetTransport.cpp"
Import "src/RakNetTransport2.cpp"
Import "src/RakNetTypes.cpp"
Import "src/RakNetworkFactory.cpp"
Import "src/RakPeer.cpp"
Import "src/RakSleep.cpp"
Import "src/RakString.cpp"
Import "src/RakThread.cpp"
Import "src/Rand.cpp"
Import "src/ReadyEvent.cpp"
Import "src/ReliabilityLayer.cpp"
Import "src/ReplicaManager.cpp"
Import "src/ReplicaManager2.cpp"
Import "src/rijndael.cpp"
Import "src/Router.cpp"
Import "src/RPCMap.cpp"
Import "src/RSACrypt.cpp"
Import "src/SHA1.cpp"
Import "src/SimpleMutex.cpp"
Import "src/SocketLayer.cpp"
Import "src/StringCompressor.cpp"
Import "src/StringTable.cpp"
Import "src/SuperFastHash.cpp"
Import "src/SystemAddressList.cpp"
Import "src/TableSerializer.cpp"
Import "src/TCPInterface.cpp"
Import "src/TelnetTransport.cpp"
Import "src/ThreadsafePacketLogger.cpp"
Import "src/UDPForwarder.cpp"
Import "src/UDPProxyClient.cpp"
Import "src/UDPProxyCoordinator.cpp"
Import "src/UDPProxyServer.cpp"
Import "src/VariadicSQLParser.cpp"


Import "glue.cpp"

	


