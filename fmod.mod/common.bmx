' Copyright (c) 2008-2010 Bruce A Henderson
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

Import BRL.Blitz

Import "-lfmodex"

Import "include/*.h"

Import "glue.cpp"

Extern
	Function bmx_fmod_getversion:Int()
	Function bmx_FMOD_ErrorString:Byte Ptr(result:Int)

	Function bmx_FMOD_System_Create:Byte Ptr(result:Int Ptr)
	Function bmx_FMOD_System_CreateSound:Byte Ptr(handle:Byte Ptr, filename:Byte Ptr, Mode:Int, exInfo:Byte Ptr, ret:Int Ptr)
	Function bmx_FMOD_System_PlaySound:Byte Ptr(handle:Byte Ptr, channelId:Int, sound:Byte Ptr, paused:Int, reuse:Byte Ptr)
	Function bmx_FMOD_System_GetChannel:Byte Ptr(handle:Byte Ptr, channelId:Int)
	Function bmx_FMOD_System_CreateDSPByType:Byte Ptr(handle:Byte Ptr, dspType:Int, ret:Int Ptr)
	Function bmx_FMOD_System_PlayDSP:Byte Ptr(handle:Byte Ptr, channelId:Int, dsp:Byte Ptr, paused:Int, reuse:Byte Ptr)
	Function bmx_FMOD_System_CreateStream:Byte Ptr(handle:Byte Ptr, filename:Byte Ptr, Mode:Int, exInfo:Byte Ptr, ret:Int Ptr)
	Function bmx_FMOD_System_CreateReverb:Byte Ptr(handle:Byte Ptr)
	Function bmx_FMOD_System_GetSpectrum:Int(handle:Byte Ptr, spectrumArray:Float[], channelOffset:Int, windowType:Int)
	Function bmx_FMOD_System_CreateChannelGroup:Byte Ptr(handle:Byte Ptr, s:Byte Ptr)
	Function bmx_FMOD_System_GetMasterChannelGroup:Byte Ptr(handle:Byte Ptr)

	Function bmx_fmodchannel_delete(handle:Byte Ptr)
	Function bmx_FMOD_Channel_GetSpectrum:Int(handle:Byte Ptr, spectrumArray:Float[], channelOffset:Int, windowType:Int)
	Function bmx_FMOD_Channel_GetCurrentSound:Byte Ptr(handle:Byte Ptr)
	Function bmx_FMOD_Channel_GetPaused:Int(handle:Byte Ptr, paused:Int Ptr)
	Function bmx_FMOD_Channel_SetPaused:Int(handle:Byte Ptr, paused:Int)
	Function bmx_FMOD_Channel_Stop:Int(handle:Byte Ptr)
	Function bmx_FMOD_Channel_GetVolume:Int(handle:Byte Ptr, volume:Float Ptr)
	Function bmx_FMOD_Channel_SetVolume:Int(handle:Byte Ptr, volume:Float)
	Function bmx_FMOD_Channel_GetPan:Int(handle:Byte Ptr, pan:Float Ptr)
	Function bmx_FMOD_Channel_SetPan:Int(handle:Byte Ptr, pan:Float)
	Function bmx_FMOD_Channel_GetDelay:Int(handle:Byte Ptr, delayType:Int, delayHi:Int Ptr, delayLo:Int Ptr)

	Function bmx_FMOD_Channel_IsPlaying:Int(handle:Byte Ptr, playing:Int Ptr)
	Function bmx_FMOD_Channel_GetFrequency:Int(handle:Byte Ptr, frequency:Float Ptr)
	Function bmx_FMOD_Channel_SetFrequency:Int(handle:Byte Ptr, frequency:Float)
	Function bmx_FMOD_Channel_GetPosition:Int(handle:Byte Ptr, position:Int Ptr, posType:Int)
	Function bmx_FMOD_Channel_IsVirtual:Int(handle:Byte Ptr, virtual:Int Ptr)
	Function bmx_FMOD_Channel_GetAudibility:Int(handle:Byte Ptr, audibility:Float Ptr)
	Function bmx_FMOD_Channel_GetIndex:Int(handle:Byte Ptr, index:Int Ptr)
	Function bmx_FMOD_Channel_GetLoopCount:Int(handle:Byte Ptr, loopCount:Int Ptr)
	Function bmx_FMOD_Channel_GetLoopPoints:Int(handle:Byte Ptr, loopStart:Int Ptr, loopStartType:Int, loopEnd:Int Ptr, loopEndType:Int)
	Function bmx_FMOD_Channel_Get3DDopplerLevel:Int(handle:Byte Ptr, level:Float Ptr)
	Function bmx_FMOD_Channel_Get3DMinMaxDistance:Int(handle:Byte Ptr, minDistance:Float Ptr, maxDistance:Float Ptr)
	Function bmx_FMOD_Channel_Get3DOcclusion:Int(handle:Byte Ptr, directOcclusion:Float Ptr, reverbOcclusion:Float Ptr)
	Function bmx_FMOD_Channel_Get3DPanLevel:Int(handle:Byte Ptr, level:Float Ptr)
	Function bmx_FMOD_Channel_Get3DSpread:Int(handle:Byte Ptr, angle:Float Ptr)
	Function bmx_FMOD_Channel_Get3DAttributes:Int(handle:Byte Ptr, pos:Byte Ptr, vel:Byte Ptr)
	Function bmx_FMOD_Channel_Get3DConeOrientation:Int(handle:Byte Ptr, orientation:Byte Ptr)
	Function bmx_FMOD_Channel_SetChannelGroup:Int(handle:Byte Ptr, group:Byte Ptr)
	Function bmx_FMOD_Channel_Get3DConeSettings:Int(handle:Byte Ptr, insideConeAngle:Float Ptr, outsideConeAngle:Float Ptr, outsideVolume:Float Ptr)
	Function bmx_FMOD_Channel_GetChannelGroup:Byte Ptr(handle:Byte Ptr)
	Function bmx_FMOD_Channel_GetMode:Int(handle:Byte Ptr, Mode:Int Ptr)
	Function bmx_FMOD_Channel_GetMute:Int(handle:Byte Ptr, mute:Int Ptr)
	Function bmx_FMOD_Channel_GetPriority:Int(handle:Byte Ptr, priority:Int Ptr)
	Function bmx_FMOD_Channel_GetReverbProperties:Int(handle:Byte Ptr, properties:Byte Ptr)
	Function bmx_FMOD_Channel_SetReverbProperties:Int(handle:Byte Ptr, properties:Byte Ptr)
	Function bmx_FMOD_Channel_SetPriority:Int(handle:Byte Ptr, priority:Int)
	Function bmx_FMOD_Channel_GetLowPassGain:Int(handle:Byte Ptr, gain:Float Ptr)
	Function bmx_FMOD_Channel_GetSpeakerLevels:Int(handle:Byte Ptr, speaker:Int, levels:Float[])
	Function bmx_FMOD_Channel_GetSpeakerMix:Int(handle:Byte Ptr, frontleft:Float Ptr, frontright:Float Ptr, center:Float Ptr, ..
			lfe:Float Ptr, backleft:Float Ptr, backright:Float Ptr, sideleft:Float Ptr, sideright:Float Ptr)
	Function bmx_FMOD_Channel_GetWaveData:Int(handle:Byte Ptr, waveArray:Float[], channelOffset:Int)
	Function bmx_FMOD_Channel_SetDelay:Int(handle:Byte Ptr, delayType:Int, delayHi:Int, delayLo:Int)
	Function bmx_FMOD_Channel_SetInputChannelMix:Int(handle:Byte Ptr, levels:Float[])
	Function bmx_FMOD_Channel_SetLoopCount:Int(handle:Byte Ptr, loopCount:Int)
	Function bmx_FMOD_Channel_SetLoopPoints:Int(handle:Byte Ptr, loopStart:Int, loopStartType:Int, loopEnd:Int, loopEndType:Int)
	Function bmx_FMOD_Channel_SetLowPassGain:Int(handle:Byte Ptr, gain:Float)
	Function bmx_FMOD_Channel_SetMode:Int(handle:Byte Ptr, Mode:Int)
	Function bmx_FMOD_Channel_SetMute:Int(handle:Byte Ptr, mute:Int)
	Function bmx_FMOD_Channel_SetPosition:Int(handle:Byte Ptr, position:Int, postype:Int)

	Function bmx_soundexinfo_create:Byte Ptr()
	Function bmx_soundexinfo_setlength(handle:Byte Ptr, length:Int)	Function bmx_soundexinfo_delete(handle:Byte Ptr)
	Function bmx_soundexinfo_setnumchannels(handle:Byte Ptr, numChannels:Int)
	Function bmx_soundexinfo_setdefaultfrequency(handle:Byte Ptr, frequency:Int)
	Function bmx_soundexinfo_setformat(handle:Byte Ptr, format:Int)
	Function bmx_soundexinfo_setdecodebuffersize(handle:Byte Ptr, bufferSize:Int)
	Function bmx_soundexinfo_setinitialsubsound(handle:Byte Ptr, initial:Int)
	Function bmx_soundexinfo_setnumsubsounds(handle:Byte Ptr, num:Int)
	Function bmx_soundexinfo_setpcmreadcallback(handle:Byte Ptr, callback:Int(sound:Byte Ptr, data:Byte Ptr, dataLen:Int))
	Function bmx_soundexinfo_setpcmsetposcallback(handle:Byte Ptr, callback:Int(sound:Byte Ptr, subsound:Int, position:Int, posType:Int))
	Function bmx_soundexinfo_getuserdata:Object(handle:Byte Ptr)
	Function bmx_soundexinfo_setuserdata(handle:Byte Ptr, obj:Object)

	Function bmx_FMOD_SoundGroup_GetSound:Byte Ptr(handle:Byte Ptr, index:Int)
	
	Function bmx_FMOD_Sound_GetTag:Byte Ptr(handle:Byte Ptr, s:Byte Ptr, index:Int)
	Function bmx_FMOD_Sound_SetSubSoundSentence:Int(handle:Byte Ptr, soundList:Int[])
	Function bmx_FMOD_Sound_GetSubSound:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_FMOD_Sound_GetUserData:Object(handle:Byte Ptr)
	'Function bmx_FMOD_Sound_SetUserData(handle:Byte Ptr, obj:Object)

	Function bmx_fmodtag_getdata:Byte Ptr(handle:Byte Ptr)
	Function bmx_fmodtag_delete(handle:Byte Ptr)
	Function bmx_fmodtag_gettagtype:Int(handle:Byte Ptr)
	Function bmx_fmodtag_getdatatype:Int(handle:Byte Ptr)
	Function bmx_fmodtag_getname:Byte Ptr(handle:Byte Ptr)
	Function bmx_fmodtag_getdatalength:Int(handle:Byte Ptr)
	Function bmx_fmodtag_getupdated:Int(handle:Byte Ptr)

	Function bmx_FMOD_ChannelGroup_GetChannel:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_fmodreverbchannelproperties_delete(handle:Byte Ptr)

End Extern

