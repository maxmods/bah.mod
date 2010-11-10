' Copyright (c) 2009-2010 Bruce A Henderson
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
bbdoc: Scripting Bridge
about: Communicate with scriptable applications.
End Rem
Module BaH.ScriptingBridge

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2009-2010 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: propertyAsObject() now returns Object. This can be either an SBObject or SBElementArray."
ModuleInfo "History: Added setPropertyAsString(), propertyAsURL() and callWithStringIntReturningObject() methods."
ModuleInfo "History: Added description() method. Useful for debugging."
ModuleInfo "History: Changed to use new 1.38 string functions."
ModuleInfo "History: Added predicate parameter to propertyAsObject and propertyAsList methods."
ModuleInfo "History: Added immediate parameter to propertyAsList method."
ModuleInfo "History: 1.00 Initial Release"



?macos

Import "common.bmx"


Rem
bbdoc: Declares methods that can be invoked on any object in a scriptable application.
about: It defines methods for getting elements and properties of an object, as well as setting a given object to a new value.
<p>
Each SBObject is built around an object specifier, which tells Scripting Bridge how to locate the object. Therefore, you can think of an
SBObject as a reference to an object in an target application rather than an object itself.
</p>
<p>
Typically, rather than create SBObject instances explictly, you receive SBObject objects by calling methods of an SBApplication.
For example, if you wanted to get an SBObject representing the current iTunes track, you would use code like this (where iTunesTrack is an SBObject):
<pre>
Local itunes:SBApplication = SBApplication.applicationWithBundleIdentifier("com.apple.iTunes")

