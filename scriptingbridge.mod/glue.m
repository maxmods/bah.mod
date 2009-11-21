/*
  Copyright (c) 2009 Bruce A Henderson
 
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

@interface MaxArray:NSObject{
	SBElementArray * array;
	NSEnumerator *enumerator;
}
-(id)initWithArray:(SBElementArray *)a;
-(SBObject *)nextObject;
@end

@implementation MaxArray
-(id)initWithArray:(SBElementArray *)a{
	array = a;
	enumerator = [array objectEnumerator];
	return self;
}
-(SBObject *)nextObject {
	return [enumerator nextObject];
}
@end

// string conversion
BBString *bbStringFromNSString(NSString *s){
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
NSString *bbStringToNSString(BBString *s){
	return [NSString stringWithCharacters:s->buf length:s->length];
}


SBApplication * bmx_sb_sbapplication_applicationWithBundleIdentifier(BBString * ident) {
	return [SBApplication applicationWithBundleIdentifier:bbStringToNSString(ident)];
}

BBString * bmx_sb_sbobject_propertyAsString(SBObject * obj, BBString * name) {
	NSString * n = bbStringToNSString(name);
	NSString * s = [obj valueForKey:n];
	
	return bbStringFromNSString(s);
}

SBObject * bmx_sb_sbobject_propertyAsObject(SBObject * obj, BBString * name) {
	NSString * n = bbStringToNSString(name);
	return [obj valueForKey:n];
}

int bmx_sb_sbobject_propertyAsInt(SBObject * obj, BBString * name) {
	NSString * n = bbStringToNSString(name);
	NSNumber * value = [obj valueForKey:n];
	return [value intValue];
}

double bmx_sb_sbobject_propertyAsDouble(SBObject * obj, BBString * name) {
	NSString * n = bbStringToNSString(name);
	NSNumber * value = [obj valueForKey:n];
	return [value doubleValue];
}

MaxArray * bmx_sb_sbobject_propertyAsList(SBObject * obj, BBString * name) {
	NSString * n = bbStringToNSString(name);
	
	SBElementArray * array = [obj valueForKey:n];
	return [[MaxArray alloc] initWithArray:array];
}

void bmx_sb_sbelementarray_free(MaxArray * array) {
	[array release];
}

SBObject * bmx_sb_sbobjectenum_nextObject(MaxArray * array) {
	return [array nextObject];
}

