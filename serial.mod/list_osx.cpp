/*
  Copyright (c) 2013 Bruce A Henderson
 
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
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <CoreFoundation/CFNumber.h>
#include <IOKit/IOBSD.h>
#include <sys/param.h>

extern "C" {

#include "blitz.h"

	BBObject * _bah_serial_TSerialPortInfo__create(BBString * portName, BBString * physicalName, BBString * productName, 
			BBString * enumeratorName, int vendorId, int productId);
	void _bah_serial_TSerialPortInfo__addInfo(BBObject * list, BBObject * info);

	void bmx_serial_listports(BBObject * list);

	void walkIterator(io_iterator_t serialPortIterator, BBObject * list);
}

void bmx_serial_listports(BBObject * list) {

    io_iterator_t serialPortIterator = 0;
    CFMutableDictionaryRef matchingDictionary;

	if (!(matchingDictionary = IOServiceMatching(kIOSerialBSDServiceValue))) {
		return;
	}

	CFDictionaryAddValue(matchingDictionary, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDAllTypes));

	if (IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &serialPortIterator) != KERN_SUCCESS) {
		return;
	}

	walkIterator(serialPortIterator, list);

    IOObjectRelease(serialPortIterator);
    serialPortIterator = 0;

	if (!(matchingDictionary = IOServiceNameMatching("AppleUSBCDC"))) {
		return;
    }

	if (IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &serialPortIterator) != KERN_SUCCESS) {
		return;
	}
	
	walkIterator(serialPortIterator, list);
	
	IOObjectRelease(serialPortIterator);
	
}

void walkIterator(io_iterator_t serialPortIterator, BBObject * list) {

    io_object_t usbDevice;

    while ((usbDevice = IOIteratorNext(serialPortIterator))) {

		BBString * bbPortName = &bbEmptyString;
		BBString * bbPhysicalName = &bbEmptyString;
		BBString * bbProductName = &bbEmptyString;
		BBString * bbEnumeratorName = &bbEmptyString;
		int bbVendorId = 0;
		int bbProductId = 0;

		CFTypeRef port = IORegistryEntryCreateCFProperty(usbDevice, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
		CFTypeRef productName = 0;
		CFTypeRef vendorId = 0;
		CFTypeRef productId = 0;
		
		io_registry_entry_t parent;
		kern_return_t result = IORegistryEntryGetParentEntry(usbDevice, kIOServicePlane, &parent);
		
		while (result == KERN_SUCCESS && !vendorId && !productId) {
			if (!productName) {
				productName = IORegistryEntrySearchCFProperty(parent, kIOServicePlane, CFSTR("Product Name"), kCFAllocatorDefault, 0);
			}
			
			vendorId = IORegistryEntrySearchCFProperty(parent, kIOServicePlane, CFSTR(kUSBVendorID), kCFAllocatorDefault, 0);
			productId = IORegistryEntrySearchCFProperty(parent, kIOServicePlane, CFSTR(kUSBProductID), kCFAllocatorDefault, 0);
			
			io_registry_entry_t previousParent = parent;
        	result = IORegistryEntryGetParentEntry(parent, kIOServicePlane, &parent);
	        IOObjectRelease(previousParent);
		}

		io_string_t physicalName;
		IORegistryEntryGetPath(usbDevice, kIOServicePlane, physicalName);
		bbPhysicalName = bbStringFromUTF8String(physicalName);
		
		if (port) {
			char buffer[MAXPATHLEN];
			if (CFStringGetCString((CFStringRef)port, buffer, PATH_MAX, kCFStringEncodingUTF8)) {
				bbPortName = bbStringFromUTF8String(buffer);
			}
			CFRelease(port);
		}
		
		if (productName) {
			char buffer[MAXPATHLEN];
			if (CFStringGetCString((CFStringRef)productName, buffer, PATH_MAX, kCFStringEncodingUTF8)) {
				bbProductName = bbStringFromUTF8String(buffer);
			}
			CFRelease(productName);
		}

		if (vendorId) {
			SInt32 id;
			if (CFNumberGetValue((CFNumberRef)vendorId, kCFNumberSInt32Type, &id)) {
				bbVendorId = id;
			}
			CFRelease(vendorId);
		}
		
		if (productId) {
			SInt32 id;
			if (CFNumberGetValue((CFNumberRef)productId, kCFNumberSInt32Type, &id)) {
				bbProductId = id;
			}
			CFRelease(productId);
		}
		IOObjectRelease(usbDevice);

		BBObject * info = _bah_serial_TSerialPortInfo__create(bbPortName, bbPhysicalName, bbProductName, bbEnumeratorName, bbVendorId, bbProductId);
		_bah_serial_TSerialPortInfo__addInfo(list, info);

	}
	
}

