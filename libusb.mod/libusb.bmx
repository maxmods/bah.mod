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

Rem
bbdoc: libusb
End Rem
Module BaH.libusb

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2007-2009 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

Import "common.bmx"


?win32
Rem
bbdoc: Installs Filter service, required on Win32.
about: libusb uses this to gain access to usb devices connected to the PC.
End Rem
Function USBInstallService:Int()
	Return usb_install_service_np()
End Function

Rem
bbdoc: Uninstalls Filter service.
End Rem
Function USBUninstallService:Int()
	Return usb_uninstall_service_np()
End Function
?

Rem
bbdoc: Initialize libusb
about: Just like the name implies, #USBInit sets up some internal structures. #USBInit <b>must</b> be called before any
other libusb functions.
End Rem
Function USBInit()
	usb_init()
End Function

Rem
bbdoc: Rescans all busses and devices.
about: Equivalent to calling #USBFindBusses and #USBFindDevices separately.
End Rem
Function USBRescan()
	USBFindBusses()
	USBFindDevices()
End Function

Rem
bbdoc: Finds all USB busses on system
returns: The number of changes since previous call to this function (total of new busses and busses removed).
about: It will find all of the busses on the system.
End Rem
Function USBFindBusses:Int()
	Return usb_find_busses()
End Function

Rem
bbdoc: Find all devices on all USB devices.
returns: The number of changes since the previous call to this function (total of new device and devices removed).
about: Will find all of the devices on each bus. This should be called after #USBFindBusses.
End Rem
Function USBFindDevices:Int()
	Return usb_find_devices()
End Function

Rem
bbdoc: Returns the list of USB busses found.
about: You can navigate the list by calling the @nextBus and @prevBus methods of the returned USBBus.
End Rem
Function USBGetBusses:USBBus()
	Return USBBus._create(bmx_usb_busses())
End Function

Rem
bbdoc: Opens a USB device.
returns: A handle used in future communication with the device.
about: Used to open up a device for use. #USBOpen must be called before attempting to perform any operations to the device.
End Rem
Function USBOpen:USBDeviceHandle(device:USBDevice)
	device.handle = USBDeviceHandle._create(bmx_usb_open(device.devicePtr), device)
	Return device.handle
End Function

Rem
bbdoc: Closes a USB device.
returns: 0 on success or < 0 on error.
about: Closes a device opened with #USBOpen. No further operations may be performed on the handle after #USBClose is called.
End Rem
Function USBClose:Int(handle:USBDeviceHandle)
	handle.device = Null
	Return usb_close(handle.handlePtr)
End Function

Rem
bbdoc: Returns the error message that caused the last error.
End Rem
Function USBError:String()
	Return bmx_usb_strerror()
End Function

