' 
' Copyright 2017 Bruce A Henderson
' 
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
' 
'     http://www.apache.org/licenses/LICENSE-2.0
' 
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
' 
SuperStrict


Import "source/*.h"
Import "source/third_party/*.h"
Import "source/third_party/butteraugli/*.h"

Import "source/guetzli/butteraugli_comparator.cc"
Import "source/guetzli/dct_double.cc"
Import "source/guetzli/debug_print.cc"
Import "source/guetzli/entropy_encode.cc"
Import "source/guetzli/fdct.cc"
Import "source/guetzli/gamma_correct.cc"
Import "source/guetzli/idct.cc"
Import "source/guetzli/jpeg_data.cc"
Import "source/guetzli/jpeg_data_decoder.cc"
Import "source/guetzli/jpeg_data_encoder.cc"
Import "source/guetzli/jpeg_data_reader.cc"
Import "source/guetzli/jpeg_data_writer.cc"
Import "source/guetzli/jpeg_huffman_decode.cc"
Import "source/guetzli/output_image.cc"
Import "source/guetzli/preprocess_downsample.cc"
Import "source/guetzli/processor.cc"
Import "source/guetzli/quality.cc"
Import "source/guetzli/quantize.cc"
Import "source/guetzli/score.cc"
Import "source/third_party/butteraugli/butteraugli/butteraugli.cc"


Import "glue.cpp"
