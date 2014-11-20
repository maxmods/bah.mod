/*
  Copyright (c) 2013-2014 Bruce A Henderson
 
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
#include <objbase.h>
#include <initguid.h>
#include <setupapi.h>

extern "C" {

// http://msdn.microsoft.com/en-us/library/windows/hardware/ff553426(v=vs.85).aspx
const GUID guids[] = {
    // COM & LPT ports
    {0x4D36E978, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}},
    // Modem
    {0x4D36E96D, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}},
    // Bluetooth
    {0xE0CBF06C, 0xCD8B, 0x4647, {0xBB, 0x8A, 0x26, 0x3B, 0x43, 0xF0, 0xF9, 0x74}},
    // com0com virtual ports
    {0xDF799E12, 0x3C56, 0x421B, {0xB2, 0x98, 0xB6, 0xD3, 0x64, 0x2B, 0xC8, 0x78}}
};


#include "blitz.h"

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

	BBObject * CB_PREF(bah_serial_TSerialPortInfo__create)(BBString * portName, BBString * physicalName, BBString * productName, 
			BBString * enumeratorName, int vendorId, int productId);
	void CB_PREF(bah_serial_TSerialPortInfo__addInfo)(BBObject * list, BBObject * info);
	void CB_PREF(bah_serial_TSerialPortInfo__getIds)(BBString * hids, int * vendorId, int * productId);

	void bmx_serial_listports(BBObject * list);

}

void bmx_serial_listports(BBObject * list) {

	int size = sizeof(guids)/sizeof(guids[0]);
	
	for (int i = 0; i < size; i++) {
		HDEVINFO deviceInfo;
		
		if ((deviceInfo = SetupDiGetClassDevs(&guids[i], NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE) {
			SP_DEVINFO_DATA deviceInfoData;
			deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
			int device = 0;
			while (SetupDiEnumDeviceInfo(deviceInfo, device, &deviceInfoData)) {
				device++;

				BBString * bbPortName = &bbEmptyString;
				BBString * bbPhysicalName = &bbEmptyString;
				BBString * bbProductName = &bbEmptyString;
				BBString * bbEnumeratorName = &bbEmptyString;
				int bbVendorId = 0;
				int bbProductId = 0;
			
				DWORD bufferSize = 0;
				BYTE *buffer = 0;
				
				SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, NULL, 0, &bufferSize);
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					buffer = new BYTE[bufferSize];
					SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, buffer, bufferSize, NULL);
					bbProductName = bbStringFromUTF8String((char*)buffer);
					delete [] buffer;
				}

				SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, NULL, 0, &bufferSize);
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					buffer = new BYTE[bufferSize];
					SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, buffer, bufferSize, NULL);
					bbPhysicalName = bbStringFromUTF8String((char*)buffer);
					delete [] buffer;
				}

				SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_ENUMERATOR_NAME, NULL, NULL, 0, &bufferSize);
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					buffer = new BYTE[bufferSize];
					SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_ENUMERATOR_NAME, NULL, buffer, bufferSize, NULL);
					bbEnumeratorName = bbStringFromUTF8String((char*)buffer);
					delete [] buffer;
				}

				HKEY devKey = SetupDiOpenDevRegKey(deviceInfo, &deviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
				
				bufferSize = 0;
				DWORD type;
				if (RegQueryValueEx(devKey, TEXT("PortName"), NULL, NULL, NULL, &bufferSize) == ERROR_SUCCESS) {
					buffer = new BYTE[size];
					if (RegQueryValueEx(devKey, TEXT("PortName"), NULL, &type, buffer, &bufferSize) == ERROR_SUCCESS) {
						bbPortName = bbStringFromUTF8String((char*)buffer);
					}
					RegCloseKey(devKey);
					delete [] buffer;
				}
				
				BBString * hids = &bbEmptyString;

				SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_HARDWAREID, NULL, NULL, 0, &bufferSize);
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					buffer = new BYTE[bufferSize];
					SetupDiGetDeviceRegistryProperty(deviceInfo, &deviceInfoData, SPDRP_HARDWAREID, NULL, buffer, bufferSize, NULL);
					hids = bbStringFromUTF8String((char*)buffer);
					delete [] buffer;
				}
				
				if (hids != &bbEmptyString) {
					CB_PREF(bah_serial_TSerialPortInfo__getIds)(hids, &bbVendorId, &bbProductId);
				}

				BBObject * info = CB_PREF(bah_serial_TSerialPortInfo__create)(bbPortName, bbPhysicalName, bbProductName, bbEnumeratorName, bbVendorId, bbProductId);
				if (info) {
					CB_PREF(bah_serial_TSerialPortInfo__addInfo)(list, info);
				}

			}
			SetupDiDestroyDeviceInfoList(deviceInfo);
		}
		
	}
}


