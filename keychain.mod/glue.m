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

#import <AppKit/AppKit.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <CoreServices/CoreServices.h>

#include <blitz.h>

// string conversion
BBString *kbbStringFromNSString(NSString *s){
	BBString *bbstring;
	unsigned short *buff;
	int n;
	n = [s length];
	buff = malloc(n*2);
	[s getCharacters:buff];
	bbstring = bbStringFromShorts(buff,n);
	free(buff);
	return bbstring;	
}

// string conversion
NSString *kbbStringToNSString(BBString *s){
	return [NSString stringWithCharacters:s->buf length:s->length];
}

// --------------------------------------------------------

extern void _bah_keychain_TKeychain__postLastError(int err);

// --------------------------------------------------------

@interface MaxKeyChain : NSObject{
	SecKeychainRef keychain;
}
-(id)init;
-(void)dealloc;
-(SecKeychainRef)Keychain;
@end

@implementation MaxKeyChain
-(id)init {
    self = [super init];
    return self;
}

-(void)dealloc {
	if (keychain) {
		CFRelease(keychain);
	}
    [super dealloc];
}

-(SecKeychainRef)Keychain {
	return keychain;
}

@end

// --------------------------------------------------------

@interface MaxKeyChainItem : NSObject{
	SecKeychainItemRef keychainItem;
}
-(id)init;
-(void)dealloc;
-(SecKeychainItemRef)KeychainItem;
@end

@implementation MaxKeyChainItem
-(id)init {
    self = [super init];
    return self;
}

-(void)dealloc {
	if (keychainItem) {
		CFRelease(keychainItem);
	}
    [super dealloc];
}

-(SecKeychainItemRef)KeychainItem {
	return keychainItem;
}

@end

// --------------------------------------------------------

MaxKeyChain * bmx_keychain_open(BBString * path) {
	SecKeychainRef ref;

	MaxKeyChain * keychain = [[MaxKeyChain alloc] init];
	ref = [keychain Keychain];
	
	if (path != &bbEmptyString) {
		char * p = bbStringToUTF8String(path);
		
		OSStatus err = SecKeychainOpen(p, &ref);
		
		_bah_keychain_TKeychain__postLastError(err);

		if (err) {
			[keychain release];
			bbMemFree(p);
			return 0;
		}

		bbMemFree(p);
	} else {
		OSStatus err = SecKeychainCopyDefault(&ref);
		
		_bah_keychain_TKeychain__postLastError(err);

		if (err) {
			[keychain release];
			return 0;
		}
		
	}
	
	return keychain;
}

void bmx_keychain_free(MaxKeyChain * keychain) {
	[keychain release];
}

MaxKeyChain * bmx_keychain_create(BBString * path, BBString * password, int promptUser) {
	SecKeychainRef ref;

	MaxKeyChain * keychain = [[MaxKeyChain alloc] init];
	ref = [keychain Keychain];

	char * p = bbStringToUTF8String(path);
	char * pass = 0;
	
	if (!promptUser) {
		pass = bbStringToUTF8String(password);
		
		OSStatus err = SecKeychainCreate(p, strlen(pass), pass, FALSE, NULL, &ref);
		
		_bah_keychain_TKeychain__postLastError(err);
		
		if (err) {
			[keychain release];
			return 0;
		}
	} else {
		OSStatus err = SecKeychainCreate(p, 0, NULL, TRUE, NULL, &ref);
		
		_bah_keychain_TKeychain__postLastError(err);
		
		if (err) {
			[keychain release];
			return 0;
		}
	}

	return keychain;	
}

BBString * bmx_keychain_SecCopyErrorMessageString(int status) {
	return kbbStringFromNSString(SecCopyErrorMessageString(status, NULL));
}

void bmx_keychain_SecKeychainSetUserInteractionAllowed(int allowed) {
	OSStatus err = SecKeychainSetUserInteractionAllowed(allowed);

	_bah_keychain_TKeychain__postLastError(err);
}

int bmx_keychain_SecKeychainGetUserInteractionAllowed() {
	int value;
	
	OSStatus err = SecKeychainGetUserInteractionAllowed(&value);
	
	_bah_keychain_TKeychain__postLastError(err);

	return value;
}

BBString * bmx_keychain_SecKeychainFindInternetPassword(MaxKeyChain * keychain, int protocol, BBString * serverName, BBString * accountName, BBString * path,
		int authenticationType, BBString * securityDomain, int port, MaxKeyChainItem * item) {

	char * sname = bbStringToUTF8String(serverName);
	char * aname = bbStringToUTF8String(accountName);
	char * p = bbStringToUTF8String(path);
	char * sdom = 0;
	if (securityDomain != &bbEmptyString) {
		sdom = bbStringToUTF8String(securityDomain);
	}

	int plength;
	char * password;
	SecKeychainItemRef itemRef = 0;
	
	if (item) {
		itemRef = [item KeychainItem];
	}

	OSStatus err = SecKeychainFindInternetPassword([keychain Keychain], strlen(sname), sname, (sdom) ? strlen(sdom) : 0, sdom, 
		strlen(aname), aname, strlen(p), p, port, protocol, authenticationType, &plength, &password, (item) ? &itemRef : NULL);

	bbMemFree(sname);
	bbMemFree(aname);
	bbMemFree(p);
	if (sdom) bbMemFree(sdom);
	
	_bah_keychain_TKeychain__postLastError(err);
	
	BBString * pass = &bbEmptyString;
	
	if (err == noErr) {
		pass = bbStringFromUTF8String(password);
		SecKeychainItemFreeContent(NULL, password);
	}

	return pass;
}

