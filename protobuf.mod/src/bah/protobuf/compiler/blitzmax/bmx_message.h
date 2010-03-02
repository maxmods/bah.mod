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

#ifndef BAH_PROTOBUF_COMPILER_BMX_MESSAGE_H__
#define BAH_PROTOBUF_COMPILER_BMX_MESSAGE_H__

#include <string>
#include <google/protobuf/stubs/common.h>
#include <bah/protobuf/compiler/blitzmax/bmx_field.h>

namespace google {
namespace protobuf {
  namespace io {
    class Printer;             // printer.h
  }
}

namespace protobuf {
namespace compiler {
namespace bmx {

class EnumGenerator;           // enum.h
class ExtensionGenerator;      // extension.h

class MessageGenerator {
 public:
  // See generator.cc for the meaning of dllexport_decl.
  explicit MessageGenerator(const Descriptor* descriptor,
                            const string& dllexport_decl);
  ~MessageGenerator();

  void GenerateType(io::Printer* printer); 
  void GenerateEnumDefinitions(io::Printer* printer);
  void GenerateEnumExterns(io::Printer* printer);
  void GenerateFieldAccessorDefinitions(io::Printer* printer);
  void GenerateInlineMethods(io::Printer* printer);
  void GenerateFieldAccessorExterns(io::Printer* printer);
  void GenerateExterns(io::Printer* printer); 
  void GenerateDeclarations(io::Printer* printer); 
  void GenerateGlue(io::Printer* printer); 
  void GenerateFieldAccessorDeclarations(io::Printer* printer);
  void GenerateFieldAccessorGlue(io::Printer* printer);
  
 private:

  const Descriptor* descriptor_;
  string classname_;
  string dllexport_decl_;
  FieldGeneratorMap field_generators_;
  scoped_array<scoped_ptr<MessageGenerator> > nested_generators_;
  scoped_array<scoped_ptr<EnumGenerator> > enum_generators_;
  scoped_array<scoped_ptr<ExtensionGenerator> > extension_generators_;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MessageGenerator);
};

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // BAH_PROTOBUF_COMPILER_BMX_MESSAGE_H__
