' Copyright (c) 2009 Bruce A Henderson
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

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

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
	Method propertyAsObject:SBObject(name:String)
		Return SBObject._create(bmx_sb_sbobject_propertyAsObject(objectPtr, name))
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
	Method propertyAsList:SBElementArray(name:String)
		Return SBElementArray._create(bmx_sb_sbobject_propertyAsList(objectPtr, name))
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

	Rem
	bbdoc: Returns the shared instance representing the target application specified by its bundle identifier.
	returns: An instance of an SBApplication that represents the target application whose bundle identifier is @ident. Returns Null if no such application can be found or if the application does not have a scripting interface.
	about: For applications that declare themselves to have a dynamic scripting interface, this method will launch the
	application if it is not already running.
	End Rem
	Function applicationWithBundleIdentifier:SBApplication(ident:String)
		Return SBApplication._create(bmx_sb_sbapplication_applicationWithBundleIdentifier(ident))
	End Function

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



?
