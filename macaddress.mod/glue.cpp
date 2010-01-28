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

BBArray * bmx_getmacaddress() {
	unsigned char address[6];
	
	BBArray * arr = &bbEmptyArray;
	
	if (MACAddressUtility::GetMACAddress(address) == 0) {

		arr = bbArrayNew1D( "i",6 );
		int *s=(int*)BBARRAYDATA( arr, arr->dims );
		for(int i = 0; i < 6; ++i){
			s[i] = address[i];
		}

	}
		
	return arr;
}

BBString * bmx_getmacaddressstring(int uppercase) {
	unsigned char address[6];
	char buffer[50];

	if (MACAddressUtility::GetMACAddress(address) == 0) {
	
		sprintf(buffer, (uppercase) ?
				"%02X:%02X:%02X:%02X:%02X:%02X" : 
				"%02x:%02x:%02x:%02x:%02x:%02x" ,
			address[0], address[1], address[2], address[3], address[4], address[5]);
	
		return bbStringFromCString(buffer);
		
	}
	
	return &bbEmptyString;
}