?win32
Extern "win32"
?Not win32
Extern
?
	Function FMOD_System_Release:Int(handle:Byte Ptr)
	Function FMOD_System_Init:Int(handle:Byte Ptr, maxChannels:Int, flags:Int, extraDriverData:Byte Ptr)
	Function FMOD_System_Update:Int(handle:Byte Ptr)
	Function FMOD_System_GetChannelsPlaying:Int(handle:Byte Ptr, channels:Int Ptr)
	Function FMOD_System_Get3DListenerAttributes:Int(handle:Byte Ptr, listener:Int, pos:Byte Ptr, vel:Byte Ptr, forward:Byte Ptr, up:Byte Ptr)
	Function FMOD_System_Get3DNumListeners:Int(handle:Byte Ptr, listeners:Int Ptr)
	Function FMOD_System_Get3DSettings:Int(handle:Byte Ptr, dopplerScale:Float Ptr, distanceFactor:Float Ptr, rollOffScale:Float Ptr)
	Function FMOD_System_Get3DSpeakerPosition:Int(handle:Byte Ptr, speaker:Int, x:Float Ptr, y:Float Ptr, active:Int Ptr)
	Function FMOD_System_GetCPUUsage:Int(handle:Byte Ptr, dsp:Float Ptr, stream:Float Ptr, geometry:Float Ptr, update:Float Ptr, total:Float Ptr)
	Function FMOD_System_GetHardwareChannels:Int(handle:Byte Ptr, num2D:Int Ptr, num3D:Int Ptr, total:Int Ptr)
	Function FMOD_System_GetVersion:Int(handle:Byte Ptr, version:Int Ptr)
	Function FMOD_System_GetNumCDROMDrives:Int(handle:Byte Ptr, numDrives:Int Ptr)
	Function FMOD_System_GetSoundRAM:Int(handle:Byte Ptr, currentAlloced:Int Ptr, maxAlloced:Int Ptr, total:Int Ptr)
	Function FMOD_System_Close:Int(handle:Byte Ptr)
	Function FMOD_System_AddDSP:Int(handle:Byte Ptr, dsp:Byte Ptr, conn:Byte Ptr)
	Function FMOD_System_GetSpeakerMode:Int(handle:Byte Ptr, speakerMode:Int Ptr)
	Function FMOD_System_GetStreamBufferSize:Int(handle:Byte Ptr, fileBufferSize:Int Ptr, fileBufferSizeType:Int Ptr)
	Function FMOD_System_GetSoftwareFormat:Int(handle:Byte Ptr, sampleRate:Int Ptr, format:Int Ptr, numOutputChannels:Int Ptr, ..
		maxInputChannels:Int Ptr, resampleMethod:Int Ptr, bits:Int Ptr)
	Function FMOD_System_GetSoftwareChannels:Int(handle:Byte Ptr, numSoftwareChannels:Int Ptr)
	'Function FMOD_System_GetRecordDriver:Int(handle:Byte Ptr, driver:Int Ptr)
	Function FMOD_System_GetRecordDriverCaps:Int(handle:Byte Ptr, id:Int, caps:Int Ptr, minFrequency:Int Ptr, maxFrequency:Int Ptr)
	Function FMOD_System_GetRecordNumDrivers:Int(handle:Byte Ptr, numDrivers:Int Ptr)
	Function FMOD_System_GetRecordPosition:Int(handle:Byte Ptr, id:Int, position:Int Ptr)
	Function FMOD_System_GetWaveData:Int(handle:Byte Ptr, waveArray:Float Ptr, numValues:Int, channelOffset:Int)
	Function FMOD_System_IsRecording:Int(handle:Byte Ptr, id:Int, recording:Int Ptr)
	Function FMOD_System_RecordStop:Int(handle:Byte Ptr, id:Int)
	Function FMOD_System_Set3DListenerAttributes:Int(handle:Byte Ptr, listener:Int, pos:Byte Ptr, vel:Byte Ptr, forward:Byte Ptr, up:Byte Ptr)
	Function FMOD_System_Set3DNumListeners:Int(handle:Byte Ptr, numListeners:Int)
	Function FMOD_System_Set3DSettings:Int(handle:Byte Ptr, dopplerScale:Float, distanceFactor:Float, rolloffScale:Float)
	Function FMOD_System_Set3DSpeakerPosition:Int(handle:Byte Ptr, speaker:Int, x:Float, y:Float, active:Int)
	Function FMOD_System_GetDSPBufferSize:Int(handle:Byte Ptr, bufferLength:Int Ptr, numBuffers:Int Ptr)
	Function FMOD_System_GetDSPClock:Int(handle:Byte Ptr, hi:Int Ptr, lo:Int Ptr)
	Function FMOD_System_GetDriver:Int(handle:Byte Ptr, driver:Int Ptr)
	Function FMOD_System_GetDriverCaps:Int(handle:Byte Ptr, id:Int, caps:Int Ptr, minFrequency:Int Ptr, maxFrequency:Int Ptr, controlPanelSpeakerMode:Int Ptr)
	Function FMOD_System_GetGeometrySettings:Int(handle:Byte Ptr, maxWorldSize:Float Ptr)
	Function FMOD_System_SetDSPBufferSize:Int(handle:Byte Ptr, bufferLength:Int, numBuffers:Int)
	Function FMOD_System_SetDriver:Int(handle:Byte Ptr, driver:Int)
	Function FMOD_System_SetGeometrySettings:Int(handle:Byte Ptr, maxWorldSize:Float)
	Function FMOD_System_SetHardwareChannels:Int(handle:Byte Ptr, min2d:Int, max2d:Int, min3d:Int, max3d:Int)
	Function FMOD_System_SetNetworkProxy:Int(handle:Byte Ptr, s:Byte Ptr)
	Function FMOD_System_SetNetworkTimeout:Int(handle:Byte Ptr, timeout:Int)
	Function FMOD_System_SetOutput:Int(handle:Byte Ptr, output:Int)
	Function FMOD_System_SetPluginPath:Int(handle:Byte Ptr, s:Byte Ptr)
	'Function FMOD_System_SetRecordDriver:Int(handle:Byte Ptr, driver:Int)
	Function FMOD_System_SetSoftwareChannels:Int(handle:Byte Ptr, numSoftwareChannels:Int)
	Function FMOD_System_SetSoftwareFormat:Int(handle:Byte Ptr, samplerate:Int, format:Int, numOutputChannels:Int, maxInputChannels:Int, resampleMethod:Int)
	Function FMOD_System_SetSpeakerMode:Int(handle:Byte Ptr, Mode:Int)
	Function FMOD_System_SetStreamBufferSize:Int(handle:Byte Ptr, fileBufferSize:Int, fileBufferSizeType:Int)
	Function FMOD_System_RecordStart:Int(handle:Byte Ptr, id:Int, sound:Byte Ptr, loop:Int)


	Function FMOD_Sound_SetMode:Int(handle:Byte Ptr, Mode:Int)
	Function FMOD_Sound_GetLength:Int(handle:Byte Ptr, length:Int Ptr, lengthType:Int)
	Function FMOD_Sound_Release:Int(handle:Byte Ptr)
	Function FMOD_Sound_GetMode:Int(handle:Byte Ptr, Mode:Int Ptr)
	Function FMOD_Sound_GetLoopCount:Int(handle:Byte Ptr, loopCount:Int Ptr)
	Function FMOD_Sound_SetLoopCount:Int(handle:Byte Ptr, loopCount:Int)
	Function FMOD_Sound_SetLoopPoints:Int(handle:Byte Ptr, loopStart:Int, loopStartType:Int, loopEnd:Int, loopEndType:Int)
	Function FMOD_Sound_GetLoopPoints:Int(handle:Byte Ptr, loopStart:Int Ptr, loopStartType:Int Ptr, loopEnd:Int Ptr, loopEndType:Int Ptr)
	Function FMOD_Sound_GetMusicNumChannels:Int(handle:Byte Ptr, numChannels:Int Ptr)
	Function FMOD_Sound_GetMusicChannelVolume:Int(handle:Byte Ptr, channel:Int, volume:Int Ptr)
	Function FMOD_Sound_SetMusicChannelVolume:Int(handle:Byte Ptr, channel:Int, volume:Int)
	Function FMOD_Sound_GetDefaults:Int(handle:Byte Ptr, frequency:Float Ptr, volume:Float Ptr, pan:Float Ptr, priority:Int Ptr)
	Function FMOD_Sound_SetDefaults:Int(handle:Byte Ptr, frequency:Float, volume:Float, pan:Float, priority:Int)
	Function FMOD_Sound_GetVariations:Int(handle:Byte Ptr, frequencyVar:Float Ptr, volumeVar:Float Ptr, panVar:Float Ptr)
	Function FMOD_Sound_SetVariations:Int(handle:Byte Ptr, frequencyVar:Float, volumeVar:Float, panVar:Float)
	Function FMOD_Sound_Get3DConeSettings:Int(handle:Byte Ptr, insideConeAngle:Float Ptr, outsideConeAngle:Float Ptr, outsideVolume:Float Ptr)
	Function FMOD_Sound_Set3DConeSettings:Int(handle:Byte Ptr, insideConeAngle:Float, outsideConeAngle:Float, outsideVolume:Float)
	Function FMOD_Sound_Get3DMinMaxDistance:Int(handle:Byte Ptr, minDist:Float Ptr, maxDist:Float Ptr)
	Function FMOD_Sound_Set3DMinMaxDistance:Int(handle:Byte Ptr, minDist:Float, maxDist:Float)
	Function FMOD_Sound_GetFormat:Int(handle:Byte Ptr, soundType:Int Ptr, soundFormat:Int Ptr, channels:Int Ptr, bits:Int Ptr)
	Function FMOD_Sound_GetNumSubSounds:Int(handle:Byte Ptr, numSubSounds:Int Ptr)
	Function FMOD_Sound_GetNumTags:Int(handle:Byte Ptr, numTags:Int Ptr, numTagsUpdated:Int Ptr)
	Function FMOD_Sound_GetOpenState:Int(handle:Byte Ptr, openState:Int Ptr, percentBuffered:Int Ptr, starving:Int Ptr)
	Function FMOD_Sound_GetName:Int(handle:Byte Ptr, vstr:Byte Ptr, length:Int)
	Function FMOD_Sound_SetSubSound:Int(handle:Byte Ptr, index:Int, subsound:Byte Ptr)
		
	Function FMOD_DSP_GetActive:Int(handle:Byte Ptr, active:Int Ptr)
	Function FMOD_DSP_SetActive:Int(handle:Byte Ptr, active:Int)
	Function FMOD_DSP_Remove:Int(handle:Byte Ptr)
	Function FMOD_DSP_Reset:Int(handle:Byte Ptr)
	Function FMOD_DSP_GetBypass:Int(handle:Byte Ptr, bypass:Int Ptr)
	Function FMOD_DSP_SetBypass:Int(handle:Byte Ptr, bypass:Int)
	Function FMOD_DSP_GetNumParameters:Int(handle:Byte Ptr, numParams:Int Ptr)
	Function FMOD_DSP_SetParameter:Int(handle:Byte Ptr, index:Int, value:Float)
	Function FMOD_DSP_GetParameter:Int(handle:Byte Ptr, index:Int, value:Float Ptr, vstr:Byte Ptr, length:Int)
	Function FMOD_DSP_GetInfo:Int(handle:Byte Ptr, vstr:Byte Ptr, version:Int Ptr, channels:Int Ptr, cw:Int Ptr, ch:Int Ptr)
	Function FMOD_DSP_GetType:Int(handle:Byte Ptr, dspType:Int Ptr)
	Function FMOD_DSP_GetDefaults:Int(handle:Byte Ptr, frequency:Float Ptr, volume:Float Ptr, pan:Float Ptr, priority:Int Ptr)
	Function FMOD_DSP_Release:Int(handle:Byte Ptr)

	Function FMOD_SoundGroup_GetMaxAudible:Int(handle:Byte Ptr, maxAudible:Int Ptr)
	Function FMOD_SoundGroup_GetMaxAudibleBehavior:Int(handle:Byte Ptr, behavior:Int Ptr)
	Function FMOD_SoundGroup_GetMuteFadeSpeed:Int(handle:Byte Ptr, speed:Float Ptr)
	Function FMOD_SoundGroup_GetName:Int(handle:Byte Ptr, vstr:Byte Ptr, length:Int)
	Function FMOD_SoundGroup_GetNumPlaying:Int(handle:Byte Ptr, numPlaying:Int Ptr)
	Function FMOD_SoundGroup_GetNumSounds:Int(handle:Byte Ptr, numSounds:Int Ptr)
	Function FMOD_SoundGroup_GetVolume:Int(handle:Byte Ptr, volume:Float Ptr)
	Function FMOD_SoundGroup_Release:Int(handle:Byte Ptr)
	Function FMOD_SoundGroup_SetMaxAudible:Int(handle:Byte Ptr, maxAudible:Int)
	Function FMOD_SoundGroup_SetMaxAudibleBehavior:Int(handle:Byte Ptr, behavior:Int)
	Function FMOD_SoundGroup_SetMuteFadeSpeed:Int(handle:Byte Ptr, speed:Float)
	Function FMOD_SoundGroup_SetVolume:Int(handle:Byte Ptr, volume:Float)
	Function FMOD_SoundGroup_Stop:Int(handle:Byte Ptr)

	Function FMOD_Reverb_GetActive:Int(handle:Byte Ptr, active:Int Ptr)
	Function FMOD_Reverb_SetActive:Int(handle:Byte Ptr, active:Int)
	Function FMOD_Reverb_Release:Int(handle:Byte Ptr)
	Function FMOD_Reverb_Get3DAttributes:Int(handle:Byte Ptr, position:Byte Ptr, minDistance:Float Ptr, maxDistance:Float Ptr)
	Function FMOD_Reverb_Set3DAttributes:Int(handle:Byte Ptr, position:Byte Ptr, minDistance:Float, maxDistance:Float)
	Function FMOD_Reverb_GetProperties:Int(handle:Byte Ptr, properties:Byte Ptr)
	Function FMOD_Reverb_SetProperties:Int(handle:Byte Ptr, properties:Byte Ptr)

	Function FMOD_ChannelGroup_GetVolume:Int(handle:Byte Ptr, volume:Float Ptr)
	Function FMOD_ChannelGroup_SetVolume:Int(handle:Byte Ptr, volume:Float)
	Function FMOD_ChannelGroup_Stop:Int(handle:Byte Ptr)
	Function FMOD_ChannelGroup_GetMute:Int(handle:Byte Ptr, mute:Int Ptr)
	Function FMOD_ChannelGroup_SetMute:Int(handle:Byte Ptr, mute:Int)
	Function FMOD_ChannelGroup_GetPaused:Int(handle:Byte Ptr, paused:Int Ptr)
	Function FMOD_ChannelGroup_SetPaused:Int(handle:Byte Ptr, paused:Int)
	Function FMOD_ChannelGroup_GetPitch:Int(handle:Byte Ptr, pitch:Float Ptr)
	Function FMOD_ChannelGroup_SetPitch:Int(handle:Byte Ptr, pitch:Float)
	Function FMOD_ChannelGroup_GetNumChannels:Int(handle:Byte Ptr, numChannels:Int Ptr)
	Function FMOD_ChannelGroup_GetNumGroups:Int(handle:Byte Ptr, numGroups:Int Ptr)
	Function FMOD_ChannelGroup_Get3DOcclusion:Int(handle:Byte Ptr, directOcclusion:Float Ptr, reverbOcclusion:Float Ptr)
	Function FMOD_ChannelGroup_Set3DOcclusion:Int(handle:Byte Ptr, directOcclusion:Float, reverbOcclusion:Float)
	Function FMOD_ChannelGroup_Override3DAttributes:Int(handle:Byte Ptr, pos:Byte Ptr, vel:Byte Ptr)
	Function FMOD_ChannelGroup_OverrideFrequency:Int(handle:Byte Ptr, frequency:Float)
	Function FMOD_ChannelGroup_OverridePan:Int(handle:Byte Ptr, pan:Float)
	Function FMOD_ChannelGroup_OverrideSpeakerMix:Int(handle:Byte Ptr, frontleft:Float, frontright:Float, center:Float, ..
		lfe:Float, backleft:Float, backright:Float, sideleft:Float, sideright:Float)
	Function FMOD_ChannelGroup_OverrideVolume:Int(handle:Byte Ptr, volume:Float)
	Function FMOD_ChannelGroup_Release:Int(handle:Byte Ptr)
	Function FMOD_ChannelGroup_AddGroup:Int(handle:Byte Ptr, group:Byte Ptr)
	Function FMOD_ChannelGroup_AddDSP:Int(handle:Byte Ptr, dsp:Byte Ptr, connection:Byte Ptr)
	Function FMOD_ChannelGroup_OverrideReverbProperties:Int(handle:Byte Ptr, properties:Byte Ptr)

	Function FMOD_File_SetDiskBusy:Int(busy:Int)
	Function FMOD_File_GetDiskBusy:Int(busy:Int Ptr)

