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

#include <set>
#include <map>

#include <bah/protobuf/compiler/blitzmax/bmx_enum.h>
#include <bah/protobuf/compiler/blitzmax/bmx_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace bmx {

EnumGenerator::EnumGenerator(const EnumDescriptor* descriptor,
                             const string& dllexport_decl)
  : descriptor_(descriptor),
    classname_(ClassName(descriptor, false)),
    dllexport_decl_(dllexport_decl) {
}

EnumGenerator::~EnumGenerator() {}


void EnumGenerator::GenerateDefinition(io::Printer* printer) {

  map<string, string> vars;
  vars["classname"] = classname_;
  vars["short_name"] = descriptor_->name();

//  printer->Print(vars, "enum $classname$ {\n");
//printer->Indent();

  const EnumValueDescriptor* min_value = descriptor_->value(0);
  const EnumValueDescriptor* max_value = descriptor_->value(0);

  for (int i = 0; i < descriptor_->value_count(); i++) {
    vars["name"] = descriptor_->value(i)->name();
    vars["number"] = SimpleItoa(descriptor_->value(i)->number());
    vars["prefix"] = (descriptor_->containing_type() == NULL) ?
      "" : classname_ + "_";

    printer->Print(vars, "Const $prefix$$name$:Int = $number$\n");

    if (descriptor_->value(i)->number() < min_value->number()) {
      min_value = descriptor_->value(i);
    }
    if (descriptor_->value(i)->number() > max_value->number()) {
      max_value = descriptor_->value(i);
    }
  }

  //printer->Outdent();
  printer->Print("\n");

  vars["min_name"] = min_value->name();
  vars["max_name"] = max_value->name();

//  if (dllexport_decl_.empty()) {
  //  vars["dllexport"] = "";
  //} else {
//    vars["dllexport"] = dllexport_decl_ + " ";
//  }

  printer->Print(vars,
    //"$dllexport$const ::google::protobuf::EnumDescriptor* $classname$_descriptor();\n"
    "Function $classname$_IsValid:Int(value:Int)\n"
    "\tSelect value\n"
    "\t\tCase ");
  
  // Multiple values may have the same number.  Make sure we only cover
  // each number once by first constructing a set containing all valid
  // numbers, then printing a case statement for each element.

  set<int> numbers;
  for (int j = 0; j < descriptor_->value_count(); j++) {
    const EnumValueDescriptor* value = descriptor_->value(j);
    numbers.insert(value->number());
  }

  int count = 0;
  for (set<int>::iterator iter = numbers.begin();
       iter != numbers.end(); ++iter) {
    if (count++ > 0) {
    	printer->Print(", ");
	}

    printer->Print(
      "$number$",
      "number", SimpleItoa(*iter));
  }

  printer->Print(vars,
  	 "\n"
    "\t\t\tReturn True\n"
    "\t\tDefault\n"
    "\t\t\tReturn False\n"
    "\tEnd Select\n"
    "End Function\n"
    "\n");

  printer->Print(vars,
    "Const $prefix$$short_name$_MIN:Int = $prefix$$min_name$\n"
    "Const $prefix$$short_name$_MAX:Int = $prefix$$max_name$\n"
    "\n");
}

void EnumGenerator::GenerateSymbolImports(io::Printer* printer) {
  map<string, string> vars;
  vars["nested_name"] = descriptor_->name();
  vars["classname"] = classname_;
  //printer->Print(vars, "typedef $classname$ $nested_name$;\n");

  for (int j = 0; j < descriptor_->value_count(); j++) {
    vars["tag"] = descriptor_->value(j)->name();
    printer->Print(vars,
      "Const _$tag$:Int = $classname$_$tag$\n");
  }

  printer->Print(vars,
    "Function $nested_name$_IsValid:Int(value:Int)\n"
    "  Return $classname$_IsValid:Int(value)\n"
    "End Function\n"
    "\n"
    "Const $nested_name$_MIN:Int = $classname$_$nested_name$_MIN\n"
    "Const $nested_name$_MAX:Int = $classname$_$nested_name$_MAX\n"
    "\n");
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
