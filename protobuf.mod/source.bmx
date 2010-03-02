' Copyright (c) 2008-2010 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz

' turns out we don't need anything other than this config...
Import "src/include/macos/*.h"

Import "src/*.h"

' protobuf
Import "src/google/protobuf/stubs/common.cc"
Import "src/google/protobuf/stubs/once.cc"
Import "src/google/protobuf/stubs/hash.cc"
Import "src/google/protobuf/extension_set.cc"
Import "src/google/protobuf/generated_message_util.cc"
Import "src/google/protobuf/message_lite.cc"
Import "src/google/protobuf/repeated_field.cc"
Import "src/google/protobuf/wire_format_lite.cc"
Import "src/google/protobuf/io/coded_stream.cc"
Import "src/google/protobuf/io/zero_copy_stream.cc"
Import "src/google/protobuf/io/zero_copy_stream_impl_lite.cc"
Import "src/google/protobuf/stubs/strutil.cc"
Import "src/google/protobuf/stubs/substitute.cc"
Import "src/google/protobuf/stubs/structurally_valid.cc"
Import "src/google/protobuf/descriptor.cc"
Import "src/google/protobuf/descriptor.pb.cc"
Import "src/google/protobuf/descriptor_database.cc"
Import "src/google/protobuf/dynamic_message.cc"
Import "src/google/protobuf/extension_set_heavy.cc"
Import "src/google/protobuf/generated_message_reflection.cc"
Import "src/google/protobuf/message.cc"
Import "src/google/protobuf/reflection_ops.cc"
Import "src/google/protobuf/service.cc"
Import "src/google/protobuf/text_format.cc"
Import "src/google/protobuf/unknown_field_set.cc"
Import "src/google/protobuf/wire_format.cc"
Import "src/google/protobuf/io/gzip_stream.cc"
Import "src/google/protobuf/io/printer.cc"
Import "src/google/protobuf/io/tokenizer.cc"
Import "src/google/protobuf/io/zero_copy_stream_impl.cc"
Import "src/google/protobuf/compiler/importer.cc"
Import "src/google/protobuf/compiler/parser.cc"


' protoc
Import "src/google/protobuf/compiler/code_generator.cc"
Import "src/google/protobuf/compiler/command_line_interface.cc"
Import "src/google/protobuf/compiler/plugin.cc"
Import "src/google/protobuf/compiler/plugin.pb.cc"
Import "src/google/protobuf/compiler/subprocess.cc"
Import "src/google/protobuf/compiler/zip_writer.cc"
Import "src/google/protobuf/compiler/cpp/cpp_enum.cc"
Import "src/google/protobuf/compiler/cpp/cpp_enum_field.cc"
Import "src/google/protobuf/compiler/cpp/cpp_extension.cc"
Import "src/google/protobuf/compiler/cpp/cpp_field.cc"
Import "src/google/protobuf/compiler/cpp/cpp_file.cc"
Import "src/google/protobuf/compiler/cpp/cpp_generator.cc"
Import "src/google/protobuf/compiler/cpp/cpp_helpers.cc"
Import "src/google/protobuf/compiler/cpp/cpp_message.cc"
Import "src/google/protobuf/compiler/cpp/cpp_message_field.cc"
Import "src/google/protobuf/compiler/cpp/cpp_primitive_field.cc"
Import "src/google/protobuf/compiler/cpp/cpp_service.cc"
Import "src/google/protobuf/compiler/cpp/cpp_string_field.cc"
Import "src/google/protobuf/compiler/java/java_enum.cc"
Import "src/google/protobuf/compiler/java/java_enum_field.cc"
Import "src/google/protobuf/compiler/java/java_extension.cc"
Import "src/google/protobuf/compiler/java/java_field.cc"
Import "src/google/protobuf/compiler/java/java_file.cc"
Import "src/google/protobuf/compiler/java/java_generator.cc"
Import "src/google/protobuf/compiler/java/java_helpers.cc"
Import "src/google/protobuf/compiler/java/java_message.cc"
Import "src/google/protobuf/compiler/java/java_message_field.cc"
Import "src/google/protobuf/compiler/java/java_primitive_field.cc"
Import "src/google/protobuf/compiler/java/java_service.cc"
Import "src/google/protobuf/compiler/python/python_generator.cc"


' our own compiler
Import "src/bah/protobuf/compiler/blitzmax/bmx_field.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_file.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_generator.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_message.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_helpers.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_extension.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_enum.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_service.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_message_field.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_string_field.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_enum_field.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_primitive_field.cc"
Import "src/bah/protobuf/compiler/blitzmax/bmx_long_field.cc"


Import "glue.cpp"