Local iTunesTrack:SBObject = itunes.propertyAsObject("currentTrack")
</pre>
You can discover the names of dynamically generated classes such as iTunesApplication and iTunesTrack by examining the header file created by the sdp tool.
</p>
End Rem
Type SBObject

	Field objectPtr:Byte Ptr

	Function _create:SBObject(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:SBObject = New SBObject
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Verifies if an object exists.
	End Rem
	Method exists:Int()
		Return callReturningInt("exists")
	End Method
	
	Rem
	bbdoc: Returns the named property as a String.
	End Rem
	Method propertyAsString:String(name:String)
		Return bmx_sb_sbobject_propertyAsString(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Returns the named property as an integer.
	End Rem
	Method propertyAsInt:Int(name:String)
		Return bmx_sb_sbobject_propertyAsInt(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Returns the named property as a Double.
	End Rem
	Method propertyAsDouble:Double(name:String)
		Return bmx_sb_sbobject_propertyAsDouble(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Returns the named property as an SBObject.
	End Rem
	Method propertyAsObject:Object(name:String, predicate:String = Null)
		If predicate Then
			Return bmx_sb_sbobject_propertyAsObject(objectPtr, name, predicate)
		Else
			Return bmx_sb_sbobject_propertyAsObject(objectPtr, name, Null)
		End If
	End Method

	Rem
	bbdoc: Returns the named property as a pointer, and the size of the data into @length.
	End Rem
	Method propertyAsPtr:Byte Ptr(name:String, length:Int Var)
		Return bmx_sb_sbobject_propertyAsPtr(objectPtr, name, Varptr length)
	End Method

	Rem
	bbdoc: Returns the named property as an SBElementArray.
	End Rem
	Method propertyAsList:SBElementArray(name:String, predicate:String = Null, immediate:Int = True)
		If predicate Then
			Return SBElementArray._create(bmx_sb_sbobject_propertyAsList(objectPtr, name, predicate, immediate))
		Else
			Return SBElementArray._create(bmx_sb_sbobject_propertyAsList(objectPtr, name, Null, immediate))
		End If
	End Method

	Rem
	bbdoc: Returns the named NSDate property as a double.
	about: The value returned is the number of seconds since 1 January 1970.
	End Rem
	Method propertyAsDate:Double(name:String)
		Return bmx_sb_sbobject_propertyAsDate(objectPtr, name)
	End Method

	Rem
	bbdoc: Returns the named NSURL property as a String.
	about: @kind may be one of #URL_PATH or #URL_ABSOLUTE.
	End Rem
	Method propertyAsURL:String(name:String, kind:Int = URL_PATH)
		Return bmx_sb_sbobject_propertyAsURL(objectPtr, name, kind)
	End Method

	Rem
	bbdoc: Sets the named property as an integer.
	End Rem
	Method setPropertyAsInt(name:String, value:Int)
		bmx_sb_sbobject_setPropertyAsInt(objectPtr, name, value)
	End Method
	
	Rem
	bbdoc: Sets the named property as a String.
	End Rem
	Method setPropertyAsString(name:String, value:String)
		bmx_sb_sbobject_setPropertyAsString(objectPtr, name, value)
	End Method
	
	Rem
	bbdoc: Calls the method @name, without parameters.
	End Rem
	Method call(name:String)
		bmx_sb_sbobject_call(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Calls the method @name, passing an Int parameter.
	End Rem
	Method callWithInt(name:String, value:Int)
		bmx_sb_sbobject_callWithInt(objectPtr, name, value)
	End Method
	
	Rem
	bbdoc: Calls the method @name, passing String and Int parameters, returning an Object.
	about: Note, the method @name should include the full signature. For examle "searchFor:only:"
	End Rem
	Method callWithStringIntReturningObject:Object(name:String, value1:String, value2:Int)
		Return bmx_sb_sbobject_callWithStringIntReturningObject(objectPtr, name, value1, value2)
	End Method
	
	Rem
	bbdoc: Calls the method @name, returning an Int result.
	End Rem
	Method callReturningInt:Int(name:String)
		Return bmx_sb_sbobject_callReturningInt(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Returns a description of the object, suitable for debugging.
	End Rem
	Method description:String()
		Return bmx_sb_sbobject_description(objectPtr)
	End Method
	
End Type


Rem
bbdoc: Provides a mechanism enabling a program to send Apple events to a scriptable application and receive Apple events in response.
about: It thereby makes it possible for that program to control the application and exchange data with it. Scripting Bridge works by bridging data
types between Apple event descriptors and Cocoa objects.
End Rem
Type SBApplication Extends SBObject

	Function _create:SBApplication(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:SBApplication = New SBApplication
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Method Delete()
		If objectPtr Then
			bmx_sb_sbapplication_free(objectPtr)
			objectPtr = Null
		End If
	End Method

	Rem
	bbdoc: Returns the shared instance representing the target application specified by its bundle identifier.
	returns: An instance of an SBApplication that represents the target application whose bundle identifier is @ident. Returns Null if no such application can be found or if the application does not have a scripting interface.
	about: For applications that declare themselves to have a dynamic scripting interface, this method will launch the
	application if it is not already running.
	End Rem
	Function applicationWithBundleIdentifier:SBApplication(ident:String)
		Return SBApplication._create(bmx_sb_sbapplication_applicationWithBundleIdentifier(ident))
	End Function
	
	Rem
	bbdoc: Returns the shared instance representing a target application specified by its process identifier.
	about: You should avoid using this function unless you know nothing about a target application but its process ID. In most cases, it
	is better to use classForApplicationWithBundleIdentifier(), which will dynamically locate the application's path at runtime, or
	classForApplicationWithURL(), which is not dependent on the target application being open at the time the method is called.
	End Rem
	Function applicationWithProcessIdentifier:SBApplication(pid:Int)
		Return SBApplication._create(bmx_sb_sbapplication_applicationWithProcessIdentifier(pid))
	End Function
	
	Rem
	bbdoc: Returns the shared instance representing a target application specified by the given URL.
	about: For applications that declare themselves to have a dynamic scripting interface, this method will launch the application if it is not already
	running. This approach to initializing SBApplication objects should be used only if you know for certain the URL of the target application. In
	most cases, it is better to use classForApplicationWithBundleIdentifier() which dynamically locates the target application at runtime.
	<p>
	This method currently supports file URLs (file:) and remote application URLs (eppc:). It checks whether a file exists at the specified path,
	but it does not check whether an application identified via eppc: exists.
	</p>
	End Rem
	Function applicationWithURL:SBApplication(url:String)
		Return SBApplication._create(bmx_sb_sbapplication_applicationWithURL(url))
	End Function

	Rem
	bbdoc: Returns whether the target application represented by the receiver is running.
	returns: True if the application is running, False otherwise.
	End Rem
	Method isRunning:Int()
		Return bmx_sb_sbapplication_isRunning(objectPtr)
	End Method
	
	Rem
	bbdoc: Moves the target application to the foreground immediately.
	about: If the target application is not already running, this method launches it.
	End Rem
	Method activate()
		bmx_sb_sbapplication_activate(objectPtr)
	End Method
	
End Type

Rem
bbdoc: An array that manages collections of related SBObject objects.
about: For example, when you ask the Finder for a list of disks, or ask iTunes for a list of playlists, you get the result back as an SBElementArray
containing Scripting Bridge objects representing those items.
End Rem
Type SBElementArray

	Field objectPtr:Byte Ptr

	Function _create:SBElementArray(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:SBElementArray = New SBElementArray
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method ObjectEnumerator:SBObjectEnum()
		Local enum:SBObjectEnum = New SBObjectEnum
		enum.obj = SBObject._create(bmx_sb_sbobjectenum_nextObject(objectPtr))
		enum.objectPtr = objectPtr
		Return enum
	End Method
	
	Rem
	bbdoc: Returns the count of elements in the array.
	End Rem
	Method count:Int()
		Return bmx_sb_sbelementarray_count(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns an array of String properties for the specified property @name.
	End Rem
	Method propertyArrayAsString:String[](name:String)
		Return bmx_sb_sbelementarray_propertyArrayAsString(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Sets all array item properties of the specified @name, with @value.
	End Rem
	Method setPropertyArrayAsInt(name:String, value:Int)
		bmx_sb_sbelementarray_setPropertyArrayAsInt(objectPtr, name, value)
	End Method

	Rem
	bbdoc: Returns a description of the array object, suitable for debugging.
	End Rem
	Method description:String()
		Return bmx_sb_sbelementarray_description(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_sb_sbelementarray_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type

' internal
Type SBObjectEnum

	Field objectPtr:Byte Ptr
	Field obj:SBObject
	
	Function _create:SBObjectEnum(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:SBObjectEnum = New SBObjectEnum
			this.objectPtr = objectPtr
			
			this.obj = SBObject._create(bmx_sb_sbobjectenum_nextObject(objectPtr))
			
			Return this
		End If
	End Function

	Method HasNext:Int()
		If obj Then
			Return True
		End If
	End Method

	Method NextObject:Object()
		Local nobj:SBObject = obj
		obj = SBObject._create(bmx_sb_sbobjectenum_nextObject(objectPtr))
		Return nobj
	End Method

End Type

Rem
bbdoc: Auto Release pool
about: Required when using scripting bridge inside a thread.
Create a new NSAutoReleasePool at the beginning of the thread function, and Free() it at the end of the function.
End Rem
Type NSAutoReleasePool
	Field poolPtr:Byte Ptr
	
	Method New()
		poolPtr = bmx_sb_nsautoreleasepool_new()
	End Method
	
	Rem
	bbdoc: Releases the pool.
	End Rem
	Method Free()
		If poolPtr Then
			bmx_sb_nsautoreleasepool_free(poolPtr)
			poolPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

?
