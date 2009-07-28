#include "PacketOutputWindowLogger.h"
#include "RakString.h"
#if defined(_WIN32) && !defined(__X360__) && !defined(_XBOX)
#include "WindowsIncludes.h"
#endif

PacketOutputWindowLogger::PacketOutputWindowLogger()
{
}
PacketOutputWindowLogger::~PacketOutputWindowLogger()
{
}
void PacketOutputWindowLogger::WriteLog(const char *str)
{
#if defined(_WIN32) && !defined(__X360__) && !defined(_XBOX)
	RakNet::RakString str2 = str;
	str2+="\n";
	OutputDebugStr(str2.C_String());
#endif
}
