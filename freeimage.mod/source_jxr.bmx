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

Import "src/LibJXR/common/include/*.h"
Import "src/LibJXR/image/sys/*.h"
Import "src/LibJXR/jxrgluelib/*.h"

Import "src/FreeImage/PluginJXR.cpp"

Import "src/LibJXR/image/decode/decode.c"
Import "src/LibJXR/image/decode/JXRTranscode.c"
Import "src/LibJXR/image/decode/postprocess.c"
Import "src/LibJXR/image/decode/segdec.c"
Import "src/LibJXR/image/decode/strdec.c"
Import "src/LibJXR/image/decode/strdec_x86.c"
Import "src/LibJXR/image/decode/strInvTransform.c"
Import "src/LibJXR/image/decode/strPredQuantDec.c"
Import "src/LibJXR/image/encode/encode.c"
Import "src/LibJXR/image/encode/segenc.c"
Import "src/LibJXR/image/encode/strenc.c"
Import "src/LibJXR/image/encode/strenc_x86.c"
Import "src/LibJXR/image/encode/strFwdTransform.c"
Import "src/LibJXR/image/encode/strPredQuantEnc.c"
Import "src/LibJXR/image/sys/adapthuff.c"
Import "src/LibJXR/image/sys/image.c"
Import "src/LibJXR/image/sys/perfTimerANSI.c"
Import "src/LibJXR/image/sys/strcodec.c"
Import "src/LibJXR/image/sys/strPredQuant.c"
Import "src/LibJXR/image/sys/strTransform.c"
Import "src/LibJXR/jxrgluelib/JXRGlue.c"
Import "src/LibJXR/jxrgluelib/JXRGlueJxr.c"
Import "src/LibJXR/jxrgluelib/JXRGluePFC.c"
Import "src/LibJXR/jxrgluelib/JXRMeta.c"

