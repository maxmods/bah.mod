/*
 Copyright (c) 2008-2010 Bruce A Henderson
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

#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/python/python_generator.h>
#include <google/protobuf/compiler/java/java_generator.h>
#include <bah/protobuf/compiler/blitzmax/bmx_generator.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {
}
}
}
}

class MaxCopyingInputStream;
class MaxCopyingOutputStream;

extern "C" {

#include "blitz.h"

	int _bah_protobuf_TCopyingInputStream__Read(BBObject * handle, void * buffer, int size);
	int _bah_protobuf_TCopyingInputStream__Skip(BBObject * handle, int count);
	bool _bah_protobuf_TCopyingOutputStream__Write(BBObject * handle, const void * buffer, int size);

	google::protobuf::compiler::CommandLineInterface * bmx_commandlineinterface_create();
	void bmx_commandlineinterface_delete(google::protobuf::compiler::CommandLineInterface * cli);
	int bmx_commandlineinterface_run(google::protobuf::compiler::CommandLineInterface * cli, BBArray * argv);

	BBString * bmx_textformat_printtostring(google::protobuf::Message * message);

	void bmx_pb_message_clear(google::protobuf::Message * message);
	void bmx_pb_message_checkinitialized(google::protobuf::Message * message);
	bool bmx_pb_message_isinitialized(google::protobuf::Message * message);
	BBString * bmx_pb_message_initializationerrorstring(google::protobuf::Message * message);
	void bmx_pb_message_discardunknownfields(google::protobuf::Message * message);
	void bmx_pb_message_delete(google::protobuf::Message * message);
	bool bmx_pb_message_parsefromstring(google::protobuf::Message * message, BBString * data);
	bool bmx_pb_message_parsefrombytes(google::protobuf::Message * message, const void * data, int size);
	bool bmx_pb_message_parsefromzerocopystream(google::protobuf::Message * message, MaxCopyingInputStream * inputStream);
	BBString * bmx_pb_message_serializetostring(google::protobuf::Message * message);
	bool bmx_pb_message_serializetobytes(google::protobuf::Message * message, void * data, int size);
	bool bmx_pb_message_serializetozerocopystream(google::protobuf::Message * message, MaxCopyingOutputStream * outputStream);

	BBString * bmx_pb_descriptor_getname(google::protobuf::Descriptor * desc);
	BBString * bmx_pb_descriptor_getfullname(google::protobuf::Descriptor * desc);
	int bmx_pb_descriptor_getindex(google::protobuf::Descriptor * desc);
	BBString * bmx_pb_descriptor_debugstring(google::protobuf::Descriptor * desc);
	int bmx_pb_descriptor_fieldcount(google::protobuf::Descriptor * desc);
	const google::protobuf::FileDescriptor * bmx_pb_descriptor_getfile(google::protobuf::Descriptor * desc);
	const google::protobuf::Descriptor * bmx_pb_descriptor_getcontainingtype(google::protobuf::Descriptor * desc);
	const google::protobuf::FieldDescriptor * bmx_pb_descriptor_getfield(google::protobuf::Descriptor * desc, int index);
	const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findfieldbynumber(google::protobuf::Descriptor * desc, int number);
	const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findfieldbyname(google::protobuf::Descriptor * desc, BBString * name);
	int bmx_pb_descriptor_nestedtypecount(google::protobuf::Descriptor * desc);
	const google::protobuf::Descriptor * bmx_pb_descriptor_getnestedtype(google::protobuf::Descriptor * desc, int index);
	const google::protobuf::Descriptor * bmx_pb_descriptor_findnestedtypebyname(google::protobuf::Descriptor * desc, BBString * name);
	int bmx_pb_descriptor_enumtypecount(google::protobuf::Descriptor * desc);
	const google::protobuf::EnumDescriptor * bmx_pb_descriptor_getenumtype(google::protobuf::Descriptor * desc, int index);
	const google::protobuf::EnumDescriptor * bmx_pb_descriptor_findenumtypebyname(google::protobuf::Descriptor * desc, BBString * name);
	const google::protobuf::EnumValueDescriptor * bmx_pb_descriptor_findenumvaluebyname(google::protobuf::Descriptor * desc, BBString * name);
	int bmx_pb_descriptor_extensionrangecount(google::protobuf::Descriptor * desc);
	const google::protobuf::Descriptor::ExtensionRange * bmx_pb_descriptor_getextensionrange(google::protobuf::Descriptor * desc, int index);
	bool bmx_pb_descriptor_isextensionnumber(google::protobuf::Descriptor * desc, int number);
	int bmx_pb_descriptor_extensioncount(google::protobuf::Descriptor * desc);
	const google::protobuf::FieldDescriptor * bmx_pb_descriptor_getextension(google::protobuf::Descriptor * desc, int index);
	const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findextensionbyname(google::protobuf::Descriptor * desc, BBString * name);


	BBString * bmx_pb_fielddescriptor_getname(google::protobuf::FieldDescriptor * desc);
	BBString * bmx_pb_fielddescriptor_getfullname(google::protobuf::FieldDescriptor * desc);
	const google::protobuf::FileDescriptor * bmx_pb_fielddescriptor_getfile(google::protobuf::FieldDescriptor * desc);
	bool bmx_pb_fielddescriptor_isextension(google::protobuf::FieldDescriptor * desc);
	int bmx_pb_fielddescriptor_getnumber(google::protobuf::FieldDescriptor * desc);
	google::protobuf::FieldDescriptor::Type bmx_pb_fielddescriptor_gettype(google::protobuf::FieldDescriptor * desc);
	bool bmx_pb_fielddescriptor_isrequired(google::protobuf::FieldDescriptor * desc);
	bool bmx_pb_fielddescriptor_isoptional(google::protobuf::FieldDescriptor * desc);
	bool bmx_pb_fielddescriptor_isrepeated(google::protobuf::FieldDescriptor * desc);
	int bmx_pb_fielddescriptor_getindex(google::protobuf::FieldDescriptor * desc);
	bool bmx_pb_fielddescriptor_hasdefaultvalue(google::protobuf::FieldDescriptor * desc);
	google::protobuf::FieldDescriptor::Label bmx_pb_fielddescriptor_getlabel(google::protobuf::FieldDescriptor * desc);

	MaxCopyingInputStream * bmx_pb_copyinginputstream_create(BBObject * handle);
	void bmx_pb_copyinginputstream_delete(MaxCopyingInputStream *stream);

	MaxCopyingOutputStream * bmx_pb_copyingoutputstream_create(BBObject * handle);
	void bmx_pb_copyingoutputstream_delete(MaxCopyingOutputStream *stream);

	BBString * bmx_pb_filedescriptor_getname(google::protobuf::FileDescriptor * desc);
	BBString * bmx_pb_filedescriptor_getpackage(google::protobuf::FileDescriptor * desc);
	const google::protobuf::DescriptorPool * bmx_pb_filedescriptor_getpool(google::protobuf::FileDescriptor * desc);
	int  bmx_pb_filedescriptor_dependencycount(google::protobuf::FileDescriptor * desc);
	const google::protobuf::FileDescriptor * bmx_pb_filedescriptor_getdependency(google::protobuf::FileDescriptor * desc, int index);
	int  bmx_pb_filedescriptor_messagetypecount(google::protobuf::FileDescriptor * desc);
	const google::protobuf::Descriptor * bmx_pb_filedescriptor_getmessagetype(google::protobuf::FileDescriptor * desc, int index);
	int  bmx_pb_filedescriptor_enumtypecount(google::protobuf::FileDescriptor * desc);
	const google::protobuf::EnumDescriptor * bmx_pb_filedescriptor_getenumtype(google::protobuf::FileDescriptor * desc, int index);
	int  bmx_pb_filedescriptor_servicecount(google::protobuf::FileDescriptor * desc);
	const google::protobuf::ServiceDescriptor * bmx_pb_filedescriptor_getservice(google::protobuf::FileDescriptor * desc, int index);
	int  bmx_pb_filedescriptor_extensioncount(google::protobuf::FileDescriptor * desc);
	const google::protobuf::FieldDescriptor * bmx_pb_filedescriptor_getextension(google::protobuf::FileDescriptor * desc, int index);
	const google::protobuf::Descriptor * bmx_pb_filedescriptor_findmessagetypebyname(google::protobuf::FileDescriptor * desc, BBString * name);
	const google::protobuf::EnumDescriptor * bmx_pb_filedescriptor_findenumtypebyname(google::protobuf::FileDescriptor * desc, BBString * name);
	const google::protobuf::EnumValueDescriptor * bmx_pb_filedescriptor_findenumvaluebyname(google::protobuf::FileDescriptor * desc, BBString * name);
	const google::protobuf::ServiceDescriptor * bmx_pb_filedescriptor_findservicebyname(google::protobuf::FileDescriptor * desc, BBString * name);
	const google::protobuf::FieldDescriptor * bmx_pb_filedescriptor_findextensionbyname(google::protobuf::FileDescriptor * desc, BBString * name);
	BBString * bmx_pb_filedescriptor_debugstring(google::protobuf::FileDescriptor * desc);

}

class MaxCopyingInputStream : public google::protobuf::io::CopyingInputStream
{
public:
	MaxCopyingInputStream(BBObject * handle)
		: maxHandle(handle)
	{
		BBRETAIN(handle);
	}
	
	~MaxCopyingInputStream() {
		BBRELEASE(maxHandle);
	}
	
	int Read(void* buffer, int size) {
		return _bah_protobuf_TCopyingInputStream__Read(maxHandle, buffer, size);
	}
	
	int Skip(int count) {
		return _bah_protobuf_TCopyingInputStream__Skip(maxHandle, count);
	}
	

private:
	BBObject * maxHandle;
};

class MaxCopyingOutputStream : public google::protobuf::io::CopyingOutputStream
{
public:
	MaxCopyingOutputStream(BBObject * handle)
		: maxHandle(handle)
	{
		BBRETAIN(handle);
	}
	
	~MaxCopyingOutputStream() {
		BBRELEASE(maxHandle);
	}
	
	bool Write(const void * buffer, int size) {
		return _bah_protobuf_TCopyingOutputStream__Write(maxHandle, buffer, size);
	}
	
private:
	BBObject * maxHandle;
};


google::protobuf::compiler::CommandLineInterface  * bmx_commandlineinterface_create() {

	google::protobuf::compiler::CommandLineInterface * cli = new google::protobuf::compiler::CommandLineInterface;
	
	// Proto2 C++
	google::protobuf::compiler::cpp::CppGenerator * cpp_generator = new google::protobuf::compiler::cpp::CppGenerator;
	cli->RegisterGenerator("--cpp_out", cpp_generator,
	                      "Generate C++ header and source.");
	
	// Proto2 Java
	google::protobuf::compiler::java::JavaGenerator * java_generator = new google::protobuf::compiler::java::JavaGenerator;
	cli->RegisterGenerator("--java_out", java_generator,
	                      "Generate Java source file.");
	
	
	// Proto2 Python
	google::protobuf::compiler::python::Generator * py_generator = new google::protobuf::compiler::python::Generator;
	cli->RegisterGenerator("--python_out", py_generator,
	                      "Generate Python source file.");


	// Proto2 BlitzMax
	google::protobuf::compiler::bmx::BmxGenerator * bmx_generator = new google::protobuf::compiler::bmx::BmxGenerator;
	cli->RegisterGenerator("--bmx_out", bmx_generator,
	                      "Generate Blitzmax source file.");

	return cli;
}

void bmx_commandlineinterface_delete(google::protobuf::compiler::CommandLineInterface * cli) {
	delete cli;
}

int bmx_commandlineinterface_run(google::protobuf::compiler::CommandLineInterface * cli, BBArray * argv) {

	int n = argv->scales[0];

	BBString **s=(BBString**)BBARRAYDATA( argv,argv->dims );
	char* args[n];

	for( int i=0;i<n;++i ){
		args[i] = bbStringToCString( s[i] );
	}

	int ret = cli->Run(n, args);

	for( int i=0;i<n;++i ){
		bbMemFree( args[i] );	
	}
	
	return ret;
}

#include <iostream>

// ******************************************************

BBString * bmx_textformat_printtostring(google::protobuf::Message * message) {
	std::string s;
	bool res = google::protobuf::TextFormat::PrintToString(*message, &s);

	return bbStringFromUTF8String(s.c_str());
}

// ******************************************************

void bmx_pb_message_clear(google::protobuf::Message * message) {
	message->Clear();
}

void bmx_pb_message_checkinitialized(google::protobuf::Message * message) {
	message->CheckInitialized();
}

bool bmx_pb_message_isinitialized(google::protobuf::Message * message) {
	return message->IsInitialized();
}

BBString * bmx_pb_message_initializationerrorstring(google::protobuf::Message * message) {
	return bbStringFromUTF8String(message->InitializationErrorString().c_str());
}

void bmx_pb_message_discardunknownfields(google::protobuf::Message * message) {
	message->DiscardUnknownFields();
}

void bmx_pb_message_delete(google::protobuf::Message * message) {
	delete message;
}

bool bmx_pb_message_parsefromstring(google::protobuf::Message * message, BBString * data) {
	char *p = bbStringToCString( data );
	std::string s = std::string(p);
	bool res = message->ParseFromString(s);
	bbMemFree(p);
	return res;
}

bool bmx_pb_message_parsefrombytes(google::protobuf::Message * message, const void * data, int size) {
	return message->ParseFromArray(data, size);
}

bool bmx_pb_message_parsefromzerocopystream(google::protobuf::Message * message, MaxCopyingInputStream * inputStream) {

	google::protobuf::io::CopyingInputStreamAdaptor adaptor(inputStream);
	
	return message->ParseFromZeroCopyStream(&adaptor);

}

BBString * bmx_pb_message_serializetostring(google::protobuf::Message * message) {
	std::string s;
	bool res = message->SerializeToString(&s);
	if (res) {
		return bbStringFromBytes(s.c_str(), s.length());
	} else {
		return &bbEmptyString;
	}
}

bool bmx_pb_message_serializetobytes(google::protobuf::Message * message, void * data, int size) {
	return message->SerializeToArray(data, size);
}

bool bmx_pb_message_serializetozerocopystream(google::protobuf::Message * message, MaxCopyingOutputStream * outputStream) {

	google::protobuf::io::CopyingOutputStreamAdaptor adaptor(outputStream);
	
	return message->SerializeToZeroCopyStream(&adaptor);

}

// ******************************************************

BBString * bmx_pb_descriptor_getname(google::protobuf::Descriptor * desc) {
	return bbStringFromUTF8String(desc->name().c_str());
}

BBString * bmx_pb_descriptor_getfullname(google::protobuf::Descriptor * desc) {
	return bbStringFromUTF8String(desc->full_name().c_str());
}

int bmx_pb_descriptor_getindex(google::protobuf::Descriptor * desc) {
	return desc->index();
}

BBString * bmx_pb_descriptor_debugstring(google::protobuf::Descriptor * desc) {
	return bbStringFromUTF8String(desc->DebugString().c_str());
}

int bmx_pb_descriptor_fieldcount(google::protobuf::Descriptor * desc) {
	return desc->field_count();
}

const google::protobuf::FileDescriptor * bmx_pb_descriptor_getfile(google::protobuf::Descriptor * desc) {
	return desc->file();
}

const google::protobuf::Descriptor * bmx_pb_descriptor_getcontainingtype(google::protobuf::Descriptor * desc) {
	return desc->containing_type();
}

const google::protobuf::FieldDescriptor * bmx_pb_descriptor_getfield(google::protobuf::Descriptor * desc, int index) {
	return desc->field(index);
}

const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findfieldbynumber(google::protobuf::Descriptor * desc, int number) {
	return desc->FindFieldByNumber(number);
}

const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findfieldbyname(google::protobuf::Descriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::FieldDescriptor * file = desc->FindFieldByName(s);
	bbMemFree(p);
	return file;
}

int bmx_pb_descriptor_nestedtypecount(google::protobuf::Descriptor * desc) {
	return desc->nested_type_count();
}

const google::protobuf::Descriptor * bmx_pb_descriptor_getnestedtype(google::protobuf::Descriptor * desc, int index) {
	return desc->nested_type(index);
}

const google::protobuf::Descriptor * bmx_pb_descriptor_findnestedtypebyname(google::protobuf::Descriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::Descriptor * nested = desc->FindNestedTypeByName(s);
	bbMemFree(p);
	return nested;
}

int bmx_pb_descriptor_enumtypecount(google::protobuf::Descriptor * desc) {
	return desc->enum_type_count();
}

const google::protobuf::EnumDescriptor * bmx_pb_descriptor_getenumtype(google::protobuf::Descriptor * desc, int index) {
	return desc->enum_type(index);
}

const google::protobuf::EnumDescriptor * bmx_pb_descriptor_findenumtypebyname(google::protobuf::Descriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::EnumDescriptor * et = desc->FindEnumTypeByName(s);
	bbMemFree(p);
	return et;
}

const google::protobuf::EnumValueDescriptor * bmx_pb_descriptor_findenumvaluebyname(google::protobuf::Descriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::EnumValueDescriptor * ev = desc->FindEnumValueByName(s);
	bbMemFree(p);
	return ev;
}

int bmx_pb_descriptor_extensionrangecount(google::protobuf::Descriptor * desc) {
	return desc->extension_range_count();
}

const google::protobuf::Descriptor::ExtensionRange * bmx_pb_descriptor_getextensionrange(google::protobuf::Descriptor * desc, int index) {
	return desc->extension_range(index);
}

bool bmx_pb_descriptor_isextensionnumber(google::protobuf::Descriptor * desc, int number) {
	return desc->IsExtensionNumber(number);
}

int bmx_pb_descriptor_extensioncount(google::protobuf::Descriptor * desc) {
	return desc->extension_count();
}

const google::protobuf::FieldDescriptor * bmx_pb_descriptor_getextension(google::protobuf::Descriptor * desc, int index) {
	return desc->extension(index);
}

const google::protobuf::FieldDescriptor * bmx_pb_descriptor_findextensionbyname(google::protobuf::Descriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::FieldDescriptor * extension = desc->FindExtensionByName(s);
	bbMemFree(p);
	return extension;
}


// ******************************************************

BBString * bmx_pb_fielddescriptor_getname(google::protobuf::FieldDescriptor * desc) {
	return bbStringFromUTF8String(desc->name().c_str());
}

BBString * bmx_pb_fielddescriptor_getfullname(google::protobuf::FieldDescriptor * desc) {
	return bbStringFromUTF8String(desc->full_name().c_str());
}

const google::protobuf::FileDescriptor * bmx_pb_fielddescriptor_getfile(google::protobuf::FieldDescriptor * desc) {
	return desc->file();
}

bool bmx_pb_fielddescriptor_isextension(google::protobuf::FieldDescriptor * desc) {
	return desc->is_extension();
}

int bmx_pb_fielddescriptor_getnumber(google::protobuf::FieldDescriptor * desc) {
	return desc->number();
}

google::protobuf::FieldDescriptor::Type bmx_pb_fielddescriptor_gettype(google::protobuf::FieldDescriptor * desc) {
	return desc->type();
}

bool bmx_pb_fielddescriptor_isrequired(google::protobuf::FieldDescriptor * desc) {
	return desc->is_required();
}

bool bmx_pb_fielddescriptor_isoptional(google::protobuf::FieldDescriptor * desc) {
	return desc->is_optional();
}

bool bmx_pb_fielddescriptor_isrepeated(google::protobuf::FieldDescriptor * desc) {
	return desc->is_repeated();
}

int bmx_pb_fielddescriptor_getindex(google::protobuf::FieldDescriptor * desc) {
	return desc->index();
}

bool bmx_pb_fielddescriptor_hasdefaultvalue(google::protobuf::FieldDescriptor * desc) {
	return desc->has_default_value();
}

google::protobuf::FieldDescriptor::Label bmx_pb_fielddescriptor_getlabel(google::protobuf::FieldDescriptor * desc) {
	return desc->label();
}

// ******************************************************

MaxCopyingInputStream * bmx_pb_copyinginputstream_create(BBObject * handle) {
	return new MaxCopyingInputStream(handle);
}

void bmx_pb_copyinginputstream_delete(MaxCopyingInputStream *stream) {
	delete stream;
}

// ******************************************************

MaxCopyingOutputStream * bmx_pb_copyingoutputstream_create(BBObject * handle) {
	return new MaxCopyingOutputStream(handle);
}

void bmx_pb_copyingoutputstream_delete(MaxCopyingOutputStream *stream) {
	delete stream;
}

// ******************************************************

BBString * bmx_pb_filedescriptor_getname(google::protobuf::FileDescriptor * desc) {
	return bbStringFromUTF8String(desc->name().c_str());
}

BBString * bmx_pb_filedescriptor_getpackage(google::protobuf::FileDescriptor * desc) {
	return bbStringFromUTF8String(desc->package().c_str());
}

const google::protobuf::DescriptorPool * bmx_pb_filedescriptor_getpool(google::protobuf::FileDescriptor * desc) {
	return desc->pool();
}

int  bmx_pb_filedescriptor_dependencycount(google::protobuf::FileDescriptor * desc) {
	return desc->dependency_count();
}

const google::protobuf::FileDescriptor * bmx_pb_filedescriptor_getdependency(google::protobuf::FileDescriptor * desc, int index) {
	return desc->dependency(index);
}

int  bmx_pb_filedescriptor_messagetypecount(google::protobuf::FileDescriptor * desc) {
	return desc->message_type_count();
}

const google::protobuf::Descriptor * bmx_pb_filedescriptor_getmessagetype(google::protobuf::FileDescriptor * desc, int index) {
	return desc->message_type(index);
}

int  bmx_pb_filedescriptor_enumtypecount(google::protobuf::FileDescriptor * desc) {
	return desc->enum_type_count();
}

const google::protobuf::EnumDescriptor * bmx_pb_filedescriptor_getenumtype(google::protobuf::FileDescriptor * desc, int index) {
	return desc->enum_type(index);
}

int  bmx_pb_filedescriptor_servicecount(google::protobuf::FileDescriptor * desc) {
	return desc->service_count();
}

const google::protobuf::ServiceDescriptor * bmx_pb_filedescriptor_getservice(google::protobuf::FileDescriptor * desc, int index) {
	return desc->service(index);
}

int  bmx_pb_filedescriptor_extensioncount(google::protobuf::FileDescriptor * desc) {
	return desc->extension_count();
}

const google::protobuf::FieldDescriptor * bmx_pb_filedescriptor_getextension(google::protobuf::FileDescriptor * desc, int index) {
	return desc->extension(index);
}

const google::protobuf::Descriptor * bmx_pb_filedescriptor_findmessagetypebyname(google::protobuf::FileDescriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::Descriptor * d = desc->FindMessageTypeByName(s);
	bbMemFree(p);
	return d;
}

const google::protobuf::EnumDescriptor * bmx_pb_filedescriptor_findenumtypebyname(google::protobuf::FileDescriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::EnumDescriptor * d = desc->FindEnumTypeByName(s);
	bbMemFree(p);
	return d;
}

const google::protobuf::EnumValueDescriptor * bmx_pb_filedescriptor_findenumvaluebyname(google::protobuf::FileDescriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::EnumValueDescriptor * d = desc->FindEnumValueByName(s);
	bbMemFree(p);
	return d;
}

const google::protobuf::ServiceDescriptor * bmx_pb_filedescriptor_findservicebyname(google::protobuf::FileDescriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::ServiceDescriptor * d = desc->FindServiceByName(s);
	bbMemFree(p);
	return d;
}

const google::protobuf::FieldDescriptor * bmx_pb_filedescriptor_findextensionbyname(google::protobuf::FileDescriptor * desc, BBString * name) {
	char *p = bbStringToCString( name );
	std::string s = std::string(p);
	const google::protobuf::FieldDescriptor * d = desc->FindExtensionByName(s);
	bbMemFree(p);
	return d;
}

BBString * bmx_pb_filedescriptor_debugstring(google::protobuf::FileDescriptor * desc) {
	return bbStringFromUTF8String(desc->DebugString().c_str());
}


