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

#include <AppKit/AppKit.h>
#import <Sparkle/Sparkle.h>

#include "Blitz.h"

@interface MaxSUUpdater : SUUpdater {
}
+ (id)sharedUpdater;
- (id)init:(BBObject *)obj;
@end

void _bah_sparkle_TSUUpdater__callback(BBObject * obj, MaxSUUpdater * upd);

id updater = nil;

void bmx_sparkle_init(BBObject * obj) {
	NSAutoreleasePool*	aPool;

	aPool = [[NSAutoreleasePool alloc] init];

	
	updater = [[MaxSUUpdater alloc] init:obj];
	
	[aPool release];

}


@implementation MaxSUUpdater {
}

+ (id)sharedUpdater
{
    return [self updaterForBundle:[NSBundle bundleForClass:[self class]]];
}

- (id)init:(BBObject *)obj
{
	_bah_sparkle_TSUUpdater__callback(obj, self);
    return [self initForBundle:[NSBundle bundleForClass:[self class]]];
}

@end

void bmx_sparkle_setAutomaticallyChecksForUpdates(SUUpdater * upd, BOOL value) {
	[upd setAutomaticallyChecksForUpdates:value];
}

BOOL bmx_sparkle_automaticallyChecksForUpdates(SUUpdater * upd) {
	return [upd automaticallyChecksForUpdates];
}

void bmx_sparkle_setUpdateCheckInterval(SUUpdater * upd, double value) {
	[upd setUpdateCheckInterval:value];
}

double bmx_sparkle_updateCheckInterval(SUUpdater * upd) {
	return [upd updateCheckInterval];
}

void bmx_sparkle_setFeedURL(SUUpdater * upd, BBString * url) {
	[upd setFeedURL:[NSURL URLWithString:NSStringFromBBString(url)]];
}

BBString * bmx_sparkle_feedURL(SUUpdater * upd) {
	return bbStringFromNSString([upd absoluteString]);
}

void bmx_sparkle_setSendsSystemProfile(SUUpdater * upd, BOOL value) {
	[upd setSendsSystemProfile:value];
}

BOOL bmx_sparkle_sendsSystemProfile(SUUpdater * upd) {
	return [upd sendsSystemProfile];
}

void bmx_sparkle_setAutomaticallyDownloadsUpdates(SUUpdater * upd, BOOL value) {
	[upd setAutomaticallyDownloadsUpdates:value];
}

BOOL bmx_sparkle_automaticallyDownloadsUpdates(SUUpdater * upd) {
	return [upd automaticallyDownloadsUpdates];
}

void bmx_sparkle_checkForUpdatesInBackground(SUUpdater * upd) {
	[upd checkForUpdatesInBackground];
}

void bmx_sparkle_checkForUpdateInformation(SUUpdater * upd) {
	[upd checkForUpdateInformation];
}

BOOL bmx_sparkle_updateInProgress(SUUpdater * upd) {
	return [upd updateInProgress];
}

double bmx_sparkle_lastUpdateCheckDate(SUUpdater * upd) {
	NSDate * date = [upd lastUpdateCheckDate];
	if (date) {
		return [date timeIntervalSince1970];	
	}
	return 0.0f;
}