Rem
bbdoc: 
End Rem
Type USBDeviceHandle

	Field handlePtr:Byte Ptr
	
	Field device:USBDevice

	Function _create:USBDeviceHandle(handlePtr:Byte Ptr, device:USBDevice)
		If handlePtr Then
			Local this:USBDeviceHandle = New USBDeviceHandle
			this.handlePtr = handlePtr
			this.device = device
			Return this
		End If
	End Function

	Rem
	bbdoc: Closes the device.
	returns: 0 on success or &lt; 0 on error.
	about: No further operations may be performed on the handle after close is called.
	<p>
	This is the same as calling #USBClose.
	</p>
	End Rem
	Method close:Int()
		Return USBClose(Self)
	End Method
	
	Rem
	bbdoc: Write data to a bulk endpoint.
	returns: The number of bytes written on success or &lt; 0 on error.
	about: Performs a bulk write request to the endpoint specified by @endpoint.
	End Rem
	Method bulkWrite:Int(endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
		Return usb_bulk_write(handlePtr, endpoint, bytes, size, timeout)
	End Method

	Rem
	bbdoc: Read data from a bulk endpoint.
	returns: The number of bytes read on success or &lt; 0 on error.
	about: Performs a bulk read request to the endpoint specified by @endpoint.
	End Rem
	Method bulkRead:Int(endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
		Return usb_bulk_read(handlePtr, endpoint, bytes, size, timeout)
	End Method

	Rem
	bbdoc: Write data to an interrupt endpoint.
	returns: The number of bytes written on success or &lt; 0 on error.
	about: Performs an interrupt write request to the endpoint specified by @endpoint.
	End Rem
	Method interruptWrite:Int(endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
		Return usb_interrupt_write(handlePtr, endpoint, bytes, size, timeout)
	End Method

	Rem
	bbdoc: Read data from a interrupt endpoint.
	returns: The number of bytes read on success or &lt; 0 on error.
	about: Performs a interrupt read request to the endpoint specified by @endpoint.
	End Rem
	Method interruptRead:Int(endpoint:Int, bytes:Byte Ptr, size:Int, timeout:Int)
		Return usb_interrupt_read(handlePtr, endpoint, bytes, size, timeout)
	End Method
	
	Rem
	bbdoc: Send a control message to the device.
	returns: The number of bytes written/read or &lt; 0 on error.
	about: Performs a control request to the default control pipe on a device. The parameters mirror the types of the
	same name in the USB specification.
	End Rem
	Method controlMsg:Int(requestType:Int, request:Int, value:Int, index:Int, bytes:Byte Ptr, size:Int, timeout:Int)
		Return usb_control_msg(handlePtr, requestType, request, value, index, bytes, size, timeout)
	End Method

	Rem
	bbdoc: Sets the active configuration of a device.
	returns: 0 on success or &lt; 0 on error.
	about: The @configuration parameter is the value as specified in the descriptor field configurationValue.
	End Rem
	Method setConfiguration:Int(configuration:Int)
		Return usb_set_configuration(handlePtr, configuration)
	End Method
	
	Rem
	bbdoc: Claim an interface of a device.
	returns: 0 on success or &lt; 0 on error.
	about: Claims the interface with the Operating System. The interface parameter is the value as specified in the
	descriptor field interfaceNumber.
	<p>
	claimInterface <b>must</b> be called before you perform any operations related to this interface (like setAltInterface,
	bulkWrite, etc).
	</p>
	End Rem
	Method claimInterface:Int(interface:Int)
		Return usb_claim_interface(handlePtr, interface)
	End Method
	
	Rem
	bbdoc: Releases a previously claimed interface.
	returns: 0 on success or &lt; 0 on error.
	about: Releases an interface previously claimed with claimInterface. The @interface parameter is the value as
	specified in the descriptor field interfaceNumber. 
	End Rem
	Method releaseInterface:Int(interface:Int)
		Return usb_release_interface(handlePtr, interface)
	End Method
	
	Rem
	bbdoc: Sets the active alternate setting of the current interface.
	returns: 0 on success or &lt; 0 on error.
	about: The @alternate parameter is the value as specified in the descriptor field alternateSetting.
	End Rem
	Method setAltInterface:Int(alternate:Int)
		Return usb_set_altinterface(handlePtr, alternate)
	End Method
	
	Rem
	bbdoc: Clears any halt status on an endpoint.
	returns: 0 on success or &lt; 0 on error.
	about: Clears any halt status on the specified endpoint. The @endpoint parameter is the value specified in the
	descriptor field endpointAddress.
	End Rem
	Method clearHalt:Int(endpoint:Int)
		Return usb_clear_halt(handlePtr, endpoint)
	End Method
	
	Rem
	bbdoc: Resets a device.
	returns: 0 on success or &lt; 0 on error.
	about: Resets the specified device by sending a RESET down the port it is connected to.
	<p>
	After calling reset, the device will need to re-enumerate and thusly, requires you to find the new device and open
	a new handle. The handle used to call reset will no longer work.
	</p>
	End Rem
	Method reset:Int()
		Return usb_reset(handlePtr)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type USBDescriptorHeader

	Field descriptorPtr:Byte Ptr
	
	Method length:Int()
		Return descriptorPtr[0]
	End Method
	
	Method descriptorType:Int()
		Return descriptorPtr[1]
	End Method


End Type

Rem
bbdoc: 
End Rem
Type USBStringDescriptorHeader Extends USBDescriptorHeader
End Type

Rem
bbdoc: 
End Rem
Type USBHIDDescriptor Extends USBDescriptorHeader

End Type

Rem
bbdoc: 
End Rem
Type USBEndpointDescriptor Extends USBDescriptorHeader

	Function _create:USBEndpointDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:USBEndpointDescriptor = New USBEndpointDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method endPointAddress:Int()
		Return bmx_usbendpointdescriptor_endpointaddress(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method attributes:Int()
		Return bmx_usbendpointdescriptor_attributes(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method maxPacketSize:Int()
		Return bmx_usbendpointdescriptor_maxpacketsize(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interval:Int()
		Return bmx_usbendpointdescriptor_interval(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method refresh:Int()
		Return bmx_usbendpointdescriptor_refresh(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method synchAddress:Int()
		Return bmx_usbendpointdescriptor_synchaddress(descriptorPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type USBInterfaceDescriptor Extends USBDescriptorHeader

	Function _create:USBInterfaceDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:USBInterfaceDescriptor = New USBInterfaceDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method interfaceNumber:Int()
		Return bmx_usbinterfacedescriptor_interfacenumber(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method alternateSetting:Int()
		Return bmx_usbinterfacedescriptor_interfacealternatesetting(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numEndpoints:Int()
		Return bmx_usbinterfacedescriptor_interfacenumendpoints(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interfaceClass:Int()
		Return bmx_usbinterfacedescriptor_interfaceclass(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interfaceSubClass:Int()
		Return bmx_usbinterfacedescriptor_interfacesubclass(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interfaceProtocol:Int()
		Return bmx_usbinterfacedescriptor_interfaceprotocol(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interface:Int()
		Return bmx_usbinterfacedescriptor_interface(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method endpoint:USBEndpointDescriptor(index:Int)
		Return USBEndpointDescriptor._create(bmx_usbinterfacedescriptor_endpoint(descriptorPtr, index))
	End Method

End Type

Rem
bbdoc: A USB interface.
End Rem
Type USBInterface

	Field interfacePtr:Byte Ptr

	Function _create:USBInterface(interfacePtr:Byte Ptr)
		If interfacePtr Then
			Local this:USBInterface = New USBInterface
			this.interfacePtr = interfacePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns an interface descriptor for the alternate setting at @index.
	End Rem
	Method altSetting:USBInterfaceDescriptor(index:Int)
		Return USBInterfaceDescriptor._create(bmx_usbinterface_descriptor(interfacePtr, index))
	End Method
	
	Rem
	bbdoc: Returns the number of alternate settings.
	End Rem
	Method numAltSetting:Int()
		Return bmx_usbinterface_numaltsetting(interfacePtr)
	End Method

End Type


Rem
bbdoc: 
End Rem
Type USBConfigDescriptor Extends USBDescriptorHeader

	Function _create:USBConfigDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:USBConfigDescriptor = New USBConfigDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method totalLength:Int()
		Return bmx_usbconfigdescriptor_totallength(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numInterfaces:Int()
		Return bmx_usbconfigdescriptor_numinterfaces(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method configurationValue:Int()
		Return bmx_usbconfigdescriptor_configurationvalue(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method configuration:Int()
		Return bmx_usbconfigdescriptor_configuration(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method attributes:Int()
		Return bmx_usbconfigdescriptor_attributes(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method maxPower:Int()
		Return bmx_usbconfigdescriptor_maxpower(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method interface:USBInterface(index:Int)
		Return USBInterface._create(bmx_usbconfigdescriptor_interface(descriptorPtr, index))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type USBDeviceDescriptor Extends USBDescriptorHeader

	Field device:USBDevice

	Function _create:USBDeviceDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:USBDeviceDescriptor = New USBDeviceDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function
	

	Rem
	bbdoc: 
	End Rem
	Method bcdUSB:Int()
		Return bmx_usbdevicedescriptor_bcdusb(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The class id for the device.
	about: One of USB_CLASS_AUDIO, USB_CLASS_COMM, USB_CLASS_HID, USB_CLASS_PRINTER, USB_CLASS_MASS_STORAGE, 
	USB_CLASS_HUB, USB_CLASS_DATA or USB_CLASS_VENDOR_SPEC.
	End Rem
	Method deviceClass:Int()
		Return bmx_usbdevicedescriptor_deviceclass(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method deviceSubClass:Int()
		Return bmx_usbdevicedescriptor_devicesubclass(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method deviceProtocol:Int()
		Return bmx_usbdevicedescriptor_deviceprotocol(descriptorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method maxPacketSize:Int()
		Return bmx_usbdevicedescriptor_maxpacketsize(descriptorPtr)
	End Method
	
	
	Rem
	bbdoc: Returns the name of the manufacturer, or Null if not available.
	End Rem
	Method manufacturer:String()
		Return bmx_usbdevicedescriptor_manufacturer(descriptorPtr, device.handle.handlePtr)
	End Method
	
	Rem
	bbdoc: Returns the name of the product, or Null if not available.
	End Rem
	Method product:String()
		Return bmx_usbdevicedescriptor_product(descriptorPtr, device.handle.handlePtr)
	End Method
	
	Rem
	bbdoc: Returns the device serial number, or Null if not available.
	End Rem
	Method serialNumber:String()
		Return bmx_usbdevicedescriptor_serialnumber(descriptorPtr, device.handle.handlePtr)
	End Method
	
	Rem
	bbdoc: Returns the vendor id.
	End Rem
	Method vendorId:Int()
		Return bmx_usbdevicedescriptor_vendorid(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Returns the product id.
	End Rem
	Method productId:Int()
		Return bmx_usbdevicedescriptor_productid(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of configuration for the device.
	End Rem
	Method numConfigurations:Int()
		Return bmx_usbdevicedescriptor_numconfig(descriptorPtr)
	End Method


	Method Delete()
		device = Null
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type USBCtrlSetup
End Type


Rem
bbdoc: A USB Bus.
End Rem
Type USBBus

	Field busPtr:Byte Ptr
	
	Function _create:USBBus(busPtr:Byte Ptr)
		If busPtr Then
			Local this:USBBus = New USBBus
			this.busPtr = busPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the next bus, or Null if none available.
	End Rem
	Method nextBus:USBBus()
		Return _create(bmx_usbbus_next(busPtr))
	End Method
	
	Rem
	bbdoc: Returns the previous bus, or Null if none available.
	End Rem
	Method prevBus:USBBus()
		Return _create(bmx_usbbus_prev(busPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method dirName:String()
		Return bmx_usbbus_dirname(busPtr)
	End Method
	
	Rem
	bbdoc: Returns the first in a list of devices on this bus.
	End Rem
	Method devices:USBDevice()
		Return USBDevice._create(bmx_usbbus_devices(busPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method location:Int()
		Return bmx_usbbus_location(busPtr)
	End Method
	
	Rem
	bbdoc: Returns the root device for this bus.
	End Rem
	Method rootDevice:USBDevice()
		Return USBDevice._create(bmx_usbbus_rootdev(busPtr))
	End Method
	
	
End Type

Rem
bbdoc: A USB Device.
End Rem
Type USBDevice

	Field devicePtr:Byte Ptr
	
	Field handle:USBDeviceHandle
	
	Function _create:USBDevice(devicePtr:Byte Ptr)
		If devicePtr Then
			Local this:USBDevice = New USBDevice
			this.devicePtr = devicePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the next device, or Null if none available.
	End Rem
	Method nextDevice:USBDevice()
		Return _create(bmx_usbdevice_next(devicePtr))
	End Method

	Rem
	bbdoc: Returns the previous device, or Null if none available.
	End Rem
	Method descriptor:USBDeviceDescriptor()
		Local desc:USBDeviceDescriptor = USBDeviceDescriptor._create(bmx_usbdevice_descriptor(devicePtr))
		desc.device = Self
		Return desc
	End Method
	
	Rem
	bbdoc: Returns the device number.
	End Rem
	Method deviceNumber:Int()
		Return bmx_usbdevice_devnum(devicePtr)
	End Method
	
	Rem
	bbdoc: Returns True if this device has configuration information.
	End Rem
	Method hasConfig:Int()
		Return bmx_usbdevice_hasconfig(devicePtr)
	End Method
	
	Rem
	bbdoc: Returns a configuration descriptor.
	End Rem
	Method config:USBConfigDescriptor(index:Int)
		Return USBConfigDescriptor._create(bmx_usbdevice_config(devicePtr, index))
	End Method
	
	Rem
	bbdoc: Returns the number of child devices.
	End Rem
	Method numChildren:Int()
		Return bmx_usbdevice_numchildren(devicePtr)
	End Method
	
	Rem
	bbdoc: Returns the child device at @index.
	End Rem
	Method children:USBDevice(index:Int)
		Return _create(bmx_usbdevice_children(devicePtr, index))
	End Method
	
	Method Delete()
		handle = Null
	End Method
	
End Type

