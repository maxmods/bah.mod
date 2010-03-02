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

#include <bah/protobuf/compiler/blitzmax/bmx_string_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

namespace {

void SetStringVariables(const FieldDescriptor* descriptor,
                        map<string, string>* variables) {
  SetCommonFieldVariables(descriptor, variables);
  (*variables)["default"] =
    "\"" + CEscape(descriptor->default_value_string()) + "\"";
  (*variables)["pointer_type"] =
      descriptor->type() == FieldDescriptor::TYPE_BYTES ? "void" : "char";
}

}  // namespace

// ===================================================================

StringFieldGenerator::
StringFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetStringVariables(descriptor, &variables_);
}

StringFieldGenerator::~StringFieldGenerator() {}

void StringFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_,
    "::std::string* $name$_;\n"
    "static const ::std::string _default_$name$_;\n");
}

void StringFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {

  printer->Print(variables_,
    "BBString * bmx_pb_$classname$_$name$_get($classname$ * handle);\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, BBString * value);\n");

}

void StringFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {
  printer->Print(variables_,
    "BBString * bmx_pb_$classname$_$name$_get($classname$ * handle) {\n"
    "  std::string s = handle->$name$();\n"
    "  return bah_protobuf__pbConvertUTF8ToMax(s.c_str(), s.length());\n"
    "}\n"
    "\n"
    "void bmx_pb_$classname$_$name$_set($classname$ * handle, BBString * value) {\n"
    "  char *p = bbStringToCString( value );\n"
    "  handle->set_$name$(p);\n"
    "  bbMemFree(p);\n"
    "}\n"
    "\n");
}


void StringFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "Method Get$uname$:String()\n"
    "\tReturn bmx_pb_$classname$_$name$_get(messagePtr);\n"
    "End Method\n"
    "\n"
    "Method Set$uname$(value:String)\n"
    "\tbmx_pb_$classname$_$name$_set(messagePtr, value)\n"
    "End Method\n"
    "\n");
}

void StringFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
  printer->Print(variables_,
    "Function bmx_pb_$classname$_$name$_get:String(handle:byte ptr)\n"
    "Function bmx_pb_$classname$_$name$_set(handle:byte ptr, value:String)\n");
}

void StringFieldGenerator::
GenerateNonInlineAccessorDefinitions(io::Printer* printer) const {
  if (descriptor_->has_default_value()) {
    printer->Print(variables_,
      "const ::std::string $classname$::_default_$name$_($default$);");
  } else {
    printer->Print(variables_,
      "const ::std::string $classname$::_default_$name$_;");
  }
}

void StringFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  if (descriptor_->has_default_value()) {
    printer->Print(variables_,
      "if ($name$_ != &_default_$name$_) {\n"
      "  $name$_->assign(_default_$name$_);\n"
      "}\n");
  } else {
    printer->Print(variables_,
      "if ($name$_ != &_default_$name$_) {\n"
      "  $name$_->clear();\n"
      "}\n");
  }
}

void StringFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "set_$name$(from.$name$());\n");
}

void StringFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  printer->Print(variables_,
    ",\n$name$_(const_cast< ::std::string*>(&_default_$name$_))");
}

void StringFieldGenerator::
GenerateDestructorCode(io::Printer* printer) const {
  printer->Print(variables_,
    "if ($name$_ != &_default_$name$_) {\n"
    "  delete $name$_;\n"
    "}\n");
}

void StringFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$("
      "input, mutable_$name$()));\n");
}

void StringFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(), output));\n");
}

void StringFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  printer->Print(variables_,
    "total_size += $tag_size$ +\n"
    "  ::google::protobuf::internal::WireFormat::$declared_type$Size(this->$name$());\n");
}

// ===================================================================

RepeatedStringFieldGenerator::
RepeatedStringFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetStringVariables(descriptor, &variables_);
}

RepeatedStringFieldGenerator::~RepeatedStringFieldGenerator() {}

void RepeatedStringFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_,
    "::google::protobuf::RepeatedPtrField< ::std::string> $name$_;\n");
}

void RepeatedStringFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  // See comment above about unknown ctypes.
  if (descriptor_->options().has_ctype()) {
    printer->Outdent();
    printer->Print(
      " private:\n"
      "  // Hidden due to unknown ctype option.\n");
    printer->Indent();
  }

  printer->Print(variables_,
    "inline const ::google::protobuf::RepeatedPtrField< ::std::string>& $name$() const;\n"
    "inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_$name$();\n"
    "inline const ::std::string& $name$(int index) const;\n"
    "inline ::std::string* mutable_$name$(int index);\n"
    "inline void set_$name$(int index, const ::std::string& value);\n"
    "inline void set_$name$(int index, const char* value);\n"
    "inline ::std::string* add_$name$();\n"
    "inline void add_$name$(const ::std::string& value);\n"
    "inline void add_$name$(const char* value);\n");

  if (descriptor_->options().has_ctype()) {
    printer->Outdent();
    printer->Print(" public:\n");
    printer->Indent();
  }
}

void RepeatedStringFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const ::google::protobuf::RepeatedPtrField< ::std::string>&\n"
    "$classname$::$name$() const {\n"
    "  return $name$_;\n"
    "}\n"
    "inline ::google::protobuf::RepeatedPtrField< ::std::string>*\n"
    "$classname$::mutable_$name$() {\n"
    "  return &$name$_;\n"
    "}\n"
    "inline const ::std::string& $classname$::$name$(int index) const {\n"
    "  return $name$_.Get(index);\n"
    "}\n"
    "inline ::std::string* $classname$::mutable_$name$(int index) {\n"
    "  return $name$_.Mutable(index);\n"
    "}\n"
    "inline void $classname$::set_$name$(int index, const ::std::string& value) {\n"
    "  $name$_.Mutable(index)->assign(value);\n"
    "}\n"
    "inline void $classname$::set_$name$(int index, const char* value) {\n"
    "  $name$_.Mutable(index)->assign(value);\n"
    "}\n"
    "inline ::std::string* $classname$::add_$name$() {\n"
    "  return $name$_.Add();\n"
    "}\n"
    "inline void $classname$::add_$name$(const ::std::string& value) {\n"
    "  $name$_.Add()->assign(value);\n"
    "}\n"
    "inline void $classname$::add_$name$(const char* value) {\n"
    "  $name$_.Add()->assign(value);\n"
    "}\n");
}

void RepeatedStringFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {
}

void RepeatedStringFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.Clear();\n");
}

void RepeatedStringFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.MergeFrom(from.$name$_);\n");
}

void RepeatedStringFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  // Not needed for repeated fields.
}

void RepeatedStringFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Read$declared_type$(\n"
    "     input, add_$name$()));\n");
}

void RepeatedStringFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$("
      "$number$, this->$name$(i), output));\n");
}

void RepeatedStringFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  printer->Print(variables_,
    "total_size += $tag_size$ * $name$_size();\n"
    "for (int i = 0; i < $name$_size(); i++) {\n"
    "  total_size += ::google::protobuf::internal::WireFormat::$declared_type$Size(\n"
    "    this->$name$(i));\n"
    "}\n");
}

void RepeatedStringFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