void bmx_keychain_SecKeychainAddInternetPassword(MaxKeyChain * keychain, BBString * password, int protocol, BBString * serverName, BBString * accountName, BBString * path,
		int authenticationType, BBString * securityDomain, int port, MaxKeyChainItem * item) {
	
	char * pass = bbStringToUTF8String(password);
	char * sname = bbStringToUTF8String(serverName);
	char * aname = bbStringToUTF8String(accountName);
	char * p = bbStringToUTF8String(path);
	char * sdom = 0;
	if (securityDomain != &bbEmptyString) {
		sdom = bbStringToUTF8String(securityDomain);
	}

	SecKeychainItemRef itemRef = 0;
	if (item) {
		itemRef = [item KeychainItem];
	}

	OSStatus err = SecKeychainAddInternetPassword([keychain Keychain], strlen(sname), sname, (sdom) ? strlen(sdom) : 0, sdom, 
		strlen(aname), aname, strlen(p), p, port, protocol, authenticationType, strlen(pass), pass, (item) ? &itemRef : NULL);
	
	_bah_keychain_TKeychain__postLastError(err);
	
	bbMemFree(pass);
	bbMemFree(sname);
	bbMemFree(aname);
	bbMemFree(p);
	if (sdom) bbMemFree(sdom);
	
}

BBString * bmx_keychain_SecKeychainFindGenericPassword(MaxKeyChain * keychain, BBString * serviceName, BBString * accountName, MaxKeyChainItem * item) {

	char * sname = bbStringToUTF8String(serviceName);
	char * aname = bbStringToUTF8String(accountName);

	int plength;
	char * password;
	SecKeychainItemRef itemRef = 0;
	
	if (item) {
		itemRef = [item KeychainItem];
	}

	OSStatus err = SecKeychainFindGenericPassword([keychain Keychain], strlen(sname), sname, strlen(aname), aname, &plength, &password, 
		(item) ? &itemRef : NULL);
	
	_bah_keychain_TKeychain__postLastError(err);
	
	BBString * pass = &bbEmptyString;
	
	if (err == noErr) {
		pass = bbStringFromUTF8String(password);
		SecKeychainItemFreeContent(NULL, password);
	}

	bbMemFree(sname);
	bbMemFree(aname);
	return pass;

}

void bmx_keychain_SecKeychainAddGenericPassword(MaxKeyChain * keychain, BBString * password, BBString * serviceName, BBString * accountName, MaxKeyChainItem * item) {

	char * pass = bbStringToUTF8String(password);
	char * sname = bbStringToUTF8String(serviceName);
	char * aname = bbStringToUTF8String(accountName);

	SecKeychainItemRef itemRef = 0;
	if (item) {
		itemRef = [item KeychainItem];
	}

	OSStatus err = SecKeychainAddGenericPassword([keychain Keychain], strlen(pass), pass, strlen(sname), sname, strlen(aname), aname, (item) ? &itemRef : NULL);
	
	_bah_keychain_TKeychain__postLastError(err);
	
	bbMemFree(pass);
	bbMemFree(sname);
	bbMemFree(aname);

}

void bmx_keychain_SecKeychainUnlock(MaxKeyChain * keychain, BBString * password, int usePassword) {
	char * p = 0;
	int length = 0;
	if (password != &bbEmptyString) {
		p = bbStringToUTF8String(password);
		length = strlen(p);
	}
	
	OSStatus err = SecKeychainUnlock([keychain Keychain], length, p, usePassword);
	
	_bah_keychain_TKeychain__postLastError(err);
	
	if (p) bbMemFree(p);
}

void bmx_keychain_SecKeychainSetDefault(MaxKeyChain * keychain) {
	OSStatus err = SecKeychainSetDefault([keychain Keychain]);
	_bah_keychain_TKeychain__postLastError(err);	
}

int bmx_keychain_SecKeychainGetStatus(MaxKeyChain * keychain) {
	int status;
	
	OSStatus err = SecKeychainGetStatus([keychain Keychain], &status);
	
	_bah_keychain_TKeychain__postLastError(err);
	return status;
}

BBString * bmx_keychain_SecKeychainGetPath(MaxKeyChain * keychain) {
	int length = 2048;
	char path[length];
	
	OSStatus err = SecKeychainGetPath([keychain Keychain], &length, path);

	_bah_keychain_TKeychain__postLastError(err);
	
	return bbStringFromUTF8String(path);
}

void bmx_keychain_SecKeychainDelete(MaxKeyChain * keychain) {
	OSStatus err = SecKeychainDelete([keychain Keychain]);
	_bah_keychain_TKeychain__postLastError(err);
}

// --------------------------------------------------------

MaxKeyChainItem * bmx_keychainitem_create() {
	return [[MaxKeyChain alloc] init];
}

void bmx_keychainitem_free(MaxKeyChainItem * item) {
	[item release];
}

void bmx_keychainitem_SecKeychainItemModifyAttributesAndData(MaxKeyChainItem * item, BBString * password) {
	char * p = bbStringToUTF8String(password);
	
	OSStatus err = SecKeychainItemModifyAttributesAndData([item KeychainItem], NULL, strlen(p), p);
	
	_bah_keychain_TKeychain__postLastError(err);

	bbMemFree(p);	
}

void bmx_keychainitem_SecKeychainItemDelete(MaxKeyChainItem * item) {
	OSStatus err = SecKeychainItemDelete([item KeychainItem]);
	_bah_keychain_TKeychain__postLastError(err);
}

