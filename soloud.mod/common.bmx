' Copyright (c) 2016 Bruce A Henderson
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict

?macos
Import "-framework AudioToolbox"
?linux
Import sdl.sdl
?

Import "source.bmx"


Extern

	' Soloud
	Function Soloud_destroy(aSoloud:Byte Ptr)
	Function Soloud_create:Byte Ptr()
	Function Soloud_init:Int(aSoloud:Byte Ptr)
	Function Soloud_initEx:Int(aSoloud:Byte Ptr, aFlags:Int, aBackend:Int, aSamplerate:Int, aBufferSize:Int, aChannels:Int)
	Function Soloud_deinit(aSoloud:Byte Ptr)
	Function Soloud_getVersion:Int(aSoloud:Byte Ptr)
	Function Soloud_getErrorString:Byte Ptr(aSoloud:Byte Ptr, aErrorCode:Int)
	Function Soloud_getBackendId:Int(aSoloud:Byte Ptr)
	Function Soloud_getBackendString:Byte Ptr(aSoloud:Byte Ptr)
	Function Soloud_getBackendChannels:Int(aSoloud:Byte Ptr)
	Function Soloud_getBackendSamplerate:Int(aSoloud:Byte Ptr)
	Function Soloud_getBackendBufferSize:Int(aSoloud:Byte Ptr)
	Function Soloud_setSpeakerPosition:Int(aSoloud:Byte Ptr, aChannel:Int, aX:Float, aY:Float, aZ:Float)
	Function Soloud_play:Int(aSoloud:Byte Ptr, aSound:Byte Ptr)
	Function Soloud_playEx:Int(aSoloud:Byte Ptr, aSound:Byte Ptr, aVolume:Float, aPan:Float, aPaused:Int, aBus:Int)
	Function Soloud_playClocked:Int(aSoloud:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr)
	Function Soloud_playClockedEx:Int(aSoloud:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aVolume:Float, aPan:Float, aBus:Int)
	Function Soloud_play3d:Int(aSoloud:Byte Ptr, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Soloud_play3dEx:Int(aSoloud:Byte Ptr, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aVelX:Float, aVelY:Float, aVelZ:Float, aVolume:Float, aPaused:Int, aBus:Int)
	Function Soloud_play3dClocked:Int(aSoloud:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Soloud_play3dClockedEx:Int(aSoloud:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aVelX:Float , aVelY:Float, aVelZ:Float, aVolume:Float, aBus:Int)
	Function Soloud_seek(aSoloud:Byte Ptr, aVoiceHandle:Int, aSeconds:Double)
	Function Soloud_stop(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_stopAll(aSoloud:Byte Ptr)
	Function Soloud_stopAudioSource(aSoloud:Byte Ptr, aSound:Byte Ptr)
	Function Soloud_setFilterParameter(aSoloud:Byte Ptr, aVoiceHandle:Int, aFilterId:Int, aAttributeId:Int, aValue:Float)
	Function Soloud_getFilterParameter:Float(aSoloud:Byte Ptr, aVoiceHandle:Int, aFilterId:Int, aAttributeId:Int)
	Function Soloud_fadeFilterParameter(aSoloud:Byte Ptr, aVoiceHandle:Int, aFilterId:Int, aAttributeId:Int, aTo:Float, aTime:Double)
	Function Soloud_oscillateFilterParameter(aSoloud:Byte Ptr, aVoiceHandle:Int, aFilterId:Int, aAttributeId:Int, aFrom:Float, aTo:Float, aTime:Double)
	Function Soloud_getStreamTime:Double(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getPause:Int(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getVolume:Float(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getOverallVolume:Float(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getPan:Float(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getSamplerate:Float(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getProtectVoice:Int(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getActiveVoiceCount:Int(aSoloud:Byte Ptr)
	Function Soloud_getVoiceCount:Int(aSoloud:Byte Ptr)
	Function Soloud_isValidVoiceHandle:Int(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getRelativePlaySpeed:Float(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getPostClipScaler:Float(aSoloud:Byte Ptr)
	Function Soloud_getGlobalVolume:Float(aSoloud:Byte Ptr)
	Function Soloud_getMaxActiveVoiceCount:Int(aSoloud:Byte Ptr)
	Function Soloud_getLooping:Int(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_setLooping(aSoloud:Byte Ptr, aVoiceHandle:Int, aLooping:Int)
	Function Soloud_setMaxActiveVoiceCount:Int(aSoloud:Byte Ptr, aVoiceCount:Int)
	Function Soloud_setInaudibleBehavior(aSoloud:Byte Ptr, aVoiceHandle:Int, aMustTick:Int, aKill:Int)
	Function Soloud_setGlobalVolume(aSoloud:Byte Ptr, aVolume:Float)
	Function Soloud_setPostClipScaler(aSoloud:Byte Ptr, aScaler:Float)
	Function Soloud_setPause(aSoloud:Byte Ptr, aVoiceHandle:Int, aPause:Int)
	Function Soloud_setPauseAll(aSoloud:Byte Ptr, aPause:Int)
	Function Soloud_setRelativePlaySpeed:Int(aSoloud:Byte Ptr, aVoiceHandle:Int, aSpeed:Float)
	Function Soloud_setProtectVoice(aSoloud:Byte Ptr, aVoiceHandle:Int, aProtect:Int)
	Function Soloud_setSamplerate(aSoloud:Byte Ptr, aVoiceHandle:Int, aSamplerate:Float)
	Function Soloud_setPan(aSoloud:Byte Ptr, aVoiceHandle:Int, aPan:Float)
	Function Soloud_setPanAbsolute(aSoloud:Byte Ptr, aVoiceHandle:Int, aLVolume:Float, aRVolume:Float)
	Function Soloud_setPanAbsoluteEx(aSoloud:Byte Ptr, aVoiceHandle:Int, aLVolume:Float, aRVolume:Float, aLBVolume:Float, aRBVolume:Float, aCVolume:Float, aSVolume:Float)
	Function Soloud_setVolume(aSoloud:Byte Ptr, aVoiceHandle:Int, aVolume:Float)
	Function Soloud_setDelaySamples(aSoloud:Byte Ptr, aVoiceHandle:Int, aSamples:Int)
	Function Soloud_fadeVolume(aSoloud:Byte Ptr, aVoiceHandle:Int, aTo:Float, aTime:Double)
	Function Soloud_fadePan(aSoloud:Byte Ptr, aVoiceHandle:Int, aTo:Float, aTime:Double)
	Function Soloud_fadeRelativePlaySpeed(aSoloud:Byte Ptr, aVoiceHandle:Int, aTo:Float, aTime:Double)
	Function Soloud_fadeGlobalVolume(aSoloud:Byte Ptr, aTo:Float, aTime:Double)
	Function Soloud_schedulePause(aSoloud:Byte Ptr, aVoiceHandle:Int, aTime:Double)
	Function Soloud_scheduleStop(aSoloud:Byte Ptr, aVoiceHandle:Int, aTime:Double)
	Function Soloud_oscillateVolume(aSoloud:Byte Ptr, aVoiceHandle:Int, aFrom:Float, aTo:Float, aTime:Double)
	Function Soloud_oscillatePan(aSoloud:Byte Ptr, aVoiceHandle:Int, aFrom:Float, aTo:Float, aTime:Double)
	Function Soloud_oscillateRelativePlaySpeed(aSoloud:Byte Ptr, aVoiceHandle:Int, aFrom:Float, aTo:Float, aTime:Double)
	Function Soloud_oscillateGlobalVolume(aSoloud:Byte Ptr, aFrom:Float, aTo:Float, aTime:Double)
	Function Soloud_setGlobalFilter(aSoloud:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Soloud_setVisualizationEnable(aSoloud:Byte Ptr, aEnable:Int)
	Function Soloud_calcFFT:Float Ptr(aSoloud:Byte Ptr)
	Function Soloud_getWave:Float Ptr(aSoloud:Byte Ptr)
	Function Soloud_getLoopCount:Int(aSoloud:Byte Ptr, aVoiceHandle:Int)
	Function Soloud_getInfo:Float(aSoloud:Byte Ptr, aVoiceHandle:Int, aInfoKey:Int)
	Function Soloud_createVoiceGroup:Int(aSoloud:Byte Ptr)
	Function Soloud_destroyVoiceGroup:Int(aSoloud:Byte Ptr, aVoiceGroupHandle:Int)
	Function Soloud_addVoiceToGroup:Int(aSoloud:Byte Ptr, aVoiceGroupHandle:Int, aVoiceHandle:Int)
	Function Soloud_isVoiceGroup:Int(aSoloud:Byte Ptr, aVoiceGroupHandle:Int)
	Function Soloud_isVoiceGroupEmpty:Int(aSoloud:Byte Ptr, aVoiceGroupHandle:Int)
	Function Soloud_update3dAudio(aSoloud:Byte Ptr)
	Function Soloud_set3dSoundSpeed:Int(aSoloud:Byte Ptr, aSpeed:Float)
	Function Soloud_get3dSoundSpeed:Float(aSoloud:Byte Ptr)
	Function Soloud_set3dListenerParameters(aSoloud:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aAtX:Float, aAtY:Float, aAtZ:Float, aUpX:Float, aUpY:Float, aUpZ:Float)
	Function Soloud_set3dListenerParametersEx(aSoloud:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aAtX:Float, aAtY:Float, aAtZ:Float, aUpX:Float, aUpY:Float, aUpZ:Float, aVelocityX:Float, aVelocityY:Float, aVelocityZ:Float)
	Function Soloud_set3dListenerPosition(aSoloud:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Soloud_set3dListenerAt(aSoloud:Byte Ptr, aAtX:Float, aAtY:Float, aAtZ:Float)
	Function Soloud_set3dListenerUp(aSoloud:Byte Ptr, aUpX:Float, aUpY:Float, aUpZ:Float)
	Function Soloud_set3dListenerVelocity(aSoloud:Byte Ptr, aVelocityX:Float, aVelocityY:Float, aVelocityZ:Float)
	Function Soloud_set3dSourceParameters(aSoloud:Byte Ptr, aVoiceHandle:Int, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Soloud_set3dSourceParametersEx(aSoloud:Byte Ptr, aVoiceHandle:Int, aPosX:Float, aPosY:Float, aPosZ:Float, aVelocityX:Float, aVelocityY:Float, aVelocityZ:Float)
	Function Soloud_set3dSourcePosition(aSoloud:Byte Ptr, aVoiceHandle:Int, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Soloud_set3dSourceVelocity(aSoloud:Byte Ptr, aVoiceHandle:Int, aVelocityX:Float, aVelocityY:Float, aVelocityZ:Float)
	Function Soloud_set3dSourceMinMaxDistance(aSoloud:Byte Ptr, aVoiceHandle:Int, aMinDistance:Float, aMaxDistance:Float)
	Function Soloud_set3dSourceAttenuation(aSoloud:Byte Ptr, aVoiceHandle:Int, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Soloud_set3dSourceDopplerFactor(aSoloud:Byte Ptr, aVoiceHandle:Int, aDopplerFactor:Float)
	Function Soloud_mix(aSoloud:Byte Ptr, aBuffer:Float Ptr, aSamples:Int)
	Function Soloud_mixSigned16(aSoloud:Byte Ptr, aBuffer:Short Ptr, aSamples:Int)

	'AudioAttenuator
	Function AudioAttenuator_destroy(aAudioAttenuator:Byte Ptr)
	Function AudioAttenuator_attenuate:Float(aAudioAttenuator:Byte Ptr, aDistance:Float, aMinDistance:Float, aMaxDistance:Float, aRolloffFactor:Float)

	' BiquadResonantFilter
	Function BiquadResonantFilter_destroy(aBiquadResonantFilter:Byte Ptr)
	Function BiquadResonantFilter_create:Byte Ptr()
	Function BiquadResonantFilter_setParams:Int(aBiquadResonantFilter:Byte Ptr, aType:Int, aSampleRate:Float, aFrequency:Float, aResonance:Float)

	' LofiFilter
	Function LofiFilter_destroy(aLofiFilter:Byte Ptr)
	Function LofiFilter_create:Byte Ptr()
	Function LofiFilter_setParams:Int(aLofiFilter:Byte Ptr, aSampleRate:Float, aBitdepth:Float)

	' Bus
	Function Bus_destroy(aBus:Byte Ptr)
	Function Bus_create:Byte Ptr()
	Function Bus_setFilter(aBus:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Bus_play:Int(aBus:Byte Ptr, aSound:Byte Ptr)
	Function Bus_playEx:Int(aBus:Byte Ptr, aSound:Byte Ptr, aVolume:Float, aPan:Float, aPaused:Int)
	Function Bus_playClocked:Int(aBus:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr)
	Function Bus_playClockedEx:Int(aBus:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aVolume:Float, aPan:Float)
	Function Bus_play3d:Int(aBus:Byte Ptr, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Bus_play3dEx:Int(aBus:Byte Ptr, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aVelX:Float, aVelY:Float, aVelZ:Float, aVolume:Float, aPaused:Int)
	Function Bus_play3dClocked:Int(aBus:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float)
	Function Bus_play3dClockedEx:Int(aBus:Byte Ptr, aSoundTime:Double, aSound:Byte Ptr, aPosX:Float, aPosY:Float, aPosZ:Float, aVelX:Float, aVelY:Float, aVelZ:Float, aVolume:Float)
	Function Bus_setChannels:Int(aBus:Byte Ptr, aChannels:Int)
	Function Bus_setVisualizationEnable(aBus:Byte Ptr, aEnable:Int)
	Function Bus_calcFFT:Float Ptr(aBus:Byte Ptr)
	Function Bus_getWave:Float Ptr(aBus:Byte Ptr)
	Function Bus_setVolume(aBus:Byte Ptr, aVolume:Float)
	Function Bus_setLooping(aBus:Byte Ptr, aLoop:Int)
	Function Bus_set3dMinMaxDistance(aBus:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Bus_set3dAttenuation(aBus:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Bus_set3dDopplerFactor(aBus:Byte Ptr, aDopplerFactor:Float)
	Function Bus_set3dProcessing(aBus:Byte Ptr, aDo3dProcessing:Int)
	Function Bus_set3dListenerRelative(aBus:Byte Ptr, aListenerRelative:Int)
	Function Bus_set3dDistanceDelay(aBus:Byte Ptr, aDistanceDelay:Int)
	Function Bus_set3dCollider(aBus:Byte Ptr, aCollider:Byte Ptr)
	Function Bus_set3dColliderEx(aBus:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Bus_set3dAttenuator(aBus:Byte Ptr, aAttenuator:Byte Ptr)
	Function Bus_setInaudibleBehavior(aBus:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Bus_stop(aBus:Byte Ptr)

	' EchoFilter
	Function EchoFilter_destroy(aEchoFilter:Byte Ptr)
	Function EchoFilter_create:Byte Ptr()
	Function EchoFilter_setParams:Int(aEchoFilter:Byte Ptr, aDelay:Float)
	Function EchoFilter_setParamsEx:Int(aEchoFilter:Byte Ptr, aDelay:Float, aDecay:Float, aFilter:Float)

	' FFTFilter
	Function FFTFilter_destroy(aFFTFilter:Byte Ptr)
	Function FFTFilter_create:Byte Ptr()

	' BassboostFilter
	Function BassboostFilter_destroy(aBassboostFilter:Byte Ptr)
	Function BassboostFilter_setParams:Int(aBassboostFilter:Byte Ptr, aBoost:Float)
	Function BassboostFilter_create:Byte Ptr()

	' Speech
	Function Speech_destroy(aSpeech:Byte Ptr)
	Function Speech_create:Byte Ptr()
	Function Speech_setText:Int(aSpeech:Byte Ptr, aText:Byte Ptr)
	Function Speech_setVolume(aSpeech:Byte Ptr, aVolume:Float)
	Function Speech_setLooping(aSpeech:Byte Ptr, aLoop:Int)
	Function Speech_set3dMinMaxDistance(aSpeech:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Speech_set3dAttenuation(aSpeech:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Speech_set3dDopplerFactor(aSpeech:Byte Ptr, aDopplerFactor:Float)
	Function Speech_set3dProcessing(aSpeech:Byte Ptr, aDo3dProcessing:Int)
	Function Speech_set3dListenerRelative(aSpeech:Byte Ptr, aListenerRelative:Int)
	Function Speech_set3dDistanceDelay(aSpeech:Byte Ptr, aDistanceDelay:Int)
	Function Speech_set3dCollider(aSpeech:Byte Ptr, aCollider:Byte Ptr)
	Function Speech_set3dColliderEx(aSpeech:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Speech_set3dAttenuator(aSpeech:Byte Ptr, aAttenuator:Byte Ptr)
	Function Speech_setInaudibleBehavior(aSpeech:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Speech_setFilter(aSpeech:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Speech_stop(aSpeech:Byte Ptr)

	' Wav
	Function Wav_destroy(aWav:Byte Ptr)
	Function Wav_create:Byte Ptr()
	Function Wav_load:Int(aWav:Byte Ptr, aFilename:Byte Ptr)
	Function Wav_loadMem:Int(aWav:Byte Ptr, aMem:Byte Ptr, aLength:Int)
	Function Wav_loadMemEx:Int(aWav:Byte Ptr, aMem:Byte Ptr, aLength:Int, aCopy:Int, aTakeOwnership:Int)
	Function Wav_loadFile:Int(aWav:Byte Ptr, aFile:Byte Ptr)
	Function Wav_getLength:Double(aWav:Byte Ptr)
	Function Wav_setVolume(aWav:Byte Ptr, aVolume:Float)
	Function Wav_setLooping(aWav:Byte Ptr, aLoop:Int)
	Function Wav_set3dMinMaxDistance(aWav:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Wav_set3dAttenuation(aWav:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Wav_set3dDopplerFactor(aWav:Byte Ptr, aDopplerFactor:Float)
	Function Wav_set3dProcessing(aWav:Byte Ptr, aDo3dProcessing:Int)
	Function Wav_set3dListenerRelative(aWav:Byte Ptr, aListenerRelative:Int)
	Function Wav_set3dDistanceDelay(aWav:Byte Ptr, aDistanceDelay:Int)
	Function Wav_set3dCollider(aWav:Byte Ptr, aCollider:Byte Ptr)
	Function Wav_set3dColliderEx(aWav:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Wav_set3dAttenuator(aWav:Byte Ptr, aAttenuator:Byte Ptr)
	Function Wav_setInaudibleBehavior(aWav:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Wav_setFilter(aWav:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Wav_stop(aWav:Byte Ptr)

	' WavStream
	Function WavStream_destroy(aWavStream:Byte Ptr)
	Function WavStream_create:Byte Ptr()
	Function WavStream_load:Int(aWavStream:Byte Ptr, aFilename:Byte Ptr)
	Function WavStream_loadMem:Int(aWavStream:Byte Ptr, aData:Byte Ptr, aDataLen:Int)
	Function WavStream_loadMemEx:Int(aWavStream:Byte Ptr, aData:Byte Ptr, aDataLen:Int, aCopy:Int, aTakeOwnership:Int)
	Function WavStream_loadToMem:Int(aWavStream:Byte Ptr, aFilename:Byte Ptr)
	Function WavStream_loadFile:Int(aWavStream:Byte Ptr, aFile:Byte Ptr)
	Function WavStream_loadFileToMem:Int(aWavStream:Byte Ptr, aFile:Byte Ptr)
	Function WavStream_getLength:Double(aWavStream:Byte Ptr)
	Function WavStream_setVolume(aWavStream:Byte Ptr, aVolume:Float)
	Function WavStream_setLooping(aWavStream:Byte Ptr, aLoop:Int)
	Function WavStream_set3dMinMaxDistance(aWavStream:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function WavStream_set3dAttenuation(aWavStream:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function WavStream_set3dDopplerFactor(aWavStream:Byte Ptr, aDopplerFactor:Float)
	Function WavStream_set3dProcessing(aWavStream:Byte Ptr, aDo3dProcessing:Int)
	Function WavStream_set3dListenerRelative(aWavStream:Byte Ptr, aListenerRelative:Int)
	Function WavStream_set3dDistanceDelay(aWavStream:Byte Ptr, aDistanceDelay:Int)
	Function WavStream_set3dCollider(aWavStream:Byte Ptr, aCollider:Byte Ptr)
	Function WavStream_set3dColliderEx(aWavStream:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function WavStream_set3dAttenuator(aWavStream:Byte Ptr, aAttenuator:Byte Ptr)
	Function WavStream_setInaudibleBehavior(aWavStream:Byte Ptr, aMustTick:Int, aKill:Int)
	Function WavStream_setFilter(aWavStream:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function WavStream_stop(aWavStream:Byte Ptr)

	' Prg
	Function Prg_destroy(aPrg:Byte Ptr)
	Function Prg_create:Byte Ptr()
	Function Prg_rand:Int(aPrg:Byte Ptr)
	Function Prg_srand(aPrg:Byte Ptr, aSeed:Int)

	' Sfxr
	Function Sfxr_destroy(aSfxr:Byte Ptr)
	Function Sfxr_create:Byte Ptr()
	Function Sfxr_resetParams(aSfxr:Byte Ptr)
	Function Sfxr_loadParams:Int(aSfxr:Byte Ptr, aFilename:Byte Ptr)
	Function Sfxr_loadParamsMem:Int(aSfxr:Byte Ptr, aMem:Byte Ptr, aLength:Int)
	Function Sfxr_loadParamsMemEx:Int(aSfxr:Byte Ptr, aMem:Byte Ptr, aLength:Int, aCopy:Int, aTakeOwnership:Int)
	Function Sfxr_loadParamsFile:Int(aSfxr:Byte Ptr, aFile:Byte Ptr)
	Function Sfxr_loadPreset:Int(aSfxr:Byte Ptr, aPresetNo:Int, aRandSeed:Int)
	Function Sfxr_setVolume(aSfxr:Byte Ptr, aVolume:Float)
	Function Sfxr_setLooping(aSfxr:Byte Ptr, aLoop:Int)
	Function Sfxr_set3dMinMaxDistance(aSfxr:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Sfxr_set3dAttenuation(aSfxr:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Sfxr_set3dDopplerFactor(aSfxr:Byte Ptr, aDopplerFactor:Float)
	Function Sfxr_set3dProcessing(aSfxr:Byte Ptr, aDo3dProcessing:Int)
	Function Sfxr_set3dListenerRelative(aSfxr:Byte Ptr, aListenerRelative:Int)
	Function Sfxr_set3dDistanceDelay(aSfxr:Byte Ptr, aDistanceDelay:Int)
	Function Sfxr_set3dCollider(aSfxr:Byte Ptr, aCollider:Byte Ptr)
	Function Sfxr_set3dColliderEx(aSfxr:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Sfxr_set3dAttenuator(aSfxr:Byte Ptr, aAttenuator:Byte Ptr)
	Function Sfxr_setInaudibleBehavior(aSfxr:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Sfxr_setFilter(aSfxr:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Sfxr_stop(aSfxr:Byte Ptr)

	' FlangerFilter
	Function FlangerFilter_destroy(aFlangerFilter:Byte Ptr)
	Function FlangerFilter_create:Byte Ptr()
	Function FlangerFilter_setParams:Int(aFlangerFilter:Byte Ptr, aDelay:Float, aFreq:Float)

	' DCRemovalFilter
	Function DCRemovalFilter_destroy(aDCRemovalFilter:Byte Ptr)
	Function DCRemovalFilter_create:Byte Ptr()
	Function DCRemovalFilter_setParams:Int(aDCRemovalFilter:Byte Ptr)
	Function DCRemovalFilter_setParamsEx:Int(aDCRemovalFilter:Byte Ptr, aLength:Float)

	' Modplug
	Function Modplug_destroy(aModplug:Byte Ptr)
	Function Modplug_create:Byte Ptr()
	Function Modplug_load:Int(aModplug:Byte Ptr, aFilename:Byte Ptr)
	Function Modplug_loadMem:Int(aModplug:Byte Ptr, aMem:Byte Ptr, aLength:Int)
	Function Modplug_loadMemEx:Int(aModplug:Byte Ptr, aMem:Byte Ptr, aLength:Int, aCopy:Int, aTakeOwnership:Int)
	Function Modplug_loadFile:Int(aModplug:Byte Ptr, aFile:Byte Ptr)
	Function Modplug_setVolume(aModplug:Byte Ptr, aVolume:Float)
	Function Modplug_setLooping(aModplug:Byte Ptr, aLoop:Int)
	Function Modplug_set3dMinMaxDistance(aModplug:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Modplug_set3dAttenuation(aModplug:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Modplug_set3dDopplerFactor(aModplug:Byte Ptr, aDopplerFactor:Float)
	Function Modplug_set3dProcessing(aModplug:Byte Ptr, aDo3dProcessing:Int)
	Function Modplug_set3dListenerRelative(aModplug:Byte Ptr, aListenerRelative:Int)
	Function Modplug_set3dDistanceDelay(aModplug:Byte Ptr, aDistanceDelay:Int)
	Function Modplug_set3dCollider(aModplug:Byte Ptr, aCollider:Byte Ptr)
	Function Modplug_set3dColliderEx(aModplug:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Modplug_set3dAttenuator(aModplug:Byte Ptr, aAttenuator:Byte Ptr)
	Function Modplug_setInaudibleBehavior(aModplug:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Modplug_setFilter(aModplug:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Modplug_stop(aModplug:Byte Ptr)

	' Monotone
	Function Monotone_destroy(aMonotone:Byte Ptr)
	Function Monotone_create:Byte Ptr()
	Function Monotone_setParams:Int(aMonotone:Byte Ptr, aHardwareChannels:Int)
	Function Monotone_setParamsEx:Int(aMonotone:Byte Ptr, aHardwareChannels:Int, aWaveform:Int)
	Function Monotone_load:Int(aMonotone:Byte Ptr, aFilename:Byte Ptr)
	Function Monotone_loadMem:Int(aMonotone:Byte Ptr, aMem:Byte Ptr, aLength:Int)
	Function Monotone_loadMemEx:Int(aMonotone:Byte Ptr, aMem:Byte Ptr, aLength:Int, aCopy:Int, aTakeOwnership:Int)
	Function Monotone_loadFile:Int(aMonotone:Byte Ptr, aFile:Byte Ptr)
	Function Monotone_setVolume(aMonotone:Byte Ptr, aVolume:Float)
	Function Monotone_setLooping(aMonotone:Byte Ptr, aLoop:Int)
	Function Monotone_set3dMinMaxDistance(aMonotone:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function Monotone_set3dAttenuation(aMonotone:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function Monotone_set3dDopplerFactor(aMonotone:Byte Ptr, aDopplerFactor:Float)
	Function Monotone_set3dProcessing(aMonotone:Byte Ptr, aDo3dProcessing:Int)
	Function Monotone_set3dListenerRelative(aMonotone:Byte Ptr, aListenerRelative:Int)
	Function Monotone_set3dDistanceDelay(aMonotone:Byte Ptr, aDistanceDelay:Int)
	Function Monotone_set3dCollider(aMonotone:Byte Ptr, aCollider:Byte Ptr)
	Function Monotone_set3dColliderEx(aMonotone:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function Monotone_set3dAttenuator(aMonotone:Byte Ptr, aAttenuator:Byte Ptr)
	Function Monotone_setInaudibleBehavior(aMonotone:Byte Ptr, aMustTick:Int, aKill:Int)
	Function Monotone_setFilter(aMonotone:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function Monotone_stop(aMonotone:Byte Ptr)

	' TedSid
	Function TedSid_destroy(aTedSid:Byte Ptr)
	Function TedSid_create:Byte Ptr()
	Function TedSid_load:Int(aTedSid:Byte Ptr, aFilename:Byte Ptr)
	Function TedSid_loadToMem:Int(aTedSid:Byte Ptr, aFilename:Byte Ptr)
	Function TedSid_loadMem:Int(aTedSid:Byte Ptr, aMem:Byte Ptr, aLength:Int)
	Function TedSid_loadMemEx:Int(aTedSid:Byte Ptr, aMem:Byte Ptr, aLength:Int, aCopy:Int, aTakeOwnership:Int)
	Function TedSid_loadFileToMem:Int(aTedSid:Byte Ptr, aFile:Byte Ptr)
	Function TedSid_loadFile:Int(aTedSid:Byte Ptr, aFile:Byte Ptr)
	Function TedSid_setVolume(aTedSid:Byte Ptr, aVolume:Float)
	Function TedSid_setLooping(aTedSid:Byte Ptr, aLoop:Int)
	Function TedSid_set3dMinMaxDistance(aTedSid:Byte Ptr, aMinDistance:Float, aMaxDistance:Float)
	Function TedSid_set3dAttenuation(aTedSid:Byte Ptr, aAttenuationModel:Int, aAttenuationRolloffFactor:Float)
	Function TedSid_set3dDopplerFactor(aTedSid:Byte Ptr, aDopplerFactor:Float)
	Function TedSid_set3dProcessing(aTedSid:Byte Ptr, aDo3dProcessing:Int)
	Function TedSid_set3dListenerRelative(aTedSid:Byte Ptr, aListenerRelative:Int)
	Function TedSid_set3dDistanceDelay(aTedSid:Byte Ptr, aDistanceDelay:Int)
	Function TedSid_set3dCollider(aTedSid:Byte Ptr, aCollider:Byte Ptr)
	Function TedSid_set3dColliderEx(aTedSid:Byte Ptr, aCollider:Byte Ptr, aUserData:Int)
	Function TedSid_set3dAttenuator(aTedSid:Byte Ptr, aAttenuator:Byte Ptr)
	Function TedSid_setInaudibleBehavior(aTedSid:Byte Ptr, aMustTick:Int, aKill:Int)
	Function TedSid_setFilter(aTedSid:Byte Ptr, aFilterId:Int, aFilter:Byte Ptr)
	Function TedSid_stop(aTedSid:Byte Ptr)
	
End Extern


Const SOLOUD_AUTO:Int = 0
Const SOLOUD_SDL:Int = 1
Const SOLOUD_SDL2:Int = 2
Const SOLOUD_PORTAUDIO:Int = 3
Const SOLOUD_WINMM:Int = 4
Const SOLOUD_XAUDIO2:Int = 5
Const SOLOUD_WASAPI:Int = 6
Const SOLOUD_ALSA:Int = 7
Const SOLOUD_OSS:Int = 8
Const SOLOUD_OPENAL:Int = 9
Const SOLOUD_COREAUDIO:Int = 10
Const SOLOUD_OPENSLES:Int = 11
Const SOLOUD_NULLDRIVER:Int = 12
Const SOLOUD_BACKEND_MAX:Int = 13

Const SOLOUD_CLIP_ROUNDOFF:Int = 1
Const SOLOUD_ENABLE_VISUALIZATION:Int = 2
Const SOLOUD_LEFT_HANDED_3D:Int = 4

Const BIQUADRESONANTFILTER_NONE:Int = 0
Const BIQUADRESONANTFILTER_LOWPASS:Int = 1
Const BIQUADRESONANTFILTER_HIGHPASS:Int = 2
Const BIQUADRESONANTFILTER_BANDPASS:Int = 3
Const BIQUADRESONANTFILTER_WET:Int = 0
Const BIQUADRESONANTFILTER_SAMPLERATE:Int = 1
Const BIQUADRESONANTFILTER_FREQUENCY:Int = 2
Const BIQUADRESONANTFILTER_RESONANCE:Int = 3

Const LOFIFILTER_WET:Int = 0
Const LOFIFILTER_SAMPLERATE:Int = 1
Const LOFIFILTER_BITDEPTH:Int = 2

Const BASSBOOSTFILTER_WET:Int = 0
Const BASSBOOSTFILTER_BOOST:Int = 1

Const SFXR_COIN:Int = 0
Const SFXR_LASER:Int = 1
Const SFXR_EXPLOSION:Int = 2
Const SFXR_POWERUP:Int = 3
Const SFXR_HURT:Int = 4
Const SFXR_JUMP:Int = 5
Const SFXR_BLIP:Int = 6

Const FLANGERFILTER_WET:Int = 0
Const FLANGERFILTER_DELAY:Int = 1
Const FLANGERFILTER_FREQ:Int = 2

Const MONOTONE_SQUARE:Int = 0
Const MONOTONE_SAW:Int = 1
Const MONOTONE_SIN:Int = 2
Const MONOTONE_SAWSIN:Int = 3


