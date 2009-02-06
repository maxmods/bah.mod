' Copyright (c) 2009 Bruce A Henderson
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

Import BaH.Raknet
Import BRL.Blitz

?macos
Import "config/macos/*.h"
?win32
Import "config/win32/*.h"
?linux
Import "config/linux/*.h"
?

Import "include/*.h"
Import "libspeex/*.h"

Import "libspeex/bits.c"
Import "libspeex/window.c"
Import "libspeex/buffer.c"
Import "libspeex/cb_search.c"
Import "libspeex/exc_5_64_table.c"
Import "libspeex/exc_5_256_table.c"
Import "libspeex/exc_8_128_table.c"
Import "libspeex/exc_10_16_table.c"
Import "libspeex/exc_10_32_table.c"
Import "libspeex/exc_20_32_table.c"
Import "libspeex/fftwrap.c"
Import "libspeex/filterbank.c"
Import "libspeex/filters.c"
Import "libspeex/gain_table_lbr.c"
Import "libspeex/gain_table.c"
Import "libspeex/hexc_10_32_table.c"
Import "libspeex/hexc_table.c"
Import "libspeex/high_lsp_tables.c"
Import "libspeex/jitter.c"
Import "libspeex/kiss_fft.c"
Import "libspeex/kiss_fftr.c"
Import "libspeex/lpc.c"
Import "libspeex/lsp_tables_nb.c"
Import "libspeex/lsp.c"
Import "libspeex/ltp.c"
Import "libspeex/mdf.c"
Import "libspeex/modes_wb.c"
Import "libspeex/modes.c"
Import "libspeex/nb_celp.c"
Import "libspeex/preprocess.c"
Import "libspeex/quant_lsp.c"
Import "libspeex/resample.c"
Import "libspeex/sb_celp.c"
Import "libspeex/scal.c"
Import "libspeex/smallft.c"
Import "libspeex/speex_callbacks.c"
Import "libspeex/speex_header.c"
Import "libspeex/speex.c"
Import "libspeex/stereo.c"
Import "libspeex/vbr.c"
Import "libspeex/vq.c"


' raknet.mod imports
?win32
Import "../raknet.mod/include/*.h"
?
Import "../raknet.mod/src/*.h"

' rakvoice
Import "src/*.h"
Import "src/RakVoice.cpp"

Import "*.h"
Import "glue.cpp"


