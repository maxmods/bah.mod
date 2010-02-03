/*
 Copyright (c) 2010 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "glue.h"

extern "C" {
	BBArray * _bah_inet_TInet__newArray(int n);
	BBObject * _bah_inet_TInet__create(BBString * name);
	void _bah_inet_TInet__newEntry(BBArray * arr, int index, BBObject * obj);

#if defined(WIN32) 
	void _bah_inet_TInet__setAddress(BBObject * obj, long unsigned int address);
	void _bah_inet_TInet__setNetmask(BBObject * obj, long unsigned int netmask);
#else
	void _bah_inet_TInet__setAddress(BBObject * obj, in_addr address);
	void _bah_inet_TInet__setNetmask(BBObject * obj, in_addr netmask);
#endif
	void _bah_inet_TInet__setFlags(BBObject * obj, unsigned int flags);
	void _bah_inet_TInet__setMACAddress(BBObject * obj, BBArray * arr, BBString * add);
}


#if defined(WIN32) 

static int countInterfaces(PIP_ADAPTER_INFO info) {
	int count = 0;
	
	while (info) {
		info = info->Next;
		count++;
	}
	
	return count;
}

static unsigned int convertFlags(unsigned int flags) {
	unsigned f = 0;
	
	if (flags & IFF_UP) {
		f |= 0x1;
	}

	if (flags & IFF_POINTTOPOINT) {
		f |= 0x10;
	}

	if (flags & IFF_LOOPBACK) {
		f |= 0x8;
	}

	if (flags & IFF_BROADCAST) {
		f |= 0x2;
	}

	if (flags & IFF_MULTICAST) {
		f |= 0x8000;
	}
	
	return f;
}

BBArray * bmx_inet_listinterfaces() {

	IP_ADAPTER_INFO adapterInfo[32];
	DWORD length = sizeof(adapterInfo);
	
	DWORD status = GetAdaptersInfo(adapterInfo, &length);
	if (status != ERROR_SUCCESS) {
		return &bbEmptyArray;
	}
	
	PIP_ADAPTER_INFO info = adapterInfo;
	
	int count = countInterfaces(info);
	BBArray * arr = _bah_inet_TInet__newArray(count);

	SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if (sd == SOCKET_ERROR) {
		return &bbEmptyArray;
	}
	
	INTERFACE_INFO interfaces[20];
	unsigned long bytes;
	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &interfaces, sizeof(interfaces), &bytes, 0, 0) == SOCKET_ERROR) {
		closesocket(sd);
		return &bbEmptyArray;
	}
	int interfaceCount = bytes / sizeof(INTERFACE_INFO);
	
	info = adapterInfo;
	int i = 0;
	while (info) {

		char buf[1024];
		sprintf(buf, "SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection",
			info->AdapterName);

		HKEY hKey;
		int res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buf, 0, KEY_READ, &hKey);

		LPDWORD lpType;
		DWORD blen = 1024;
		res = RegQueryValueEx(hKey, "Name", NULL, lpType, (LPBYTE)buf, &blen);

		res = RegCloseKey(hKey);

		BBObject * obj = _bah_inet_TInet__create(bbStringFromCString(buf));
		_bah_inet_TInet__setAddress(obj, inet_addr(info->IpAddressList.IpAddress.String));
		_bah_inet_TInet__setNetmask(obj, inet_addr(info->IpAddressList.IpMask.String));
		
		
		for (int n = 0; n < interfaceCount; n++) {
			if (strcmp(info->IpAddressList.IpAddress.String, inet_ntoa(((sockaddr_in *) & (interfaces[n].iiAddress))->sin_addr)) == 0) {
				_bah_inet_TInet__setFlags(obj, convertFlags(interfaces[i].iiFlags));
				break;
			}
		}
		
		_bah_inet_TInet__newEntry(arr, i++, obj);


		BBArray * arr = bbArrayNew1D( "i",6 );
		int *s=(int*)BBARRAYDATA( arr, arr->dims );
		for (int n = 0; n < 6; ++n){
			s[n] = info->Address[n];
		}
	
		char buffer[50];
		sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x" ,	info->Address[0], info->Address[1],
			info->Address[2], info->Address[3], info->Address[4], info->Address[5]);
	
		_bah_inet_TInet__setMACAddress(obj, arr, bbStringFromCString(buffer));

	
		info = info->Next;
	}
	

	closesocket(sd);
	
	return arr;

}

#elif defined(__APPLE__)


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

static void getMACAddress(BBObject * obj, const char * name) {
	int                     mib[6];
	size_t len;
	char                    *buf;
	unsigned char           *address;
	struct if_msghdr        *ifm;
	struct sockaddr_dl      *sdl;
	
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	
	if ((mib[5] = if_nametoindex(name)) == 0) {
		return;
	}
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		return;
	}
	
	if ((buf = (char*)malloc(len)) == NULL) {
		return;
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		free(buf);
		return;
	}
	
	ifm = (struct if_msghdr *)buf;
	sdl = (struct sockaddr_dl *)(ifm + 1);
	address = (unsigned char *)LLADDR(sdl);
	
	BBArray * arr = bbArrayNew1D( "i",6 );
	int *s=(int*)BBARRAYDATA( arr, arr->dims );
	for(int i = 0; i < 6; ++i){
		s[i] = address[i];
	}
	
	char buffer[50];
	sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x" ,	address[0], address[1], address[2], address[3], address[4], address[5]);
	
	_bah_inet_TInet__setMACAddress(obj, arr, bbStringFromCString(buffer));
	
	free(buf);			
}

BBArray * bmx_inet_listinterfaces() {
	struct ifaddrs *list;
	if (getifaddrs(&list) < 0) {
		return &bbEmptyArray;
	}
	
	int n = countInterfaces(list);
	BBArray * arr = _bah_inet_TInet__newArray(n);
	
	struct ifaddrs *addr = list;
	struct sockaddr_dl sdl;
	
	int i = 0;
	while (addr) {
		
		if (addr->ifa_addr->sa_family == AF_INET) {
			BBObject * obj = _bah_inet_TInet__create(bbStringFromUTF8String(addr->ifa_name));
			_bah_inet_TInet__setAddress(obj, ((struct sockaddr_in *)addr->ifa_addr)->sin_addr);
			_bah_inet_TInet__setNetmask(obj, ((struct sockaddr_in *)addr->ifa_netmask)->sin_addr);
			_bah_inet_TInet__setFlags(obj, addr->ifa_flags);
			
			getMACAddress(obj, addr->ifa_name);

			_bah_inet_TInet__newEntry(arr, i++, obj);
		}
		
		addr = addr->ifa_next;
	}
	
	freeifaddrs(list);
	
	return arr;
}

#elif defined(linux)

BBArray * bmx_inet_listinterfaces() {
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (s < 0) {
		return &bbEmptyArray;
	}
	
	struct ifconf conf;
	char ifconfbuf[128 * sizeof(struct ifreq)];
	
	memset(ifconfbuf, 0, sizeof( ifconfbuf ));
	
	conf.ifc_buf = ifconfbuf;
	conf.ifc_len = sizeof(ifconfbuf);
	
	if (ioctl(s, SIOCGIFCONF, &conf)) {
		shutdown(s, SHUT_RDWR);
		return &bbEmptyArray;
	}
	

	BBArray * arr = _bah_inet_TInet__newArray(conf.ifc_len);


	struct ifreq* ifreq = conf.ifc_req;
	int i = 0;
	while (ifreq < conf.ifc_req + conf.ifc_len) {
	
		if (! ioctl(s, SIOCGIFNAME, ifreq)) {
		
			BBObject * obj = _bah_inet_TInet__create(bbStringFromUTF8String(ifreq->ifr_name));
			
			_bah_inet_TInet__setAddress(obj, ((struct sockaddr_in *)&ifreq->ifr_addr)->sin_addr);
			
			if (! ioctl(s, SIOCGIFNETMASK, ifreq)) {
				_bah_inet_TInet__setNetmask(obj, ((struct sockaddr_in *)&ifreq->ifr_netmask)->sin_addr);
			}

			if (! ioctl(s, SIOCGIFHWADDR, ifreq)) {
			
				unsigned char * address = (unsigned char*)ifreq->ifr_hwaddr.sa_data;
			
				BBArray * arr = bbArrayNew1D( "i",6 );
				int *s=(int*)BBARRAYDATA( arr, arr->dims );
				for (int n = 0; n < 6; ++n){
					s[n] = address[n];
				}
			
				char buffer[50];
				sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", address[0], address[1],
					address[2], address[3], address[4], address[5]);
			
				_bah_inet_TInet__setMACAddress(obj, arr, bbStringFromCString(buffer));
				
			}
			
			if (! ioctl(s, SIOCGIFFLAGS, ifreq)) {
				_bah_inet_TInet__setFlags(obj, ifreq->ifr_flags);
			}

			_bah_inet_TInet__newEntry(arr, i++, obj);
		}
		
		ifreq++;
	}
	
	shutdown(s, SHUT_RDWR);
	return arr;
}

#endif

