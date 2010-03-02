' Copyright 2008-2010 Bruce A Henderson
'
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
'
'     http://www.apache.org/licenses/LICENSE-2.0
'
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
'
SuperStrict

Rem
bbdoc: Protocol Buffers
End Rem
Module BaH.Protobuf

ModuleInfo "Version: 1.00"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "Copyright: Wrapper - 2008-2010 Bruce A Henderson"
ModuleInfo "Copyright: Protobuf library - 2008 Google Inc."

Import BRL.Stream
Import BRL.MaxUtil

Import "common.bmx"


Rem
bbdoc: 
End Rem
Global ProtobufCLI:TCommandLineInterface = New TCommandLineInterface


Rem
bbdoc: 
End Rem
Type TCommandLineInterface

	Field cliPtr:Byte Ptr

	Method New()
		cliPtr = bmx_commandlineinterface_create()
		
		putenv_("MODPATH=" + ModulePath(""))
	End Method


	Method Delete()
		If cliPtr Then
			bmx_commandlineinterface_delete(cliPtr)
			cliPtr = Null
		End If
	End Method
	
	Method Run:Int(argv:String[])
		Return bmx_commandlineinterface_run(cliPtr, AppArgs + argv)
	End Method
	
End Type

Rem
bbdoc: Interface for protocol messages.
End Rem
Type TMessage

	Field messagePtr:Byte Ptr
	
	Field owner:Int
	
	Rem
	bbdoc: Get a Descriptor for this message's type.
	about: This describes what fields the message contains, the types of those fields, etc.
	End Rem
	Method GetDescriptor:TDescriptor() Abstract
	
	Rem
	bbdoc: Get the Reflection interface for this Message, which can be used to read and modify the fields of the Message dynamically (in other words, without knowing the message type at compile time).
	End Rem
	Method GetReflection:TReflection() Abstract
	
	Rem
	bbdoc: Clear all fields of the message and set them to their default values.
	about: Clear() avoids freeing memory, assuming that any memory allocated to hold parts of the message will be needed
	again to hold the next message. If you actually want to free the memory used by a Message, you must Free() it.
	End Rem
	Method Clear()
		bmx_pb_message_clear(messagePtr)
	End Method

	Rem
	bbdoc: Verifies that IsInitialized() returns true.
	End Rem
	Method CheckInitialized()
		bmx_pb_message_checkinitialized(messagePtr)
	End Method

	Rem
	bbdoc: Quickly check if all required fields have values set.
	End Rem
	Method IsInitialized:Int()
		Return bmx_pb_message_isinitialized(messagePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method InitializationErrorString:String()
		Return bmx_pb_message_initializationerrorstring(messagePtr)
	End Method

	Rem
	bbdoc: Clears all unknown fields from this message and all embedded messages.
	about: Normally, if unknown tag numbers are encountered when parsing a message, the tag and value are stored in the
	message's UnknownFieldSet and then written back out when the message is serialized. This allows servers which simply
	route messages to other servers to pass through messages that have new field definitions which they don't yet know
	about. However, this behavior can have security implications. To avoid it, call this method after parsing.
	End Rem
	Method DiscardUnknownFields()
		bmx_pb_message_discardunknownfields(messagePtr)
	End Method
	
	Rem
	bbdoc: Parse a protocol buffer contained in a string.
	End Rem
	Method ParseFromString:Int(data:String)
		Return bmx_pb_message_parsefromstring(messagePtr, data)
	End Method
	
	Rem
	bbdoc: Parse a protocol buffer contained in an array of bytes.
	End Rem
	Method ParseFromBytes:Int(data:Byte Ptr, size:Int)
		Return bmx_pb_message_parsefrombytes(messagePtr, data, size)
	End Method
	
	Rem
	bbdoc: Parse a protocol buffer contained in a file or stream.
	End Rem
	Method ParseFromFile:Int(file:Object)
	
		Local owner:Int
		Local stream:TStream = TStream(file)
		
		' not a stream?
		If Not stream Then

			' maybe a filename!
			If String(file) Then
			
				stream = OpenStream(file, True, False)
				owner = True
				
			End If
		
		End If
	
		' process the stream now?
		If stream Then
		
			Local inputStream:TCopyingInputStream = TCopyingInputStream.Create(stream)
			
			Local ret:Int = bmx_pb_message_parsefromzerocopystream(messagePtr, inputStream.streamPtr)
			
			If owner Then
				CloseStream(stream)
			End If
		
			Return ret
		End If

		Return False
	End Method
	
	Rem
	bbdoc: Serialize the message into a string.
	about: All required fields must be set.
	End Rem
	Method SerializeToString:String()
		Return bmx_pb_message_serializetostring(messagePtr)
	End Method
	
	Rem
	bbdoc: Serialize the message and store it in the given byte array.
	about: All required fields must be set.
	End Rem
	Method SerializeToBytes:Int(data:Byte Ptr, size:Int)
		Return bmx_pb_message_serializetobytes(messagePtr, data, size)
	End Method
	
	Rem
	bbdoc: Serialize the message to a file or stream.
	about: All required fields must be set.
	End Rem
	Method SerializeToFile:Int(file:Object)
	
		Local owner:Int
		Local stream:TStream = TStream(file)
		
		' not a stream?
		If Not stream Then

			' maybe a filename!
			If String(file) Then
			
				stream = OpenStream(file, False, True)
				owner = True
				
			End If
		
		End If
	
		' write to the stream now?
		If stream Then
		
			Local outputStream:TCopyingOutputStream = TCopyingOutputStream.Create(stream)
			
			Local ret:Int = bmx_pb_message_serializetozerocopystream(messagePtr, outputStream.streamPtr)
			
			If owner Then
				CloseStream(stream)
			End If
		
			Return ret
			
		End If
		
		Return False
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If messagePtr And owner Then
			bmx_pb_message_delete(messagePtr)
			messagePtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type


Rem
bbdoc: Implements protocol buffer text format.
bbdoc: Printing and parsing protocol messages in text format is useful for debugging and human editing of messages.
End Rem
Type TTextFormat

	Rem
	bbdoc: 
	End Rem
	Function PrintToString:String(message:TMessage)
		Return bmx_textformat_printtostring(message.messagePtr)
	End Function

End Type

Rem
bbdoc: Describes a type of protocol message, or a particular group within a message.
about: To obtain the Descriptor for a given message object, call Message::GetDescriptor(). Generated message classes
also have a static method called descriptor() which returns the type's descriptor. Use DescriptorPool to construct
your own descriptors.
End Rem
Type TDescriptor

	Field descriptorPtr:Byte Ptr
	
	Function _create:TDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TDescriptor = New TDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The name of the message type, not including its scope.
	End Rem
	Method GetName:String()
		Return bmx_pb_descriptor_getname(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The fully-qualified name of the message type, scope delimited by periods.
	about: For example, message type "Foo" which is declared in package "bar" has full name "bar.Foo". If a type "Baz"
	is nested within Foo, Baz's full_name is "bar.Foo.Baz". To get only the part that comes after the last '.',
	use GetName().
	End Rem
	Method GetFullName:String()
		Return bmx_pb_descriptor_getfullname(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Index of this descriptor within the file or containing type's message type array.
	End Rem
	Method GetIndex:Int()
		Return bmx_pb_descriptor_getindex(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The .proto file in which this message type was defined. Never NULL.
	End Rem
	Method GetFile:TFileDescriptor()
		Return TFileDescriptor._create(bmx_pb_descriptor_getfile(descriptorPtr))
	End Method
	
	Rem
	bbdoc: If this Descriptor describes a nested type, this returns the type in which it is nested.
	about: Otherwise, returns NULL.
	End Rem
	Method GetContainingType:TDescriptor()
		Return TDescriptor._create(bmx_pb_descriptor_getcontainingtype(descriptorPtr))
	End Method
	
	'Method GetOptions:TMessageOptions()
	'End Method
	
	Rem
	bbdoc: Write the contents of this decriptor in a human-readable form.
	about: Output will be suitable for re-parsing.
	End Rem
	Method DebugString:String()
		Return bmx_pb_descriptor_debugstring(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The number of fields in this message type.
	End Rem
	Method FieldCount:Int()
		Return bmx_pb_descriptor_fieldcount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets a field by index, where 0 <= index < FieldCount().
	End Rem
	Method GetField:TFieldDescriptor(index:Int)
		Return TFieldDescriptor._create(bmx_pb_descriptor_getfield(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Looks up a field by declared tag number.
	returns: Null if no such field exists.
	End Rem
	Method FindFieldByNumber:TFieldDescriptor(number:Int)
		Return TFieldDescriptor._create(bmx_pb_descriptor_findfieldbynumber(descriptorPtr, number))
	End Method
	
	Rem
	bbdoc: Looks up a nested type by name.
	returns: Null if no such nested type exists.
	End Rem
	Method FindFieldByName:TFieldDescriptor(name:String)
		Return TFieldDescriptor._create(bmx_pb_descriptor_findfieldbyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: The number of nested types in this message type. 
	End Rem
	Method NestedTypeCount:Int()
		Return bmx_pb_descriptor_nestedtypecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets a nested type by index, where 0 <= index < nested_type_count(). 
	End Rem
	Method GetNestedType:TDescriptor(index:Int)
		Return TDescriptor._create(bmx_pb_descriptor_getnestedtype(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Looks up a nested type by name.
	returns: Null if no such nested type exists.
	End Rem
	Method FindNestedTypeByName:TDescriptor(name:String)
		Return TDescriptor._create(bmx_pb_descriptor_findnestedtypebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: The number of enum types in this message type. 
	End Rem
	Method EnumTypeCount:Int()
		Return bmx_pb_descriptor_enumtypecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets an enum type by index, where 0 <= index < enum_type_count(). 
	End Rem
	Method GetEnumType:TEnumDescriptor(index:Int)
		Return TEnumDescriptor._create(bmx_pb_descriptor_getenumtype(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Looks up an enum type by name.
	returns: Null if no such enum type exists. 
	End Rem
	Method FindEnumTypeByName:TEnumDescriptor(name:String)
		Return TEnumDescriptor._create(bmx_pb_descriptor_findenumtypebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: Looks up an enum value by name, among all enum types in this message.
	returns: Null if no such value exists.
	End Rem
	Method FindEnumValueByName:TEnumValueDescriptor(name:String)
		Return TEnumValueDescriptor._create(bmx_pb_descriptor_findenumvaluebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: The number of extension ranges in this message type. 
	End Rem
	Method ExtensionRangeCount:Int()
		Return bmx_pb_descriptor_extensionrangecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets an extension range by index, where 0 <= index < extension_range_count(). 
	End Rem
	Method GetExtensionRange:TExtensionRange(index:Int)
		Return TExtensionRange._create(bmx_pb_descriptor_getextensionrange(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Returns true if the number is in one of the extension ranges. 
	End Rem
	Method IsExtensionNumber:Int(number:Int)
		Return bmx_pb_descriptor_isextensionnumber(descriptorPtr, number)
	End Method
	
	Rem
	bbdoc: The number of extensions -- extending *other* messages -- that were defined nested within this message type's scope. 
	End Rem
	Method ExtensionCount:Int()
		Return bmx_pb_descriptor_extensioncount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Get an extension by index, where 0 <= index < extension_count(). 
	End Rem
	Method GetExtension:TFieldDescriptor(index:Int)
		Return TFieldDescriptor._create(bmx_pb_descriptor_getextension(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Looks up a named extension (which extends some *other* message type) defined within this message type's scope. 
	End Rem
	Method FindExtensionByName:TFieldDescriptor(name:String)
		Return TFieldDescriptor._create(bmx_pb_descriptor_findextensionbyname(descriptorPtr, name))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TReflection

	Field reflectionPtr:Byte Ptr
	
	Function _create:TReflection(reflectionPtr:Byte Ptr)
		If reflectionPtr Then
			Local this:TReflection = New TReflection
			this.reflectionPtr = reflectionPtr
			Return this
		End If
	End Function

	Method HasField:Int(_field:TFieldDescriptor)
	End Method
	
	Method FieldSize:Int(_field:TFieldDescriptor)
	End Method
	
	Method ClearField(_field:TFieldDescriptor)
	End Method
	
End Type

Rem
bbdoc: Describes a whole .proto file.
about: To get the FileDescriptor for a compiled-in file, get the descriptor for something defined in that file and call
descriptor.GetFile().
End Rem
Type TFileDescriptor

	Field descriptorPtr:Byte Ptr

	Function _create:TFileDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TFileDescriptor = New TFileDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The filename, relative to the source tree.
	about: e.g. "google/protobuf/descriptor.proto"
	End Rem
	Method GetName:String()
		Return bmx_pb_filedescriptor_getname(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The package, e.g. "proto2.compiler".
	End Rem
	Method GetPackage:String()
		Return bmx_pb_filedescriptor_getpackage(descriptorPtr)
	End Method
	
	Rem
	bbdoc: The TDescriptorPool in which this TFileDescriptor and all its contents were allocated.
	about: Never NULL.
	End Rem
	Method GetPool:TDescriptorPool()
		Return TDescriptorPool._create(bmx_pb_filedescriptor_getpool(descriptorPtr))
	End Method
	
	Rem
	bbdoc: The number of files imported by this one.
	End Rem
	Method DependencyCount:Int()
		Return bmx_pb_filedescriptor_dependencycount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets an imported file by index, where 0 <= index < dependency_count().
	End Rem
	Method GetDependency:TFileDescriptor(index:Int)
		Return TFileDescriptor._create(bmx_pb_filedescriptor_getdependency(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Number of top-level message types defined in this file.
	about: This does not include nested types.
	End Rem
	Method MessageTypeCount:Int()
		Return bmx_pb_filedescriptor_messagetypecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets a top-level message type, where 0 <= index < message_type_count().
	End Rem
	Method GetMessageType:TDescriptor(index:Int)
		Return TDescriptor._create(bmx_pb_filedescriptor_getmessagetype(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Number of top-level enum types defined in this file.
	about: This does not include nested types.
	End Rem
	Method EnumTypeCount:Int()
		Return bmx_pb_filedescriptor_enumtypecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets a top-level enum type, where 0 <= index < enum_type_count().
	End Rem
	Method GetEnumType:TEnumDescriptor(index:Int)
		Return TEnumDescriptor._create(bmx_pb_filedescriptor_getenumtype(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Number of services defined in this file.
	End Rem
	Method ServiceCount:Int()
		Return bmx_pb_filedescriptor_servicecount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets a service, where 0 <= index < service_count().
	End Rem
	Method GetService:TServiceDescriptor(index:Int)
		Return TServiceDescriptor._create(bmx_pb_filedescriptor_getservice(descriptorPtr, index))
	End Method
	
	Rem
	bbdoc: Number of extensions defined at file scope.
	about: This does not include extensions nested within message types.
	End Rem
	Method ExtensionCount:Int()
		Return bmx_pb_filedescriptor_extensioncount(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Gets an extension's descriptor, where 0 <= index < extension_count().
	End Rem
	Method GetExtension:TFieldDescriptor(index:Int)
		Return TFieldDescriptor._create(bmx_pb_filedescriptor_getextension(descriptorPtr, index))
	End Method
	
	Method GetOptions:TFileOptions()
'		Return TFileOptions._create(bmx_pb_filedescriptor_getoptions(descriptorPtr))
	End Method
	
	Rem
	bbdoc: Find a top-level message type by name.
	returns: Null if not found.
	End Rem
	Method FindMessageTypeByName:TDescriptor(name:String)
		Return TDescriptor._create(bmx_pb_filedescriptor_findmessagetypebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: Find a top-level enum type by name.
	returns: Null if not found.
	End Rem
	Method FindEnumTypeByName:TEnumDescriptor(name:String)
		Return TEnumDescriptor._create(bmx_pb_filedescriptor_findenumtypebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: Find an enum value defined in any top-level enum by name.
	returns: Null if not found.
	End Rem
	Method FindEnumValueByName:TEnumValueDescriptor(name:String)
		Return TEnumValueDescriptor._create(bmx_pb_filedescriptor_findenumvaluebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: Find a service definition by name.
	returns: Null if not found.
	End Rem
	Method FindServiceByName:TServiceDescriptor(name:String)
		Return TServiceDescriptor._create(bmx_pb_filedescriptor_findservicebyname(descriptorPtr, name))
	End Method
	
	Rem
	bbdoc: Find a top-level extension definition by name.
	returns: Null if not found.
	End Rem
	Method FindExtensionByName:TFieldDescriptor(name:String)
		Return TFieldDescriptor._create(bmx_pb_filedescriptor_findextensionbyname(descriptorPtr, name))
	End Method
	
	Method CopyTo(proto:TFileDescriptorProto)
'		bmx_pb_filedescriptor_copyto(descriptorPtr, proto.protoPtr)
	End Method
	
	Rem
	bbdoc: Write the contents of this decriptor in a human-readable form.
	about: Output will be suitable for re-parsing.
	End Rem
	Method DebugString:String()
		Return bmx_pb_filedescriptor_debugstring(descriptorPtr)
	End Method
	
End Type

Rem
bbdoc: Describes a single field of a message.
about: To get the descriptor for a given field, first get the Descriptor for the message in which it is defined, then
call TDescriptor.FindFieldByName(). To get a FieldDescriptor for an extension, do one of the following:
<ul>
<li>Get the TDescriptor or TFileDescriptor for its containing scope, then call TDescriptor.FindExtensionByName() or
TFileDescriptor::FindExtensionByName().</li>
<li>Given a TDescriptorPool, call TDescriptorPool.FindExtensionByNumber().</li>
<li>Given a TReflection for a message object, call TReflection.FindKnownExtensionByName() or
TReflection::FindKnownExtensionByNumber(). Use TDescriptorPool to construct your own descriptors.</li>
</ul>
End Rem
Type TFieldDescriptor

	Field descriptorPtr:Byte Ptr

	Const TYPE_DOUBLE:Int = 1
	Const TYPE_FLOAT:Int = 2
	Const TYPE_INT64:Int = 3
	Const TYPE_UINT64:Int = 4
	Const TYPE_INT32:Int = 5
	Const TYPE_FIXED64:Int = 6
	Const TYPE_FIXED32:Int = 7
	Const TYPE_BOOL:Int = 8
	Const TYPE_STRING:Int = 9
	Const TYPE_GROUP:Int = 10
	Const TYPE_MESSAGE:Int = 11
	Const TYPE_BYTES:Int = 12
	Const TYPE_UINT32:Int = 13
	Const TYPE_ENUM:Int = 14
	Const TYPE_SFIXED32:Int = 15
	Const TYPE_SFIXED64:Int = 16
	Const TYPE_SINT32:Int = 17
	Const TYPE_SINT64:Int = 18
	Const MAX_TYPE:Int = 18
	
	Const LABEL_OPTIONAL:Int = 1
	Const LABEL_REQUIRED:Int = 2
	Const LABEL_REPEATED:Int = 3
	Const MAX_LABEL:Int = 3
		
	Function _create:TFieldDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TFieldDescriptor = New TFieldDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Name of this field within the message. 
	End Rem
	Method GetName:String()
		Return bmx_pb_fielddescriptor_getname(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Fully-qualified name of the field. 
	End Rem
	Method GetFullName:String()
		Return bmx_pb_fielddescriptor_getfullname(descriptorPtr)
	End Method
	
	Rem
	bbdoc: File in which this field was defined. 
	End Rem
	Method GetFile:TFileDescriptor()
		Return TFileDescriptor._create(bmx_pb_fielddescriptor_getfile(descriptorPtr))
	End Method
	
	Rem
	bbdoc: Is this an extension field? 
	End Rem
	Method IsExtension:Int()
		Return bmx_pb_fielddescriptor_isextension(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Declared tag number. 
	End Rem
	Method GetNumber:Int()
		Return bmx_pb_fielddescriptor_getnumber(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Declared type of this field. 
	End Rem
	Method GetType:Int()
		Return bmx_pb_fielddescriptor_gettype(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Identifies whether the field is optional, required, or repeated.
	about: One of LABEL_OPTIONAL, LABEL_REQUIRED or LABEL_REPEATED.
	End Rem
	Method GetLabel:Int()
		Return bmx_pb_fielddescriptor_getlabel(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Shorthand for GetLabel() = LABEL_REQUIRED 
	End Rem
	Method IsRequired:Int()
		Return bmx_pb_fielddescriptor_isrequired(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Shorthand for GetLabel() = LABEL_OPTIONAL
	End Rem
	Method IsOptional:Int()
		Return bmx_pb_fielddescriptor_isoptional(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Shorthand for GetLabel() = LABEL_REPEATED
	End Rem
	Method IsRepeated:Int()
		Return bmx_pb_fielddescriptor_isrepeated(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Index of this field within the message's field array, or the file or extension scope's extensions array. 
	End Rem
	Method GetIndex:Int()
		Return bmx_pb_fielddescriptor_getindex(descriptorPtr)
	End Method
	
	Rem
	bbdoc: Does this field have an explicitly-declared default value? 
	End Rem
	Method HasDefaultValue:Int()
		Return bmx_pb_fielddescriptor_hasdefaultvalue(descriptorPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueInt:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueLong:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueDouble:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueFloat:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueString:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaultValueEnum:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetContainingType:TDescriptor()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetExtensionScope:TDescriptor()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetMessageType:TDescriptor()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetEnumType:TEnumDescriptor()
	End Method
	
	Rem
	bbdoc: Not (yet) implemented
	End Rem
	Method GetOptions:TFieldOptions()
	End Method
	
End Type

Rem
bbdoc: Describes an enum type defined in a .proto file. 
End Rem
Type TEnumDescriptor

	Field descriptorPtr:Byte Ptr
	
	Function _create:TEnumDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TEnumDescriptor = New TEnumDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function
	
	Method GetName:String()
	End Method
	
	Method GetFullName:String()
	End Method
	
	Method GetIndex:Int()
	End Method
	
	Method GetFile:TFileDescriptor()
	End Method
	
	Method ValueCount:Int()
	End Method
	
	Method GetValue:TEnumValueDescriptor(index:Int)
	End Method
	
	Method FindValueByName:TEnumValueDescriptor(name:String)
	End Method
	
	Method FindValueByNumber:TEnumValueDescriptor(number:Int)
	End Method
	
	Method GetContainingType:TDescriptor()
	End Method
	
	Method GetOptions:TEnumOptions()
	End Method
	
	Method CopyTo(proto:TEnumDescriptorProto)
	End Method
	
	Method DebugString:String()
	End Method

End Type

Rem
bbdoc: Describes an individual enum constant of a particular type. 
End Rem
Type TEnumValueDescriptor

	Field descriptorPtr:Byte Ptr

	Function _create:TEnumValueDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TEnumValueDescriptor = New TEnumValueDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function
	
	Method GetName:String()
	End Method
	
	Method GetIndex:Int()
	End Method
	
	Method GetNumber:Int()
	End Method
	
	Method GetFullName:String()
	End Method
	
	Method GetType:TEnumDescriptor()
	End Method
	
	Method GetOptions:TEnumValueOptions()
	End Method
	
	Method CopyTo(proto:TEnumValueDescriptorProto)
	End Method
	
	Method DebugString:String()
	End Method
	
End Type

Rem
bbdoc: A range of field numbers which are designated for third-party extensions. 
End Rem
Type TExtensionRange

	Field rangePtr:Byte Ptr

	Function _create:TExtensionRange(rangePtr:Byte Ptr)
		If rangePtr Then
			Local this:TExtensionRange = New TExtensionRange
			this.rangePtr = rangePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The range start - inclusive.
	End Rem
	Method GetStart:Int()
	End Method
	
	Rem
	bbdoc: The range end - exclusive.
	End Rem
	Method GetEnd:Int()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TServiceDescriptor

	Field descriptorPtr:Byte Ptr

	Function _create:TServiceDescriptor(descriptorPtr:Byte Ptr)
		If descriptorPtr Then
			Local this:TServiceDescriptor = New TServiceDescriptor
			this.descriptorPtr = descriptorPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TFieldOptions
End Type

Rem
bbdoc: 
End Rem
Type TEnumValueOptions
End Type

Rem
bbdoc: 
End Rem
Type TEnumValueDescriptorProto
End Type

Rem
bbdoc: 
End Rem
Type TEnumDescriptorProto
End Type

Rem
bbdoc: 
End Rem
Type TFileDescriptorProto
End Type

Rem
bbdoc: 
End Rem
Type TEnumOptions
End Type

Rem
bbdoc: 
End Rem
Type TFileOptions
End Type

Rem
bbdoc: 
End Rem
Type TDescriptorPool

	Field poolPtr:Byte Ptr

	Function _create:TDescriptorPool(poolPtr:Byte Ptr)
		If poolPtr Then
			Local this:TDescriptorPool = New TDescriptorPool
			this.poolPtr = poolPtr
			Return this
		End If
	End Function

End Type


' internal type
Type TCopyingInputStream

	Field streamPtr:Byte Ptr
	Field stream:TStream
	
	Function Create:TCopyingInputStream(stream:TStream)	
		Local this:TCopyingInputStream = New TCopyingInputStream
		this.stream = stream
		this.streamPtr = bmx_pb_copyinginputstream_create(this)
		Return this
	End Function

	Function _Read:Int(stream:TCopyingInputStream, buffer:Byte Ptr, size:Int)
		Return stream.stream.Read(buffer, size)
	End Function

	Function _Skip:Int(stream:TCopyingInputStream, count:Int)
		Return stream.stream.SkipBytes(count)
	End Function

	Method Delete()
		If streamPtr Then
			bmx_pb_copyinginputstream_delete(streamPtr)
			streamPtr = Null
		End If
	End Method
End Type

' internal type
Type TCopyingOutputStream

	Field streamPtr:Byte Ptr
	Field stream:TStream
	
	Function Create:TCopyingOutputStream(stream:TStream)	
		Local this:TCopyingOutputStream = New TCopyingOutputStream
		this.stream = stream
		this.streamPtr = bmx_pb_copyingoutputstream_create(this)
		Return this
	End Function

	Function _Write:Int(stream:TCopyingOutputStream, buffer:Byte Ptr, size:Int)
		Return stream.stream.Write(buffer, size)
	End Function

	Method Delete()
		If streamPtr Then
			bmx_pb_copyingoutputstream_delete(streamPtr)
			streamPtr = Null
		End If
	End Method
End Type