End Extern

Extern
	Rem
	bbdoc: Structure describing a point in 3D space.  
	about: FMOD uses a left handed co-ordinate system by default.
	<p>
	To use a right handed co-ordinate system specify FMOD_INIT_3D_RIGHTHANDED from FMOD_INITFLAGS in System::init. 
	</p>
	End Rem
	Type TFMODVector
		Rem
		bbdoc: X co-ordinate in 3D spa
		End Rem
		Field x:Float
		Rem
		bbdoc: Y co-ordinate in 3D spa
		End Rem
		Field y:Float
		Rem
		bbdoc: Z co-ordinate in 3D spa
		End Rem
		Field z:Float
	End Type

	Rem
	bbdoc: Structure defining a reverb environment.
	End Rem
	Type TFMODReverbProperties
		Rem
		bbdoc: Instance
		about: 0 , 3 , 0 , Environment Instance.
		Simultaneous HW reverbs are possible on some platforms.
		<p>
		(SUPPORTED:EAX4/SFX(3 instances)/GC and Wii (2 instances)) 
		</p>
		End Rem
		Field Instance:Int
		Rem
		bbdoc: Environment
		about: -1 , 25 , -1 , sets all listener properties. -1 = OFF.
		<p>
		(SUPPORTED:EAX/PS2) 
		</p>
		End Rem
		Field Environment:Int
		Rem
		bbdoc: EnvSize
		about: 1.0 , 100.0 , 7.5 , environment size in meters
		<p>
		(SUPPORTED:EAX) 
		</p>
		End Rem
		Field EnvSize:Float
		Rem
		bbdoc: EnvDiffusion
		about: 0.0 , 1.0 , 1.0 , environment diffusion
		<p>
		(SUPPORTED:EAX/Xbox1/GC)
		</p>
		End Rem
		Field EnvDiffusion:Float
		Rem
		bbdoc: Room
		about: 
		End Rem
		Field Room:Int
		Rem
		bbdoc: RoomHF
		about: 
		End Rem
		Field RoomHF:Int
		Rem
		bbdoc: RoomLF
		about: 
		End Rem
		Field RoomLF:Int
		Rem
		bbdoc: DecayTime
		about: 
		End Rem
		Field DecayTime:Float
		Rem
		bbdoc: DecayHFRatio
		about: 
		End Rem
		Field DecayHFRatio:Float
		Rem
		bbdoc: DecayLFRatio
		about: 
		End Rem
		Field DecayLFRatio:Float
		Rem
		bbdoc: Reflections
		about: 
		End Rem
		Field Reflections:Int
		Rem
		bbdoc: ReflectionsDelay
		about: 
		End Rem
		Field ReflectionsDelay:Float
		Rem
		bbdoc: ReflectionsPan1
		about: 
		End Rem
		Field ReflectionsPan1:Float
		Rem
		bbdoc: ReflectionsPan2
		about: 
		End Rem
		Field ReflectionsPan2:Float
		Rem
		bbdoc: ReflectionsPan3
		about: 
		End Rem
		Field ReflectionsPan3:Float
		Rem
		bbdoc: Reverb
		about: 
		End Rem
		Field Reverb:Int
		Rem
		bbdoc: ReverbDelay
		about: 
		End Rem
		Field ReverbDelay:Float
		Rem
		bbdoc: ReverbPan1
		about: 
		End Rem
		Field ReverbPan1:Float
		Rem
		bbdoc: ReverbPan2
		about: 
		End Rem
		Field ReverbPan2:Float
		Rem
		bbdoc: ReverbPan3
		about: 
		End Rem
		Field ReverbPan3:Float
		Rem
		bbdoc: EchoTime
		about: 
		End Rem
		Field EchoTime:Float
		Rem
		bbdoc: EchoDepth
		about: 
		End Rem
		Field EchoDepth:Float
		Rem
		bbdoc: ModulationTime
		about: 
		End Rem
		Field ModulationTime:Float
		Rem
		bbdoc: ModulationDepth
		about: 
		End Rem
		Field ModulationDepth:Float
		Rem
		bbdoc: AirAbsorptionHF
		about: 
		End Rem
		Field AirAbsorptionHF:Float
		Rem
		bbdoc: HFReference
		about: 
		End Rem
		Field HFReference:Float
		Rem
		bbdoc: LFReference
		about: 
		End Rem
		Field LFReference:Float
		Rem
		bbdoc: RoomRolloffFactor
		about: 
		End Rem
		Field RoomRolloffFactor:Float
		Rem
		bbdoc: Diffusion
		about: 
		End Rem
		Field Diffusion:Float
		Rem
		bbdoc: Density
		about: 
		End Rem
		Field Density:Float
		Rem
		bbdoc: Flags
		about: 
		End Rem
		Field Flags:Int
	End Type
	
	Rem
	bbdoc: 
	End Rem
	Type TFMODReverbChannelProperties
		Field Direct:Int
		Field DirectHF:Int
		Field Room:Int
		Field RoomHF:Int
		Field Obstruction:Int
		Field ObstructionLFRatio:Float
		Field Occlusion:Int
		Field OcclusionLFRatio:Float
		Field OcclusionRoomRatio:Float
		Field OcclusionDirectRatio:Float
		Field Exclusion:Int
		Field ExclusionLFRatio:Float
		Field OutsideVolumeHF:Int
		Field DopplerFactor:Float
		Field RolloffFactor:Float
		Field RoomRolloffFactor:Float
		Field AirAbsorptionFactor:Float
		Field Flags:Int
End Type
	
End Extern

Const FMOD_CHANNEL_FREE:Int = -1      ' For a channel index, FMOD chooses a free voice using the priority system. */
Const FMOD_CHANNEL_REUSE:Int = -2     ' For a channel index, re-use the channel handle that was passed in. */

