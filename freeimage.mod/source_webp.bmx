' Copyright (c) 2007-2016 Bruce A Henderson
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

Import "src/*.h"
Import "src/FreeImage/*.h"

Import "src/FreeImage/PluginWebP.cpp"

Import "src/LibWebP/src/dec/dec.alpha.c"
Import "src/LibWebP/src/dec/dec.buffer.c"
Import "src/LibWebP/src/dec/dec.frame.c"
Import "src/LibWebP/src/dec/dec.idec.c"
Import "src/LibWebP/src/dec/dec.io.c"
Import "src/LibWebP/src/dec/dec.quant.c"
Import "src/LibWebP/src/dec/dec.tree.c"
Import "src/LibWebP/src/dec/dec.vp8.c"
Import "src/LibWebP/src/dec/dec.vp8l.c"
Import "src/LibWebP/src/dec/dec.webp.c"
Import "src/LibWebP/src/dsp/dsp.alpha_processing.c"
Import "src/LibWebP/src/dsp/dsp.alpha_processing_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.alpha_processing_sse2.c"
Import "src/LibWebP/src/dsp/dsp.argb.c"
Import "src/LibWebP/src/dsp/dsp.argb_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.argb_sse2.c"
Import "src/LibWebP/src/dsp/dsp.cost.c"
Import "src/LibWebP/src/dsp/dsp.cost_mips32.c"
Import "src/LibWebP/src/dsp/dsp.cost_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.cost_sse2.c"
Import "src/LibWebP/src/dsp/dsp.cpu.c"
Import "src/LibWebP/src/dsp/dsp.dec.c"
Import "src/LibWebP/src/dsp/dsp.dec_clip_tables.c"
Import "src/LibWebP/src/dsp/dsp.dec_mips32.c"
Import "src/LibWebP/src/dsp/dsp.dec_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.dec_neon.c"
Import "src/LibWebP/src/dsp/dsp.dec_sse2.c"
Import "src/LibWebP/src/dsp/dsp.enc.c"
Import "src/LibWebP/src/dsp/dsp.enc_avx2.c"
Import "src/LibWebP/src/dsp/dsp.enc_mips32.c"
Import "src/LibWebP/src/dsp/dsp.enc_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.enc_neon.c"
Import "src/LibWebP/src/dsp/dsp.enc_sse2.c"
Import "src/LibWebP/src/dsp/dsp.filters.c"
Import "src/LibWebP/src/dsp/dsp.filters_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.filters_sse2.c"
Import "src/LibWebP/src/dsp/dsp.lossless.c"
Import "src/LibWebP/src/dsp/dsp.lossless_mips32.c"
Import "src/LibWebP/src/dsp/dsp.lossless_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.lossless_neon.c"
Import "src/LibWebP/src/dsp/dsp.lossless_sse2.c"
Import "src/LibWebP/src/dsp/dsp.rescaler.c"
Import "src/LibWebP/src/dsp/dsp.rescaler_mips32.c"
Import "src/LibWebP/src/dsp/dsp.rescaler_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.upsampling.c"
'Import "src/LibWebP/src/dsp/dsp.upsampling_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.upsampling_neon.c"
Import "src/LibWebP/src/dsp/dsp.upsampling_sse2.c"
Import "src/LibWebP/src/dsp/dsp.yuv.c"
Import "src/LibWebP/src/dsp/dsp.yuv_mips32.c"
'Import "src/LibWebP/src/dsp/dsp.yuv_mips_dsp_r2.c"
Import "src/LibWebP/src/dsp/dsp.yuv_sse2.c"
Import "src/LibWebP/src/enc/enc.alpha.c"
Import "src/LibWebP/src/enc/enc.analysis.c"
Import "src/LibWebP/src/enc/enc.backward_references.c"
Import "src/LibWebP/src/enc/enc.config.c"
Import "src/LibWebP/src/enc/enc.cost.c"
Import "src/LibWebP/src/enc/enc.filter.c"
Import "src/LibWebP/src/enc/enc.frame.c"
Import "src/LibWebP/src/enc/enc.histogram.c"
Import "src/LibWebP/src/enc/enc.iterator.c"
Import "src/LibWebP/src/enc/enc.near_lossless.c"
Import "src/LibWebP/src/enc/enc.picture.c"
Import "src/LibWebP/src/enc/enc.picture_csp.c"
Import "src/LibWebP/src/enc/enc.picture_psnr.c"
Import "src/LibWebP/src/enc/enc.picture_rescale.c"
Import "src/LibWebP/src/enc/enc.picture_tools.c"
Import "src/LibWebP/src/enc/enc.quant.c"
Import "src/LibWebP/src/enc/enc.syntax.c"
Import "src/LibWebP/src/enc/enc.token.c"
Import "src/LibWebP/src/enc/enc.tree.c"
Import "src/LibWebP/src/enc/enc.vp8l.c"
Import "src/LibWebP/src/enc/enc.webpenc.c"
Import "src/LibWebP/src/utils/utils.bit_reader.c"
Import "src/LibWebP/src/utils/utils.bit_writer.c"
Import "src/LibWebP/src/utils/utils.color_cache.c"
Import "src/LibWebP/src/utils/utils.filters.c"
Import "src/LibWebP/src/utils/utils.huffman.c"
Import "src/LibWebP/src/utils/utils.huffman_encode.c"
Import "src/LibWebP/src/utils/utils.quant_levels.c"
Import "src/LibWebP/src/utils/utils.quant_levels_dec.c"
Import "src/LibWebP/src/utils/utils.random.c"
Import "src/LibWebP/src/utils/utils.rescaler.c"
Import "src/LibWebP/src/utils/utils.thread.c"
Import "src/LibWebP/src/utils/utils.utils.c"
Import "src/LibWebP/src/mux/mux.anim_encode.c"
Import "src/LibWebP/src/mux/mux.muxedit.c"
Import "src/LibWebP/src/mux/mux.muxinternal.c"
Import "src/LibWebP/src/mux/mux.muxread.c"
Import "src/LibWebP/src/demux/demux.demux.c"


