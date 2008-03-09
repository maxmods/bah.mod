/*
 Copyright (c) 2008, Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <blitz.h>

#include "Growl.h"
#include <CoreFoundation/CoreFoundation.h>


extern "C" {

	struct Growl_Delegate * bmx_Growl_Delegate_create();
	Boolean bmx_Growl_SetDelegate(struct Growl_Delegate *newDelegate);
	void bmx_growldelegate_setapplicationname(struct Growl_Delegate * delegate, BBString * name);
	void bmx_growldelegate_setregistrationdictionary_array(struct Growl_Delegate * delegate, BBArray * allNot, BBArray * defNot, BBString * id);
	void bmx_growldelegate_setapplicationicondata(struct Growl_Delegate * delegate, const UInt8 *pix, int size);

	struct Growl_Notification * bmx_Growl_Notification_create(BBString * name, BBString * title, BBString * description);
	void bmx_Growl_PostNotification(const struct Growl_Notification *notification);
	void bmx_Growl_Notification_delete(struct Growl_Notification *notification);
	void bmx_Growl_Notification_setname(struct Growl_Notification *notification, BBString * name);
	void bmx_Growl_Notification_settitle(struct Growl_Notification *notification, BBString * title);
	void bmx_Growl_Notification_setdescription(struct Growl_Notification *notification, BBString * description);
	void bmx_Growl_Notification_setpriority(struct Growl_Notification *notification, int priority);
	void bmx_Growl_Notification_seticondata(struct Growl_Notification *notification, const UInt8 *pix, int size);

	Boolean bmx_Growl_IsInstalled();
	Boolean bmx_Growl_IsRunning();

}

CFStringRef CFStringFromWString( const UniChar *str ){
	int sz=0;
	const UniChar *p=str;
	
	while( *p++ ) ++sz;
	
	return CFStringCreateWithCharacters(kCFAllocatorDefault, str , sz);
}

CFStringRef CFStringFromBBString( const BBString *str ){
	UniChar *p = bbStringToWString( (BBString*)str );
	CFStringRef t = CFStringFromWString(p);
	bbMemFree( p );
	return t;
}

// ************************

struct Growl_Delegate * bmx_Growl_Delegate_create() {
	struct Growl_Delegate * delegate = new struct Growl_Delegate();
	InitGrowlDelegate(delegate);
	return delegate;
}

Boolean bmx_Growl_SetDelegate(struct Growl_Delegate *newDelegate) {
	return Growl_SetDelegate(newDelegate);
}

void bmx_growldelegate_setapplicationname(struct Growl_Delegate * delegate, BBString * name) {
	delegate->applicationName = CFStringFromBBString(name);
}

void bmx_growldelegate_setapplicationicondata(struct Growl_Delegate * delegate, const UInt8 *pix, int size) {
	if (delegate->applicationIconData) {
		CFRelease(delegate->applicationIconData);
	}
	
	if (pix && size > 0) {
		delegate->applicationIconData = CFDataCreate(kCFAllocatorDefault, pix, size);
	} else {
		delegate->applicationIconData = NULL;
	}
}

CFMutableArrayRef CFArrayFromBBStringArray(BBArray * array) {

	int size = array->scales[0];
	CFMutableArrayRef list = CFArrayCreateMutable(kCFAllocatorDefault,
					size, &kCFTypeArrayCallBacks);
	
	BBString **s=(BBString**)BBARRAYDATA(array, array->dims);

	for (int i = 0; i < size; i++) {
		CFStringRef value = CFStringFromBBString(s[i]);
		CFArrayAppendValue(list, value);
		CFRelease(value);
	}

	return list;
}

void bmx_growldelegate_setregistrationdictionary_array(struct Growl_Delegate * delegate, BBArray * allNot, BBArray * defNot, BBString * id) {
	if (delegate->registrationDictionary) {
		CFRelease(delegate->registrationDictionary);
	}

	CFMutableArrayRef cfAll = CFArrayFromBBStringArray(allNot);
	CFMutableArrayRef cfDef = CFArrayFromBBStringArray(defNot);
	CFStringRef cfId = CFStringFromBBString(id);

	
	CFTypeRef values[] = { cfAll, cfDef, cfId };

	CFTypeRef keys[] = { GROWL_NOTIFICATIONS_ALL, GROWL_NOTIFICATIONS_DEFAULT, GROWL_APP_ID };
	
	delegate->registrationDictionary = CFDictionaryCreate(kCFAllocatorDefault,
						keys,
						values,
						sizeof(keys) / sizeof(keys[0]),
						&kCFTypeDictionaryKeyCallBacks,
						&kCFTypeDictionaryValueCallBacks);
	
	CFRelease(cfAll);
	CFRelease(cfDef);
	CFRelease(cfId);
	
}

// **********************

struct Growl_Notification * bmx_Growl_Notification_create(BBString * name, BBString * title, BBString * description) {
	struct Growl_Notification * notification = new struct Growl_Notification();
	InitGrowlNotification(notification);
	
	notification->name = CFStringFromBBString(name);
	notification->title = CFStringFromBBString(title);
	notification->description = CFStringFromBBString(description);
	
	return notification;
}

void bmx_Growl_PostNotification(const struct Growl_Notification *notification) {
	Growl_PostNotification(notification);
}

void bmx_Growl_Notification_delete(struct Growl_Notification *notification) {
	if (notification->name) {
		CFRelease(notification->name);
	}
	if (notification->title) {
		CFRelease(notification->title);
	}
	if (notification->description) {
		CFRelease(notification->description);
	}
	if (notification->iconData) {
		CFRelease(notification->iconData);
	}
	
	delete notification;
}

void bmx_Growl_Notification_setname(struct Growl_Notification *notification, BBString * name) {
	if (notification->name) {
		CFRelease(notification->name);
	}
	notification->name = CFStringFromBBString(name);
}

void bmx_Growl_Notification_settitle(struct Growl_Notification *notification, BBString * title) {
	if (notification->title) {
		CFRelease(notification->title);
	}
	notification->title = CFStringFromBBString(title);
}

void bmx_Growl_Notification_setdescription(struct Growl_Notification *notification, BBString * description) {
	if (notification->description) {
		CFRelease(notification->description);
	}
	notification->description = CFStringFromBBString(description);
}

void bmx_Growl_Notification_setpriority(struct Growl_Notification *notification, int priority) {
	notification->priority = priority;
}

void bmx_Growl_Notification_seticondata(struct Growl_Notification *notification, const UInt8 *pix, int size) {
	if (notification->iconData) {
		CFRelease(notification->iconData);
	}
	
	if (pix && size > 0) {
		notification->iconData = CFDataCreate(kCFAllocatorDefault, pix, size);
	} else {
		notification->iconData = NULL;
	}
}


// **********************

Boolean bmx_Growl_IsInstalled() {
	return Growl_IsInstalled();
}

Boolean bmx_Growl_IsRunning() {
	return Growl_IsRunning();
}
