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

extern "C" {

#include "blitz.h"

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

	BBObject * CB_PREF(bah_serial_TSerialPortInfo__create)(BBString * portName, BBString * physicalName, BBString * productName, 
			BBString * enumeratorName, int vendorId, int productId);
	void CB_PREF(bah_serial_TSerialPortInfo__addInfo)(BBObject * list, BBObject * info);
	void CB_PREF(bah_serial_TSerialPortInfo__listPorts)(BBObject * list);

	void bmx_serial_listports(BBObject * list);

}

void bmx_serial_listports(BBObject * list) {
	CB_PREF(bah_serial_TSerialPortInfo__listPorts)(list);
}

