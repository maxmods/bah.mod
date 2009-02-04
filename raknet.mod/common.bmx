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

Import BRL.Map

?win32
Import "-lws2_32"
?

Import "source.bmx"


Extern

	Function bmx_raknet_seedMT(seed:Int)
	Function bmx_raknet_gettime(v:Long Ptr)
	Function bmx_raknet_gettimens(v:Long Ptr)
	Function bmx_raknet_randomMT:Int()
	Function bmx_raknet_frandomMT:Float()
	Function bmx_raknet_fillBufferMT(buffer:Byte Ptr, size:Int)

	Function bmx_RakNetworkFactory_GetRakPeerInterface:Byte Ptr()
	Function bmx_RakNetworkFactory_GetConsoleServer:Byte Ptr()
	Function bmx_RakNetworkFactory_GetReplicaManager:Byte Ptr()
	Function bmx_RakNetworkFactory_GetLogCommandParser:Byte Ptr()
	Function bmx_RakNetworkFactory_GetPacketLogger:Byte Ptr()
	Function bmx_RakNetworkFactory_GetRakNetCommandParser:Byte Ptr()
	Function bmx_RakNetworkFactory_GetRakNetTransport:Byte Ptr()
	Function bmx_RakNetworkFactory_GetTelnetTransport:Byte Ptr()
	Function bmx_RakNetworkFactory_GetPacketConsoleLogger:Byte Ptr()
	Function bmx_RakNetworkFactory_GetPacketFileLogger:Byte Ptr()
	Function bmx_RakNetworkFactory_GetRouter:Byte Ptr()
	Function bmx_RakNetworkFactory_GetConnectionGraph:Byte Ptr()
	Function bmx_RakNetworkFactory_DestroyRakPeerInterface(handle:Byte Ptr)

	Function bmx_SocketDescriptor_new:Byte Ptr(port:Int, hostAddress:String)
	Function bmx_SocketDescriptor_delete(handle:Byte Ptr)
	Function bmx_SocketDescriptor_setport(handle:Byte Ptr, port:Int)
	Function bmx_SocketDescriptor_getport:Int(handle:Byte Ptr)
	Function bmx_SocketDescriptor_sethostaddress(handle:Byte Ptr, hostAddress:String)
	Function bmx_SocketDescriptor_gethostaddress:String(handle:Byte Ptr)

	Function bmx_RakPeer_Startup:Int(handle:Byte Ptr, maxConnections:Int, threadSleepTimer:Int, descriptor:Byte Ptr)
	Function bmx_RakPeer_Receive:Byte Ptr(handle:Byte Ptr)
	Function bmx_RakPeer_DeallocatePacket(handle:Byte Ptr, Packet:Byte Ptr)
	Function bmx_RakPeer_SetMaximumIncomingConnections(handle:Byte Ptr, numberAllowed:Int)
	Function bmx_RakPeer_Connect:Int(handle:Byte Ptr, host:String, remotePort:Int, passwordData:String)
	Function bmx_RakPeer_RegisterAsRemoteProcedureCall(handle:Byte Ptr, id:String, rpcCallback(params:Byte Ptr))
	Function bmx_RakPeer_DisableSecurity(handle:Byte Ptr)
	Function bmx_RakPeer_GetMaximumIncomingConnections:Int(handle:Byte Ptr)
	Function bmx_RakPeer_NumberOfConnections:Int(handle:Byte Ptr)
	Function bmx_RakPeer_SetIncomingPassword(handle:Byte Ptr, passwordData:String)
	Function bmx_RakPeer_GetIncomingPassword:String(handle:Byte Ptr)
	Function bmx_RakPeer_Shutdown(handle:Byte Ptr, blockDuration:Int, orderingChannel:Int)
	Function bmx_RakPeer_IsActive:Int(handle:Byte Ptr)
	Function bmx_RakPeer_UnregisterAsRemoteProcedureCall(handle:Byte Ptr, name:String)
	Function bmx_RakPeer_RPC(handle:Byte Ptr, id:Byte Ptr, data:Byte Ptr, bitLength:Int, priority:Int, reliability:Int, ..
		orderingChannel:Int, addr:Byte Ptr, broadcast:Int, includedTimestamp:Long Ptr, ..
		replyFromTarget:Byte Ptr)
	Function bmx_RakPeer_AddToSecurityExceptionList(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_RemoveFromSecurityExceptionList(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_IsInSecurityExceptionList:Int(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_GetMaximumNumberOfPeers:Int(handle:Byte Ptr)
	Function bmx_RakPeer_InitializeSecurity(handle:Byte Ptr, pubKeyE:Int Ptr, pubKeyN:Byte Ptr, privKeyP:Byte Ptr, privKeyQ:Byte Ptr)
	Function bmx_RakPeer_SendBitStream:Int(handle:Byte Ptr, bitStream:Byte Ptr, priority:Int, reliability:Int, ..
		orderingChannel:Int, systemAddress:Byte Ptr, broadcast:Int)
	Function bmx_RakPeer_SetOccasionalPing(handle:Byte Ptr, doPing:Int)
	Function bmx_RakPeer_GetSystemAddressFromIndex:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_RakPeer_CloseConnection(handle:Byte Ptr, target:Byte Ptr, sendDisconnectionNotification:Int, orderingChannel:Int)
	Function bmx_RakPeer_IsConnected:Int(handle:Byte Ptr, addr:Byte Ptr, includeInProgress:Int, includeDisconnecting:Int)
	Function bmx_RakPeer_GetIndexFromSystemAddress:Int(handle:Byte Ptr, addr:Byte Ptr)
	Function bmx_RakPeer_AddToBanList(handle:Byte Ptr, ip:String, milliseconds:Long)
	Function bmx_RakPeer_RemoveFromBanList(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_ClearBanList(handle:Byte Ptr)
	Function bmx_RakPeer_IsBanned:Int(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_Ping(handle:Byte Ptr, addr:Byte Ptr)
	Function bmx_RakPeer_GetAveragePing:Int(handle:Byte Ptr, addr:Byte Ptr)
	Function bmx_RakPeer_GetLastPing:Int(handle:Byte Ptr, addr:Byte Ptr)
	Function bmx_RakPeer_GetLowestPing:Int(handle:Byte Ptr, addr:Byte Ptr)
	Function bmx_RakPeer_SetOfflinePingResponse(handle:Byte Ptr, data:String)
	Function bmx_RakPeer_GetOfflinePingResponse:String(handle:Byte Ptr)
	Function bmx_RakPeer_SetCompileFrequencyTable(handle:Byte Ptr, doCompile:Int)
	Function bmx_RakPeer_AttachPlugin(handle:Byte Ptr, plugin:Byte Ptr)
	Function bmx_RakPeer_DetachPlugin(handle:Byte Ptr, plugin:Byte Ptr)
	Function bmx_RakPeer_Send:Int(handle:Byte Ptr, data:Byte Ptr, length:Int, priority:Int, reliability:Int, orderingChannel:Int, systemAddress:Byte Ptr, broadcast:Int)
	Function bmx_RakPeer_GetLocalIP:String(handle:Byte Ptr, index:Int)
	Function bmx_RakPeer_IsLocalIP:Int(handle:Byte Ptr, ip:String)
	Function bmx_RakPeer_GetGuidFromSystemAddress:Byte Ptr(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_RakPeer_GetStatistics:Byte Ptr(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_RakPeer_GetInternalID:Byte Ptr(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_RakPeer_GetExternalID:Byte Ptr(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_RakPeer_SetTimeoutTime(handle:Byte Ptr, timeMS:Long, target:Byte Ptr)
	Function bmx_RakPeer_SetMTUSize:Int(handle:Byte Ptr, size:Int)
	Function bmx_RakPeer_GetMTUSize:Int(handle:Byte Ptr, target:Byte Ptr)
	Function bmx_RakPeer_GetNumberOfAddresses:Int(handle:Byte Ptr)
	Function bmx_RakPeer_DeleteCompressionLayer:Int(handle:Byte Ptr, inputLayer:Int)
	Function bmx_RakPeer_GetCompressionRatio:Float(handle:Byte Ptr)
	Function bmx_RakPeer_GetDecompressionRatio:Float(handle:Byte Ptr)
	Function bmx_RakPeer_ApplyNetworkSimulator(handle:Byte Ptr, maxSendBPS:Double, minExtraPing:Int, extraPingVariance:Int)
	Function bmx_RakPeer_SetPerConnectionOutgoingBandwidthLimit(handle:Byte Ptr, maxBitsPerSecond:Int)
	Function bmx_RakPeer_IsNetworkSimulatorActive:Int(handle:Byte Ptr)
	Function bmx_RakPeer_AllowConnectionResponseIPMigration(handle:Byte Ptr, allow:Int)
	Function bmx_RakPeer_AdvertiseSystem:Int(handle:Byte Ptr, host:String, remotePort:Int, data:Byte Ptr, dataLength:Int, connectionSocketIndex:Int)
	Function bmx_RakPeer_SetSplitMessageProgressInterval(handle:Byte Ptr, interval:Int)
	Function bmx_RakPeer_SetUnreliableTimeout(handle:Byte Ptr, timeoutMS:Long)
	Function bmx_RakPeer_SendTTL(handle:Byte Ptr, host:String, remotePort:Int, ttl:Int, connectionSocketIndex:Int)
	Function bmx_RakPeer_PushBackPacket(handle:Byte Ptr, packet:Byte Ptr, pushAtHead:Int)
	Function bmx_RakPeer_AllocatePacket:Byte Ptr(handle:Byte Ptr, dataSize:Int)
	Function bmx_RakPeer_PingHost(handle:Byte Ptr, host:String, remotePort:Int, onlyReplyOnAcceptingConnections:Int, connectionSocketIndex:Int)

	Function bmx_Packet_GetData:Byte Ptr(handle:Byte Ptr)
	Function bmx_Packet_GetBitSize:Int(handle:Byte Ptr)
	Function bmx_Packet_GetSystemAddress:Byte Ptr(handle:Byte Ptr)
	Function bmx_Packet_GetPacketIdentifier:Int(handle:Byte Ptr)
	Function bmx_Packet_GetGuid:Byte Ptr(handle:Byte Ptr)

	Function bmx_RPCParameters_GetFunctionName:String(handle:Byte Ptr)
	Function bmx_RPCParameters_GetInput:Byte Ptr(handle:Byte Ptr)
	Function bmx_RPCParameters_GetNumberOfBitsOfData:Int(handle:Byte Ptr)
	Function bmx_RPCParameters_GetSender:Byte Ptr(handle:Byte Ptr)
	Function bmx_RPCParameters_GetRecipient:Byte Ptr(handle:Byte Ptr)
	Function bmx_RPCParameters_GetReplyToSender:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_NetworkID_delete(handle:Byte Ptr)
	Function bmx_NetworkID_unassigned:Byte Ptr()

	Function bmx_SystemAddress_delete(handle:Byte Ptr)
	Function bmx_SystemAddress_unassigned:Byte Ptr()
	Function bmx_SystemAddress_Equals:Int(handle:Byte Ptr, address:Byte Ptr)
	Function bmx_SystemAddress_ToString:String(handle:Byte Ptr)
	Function bmx_SystemAddress_GetBinaryAddress:Int(handle:Byte Ptr)
	Function bmx_SystemAddress_GetPort:Int(handle:Byte Ptr)

	Function bmx_BitStream_Create:Byte Ptr()
	Function bmx_BitStream_CreateFromData:Byte Ptr(data:Byte Ptr, size:Int, copy:Int)
	Function bmx_BitStream_Delete(handle:Byte Ptr)
	Function bmx_BitStream_Reset(handle:Byte Ptr)
	Function bmx_BitStream_SerializeByte:Int(handle:Byte Ptr, writeToBitstream:Int, value:Byte Ptr)
	Function bmx_BitStream_SerializeShort:Int(handle:Byte Ptr, writeToBitstream:Int, value:Short Ptr)
	Function bmx_BitStream_SerializeInt:Int(handle:Byte Ptr, writeToBitstream:Int, value:Int Ptr)
	Function bmx_BitStream_SerializeFloat:Int(handle:Byte Ptr, writeToBitstream:Int, value:Float Ptr)
	Function bmx_BitStream_SerializeDouble:Int(handle:Byte Ptr, writeToBitstream:Int, value:Double Ptr)
	Function bmx_BitStream_SerializeDeltaLastByte:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Byte Ptr, lastValue:Byte)
	Function bmx_BitStream_SerializeDeltaLastShort:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Short Ptr, lastValue:Short)
	Function bmx_BitStream_SerializeDeltaLastInt:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Int Ptr, lastValue:Int)
	Function bmx_BitStream_SerializeDeltaLastFloat:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Float Ptr, lastValue:Float)
	Function bmx_BitStream_SerializeDeltaLastDouble:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Double Ptr, lastValue:Double)
	Function bmx_BitStream_SerializeDeltaByte:Int(handle:Byte Ptr, writeToBitstream:Int, value:Byte Ptr)
	Function bmx_BitStream_SerializeDeltaShort:Int(handle:Byte Ptr, writeToBitstream:Int, value:Short Ptr)
	Function bmx_BitStream_SerializeDeltaInt:Int(handle:Byte Ptr, writeToBitstream:Int, value:Int Ptr)
	Function bmx_BitStream_SerializeDeltaFloat:Int(handle:Byte Ptr, writeToBitstream:Int, value:Float Ptr)
	Function bmx_BitStream_SerializeDeltaDouble:Int(handle:Byte Ptr, writeToBitstream:Int, value:Double Ptr)
	Function bmx_BitStream_SerializeCompressedByte:Int(handle:Byte Ptr, writeToBitstream:Int, value:Byte Ptr)
	Function bmx_BitStream_SerializeCompressedShort:Int(handle:Byte Ptr, writeToBitstream:Int, value:Short Ptr)
	Function bmx_BitStream_SerializeCompressedInt:Int(handle:Byte Ptr, writeToBitstream:Int, value:Int Ptr)
	Function bmx_BitStream_SerializeCompressedFloat:Int(handle:Byte Ptr, writeToBitstream:Int, value:Float Ptr)
	Function bmx_BitStream_SerializeCompressedDouble:Int(handle:Byte Ptr, writeToBitstream:Int, value:Double Ptr)
	Function bmx_BitStream_SerializeCompressedDeltaLastByte:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Byte Ptr, lastValue:Byte)
	Function bmx_BitStream_SerializeCompressedDeltaLastShort:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Short Ptr, lastValue:Short)
	Function bmx_BitStream_SerializeCompressedDeltaLastInt:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Int Ptr, lastValue:Int)
	Function bmx_BitStream_SerializeCompressedDeltaLastFloat:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Float Ptr, lastValue:Float)
	Function bmx_BitStream_SerializeCompressedDeltaLastDouble:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Double Ptr, lastValue:Double)
	Function bmx_BitStream_SerializeCompressedDeltaByte:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Byte Ptr)
	Function bmx_BitStream_SerializeCompressedDeltaShort:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Short Ptr)
	Function bmx_BitStream_SerializeCompressedDeltaInt:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Int Ptr)
	Function bmx_BitStream_SerializeCompressedDeltaFloat:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Float Ptr)
	Function bmx_BitStream_SerializeCompressedDeltaDouble:Int(handle:Byte Ptr, writeToBitstream:Int, currentValue:Double Ptr)
	Function bmx_BitStream_ReadBool:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadByte:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_BitStream_ReadShort:Int(handle:Byte Ptr, value:Short Ptr)
	Function bmx_BitStream_ReadInt:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadFloat:Int(handle:Byte Ptr, value:Float Ptr)
	Function bmx_BitStream_ReadDouble:Int(handle:Byte Ptr, value:Double Ptr)
	Function bmx_BitStream_ReadLong:Int(handle:Byte Ptr, value:Long Ptr)
	Function bmx_BitStream_ReadTime:Int(handle:Byte Ptr, value:Long Ptr)
	Function bmx_BitStream_ReadString:String(handle:Byte Ptr, length:Int)
	Function bmx_BitStream_ReadDeltaByte:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_BitStream_ReadDeltaShort:Int(handle:Byte Ptr, value:Short Ptr)
	Function bmx_BitStream_ReadDeltaInt:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadDeltaFloat:Int(handle:Byte Ptr, value:Float Ptr)
	Function bmx_BitStream_ReadDeltaDouble:Int(handle:Byte Ptr, value:Double Ptr)
	Function bmx_BitStream_ReadCompressedByte:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_BitStream_ReadCompressedShort:Int(handle:Byte Ptr, value:Short Ptr)
	Function bmx_BitStream_ReadCompressedInt:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadCompressedFloat:Int(handle:Byte Ptr, value:Float Ptr)
	Function bmx_BitStream_ReadCompressedDouble:Int(handle:Byte Ptr, value:Double Ptr)
	Function bmx_BitStream_ReadCompressedDeltaByte:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_BitStream_ReadCompressedDeltaShort:Int(handle:Byte Ptr, value:Short Ptr)
	Function bmx_BitStream_ReadCompressedDeltaInt:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadCompressedDeltaFloat:Int(handle:Byte Ptr, value:Float Ptr)
	Function bmx_BitStream_ReadCompressedDeltaDouble:Int(handle:Byte Ptr, value:Double Ptr)
	Function bmx_BitStream_WriteByte(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_BitStream_WriteShort(handle:Byte Ptr, value:Short Ptr)
	Function bmx_BitStream_WriteInt(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_WriteFloat(handle:Byte Ptr, value:Float Ptr)
	Function bmx_BitStream_WriteDouble(handle:Byte Ptr, value:Double Ptr)
	Function bmx_BitStream_WriteLong(handle:Byte Ptr, value:Long Ptr)
	Function bmx_BitStream_WriteTime(handle:Byte Ptr, value:Long Ptr)

	Function bmx_BitStream_ReadDeltaBool:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadCompressedBool:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_ReadCompressedDeltaBool:Int(handle:Byte Ptr, value:Int Ptr)
	Function bmx_BitStream_AssertStreamEmpty(handle:Byte Ptr)
	Function bmx_BitStream_IgnoreBits(handle:Byte Ptr, numberOfBits:Int)
	Function bmx_BitStream_IgnoreBytes(handle:Byte Ptr, numberOfBytes:Int)
	Function bmx_BitStream_SetWriteOffset(handle:Byte Ptr, offset:Int)

	Function bmx_RSACrypt_new:Byte Ptr()
	Function bmx_RSACrypt_delete(handle:Byte Ptr)
	Function bmx_RSACrypt_generatePrivateKey:Int(handle:Byte Ptr, limbs:Int)
	Function bmx_RSACrypt_getFactorLimbs:Int(handle:Byte Ptr)
	Function bmx_RSACrypt_getPrivateP(handle:Byte Ptr, p:Byte Ptr)
	Function bmx_RSACrypt_getPrivateQ(handle:Byte Ptr, q:Byte Ptr)
	Function bmx_RSACrypt_getModLimbs:Int(handle:Byte Ptr)
	Function bmx_RSACrypt_getPublicModulus(handle:Byte Ptr, modulus:Byte Ptr)
	Function bmx_RSACrypt_getPublicExponent:Int(handle:Byte Ptr)

	Function bmx_FileListTransfer_new:Byte Ptr() 
	Function bmx_FileListTransfer_SetupReceive:Int(handle:Byte Ptr, handler:Byte Ptr, deleteHandler:Int, allowedSender:Byte Ptr)
	Function bmx_FileListTransfer_Send(handle:Byte Ptr, fileList:Byte Ptr, rakPeer:Byte Ptr, recipient:Byte Ptr, setID:Int, priority:Int, orderingChannel:Int, compressData:Int)
	Function bmx_FileListTransfer_CancelReceive(handle:Byte Ptr, setID:Int)
	Function bmx_FileListTransfer_RemoveReceiver(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_FileListTransfer_IsHandlerActive:Int(handle:Byte Ptr, setID:Int)
	Function bmx_FileListTransfer_delete(handle:Byte Ptr)

	Function bmx_MessageFilter_new:Byte Ptr()
	Function bmx_MessageFilter_delete(handle:Byte Ptr)
	Function bmx_MessageFilter_SetAutoAddNewConnectionsToFilter(handle:Byte Ptr, filterSetID:Int)
	Function bmx_MessageFilter_SetAllowMessageID(handle:Byte Ptr, allow:Int, messageIDStart:Int, messageIDEnd:Int, filterSetID:Int)
	Function bmx_MessageFilter_SetAllowRPC(handle:Byte Ptr, allow:Int, functionName:String, filterSetID:Int)
	Function bmx_MessageFilter_SetSystemFilterSet(handle:Byte Ptr, systemAddress:Byte Ptr, filterSetID:Int)
	Function bmx_MessageFilter_SetActionOnDisallowedMessage(handle:Byte Ptr, kickOnDisallowed:Int, banOnDisallowed:Int, banTimeMS:Long, filterSetID:Int)
	Function bmx_MessageFilter_SetFilterMaxTime(handle:Byte Ptr, allowedTimeMS:Int, banOnExceed:Int, banTimeMS:Long, filterSetID:Int)
	Function bmx_MessageFilter_GetSystemFilterSet:Int(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_MessageFilter_GetSystemCount:Int(handle:Byte Ptr, filterSetID:Int)
	Function bmx_MessageFilter_GetSystemByIndex:Byte Ptr(handle:Byte Ptr, filterSetID:Int, index:Int)
	Function bmx_MessageFilter_GetFilterSetCount:Int(handle:Byte Ptr)
	Function bmx_MessageFilter_GetFilterSetIDByIndex:Int(handle:Byte Ptr, index:Int)
	Function bmx_MessageFilter_DeleteFilterSet(handle:Byte Ptr, filterSetID:Int)

	Function bmx_RakNetGUID_ToString:String(handle:Byte Ptr)
	Function bmx_RakNetGUID_delete(handle:Byte Ptr)
	Function bmx_RakNetGUID_unassigned:Byte Ptr()

	Function bmx_RakNetStatistics_ToStringLevel:String(handle:Byte Ptr, verbosityLevel:Int)
	Function bmx_RakNetStatistics_messageSendBuffer:Int(handle:Byte Ptr, priority:Int)
	Function bmx_RakNetStatistics_messagesSent:Int(handle:Byte Ptr, priority:Int)
	Function bmx_RakNetStatistics_messageDataBitsSent(handle:Byte Ptr, priority:Int, v:Long Ptr)
	Function bmx_RakNetStatistics_messageTotalBitsSent(handle:Byte Ptr, priority:Int, v:Long Ptr)
	Function bmx_RakNetStatistics_packetsContainingOnlyAcknowlegements:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_acknowlegementsSent:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_acknowlegementsPending:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_acknowlegementBitsSent(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_packetsContainingOnlyAcknowlegementsAndResends:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_messageResends:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_messageDataBitsResent(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_messagesTotalBitsResent(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_messagesOnResendQueue:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_numberOfUnsplitMessages:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_numberOfSplitMessages:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_totalSplits:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_packetsSent:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_encryptionBitsSent(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_totalBitsSent(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_sequencedMessagesOutOfOrder:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_sequencedMessagesInOrder:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_orderedMessagesOutOfOrder:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_orderedMessagesInOrder:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_packetsReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_packetsWithBadCRCReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_bitsReceived(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_bitsWithBadCRCReceived(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_acknowlegementsReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_duplicateAcknowlegementsReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_messagesReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_invalidMessagesReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_duplicateMessagesReceived:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_messagesWaitingForReassembly:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_internalOutputQueueSize:Int(handle:Byte Ptr)
	Function bmx_RakNetStatistics_bitsPerSecond:Double(handle:Byte Ptr)
	Function bmx_RakNetStatistics_connectionStartTime(handle:Byte Ptr, v:Long Ptr)
	Function bmx_RakNetStatistics_bandwidthExceeded:Int(handle:Byte Ptr)

	Function bmx_raknet_SizeOfRakNetTime:Int()

	Function bmx_FileList_new:Byte Ptr()
	Function bmx_FileList_AddFilesFromDirectory(handle:Byte Ptr, applicationDirectory:String, subDirectory:String, writeHash:Int, writeData:Int, recursive:Int, context:Byte Ptr)
	Function bmx_FileList_Clear(handle:Byte Ptr)
	Function bmx_FileList_Deserialize:Int(handle:Byte Ptr, bitStream:Byte Ptr)
	Function bmx_FileList_ListMissingOrChangedFiles(handle:Byte Ptr, applicationDirectory:String, missingOrChangedFiles:Byte Ptr, alwaysWriteHash:Int, neverWriteHash:Int)
	Function bmx_FileList_GetDeltaToCurrent(handle:Byte Ptr, inputList:Byte Ptr, outputList:Byte Ptr, dirSubset:String, remoteSubdir:String)
	Function bmx_FileList_PopulateDataFromDisk(handle:Byte Ptr, applicationDirectory:String, writeFileData:Int, writeFileHash:Int, removeUnknownFiles:Int)
	Function bmx_FileList_WriteDataToDisk(handle:Byte Ptr, applicationDirectory:String)
	Function bmx_FileList_AddFileMemory(handle:Byte Ptr, filename:String, data:Byte Ptr, dataLength:Int, fileLength:Int, context:Byte Ptr, isAReference:Int)
	Function bmx_FileList_AddFile(handle:Byte Ptr, filepath:String, filename:String, context:Byte Ptr)
	Function bmx_FileList_DeleteFiles(handle:Byte Ptr, applicationDirectory:String)
	Function bmx_FileList_delete(handle:Byte Ptr)

	Function bmx_FileListNodeContext_new:Byte Ptr()
	Function bmx_FileListNodeContext_delete(handle:Byte Ptr)

End Extern

Rem
bbdoc: 16 * 4 * 8 = 512 bit. Used For InitializeSecurity()
End Rem
Const RAKNET_RSA_FACTOR_LIMBS:Int = 16
Const RAKNET_RSA_FACTOR_LIMBS_BYTES:Int = RAKNET_RSA_FACTOR_LIMBS * 4

Rem
bbdoc: Size, in bytes, of RakNetTime type.
End Rem
Global SizeOfRakNetTime:Int = bmx_raknet_SizeOfRakNetTime()

'
' RESERVED TYPES - DO NOT CHANGE THESE
' All types from RakPeer
'
' These types are never returned to the user.
Rem
bbdoc: Ping from a connected system.  Update timestamps (internal use only)
End Rem
Const ID_INTERNAL_PING:Byte = 0
Rem
bbdoc: Ping from an unconnected system.  Reply but do not update timestamps. (internal use only)
End Rem
Const ID_PING:Byte = 1
Rem
bbdoc: Ping from an unconnected system.  Only reply if we have open connections. Do not update timestamps. (internal use only)
End Rem
Const ID_PING_OPEN_CONNECTIONS:Byte = 2
Rem
bbdoc: Pong from a connected system.  Update timestamps (internal use only)
End Rem
Const ID_CONNECTED_PONG:Byte = 3
Rem
bbdoc: Asking for a new connection (internal use only)
End Rem
Const ID_CONNECTION_REQUEST:Byte = 4
Rem
bbdoc: Connecting to a secured server/peer (internal use only)
End Rem
Const ID_SECURED_CONNECTION_RESPONSE:Byte = 5
Rem
bbdoc: Connecting to a secured server/peer (internal use only)
End Rem
Const ID_SECURED_CONNECTION_CONFIRMATION:Byte = 6
Rem
bbdoc: Packet that tells us the packet contains an integer ID to name mapping for the remote system (internal use only)
End Rem
Const ID_RPC_MAPPING:Byte = 7
Rem
bbdoc: A reliable packet to detect lost connections (internal use only)
End Rem
Const ID_DETECT_LOST_CONNECTIONS:Byte = 8
Rem
bbdoc: Offline message so we know when to reset and start a new connection (internal use only)
End Rem
Const ID_OPEN_CONNECTION_REQUEST:Byte = 9
Rem
bbdoc: Offline message response so we know when to reset and start a new connection (internal use only)
End Rem
Const ID_OPEN_CONNECTION_REPLY:Byte = 10
Rem
bbdoc: Remote procedure call (internal use only)
End Rem
Const ID_RPC:Byte = 11
Rem
bbdoc: Remote procedure call reply, for RPCs that return data (internal use only)
End Rem
Const ID_RPC_REPLY:Byte = 12
Rem
bbdoc: RakPeer - Same as End Rem
const ID_ADVERTISE_SYSTEM, but intended for internal use rather than being passed to the user. Second byte indicates type. Used currently for NAT punchthrough for receiver port advertisement. See End Rem
const ID_NAT_ADVERTISE_RECIPIENT_PORT
End Rem
Const ID_OUT_OF_BAND_INTERNAL:Byte = 13


'
' USER TYPES - DO NOT CHANGE THESE
'

Rem
bbdoc: RakPeer - In a client/server environment, our connection request to the server has been accepted.
End Rem
Const ID_CONNECTION_REQUEST_ACCEPTED:Byte = 14
Rem
bbdoc: RakPeer - Sent to the player when a connection request cannot be completed due to inability to connect. 
End Rem
Const ID_CONNECTION_ATTEMPT_FAILED:Byte = 15
Rem
bbdoc: RakPeer - Sent a connect request to a system we are currently connected to.
End Rem
Const ID_ALREADY_CONNECTED:Byte = 16
Rem
bbdoc: RakPeer - A remote system has successfully connected.
End Rem
Const ID_NEW_INCOMING_CONNECTION:Byte = 17
Rem
bbdoc: RakPeer - The system we attempted to connect to is not accepting new connections.
End Rem
Const ID_NO_FREE_INCOMING_CONNECTIONS:Byte = 18
Rem
bbdoc: RakPeer - The system specified in Packet::systemAddress has disconnected from us.  For the client, this would mean the server has shutdown. 
End Rem
Const ID_DISCONNECTION_NOTIFICATION:Byte = 19
Rem
bbdoc: RakPeer - Reliable packets cannot be delivered to the system specified in Packet::systemAddress.  The connection to that system has been closed. 
End Rem
Const ID_CONNECTION_LOST:Byte = 20
Rem
bbdoc: RakPeer - We preset an RSA public key which does not match what the system we connected to is using.
End Rem
Const ID_RSA_PUBLIC_KEY_MISMATCH:Byte = 21
Rem
bbdoc: RakPeer - We are banned from the system we attempted to connect to.
End Rem
Const ID_CONNECTION_BANNED:Byte = 22
Rem
bbdoc: RakPeer - The remote system is using a password and has refused our connection because we did not set the correct password.
End Rem
Const ID_INVALID_PASSWORD:Byte = 23
Rem
bbdoc: RakPeer - A packet has been tampered with in transit.  The sender is contained in Packet::systemAddress.
End Rem
Const ID_MODIFIED_PACKET:Byte = 24
Rem
bbdoc: RakPeer - The 8 bytes following this byte represent an unsigned int which is automatically modified by the difference in system times between the sender and the recipient. Requires that you call SetOccasionalPing.
End Rem
Const ID_TIMESTAMP:Byte = 25
Rem
bbdoc: RakPeer - Pong from an unconnected system. 
about: First byte is ID_PONG, second 4 bytes is the ping, following bytes is system specific enumeration data.
End Rem
Const ID_PONG:Byte = 26
Rem
bbdoc: RakPeer - Inform a remote system of our IP/Port, plus some offline data
End Rem
Const ID_ADVERTISE_SYSTEM:Byte = 27
Rem
bbdoc: ConnectionGraph plugin - In a client/server environment, a client other than ourselves has disconnected gracefully.  Packet::systemAddress is modified to reflect the systemAddress of this client.
End Rem
Const ID_REMOTE_DISCONNECTION_NOTIFICATION:Byte = 28
Rem
bbdoc: ConnectionGraph plugin - In a client/server environment, a client other than ourselves has been forcefully dropped. Packet::systemAddress is modified to reflect the systemAddress of this client.
End Rem
Const ID_REMOTE_CONNECTION_LOST:Byte = 29
Rem
bbdoc: ConnectionGraph plugin - In a client/server environment, a client other than ourselves has connected.  Packet::systemAddress is modified to reflect the systemAddress of the client that is not connected directly to us. The packet encoding is SystemAddress 1, ConnectionGraphGroupID 1, SystemAddress 2, ConnectionGraphGroupID 2
End Rem
Const ID_REMOTE_NEW_INCOMING_CONNECTION:Byte = 30
Rem
bbdoc: RakPeer - Downloading a large message. Format is ID_DOWNLOAD_PROGRESS (MessageID), partCount (unsigned int), partTotal (unsigned int), partLength (unsigned int), first part data (length <= MAX_MTU_SIZE). See the three parameters partCount, partTotal and partLength in OnFileProgress in FileListTransferCBInterface.h
End Rem
Const ID_DOWNLOAD_PROGRESS:Byte = 31

Rem
bbdoc: FileListTransfer plugin - Setup data
End Rem
Const ID_FILE_LIST_TRANSFER_HEADER:Byte = 32
Rem
bbdoc: FileListTransfer plugin - A file
End Rem
Const ID_FILE_LIST_TRANSFER_FILE:Byte = 33

Rem
bbdoc: DirectoryDeltaTransfer plugin - Request from a remote system for a download of a directory
End Rem
Const ID_DDT_DOWNLOAD_REQUEST:Byte = 34

Rem
bbdoc: RakNetTransport plugin - Transport provider message, used for remote console
End Rem
Const ID_TRANSPORT_STRING:Byte = 35

Rem
bbdoc: ReplicaManager plugin - Create an object
End Rem
Const ID_REPLICA_MANAGER_CONSTRUCTION:Byte = 36
Rem
bbdoc: ReplicaManager plugin - Destroy an object
End Rem
Const ID_REPLICA_MANAGER_DESTRUCTION:Byte = 37
Rem
bbdoc: ReplicaManager plugin - Changed scope of an object
End Rem
Const ID_REPLICA_MANAGER_SCOPE_CHANGE:Byte = 38
Rem
bbdoc: ReplicaManager plugin - Serialized data of an object
End Rem
Const ID_REPLICA_MANAGER_SERIALIZE:Byte = 39
Rem
bbdoc: ReplicaManager plugin - New connection, about to send all world objects
End Rem
Const ID_REPLICA_MANAGER_DOWNLOAD_STARTED:Byte = 40
Rem
bbdoc: ReplicaManager plugin - Finished downloading all serialized objects
End Rem
Const ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:Byte = 41

Rem
bbdoc: ConnectionGraph plugin - Request the connection graph from another system
End Rem
Const ID_CONNECTION_GRAPH_REQUEST:Byte = 42
Rem
bbdoc: ConnectionGraph plugin - Reply to a connection graph download request
End Rem
Const ID_CONNECTION_GRAPH_REPLY:Byte = 43
Rem
bbdoc: ConnectionGraph plugin - Update edges / nodes for a system with a connection graph
End Rem
Const ID_CONNECTION_GRAPH_UPDATE:Byte = 44
Rem
bbdoc: ConnectionGraph plugin - Add a new connection to a connection graph
End Rem
Const ID_CONNECTION_GRAPH_NEW_CONNECTION:Byte = 45
Rem
bbdoc: ConnectionGraph plugin - Remove a connection from a connection graph - connection was abruptly lost
End Rem
Const ID_CONNECTION_GRAPH_CONNECTION_LOST:Byte = 46
Rem
bbdoc: ConnectionGraph plugin - Remove a connection from a connection graph - connection was gracefully lost
End Rem
Const ID_CONNECTION_GRAPH_DISCONNECTION_NOTIFICATION:Byte = 47

Rem
bbdoc: Router plugin - route a message through another system
End Rem
Const ID_ROUTE_AND_MULTICAST:Byte = 48

Rem
bbdoc: RakVoice plugin - Open a communication channel
End Rem
Const ID_RAKVOICE_OPEN_CHANNEL_REQUEST:Byte = 49
Rem
bbdoc: RakVoice plugin - Communication channel accepted
End Rem
Const ID_RAKVOICE_OPEN_CHANNEL_REPLY:Byte = 50
Rem
bbdoc: RakVoice plugin - Close a communication channel
End Rem
Const ID_RAKVOICE_CLOSE_CHANNEL:Byte = 51
Rem
bbdoc: RakVoice plugin - Voice data
End Rem
Const ID_RAKVOICE_DATA:Byte = 52

Rem
bbdoc: Autopatcher plugin - Get a list of files that have changed since a certain date
End Rem
Const ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE:Byte = 53
Rem
bbdoc: Autopatcher plugin - A list of files to create
End Rem
Const ID_AUTOPATCHER_CREATION_LIST:Byte = 54
Rem
bbdoc: Autopatcher plugin - A list of files to delete
End Rem
Const ID_AUTOPATCHER_DELETION_LIST:Byte = 55
Rem
bbdoc: Autopatcher plugin - A list of files to get patches for
End Rem
Const ID_AUTOPATCHER_GET_PATCH:Byte = 56
Rem
bbdoc: Autopatcher plugin - A list of patches for a list of files
End Rem
Const ID_AUTOPATCHER_PATCH_LIST:Byte = 57
Rem
bbdoc: Autopatcher plugin - Returned to the user: An error from the database repository for the autopatcher.
End Rem
Const ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR:Byte = 58
Rem
bbdoc: Autopatcher plugin - Finished getting all files from the autopatcher
End Rem
Const ID_AUTOPATCHER_FINISHED_INTERNAL:Byte = 59
Rem
bbdoc: 
End Rem
Const ID_AUTOPATCHER_FINISHED:Byte = 60
Rem
bbdoc: Autopatcher plugin - Returned to the user: You must restart the application to finish patching.
End Rem
Const ID_AUTOPATCHER_RESTART_APPLICATION:Byte = 61

Rem
bbdoc: NATPunchthrough plugin - Intermediary got a request to help punch through a nat
End Rem
Const ID_NAT_PUNCHTHROUGH_REQUEST:Byte = 62
Rem
bbdoc: NATPunchthrough plugin - Intermediary cannot complete the request because the target system is not connected
End Rem
Const ID_NAT_TARGET_NOT_CONNECTED:Byte = 63
Rem
bbdoc: NATPunchthrough plugin - While attempting to connect, we lost the connection to the target system
End Rem
Const ID_NAT_TARGET_CONNECTION_LOST:Byte = 64
Rem
bbdoc: NATPunchthrough plugin - Internal message to connect at a certain time
End Rem
Const ID_NAT_CONNECT_AT_TIME:Byte = 65
Rem
bbdoc: NATPunchthrough plugin - Internal message to send a message (to punch through the nat) at a certain time
End Rem
Const ID_NAT_SEND_OFFLINE_MESSAGE_AT_TIME:Byte = 66
Rem
bbdoc: NATPunchthrough plugin - The facilitator is already attempting this connection
End Rem
Const ID_NAT_IN_PROGRESS:Byte = 67

Rem
bbdoc: LightweightDatabase plugin - Query
End Rem
Const ID_DATABASE_QUERY_REQUEST:Byte = 68
Rem
bbdoc: LightweightDatabase plugin - Update
End Rem
Const ID_DATABASE_UPDATE_ROW:Byte = 69
Rem
bbdoc: LightweightDatabase plugin - Remove
End Rem
Const ID_DATABASE_REMOVE_ROW:Byte = 70
Rem
bbdoc: LightweightDatabase plugin - A serialized table.  Bytes 1+ contain the table.  Pass to TableSerializer::DeserializeTable
End Rem
Const ID_DATABASE_QUERY_REPLY:Byte = 71
Rem
bbdoc: LightweightDatabase plugin - Specified table not found
End Rem
Const ID_DATABASE_UNKNOWN_TABLE:Byte = 72
Rem
bbdoc: LightweightDatabase plugin - Incorrect password
End Rem
Const ID_DATABASE_INCORRECT_PASSWORD:Byte = 73

Rem
bbdoc: ReadyEvent plugin - Set the ready state for a particular system
End Rem
Const ID_READY_EVENT_SET:Byte = 74
Rem
bbdoc: ReadyEvent plugin - Unset the ready state for a particular system
End Rem
Const ID_READY_EVENT_UNSET:Byte = 75
Rem
bbdoc: All systems are in state End Rem
const ID_READY_EVENT_SET
End Rem
Const ID_READY_EVENT_ALL_SET:Byte = 76
Rem
bbdoc: ReadyEvent plugin - Request of ready event state - used for pulling data when newly connecting
End Rem
Const ID_READY_EVENT_QUERY:Byte = 77

Rem
bbdoc: Lobby packets. Second byte indicates type.
End Rem
Const ID_LOBBY_GENERAL:Byte = 78

Rem
bbdoc: Auto RPC procedure call
End Rem
Const ID_AUTO_RPC_CALL:Byte = 79

Rem
bbdoc: Auto RPC functionName to index mapping
End Rem
Const ID_AUTO_RPC_REMOTE_INDEX:Byte = 80

Rem
bbdoc: Auto RPC functionName to index mapping, lookup failed. Will try to auto recover
End Rem
Const ID_AUTO_RPC_UNKNOWN_REMOTE_INDEX:Byte = 81

Rem
bbdoc: Auto RPC error code
Rem
bbdoc: See AutoRPC.h for codes, stored in packet->data[1]
End Rem
Const ID_RPC_REMOTE_ERROR:Byte = 82

Rem
bbdoc: FileListTransfer transferring large files in chunks that are read only when needed, to save memory
End Rem
Const ID_FILE_LIST_REFERENCE_PUSH:Byte = 83

Rem
bbdoc: Force the ready event to all set
End Rem
Const ID_READY_EVENT_FORCE_ALL_SET:Byte = 84

' Rooms function
Rem
bbdoc: 
End Rem
Const ID_ROOMS_EXECUTE_FUNC:Byte = 85
Rem
bbdoc: 
End Rem
Const ID_ROOMS_LOGON_STATUS:Byte = 86
Rem
bbdoc: 
End Rem
Const ID_ROOMS_HANDLE_CHANGE:Byte = 87

' Lobby2 message
Rem
bbdoc: 
End Rem
Const ID_LOBBY2_SEND_MESSAGE:Byte = 88
Rem
bbdoc: 
End Rem
Const ID_LOBBY2_SERVER_ERROR:Byte = 89

' 
Rem
bbdoc: RAKNET_PROTOCOL_VERSION in RakNetVersion.h does not match on the remote system what we have on our system
about: This means the two systems cannot communicate.
The 2nd byte of the message contains the value of RAKNET_PROTOCOL_VERSION for the remote system
End Rem
Const ID_INCOMPATIBLE_PROTOCOL_VERSION:Byte = 90

Rem
bbdoc:  For the user to use.  Start your first enumeration at this value.
End Rem
Const ID_USER_PACKET_ENUM:Byte = 91

Const SYSTEM_PRIORITY:Int = 0
Const HIGH_PRIORITY:Int = 1
Const MEDIUM_PRIORITY:Int = 2
Const LOW_PRIORITY:Int = 3


Const UNRELIABLE:Int = 0
Const UNRELIABLE_SEQUENCED:Int = 1
Const RELIABLE:Int = 2
Const RELIABLE_ORDERED:Int = 3
Const RELIABLE_SEQUENCED:Int = 4



