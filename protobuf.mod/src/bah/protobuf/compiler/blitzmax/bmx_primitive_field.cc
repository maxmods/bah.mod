// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <bah/protobuf/compiler/blitzmax/bmx_primitive_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

using internal::WireFormatLite;

namespace {

// For encodings with fixed sizes, returns that size in bytes.  Otherwise
// returns -1.
int FixedSize(FieldDescriptor::Type type) {
  switch (type) {
    case FieldDescriptor::TYPE_INT32   : return -1;
    case FieldDescriptor::TYPE_INT64   : return -1;
    case FieldDescriptor::TYPE_UINT32  : return -1;
    case FieldDescriptor::TYPE_UINT64  : return -1;
    case FieldDescriptor::TYPE_SINT32  : return -1;
    case FieldDescriptor::TYPE_SINT64  : return -1;
    case FieldDescriptor::TYPE_FIXED32 : return WireFormatLite::kFixed32Size;
    case FieldDescriptor::TYPE_FIXED64 : return WireFormatLite::kFixed64Size;
    case FieldDescriptor::TYPE_SFIXED32: return WireFormatLite::kSFixed32Size;
    case FieldDescriptor::TYPE_SFIXED64: return WireFormatLite::kSFixed64Size;
    case FieldDescriptor::TYPE_FLOAT   : return WireFormatLite::kFloatSize;
    case FieldDescriptor::TYPE_DOUBLE  : return WireFormatLite::kDoubleSize;

    case FieldDescriptor::TYPE_BOOL    : return WireFormatLite::kBoolSize;
    case FieldDescriptor::TYPE_ENUM    : return -1;

    case FieldDescriptor::TYPE_STRING  : return -1;
    case FieldDescriptor::TYPE_BYTES   : return -1;
    case FieldDescriptor::TYPE_GROUP   : return -1;
    case FieldDescriptor::TYPE_MESSAGE : return -1;

    // No default because we want the compiler to complain if any new
    // types are added.
  }
  GOOGLE_LOG(FATAL) << "Can't get here.";
  return -1;
}

string DefaultValue(const FieldDescriptor* field) {
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      return SimpleItoa(field->default_value_int32());
    case FieldDescriptor::CPPTYPE_UINT32:
      return SimpleItoa(field->default_value_uint32()) + "u";
    case FieldDescriptor::CPPTYPE_INT64:
      return "GOOGLE_LONGLONG(" + SimpleItoa(field->default_value_int64()) + ")";
    case FieldDescriptor::CPPTYPE_UINT64:
      return "GOOGLE_ULONGLONG(" + SimpleItoa(field->default_value_uint64())+ ")";
    case FieldDescriptor::CPPTYPE_DOUBLE:
      return SimpleDtoa(field->default_value_double());
    case FieldDescriptor::CPPTYPE_FLOAT:
      return SimpleFtoa(field->default_value_float());
    case FieldDescriptor::CPPTYPE_BOOL:
      return field->default_value_bool() ? "true" : "false";

    case FieldDescriptor::CPPTYPE_ENUM:
    case FieldDescriptor::CPPTYPE_STRING:
    case FieldDescriptor::CPPTYPE_MESSAGE:
      GOOGLE_LOG(FATAL) << "Shouldn't get here.";
      return "";
  }
  // Can't actually get here; make compiler happy.  (We could add a default
  // case above but then we wouldn't get the nice compiler warning when a
  // new type is added.)
  return "";
}

void SetPrimitiveVariables(const FieldDescriptor* descriptor,
                           map<string, string>* variables) {
  SetCommonFieldVariables(descriptor, variables);
  (*variables)["type"] = PrimitiveTypeName(descriptor->cpp_type());
  (*variables)["uname"] = UnderscoresToCapitalizedCamelCase(descriptor);
  (*variables)["gtype"] = PrimitiveGlueTypeName(descriptor->cpp_type());
  (*variables)["default"] = DefaultValue(descriptor);
  (*variables)["tag"] = SimpleItoa(internal::WireFormat::MakeTag(descriptor));
  (*variables)["declared_type"] = DeclaredTypeMethodName(descriptor->type());
 int fixed_size = FixedSize(descriptor->type());
  if (fixed_size != -1) {
    (*variables)["fixed_size"] = SimpleItoa(fixed_size);
  }
  (*variables)["wire_format_field_type"] =
      "::google::protobuf::internal::WireFormatLite::" + FieldDescriptorProto_Type_Name(
          static_cast<FieldDescriptorProto_Type>(descriptor->type()));
}

}  // namespace

// ===================================================================

PrimitiveFieldGenerator::
PrimitiveFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetPrimitiveVariables(descriptor, &variables_);
}

PrimitiveFieldGenerator::~PrimitiveFieldGenerator() {}

void PrimitiveFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_, "$type$ $name$_;\n");
}


void PrimitiveFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    "$gtype$ bmx_pb_$classname$_$name$_get($classname$ * handle);\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, $gtype$ value);\n");
}

void PrimitiveFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {

  printer->Print(variables_,
    "$gtype$ bmx_pb_$classname$_$name$_get($classname$ * handle) {\n"
    "  return handle->$name$();\n"
    "}\n"
    "\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, $gtype$ value) {\n"
    "  handle->set_$name$(value);\n"
    "}\n"
    "\n");

}


void PrimitiveFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "Method Get$uname$:$type$()\n"
    "  Return bmx_pb_$classname$_$name$_get(messagePtr)\n"
    "End Method\n"
    "\n"
    "Method Set$uname$(value:$type$)\n"
    "  bmx_pb_$classname$_$name$_set(messagePtr, value)\n"
    "End Method\n");
}

void PrimitiveFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
  printer->Print(variables_,
    "Function bmx_pb_$classname$_$name$_get:$type$(handle:Byte Ptr)\n"
    "Function bmx_pb_$classname$_$name$_set(handle:Byte Ptr, value:$type$)\n");
    
}


void PrimitiveFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_ = $default$;\n");
}

void PrimitiveFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "set_$name$(from.$name$());\n");
}

void PrimitiveFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  printer->Print(variables_, ",\n$name$_($default$)");
}

void PrimitiveFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$(\n"
    "      input, &$name$_));\n"
    "_set_bit($index$);\n");
}

void PrimitiveFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(), output));\n");
}

void PrimitiveFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  int fixed_size = FixedSize(descriptor_->type());
  if (fixed_size == -1) {
    printer->Print(variables_,
      "total_size += $tag_size$ +\n"
      "  ::google::protobuf::internal::WireFormat::$declared_type$Size(\n"
      "    this->$name$());\n");
  } else {
    printer->Print(variables_,
      "total_size += $tag_size$ + $fixed_size$;\n");
  }
}

// ===================================================================

RepeatedPrimitiveFieldGenerator::
RepeatedPrimitiveFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetPrimitiveVariables(descriptor, &variables_);
}

RepeatedPrimitiveFieldGenerator::~RepeatedPrimitiveFieldGenerator() {}

void RepeatedPrimitiveFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_,
    "::google::protobuf::RepeatedField< $type$ > $name$_;\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    //"inline const ::google::protobuf::RepeatedField< $type$ >& $name$() const;\n"
    //"inline ::google::protobuf::RepeatedField< $type$ >* mutable_$name$();\n"
    "$gtype$ bmx_pb_$classname$_$name$_get($classname$ * handle, int index);\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, int index, $gtype$ value);\n"
    "void bmx_pb_$classname$_$name$_add($classname$ * handle, $gtype$ value);\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {

  printer->Print(variables_,
    "$gtype$ bmx_pb_$classname$_$name$_get($classname$ * handle, int index) {\n"
    "  return handle->$name$(index);\n"
    "}\n"
    "\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, int index, $gtype$ value) {\n"
    "  handle->set_$name$(index, value);\n"
    "}\n"
    "\n"
    "void bmx_pb_$classname$_$name$_add($classname$ * handle, $gtype$ value) {\n"
    "  handle->add_$name$(value);\n"
    "}\n"
    "\n");

}

void RepeatedPrimitiveFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    //"inline const ::google::protobuf::RepeatedField< $type$ >&\n"
    //"$classname$::$name$() const {\n"
    //"  return $name$_;\n"
    //"}\n"
    //"inline ::google::protobuf::RepeatedField< $type$ >*\n"
    //"$classname$::mutable_$name$() {\n"
    //"  return &$name$_;\n"
    //"}\n"
    "Method Get$name$:$type$(index:Int)\n"
    "\tReturn bmx_pb_$classname$_$name$_get(messagePtr, index)\n"
    "End Method\n"
    "\n"
    "Method Set$name$(index:Int, value:$type$)\n"
    "\tbmx_pb_$classname$_$name$_set(messagePtr, index, value)\n"
    "End Method\n"
    "\n"
    "Method Add$name$(value:$type$)\n"
    "\tbmx_pb_$classname$_$name$_add(messagePtr, value)\n"
    "End Method\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.Clear();\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.MergeFrom(from.$name$_);\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  // Not needed for repeated fields.
}

void RepeatedPrimitiveFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "$type$ value;\n"
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$(input, &value));\n"
    "add_$name$(value);\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(i), output));\n");
}

void RepeatedPrimitiveFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  int fixed_size = FixedSize(descriptor_->type());
  if (fixed_size == -1) {
    printer->Print(variables_,
      "total_size += $tag_size$ * $name$_size();\n"
      "for (int i = 0; i < $name$_size(); i++) {\n"
      "  total_size += ::google::protobuf::internal::WireFormat::$declared_type$Size(\n"
      "    this->$name$(i));\n"
      "}\n");
  } else {
    printer->Print(variables_,
      "total_size += ($tag_size$ + $fixed_size$) * $name$_size();\n");
  }
}

void RepeatedPrimitiveFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {

  printer->Print(variables_,
    "Function bmx_pb_$classname$_$name$_get:$type$(handle:Byte Ptr, index:Int)\n"
    "Function bmx_pb_$classname$_$name$_set(handle:Byte Ptr, index:Int, value:$type$)\n"
    "Function bmx_pb_$classname$_$name$_add(handle:Byte Ptr, value:$type$)\n");

}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
