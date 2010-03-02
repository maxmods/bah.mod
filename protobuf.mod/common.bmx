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

Import "source.bmx"



Extern

	Function bmx_commandlineinterface_create:Byte Ptr()
	Function bmx_commandlineinterface_delete(handle:Byte Ptr)
	Function bmx_commandlineinterface_run:Int(handle:Byte Ptr, argv:String[])

	Function bmx_textformat_printtostring:String(message:Byte Ptr)

	Function bmx_pb_message_clear(handle:Byte Ptr)
	Function bmx_pb_message_checkinitialized(handle:Byte Ptr)
	Function bmx_pb_message_isinitialized:Int(handle:Byte Ptr)
	Function bmx_pb_message_initializationerrorstring:String(handle:Byte Ptr)
	Function bmx_pb_message_discardunknownfields(handle:Byte Ptr)
	Function bmx_pb_message_delete(handle:Byte Ptr)
	Function bmx_pb_message_parsefromstring:Int(handle:Byte Ptr, data:String)
	Function bmx_pb_message_parsefrombytes:Int(handle:Byte Ptr, data:Byte Ptr, size:Int)
	Function bmx_pb_message_parsefromzerocopystream:Int(handle:Byte Ptr, inputStream:Byte Ptr)
	Function bmx_pb_message_serializetostring:String(handle:Byte Ptr)
	Function bmx_pb_message_serializetobytes:Int(handle:Byte Ptr, data:Byte Ptr, size:Int)
	Function bmx_pb_message_serializetozerocopystream:Int(handle:Byte Ptr, outputStream:Byte Ptr)

	Function bmx_pb_descriptor_getname:String(handle:Byte Ptr)
	Function bmx_pb_descriptor_getfullname:String(handle:Byte Ptr)
	Function bmx_pb_descriptor_getindex:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_debugstring:String(handle:Byte Ptr)
	Function bmx_pb_descriptor_fieldcount:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_getfile:Byte Ptr(handle:Byte Ptr)
	Function bmx_pb_descriptor_getcontainingtype:Byte Ptr(handle:Byte Ptr)
	Function bmx_pb_descriptor_getfield:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_descriptor_findfieldbynumber:Byte Ptr(handle:Byte Ptr, number:Int)
	Function bmx_pb_descriptor_findfieldbyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_descriptor_nestedtypecount:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_getnestedtype:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_descriptor_findnestedtypebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_descriptor_enumtypecount:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_getenumtype:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_descriptor_findenumtypebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_descriptor_findenumvaluebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_descriptor_extensionrangecount:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_getextensionrange:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_descriptor_isextensionnumber:Int(handle:Byte Ptr, number:Int)
	Function bmx_pb_descriptor_extensioncount:Int(handle:Byte Ptr)
	Function bmx_pb_descriptor_getextension:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_descriptor_findextensionbyname:Byte Ptr(handle:Byte Ptr, name:String)

	Function bmx_pb_fielddescriptor_getname:String(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_getfullname:String(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_getfile:Byte Ptr(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_isextension:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_getnumber:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_gettype:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_isrequired:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_isoptional:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_isrepeated:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_getindex:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_hasdefaultvalue:Int(handle:Byte Ptr)
	Function bmx_pb_fielddescriptor_getlabel:Int(handle:Byte Ptr)

	Function bmx_pb_copyinginputstream_create:Byte Ptr(handle:Object)
	Function bmx_pb_copyinginputstream_delete(handle:Byte Ptr)

	Function bmx_pb_copyingoutputstream_create:Byte Ptr(handle:Object)
	Function bmx_pb_copyingoutputstream_delete(handle:Byte Ptr)

	Function bmx_pb_filedescriptor_getname:String(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getpackage:String(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getpool:Byte Ptr(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_dependencycount:Int(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getdependency:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_filedescriptor_messagetypecount:Int(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getmessagetype:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_filedescriptor_enumtypecount:Int(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getenumtype:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_filedescriptor_servicecount:Int(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getservice:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_filedescriptor_extensioncount:Int(handle:Byte Ptr)
	Function bmx_pb_filedescriptor_getextension:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_pb_filedescriptor_findmessagetypebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_filedescriptor_findenumtypebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_filedescriptor_findenumvaluebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_filedescriptor_findservicebyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_filedescriptor_findextensionbyname:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_pb_filedescriptor_debugstring:String(handle:Byte Ptr)

End Extern


' converts a UTF character array from byte-size characters to short-size characters
' based on the TextStream UTF code...
Function _pbConvertUTF8ToMax:String(s:Byte Ptr, l:Int)
	If s Then
		Local b:Short[] = New Short[l]
		Local bc:Int = -1
		Local c:Int
		Local d:Int
		Local e:Int
		For Local i:Int = 0 Until l
			bc:+1
			c = s[i]
			If c<128 
				b[bc] = c
				Continue
			End If
			i:+1
			d=s[i]
			If c<224 
				b[bc] = (c-192)*64+(d-128)
				Continue
			End If
			i:+1
			e = s[i]
			If c < 240 
				b[bc] = (c-224)*4096+(d-128)*64+(e-128)
				Continue
			End If
		Next
	
		Return String.fromshorts(b, bc + 1)
	End If
	
	Return ""
	
End Function
