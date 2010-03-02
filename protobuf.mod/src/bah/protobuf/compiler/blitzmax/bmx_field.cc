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

#include <bah/protobuf/compiler/blitzmax/bmx_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_primitive_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_string_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_enum_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_message_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_long_field.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

using internal::WireFormat;

void SetCommonFieldVariables(const FieldDescriptor* descriptor,
                             map<string, string>* variables) {
  (*variables)["name"] = FieldName(descriptor);
  (*variables)["index"] = SimpleItoa(descriptor->index());
  (*variables)["number"] = SimpleItoa(descriptor->number());
  (*variables)["classname"] = ClassName(FieldScope(descriptor), false);
  (*variables)["declared_type"] = DeclaredTypeMethodName(descriptor->type());

  (*variables)["uname"] = UnderscoresToCapitalizedCamelCase(descriptor);
  (*variables)["lname"] = FieldName(descriptor);
//  (*variables)["gtype"] = PrimitiveGlueTypeName(descriptor->cpp_type());
  (*variables)["gtype"] = descriptor->type();


  (*variables)["tag_size"] = SimpleItoa(
    WireFormat::TagSize(descriptor->number(), descriptor->type()));
  (*variables)["deprecation"] = descriptor->options().deprecated()
      ? " PROTOBUF_DEPRECATED" : "";

}

FieldGenerator::~FieldGenerator() {}

FieldGeneratorMap::FieldGeneratorMap(const Descriptor* descriptor)
  : descriptor_(descriptor),
    field_generators_(
      new scoped_ptr<FieldGenerator>[descriptor->field_count()]) {
  // Construct all the FieldGenerators.
  for (int i = 0; i < descriptor->field_count(); i++) {
    field_generators_[i].reset(MakeGenerator(descriptor->field(i)));
  }
}

FieldGenerator* FieldGeneratorMap::MakeGenerator(const FieldDescriptor* field) {
  if (field->is_repeated()) {
    switch (field->cpp_type()) {
      case FieldDescriptor::CPPTYPE_MESSAGE:
        return new RepeatedMessageFieldGenerator(field);
      case FieldDescriptor::CPPTYPE_STRING:
          return new RepeatedStringFieldGenerator(field);
      case FieldDescriptor::CPPTYPE_ENUM:
        return new RepeatedEnumFieldGenerator(field);
      default:
        if (MaxType(field) == 2) {
	        return new RepeatedLongFieldGenerator(field);
        } else {
	        return new RepeatedPrimitiveFieldGenerator(field);
	    }
    }
  } else {
    switch (field->cpp_type()) {
      case FieldDescriptor::CPPTYPE_MESSAGE:
        return new MessageFieldGenerator(field);
      case FieldDescriptor::CPPTYPE_STRING:
          return new StringFieldGenerator(field);
      case FieldDescriptor::CPPTYPE_ENUM:
        return new EnumFieldGenerator(field);
      default:
        if (MaxType(field) == 2) {
	        return new LongFieldGenerator(field);
        } else {
	        return new PrimitiveFieldGenerator(field);
        }
    }
  }
}

FieldGeneratorMap::~FieldGeneratorMap() {}

const FieldGenerator& FieldGeneratorMap::get(
    const FieldDescriptor* field) const {
  GOOGLE_CHECK_EQ(field->containing_type(), descriptor_);
  return *field_generators_[field->index()];
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
