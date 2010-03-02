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

#include <algorithm>
#include <google/protobuf/stubs/hash.h>
#include <bah/protobuf/compiler/blitzmax/bmx_message.h>
#include <bah/protobuf/compiler/blitzmax/bmx_enum.h>
#include <bah/protobuf/compiler/blitzmax/bmx_extension.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

using internal::WireFormat;

namespace {

void PrintFieldComment(io::Printer* printer, const FieldDescriptor* field) {
  // Print the field's proto-syntax definition as a comment.  We don't want to
  // print group bodies so we cut off after the first line.
  string def = field->DebugString();
  printer->Print("' $def$\n",
    "def", def.substr(0, def.find_first_of('\n')));
}

struct FieldOrderingByNumber {
  inline bool operator()(const FieldDescriptor* a,
                         const FieldDescriptor* b) const {
    return a->number() < b->number();
  }
};

const char* kWireTypeNames[] = {
  "VARINT",
  "FIXED64",
  "LENGTH_DELIMITED",
  "START_GROUP",
  "END_GROUP",
  "FIXED32",
};

// Sort the fields of the given Descriptor by number into a new[]'d array
// and return it.
const FieldDescriptor** SortFieldsByNumber(const Descriptor* descriptor) {
  const FieldDescriptor** fields =
    new const FieldDescriptor*[descriptor->field_count()];
  for (int i = 0; i < descriptor->field_count(); i++) {
    fields[i] = descriptor->field(i);
  }
  sort(fields, fields + descriptor->field_count(),
       FieldOrderingByNumber());
  return fields;
}

// Functor for sorting extension ranges by their "start" field number.
struct ExtensionRangeSorter {
  bool operator()(const Descriptor::ExtensionRange* left,
                  const Descriptor::ExtensionRange* right) const {
    return left->start < right->start;
  }
};

// Returns true if the message type has any required fields.  If it doesn't,
// we can optimize out calls to its IsInitialized() method.
//
// already_seen is used to avoid checking the same type multiple times
// (and also to protect against recursion).
static bool HasRequiredFields(
    const Descriptor* type,
    hash_set<const Descriptor*>* already_seen) {
  if (already_seen->count(type) > 0) {
    // Since the first occurrence of a required field causes the whole
    // function to return true, we can assume that if the type is already
    // in the cache it didn't have any required fields.
    return false;
  }
  already_seen->insert(type);

  // If the type has extensions, an extension with message type could contain
  // required fields, so we have to be conservative and assume such an
  // extension exists.
  if (type->extension_range_count() > 0) return true;

  for (int i = 0; i < type->field_count(); i++) {
    const FieldDescriptor* field = type->field(i);
    if (field->is_required()) {
      return true;
    }
    if (field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      if (HasRequiredFields(field->message_type(), already_seen)) {
        return true;
      }
    }
  }

  return false;
}

static bool HasRequiredFields(const Descriptor* type) {
  hash_set<const Descriptor*> already_seen;
  return HasRequiredFields(type, &already_seen);
}

}

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor,
                                   const string& dllexport_decl)
  : descriptor_(descriptor),
    classname_(ClassName(descriptor, false)),
    dllexport_decl_(dllexport_decl),
    field_generators_(descriptor),
    nested_generators_(new scoped_ptr<MessageGenerator>[
      descriptor->nested_type_count()]),
    enum_generators_(new scoped_ptr<EnumGenerator>[
      descriptor->enum_type_count()]),
    extension_generators_(new scoped_ptr<ExtensionGenerator>[
      descriptor->extension_count()]) {

  for (int i = 0; i < descriptor->nested_type_count(); i++) {
    nested_generators_[i].reset(
      new MessageGenerator(descriptor->nested_type(i), dllexport_decl));
  }

  for (int i = 0; i < descriptor->enum_type_count(); i++) {
    enum_generators_[i].reset(
      new EnumGenerator(descriptor->enum_type(i), dllexport_decl));
  }

  for (int i = 0; i < descriptor->extension_count(); i++) {
    extension_generators_[i].reset(
      new ExtensionGenerator(descriptor->extension(i), dllexport_decl));
  }
}

