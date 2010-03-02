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

#include <bah/protobuf/compiler/blitzmax/bmx_message_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

//using internal::WireFormat;

namespace {

// TODO(kenton):  Factor out a "SetCommonFieldVariables()" to get rid of
//   repeat code between this and the other field types.
void SetMessageVariables(const FieldDescriptor* descriptor,
                         map<string, string>* variables) {
/*  (*variables)["name"] = FieldName(descriptor);
  (*variables)["type"] = ClassName(descriptor->message_type(), true);
  (*variables)["index"] = SimpleItoa(descriptor->index());
  (*variables)["number"] = SimpleItoa(descriptor->number());
  (*variables)["classname"] = ClassName(FieldScope(descriptor), false);
  (*variables)["declared_type"] = DeclaredTypeMethodName(descriptor->type());
  (*variables)["tag_size"] = SimpleItoa(
    WireFormat::TagSize(descriptor->number(), descriptor->type()));*/
  SetCommonFieldVariables(descriptor, variables);
  (*variables)["type"] = FieldMessageTypeName(descriptor);
  //(*variables)["gtype"] = PrimitiveGlueTypeName(descriptor->cpp_type());
  (*variables)["gtype"] = "XX";
  (*variables)["number"] = SimpleItoa(descriptor->number());
  (*variables)["declared_type"] = DeclaredTypeMethodName(descriptor->type());
 (*variables)["stream_writer"] = (*variables)["declared_type"] +
      (HasFastArraySerialization(descriptor->message_type()->file()) ?
       "MaybeToArray" :
       "");
}

}  // namespace

// ===================================================================

MessageFieldGenerator::
MessageFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetMessageVariables(descriptor, &variables_);
}

MessageFieldGenerator::~MessageFieldGenerator() {}

void MessageFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_, "$type$* $name$_;\n");
}

void MessageFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const $type$& $name$() const;\n"
    "inline $type$* mutable_$name$();\n");
}

void MessageFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const $type$& $name$() const;\n"
    "inline $type$* mutable_$name$();\n");
}

void MessageFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    "inline const $type$& $classname$::$name$() const {\n"
    "  return $name$_ != NULL ? *$name$_ : *default_instance_.$name$_;\n"
    "}\n"
    "inline $type$* $classname$::mutable_$name$() {\n"
    "  _set_bit($index$);\n"
    "  if ($name$_ == NULL) $name$_ = new $type$;\n"
    "  return $name$_;\n"
    "}\n");
}

void MessageFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {
}

void MessageFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_,
    "if ($name$_ != NULL) $name$_->$type$::Clear();\n");
}

void MessageFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_,
    "mutable_$name$()->$type$::MergeFrom(from.$name$());\n");
}

void MessageFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  printer->Print(variables_, ",\n$name$_(NULL)");
}

void MessageFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  if (descriptor_->type() == FieldDescriptor::TYPE_MESSAGE) {
    printer->Print(variables_,
      "DO_(::google::protobuf::internal::WireFormat::ReadMessageNoVirtual(\n"
      "     input, mutable_$name$()));\n");
  } else {
    printer->Print(variables_,
      "DO_(::google::protobuf::internal::WireFormat::ReadGroupNoVirtual("
        "$number$, input, mutable_$name$()));\n");
  }
}

void MessageFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$NoVirtual("
      "$number$, this->$name$(), output));\n");
}

void MessageFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  printer->Print(variables_,
    "total_size += $tag_size$ +\n"
    "  ::google::protobuf::internal::WireFormat::$declared_type$SizeNoVirtual(\n"
    "    this->$name$());\n");
}

// ===================================================================

RepeatedMessageFieldGenerator::
RepeatedMessageFieldGenerator(const FieldDescriptor* descriptor)
  : descriptor_(descriptor) {
  SetMessageVariables(descriptor, &variables_);
}

RepeatedMessageFieldGenerator::~RepeatedMessageFieldGenerator() {}

void RepeatedMessageFieldGenerator::
GeneratePrivateMembers(io::Printer* printer) const {
  printer->Print(variables_,
    "::google::protobuf::RepeatedPtrField< $type$ > $name$_;\n");
}

