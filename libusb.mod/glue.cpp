/*
  Copyright (c) 2007-2009 Bruce A Henderson
 
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

#include "usb.h";
#include <blitz.h>

extern "C" {

	struct usb_bus * bmx_usbbus_next(struct usb_bus * bus);
	struct usb_bus * bmx_usbbus_prev(struct usb_bus * bus);
	BBString * bmx_usbbus_dirname(struct usb_bus * bus);
	struct usb_device * bmx_usbbus_devices(struct usb_bus * bus);
	unsigned long bmx_usbbus_location(struct usb_bus * bus);
	struct usb_device * bmx_usbbus_rootdev(struct usb_bus * bus);

	usb_dev_handle * bmx_usb_open(struct usb_device *dev);
	struct usb_bus * bmx_usb_busses();

	BBString * bmx_usbdevicedescriptor_manufacturer(struct usb_device_descriptor * desc, usb_dev_handle * udev);
	BBString * bmx_usbdevicedescriptor_product(struct usb_device_descriptor * desc, usb_dev_handle * udev);
	BBString * bmx_usbdevicedescriptor_serialnumber(struct usb_device_descriptor * desc, usb_dev_handle * udev);
	int bmx_usbdevicedescriptor_vendorid(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_productid(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_numconfig(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_bcdusb(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_deviceclass(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_devicesubclass(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_deviceprotocol(struct usb_device_descriptor * desc);
	int bmx_usbdevicedescriptor_maxpacketsize(struct usb_device_descriptor * desc);


	struct usb_device_descriptor * bmx_usbdevice_descriptor(struct usb_device *dev);
	int bmx_usbdevice_devnum(struct usb_device *dev);
	struct usb_device * bmx_usbdevice_next(struct usb_device *dev);
	bool bmx_usbdevice_hasconfig(struct usb_device *dev);
	struct usb_config_descriptor * bmx_usbdevice_config(struct usb_device *dev, int index);
	int bmx_usbdevice_numchildren(struct usb_device *dev);
	struct usb_device * bmx_usbdevice_children(struct usb_device *dev, int index);

	int bmx_usbendpointdescriptor_endpointaddress(struct usb_endpoint_descriptor * endpoint);
	int bmx_usbendpointdescriptor_attributes(struct usb_endpoint_descriptor * endpoint);
	int bmx_usbendpointdescriptor_maxpacketsize(struct usb_endpoint_descriptor * endpoint);
	int bmx_usbendpointdescriptor_interval(struct usb_endpoint_descriptor * endpoint);
	int bmx_usbendpointdescriptor_refresh(struct usb_endpoint_descriptor * endpoint);
	int bmx_usbendpointdescriptor_synchaddress(struct usb_endpoint_descriptor * endpoint);

	int bmx_usbconfigdescriptor_totallength(struct usb_config_descriptor * config);
	int bmx_usbconfigdescriptor_numinterfaces(struct usb_config_descriptor * config);
	int bmx_usbconfigdescriptor_configurationvalue(struct usb_config_descriptor * config);
	int bmx_usbconfigdescriptor_configuration(struct usb_config_descriptor * config);
	int bmx_usbconfigdescriptor_attributes(struct usb_config_descriptor * config);
	int bmx_usbconfigdescriptor_maxpower(struct usb_config_descriptor * config);
	struct usb_interface * bmx_usbconfigdescriptor_interface(struct usb_config_descriptor * config, int index);

	struct usb_interface_descriptor * bmx_usbinterface_descriptor(struct usb_interface * interface, int index);
	int bmx_usbinterface_numaltsetting(struct usb_interface * interface);

	int bmx_usbinterfacedescriptor_interfacenumber(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interfacealternatesetting(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interfacenumendpoints(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interfaceclass(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interfacesubclass(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interfaceprotocol(struct usb_interface_descriptor * interface);
	int bmx_usbinterfacedescriptor_interface(struct usb_interface_descriptor * interface);
	struct usb_endpoint_descriptor * bmx_usbinterfacedescriptor_endpoint(struct usb_interface_descriptor * interface, int index);

	BBString * bmx_usb_strerror();

}


struct usb_bus * bmx_usbbus_next(struct usb_bus * bus) {
	return bus->next;
}

struct usb_bus * bmx_usbbus_prev(struct usb_bus * bus) {
	return bus->prev;
}

BBString * bmx_usbbus_dirname(struct usb_bus * bus) {
	return bbStringFromCString(bus->dirname);
}

struct usb_device * bmx_usbbus_devices(struct usb_bus * bus) {
	return bus->devices;
}

unsigned long bmx_usbbus_location(struct usb_bus * bus) {
	return bus->location;
}

struct usb_device * bmx_usbbus_rootdev(struct usb_bus * bus) {
	return bus->root_dev;
}

usb_dev_handle * bmx_usb_open(struct usb_device *dev) {
	return usb_open(dev);
}

struct usb_bus * bmx_usb_busses() {
	return usb_get_busses();
}

BBString * bmx_usbdevicedescriptor_manufacturer(struct usb_device_descriptor * desc, usb_dev_handle * udev) {
	char string[256];
	if (desc->iManufacturer) {
		int ret = usb_get_string_simple(udev, desc->iManufacturer, string, sizeof(string));
		if (ret > 0) {
			return bbStringFromCString(string);
		}
	}
	return &bbEmptyString;
}

BBString * bmx_usbdevicedescriptor_product(struct usb_device_descriptor * desc, usb_dev_handle * udev) {
	char string[256];
	if (desc->iProduct) {
		int ret = usb_get_string_simple(udev, desc->iProduct, string, sizeof(string));
		if (ret > 0) {
			return bbStringFromCString(string);
		}
	}
	return &bbEmptyString;
}

BBString * bmx_usbdevicedescriptor_serialnumber(struct usb_device_descriptor * desc, usb_dev_handle * udev) {
	char string[256];
	if (desc->iSerialNumber) {
		int ret = usb_get_string_simple(udev, desc->iSerialNumber, string, sizeof(string));
		if (ret > 0) {
			return bbStringFromCString(string);
		}
	}
	return &bbEmptyString;
}

int bmx_usbdevicedescriptor_vendorid(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->idVendor);
}

int bmx_usbdevicedescriptor_productid(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->idProduct);
}

int bmx_usbdevicedescriptor_numconfig(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bNumConfigurations);
}

int bmx_usbdevicedescriptor_bcdusb(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bcdUSB);
}

int bmx_usbdevicedescriptor_deviceclass(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bDeviceClass);
}

int bmx_usbdevicedescriptor_devicesubclass(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bDeviceSubClass);
}

int bmx_usbdevicedescriptor_deviceprotocol(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bDeviceProtocol);
}

int bmx_usbdevicedescriptor_maxpacketsize(struct usb_device_descriptor * desc) {
	return static_cast<int>(desc->bMaxPacketSize0);
}




struct usb_device_descriptor * bmx_usbdevice_descriptor(struct usb_device *dev) {
	return &dev->descriptor;
}

int bmx_usbdevice_devnum(struct usb_device *dev) {
	return static_cast<int>(dev->devnum);
}

struct usb_device * bmx_usbdevice_next(struct usb_device *dev) {
	return dev->next;
}

bool bmx_usbdevice_hasconfig(struct usb_device *dev) {
	if (dev->config) {
		return true;
	}
	return false;
}

int bmx_usbdevice_numchildren(struct usb_device *dev) {
	return static_cast<int>(dev->num_children);
}

struct usb_device * bmx_usbdevice_children(struct usb_device *dev, int index) {
	return dev->children[index];
}



struct usb_config_descriptor * bmx_usbdevice_config(struct usb_device *dev, int index) {
	return &dev->config[index];
}


int bmx_usbendpointdescriptor_endpointaddress(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->bEndpointAddress);
}

int bmx_usbendpointdescriptor_attributes(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->bmAttributes);
}

int bmx_usbendpointdescriptor_maxpacketsize(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->wMaxPacketSize);
}

int bmx_usbendpointdescriptor_interval(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->bInterval);
}

int bmx_usbendpointdescriptor_refresh(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->bRefresh);
}

int bmx_usbendpointdescriptor_synchaddress(struct usb_endpoint_descriptor * endpoint) {
	return static_cast<int>(endpoint->bSynchAddress);
}


int bmx_usbconfigdescriptor_totallength(struct usb_config_descriptor * config) {
	return static_cast<int>(config->wTotalLength);
}

int bmx_usbconfigdescriptor_numinterfaces(struct usb_config_descriptor * config) {
	return static_cast<int>(config->bNumInterfaces);
}

int bmx_usbconfigdescriptor_configurationvalue(struct usb_config_descriptor * config) {
	return static_cast<int>(config->bConfigurationValue);
}

int bmx_usbconfigdescriptor_configuration(struct usb_config_descriptor * config) {
	return static_cast<int>(config->iConfiguration);
}

int bmx_usbconfigdescriptor_attributes(struct usb_config_descriptor * config) {
	return static_cast<int>(config->bmAttributes);
}

int bmx_usbconfigdescriptor_maxpower(struct usb_config_descriptor * config) {
	return static_cast<int>(config->MaxPower);
}

struct usb_interface * bmx_usbconfigdescriptor_interface(struct usb_config_descriptor * config, int index) {
	return &config->interface[index];
}

struct usb_interface_descriptor * bmx_usbinterface_descriptor(struct usb_interface * interface, int index) {
	return &interface->altsetting[index];
}

int bmx_usbinterface_numaltsetting(struct usb_interface * interface) {
	return interface->num_altsetting;
}


int bmx_usbinterfacedescriptor_interfacenumber(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bInterfaceNumber);
}

int bmx_usbinterfacedescriptor_interfacealternatesetting(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bAlternateSetting);
}

int bmx_usbinterfacedescriptor_interfacenumendpoints(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bNumEndpoints);
}

int bmx_usbinterfacedescriptor_interfaceclass(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bInterfaceClass);
}

int bmx_usbinterfacedescriptor_interfacesubclass(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bInterfaceSubClass);
}

int bmx_usbinterfacedescriptor_interfaceprotocol(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->bInterfaceProtocol);
}

int bmx_usbinterfacedescriptor_interface(struct usb_interface_descriptor * interface) {
	return static_cast<int>(interface->iInterface);
}

struct usb_endpoint_descriptor * bmx_usbinterfacedescriptor_endpoint(struct usb_interface_descriptor * interface, int index) {
	return &interface->endpoint[index];
}

BBString * bmx_usb_strerror() {
	return bbStringFromCString(usb_strerror());
}