Rem
bbdoc: All platforms - Initialize normally 
End Rem
Const FMOD_INIT_NORMAL:Int = $00000000
Rem
bbdoc: All platforms - No stream thread is created internally.
about: Streams are driven from System::update.  Mainly used with non-realtime outputs. 
End Rem
Const FMOD_INIT_STREAM_FROM_UPDATE:Int = $00000001
Rem
bbdoc: All platforms - FMOD will treat +X as right, +Y as up and +Z as backwards (towards you). 
End Rem
Const FMOD_INIT_3D_RIGHTHANDED:Int = $00000002
Rem
bbdoc: All platforms - Disable software mixer to save memory.
about: Anything created with FMOD_SOFTWARE will fail and DSP will not work. 
End Rem
Const FMOD_INIT_SOFTWARE_DISABLE:Int = $00000004
Rem
bbdoc: All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which is automatically used when Channel::set3DOcclusion is used or the geometry API. 
End Rem
Const FMOD_INIT_SOFTWARE_OCCLUSION:Int = $00000008
Rem
bbdoc: All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which causes sounds to sound duller when the sound goes behind the listener.
about: Use System::setAdvancedSettings to adjust cutoff frequency. 
End Rem
Const FMOD_INIT_SOFTWARE_HRTF:Int = $00000010
Rem
bbdoc: All platforms - SFX reverb is run using 22/24khz delay buffers, halving the memory required. 
End Rem
Const FMOD_INIT_SOFTWARE_REVERB_LOWMEM:Int = $00000040
Rem
bbdoc: All platforms - Enable TCP/IP based host which allows FMOD Designer or FMOD Profiler to connect to it, and view memory, CPU and the DSP network graph in real-time. 
End Rem
Const FMOD_INIT_ENABLE_PROFILE:Int = $00000020
Rem
bbdoc: All platforms - Any sounds that are 0 volume will go virtual and not be processed except for having their positions updated virtually.
about: Use System::setAdvancedSettings to adjust what volume besides zero to switch to virtual at. 
End Rem
Const FMOD_INIT_VOL0_BECOMES_VIRTUAL:Int = $00000080
Rem
bbdoc: Win32 Vista only - for WASAPI output - Enable exclusive access to hardware, lower latency at the expense of excluding other applications from accessing the audio hardware. 
End Rem
Const FMOD_INIT_WASAPI_EXCLUSIVE:Int = $00000100
Rem
bbdoc: Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use simple stereo panning/doppler/attenuation when 3D hardware acceleration is not present. 
End Rem
Const FMOD_INIT_DSOUND_HRTFNONE:Int = $00000200
Rem
bbdoc: Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use a slightly higher quality algorithm when 3D hardware acceleration is not present. 
End Rem
Const FMOD_INIT_DSOUND_HRTFLIGHT:Int = $00000400
Rem
bbdoc: Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use full quality 3D playback when 3d hardware acceleration is not present. 
End Rem
Const FMOD_INIT_DSOUND_HRTFFULL:Int = $00000800
Rem
bbdoc: PS2 only - Disable reverb on CORE 0 to regain 256k SRAM. 
End Rem
Const FMOD_INIT_PS2_DISABLECORE0REVERB:Int = $00010000
Rem
bbdoc: PS2 only - Disable reverb on CORE 1 to regain 256k SRAM. 
End Rem
Const FMOD_INIT_PS2_DISABLECORE1REVERB:Int = $00020000
Rem
bbdoc: PS2 only - Disable FMOD's usage of the scratchpad. 
End Rem
Const FMOD_INIT_PS2_DONTUSESCRATCHPAD:Int = $00040000
Rem
bbdoc: PS2 only - Changes FMOD from using SPU DMA channel 0 for software mixing, and 1 for sound data upload/file streaming, to 1 and 0 respectively. 
End Rem
Const FMOD_INIT_PS2_SWAPDMACHANNELS:Int = $00080000
Rem
bbdoc: PS3 only - Prefer DTS over Dolby Digital if both are supported.
about: Note: 8 and 6 channel LPCM is always preferred over both DTS and Dolby Digital. 
End Rem
Const FMOD_INIT_PS3_PREFERDTS:Int = $00800000
Rem
bbdoc: PS3 only - Force PS3 system output mode to 2 channel LPCM. 
End Rem
Const FMOD_INIT_PS3_FORCE2CHLPCM:Int = $01000000
Rem
bbdoc: Xbox only - By default DirectSound attenuates all sound by 6db to avoid clipping/distortion.
about: CAUTION.  If you use this flag you are responsible for the final mix to make sure clipping / distortion doesn't happen. 
End Rem
Const FMOD_INIT_XBOX_REMOVEHEADROOM:Int = $00100000
Rem
bbdoc: Xbox 360 / PS3 - The "music" channelgroup which by default pauses when custom 360 dashboard / PS3 BGM music is played, can be changed to mute (therefore continues playing) instead of pausing, by using this flag. 
End Rem
Const FMOD_INIT_SYSTEM_MUSICMUTENOTPAUSE:Int = $00200000
Rem
bbdoc: Win32/Wii/PS3/Xbox/Xbox 360 - FMOD Mixer thread is woken up to do a mix when System::update is called rather than waking periodically on its own timer. 
End Rem
Const FMOD_INIT_SYNCMIXERWITHUPDATE:Int = $00400000
Rem
bbdoc: Win32/Mac/Linux/Solaris - Use DTS Neural surround downmixing from 7.1 if speakermode set to FMOD_SPEAKERMODE_STEREO or FMOD_SPEAKERMODE_5POINT1.
about: Internal DSP structure will be set to 7.1. 
End Rem
Const FMOD_INIT_DTS_NEURALSURROUND:Int = $02000000
Rem
bbdoc: All platforms - With the geometry engine, only process the closest polygon rather than accumulating all polygons the sound to listener line intersects. 
End Rem
Const FMOD_INIT_GEOMETRY_USECLOSEST:Int = $04000000
Rem
bbdoc: Win32 - Disables MyEars HRTF 7.1 downmixing.
about: MyEars will otherwise be disbaled if speakermode is not set to FMOD_SPEAKERMODE_STEREO or the data file is missing. 
End Rem
Const FMOD_INIT_DISABLE_MYEARS:Int = $08000000


Rem
bbdoc: Milliseconds.
End Rem
Const FMOD_TIMEUNIT_MS:Int = $00000001 
Rem
bbdoc: PCM Samples, related to milliseconds * samplerate / 1000.
End Rem
Const FMOD_TIMEUNIT_PCM:Int = $00000002 
Rem
bbdoc: Bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes).
End Rem
Const FMOD_TIMEUNIT_PCMBYTES:Int = $00000004 
Rem
bbdoc: Raw file bytes of (compressed) sound data (does not include headers).
about: Only used by Sound::getLength and Channel::getPosition.
End Rem
Const FMOD_TIMEUNIT_RAWBYTES:Int = $00000008 
Rem
bbdoc: Order in a sequenced module format (MOD/S3M/XM/IT).
about: Use Sound::getFormat to determine the PCM format being decoded to.
End Rem
Const FMOD_TIMEUNIT_MODORDER:Int = $00000100 
Rem
bbdoc: Current row in a sequenced module format (MOD/S3M/XM/IT).
about: Sound::getLength will return the number of rows in the currently playing or seeked to pattern.
End Rem
Const FMOD_TIMEUNIT_MODROW:Int = $00000200 
Rem
bbdoc: Current pattern in a sequenced module format (MOD/S3M/XM/IT).
about: Sound::getLength will return the number of patterns in the song and Channel::getPosition will return the currently playing pattern.
End Rem
Const FMOD_TIMEUNIT_MODPATTERN:Int = $00000400 
Rem
bbdoc: Currently playing subsound in a sentence time in milliseconds.
End Rem
Const FMOD_TIMEUNIT_SENTENCE_MS:Int = $00010000 
Rem
bbdoc: Currently playing subsound in a sentence time in PCM Samples, related to milliseconds * samplerate / 1000.
End Rem
Const FMOD_TIMEUNIT_SENTENCE_PCM:Int = $00020000 
Rem
bbdoc: Currently playing subsound in a sentence time in bytes, related to PCM samples * channels * datawidth (ie 16bit = 2 bytes).
End Rem
Const FMOD_TIMEUNIT_SENTENCE_PCMBYTES:Int = $00040000 
Rem
bbdoc: Currently playing sentence index according to the channel.
End Rem
Const FMOD_TIMEUNIT_SENTENCE:Int = $00080000 
Rem
bbdoc: Currently playing subsound index in a sentence.
End Rem
Const FMOD_TIMEUNIT_SENTENCE_SUBSOUND:Int = $00100000 
Rem
bbdoc: Time value as seen by buffered stream.
about: This is always ahead of audible time, and is only used for processing.
End Rem
Const FMOD_TIMEUNIT_BUFFERED:Int = $10000000 

Rem
bbdoc: FMOD_DEFAULT is a default sound type.
about: Equivalent to all the defaults listed below.  FMOD_LOOP_OFF, FMOD_2D, FMOD_HARDWARE. 
(Note - only Windows with a high spec soundcard, PS2, PSP, and Wii support FMOD_HARDWARE) 
End Rem
Const FMOD_DEFAULT:Int = $00000000
Rem
bbdoc: For non looping sounds. (DEFAULT).
about: Overrides FMOD_LOOP_NORMAL / FMOD_LOOP_BIDI. 
End Rem
Const FMOD_LOOP_OFF:Int = $00000001
Rem
bbdoc: For forward looping sounds. 
End Rem
Const FMOD_LOOP_NORMAL:Int = $00000002
Rem
bbdoc: For bidirectional looping sounds.
about: (only works on software mixed static sounds). 
End Rem
Const FMOD_LOOP_BIDI:Int = $00000004
Rem
bbdoc: Ignores any 3d processing. (DEFAULT). 
End Rem
Const FMOD_2D:Int = $00000008
Rem
bbdoc: Makes the sound positionable in 3D.
about: Overrides FMOD_2D. 
End Rem
Const FMOD_3D:Int = $00000010
Rem
bbdoc: Attempts to make sounds use hardware acceleration. (DEFAULT).
about: Note on platforms that don't support FMOD_HARDWARE (only Windows with a high spec soundcard, PS2, PSP, and Wii support FMOD_HARDWARE), this will be internally treated as FMOD_SOFTWARE. 
End Rem
Const FMOD_HARDWARE:Int = $00000020
Rem
bbdoc: Makes the sound be mixed by the FMOD CPU based software mixer.
about: Overrides FMOD_HARDWARE.  Use this for FFT, DSP, compressed sample support, 2D multi-speaker support and other software related features. 
End Rem
Const FMOD_SOFTWARE:Int = $00000040
Rem
bbdoc: Decompress at runtime, streaming from the source provided (ie from disk).
about: Overrides FMOD_CREATESAMPLE and FMOD_CREATECOMPRESSEDSAMPLE.  Note a stream can only be played once at a time due to a stream only having 1 stream buffer and file handle.  Open multiple streams to have them play concurrently. 
End Rem
Const FMOD_CREATESTREAM:Int = $00000080
Rem
bbdoc: Decompress at loadtime, decompressing or decoding whole file into memory as the target sample format (ie PCM).
about: Fastest for FMOD_SOFTWARE based playback and most flexible.  
End Rem
Const FMOD_CREATESAMPLE:Int = $00000100
Rem
bbdoc: Load MP2, MP3, IMAADPCM or XMA into memory and leave it compressed.
about: During playback the FMOD software mixer will decode it in realtime as a 'compressed sample'.  Can only be used in combination with FMOD_SOFTWARE.  Overrides FMOD_CREATESAMPLE.  If the sound data is not ADPCM, MPEG or XMA it will behave as if it was created with FMOD_CREATESAMPLE and decode the sound into PCM. 
End Rem
Const FMOD_CREATECOMPRESSEDSAMPLE:Int = $00000200
Rem
bbdoc: Opens a user created static sample or stream.
about: Use FMOD_CREATESOUNDEXINFO to specify format and/or read callbacks.  If a user created 'sample' is created with no read callback, the sample will be empty.  Use Sound::lock and Sound::unlock to place sound data into the sound if this is the case. 
End Rem
Const FMOD_OPENUSER:Int = $00000400
Rem
bbdoc: "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.
about: Use FMOD_CREATESOUNDEXINFO to specify length.  If used with FMOD_CREATESAMPLE or FMOD_CREATECOMPRESSEDSAMPLE, FMOD duplicates the memory into its own buffers.  Your own buffer can be freed after open.  If used with FMOD_CREATESTREAM, FMOD will stream out of the buffer whose pointer you passed in.  In this case, your own buffer should not be freed until you have finished with and released the stream.
End Rem
Const FMOD_OPENMEMORY:Int = $00000800
Rem
bbdoc: "name_or_data" will be interpreted as a pointer to memory instead of filename for creating sounds.
about: Use FMOD_CREATESOUNDEXINFO to specify length.  This differs to FMOD_OPENMEMORY in that it uses the memory as is, without duplicating the memory into its own buffers.  For Wii/PSP FMOD_HARDWARE supports this flag for the GCADPCM/VAG formats.  On other platforms FMOD_SOFTWARE must be used, as sound hardware on the other platforms (ie PC) cannot access main ram.  Cannot be freed after open, only after Sound::release.   Will not work if the data is compressed and FMOD_CREATECOMPRESSEDSAMPLE is not used. 
End Rem
Const FMOD_OPENMEMORY_POINT:Int = $10000000
Rem
bbdoc: Will ignore file format and treat as raw pcm.
about: Use FMOD_CREATESOUNDEXINFO to specify format.  Requires at least defaultfrequency, numchannels and format to be specified before it will open.  Must be little endian data. 
End Rem
Const FMOD_OPENRAW:Int = $00001000
Rem
bbdoc: Just open the file, dont prebuffer or read.
about: Good for fast opens for info, or when sound::readData is to be used. 
End Rem
Const FMOD_OPENONLY:Int = $00002000
Rem
bbdoc: For System::createSound - for accurate Sound::getLength/Channel::setPosition on VBR MP3, and MOD/S3M/XM/IT/MIDI files.
about: Scans file first, so takes longer to open. FMOD_OPENONLY does not affect this. 
End Rem
Const FMOD_ACCURATETIME:Int = $00004000
Rem
bbdoc: For corrupted / bad MP3 files.
about: This will search all the way through the file until it hits a valid MPEG header.  Normally only searches for 4k. 
End Rem
Const FMOD_MPEGSEARCH:Int = $00008000
Rem
bbdoc: For opening sounds and getting streamed subsounds (seeking) asyncronously.
about: Use Sound::getOpenState to poll the state of the sound as it opens or retrieves the subsound in the background. 
End Rem
Const FMOD_NONBLOCKING:Int = $00010000
Rem
bbdoc: Unique sound, can only be played one at a time 
End Rem
Const FMOD_UNIQUE:Int = $00020000
Rem
bbdoc: Make the sound's position, velocity and orientation relative to the listener. 
End Rem
Const FMOD_3D_HEADRELATIVE:Int = $00040000
Rem
bbdoc: Make the sound's position, velocity and orientation absolute (relative to the world). (DEFAULT) 
End Rem
Const FMOD_3D_WORLDRELATIVE:Int = $00080000
Rem
bbdoc: This sound will follow the standard logarithmic rolloff model where mindistance = full volume, maxdistance = where sound stops attenuating, and rolloff is fixed according to the global rolloff factor.  (DEFAULT) 
End Rem
Const FMOD_3D_LOGROLLOFF:Int = $00100000
Rem
bbdoc: This sound will follow a linear rolloff model where mindistance = full volume, maxdistance = silence.
about: Rolloffscale is ignored. 
End Rem
Const FMOD_3D_LINEARROLLOFF:Int = $00200000
Rem
bbdoc: This sound will follow a rolloff model defined by Sound::set3DCustomRolloff / Channel::set3DCustomRolloff.  
End Rem
Const FMOD_3D_CUSTOMROLLOFF:Int = $04000000
Rem
bbdoc: Is not affect by geometry occlusion.
about: If not specified in Sound::setMode, or Channel::setMode, the flag is cleared and it is affected by geometry again. 
End Rem
Const FMOD_3D_IGNOREGEOMETRY:Int = $40000000
Rem
bbdoc: For CDDA sounds only - use ASPI instead of NTSCSI to access the specified CD/DVD device. 
End Rem
Const FMOD_CDDA_FORCEASPI:Int = $00400000
Rem
bbdoc: For CDDA sounds only - perform jitter correction.
about: Jitter correction helps produce a more accurate CDDA stream at the cost of more CPU time. 
End Rem
Const FMOD_CDDA_JITTERCORRECT:Int = $00800000
Rem
bbdoc: Filename is double-byte unicode. 
End Rem
Const FMOD_UNICODE:Int = $01000000
Rem
bbdoc: Skips id3v2/asf/etc tag checks when opening a sound, to reduce seek/read overhead when opening files (helps with CD performance). 
End Rem
Const FMOD_IGNORETAGS:Int = $02000000
Rem
bbdoc: Removes some features from samples to give a lower memory overhead, like Sound::getName.
about: See remarks. 
End Rem
Const FMOD_LOWMEM:Int = $08000000
Rem
bbdoc: Load sound into the secondary RAM of supported platform. On PS3, sounds will be loaded into RSX/VRAM. 
End Rem
Const FMOD_LOADSECONDARYRAM:Int = $20000000
Rem
bbdoc: For sounds that start virtual (due to being quiet or low importance), instead of swapping back to audible, and playing at the correct offset according to time, this flag makes the sound play from the start. 
End Rem
Const FMOD_VIRTUAL_PLAYFROMSTART:Int = $80000000


