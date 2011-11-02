' Copyright (c) 2011, Bruce A Henderson
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


Import Pub.OggVorbis

Import "source.bmx"


Extern

	Function bmx_TheoraVideoManager_new:Byte Ptr()
	Function bmx_TheoraVideoManager_free(handle:Byte Ptr)
	Function bmx_TheoraVideoManager_createVideoClip:Byte Ptr(handle:Byte Ptr, filename:String, outputMode:Int, numPrecachedOverride:Int, usePower2Stride:Int)
	Function bmx_TheoraVideoManager_update(handle:Byte Ptr, timeIncrease:Float)
	Function bmx_TheoraVideoManager_setAudioInterfaceFactory(handle:Byte Ptr, factory:Byte Ptr)

	Function bmx_TheoraVideoClip_getName:String(handle:Byte Ptr)
	Function bmx_TheoraVideoClip_getWidth:Int(handle:Byte Ptr)
	Function bmx_TheoraVideoClip_getHeight:Int(handle:Byte Ptr)
	Function bmx_TheoraVideoClip_getStride:Int(handle:Byte Ptr)
	Function bmx_TTheoraVideoClip_getNextFrame:Byte Ptr(handle:Byte Ptr)
	Function bmx_TheoraVideoClip_popFrame(handle:Byte Ptr)

	Function bmx_TheoraVideoFrame_getBuffer:Byte Ptr(handle:Byte Ptr)

End Extern


Type TTheoraGenericException Extends TRuntimeException
	Field mErrText:String
	Field mFile:String
	Field mType:String
	
	Function _create:TTheoraGenericException(mErrText:String, mFile:String, mType:String)
		Local this:TTheoraGenericException = New TTheoraGenericException
		this.mErrText = mErrText
		this.mFile = mFile
		this.mType = mType
		Return this
	End Function
	
	Method ToString:String()
		Return mErrText
	End Method
	
End Type

Const TH_RGB:Int = 1
Const TH_RGBA:Int = 2
Const TH_ARGB:Int = 3
Const TH_BGR:Int = 4
Const TH_BGRA:Int = 5
Const TH_ABGR:Int = 6
Const TH_GREY:Int = 7
Const TH_GREY3:Int = 8 ' RGB but all three components are luma
Const TH_GREY3A:Int = 9
Const TH_AGREY3:Int = 10
Const TH_YUV:Int = 11
Const TH_YUVA:Int = 12
Const TH_AYUV:Int = 13
