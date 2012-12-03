' Copyright (c) 2011-2012 Bruce A Henderson
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

Import "libtheora/include/*.h"

Import "libtheoraplayer/include/theoraplayer/*.h"
Import "libtheoraplayer/src/*.h"

Import "../oggvorbis.mod/libogg-1.3.0/include/*.h"
Import "../oggvorbis.mod/libvorbis-1.2.3/include/*.h"

Import "libtheoraplayer/src/TheoraAsync.cpp"
Import "libtheoraplayer/src/TheoraAudioInterface.cpp"
Import "libtheoraplayer/src/TheoraDataSource.cpp"
Import "libtheoraplayer/src/TheoraException.cpp"
Import "libtheoraplayer/src/TheoraFrameQueue.cpp"
Import "libtheoraplayer/src/TheoraTimer.cpp"
Import "libtheoraplayer/src/TheoraUtil.cpp"
Import "libtheoraplayer/src/TheoraVideoClip.cpp"
Import "libtheoraplayer/src/TheoraVideoFrame.cpp"
Import "libtheoraplayer/src/TheoraVideoManager.cpp"
Import "libtheoraplayer/src/TheoraWorkerThread.cpp"


Import "libtheora/src/analyze.c"
Import "libtheora/src/apiwrapper.c"
Import "libtheora/src/bitpack.c"
Import "libtheora/src/cpu.c"
Import "libtheora/src/decapiwrapper.c"
Import "libtheora/src/decinfo.c"
Import "libtheora/src/decode.c"
Import "libtheora/src/dequant.c"
Import "libtheora/src/encapiwrapper.c"
Import "libtheora/src/encfrag.c"
Import "libtheora/src/encinfo.c"
'Import "libtheora/src/encode.c"
Import "libtheora/src/encoder_disabled.c"
Import "libtheora/src/enquant.c"
Import "libtheora/src/fdct.c"
Import "libtheora/src/fragment.c"
Import "libtheora/src/huffdec.c"
Import "libtheora/src/huffenc.c"
Import "libtheora/src/idct.c"
Import "libtheora/src/info.c"
Import "libtheora/src/internal.c"
Import "libtheora/src/mathops.c"
Import "libtheora/src/mcenc.c"
Import "libtheora/src/quant.c"
Import "libtheora/src/rate.c"
Import "libtheora/src/state.c"
Import "libtheora/src/tokenize.c"

Import "glue.cpp"