MessageGenerator::~MessageGenerator() {}

void MessageGenerator::GenerateType(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateType(printer);
    printer->Print("\n");
    printer->Print(kThinSeparator);
    printer->Print("\n");
  }

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["field_count"] = SimpleItoa(descriptor_->field_count());

  printer->Print(vars,
    "Type $classname$ Extends TMessage\n"
    "\n");

  printer->Indent();

  printer->Print(vars,
    "\n"
    "Function _create:$classname$(messagePtr:Byte Ptr)\n"
	"\tIf messagePtr Then\n"
	"\t\tLocal this:$classname$ = new $classname$\n"
	"\t\tthis.messagePtr = messagePtr\n"
	"\t\tReturn this\n"
	"\tEnd If\n"
	"End Function\n"
	"\n"
	"Function Create$classname$:$classname$()\n"
	"\tReturn New $classname$.Create()\n"
	"End Function\n"
	"\n"
	"Method Create:$classname$()\n"
	"\tmessagePtr = bmx_pb_$classname$_create()\n"
	"\towner = True\n"
	"\tReturn Self\n"
	"End Method\n"
	"\n"
    "' implements Message ----------------------------------------------\n"
    "\n"
    //"$classname$* New() const;\n"
    );

/*  if (descriptor_->file()->options().optimize_for() == FileOptions::SPEED) {
    printer->Print(vars,
      "void CopyFrom(const ::google::protobuf::Message& from);\n"
      "void MergeFrom(const ::google::protobuf::Message& from);\n"
      "void CopyFrom(const $classname$& from);\n"
      "void MergeFrom(const $classname$& from);\n"
      "void Clear();\n"
      "bool IsInitialized() const;\n"
      "int ByteSize() const;\n"
      "\n"
      "bool MergePartialFromCodedStream(\n"
      "    ::google::protobuf::io::CodedInputStream* input);\n"
      "bool SerializeWithCachedSizes(\n"
      "    ::google::protobuf::io::CodedOutputStream* output) const;\n");
  } */

  printer->Print(vars,
    "Method GetCachedSize:Int()\n"
    "\tReturn bmx_pb_$classname$_cached_size(messagePtr)\n"
    "End Method\n"
    "\n"
    "Method GetDescriptor:TDescriptor()\n"
    "\tReturn TDescriptor._create(bmx_pb_$classname$_getdescriptor(messagePtr))\n"
    "End Method\n"
    "\n"
    "Method GetReflection:TReflection()\n"
    "\tReturn TReflection._create(bmx_pb_$classname$_getreflection(messagePtr))\n"
    "End Method\n"
    "\n"
//    "::google::protobuf::Message::Reflection* GetReflection();\n"
    "\n"
    "' nested types ----------------------------------------------------\n"
    "\n");

  // Import all nested message classes into this class's scope with typedefs.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    const Descriptor* nested_type = descriptor_->nested_type(i);
    printer->Print("Field $nested_name$:$nested_full_name$\n",
                   "nested_name", nested_type->name(),
                   "nested_full_name", ClassName(nested_type, false));
  }

  if (descriptor_->nested_type_count() > 0) {
    printer->Print("\n");
  }

  // Import all nested enums and their values into this class's scope with
  // typedefs and constants.
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateSymbolImports(printer);
    printer->Print("\n");
  }

  printer->Print(
    "' accessors -------------------------------------------------------\n"
    "\n");

  // Generate accessor methods for all fields.
  GenerateFieldAccessorDefinitions(printer);

  // Declare extension identifiers.
  //for (int i = 0; i < descriptor_->extension_count(); i++) {
  //  extension_generators_[i]->GenerateDeclaration(printer);
  //}

  // Generate private members for fields.
