' Copyright (c) 2007-2009 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import BRL.Blitz

Import "-lusb"

?win32
Import "include/win32/*.h"
?
Import "glue.cpp"


Extern

	Function usb_init()
	Function usb_find_busses:Int()
	Function usb_find_devices:Int()
	Function usb_close:Int(handle:Byte Ptr)
?win32
	Function usb_install_service_np:Int()
	Function usb_uninstall_service_np:Int()
?
	
	Function bmx_usb_open:Byte Ptr(device:Byte Ptr)
	Function bmx_usb_busses:Byte Ptr()
	
	Function bmx_usbbus_next:Byte Ptr(handle:Byte Ptr)
	Function bmx_usbbus_prev:Byte Ptr(handle:Byte Ptr)
	Function bmx_usbbus_dirname:String(handle:Byte Ptr)
	Function bmx_usbbus_devices:Byte Ptr(handle:Byte Ptr)
	Function bmx_usbbus_location:Int(handle:Byte Ptr)
	Function bmx_usbbus_rootdev:Byte Ptr(handle:Byte Ptr)

	Function bmx_usbdevicedescriptor_manufacturer:String(handle:Byte Ptr, udev:Byte Ptr)
	Function bmx_usbdevicedescriptor_product:String(handle:Byte Ptr, udev:Byte Ptr)
	Function bmx_usbdevicedescriptor_serialnumber:String(handle:Byte Ptr, udev:Byte Ptr)
	Function bmx_usbdevicedescriptor_vendorid:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_productid:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_numconfig:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_bcdusb:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_deviceclass:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_devicesubclass:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_deviceprotocol:Int(handle:Byte Ptr)
	Function bmx_usbdevicedescriptor_maxpacketsize:Int(handle:Byte Ptr)


	Function bmx_usbdevice_descriptor:Byte Ptr(handle:Byte Ptr)
	Function bmx_usbdevice_devnum:Int(handle:Byte Ptr)
	Function bmx_usbdevice_next:Byte Ptr(handle:Byte Ptr)
	Function bmx_usbdevice_hasconfig:Int(handle:Byte Ptr)
	Function bmx_usbdevice_config:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_usbdevice_numchildren:Int(handle:Byte Ptr)
	Function bmx_usbdevice_children:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_usbendpointdescriptor_endpointaddress:Int(handle:Byte Ptr)
	Function bmx_usbendpointdescriptor_attributes:Int(handle:Byte Ptr)
	Function bmx_usbendpointdescriptor_maxpacketsize:Int(handle:Byte Ptr)
	Function bmx_usbendpointdescriptor_interval:Int(handle:Byte Ptr)
	Function bmx_usbendpointdescriptor_refresh:Int(handle:Byte Ptr)
	Function bmx_usbendpointdescriptor_synchaddress:Int(handle:Byte Ptr)

	Function bmx_usbconfigdescriptor_totallength:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_numinterfaces:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_configurationvalue:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_configuration:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_attributes:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_maxpower:Int(handle:Byte Ptr)
	Function bmx_usbconfigdescriptor_interface:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_usbinterface_descriptor:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_usbinterface_numaltsetting:Int(handle:Byte Ptr)

	Function bmx_usbinterfacedescriptor_interfacenumber:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interfacealternatesetting:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interfacenumendpoints:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interfaceclass:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interfacesubclass:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interfaceprotocol:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_interface:Int(handle:Byte Ptr)
	Function bmx_usbinterfacedescriptor_endpoint:Byte Ptr(handle:Byte Ptr, index:Int)

	Function usb_bulk_write:Int(handle:Byte Ptr, endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
	Function usb_bulk_read:Int(handle:Byte Ptr, endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
	Function usb_interrupt_write:Int(handle:Byte Ptr, endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
	Function usb_interrupt_read:Int(handle:Byte Ptr, endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
	Function usb_control_msg:Int(handle:Byte Ptr, requestType:Int, request:Int, value:Int, index:Int, bytes:Byte Ptr, size:Int, timeout:Int)
	Function usb_set_configuration:Int(handle:Byte Ptr, configuration:Int)
	Function usb_claim_interface:Int(handle:Byte Ptr, interface:Int)
	Function usb_release_interface:Int(handle:Byte Ptr, interface:Int)
	Function usb_set_altinterface:Int(handle:Byte Ptr, alternate:Int)
	Function usb_clear_halt:Int(handle:Byte Ptr, endpoint:Int)
	Function usb_reset:Int(handle:Byte Ptr)

	Function bmx_usb_strerror:String()

End Extern


Function ToHex:String( val:Int, size:Int = 8 )
	Local buf:Short[size]
	For Local k:Int = (size-1) To 0 Step -1
		Local n:Int = (val&15) + 48
		If n > 57 n:+ 7
		buf[k]=n
		val:Shr 4
	Next
	Return String.FromShorts( buf, size )
End Function


Const USB_MAXENDPOINTS:Int = 32

Const USB_MAXINTERFACES:Int = 32

Const USB_MAXCONFIG:Int = 8

Const USB_MAXALTSETTING:Int = 128	' Hard limit

Const USB_ENDPOINT_ADDRESS_MASK:Int = $0f    ' in bEndpointAddress
Const USB_ENDPOINT_DIR_MASK:Int = $80

Const USB_ENDPOINT_TYPE_MASK:Int = $03    ' in bmAttributes
Const USB_ENDPOINT_TYPE_CONTROL:Int = 0
Const USB_ENDPOINT_TYPE_ISOCHRONOUS:Int = 1
Const USB_ENDPOINT_TYPE_BULK:Int = 2
Const USB_ENDPOINT_TYPE_INTERRUPT:Int = 3


' Standard requests
Const USB_REQ_GET_STATUS:Int = $00
Const USB_REQ_CLEAR_FEATURE:Int = $01
' $02 is reserved
Const USB_REQ_SET_FEATURE:Int = $03
' $04 is reserved
Const USB_REQ_SET_ADDRESS:Int = $05
Const USB_REQ_GET_DESCRIPTOR:Int = $06
Const USB_REQ_SET_DESCRIPTOR:Int = $07
Const USB_REQ_GET_CONFIGURATION:Int = $08
Const USB_REQ_SET_CONFIGURATION:Int = $09
Const USB_REQ_GET_INTERFACE:Int = $0A
Const USB_REQ_SET_INTERFACE:Int = $0B
Const USB_REQ_SYNCH_FRAME:Int = $0C

Const USB_TYPE_STANDARD:Int = $00 Shl 5
Const USB_TYPE_CLASS:Int = $01 Shl 5
Const USB_TYPE_VENDOR:Int = $02 Shl 5
Const USB_TYPE_RESERVED:Int = $03 Shl 5

Const USB_RECIP_DEVICE:Int = $00
Const USB_RECIP_INTERFACE:Int = $01
Const USB_RECIP_ENDPOINT:Int = $02
Const USB_RECIP_OTHER:Int = $03

' Various libusb API related stuff

Const USB_ENDPOINT_IN:Int = $80
Const USB_ENDPOINT_OUT:Int = $00

' Error codes */
Const USB_ERROR_BEGIN:Int = 500000


Const USB_CLASS_PER_INTERFACE:Int = 0	' For DeviceClass */
Const USB_CLASS_AUDIO:Int = 1
Const USB_CLASS_COMM:Int = 2
Const USB_CLASS_HID:Int = 3
Const USB_CLASS_PRINTER:Int = 7
Const USB_CLASS_MASS_STORAGE:Int = 8
Const USB_CLASS_HUB:Int = 9
Const USB_CLASS_DATA:Int = 10
Const USB_CLASS_VENDOR_SPEC:Int = $ff

' Descriptor types

Const USB_DT_DEVICE:Int = $01
Const USB_DT_CONFIG:Int = $02
Const USB_DT_STRING:Int = $03
Const USB_DT_INTERFACE:Int = $04
Const USB_DT_ENDPOINT:Int = $05

Const USB_DT_HID:Int = $21
Const USB_DT_REPORT:Int = $22
Const USB_DT_PHYSICAL:Int = $23
Const USB_DT_HUB:Int = $29