void RepeatedMessageFieldGenerator::
GenerateAccessorDeclarations(io::Printer* printer) const {
  printer->Print(variables_,
    "// TODO : inline const ::google::protobuf::RepeatedPtrField< $type$ >& $name$() const;\n"
    "// TODO : inline ::google::protobuf::RepeatedPtrField< $type$ >* mutable_$name$();\n"
    "// TODO : inline const $type$& $name$(int index) const;\n"
    "// TODO : inline $type$* mutable_$name$(int index);\n"
    "// TODO : inline $type$* add_$name$();\n");

  printer->Print(variables_,
    "$type$ * bmx_bp_$classname$_$name$_Get($classname$ * handle, int index);\n"
    "$type$ * bmx_bp_$classname$_$name$_Add($classname$ * handle);\n");
}

void RepeatedMessageFieldGenerator::
GenerateAccessorGlue(io::Printer* printer) const {

  printer->Print(variables_,
    "$type$ * bmx_bp_$classname$_$name$_Get($classname$ * handle, int index) {\n"
    "  return handle->mutable_$name$(index);\n"
    "}\n"
    "\n"
    "$type$ * bmx_bp_$classname$_$name$_Add($classname$ * handle) {\n"
    "  return handle->add_$name$();\n"
    "}\n"
    "\n");

}

void RepeatedMessageFieldGenerator::
GenerateAccessorDefinitions(io::Printer* printer) const {
  printer->Print(variables_,
    //"inline const ::google::protobuf::RepeatedPtrField< $type$ >&\n"
    //"$classname$::$name$() const {\n"
    //"  return $name$_;\n"
    //"}\n"
    //"inline ::google::protobuf::RepeatedPtrField< $type$ >*\n"
    //"$classname$::mutable_$name$() {\n"
    //"  return &$name$_;\n"
    //"}\n"
    "Method Get$name$:$type$(index:Int)\n"
    "  return $type$._create(bmx_bp_$classname$_$name$_Get(messagePtr, index))\n"
    "End Method\n"
    "\n"
    //"Method $type$* $classname$::mutable_$name$(int index) {\n"
    //"  return $name$_.Mutable(index);\n"
    //"End Method\n"
    //"\n"
    "Method Add$name$:$type$()\n"
    "  Return $type$._create(bmx_bp_$classname$_$name$_Add(messagePtr))\n"
    "End Method\n"
    "\n");
}

void RepeatedMessageFieldGenerator::
GenerateClearingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.Clear();\n");
}

void RepeatedMessageFieldGenerator::
GenerateMergingCode(io::Printer* printer) const {
  printer->Print(variables_, "$name$_.MergeFrom(from.$name$_);\n");
}

void RepeatedMessageFieldGenerator::
GenerateInitializer(io::Printer* printer) const {
  // Not needed for repeated fields.
}

void RepeatedMessageFieldGenerator::
GenerateMergeFromCodedStream(io::Printer* printer) const {
  if (descriptor_->type() == FieldDescriptor::TYPE_MESSAGE) {
    printer->Print(variables_,
      "DO_(::google::protobuf::internal::WireFormat::ReadMessageNoVirtual(\n"
      "     input, add_$name$()));\n");
  } else {
    printer->Print(variables_,
      "DO_(::google::protobuf::internal::WireFormat::ReadGroupNoVirtual("
        "$number$, input, add_$name$()));\n");
  }
}

void RepeatedMessageFieldGenerator::
GenerateSerializeWithCachedSizes(io::Printer* printer) const {
  printer->Print(variables_,
    "DO_(::google::protobuf::internal::WireFormat::Write$declared_type$NoVirtual("
      "$number$, this->$name$(i), output));\n");
}

void RepeatedMessageFieldGenerator::
GenerateByteSize(io::Printer* printer) const {
  printer->Print(variables_,
    "total_size += $tag_size$ * $name$_size();\n"
    "for (int i = 0; i < $name$_size(); i++) {\n"
    "  total_size +=\n"
    "    ::google::protobuf::internal::WireFormat::$declared_type$SizeNoVirtual(\n"
    "      this->$name$(i));\n"
    "}\n");
}

void RepeatedMessageFieldGenerator::
GenerateAccessorExterns(io::Printer* printer) const {

  printer->Print(variables_,
    "Function bmx_bp_$classname$_$name$_Get:Byte Ptr(handle:Byte Ptr, index:Int)\n"
    "Function bmx_bp_$classname$_$name$_Add:Byte Ptr(handle:Byte Ptr)\n");

}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