//  printer->Outdent();
//  printer->Print(" private:\n");
//  printer->Indent();

  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
      "::google::protobuf::internal::ExtensionSet _extensions_;\n");
  }


  //for (int i = 0; i < descriptor_->field_count(); i++) {
  //  field_generators_.get(descriptor_->field(i))
  //                   .GeneratePrivateMembers(printer);
  //}


  printer->Outdent();
  printer->Print(vars, "\n"
  "End Type\n"
  "\n"
  "\n");
}

void MessageGenerator::
GenerateEnumDefinitions(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateEnumDefinitions(printer);
  }

  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    enum_generators_[i]->GenerateDefinition(printer);
  }
}

void MessageGenerator::
GenerateFieldAccessorDefinitions(io::Printer* printer) {
  printer->Print("' $classname$\n\n", "classname", classname_);

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    PrintFieldComment(printer, field);

    map<string, string> vars;
    SetCommonFieldVariables(field, &vars);
    
//    vars["uname"] = UnderscoresToCapitalizedCamelCase(field);
//    vars["lname"] = FieldName(field);
//    vars["index"] = SimpleItoa(field->index());
//    vars["classname"] = classname_;

    // Generate has_$name$() or $name$_size().
    if (field->is_repeated()) {
      printer->Print(vars,
        "\tMethod $uname$Size:Int()\n"
        "\t\tReturn bmx_pb_$classname$_$lname$_size(messagePtr)\n"
        "\tEnd Method\n"
        "\n");
    } else {
      // Singular field.
      printer->Print(vars,
        "\tMethod Has$uname$:Int()\n"
        "\t\tReturn bmx_pb_$classname$_has_$lname$(messagePtr)\n"
        "\tEnd Method\n"
        "\n");
    }

    // Generate clear_$name$()
    printer->Print(vars,
      "\tMethod Clear$uname$()\n");

    printer->Indent();
//    field_generators_.get(field).GenerateClearingCode(printer);
    printer->Outdent();

    if (!field->is_repeated()) {
      printer->Print(vars, "\t\tbmx_pb_$classname$_clear_$lname$(messagePtr)\n");
    }

    printer->Print("\tEnd Method\n"
    "\n");

    // Generate type-specific accessors.
    field_generators_.get(field).GenerateAccessorDefinitions(printer);

    printer->Print("\n");
  }
}

void MessageGenerator::
GenerateFieldAccessorExterns(io::Printer* printer) {

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    map<string, string> vars;
    vars["uname"] = UnderscoresToCapitalizedCamelCase(field);
    vars["lname"] = FieldName(field);
    vars["index"] = SimpleItoa(field->index());
    vars["classname"] = classname_;

    // Generate has_$name$() or $name$_size().
    if (field->is_repeated()) {
      printer->Print(vars,
        "Function bmx_pb_$classname$_$lname$_size:Int(handle:Byte Ptr)\n");
    } else {
      // Singular field.
      printer->Print(vars,
        "Function bmx_pb_$classname$_has_$lname$:Int(handle:Byte Ptr)\n");
    }

    if (!field->is_repeated()) {
      printer->Print(vars, "Function bmx_pb_$classname$_clear_$lname$(handle:Byte Ptr)\n");
    }

    // Generate type-specific accessors.
    field_generators_.get(field).GenerateAccessorExterns(printer);

  }

}


void MessageGenerator::
GenerateInlineMethods(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateInlineMethods(printer);
    printer->Print(kThinSeparator);
    printer->Print("\n");
  }

  GenerateFieldAccessorDefinitions(printer);
}

void MessageGenerator::
GenerateExterns(io::Printer* printer) {
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateExterns(printer);
  }

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["field_count"] = SimpleItoa(descriptor_->field_count());

  printer->Print(vars,
    "Function bmx_pb_$classname$_create:Byte Ptr()\n"
    "Function bmx_pb_$classname$_cached_size:Int(handle:Byte Ptr)\n"
    "Function bmx_pb_$classname$_getdescriptor:Byte Ptr(handle:Byte Ptr)\n"
    "Function bmx_pb_$classname$_getreflection:Byte Ptr(handle:Byte Ptr)\n");
  
  GenerateFieldAccessorExterns(printer);
  
}