Rem
bbdoc: No errors.
End Rem
Const FMOD_OK:Int = 0                       
Rem
bbdoc: Tried to call lock a second time before unlock was called.
End Rem
Const FMOD_ERR_ALREADYLOCKED:Int = 1        
Rem
bbdoc: Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound).
End Rem
Const FMOD_ERR_BADCOMMAND:Int = 2           
Rem
bbdoc: Neither NTSCSI nor ASPI could be initialised.
End Rem
Const FMOD_ERR_CDDA_DRIVERS:Int = 3         
Rem
bbdoc: An error occurred while initialising the CDDA subsystem.
End Rem
Const FMOD_ERR_CDDA_INIT:Int = 4            
Rem
bbdoc: Couldn't find the specified device.
End Rem
Const FMOD_ERR_CDDA_INVALID_DEVICE:Int = 5  
Rem
bbdoc: No audio tracks on the specified disc.
End Rem
Const FMOD_ERR_CDDA_NOAUDIO:Int = 6         
Rem
bbdoc: No CD/DVD devices were found. 
End Rem
Const FMOD_ERR_CDDA_NODEVICES:Int = 7       
Rem
bbdoc: No disc present in the specified drive.
End Rem
Const FMOD_ERR_CDDA_NODISC:Int = 8          
Rem
bbdoc: A CDDA read error occurred.
End Rem
Const FMOD_ERR_CDDA_READ:Int = 9            
Rem
bbdoc: Error trying to allocate a channel.
End Rem
Const FMOD_ERR_CHANNEL_ALLOC:Int = 10        
Rem
bbdoc: The specified channel has been reused to play another sound.
End Rem
Const FMOD_ERR_CHANNEL_STOLEN:Int = 11       
Rem
bbdoc: A Win32 COM related error occured.
about: COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly.
End Rem
Const FMOD_ERR_COM:Int = 12                  
Rem
bbdoc: DMA Failure.
about: See debug output for more information.
End Rem
Const FMOD_ERR_DMA:Int = 13                  
Rem
bbdoc: DSP connection error.
about: Connection possibly caused a cyclic dependancy.
End Rem
Const FMOD_ERR_DSP_CONNECTION:Int = 14       
Rem
bbdoc: DSP Format error.
about: A DSP unit may have attempted to connect to this network with the wrong format.
End Rem
Const FMOD_ERR_DSP_FORMAT:Int = 15           
Rem
bbdoc: DSP connection error.
about: Couldn't find the DSP unit specified.
End Rem
Const FMOD_ERR_DSP_NOTFOUND:Int = 16         
Rem
bbdoc: DSP error.
about: Cannot perform this operation while the network is in the middle of running.
This will most likely happen if a connection or disconnection is attempted in a DSP callback.
End Rem
Const FMOD_ERR_DSP_RUNNING:Int = 17          
Rem
bbdoc: DSP connection error.
about: The unit being connected to or disconnected should only have 1 input or output.
End Rem
Const FMOD_ERR_DSP_TOOMANYCONNECTIONS:Int = 18
Rem
bbdoc: Error loading file.
End Rem
Const FMOD_ERR_FILE_BAD:Int = 19             
Rem
bbdoc: Couldn't perform seek operation.
about: This is a limitation of the medium (ie netstreams) or the file format.
End Rem
Const FMOD_ERR_FILE_COULDNOTSEEK:Int = 20    
Rem
bbdoc: Media was ejected while reading.
End Rem
Const FMOD_ERR_FILE_DISKEJECTED:Int = 21     
Rem
bbdoc: End of file unexpectedly reached while trying to read essential data (truncated data?).
End Rem
Const FMOD_ERR_FILE_EOF:Int = 22             
Rem
bbdoc: File not found.
End Rem
Const FMOD_ERR_FILE_NOTFOUND:Int = 23        
Rem
bbdoc: Unwanted file access occured.
End Rem
Const FMOD_ERR_FILE_UNWANTED:Int = 24        
Rem
bbdoc: Unsupported file or audio format.
End Rem
Const FMOD_ERR_FORMAT:Int = 25               
Rem
bbdoc: A HTTP error occurred.
about: This is a catch-all for HTTP errors not listed elsewhere.
End Rem
Const FMOD_ERR_HTTP:Int = 26                 
Rem
bbdoc: The specified resource requires authentication or is forbidden.
End Rem
Const FMOD_ERR_HTTP_ACCESS:Int = 27          
Rem
bbdoc: Proxy authentication is required to access the specified resource.
End Rem
Const FMOD_ERR_HTTP_PROXY_AUTH:Int = 28      
Rem
bbdoc: A HTTP server error occurred.
End Rem
Const FMOD_ERR_HTTP_SERVER_ERROR:Int = 29    
Rem
bbdoc: The HTTP request timed out.
End Rem
Const FMOD_ERR_HTTP_TIMEOUT:Int = 30         
Rem
bbdoc: FMOD was not initialized correctly to support this function.
End Rem
Const FMOD_ERR_INITIALIZATION:Int = 31       
Rem
bbdoc: Cannot call this command after System::init.
End Rem
Const FMOD_ERR_INITIALIZED:Int = 32          
Rem
bbdoc: An error occured that wasn't supposed to.
about: Contact support.
End Rem
Const FMOD_ERR_INTERNAL:Int = 33             
Rem
bbdoc: On Xbox 360, this memory address passed to FMOD must be physical, (ie allocated with XPhysicalAlloc.)
End Rem
Const FMOD_ERR_INVALID_ADDRESS:Int = 34      
Rem
bbdoc: Value passed in was a NaN, Inf or denormalized float.
End Rem
Const FMOD_ERR_INVALID_FLOAT:Int = 35        
Rem
bbdoc: An invalid object handle was used.
End Rem
Const FMOD_ERR_INVALID_HANDLE:Int = 36       
Rem
bbdoc: An invalid parameter was passed to this function.
End Rem
Const FMOD_ERR_INVALID_PARAM:Int = 37        
Rem
bbdoc: An invalid speaker was passed to this function based on the current speaker mode.
End Rem
Const FMOD_ERR_INVALID_SPEAKER:Int = 38      
Rem
bbdoc: The vectors passed in are not unit length, or perpendicular.
End Rem
Const FMOD_ERR_INVALID_VECTOR:Int = 39       
Rem
bbdoc: PS2 only.  fmodex.irx failed to initialize.
about: This is most likely because you forgot to load it.
End Rem
Const FMOD_ERR_IRX:Int = 40                  
Rem
bbdoc: Reached maximum audible playback count for this sound's soundgroup.
End Rem
Const FMOD_ERR_MAXAUDIBLE:Int = 41           
Rem
bbdoc: Not enough memory or resources.
End Rem
Const FMOD_ERR_MEMORY:Int = 42               
Rem
bbdoc: Can't use FMOD_OPENMEMORY_POINT on non PCM source data, or non mp3/xma/adpcm data if const FMOD_CREATECOMPRESSEDSAMPLE was used.
End Rem
Const FMOD_ERR_MEMORY_CANTPOINT:Int = 43     
Rem
bbdoc: PS2 only.  Not enough memory or resources on PlayStation 2 IOP ram.
End Rem
Const FMOD_ERR_MEMORY_IOP:Int = 44           
Rem
bbdoc: Not enough memory or resources on console sound ram.
End Rem
Const FMOD_ERR_MEMORY_SRAM:Int = 45          
Rem
bbdoc: Tried to call a command on a 3d sound when the command was meant for 2d sound.
End Rem
Const FMOD_ERR_NEEDS2D:Int = 46              
Rem
bbdoc: Tried to call a command on a 2d sound when the command was meant for 3d sound.
End Rem
Const FMOD_ERR_NEEDS3D:Int = 47              
Rem
bbdoc: Tried to use a feature that requires hardware support.
about: (ie trying to play a VAG compressed sound in software on PS2).
End Rem
Const FMOD_ERR_NEEDSHARDWARE:Int = 48        
Rem
bbdoc: Tried to use a feature that requires the software engine.
about: Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature.
End Rem
Const FMOD_ERR_NEEDSSOFTWARE:Int = 49        
Rem
bbdoc: Couldn't connect to the specified host.
End Rem
Const FMOD_ERR_NET_CONNECT:Int = 50          
Rem
bbdoc: A socket error occurred.
about: This is a catch-all for socket-related errors not listed elsewhere.
End Rem
Const FMOD_ERR_NET_SOCKET_ERROR:Int = 51     
Rem
bbdoc: The specified URL couldn't be resolved.
End Rem
Const FMOD_ERR_NET_URL:Int = 52              
Rem
bbdoc: Operation on a non-blocking socket could not complete immediately.
End Rem
Const FMOD_ERR_NET_WOULD_BLOCK:Int = 53      
Rem
bbdoc: Operation could not be performed because specified sound is not ready.
End Rem
Const FMOD_ERR_NOTREADY:Int = 54             
Rem
bbdoc: Error initializing output device, but more specifically, the output device is already in use and cannot be reused.
End Rem
Const FMOD_ERR_OUTPUT_ALLOCATED:Int = 55     
Rem
bbdoc: Error creating hardware sound buffer.
End Rem
Const FMOD_ERR_OUTPUT_CREATEBUFFER:Int = 56  
Rem
bbdoc: A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted.
End Rem
Const FMOD_ERR_OUTPUT_DRIVERCALL:Int = 57    
Rem
bbdoc: Error enumerating the available driver list.
about: List may be inconsistent due to a recent device addition or removal.
End Rem
Const FMOD_ERR_OUTPUT_ENUMERATION:Int = 58   
Rem
bbdoc: Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output).
End Rem
Const FMOD_ERR_OUTPUT_FORMAT:Int = 59        
Rem
bbdoc: Error initializing output device.
End Rem
Const FMOD_ERR_OUTPUT_INIT:Int = 60          
Rem
bbdoc: FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it.
End Rem
Const FMOD_ERR_OUTPUT_NOHARDWARE:Int = 61    
Rem
bbdoc: Attempted to create a software sound but no software channels were specified in System::init.
End Rem
Const FMOD_ERR_OUTPUT_NOSOFTWARE:Int = 62    
Rem
bbdoc: Panning only works with mono or stereo sound sources.
End Rem
Const FMOD_ERR_PAN:Int = 63                  
Rem
bbdoc: An unspecified error has been returned from a 3rd party plugin.
End Rem
Const FMOD_ERR_PLUGIN:Int = 64               
Rem
bbdoc: The number of allowed instances of a plugin has been exceeded.
End Rem
Const FMOD_ERR_PLUGIN_INSTANCES:Int = 65     
Rem
bbdoc: A requested output, dsp unit type or codec was not available.
End Rem
Const FMOD_ERR_PLUGIN_MISSING:Int = 66       
Rem
bbdoc: A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback)
End Rem
Const FMOD_ERR_PLUGIN_RESOURCE:Int = 67      
Rem
bbdoc: An error occured trying to initialize the recording device.
End Rem
Const FMOD_ERR_RECORD:Int = 68               
Rem
bbdoc: Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set.
about: Most likely because it is an invalid instance number, or another application has locked the EAX4 FX slot.
End Rem
Const FMOD_ERR_REVERB_INSTANCE:Int = 69      
Rem
bbdoc: This subsound is already being used by another sound, you cannot have more than one parent to a sound.
about: Null out the other parent's entry first.
End Rem
Const FMOD_ERR_SUBSOUND_ALLOCATED:Int = 70   
Rem
bbdoc: The subsound's mode bits do not match with the parent sound's mode bits.
about: See documentation for function that it was called with.
End Rem
Const FMOD_ERR_SUBSOUND_MODE:Int = 71        
Rem
bbdoc: The error occured because the sound referenced contains subsounds.
about: ie. you cannot play the parent sound as a static sample, only its subsounds.
End Rem
Const FMOD_ERR_SUBSOUNDS:Int = 72            
Rem
bbdoc: The specified tag could not be found or there are no tags.
End Rem
Const FMOD_ERR_TAGNOTFOUND:Int = 73          
Rem
bbdoc: The sound created exceeds the allowable input channel count.
about: This can be increased using the maxinputchannels parameter in System::setSoftwareFormat.
End Rem
Const FMOD_ERR_TOOMANYCHANNELS:Int = 74      
Rem
bbdoc: Something in FMOD hasn't been implemented when it should be! contact support!
End Rem
Const FMOD_ERR_UNIMPLEMENTED:Int = 75        
Rem
bbdoc: This command failed because System::init or System::setDriver was not called.
End Rem
Const FMOD_ERR_UNINITIALIZED:Int = 76        
Rem
bbdoc: A command issued was not supported by this object.
about: Possibly a plugin without certain callbacks specified.
End Rem
Const FMOD_ERR_UNSUPPORTED:Int = 77          
Rem
bbdoc: An error caused by System::update occured.
End Rem
Const FMOD_ERR_UPDATE:Int = 78               
Rem
bbdoc: The version number of this file format is not supported.
End Rem
Const FMOD_ERR_VERSION:Int = 79              
Rem
bbdoc: An Event failed to be retrieved, most likely due to 'just fail' being specified as the max playbacks behavior.
End Rem
Const FMOD_ERR_EVENT_FAILED:Int = 80         
Rem
bbdoc: An error occured that wasn't supposed to.
about: See debug log for reason.
End Rem
Const FMOD_ERR_EVENT_INTERNAL:Int = 81       
Rem
bbdoc: Can't execute this command on an EVENT_INFOONLY event.
End Rem
Const FMOD_ERR_EVENT_INFOONLY:Int = 82       
Rem
bbdoc: Event failed because 'Max streams' was hit when FMOD_INIT_FAIL_ON_MAXSTREAMS was specified.
End Rem
Const FMOD_ERR_EVENT_MAXSTREAMS:Int = 83     
Rem
bbdoc: FSB mismatches the FEV it was compiled with or FEV was built for a different platform.
End Rem
Const FMOD_ERR_EVENT_MISMATCH:Int = 84       
Rem
bbdoc: A category with the same name already exists.
End Rem
Const FMOD_ERR_EVENT_NAMECONFLICT:Int = 85   
Rem
bbdoc: The requested event, event group, event category or event property could not be found.
End Rem
Const FMOD_ERR_EVENT_NOTFOUND:Int = 86       
Rem
bbdoc: Tried to call a function on a complex event that's only supported by simple events.
End Rem
Const FMOD_ERR_EVENT_NEEDSSIMPLE:Int = 87    
Rem
bbdoc: An event with the same GUID already exists.
End Rem
Const FMOD_ERR_EVENT_GUIDCONFLICT:Int = 88   
Rem
bbdoc: The specified project has already been loaded.
about: Having multiple copies of the same project loaded simultaneously is forbidden.
End Rem
Const FMOD_ERR_EVENT_ALREADY_LOADED:Int = 89 
Rem
bbdoc: Music system is not initialized probably because no music data is loaded.
End Rem
Const FMOD_ERR_MUSIC_UNINITIALIZED:Int = 90  
Rem
bbdoc: The requested music entity could not be found.
End Rem
Const FMOD_ERR_MUSIC_NOTFOUND:Int = 91       


