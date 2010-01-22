#include "CCRakNetUDT.h"
#include "Rand.h"
#include "MTUSize.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//#include <memory.h>
#include "RakAssert.h"
#include "RakAlloca.h"

using namespace RakNet;

static const double UNSET_TIME_US=-1;
static const double CWND_MIN_THRESHOLD=2.0;
static const double UNDEFINED_TRANSFER_RATE=0.0;
/// Interval at which to update aspects of the system
/// 1. send acks
/// 2. update time interval between outgoing packets
/// 3, Yodate retransmit timeout
#if CC_TIME_TYPE_BYTES==4
static const CCTimeType SYN=10;
#else
static const CCTimeType SYN=10000;
#endif

#if CC_TIME_TYPE_BYTES==4
#define MAX_RTT 1000
#else
#define MAX_RTT 1000000
#endif

double RTTVarMultiple=4.0;


// ****************************************************** PUBLIC METHODS ******************************************************

CCRakNetUDT::CCRakNetUDT()
{
}

// ----------------------------------------------------------------------------------------------------------------------------

CCRakNetUDT::~CCRakNetUDT()
{
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::Init(CCTimeType curTime, uint32_t maxDatagramPayload)
{
	(void) curTime;

	nextSYNUpdate=0;
	packetPairRecieptHistoryWriteIndex=0;
	packetArrivalHistoryWriteIndex=0;
	packetArrivalHistoryWriteCount=0;
	RTT=UNSET_TIME_US;
	//	RTTVar=UNSET_TIME_US;
	isInSlowStart=true;
	NAKCount=1000;
	AvgNAKNum=1;
	DecInterval=1;
	DecCount=0;
	nextDatagramSequenceNumber=0;
	lastPacketPairPacketArrivalTime=0;
	lastPacketPairSequenceNumber=(DatagramSequenceNumberType)(const uint32_t)-1;
	lastPacketArrivalTime=0;
	CWND=CWND_MIN_THRESHOLD;
	lastUpdateWindowSizeAndAck=0;
	lastTransmitOfBAndAS=0;
	ExpCount=1.0;
	totalUserDataBytesSent=0;
	oldestUnsentAck=0;
	MAXIMUM_MTU_INCLUDING_UDP_HEADER=maxDatagramPayload;
	CWND_MAX_THRESHOLD=25600;
#if CC_TIME_TYPE_BYTES==4
	const BytesPerMicrosecond DEFAULT_TRANSFER_RATE=(BytesPerMicrosecond) 3.6;
#else
	const BytesPerMicrosecond DEFAULT_TRANSFER_RATE=(BytesPerMicrosecond) .0036;
#endif

	//	B=DEFAULT_TRANSFER_RATE;
	AS=UNDEFINED_TRANSFER_RATE;
	const MicrosecondsPerByte DEFAULT_BYTE_INTERVAL=(MicrosecondsPerByte) (1.0/DEFAULT_TRANSFER_RATE);
	SND=DEFAULT_BYTE_INTERVAL;
	expectedNextSequenceNumber=0;
	sendBAndASCount=0;
	packetArrivalHistoryContinuousGapsIndex=0;
	//packetPairRecipetHistoryGapsIndex=0;
	hasWrittenToPacketPairReceiptHistory=false;
	InitPacketArrivalHistory();


	estimatedLinkCapacityBytesPerSecond=0;
	gotPacketlossThisUpdate=false;
#if CC_TIME_TYPE_BYTES==4
	lastRttHigh=1000;
	lastRttLow=100;
#else
	lastRttHigh=1000000;
	lastRttLow=100000;
#endif

	bytesCanSendThisTick=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::SetMTU(uint32_t bytes)
{
	MAXIMUM_MTU_INCLUDING_UDP_HEADER=bytes;
	CWND_MAX_THRESHOLD=(1024*256)/MAXIMUM_MTU_INCLUDING_UDP_HEADER;
}
// ----------------------------------------------------------------------------------------------------------------------------
uint32_t CCRakNetUDT::GetMTU(void) const
{
	return MAXIMUM_MTU_INCLUDING_UDP_HEADER;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::Update(CCTimeType curTime, bool hasDataToSendOrResend)
{
	(void) hasDataToSendOrResend;
	(void) curTime;

	return;

	// I suspect this is causing major lag

	/*
	if (hasDataToSendOrResend==false)
	halveSNDOnNoDataTime=0;
	else if (halveSNDOnNoDataTime==0)
	{
	UpdateHalveSNDOnNoDataTime(curTime);
	ExpCount=1.0;
	}

	// If you send, and get no data at all from that time to RTO, then halve send rate7
	if (HasHalveSNDOnNoDataTimeElapsed(curTime))
	{
	/// 2000 bytes per second
	/// 0.0005 seconds per byte
	/// 0.5 milliseconds per byte
	/// 500 microseconds per byte
	// printf("No incoming data, halving send rate\n");
	SND*=2.0;
	CapMinSnd(__FILE__,__LINE__);
	ExpCount+=1.0;
	if (ExpCount>8.0)
	ExpCount=8.0;

	UpdateHalveSNDOnNoDataTime(curTime);
	}
	*/
}
// ----------------------------------------------------------------------------------------------------------------------------
int CCRakNetUDT::GetTransmissionBandwidth(CCTimeType curTime, CCTimeType timeSinceLastTick, uint32_t unacknowledgedBytes, bool isContinuousSend)
{
	(void) curTime;

	if (isInSlowStart)
	{
		uint32_t CWNDLimit = (uint32_t) (CWND*MAXIMUM_MTU_INCLUDING_UDP_HEADER-unacknowledgedBytes);
		return CWNDLimit;
	}
	if (bytesCanSendThisTick>0)
		bytesCanSendThisTick=0;
	if (isContinuousSend==false && timeSinceLastTick>SND*3)
		timeSinceLastTick=(int) (SND*3.0);
	bytesCanSendThisTick=(int)((double)timeSinceLastTick*((double)1.0/SND)+(double)bytesCanSendThisTick);
	if (bytesCanSendThisTick>0)
		return bytesCanSendThisTick;
	return 0;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCRakNetUDT::ShouldSendACKs(CCTimeType curTime, CCTimeType estimatedTimeToNextTick)
{
	CCTimeType rto = GetSenderRTOForACK();

	// iphone crashes on comparison between double and int64 http://www.jenkinssoftware.com/forum/index.php?topic=2717.0
	if (rto==(CCTimeType) UNSET_TIME_US)
	{
		// Unknown how long until the remote system will retransmit, so better send right away
		return true;
	}


	//	CCTimeType remoteRetransmitTime=oldestUnsentAck+rto-RTT*.5;
	//	CCTimeType ackArrivalTimeIfWeDelay=RTT*.5+estimatedTimeToNextTick+curTime;
	//	return ackArrivalTimeIfWeDelay<remoteRetransmitTime;

	// Simplified equation
	// GU: At least one ACK should be sent per SYN, otherwise your protocol will increase slower.
	return curTime >= oldestUnsentAck + SYN ||
		estimatedTimeToNextTick+curTime < oldestUnsentAck+rto-RTT;
}
// ----------------------------------------------------------------------------------------------------------------------------
DatagramSequenceNumberType CCRakNetUDT::GetNextDatagramSequenceNumber(void)
{
	DatagramSequenceNumberType dsnt=nextDatagramSequenceNumber;
	nextDatagramSequenceNumber++;
	return dsnt;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnSendBytes(CCTimeType curTime, uint32_t numBytes)
{
	(void) curTime;

	totalUserDataBytesSent+=numBytes;
	if (isInSlowStart==false)
		bytesCanSendThisTick-=numBytes;
}

// ****************************************************** PROTECTED METHODS ******************************************************

void CCRakNetUDT::SetNextSYNUpdate(CCTimeType currentTime)
{
	nextSYNUpdate+=SYN;
	if (nextSYNUpdate < currentTime)
		nextSYNUpdate=currentTime+SYN;
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCRakNetUDT::ReceiverCalculateDataArrivalRate(CCTimeType curTime) const
{
	(void) curTime;
	// Not an instantaneous measurement
	/*
	if (continuousBytesReceivedStartTime!=0 && curTime>continuousBytesReceivedStartTime)
	{
	#if CC_TIME_TYPE_BYTES==4
	const CCTimeType threshold=100;
	#else
	const CCTimeType threshold=100000;
	#endif
	if (curTime-continuousBytesReceivedStartTime>threshold)
	return (BytesPerMicrosecond) continuousBytesReceived/(BytesPerMicrosecond) (curTime-continuousBytesReceivedStartTime);
	}

	return UNDEFINED_TRANSFER_RATE;
	*/


	if (packetArrivalHistoryWriteCount<CC_RAKNET_UDT_PACKET_HISTORY_LENGTH)
		return UNDEFINED_TRANSFER_RATE;

	BytesPerMicrosecond median = ReceiverCalculateDataArrivalRateMedian();
	int i;
	const BytesPerMicrosecond oneEighthMedian=median*(1.0/8.0);
	const BytesPerMicrosecond eightTimesMedian=median*8.0f;
	BytesPerMicrosecond medianListLength=0.0;
	BytesPerMicrosecond sum=0.0;
	// Find average of acceptedMedianValues
	for (i=0; i < CC_RAKNET_UDT_PACKET_HISTORY_LENGTH; i++)
	{
		if (packetArrivalHistory[i]>=oneEighthMedian &&
			packetArrivalHistory[i]<eightTimesMedian)
		{
			medianListLength=medianListLength+1.0;
			sum+=packetArrivalHistory[i];
		}
	}
	if (medianListLength==0.0)
		return UNDEFINED_TRANSFER_RATE;
	return sum/medianListLength;
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCRakNetUDT::ReceiverCalculateDataArrivalRateMedian(void) const
{
	return CalculateListMedianRecursive(packetArrivalHistory, CC_RAKNET_UDT_PACKET_HISTORY_LENGTH, 0, 0);
}
// ----------------------------------------------------------------------------------------------------------------------------
BytesPerMicrosecond CCRakNetUDT::CalculateListMedianRecursive(const BytesPerMicrosecond inputList[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum)
{
	BytesPerMicrosecond lessThanMedian[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH], greaterThanMedian[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH];
	int lessThanMedianListLength=0, greaterThanMedianListLength=0;
	BytesPerMicrosecond median=inputList[0];
	int i;
	for (i=1; i < inputListLength; i++)
	{
		// If same value, spread among lists evenly
		if (inputList[i]<median || ((i&1)==0 && inputList[i]==median))
			lessThanMedian[lessThanMedianListLength++]=inputList[i];
		else
			greaterThanMedian[greaterThanMedianListLength++]=inputList[i];
	}
	RakAssert(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH%2==0);
	if (lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum+1 ||
		lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum-1)
		return median;

	if (lessThanMedianListLength+lessThanSum < greaterThanMedianListLength+greaterThanSum)
	{
		lessThanMedian[lessThanMedianListLength++]=median;
		return CalculateListMedianRecursive(greaterThanMedian, greaterThanMedianListLength, lessThanMedianListLength+lessThanSum, greaterThanSum);
	}
	else
	{
		greaterThanMedian[greaterThanMedianListLength++]=median;
		return CalculateListMedianRecursive(lessThanMedian, lessThanMedianListLength, lessThanSum, greaterThanMedianListLength+greaterThanSum);		
	}
}
/*
uint32_t CCRakNetUDT::CalculateListMedianRecursive(const uint32_t inputList[RTT_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum)
{
uint32_t *lessThanMedian, *greaterThanMedian;

#if !defined(_XBOX) && !defined(X360)
lessThanMedian = ( uint32_t* ) alloca( (size_t) inputListLength * sizeof(uint32_t) ) ;
greaterThanMedian = ( uint32_t* ) alloca( (size_t) inputListLength * sizeof(uint32_t) ) ;
#else
lessThanMedian = (uint32_t*) rakMalloc_Ex((size_t) inputListLength * sizeof(uint32_t), __FILE__, __LINE__);
greaterThanMedian = (uint32_t*) rakMalloc_Ex((size_t) inputListLength * sizeof(uint32_t), __FILE__, __LINE__);
#endif

int lessThanMedianListLength=0, greaterThanMedianListLength=0;
uint32_t median=inputList[0];
int i;
for (i=1; i < inputListLength; i++)
{
// If same value, spread among lists evenly
if (inputList[i]<median || ((i&1)==0 && inputList[i]==median))
lessThanMedian[lessThanMedianListLength++]=inputList[i];
else
greaterThanMedian[greaterThanMedianListLength++]=inputList[i];
}
RakAssert(RTT_HISTORY_LENGTH%2==0);
if (lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum+1 ||
lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum-1)
return median;

if (lessThanMedianListLength+lessThanSum < greaterThanMedianListLength+greaterThanSum)
{
lessThanMedian[lessThanMedianListLength++]=median;
return CalculateListMedianRecursive(greaterThanMedian, greaterThanMedianListLength, lessThanMedianListLength+lessThanSum, greaterThanSum);
}
else
{
greaterThanMedian[greaterThanMedianListLength++]=median;
return CalculateListMedianRecursive(lessThanMedian, lessThanMedianListLength, lessThanSum, greaterThanMedianListLength+greaterThanSum);		
}


#if !defined(_XBOX) && !defined(X360)
#else
rakFree_Ex(lessThanMedian, __FILE__, __LINE__);
rakFree_Ex(greaterThanMedian, __FILE__, __LINE__);
#endif
}
*/
// ----------------------------------------------------------------------------------------------------------------------------
bool CCRakNetUDT::GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
	// a > b?
	const DatagramSequenceNumberType halfSpan =(DatagramSequenceNumberType) (((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2);
	return b!=a && b-a>halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCRakNetUDT::LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
{
	// a < b?
	const DatagramSequenceNumberType halfSpan = ((DatagramSequenceNumberType)(const uint32_t)-1)/(DatagramSequenceNumberType)2;
	return b!=a && b-a<halfSpan;
}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCRakNetUDT::GetSenderRTOForACK(void) const
{
	if (RTT==UNSET_TIME_US)
		return (CCTimeType) UNSET_TIME_US;
	double RTTVar = maxRTT-minRTT;
	return (CCTimeType)(RTT + RTTVarMultiple * RTTVar + SYN);
}
// ----------------------------------------------------------------------------------------------------------------------------
CCTimeType CCRakNetUDT::GetRTOForRetransmission(void) const
{
#if CC_TIME_TYPE_BYTES==4
	const CCTimeType maxThreshold=1000;
	const CCTimeType minThreshold=100;
#else
	const CCTimeType maxThreshold=1000000;
	const CCTimeType minThreshold=100000;
#endif

	if (RTT==UNSET_TIME_US)
	{
		return (CCTimeType) maxThreshold;
	}

	CCTimeType ret = lastRttHigh*2;
	if (ret<minThreshold)
		return minThreshold;
	if (ret>maxThreshold)
		return maxThreshold;
	return ret;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnResend(CCTimeType curTime)
{
	(void) curTime;

	if (isInSlowStart)
	{
		if (AS!=UNDEFINED_TRANSFER_RATE)
			EndSlowStart();
		return;
	}

	if (gotPacketlossThisUpdate==false)
	{
//		printf("- rtt=%.0f Resend first\n", RTT/1000);
		IncreaseTimeBetweenSends();
		gotPacketlossThisUpdate=true;
	}
	// 	else if (randomMT()%congestionPeriodLength==0)
	// 	{
	// 		printf("- rtt=%.0f Resend subsequent\n", RTT/1000);
	// 		IncreaseTimeBetweenSends();
	// 	}
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnNAK(CCTimeType curTime, DatagramSequenceNumberType nakSequenceNumber)
{
	(void) nakSequenceNumber;
	(void) curTime;

	if (isInSlowStart)
	{
		if (AS!=UNDEFINED_TRANSFER_RATE)
			EndSlowStart();
		return;
	}

	if (gotPacketlossThisUpdate==false)
	{
//		printf("- rtt=%.0f NAK\n", RTT/1000);
		IncreaseTimeBetweenSends();
		gotPacketlossThisUpdate=true;
	}
	// 	else if (randomMT()%congestionPeriodLength==0)
	// 	{
	// 		printf("- rtt=%.0f Resend subsequent\n", RTT/1000);
	// 		IncreaseTimeBetweenSends();
	// 	}
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::EndSlowStart(void)
{
	RakAssert(isInSlowStart==true);
	RakAssert(AS!=UNDEFINED_TRANSFER_RATE);

	// This overestimates
	estimatedLinkCapacityBytesPerSecond=AS * 1000000.0;

	isInSlowStart=false;
	SND=1.0/AS;
	CapMinSnd(__FILE__,__LINE__);

	// printf("ENDING SLOW START\n");
#if CC_TIME_TYPE_BYTES==4
	//	printf("Initial SND=%f Kilobytes per second\n", 1.0/SND);
#else
	//	printf("Initial SND=%f Megabytes per second\n", 1.0/SND);
#endif
	if (SND > .1)
		PrintLowBandwidthWarning();

	StartNewRttHistory();
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, uint32_t sizeInBytes, CCTimeType curTime)
{
	(void) datagramSequenceNumber;
	(void) sizeInBytes;
	(void) curTime;

}
// ----------------------------------------------------------------------------------------------------------------------------
bool CCRakNetUDT::OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, CCTimeType curTime, uint32_t sizeInBytes, uint32_t *skippedMessageCount)
{	
	CC_DEBUG_PRINTF_2("R%i ",datagramSequenceNumber.val);

	if (datagramSequenceNumber==expectedNextSequenceNumber)
	{
		*skippedMessageCount=0;
		expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
	}
	else if (GreaterThan(datagramSequenceNumber, expectedNextSequenceNumber))
	{
		*skippedMessageCount=datagramSequenceNumber-expectedNextSequenceNumber;
		// Sanity check, just use timeout resend if this was really valid
		if (*skippedMessageCount>1000)
		{
			// During testing, the nat punchthrough server got 51200 on the first packet. I have no idea where this comes from, but has happened twice
			if (*skippedMessageCount>(uint32_t)50000)
				return false;
			*skippedMessageCount=1000;
		}
		expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
	}
	else
	{
		*skippedMessageCount=0;
	}

	if (curTime>lastPacketArrivalTime)
	{
		CCTimeType interval = curTime-lastPacketArrivalTime;

		//		printf("Packet arrival gap is %I64u\n", (interval));

		if (isContinuousSend)
		{
			continuousBytesReceived+=sizeInBytes;
			if (continuousBytesReceivedStartTime==0)
				continuousBytesReceivedStartTime=lastPacketArrivalTime;


			mostRecentPacketArrivalHistory=(BytesPerMicrosecond)sizeInBytes/(BytesPerMicrosecond)interval;

			//		if (mostRecentPacketArrivalHistory < (BytesPerMicrosecond)0.0035)
			//		{
			//			printf("%s:%i LIKELY BUG: Calculated packetArrivalHistory is below 28.8 Kbps modem\nReport to rakkar@jenkinssoftware.com with file and line number\n", __FILE__, __LINE__);
			//		}

			packetArrivalHistoryContinuousGaps[packetArrivalHistoryContinuousGapsIndex++]=(int) interval;
			packetArrivalHistoryContinuousGapsIndex&=(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH-1);

			packetArrivalHistoryWriteCount++;
			packetArrivalHistory[packetArrivalHistoryWriteIndex++]=mostRecentPacketArrivalHistory;
			// Wrap to 0 at the end of the range
			// Assumes power of 2 for CC_RAKNET_UDT_PACKET_HISTORY_LENGTH
			packetArrivalHistoryWriteIndex&=(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH-1);
		}
		else
		{
			continuousBytesReceivedStartTime=0;
			continuousBytesReceived=0;
		}

		lastPacketArrivalTime=curTime;
	}
	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnAck(CCTimeType curTime, CCTimeType rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, double totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber )
{
#if CC_TIME_TYPE_BYTES==4
	RakAssert(rtt < 10000);
#else
	RakAssert(rtt < 10000000);
#endif
	(void) _B;

	if (hasBAndAS)
	{
		///	RakAssert(_B!=UNDEFINED_TRANSFER_RATE && _AS!=UNDEFINED_TRANSFER_RATE);
		//	B=B * .875 + _B * .125;
		// AS is packet arrival rate
		RakAssert(_AS!=UNDEFINED_TRANSFER_RATE);
		AS=_AS;
		CC_DEBUG_PRINTF_4("ArrivalRate=%f linkCap=%f incomingLinkCap=%f\n", _AS,B,_B);
	}

	TrackRTT(curTime, rtt);

	if (oldestUnsentAck==0)
		oldestUnsentAck=curTime;

	if (isInSlowStart==true)
	{
		UpdateWindowSizeAndAckOnAckPreSlowStart(totalUserDataBytesAcked);
	}
	else if (isContinuousSend)
	{
		UpdateWindowSizeAndAckOnAckPerSyn(curTime, rtt, isContinuousSend, sequenceNumber);
	}

	lastUpdateWindowSizeAndAck=curTime;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::TrackRTT(CCTimeType curTime, CCTimeType rtt)
{
	(void) curTime;

	RTT=(double)rtt;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnSendAckGetBAndAS(CCTimeType curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS)
{
	if (curTime>lastTransmitOfBAndAS+SYN)
	{
		*_B=0;
		*_AS=ReceiverCalculateDataArrivalRate(curTime);

		if (*_AS==UNDEFINED_TRANSFER_RATE)
		{
			*hasBAndAS=false;
		}
		else
		{
			*hasBAndAS=true;
		}
	}
	else
	{
		*hasBAndAS=false;
	}
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnSendAck(CCTimeType curTime, uint32_t numBytes)
{
	(void) numBytes;
	(void) curTime;

	// This is not accounted for on the remote system, and thus causes bandwidth to be underutilized
	//UpdateNextAllowedSend(curTime, numBytes+UDP_HEADER_SIZE);

	oldestUnsentAck=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::OnSendNACK(CCTimeType curTime, uint32_t numBytes)
{
	(void) numBytes;
	(void) curTime;

	// This is not accounted for on the remote system, and thus causes bandwidth to be underutilized
	//	UpdateNextAllowedSend(curTime, numBytes+UDP_HEADER_SIZE);
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::UpdateWindowSizeAndAckOnAckPreSlowStart(double totalUserDataBytesAcked)
{
	// During slow start, max window size is the number of full packets that have been sent out
	// CWND=(double) ((double)totalUserDataBytesSent/(double)MAXIMUM_MTU_INCLUDING_UDP_HEADER);
	CC_DEBUG_PRINTF_3("CWND increasing from %f to %f\n", CWND, (double) ((double)totalUserDataBytesAcked/(double)MAXIMUM_MTU_INCLUDING_UDP_HEADER));
	CWND=(double) ((double)totalUserDataBytesAcked/(double)MAXIMUM_MTU_INCLUDING_UDP_HEADER);
	if (CWND>=CWND_MAX_THRESHOLD)
	{
		CWND=CWND_MAX_THRESHOLD;

		if (AS!=UNDEFINED_TRANSFER_RATE)
			EndSlowStart();
	}
	if (CWND<CWND_MIN_THRESHOLD)
		CWND=CWND_MIN_THRESHOLD;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::UpdateWindowSizeAndAckOnAckPerSyn(CCTimeType curTime, CCTimeType rtt, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber)
{	
	(void) curTime;
	(void) sequenceNumber;
	if (isContinuousSend==false)
		return;

	uint32_t rtt32 = (uint32_t) rtt;
	RttAnalysis rtta = PushToRttHistory(rtt32, sequenceNumber);

	if (rtta==RTTA_STEADY)
	{
		if (gotPacketlossThisUpdate==false)
		{
			// Speed up
//			printf("+ rtt=%i\n", rtt32/1000);
			DecreaseTimeBetweenSends();
		}

		lastRttHigh=rttHigh;
		lastRttLow=rttLow;
		StartNewRttHistory();
		gotPacketlossThisUpdate=false;
	}
	else if (rtta==RTTA_SPIKE)
	{
		// Slow down
//		printf("- rtt=%i\n", rtt32/1000);
		IncreaseTimeBetweenSends();
		estimatedLinkCapacityBytesPerSecond=(1.0/SND) * 1000000.0;

		lastRttHigh=rttHigh;
		lastRttLow=rttLow;
		StartNewRttHistory();
		gotPacketlossThisUpdate=false;
	}
}

// ----------------------------------------------------------------------------------------------------------------------------
double CCRakNetUDT::BytesPerMicrosecondToPacketsPerMillisecond(BytesPerMicrosecond in)
{
#if CC_TIME_TYPE_BYTES==4
	const BytesPerMicrosecond factor = 1.0 / (BytesPerMicrosecond) MAXIMUM_MTU_INCLUDING_UDP_HEADER;
#else
	const BytesPerMicrosecond factor = 1000.0 / (BytesPerMicrosecond) MAXIMUM_MTU_INCLUDING_UDP_HEADER;
#endif
	return in * factor;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::InitPacketArrivalHistory(void)
{
	unsigned int i;
	for (i=0; i < CC_RAKNET_UDT_PACKET_HISTORY_LENGTH; i++)
	{
		packetArrivalHistory[i]=UNDEFINED_TRANSFER_RATE;
		packetArrivalHistoryContinuousGaps[i]=0;
	}

	packetArrivalHistoryWriteCount=0;
	continuousBytesReceived=0;
	continuousBytesReceivedStartTime=0;
}
// ----------------------------------------------------------------------------------------------------------------------------
void CCRakNetUDT::PrintLowBandwidthWarning(void)
{

	/*
	printf("\n-------LOW BANDWIDTH -----\n");
	if (isInSlowStart==false)
	printf("SND=%f Megabytes per second\n", 1.0/SND);
	printf("Window size=%f\n", CWND);
	printf("Pipe from packet pair = %f megabytes per second\n", B);
	printf("RTT=%f milliseconds\n", RTT/1000.0);
	printf("RTT Variance=%f milliseconds\n", RTTVar/1000.0);
	printf("Retransmission=%i milliseconds\n", GetRTOForRetransmission()/1000);
	printf("Packet arrival rate on the remote system=%f megabytes per second\n", AS);
	printf("Packet arrival rate on our system=%f megabytes per second\n", ReceiverCalculateDataArrivalRate());
	printf("isInSlowStart=%i\n", isInSlowStart);
	printf("---------------\n");
	*/
}
BytesPerMicrosecond CCRakNetUDT::GetLocalReceiveRate(CCTimeType currentTime) const
{
	return ReceiverCalculateDataArrivalRate(currentTime);
}
double CCRakNetUDT::GetRTT(void) const
{
	if (RTT==UNSET_TIME_US)
		return 0.0;
	return RTT;
}
void CCRakNetUDT::CapMinSnd(const char *file, int line)
{
	(void) file;
	(void) line;

	if (SND > 500)
	{
		SND=500;
		CC_DEBUG_PRINTF_3("%s:%i LIKELY BUG: SND has gotten below 500 microseconds between messages (28.8 modem)\nReport to rakkar@jenkinssoftware.com with file and line number\n", file, line);
	}
}
void CCRakNetUDT::IncreaseTimeBetweenSends(void)
{
	// In order to converge, bigger numbers have to increase slower and decrease faster
	// SND==500 then increment is .02
	// SND==0 then increment is near 0
	// (SND+1.0) brings it to the range of 1 to 501
	// Square the number, which is the range of 1 to 251001
	// Divide by 251001, which is the range of 1/251001 to 1
	// Multiple by .02

	double increment;
	increment = .02 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
	// SND=500 then increment=.02
	// SND=0 then increment=near 0
	SND*=(1.04 - increment);

	// SND=0 then fast increase, slow decrease
	// SND=500 then slow increase, fast decrease
}
void CCRakNetUDT::DecreaseTimeBetweenSends(void)
{
	double increment;
	increment = .01 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
	// SND=500 then increment=.01
	// SND=0 then increment=near 0
	SND*=(.99 - increment);
}
void CCRakNetUDT::StartNewRttHistory(void)
{
	rttDeltaHistory.Clear(__FILE__,__LINE__);
	rttDelta=0;
	rttLow=(uint32_t)-1;
	rttHigh=0;
	historyEndDatagramNumber=nextDatagramSequenceNumber+(DatagramSequenceNumberType)16;
}
CCRakNetUDT::RttAnalysis CCRakNetUDT::PushToRttHistory(uint32_t rtt, DatagramSequenceNumberType ackSequenceNumber)
{
	if (rtt < rttLow)
		rttLow=rtt;
	if (rtt > rttHigh)
		rttHigh=rtt;
	rttDeltaHistory.Push(rtt,__FILE__,__LINE__);
	if (rttDeltaHistory.Size()>1)
	{
		if (rttDeltaHistory.Size()>32 && rtt > lastRttHigh + ((lastRttHigh-lastRttLow)>>3) )
			return RTTA_SPIKE;
		rttDelta+=rtt-rttDeltaHistory[rttDeltaHistory.Size()-1];
		if (rttDeltaHistory.Size()>32)
		{
			rttDelta-=rttDeltaHistory[1]-rttDeltaHistory[0];
			rttDeltaHistory.Pop();
		}
	}
	if (GreaterThan(ackSequenceNumber, historyEndDatagramNumber))
	{
		return GetRTTAnalysis(rtt);
	}
	return RTTA_NEITHER;
}
CCRakNetUDT::RttAnalysis CCRakNetUDT::GetRTTAnalysis(uint32_t rtt)
{
	(void) rtt;

	if (fabs((float)rttDelta)<(.25 * (rttHigh-rttLow)))
		return RTTA_STEADY;
	if (rttDelta < -(.25 * (rttHigh-rttLow)))
		return RTTA_DOWNWARD;
	if (rttDelta > .5 * (rttHigh-rttLow))
		return RTTA_SPIKE;
	return RTTA_NEITHER;
}