void MessageGenerator::
GenerateEnumExterns(io::Printer* printer) {
}

void MessageGenerator::
GenerateGlue(io::Printer* printer) {

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateGlue(printer);
  }

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["field_count"] = SimpleItoa(descriptor_->field_count());

  printer->Print(vars,
    "$classname$ * bmx_pb_$classname$_create() {\n"
    "  return new $classname$;\n"
    "}\n"
    "\n"
    "int bmx_pb_$classname$_cached_size($classname$ * handle) {\n"
	"  return handle->GetCachedSize();\n"
	"}\n"
	"\n"
	"const ::google::protobuf::Descriptor * bmx_pb_$classname$_getdescriptor($classname$ * handle) {\n"
	"  return handle->GetDescriptor();\n"
	"}\n"
	"\n"
    "const ::google::protobuf::Message::Reflection * bmx_pb_$classname$_getreflection($classname$ * handle) {\n"
    "  return handle->GetReflection();\n"
    "}\n"
	"\n");

  GenerateFieldAccessorGlue(printer);

}

void MessageGenerator::
GenerateDeclarations(io::Printer* printer) {

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    nested_generators_[i]->GenerateDeclarations(printer);
  }

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["field_count"] = SimpleItoa(descriptor_->field_count());

  printer->Print(vars,
    "$classname$ * bmx_pb_$classname$_create();\n"
    "int bmx_pb_$classname$_cached_size($classname$ * handle);\n"
    "const ::google::protobuf::Descriptor * bmx_pb_$classname$_getdescriptor($classname$ * handle);\n"
    "const ::google::protobuf::Message::Reflection * bmx_pb_$classname$_getreflection($classname$ * handle);\n");
  
  GenerateFieldAccessorDeclarations(printer);

}

void MessageGenerator::
GenerateFieldAccessorDeclarations(io::Printer* printer) {

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    map<string, string> vars;
    vars["uname"] = UnderscoresToCapitalizedCamelCase(field);
    vars["lname"] = FieldName(field);
    vars["index"] = SimpleItoa(field->index());
    vars["classname"] = classname_;

    // Generate has_$name$() or $name$_size().
    if (field->is_repeated()) {
      printer->Print(vars,
        "int bmx_pb_$classname$_$lname$_size($classname$ * handle);\n");
    } else {
      // Singular field.
      printer->Print(vars,
        "bool bmx_pb_$classname$_has_$lname$($classname$ * handle);\n");
    }

    if (!field->is_repeated()) {
      printer->Print(vars, "void bmx_pb_$classname$_clear_$lname$($classname$ * handle);\n");
    }

    // Generate type-specific accessors.
    field_generators_.get(field).GenerateAccessorDeclarations(printer);

  }

   map<string, string> vars;
   vars["classname"] = classname_;


}

void MessageGenerator::
GenerateFieldAccessorGlue(io::Printer* printer) {

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);

    map<string, string> vars;
    vars["uname"] = UnderscoresToCapitalizedCamelCase(field);
    vars["lname"] = FieldName(field);
    vars["index"] = SimpleItoa(field->index());
    vars["classname"] = classname_;

    // Generate has_$name$() or $name$_size().
    if (field->is_repeated()) {
      printer->Print(vars,
        "int bmx_pb_$classname$_$lname$_size($classname$ * handle) {\n"
        "  return handle->$lname$_size();\n"
        "}\n"
        "\n");
    } else {
      // Singular field.
      printer->Print(vars,
        "bool bmx_pb_$classname$_has_$lname$($classname$ * handle) {\n"
      "  return handle->has_$lname$();\n"
      "}\n"
      "\n");
    }

    if (!field->is_repeated()) {
      printer->Print(vars, "void bmx_pb_$classname$_clear_$lname$($classname$ * handle) {\n"
      "  handle->clear_$lname$();\n"
      "}\n"
      "\n");
    }


    // Generate type-specific accessors.
    field_generators_.get(field).GenerateAccessorGlue(printer);

  }

}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
