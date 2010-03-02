// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.
// http://code.google.com/p/protobuf/
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <bah/protobuf/compiler/blitzmax/bmx_long_field.h>
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
  (*variables)["gtype"] = PrimitiveGlueTypeName(descriptor->cpp_type());
  (*variables)["default"] = DefaultValue(descriptor);
  (*variables)["tag"] = SimpleItoa(internal::WireFormat::MakeTag(descriptor));
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

LongFieldGenerator::
LongFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetPrimitiveVariables(descriptor, &variables_);
}

LongFieldGenerator::~LongFieldGenerator() {}

void LongFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_, "$type$ $name$_;\n");
}


void LongFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    "void bmx_pb_$classname$_$name$_get($classname$ * handle, $gtype$ * value);\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, $gtype$ value);\n");
}

void LongFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {

  printer->Print(variables_,
    "void bmx_pb_$classname$_$name$_get($classname$ * handle, $gtype$ * value) {\n"
    "  *value = handle->$name$();\n"
    "}\n"
    "\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, $gtype$ value) {\n"
    "  handle->set_$name$(value);\n"
    "}\n"
    "\n");

}


void LongFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "Method Get$uname$:$type$()\n"
    "  Local value:$type$\n"
    "  bmx_pb_$classname$_$name$_get(messagePtr, VarPtr value)\n"
    "  Return value\n"
    "End Method\n"
    "\n"
    "Method Set$uname$(value:$type$)\n"
    "  bmx_pb_$classname$_$name$_set(messagePtr, value)\n"
    "End Method\n");
}

void LongFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
  printer->Print(variables_,
    "Function bmx_pb_$classname$_$name$_get(handle:Byte Ptr, value:$type$ Ptr)\n"
    "Function bmx_pb_$classname$_$name$_set(handle:Byte Ptr, value:$type$)\n");
    
}


void LongFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_ = $default$;\n");
}

void LongFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "set_$name$(from.$name$());\n");
}

void LongFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  printer->Print(variables_, ",\n$name$_($default$)");
}

void LongFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$(\n"
    "      input, &$name$_));\n"
    "_set_bit($index$);\n");
}

void LongFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(), output));\n");
}

void LongFieldGenerator::
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

RepeatedLongFieldGenerator::
RepeatedLongFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetPrimitiveVariables(descriptor, &variables_);
}

RepeatedLongFieldGenerator::~RepeatedLongFieldGenerator() {}

void RepeatedLongFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_,
    "::google::protobuf::RepeatedField< $type$ > $name$_;\n");
}

void RepeatedLongFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const ::google::protobuf::RepeatedField< $type$ >& $name$() const;\n"
    "inline ::google::protobuf::RepeatedField< $type$ >* mutable_$name$();\n"
    "inline $type$ $name$(int index) const;\n"
    "inline void set_$name$(int index, $type$ value);\n"
    "inline void add_$name$($type$ value);\n");
}

void RepeatedLongFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {
}

void RepeatedLongFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const ::google::protobuf::RepeatedField< $type$ >&\n"
    "$classname$::$name$() const {\n"
    "  return $name$_;\n"
    "}\n"
    "inline ::google::protobuf::RepeatedField< $type$ >*\n"
    "$classname$::mutable_$name$() {\n"
    "  return &$name$_;\n"
    "}\n"
    "inline $type$ $classname$::$name$(int index) const {\n"
    "  return $name$_.Get(index);\n"
    "}\n"
    "inline void $classname$::set_$name$(int index, $type$ value) {\n"
    "  $name$_.Set(index, value);\n"
    "}\n"
    "inline void $classname$::add_$name$($type$ value) {\n"
    "  $name$_.Add(value);\n"
    "}\n");
}

void RepeatedLongFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.Clear();\n");
}

void RepeatedLongFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.MergeFrom(from.$name$_);\n");
}

void RepeatedLongFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  // Not needed for repeated fields.
}

void RepeatedLongFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "$type$ value;\n"
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$(input, &value));\n"
    "add_$name$(value);\n");
}

void RepeatedLongFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(i), output));\n");
}

void RepeatedLongFieldGenerator::
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

void RepeatedLongFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
