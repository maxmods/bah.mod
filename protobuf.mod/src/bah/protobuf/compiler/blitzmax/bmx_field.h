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

#ifndef BAH_PROTOBUF_COMPILER_BMX_FIELD_H__
#define BAH_PROTOBUF_COMPILER_BMX_FIELD_H__

#include <map>
#include <string>


#include <google/protobuf/stubs/common.h>
#include <google/protobuf/descriptor.h>

namespace google {
namespace protobuf {
  namespace io {
    class Printer;             // printer.h
  }
}

namespace protobuf {
namespace compiler {
namespace bmx {

// Helper function: set variables in the map that are the same for all
// field code generators.
// ['name', 'index', 'number', 'classname', 'declared_type', 'tag_size',
// 'deprecation'].
void SetCommonFieldVariables(const FieldDescriptor* descriptor,
                             map<string, string>* variables);


class FieldGenerator {
 public:
  FieldGenerator() {}
  virtual ~FieldGenerator();


  virtual void GenerateAccessorDefinitions(
    io::Printer* printer) const = 0;
  virtual void GenerateAccessorExterns(
    io::Printer* printer) const = 0;
    
  virtual void GenerateAccessorDeclarations(
    io::Printer* printer) const = 0;
  virtual void GenerateAccessorGlue(
    io::Printer* printer) const = 0;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(FieldGenerator);
};

// Convenience class which constructs FieldGenerators for a Descriptor.
class FieldGeneratorMap {
 public:
  explicit FieldGeneratorMap(const Descriptor* descriptor);
  ~FieldGeneratorMap();

  const FieldGenerator& get(const FieldDescriptor* field) const;

 private:
  const Descriptor* descriptor_;
  scoped_array<scoped_ptr<FieldGenerator> > field_generators_;

  static FieldGenerator* MakeGenerator(const FieldDescriptor* field);

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(FieldGeneratorMap);
};

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // BAH_PROTOBUF_COMPILER_BMX_FIELD_H__
