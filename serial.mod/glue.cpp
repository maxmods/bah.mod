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

#include <serial/serial.h>

class MaxTimeout;

extern "C" {

#include "blitz.h"

	BBObject * _bah_serial_TSerialException__create(BBString * what);
	BBObject * _bah_serial_TIOException__create(BBString * what);
	BBObject * _bah_serial_TPortNotOpenedException__create(BBString * what);

	void bmx_serial_throw_serialexception(serial::SerialException &e);
	void bmx_serial_throw_ioexception(serial::IOException &e);
	void bmx_serial_throw_portnotopenexception(serial::PortNotOpenedException &e);

	serial::Serial * bmx_serial_create(BBString * port, int baudrate, MaxTimeout * timeout, int bytesize, int parity, int stopbits, int flowcontrol);
	void bmx_serial_open(serial::Serial * ser);
	int bmx_serial_isopen(serial::Serial * ser);
	int bmx_serial_available(serial::Serial * ser);
	int bmx_serial_read(serial::Serial * ser, uint8_t * buffer, int size);
	BBString * bmx_serial_readline(serial::Serial * ser, int size, BBString * eol);
	int bmx_serial_write(serial::Serial * ser, uint8_t * data, int size);
	int bmx_serial_writestring(serial::Serial * ser, BBString * data);
	void bmx_serial_setport(serial::Serial * ser, BBString * port);
	BBString * bmx_serial_getport(serial::Serial * ser);
	void bmx_serial_setbaudrate(serial::Serial * ser, int baudrate);
	int bmx_serial_getbaudrate(serial::Serial * ser);
	void bmx_serial_setbytesize(serial::Serial * ser, int bytesize);
	int bmx_serial_getbytesize(serial::Serial * ser);
	void bmx_serial_setparity(serial::Serial * ser, int parity);
	int bmx_serial_getparity(serial::Serial * ser);
	void bmx_serial_setstopbits(serial::Serial * ser, int stopbits);
	int bmx_serial_getstopbits(serial::Serial * ser);
	void bmx_serial_setflowcontrol(serial::Serial * ser, int flowcontrol);
	int bmx_serial_getflowcontrol(serial::Serial * ser);
	void bmx_serial_flush(serial::Serial * ser);
	void bmx_serial_flushinput(serial::Serial * ser);
	void bmx_serial_flushoutput(serial::Serial * ser);
	void bmx_serial_sendbreak(serial::Serial * ser, int duration);
	void bmx_serial_setbreak(serial::Serial * ser, int level);
	void bmx_serial_setrts(serial::Serial * ser, int level);
	void bmx_serial_setdtr(serial::Serial * ser, int level);
	void bmx_serial_waitforchange(serial::Serial * ser);
	int bmx_serial_getcts(serial::Serial * ser);
	int bmx_serial_getdsr(serial::Serial * ser);
	int bmx_serial_getri(serial::Serial * ser);
	int bmx_serial_getcd(serial::Serial * ser);

	int bmx_serial_timeout_max();
	void bmx_serial_timeout_free(MaxTimeout * timeout);
	MaxTimeout * bmx_serial_timeout_create(int interByteTimeout, int readTimeoutConstant, int readTimeoutMultiplier, int writeTimeoutConstant, 
			int writeTimeoutMultiplier);

}


class MaxTimeout
{
public:
	MaxTimeout() {}
	MaxTimeout(serial::Timeout & t) : timeout(t)
	{}

	serial::Timeout & Timeout() {
		return timeout;
	}

private:
	serial::Timeout timeout;
};

// ********************************************************

void bmx_serial_throw_serialexception(serial::SerialException &e) {
	bbExThrow(_bah_serial_TSerialException__create(bbStringFromUTF8String(e.what())));
}

void bmx_serial_throw_ioexception(serial::IOException &e) {
	bbExThrow(_bah_serial_TIOException__create(bbStringFromUTF8String(e.what())));
}

void bmx_serial_throw_portnotopenexception(serial::PortNotOpenedException &e) {
	bbExThrow(_bah_serial_TPortNotOpenedException__create(bbStringFromUTF8String(e.what())));
}

// ********************************************************

serial::Serial * bmx_serial_create(BBString * port, int baudrate, MaxTimeout * timeout, int bytesize, int parity, int stopbits, int flowcontrol) {
	serial::Serial * ser = 0;
	try {
		if (port == &bbEmptyString) {
			ser = new serial::Serial("", baudrate, (timeout) ? timeout->Timeout() : serial::Timeout(), static_cast<serial::bytesize_t>(bytesize),
				static_cast<serial::parity_t>(parity), static_cast<serial::stopbits_t>(stopbits), static_cast<serial::flowcontrol_t>(flowcontrol));
		} else {
			char * s = bbStringToUTF8String(port);
			ser = new serial::Serial(s, baudrate, (timeout) ? timeout->Timeout() : serial::Timeout(), static_cast<serial::bytesize_t>(bytesize),
				static_cast<serial::parity_t>(parity), static_cast<serial::stopbits_t>(stopbits), static_cast<serial::flowcontrol_t>(flowcontrol));
			bbMemFree(s);
		}
	} catch (serial::PortNotOpenedException &e) {
		bmx_serial_throw_portnotopenexception(e);
	} catch (std::exception & e) {
	bbExThrow(_bah_serial_TSerialException__create(bbStringFromUTF8String(e.what())));
	}
	
	return ser;
}

