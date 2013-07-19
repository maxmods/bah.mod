extern "C" {

#include "blitz.h"

	BBObject * _bah_serial_TSerialPortInfo__create(BBString * portName, BBString * physicalName, BBString * productName, 
			BBString * enumeratorName, int vendorId, int productId);
	void _bah_serial_TSerialPortInfo__addInfo(BBObject * list, BBObject * info);

	void bmx_serial_listports(BBObject * list);

}

void bmx_serial_listports(BBObject * list) {

}

