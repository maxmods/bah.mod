/*
  Copyright (c) 2009-2010 Bruce A Henderson
 
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
#import <ScriptingBridge/ScriptingBridge.h>


#include <blitz.h>

// a class to keep the array and enumerator in scope!
@class MaxArray;

extern BBObject * _bah_scriptingbridge_SBObject__create(SBObject * obj);
extern BBObject * _bah_scriptingbridge_SBElementArray__create(MaxArray * arr);

// --------------------------------------------------------


@interface MaxArray:NSObject{
	NSArray * array;
	NSEnumerator *enumerator;
}
-(id)initWithArray:(NSArray *)a;
-(SBObject *)nextObject;
-(BBArray *)propertyArrayAsString:(NSString *)name;
-(void)setPropertyArrayAsInt:(NSString *)name withValue:(int)value;
-(int)count;
-(NSString *)getDescription;
@end

@implementation MaxArray
-(id)initWithArray:(NSArray *)a{
	array = a;
	enumerator = [array objectEnumerator];
	return self;
}
-(SBObject *)nextObject {
	return [enumerator nextObject];
}
-(BBArray *)propertyArrayAsString:(NSString *)name {
	NSArray *stringArray = [array valueForKey:name];

	int n = [stringArray count];

	BBArray *p = bbArrayNew1D( "$", n);
	BBString **s = (BBString**)BBARRAYDATA(p, p->dims);
	
	NSEnumerator * ae = [stringArray objectEnumerator];
	NSString * text;
	int i = 0;
	while (text = [ae nextObject]) {
		s[i] = bbStringFromNSString(text);
		BBRETAIN( s[i] );
		i++;
	}
	return p;
}
-(void)setPropertyArrayAsInt:(NSString *)name withValue:(int)value {
	 [array setValue:[NSNumber numberWithInt:value] forKey:name];
}
-(int)count {
	return [array count];
}
-(NSString *)getDescription {
	return [array description];
}
@end

// --------------------------------------------------------

SBApplication * bmx_sb_sbapplication_applicationWithBundleIdentifier(BBString * ident) {
	return [SBApplication applicationWithBundleIdentifier:NSStringFromBBString(ident)];
}

SBApplication * bmx_sb_sbapplication_applicationWithProcessIdentifier(int pid) {
	return [SBApplication applicationWithProcessIdentifier:pid];
}

SBApplication * bmx_sb_sbapplication_applicationWithURL(BBString * url) {
	NSURL * s = [NSURL URLWithString:NSStringFromBBString(url)];
	return [SBApplication applicationWithURL:s];
}

int bmx_sb_sbapplication_isRunning(SBApplication * app) {
	return [app isRunning];
}

void bmx_sb_sbapplication_activate(SBApplication * app) {
	[app activate];
}

// --------------------------------------------------------

BBString * bmx_sb_sbobject_propertyAsString(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	NSString * s = [obj valueForKey:n];
	
	return bbStringFromNSString(s);
}

BBObject * bmx_sb_sbobject_propertyAsObject(SBObject * obj, BBString * name, BBString * predicate) {
	NSObject * o;

	NSString * n = NSStringFromBBString(name);
	o = [[obj valueForKey:n] get];
	
	if ([o isKindOfClass:[SBObject class]]) {

		return _bah_scriptingbridge_SBObject__create((SBObject *)o);

	} else if ([o isKindOfClass:[NSArray class]]) {
		NSPredicate *pred = NULL;
		
		if (predicate != &bbEmptyString) {
			NSString * p = NSStringFromBBString(predicate);
			pred = [NSPredicate predicateWithFormat:p];
		}
	
		if (pred) {
			return _bah_scriptingbridge_SBElementArray__create([[MaxArray alloc] initWithArray:[(NSArray*)o filteredArrayUsingPredicate:pred]]);
		} else {
			return _bah_scriptingbridge_SBElementArray__create([[MaxArray alloc] initWithArray:(NSArray*)o]);
		}
	
	} else {
/*
		BBString * s = bbStringFromNSString([o description]);
		char * c = bbStringToCString(s);
		printf("Warning! Property is of an unknown type. : \n%s\n", c);fflush(stdout);
		bbMemFree(c);
*/
		return _bah_scriptingbridge_SBObject__create([obj valueForKey:n]);
	}
	
}

int bmx_sb_sbobject_propertyAsInt(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	NSNumber * value = [obj valueForKey:n];
	return [value intValue];
}

double bmx_sb_sbobject_propertyAsDouble(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	NSNumber * value = [obj valueForKey:n];
	return [value doubleValue];
}

