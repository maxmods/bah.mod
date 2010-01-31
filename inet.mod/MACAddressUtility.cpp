#include "MACAddressUtility.h"

#include <stdio.h>

#if defined(_WIN32) || defined(UNDER_CE)

#include <windows.h>
#	if defined(UNDER_CE)
#		include <Iphlpapi.h>
#	endif

// this should include from rpcdce.h, but doesn't
extern "C" {
	RPC_STATUS RPC_ENTRY UuidCreateSequential(UUID*);
}

#elif defined(__APPLE__)
#	include <CoreFoundation/CoreFoundation.h>
#	include <IOKit/IOKitLib.h>
#	include <IOKit/network/IOEthernetInterface.h>
#	include <IOKit/network/IONetworkInterface.h>
#	include <IOKit/network/IOEthernetController.h>
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if_dl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <arpa/inet.h>

#elif defined(LINUX) || defined(linux)
#	include <string.h>
#	include <net/if.h>
#	include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#endif

long MACAddressUtility::GetMACAddress(unsigned char * result)
{
	// Fill result with zeroes
	memset(result, 0, 6);
	// Call appropriate function for each platform
#if defined(WIN32) || defined(UNDER_CE)
	return GetMACAddressMSW(result);
#elif defined(__APPLE__)
	return GetMACAddressMAC(result);
#elif defined(LINUX) || defined(linux)
	return GetMACAddressLinux(result);
#endif
	// If platform is not supported then return error code
	return -1;
}

BBArray * MACAddressUtility::ListInterfaces() {
#if defined(WIN32) || defined(UNDER_CE)
//	return GetMACAddressMSW(result);
#elif defined(__APPLE__)
	return ListInterfacesMac();
#elif defined(LINUX) || defined(linux)
//	return GetMACAddressLinux(result);
#endif
}


#if defined(WIN32) || defined(UNDER_CE)

inline long MACAddressUtility::GetMACAddressMSW(unsigned char * result)
{
	
#if defined(UNDER_CE)
	IP_ADAPTER_INFO AdapterInfo[16]; // Allocate information
	DWORD dwBufLen = sizeof(AdapterInfo); // Save memory size of buffer
	if(GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_SUCCESS) 
	{
		memcpy(result, AdapterInfo->Address, 6);
	}
	else return -1;
#else
	UUID uuid;
	if(UuidCreateSequential(&uuid) == RPC_S_UUID_NO_ADDRESS) return -1;
	memcpy(result, (char*)(uuid.Data4+2), 6);
#endif
	return 0;
}

#elif defined(__APPLE__)

extern "C" {
	BBArray * _bah_macaddress_TInet__newArray(int n);
	BBObject * _bah_macaddress_TInet__create(BBString * name);
	void _bah_macaddress_TInet__newEntry(BBArray * arr, int index, BBObject * obj);
	void _bah_macaddress_TInet__setAddress(BBObject * obj, in_addr address);
	void _bah_macaddress_TInet__setNetmask(BBObject * obj, in_addr netmask);
}

static int countInterfaces(struct ifaddrs *list) {
	int count = 0;
	
	struct ifaddrs *addr = list;
	while (addr) {
		if (addr->ifa_addr->sa_family == AF_INET) {
			count++;
		}
		addr = addr->ifa_next;
	}
	
	return count;
}


BBArray * MACAddressUtility::ListInterfacesMac() {
	struct ifaddrs *list;
	if (getifaddrs(&list) < 0) {
		return &bbEmptyArray;
	}
	
	int n = countInterfaces(list);
	BBArray * arr = _bah_macaddress_TInet__newArray(n);
	
	struct ifaddrs *addr = list;
	struct sockaddr_dl sdl;
	
	int i = 0;
	while (addr) {
		
		if (addr->ifa_addr->sa_family == AF_INET) {
			BBObject * obj = _bah_macaddress_TInet__create(bbStringFromUTF8String(addr->ifa_name));
			_bah_macaddress_TInet__setAddress(obj, ((struct sockaddr_in *)addr->ifa_addr)->sin_addr);
			_bah_macaddress_TInet__setNetmask(obj, ((struct sockaddr_in *)addr->ifa_netmask)->sin_addr);
			
			_bah_macaddress_TInet__newEntry(arr, i++, obj);



int                     mib[6];
size_t len;
char                    *buf;
unsigned char           *ptr;
struct if_msghdr        *ifm;
struct sockaddr_dl      *sdl;

mib[0] = CTL_NET;
mib[1] = AF_ROUTE;
mib[2] = 0;
mib[3] = AF_LINK;
mib[4] = NET_RT_IFLIST;
if ((mib[5] = if_nametoindex(addr->ifa_name)) == 0) {
   perror("if_nametoindex error");
   exit(2);
}

if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
   perror("sysctl 1 error");
   exit(3);
}

if ((buf = (char*)malloc(len)) == NULL) {
   perror("malloc error");
   exit(4);
}

if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
   perror("sysctl 2 error");
   exit(5);
}

ifm = (struct if_msghdr *)buf;
sdl = (struct sockaddr_dl *)(ifm + 1);
ptr = (unsigned char *)LLADDR(sdl);



			
//			if (sdl) {
//printf("%d\n", sdl->sdl_alen);fflush(stdout);
//			}
		}
		
		addr = addr->ifa_next;
	}
	
	freeifaddrs(list);
	
	return arr;
}

