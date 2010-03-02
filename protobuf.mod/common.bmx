' Copyright (c) 2008-2010 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
