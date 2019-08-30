' Copyright (c) 2019 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Import "brotli/c/include/*.h"

Import "brotli/c/common/dictionary.c"
Import "brotli/c/common/transform.c"
Import "brotli/c/dec/bit_reader.c"
Import "brotli/c/dec/decode.c"
Import "brotli/c/dec/huffman.c"
Import "brotli/c/dec/state.c"
Import "brotli/c/enc/backward_references.c"
Import "brotli/c/enc/backward_references_hq.c"
Import "brotli/c/enc/bit_cost.c"
Import "brotli/c/enc/block_splitter.c"
Import "brotli/c/enc/brotli_bit_stream.c"
Import "brotli/c/enc/cluster.c"
Import "brotli/c/enc/compress_fragment.c"
Import "brotli/c/enc/compress_fragment_two_pass.c"
Import "brotli/c/enc/dictionary_hash.c"
Import "brotli/c/enc/encode.c"
Import "brotli/c/enc/encoder_dict.c"
Import "brotli/c/enc/entropy_encode.c"
Import "brotli/c/enc/histogram.c"
Import "brotli/c/enc/literal_cost.c"
Import "brotli/c/enc/memory.c"
Import "brotli/c/enc/metablock.c"
Import "brotli/c/enc/static_dict.c"
Import "brotli/c/enc/utf8_util.c"

Import "glue.c"
