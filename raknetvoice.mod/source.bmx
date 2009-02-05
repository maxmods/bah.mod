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