Const FMOD_DSP_TYPE_UNKNOWN:Int = 0
Const FMOD_DSP_TYPE_MIXER:Int = 1
Const FMOD_DSP_TYPE_OSCILLATOR:Int = 2
Const FMOD_DSP_TYPE_LOWPASS:Int = 3
Const FMOD_DSP_TYPE_ITLOWPASS:Int = 4
Const FMOD_DSP_TYPE_HIGHPASS:Int = 5
Const FMOD_DSP_TYPE_ECHO:Int = 6
Const FMOD_DSP_TYPE_FLANGE:Int = 7
Const FMOD_DSP_TYPE_DISTORTION:Int = 8
Const FMOD_DSP_TYPE_NORMALIZE:Int = 9
Const FMOD_DSP_TYPE_PARAMEQ:Int = 10
Const FMOD_DSP_TYPE_PITCHSHIFT:Int = 11
Const FMOD_DSP_TYPE_CHORUS:Int = 12
Const FMOD_DSP_TYPE_REVERB:Int = 13
Const FMOD_DSP_TYPE_VSTPLUGIN:Int = 14
Const FMOD_DSP_TYPE_WINAMPPLUGIN:Int = 15
Const FMOD_DSP_TYPE_ITECHO:Int = 16
Const FMOD_DSP_TYPE_COMPRESSOR:Int = 17
Const FMOD_DSP_TYPE_SFXREVERB:Int = 18
Const FMOD_DSP_TYPE_LOWPASS_SIMPLE:Int = 19

Rem
bbdoc: Echo delay in ms.
about: 10  to 5000.  Default = 500.
End Rem
Const FMOD_DSP_ECHO_DELAY:Int = 0      
Rem
bbdoc: Echo decay per delay.
about: 0 to 1.  1.0 = No decay, 0.0 = total decay (ie simple 1 line delay).  Default = 0.5.
End Rem
Const FMOD_DSP_ECHO_DECAYRATIO:Int = 1 
Rem
bbdoc: Maximum channels supported.
about: 0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  See remarks for more.  Default = 0.  It is suggested to leave at 0!
End Rem
Const FMOD_DSP_ECHO_MAXCHANNELS:Int = 2
Rem
bbdoc: Volume of original signal to pass to output.
about: 0.0 to 1.0. Default = 1.0.
End Rem
Const FMOD_DSP_ECHO_DRYMIX:Int = 3     
Rem
bbdoc: Volume of echo signal to pass to output.
about: 0.0 to 1.0. Default = 1.0.
End Rem
Const FMOD_DSP_ECHO_WETMIX:Int = 4      

Rem
bbdoc: Distortion value.
about: 0.0 To 1.0.  Default = 0.5.
End Rem
Const FMOD_DSP_DISTORTION_LEVEL:Int = 0   

Rem
bbdoc: Frequency center.
about: 20.0 to 22000.0.  Default = 8000.0.
End Rem
Const FMOD_DSP_PARAMEQ_CENTER:Int = 0    
Rem
bbdoc: Octave range around the center frequency to filter.
about: 0.2 to 5.0.  Default = 1.0.
End Rem
Const FMOD_DSP_PARAMEQ_BANDWIDTH:Int = 1 
Rem
bbdoc: Frequency Gain.
about: 0.05 to 3.0.  Default = 1.0. 
End Rem
Const FMOD_DSP_PARAMEQ_GAIN:Int = 2       

Rem
bbdoc: Waveform type.
about: 0 = sine.  1 = square. 2 = sawup. 3 = sawdown. 4 = triangle. 5 = noise.
End Rem
Const FMOD_DSP_OSCILLATOR_TYPE:Int = 0  
Rem
bbdoc: Frequency of the sinewave in hz.
about: 1.0 to 22000.0.  Default = 220.0.
End Rem
Const FMOD_DSP_OSCILLATOR_RATE:Int = 1   

Rem
bbdoc: Lowpass cutoff frequency in hz.
about: 1.0 to 22000.0.  Default = 5000.0/
End Rem
Const FMOD_DSP_ITLOWPASS_CUTOFF:Int = 0   
Rem
bbdoc: Lowpass resonance Q value.
about: 0.0 to 127.0.  Default = 1.0.
End Rem
Const FMOD_DSP_ITLOWPASS_RESONANCE:Int = 1 

