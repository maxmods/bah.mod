' Copyright (c) 2018-2021 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'
' THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "zstd/lib/*.h"
Import "zstd/lib/common/*.h"

Import "zstd/lib/common/debug.c"
Import "zstd/lib/common/entropy_common.c"
Import "zstd/lib/common/error_private.c"
Import "zstd/lib/common/fse_decompress.c"
Import "zstd/lib/common/pool.c"
Import "zstd/lib/common/threading.c"
?Not bmxng
Import "zstd/lib/common/xxhash.c"
?
Import "zstd/lib/common/zstd_common.c"
Import "zstd/lib/compress/fse_compress.c"
Import "zstd/lib/compress/hist.c"
Import "zstd/lib/compress/huf_compress.c"
Import "zstd/lib/compress/zstdmt_compress.c"
Import "zstd/lib/compress/zstd_compress.c"
Import "zstd/lib/compress/zstd_compress_literals.c"
Import "zstd/lib/compress/zstd_compress_sequences.c"
Import "zstd/lib/compress/zstd_compress_superblock.c"
Import "zstd/lib/compress/zstd_double_fast.c"
Import "zstd/lib/compress/zstd_fast.c"
Import "zstd/lib/compress/zstd_lazy.c"
Import "zstd/lib/compress/zstd_ldm.c"
Import "zstd/lib/compress/zstd_opt.c"
Import "zstd/lib/decompress/huf_decompress.c"
Import "zstd/lib/decompress/zstd_ddict.c"
Import "zstd/lib/decompress/zstd_decompress.c"
Import "zstd/lib/decompress/zstd_decompress_block.c"
Import "zstd/lib/dictBuilder/cover.c"
Import "zstd/lib/dictBuilder/divsufsort.c"
Import "zstd/lib/dictBuilder/zdict.c"