const void * bmx_sb_sbobject_propertyAsPtr(SBObject * obj, BBString * name, int * length) {
	NSString * n = NSStringFromBBString(name);
	NSObject * o = [obj valueForKey:n];
	
	if ([o isKindOfClass:[NSData class]]) {

		NSData * value = (NSData*)o;
		*length = (int)[value length];
		return [value bytes];

	} else if ([o isKindOfClass:[SBObject class]]) {

		NSData * value = (NSData*)[[o get] data]; //you have to ask the "NSAppleEventDescriptor" for the data

		*length = (int)[value length];
		return [value bytes];

	}

	return NULL;	
}

double bmx_sb_sbobject_propertyAsDate(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	NSDate * d = [obj valueForKey:n];
	return [d timeIntervalSince1970];
}

MaxArray * bmx_sb_sbobject_propertyAsList(SBObject * obj, BBString * name, BBString * predicate, int immediate) {
	NSString * n = NSStringFromBBString(name);

	NSPredicate *pred = NULL;
	
	if (predicate != &bbEmptyString) {
		NSString * p = NSStringFromBBString(predicate);
		pred = [NSPredicate predicateWithFormat:p];
	}
	
	SBElementArray * array = [obj valueForKey:n];
	if (immediate) {
		array = [array get];
	}
	
	if (pred) {
		return [[MaxArray alloc] initWithArray:[array filteredArrayUsingPredicate:pred]];
	} else {
		return [[MaxArray alloc] initWithArray:array];
	}
}

void bmx_sb_sbobject_setPropertyAsInt(SBObject * obj, BBString * name, int value) {
	NSString * n = NSStringFromBBString(name);
	[obj setValue:[NSNumber numberWithInt:value] forKey:n];
}

void bmx_sb_sbobject_setPropertyAsString(SBObject * obj, BBString * name, BBString * value) {
	NSString * n = NSStringFromBBString(name);
	[obj setValue:NSStringFromBBString(value) forKey:n];
}

void bmx_sb_sbobject_call(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	SEL selector = NSSelectorFromString(n);
	[obj performSelector:selector];
}

void bmx_sb_sbobject_callWithInt(SBObject * obj, BBString * name, int value) {
	NSString * n = NSStringFromBBString(name);
	SEL selector = NSSelectorFromString(n);
	[obj performSelector:selector withObject:[NSNumber numberWithInt:value]];
}

BBObject * bmx_sb_sbobject_callWithStringIntReturningObject(SBObject * obj, BBString * name, BBString * value1, int value2) {
	NSString * n = NSStringFromBBString(name);
	NSString * v = NSStringFromBBString(value1);
	
	SEL selector = NSSelectorFromString(n);
	
	NSObject * o = [obj performSelector:selector withObject:v withObject:[NSNumber numberWithInt:value2]];

	if ([o isKindOfClass:[NSArray class]]) {
		return _bah_scriptingbridge_SBElementArray__create([[MaxArray alloc] initWithArray:(NSArray*)o]);
	} else {
		return _bah_scriptingbridge_SBObject__create((SBObject *)o);
	}
}

int bmx_sb_sbobject_callReturningInt(SBObject * obj, BBString * name) {
	NSString * n = NSStringFromBBString(name);
	SEL selector = NSSelectorFromString(n);
	return [obj performSelector:selector];
}

BBString * bmx_sb_sbobject_description(SBObject * obj) {
	NSString * s = [obj description];
	return bbStringFromNSString(s);
}

BBString * bmx_sb_sbobject_propertyAsURL(SBObject * obj, BBString * name, int kind) {
	NSString * n = NSStringFromBBString(name);
	NSURL * u = [obj valueForKey:n];
	
	if (kind == 1) {
		return bbStringFromNSString([u absoluteString]);
	} else {
		return bbStringFromNSString([u path]);
	}
}

// --------------------------------------------------------

void bmx_sb_sbelementarray_free(MaxArray * array) {
	[array release];
}

int bmx_sb_sbelementarray_count(MaxArray * array) {
	return [array count];
}

BBArray * bmx_sb_sbelementarray_propertyArrayAsString(MaxArray * array, BBString * name) {
	[array propertyArrayAsString:NSStringFromBBString(name)];
}

void bmx_sb_sbelementarray_setPropertyArrayAsInt(MaxArray * array, BBString * name, int value) {
	[array setPropertyArrayAsInt:NSStringFromBBString(name) withValue:value];
}

BBString * bmx_sb_sbelementarray_description(MaxArray * array) {
	return bbStringFromNSString([array getDescription]);
}

// --------------------------------------------------------

SBObject * bmx_sb_sbobjectenum_nextObject(MaxArray * array) {
	return [array nextObject];
}