void bmx_serial_open(serial::Serial * ser) {
	try {
	
		ser->open();
		
	} catch (serial::IOException &e) {
		bmx_serial_throw_ioexception(e);
	} catch (serial::SerialException &e) {
		bmx_serial_throw_serialexception(e);
	}
}

int bmx_serial_isopen(serial::Serial * ser) {
	return static_cast<int>(ser->isOpen());
}

int bmx_serial_available(serial::Serial * ser) {
	return static_cast<int>(ser->available());
}

int bmx_serial_read(serial::Serial * ser, uint8_t * buffer, int size) {
	return static_cast<int>(ser->read(buffer, size));
}

BBString * bmx_serial_readline(serial::Serial * ser, int size, BBString * eol) {
	char * s = bbStringToUTF8String(eol);
	BBString * ret = bbStringFromUTF8String(ser->readline(size, s).c_str());
	bbMemFree(s);
	return ret;
}

int bmx_serial_write(serial::Serial * ser, uint8_t * data, int size) {
	return static_cast<int>(ser->write(data, size));
}

int bmx_serial_writestring(serial::Serial * ser, BBString * data) {
	char * s = bbStringToUTF8String(data);
	int ret = static_cast<int>(ser->write(s));
}

void bmx_serial_setport(serial::Serial * ser, BBString * port) {
	char * s = bbStringToUTF8String(port);
	ser->setPort(s);
	bbMemFree(s);
}

BBString * bmx_serial_getport(serial::Serial * ser) {
	return bbStringFromUTF8String(ser->getPort().c_str());
}

void bmx_serial_setbaudrate(serial::Serial * ser, int baudrate) {
	ser->setBaudrate(baudrate);
}

int bmx_serial_getbaudrate(serial::Serial * ser) {
	return ser->getBaudrate();
}

void bmx_serial_setbytesize(serial::Serial * ser, int bytesize) {
	ser->setBytesize(static_cast<serial::bytesize_t>(bytesize));
}

int bmx_serial_getbytesize(serial::Serial * ser) {
	return static_cast<int>(ser->getBytesize());
}

void bmx_serial_setparity(serial::Serial * ser, int parity) {
	ser->setParity(static_cast<serial::parity_t>(parity));
}

int bmx_serial_getparity(serial::Serial * ser) {
	return static_cast<int>(ser->getParity());
}

void bmx_serial_setstopbits(serial::Serial * ser, int stopbits) {
	ser->setStopbits(static_cast<serial::stopbits_t>(stopbits));
}

int bmx_serial_getstopbits(serial::Serial * ser) {
	return static_cast<int>(ser->getStopbits());
}

void bmx_serial_setflowcontrol(serial::Serial * ser, int flowcontrol) {
	ser->setFlowcontrol(static_cast<serial::flowcontrol_t>(flowcontrol));
}

int bmx_serial_getflowcontrol(serial::Serial * ser) {
	return static_cast<int>(ser->getFlowcontrol());
}

void bmx_serial_flush(serial::Serial * ser) {
	ser->flush();
}

void bmx_serial_flushinput(serial::Serial * ser) {
	ser->flushInput();
}

void bmx_serial_flushoutput(serial::Serial * ser) {
	ser->flushOutput();	
}

void bmx_serial_sendbreak(serial::Serial * ser, int duration) {
	ser->sendBreak(duration);
}

void bmx_serial_setbreak(serial::Serial * ser, int level) {
	ser->setBreak(static_cast<bool>(level));
}

void bmx_serial_setrts(serial::Serial * ser, int level) {
	ser->setRTS(static_cast<bool>(level));
}

void bmx_serial_setdtr(serial::Serial * ser, int level) {
	ser->setDTR(static_cast<bool>(level));
}

void bmx_serial_waitforchange(serial::Serial * ser) {
	try {
	
		ser->waitForChange();
		
	} catch (serial::SerialException &e) {
		bmx_serial_throw_serialexception(e);
	}
}

int bmx_serial_getcts(serial::Serial * ser) {
	return static_cast<int>(ser->getCTS());
}

int bmx_serial_getdsr(serial::Serial * ser) {
	return static_cast<int>(ser->getDSR());
}

int bmx_serial_getri(serial::Serial * ser) {
	return static_cast<int>(ser->getRI());
}

int bmx_serial_getcd(serial::Serial * ser) {
	return static_cast<int>(ser->getCD());
}

// ********************************************************

int bmx_serial_timeout_max() {
	return serial::Timeout::max();
}

void bmx_serial_timeout_free(MaxTimeout * timeout) {
	delete timeout;
}

MaxTimeout * bmx_serial_timeout_create(int interByteTimeout, int readTimeoutConstant, int readTimeoutMultiplier, int writeTimeoutConstant, 
			int writeTimeoutMultiplier) {
	serial::Timeout t(interByteTimeout, readTimeoutConstant, readTimeoutMultiplier, writeTimeoutConstant, writeTimeoutMultiplier);
	return new MaxTimeout(t);
}