Rem
bbdoc: Highpass cutoff frequency in hz.
about: 1.0 to output 22000.0.  Default = 5000.0.
End Rem
Const FMOD_DSP_HIGHPASS_CUTOFF:Int = 0   
Rem
bbdoc: Highpass resonance Q value.
about: 1.0 to 10.0.  Default = 1.0.
End Rem
Const FMOD_DSP_HIGHPASS_RESONANCE:Int = 1 

Rem
bbdoc: Volume of original signal to pass to output.
about: 0.0 to 1.0. Default = 0.45.
End Rem
Const FMOD_DSP_FLANGE_DRYMIX:Int = 0     
Rem
bbdoc: Volume of flange signal to pass to output.
about: 0.0 to 1.0. Default = 0.55.
End Rem
Const FMOD_DSP_FLANGE_WETMIX:Int = 1     
Rem
bbdoc: Flange depth.
about: 0.01 to 1.0.  Default = 1.0.
End Rem
Const FMOD_DSP_FLANGE_DEPTH:Int = 2      
Rem
bbdoc: Flange speed in hz.
about: 0.0 to 20.0.  Default = 0.1.
End Rem
Const FMOD_DSP_FLANGE_RATE:Int = 3       

Rem
bbdoc: Time to ramp the silence to full in ms.
about: 0.0 to 20000.0. Default = 5000.0.
End Rem
Const FMOD_DSP_NORMALIZE_FADETIME:Int = 0   
Rem
bbdoc: Lower volume range threshold to ignore.
about: 0.0 to 1.0.  Default = 0.1.  Raise higher to stop amplification of very quiet signals.
End Rem
Const FMOD_DSP_NORMALIZE_THRESHHOLD:Int = 1 
Rem
bbdoc: Maximum amplification allowed.
about: 1.0 to 100000.0.  Default = 20.0.  1.0 = no amplifaction, higher values allow more boost.
End Rem
Const FMOD_DSP_NORMALIZE_MAXAMP:Int = 2      

Rem
bbdoc: Pitch value.
about: 0.5 to 2.0.  Default = 1.0. 0.5 = one octave down, 2.0 = one octave up.  1.0 does not change the pitch.
End Rem
Const FMOD_DSP_PITCHSHIFT_PITCH:Int = 0      
Rem
bbdoc: FFT window size.
about: 256, 512, 1024, 2048, 4096.  Default = 1024.  Increase this to reduce 'smearing'.  This effect is a warbling sound similar to when an mp3 is encoded at very low bitrates.
End Rem
Const FMOD_DSP_PITCHSHIFT_FFTSIZE:Int = 1    
' Removed.  Do not use.  FMOD now uses 4 overlaps and cannot be changed.
Const FMOD_DSP_PITCHSHIFT_OVERLAP:Int = 2    
Rem
bbdoc: Maximum channels supported.
about: 0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  See remarks for more.  Default = 0.  It is suggested to leave at 0!
End Rem
Const FMOD_DSP_PITCHSHIFT_MAXCHANNELS:Int = 3 

Rem
bbdoc: Volume of original signal to pass to output.
about: 0.0 to 1.0. Default = 0.5.
End Rem
Const FMOD_DSP_CHORUS_DRYMIX:Int = 0   
Rem
bbdoc: Volume of 1st chorus tap.
about: 0.0 to 1.0.  Default = 0.5.
End Rem
Const FMOD_DSP_CHORUS_WETMIX1:Int = 1  
Rem
bbdoc: Volume of 2nd chorus tap.
about: This tap is 90 degrees out of phase of the first tap.  0.0 to 1.0.  Default = 0.5.
End Rem
Const FMOD_DSP_CHORUS_WETMIX2:Int = 2  
Rem
bbdoc: Volume of 3rd chorus tap.
about: This tap is 90 degrees out of phase of the second tap.  0.0 to 1.0.  Default = 0.5.
End Rem
Const FMOD_DSP_CHORUS_WETMIX3:Int = 3  
Rem
bbdoc: Chorus delay in ms.
about: 0.1 to 100.0.  Default = 40.0 ms.
End Rem
Const FMOD_DSP_CHORUS_DELAY:Int = 4    
Rem
bbdoc: Chorus modulation rate in hz.
about: 0.0 to 20.0.  Default = 0.8 hz.
End Rem
Const FMOD_DSP_CHORUS_RATE:Int = 5     
Rem
bbdoc: Chorus modulation depth.
about: 0.0 to 1.0.  Default = 0.03.
End Rem
Const FMOD_DSP_CHORUS_DEPTH:Int = 6    
Rem
bbdoc: Chorus feedback.
about: Controls how much of the wet signal gets fed back into the chorus buffer.  0.0 to 1.0.  Default = 0.0.
End Rem
Const FMOD_DSP_CHORUS_FEEDBACK:Int = 7 

Rem
bbdoc: Roomsize.
about: 0.0 to 1.0.  Default = 0.5
End Rem
Const FMOD_DSP_REVERB_ROOMSIZE:Int = 0
Rem
bbdoc: Damp.
about: 0.0 to 1.0.  Default = 0.5
End Rem
Const FMOD_DSP_REVERB_DAMP:Int = 1    
Rem
bbdoc: Wet mix.
about: 0.0 to 1.0.  Default = 0.33
End Rem
Const FMOD_DSP_REVERB_WETMIX:Int = 2  
Rem
bbdoc: Dry mix.
about: 0.0 to 1.0.  Default = 0.66
End Rem
Const FMOD_DSP_REVERB_DRYMIX:Int = 3  
Rem
bbdoc: Stereo width.
about: 0.0 to 1.0.  Default = 1.0
End Rem
Const FMOD_DSP_REVERB_WIDTH:Int = 4   
Rem
bbdoc: Mode.
about: 0 (normal), 1 (freeze).  Default = 0
End Rem
Const FMOD_DSP_REVERB_MODE:Int = 5    

Rem
bbdoc: Ratio of wet (processed) signal to dry (unprocessed) signal.
about: Must be in the range from 0.0 through 100.0 (all wet). The default value is 50.
End Rem
Const FMOD_DSP_ITECHO_WETDRYMIX:Int = 0     
Rem
bbdoc: Percentage of output fed back into input, in the range from 0.0 through 100.0.
about: The default value is 50.
End Rem
Const FMOD_DSP_ITECHO_FEEDBACK:Int = 1      
Rem
bbdoc: Delay for left channel, in milliseconds, in the range from 1.0 through 2000.0.
about: The default value is 500 ms.
End Rem
Const FMOD_DSP_ITECHO_LEFTDELAY:Int = 2     
Rem
bbdoc: Delay for right channel, in milliseconds, in the range from 1.0 through 2000.0.
about: The default value is 500 ms.
End Rem
Const FMOD_DSP_ITECHO_RIGHTDELAY:Int = 3    
Rem
bbdoc: Value that specifies whether to swap left and right delays with each successive echo.
about: The default value is zero, meaning no swap. Possible values are defined as 0.0 (equivalent to FALSE) and 1.0 (equivalent to TRUE).  CURRENTLY NOT SUPPORTED.
End Rem
Const FMOD_DSP_ITECHO_PANDELAY:Int = 4      

Rem
bbdoc: Threshold level (dB) in the range from -60 through 0.
about: The default value is 0.
End Rem
Const FMOD_DSP_COMPRESSOR_THRESHOLD:Int = 0 
Rem
bbdoc: Gain reduction attack time (milliseconds), in the range from 10 through 200.
about: The default value is 50.
End Rem
Const FMOD_DSP_COMPRESSOR_ATTACK:Int = 1    
Rem
bbdoc: Gain reduction release time (milliseconds), in the range from 20 through 1000.
about: The default value is 50.
End Rem
Const FMOD_DSP_COMPRESSOR_RELEASE:Int = 2   
Rem
bbdoc: Make-up gain (dB) applied after limiting, in the range from 0 through 30.
about: The default value is 0.
End Rem
Const FMOD_DSP_COMPRESSOR_GAINMAKEUP:Int = 3 

Rem
bbdoc: Dry Level.
about: Mix level of dry signal in output in mB.  Ranges from -10000.0 to 0.0.  Default is 0.
End Rem
Const FMOD_DSP_SFXREVERB_DRYLEVEL:Int = 0           
Rem
bbdoc: Room.
about: Room effect level at low frequencies in mB.  Ranges from -10000.0 to 0.0.  Default is 0.0.
End Rem
Const FMOD_DSP_SFXREVERB_ROOM:Int = 1               
Rem
bbdoc: Room HF.
about: Room effect high-frequency level re. low frequency level in mB.  Ranges from -10000.0 to 0.0.  Default is 0.0.
End Rem
Const FMOD_DSP_SFXREVERB_ROOMHF:Int = 2             
Rem
bbdoc: Room Rolloff.
about: Like DS3D flRolloffFactor but for room effect.  Ranges from 0.0 to 10.0. Default is 10.0
End Rem
Const FMOD_DSP_SFXREVERB_ROOMROLLOFFFACTOR:Int = 3  
Rem
bbdoc: Decay Time.
about: Reverberation decay time at low-frequencies in seconds.  Ranges from 0.1 to 20.0. Default is 1.0.
End Rem
Const FMOD_DSP_SFXREVERB_DECAYTIME:Int = 4          
Rem
bbdoc: Decay HF Ratio.
about: High-frequency to low-frequency decay time ratio.  Ranges from 0.1 to 2.0. Default is 0.5.
End Rem
Const FMOD_DSP_SFXREVERB_DECAYHFRATIO:Int = 5       
Rem
bbdoc: Reflections.
about: Early reflections level relative to room effect in mB.  Ranges from -10000.0 to 1000.0.  Default is -10000.0.
End Rem
Const FMOD_DSP_SFXREVERB_REFLECTIONSLEVEL:Int = 6   
Rem
bbdoc: Reflect Delay.
about: Delay time of first reflection in seconds.  Ranges from 0.0 to 0.3.  Default is 0.02.
End Rem
Const FMOD_DSP_SFXREVERB_REFLECTIONSDELAY:Int = 7   
Rem
bbdoc: Reverb.
about: Late reverberation level relative to room effect in mB.  Ranges from -10000.0 to 2000.0.  Default is 0.0.
End Rem
Const FMOD_DSP_SFXREVERB_REVERBLEVEL:Int = 8        
Rem
bbdoc: Reverb Delay.
about: Late reverberation delay time relative to first reflection in seconds.  Ranges from 0.0 to 0.1.  Default is 0.04.
End Rem
Const FMOD_DSP_SFXREVERB_REVERBDELAY:Int = 9        
Rem
bbdoc: Diffusion.
about: Reverberation diffusion (echo density) in percent.  Ranges from 0.0 to 100.0.  Default is 100.0.
End Rem
Const FMOD_DSP_SFXREVERB_DIFFUSION:Int = 10         
Rem
bbdoc: Density.
about: Reverberation density (modal density) in percent.  Ranges from 0.0 to 100.0.  Default is 100.0.
End Rem
Const FMOD_DSP_SFXREVERB_DENSITY:Int = 11           
Rem
bbdoc: HF Reference.
about: Reference high frequency in Hz.  Ranges from 20.0 to 20000.0. Default is 5000.0.
End Rem
Const FMOD_DSP_SFXREVERB_HFREFERENCE:Int = 12       
Rem
bbdoc: Room LF.
about: Room effect low-frequency level in mB.  Ranges from -10000.0 to 0.0.  Default is 0.0.
End Rem
Const FMOD_DSP_SFXREVERB_ROOMLF:Int = 13            
Rem
bbdoc: LF Reference.
about: Reference low-frequency in Hz.  Ranges from 20.0 to 1000.0. Default is 250.0.
End Rem
Const FMOD_DSP_SFXREVERB_LFREFERENCE:Int = 14       