static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
    kern_return_t		kernResult;
    CFMutableDictionaryRef	matchingDict;
    CFMutableDictionaryRef	propertyMatchDict;

    matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);

    if (NULL != matchingDict)
	{
        propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
													  &kCFTypeDictionaryKeyCallBacks,
													  &kCFTypeDictionaryValueCallBacks);

        if (NULL != propertyMatchDict)
		{
            CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue);
            CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
            CFRelease(propertyMatchDict);
        }
    }
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, matchingServices);
    return kernResult;
}

static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize)
{
    io_object_t		intfService;
    io_object_t		controllerService;
    kern_return_t	kernResult = KERN_FAILURE;
	
	if (bufferSize < kIOEthernetAddressSize) {
		return kernResult;
	}
	
    bzero(MACAddress, bufferSize);
	
    while (intfService = IOIteratorNext(intfIterator))
    {
        CFTypeRef	MACAddressAsCFData = NULL;        
		CFTypeRef BSDNameAsCFString = NULL;
		
        // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call, 
        // since they are hardware nubs and do not participate in driver matching. In other words,
        // registerService() is never called on them. So we've found the IONetworkInterface and will 
        // get its parent controller by asking for it specifically.
        
        // IORegistryEntryGetParentEntry retains the returned object, so release it when we're done with it.
        kernResult = IORegistryEntryGetParentEntry(intfService,
												   kIOServicePlane,
												   &controllerService);
		
        if (KERN_SUCCESS != kernResult) {
            printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);
        }
        else {
            // Retrieve the MAC address property from the I/O Registry in the form of a CFData
            MACAddressAsCFData = IORegistryEntryCreateCFProperty(controllerService,
																 CFSTR(kIOMACAddress),
																 kCFAllocatorDefault,
																 0);
			//BSDNameAsCFString = IORegistryEntryCreateCFProperty( interfaceService, CFSTR("BSD Name"), kCFAllocatorDefault, 0 );

            if (MACAddressAsCFData) {
                //CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr
                
                // Get the raw bytes of the MAC address from the CFData
                CFDataGetBytes((CFDataRef)MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
                CFRelease(MACAddressAsCFData);
            }
			
            // Done with the parent Ethernet controller object so we release it.
            (void) IOObjectRelease(controllerService);
        }
        
        // Done with the Ethernet interface object so we release it.
        (void) IOObjectRelease(intfService);
    }

    return kernResult;
}

long MACAddressUtility::GetMACAddressMAC(unsigned char * result)
{
	io_iterator_t	intfIterator;
	kern_return_t	kernResult = KERN_FAILURE;
	do
	{
		kernResult = ::FindEthernetInterfaces(&intfIterator);
		if (KERN_SUCCESS != kernResult) break;
	    kernResult = ::GetMACAddress(intfIterator, (UInt8*)result, 6);
    }
	while(false);
    (void) IOObjectRelease(intfIterator);

	return 0;
}

#elif defined(LINUX) || defined(linux)

long MACAddressUtility::GetMACAddressLinux(unsigned char * result)
{
	struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    int ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1)
	{
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++)
	{
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0)
		{
            if (! (ifr.ifr_flags & IFF_LOOPBACK))
			{
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
				{
                    ok = 1;
                    break;
                }
            }
        }
    }

    shutdown(s, SHUT_RDWR);
    if (ok)
	{
        bcopy( ifr.ifr_hwaddr.sa_data, result, 6);
    }
    else
	{
        return -1;
    }
    return 0;
}

#endif