Rem
bbdoc: Lowpass cutoff frequency in hz.
about: 10.0 to 22000.0.  Default = 5000.0
End Rem
Const FMOD_DSP_LOWPASS_SIMPLE_CUTOFF:Int = 0    

Rem
bbdoc: Lowpass cutoff frequency in hz.
about: 10.0 to 22000.0.  Default = 5000.0.
End Rem
Const FMOD_DSP_LOWPASS_CUTOFF:Int = 0   
Rem
bbdoc: Lowpass resonance Q value.
about: 1.0 to 10.0.  Default = 1.0.
End Rem
Const FMOD_DSP_LOWPASS_RESONANCE:Int = 1


Const FMOD_SOUND_FORMAT_NONE:Int = 0
Const FMOD_SOUND_FORMAT_PCM8:Int = 1
Const FMOD_SOUND_FORMAT_PCM16:Int = 2
Const FMOD_SOUND_FORMAT_PCM24:Int = 3
Const FMOD_SOUND_FORMAT_PCM32:Int = 4
Const FMOD_SOUND_FORMAT_PCMFLOAT:Int = 5
Const FMOD_SOUND_FORMAT_GCADPCM:Int = 6
Const FMOD_SOUND_FORMAT_IMAADPCM:Int = 7
Const FMOD_SOUND_FORMAT_VAG:Int = 8
Const FMOD_SOUND_FORMAT_XMA:Int = 9
Const FMOD_SOUND_FORMAT_MPEG:Int = 10

Rem
bbdoc: 3rd party / unknown plugin format. 
End Rem
Const FMOD_SOUND_TYPE_UNKNOWN:Int = 0         
Rem
bbdoc: AAC.  Currently unsupported. 
End Rem
Const FMOD_SOUND_TYPE_AAC:Int = 1             
Rem
bbdoc: AIFF.
End Rem
Const FMOD_SOUND_TYPE_AIFF:Int = 2            
Rem
bbdoc: Microsoft Advanced Systems Format (ie WMA/ASF/WMV). 
End Rem
Const FMOD_SOUND_TYPE_ASF:Int = 3             
Rem
bbdoc: Sony ATRAC 3 format 
End Rem
Const FMOD_SOUND_TYPE_AT3:Int = 4             
Rem
bbdoc: Digital CD audio. 
End Rem
Const FMOD_SOUND_TYPE_CDDA:Int = 5            
Rem
bbdoc: Sound font / downloadable sound bank. 
End Rem
Const FMOD_SOUND_TYPE_DLS:Int = 6             
Rem
bbdoc: FLAC lossless codec. 
End Rem
Const FMOD_SOUND_TYPE_FLAC:Int = 7            
Rem
bbdoc: FMOD Sample Bank. 
End Rem
Const FMOD_SOUND_TYPE_FSB:Int = 8             
Rem
bbdoc: GameCube ADPCM
End Rem
Const FMOD_SOUND_TYPE_GCADPCM:Int = 9         
Rem
bbdoc: Impulse Tracker.
End Rem
Const FMOD_SOUND_TYPE_IT:Int = 10             
Rem
bbdoc: MIDI. 
End Rem
Const FMOD_SOUND_TYPE_MIDI:Int = 11           
Rem
bbdoc: Protracker / Fasttracker MOD. 
End Rem
Const FMOD_SOUND_TYPE_MOD:Int = 12            
Rem
bbdoc: MP2/MP3 MPEG. 
End Rem
Const FMOD_SOUND_TYPE_MPEG:Int = 13           
Rem
bbdoc: Ogg vorbis.
End Rem
Const FMOD_SOUND_TYPE_OGGVORBIS:Int = 14      
Rem
bbdoc: Information only from ASX/PLS/M3U/WAX playlists
End Rem
Const FMOD_SOUND_TYPE_PLAYLIST:Int = 15       
Rem
bbdoc: Raw PCM data.
End Rem
Const FMOD_SOUND_TYPE_RAW:Int = 16            
Rem
bbdoc: ScreamTracker 3. 
End Rem
Const FMOD_SOUND_TYPE_S3M:Int = 17            
Rem
bbdoc: Sound font 2 format. 
End Rem
Const FMOD_SOUND_TYPE_SF2:Int = 18            
Rem
bbdoc: User created sound. 
End Rem
Const FMOD_SOUND_TYPE_USER:Int = 19           
Rem
bbdoc: Microsoft WAV.
End Rem
Const FMOD_SOUND_TYPE_WAV:Int = 20            
Rem
bbdoc: FastTracker 2 XM.
End Rem
Const FMOD_SOUND_TYPE_XM:Int = 21             
Rem
bbdoc: Xbox360 XMA 
End Rem
Const FMOD_SOUND_TYPE_XMA:Int = 22            
Rem
bbdoc: PlayStation 2 / PlayStation Portable adpcm VAG format.
End Rem
Const FMOD_SOUND_TYPE_VAG:Int = 23            
Rem
bbdoc: Maximum number of sound types supported. 
End Rem
Const FMOD_SOUND_TYPE_MAX:Int = 24            
   
Rem
bbdoc: 'EnvSize' affects reverberation decay time
End Rem
Const FMOD_REVERB_FLAGS_DECAYTIMESCALE:Int = $00000001
Rem
bbdoc: 'EnvSize' affects reflection level
End Rem
Const FMOD_REVERB_FLAGS_REFLECTIONSSCALE:Int = $00000002
Rem
bbdoc: 'EnvSize' affects initial reflection delay time
End Rem
Const FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE:Int = $00000004
Rem
bbdoc: 'EnvSize' affects reflections level
End Rem
Const FMOD_REVERB_FLAGS_REVERBSCALE:Int = $00000008
Rem
bbdoc: 'EnvSize' affects late reverberation delay time
End Rem
Const FMOD_REVERB_FLAGS_REVERBDELAYSCALE:Int = $00000010
Rem
bbdoc: AirAbsorptionHF affects DecayHFRatio
End Rem
Const FMOD_REVERB_FLAGS_DECAYHFLIMIT:Int = $00000020
Rem
bbdoc: 'EnvSize' affects echo time
End Rem
Const FMOD_REVERB_FLAGS_ECHOTIMESCALE:Int = $00000040
Rem
bbdoc: 'EnvSize' affects modulation time
End Rem
Const FMOD_REVERB_FLAGS_MODULATIONTIMESCALE:Int = $00000080
Rem
bbdoc: PS2 Only - Reverb is applied to CORE0 (hw voices 0-23)
End Rem
Const FMOD_REVERB_FLAGS_CORE0:Int = $00000100
Rem
bbdoc: PS2 Only - Reverb is applied to CORE1 (hw voices 24-47)
End Rem
Const FMOD_REVERB_FLAGS_CORE1:Int = $00000200
Rem
bbdoc: GameCube/Wii. Use high quality reverb
End Rem
Const FMOD_REVERB_FLAGS_HIGHQUALITYREVERB:Int = $00000400
Rem
bbdoc: GameCube/Wii. Use high quality DPL2 reverb
End Rem
Const FMOD_REVERB_FLAGS_HIGHQUALITYDPL2REVERB:Int = $00000800

Const FMOD_REVERB_FLAGS_DEFAULT:Int = FMOD_REVERB_FLAGS_DECAYTIMESCALE | FMOD_REVERB_FLAGS_REFLECTIONSSCALE | ..
	FMOD_REVERB_FLAGS_REFLECTIONSDELAYSCALE | FMOD_REVERB_FLAGS_REVERBSCALE | FMOD_REVERB_FLAGS_REVERBDELAYSCALE | ..
	FMOD_REVERB_FLAGS_DECAYHFLIMIT | FMOD_REVERB_FLAGS_CORE0 | FMOD_REVERB_FLAGS_CORE1

Const FMOD_TAGDATATYPE_BINARY:Int = 0
Const FMOD_TAGDATATYPE_INT:Int = 1
Const FMOD_TAGDATATYPE_FLOAT:Int = 2
Const FMOD_TAGDATATYPE_STRING:Int = 3
Const FMOD_TAGDATATYPE_STRING_UTF16:Int = 4
Const FMOD_TAGDATATYPE_STRING_UTF16BE:Int = 5
Const FMOD_TAGDATATYPE_STRING_UTF8:Int = 6
Const FMOD_TAGDATATYPE_CDTOC:Int = 7

Const FMOD_TAGTYPE_UNKNOWN:Int = 0
Const FMOD_TAGTYPE_ID3V1:Int = 1
Const FMOD_TAGTYPE_ID3V2:Int = 2
Const FMOD_TAGTYPE_VORBISCOMMENT:Int = 3
Const FMOD_TAGTYPE_SHOUTCAST:Int = 4
Const FMOD_TAGTYPE_ICECAST:Int = 5
Const FMOD_TAGTYPE_ASF:Int = 6
Const FMOD_TAGTYPE_MIDI:Int = 7
Const FMOD_TAGTYPE_PLAYLIST:Int = 8
Const FMOD_TAGTYPE_FMOD:Int = 9
Const FMOD_TAGTYPE_USER:Int = 10

Const FMOD_DELAYTYPE_END_MS:Int = 0
Const FMOD_DELAYTYPE_DSPCLOCK_START:Int = 1
Const FMOD_DELAYTYPE_DSPCLOCK_END:Int = 2
Const FMOD_DELAYTYPE_DSPCLOCK_PAUSE:Int = 3

Rem
bbdoc: Speaker Position - Front Left
end rem
Const FMOD_SPEAKER_FRONT_LEFT:Int = 0
Rem
bbdoc: Speaker Position - Front Right
end rem
Const FMOD_SPEAKER_FRONT_RIGHT:Int = 1
Rem
bbdoc: Speaker Position - Front Center
end rem
Const FMOD_SPEAKER_FRONT_CENTER:Int = 2
Rem
bbdoc: Speaker Position - Low Frequency
end rem
Const FMOD_SPEAKER_LOW_FREQUENCY:Int = 3
Rem
bbdoc: Speaker Position - Back Left
end rem
Const FMOD_SPEAKER_BACK_LEFT:Int = 4
Rem
bbdoc: Speaker Position - Back Right
end rem
Const FMOD_SPEAKER_BACK_RIGHT:Int = 5
Rem
bbdoc: Speaker Position - Side Left
end rem
Const FMOD_SPEAKER_SIDE_LEFT:Int = 6
Rem
bbdoc: Speaker Position - Side Right
end rem
Const FMOD_SPEAKER_SIDE_RIGHT:Int = 7

Rem
bbdoc: Maximum number of speaker types supported.
end rem
Const FMOD_SPEAKER_MAX:Int = 8
Rem
bbdoc: /* For use with FMOD_SPEAKERMODE_MONO and Channel::SetSpeakerLevels.
about: Mapped to same value as FMOD_SPEAKER_FRONT_LEFT. */
end rem
Const FMOD_SPEAKER_MONO:Int = FMOD_SPEAKER_FRONT_LEFT
Rem
bbdoc: A non speaker.
about: Use this to send.
end rem
Const FMOD_SPEAKER_NULL:Int = FMOD_SPEAKER_MAX
Rem
bbdoc: For use with FMOD_SPEAKERMODE_7POINT1 on PS3 where the extra speakers are surround back inside of side speakers.
end rem
Const FMOD_SPEAKER_SBL:Int = FMOD_SPEAKER_SIDE_LEFT
Rem
bbdoc: For use with FMOD_SPEAKERMODE_7POINT1 on PS3 where the extra speakers are surround back inside of side speakers.
end rem
Const FMOD_SPEAKER_SBR:Int = FMOD_SPEAKER_SIDE_RIGHT

