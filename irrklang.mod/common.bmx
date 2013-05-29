' Copyright (c) 2008-2013 Bruce A Henderson
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
Import BRL.FileSystem

?linux
Import "-lIrrKlang"
?Not linux
Import "-lirrKlang"
?

Import "include/*.h"
Import "glue.cpp"



Extern

	Function bmx_createIrrKlangDevice:Byte Ptr(driver:Int, options:Int, deviceID:Byte Ptr)

	Function bmx_soundengine_play2d:Byte Ptr(handle:Byte Ptr, s:Byte Ptr, playLooped:Int, startPaused:Int, track:Int, streamMode:Int, enableSoundEffects:Int)
	Function bmx_soundengine_drop:Int(handle:Byte Ptr)
	Function bmx_soundengine_stopallsounds(handle:Byte Ptr)
	Function bmx_soundengine_setallsoundspaused(handle:Byte Ptr, paused:Int)
	Function bmx_soundengine_removeallsoundsources(handle:Byte Ptr)
	Function bmx_soundengine_setsoundvolume(handle:Byte Ptr, volume:Float)
	Function bmx_soundengine_getsoundvolume:Float(handle:Byte Ptr)
	Function bmx_soundengine_update(handle:Byte Ptr)
	Function bmx_soundengine_setdefault3dsoundmindistance(handle:Byte Ptr, minDistance:Float)
	Function bmx_soundengine_getdefault3dsoundmindistance:Float(handle:Byte Ptr)
	Function bmx_soundengine_setdefault3dsoundmaxdistance(handle:Byte Ptr, maxDistance:Float)
	Function bmx_soundengine_getdefault3dsoundmaxdistance:Float(handle:Byte Ptr)
	Function bmx_soundengine_setrollofffactor(handle:Byte Ptr, rolloff:Float)
	Function bmx_soundengine_setdopplereffectparameters(handle:Byte Ptr, dopplerFactor:Float, distanceFactor:Float)
	Function bmx_soundengine_addsoundsourcefromfile:Byte Ptr(handle:Byte Ptr, filename:Byte Ptr, Mode:Int, preload:Int)
	Function bmx_soundengine_addsoundsourcefrommemory:Byte Ptr(handle:Byte Ptr, memory:Byte Ptr, sizeInBytes:Int, name:Byte Ptr, copyMemory:Int)
	Function bmx_soundengine_play2dsource:Byte Ptr(handle:Byte Ptr, source:Byte Ptr, playLooped:Int, startPaused:Int, track:Int, enableSoundEffects:Int)
	Function bmx_soundengine_play3d:Byte Ptr(handle:Byte Ptr, s:Byte Ptr, pos:Byte Ptr, playLooped:Int, startPaused:Int, track:Int, streamMode:Int, enableSoundEffects:Int)
	Function bmx_soundengine_setlistenerposition(handle:Byte Ptr, pos:Byte Ptr, lookDir:Byte Ptr, velPerSecond:Byte Ptr, upVector:Byte Ptr)
	Function bmx_soundengine_play3dsource:Byte Ptr(handle:Byte Ptr, source:Byte Ptr, pos:Byte Ptr, playLooped:Int, startPaused:Int, track:Int, enableSoundEffects:Int)
	Function bmx_soundengine_iscurrentlyplaying:Int(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_soundengine_iscurrentlyplayingsource:Int(handle:Byte Ptr, source:Byte Ptr)
	Function bmx_soundengine_getsoundsourcecount:Int(handle:Byte Ptr)
	Function bmx_soundengine_removesoundsource(handle:Byte Ptr, name:Byte Ptr)
	Function bmx_soundengine_removesoundsourcesource(handle:Byte Ptr, source:Byte Ptr)
	Function bmx_soundengine_getdrivername:Byte Ptr(handle:Byte Ptr)
	Function bmx_soundengine_addsoundsourcealias:Byte Ptr(handle:Byte Ptr, baseSource:Byte Ptr, s:Byte Ptr)
	Function bmx_soundengine_loadplugins:Int(handle:Byte Ptr, path:Byte Ptr)

	Function bmx_sound_setispaused(handle:Byte Ptr, paused:Int)
	Function bmx_sound_getispaused:Int(handle:Byte Ptr)
	Function bmx_sound_stop(handle:Byte Ptr)
	Function bmx_sound_getvolume:Float(handle:Byte Ptr)
	Function bmx_sound_setvolume(handle:Byte Ptr, volume:Float)
	Function bmx_sound_setpan(handle:Byte Ptr, pan:Float)
	Function bmx_sound_getpan:Float(handle:Byte Ptr)
	Function bmx_sound_islooped:Int(handle:Byte Ptr)
	Function bmx_sound_setislooped(handle:Byte Ptr, looped:Int)
	Function bmx_sound_isfinished:Int(handle:Byte Ptr)
	Function bmx_sound_setmindistance(handle:Byte Ptr, minDistance:Float)
	Function bmx_sound_getmindistance:Float(handle:Byte Ptr)
	Function bmx_sound_setmaxdistance(handle:Byte Ptr, maxDistance:Float)
	Function bmx_sound_getmaxdistance:Float(handle:Byte Ptr)
	Function bmx_sound_getplayposition:Int(handle:Byte Ptr)
	Function bmx_sound_setplayposition:Int(handle:Byte Ptr, pos:Int)
	Function bmx_sound_setplaybackspeed:Int(handle:Byte Ptr, speed:Float)
	Function bmx_sound_getplaybackspeed:Float(handle:Byte Ptr)
	Function bmx_sound_getplaylength:Int(handle:Byte Ptr)
	Function bmx_sound_getsoundeffectcontrol:Byte Ptr(handle:Byte Ptr)
	Function bmx_sound_drop(handle:Byte Ptr)
	Function bmx_sound_setposition(handle:Byte Ptr, position:Byte Ptr)
	Function bmx_sound_Setsoundstopeventreceiver(handle:Byte Ptr, receiver:Byte Ptr)
	Function bmx_sound_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_sound_setvelocity(handle:Byte Ptr, vel:Byte Ptr)
	Function bmx_sound_getvelocity:Byte Ptr(handle:Byte Ptr)

	Function bmx_soundsource_getname:Byte Ptr(handle:Byte Ptr)
	Function bmx_soundsource_setstreammode(handle:Byte Ptr, Mode:Int)
	Function bmx_soundsource_getstreammode:Int(handle:Byte Ptr)
	Function bmx_soundsource_getplaylength:Int(handle:Byte Ptr)
	Function bmx_soundsource_getaudioformat:Byte Ptr(handle:Byte Ptr)
	Function bmx_soundsource_isseekingsupported:Int(handle:Byte Ptr)
	Function bmx_soundsource_setdefaultvolume(handle:Byte Ptr, volume:Float)
	Function bmx_soundsource_getdefaultvolume:Float(handle:Byte Ptr)
	Function bmx_soundsource_setdefaultmindistance(handle:Byte Ptr, minDistance:Float)
	Function bmx_soundsource_getdefaultmindistance:Float(handle:Byte Ptr)
	Function bmx_soundsource_setdefaultmaxdistance(handle:Byte Ptr, maxDistance:Float)
	Function bmx_soundsource_getdefaultmaxdistance:Float(handle:Byte Ptr)
	Function bmx_soundsource_forcereloadatnextuse(handle:Byte Ptr)
	Function bmx_soundsource_setforcedstreamingthreshold(handle:Byte Ptr, thresholdBytes:Int)
	Function bmx_soundsource_getforcedstreamingthreshold:Int(handle:Byte Ptr)

	Function bmx_vec3df_create:Byte Ptr(x:Float, y:Float, z:Float)
	Function bmx_vec3df_delete(handle:Byte Ptr)
	Function bmx_vec3df_x:Float(handle:Byte Ptr)
	Function bmx_vec3df_y:Float(handle:Byte Ptr)
	Function bmx_vec3df_z:Float(handle:Byte Ptr)

	Function bmx_audiostreamformat_delete(handle:Byte Ptr)
	Function bmx_audiostreamformat_getchannelcount:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getframecount:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getsamplerate:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getsampleformat:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getsamplesize:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getframesize:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getsampledatasize:Int(handle:Byte Ptr)
	Function bmx_audiostreamformat_getbytespersecond:Int(handle:Byte Ptr)

	Function bmx_soundeffect_disablealleffects(handle:Byte Ptr)
	Function bmx_soundeffect_enablechorussoundeffect:Int(handle:Byte Ptr, fWetDryMix:Float, fDepth:Float, fFeedback:Float, ..
		fFrequency:Float, sinusWaveForm:Int, fDelay:Float, lPhase:Int)
	Function bmx_soundeffect_disablechorussoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_ischorussoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enablecompressorsoundeffect:Int(handle:Byte Ptr, fGain:Float, fAttack:Float, fRelease:Float, ..
		fThreshold:Float, fRatio:Float, fPredelay:Float)
	Function bmx_soundeffect_disablecompressorsoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_iscompressorsoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enabledistortionsoundeffect:Int(handle:Byte Ptr, fGain:Float, fEdge:Float, fPostEQCenterFrequency:Float, ..
		fPostEQBandwidth:Float, fPreLowpassCutoff:Float)
	Function bmx_soundeffect_disabledistortionsoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isdistortionsoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enableechosoundeffect:Int(handle:Byte Ptr, fWetDryMix:Float, fFeedback:Float, fLeftDelay:Float, ..
		fRightDelay:Float, lPanDelay:Int)
	Function bmx_soundeffect_disableechosoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isechosoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enableflangersoundeffect:Int(handle:Byte Ptr, fWetDryMix:Float, fDepth:Float, fFeedback:Float, ..
		fFrequency:Float, triangleWaveForm:Int, fDelay:Float, lPhase:Int)
	Function bmx_soundeffect_disableflangersoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isflangersoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enablegarglesoundeffect:Int(handle:Byte Ptr, rateHz:Int, sinusWaveForm:Int)
	Function bmx_soundeffect_disablegarglesoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isgarglesoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enablei3dl2reverbsoundeffect:Int(handle:Byte Ptr, lRoom:Int, lRoomHF:Int, flRoomRolloffFactor:Float, ..
		flDecayTime:Float, flDecayHFRatio:Float, lReflections:Int, flReflectionsDelay:Float, ..
		lReverb:Int, flReverbDelay:Float, flDiffusion:Float, flDensity:Float, flHFReference:Float)
	Function bmx_soundeffect_disablei3dl2reverbsoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isi3dl2reverbsoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enableparameqsoundeffect:Int(handle:Byte Ptr, fCenter:Float, fBandwidth:Float, fGain:Float)
	Function bmx_soundeffect_disableparameqsoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_isparameqsoundeffectenabled:Int(handle:Byte Ptr)
	Function bmx_soundeffect_enablewavesreverdsoundeffect:Int(handle:Byte Ptr, fInGain:Float, fReverbMix:Float, fReverbTime:Float, fHighFreqRTRatio:Float)
	Function bmx_soundeffect_disablewavesreverbsoundeffect(handle:Byte Ptr)
	Function bmx_soundeffect_iswavesreverbsoundeffectenabled:Int(handle:Byte Ptr)

	Function bmx_soundstopeventreceiver_create:Byte Ptr(handle:Object)
	Function bmx_soundstopeventreceiver_delete(handle:Byte Ptr)

End Extern


Rem
bbdoc: Autodetects the best sound driver for the system.
End Rem
Const ESOD_AUTO_DETECT:Int = 0
Rem
bbdoc: DirectSound8 sound output driver, windows only.
about: In contrast to ESOD_DIRECT_SOUND, this supports sophisticated sound effects
but may not be available on old windows versions. It behaves very similar 
to ESOD_DIRECT_SOUND but also supports DX8 sound effects.
End Rem
Const ESOD_DIRECT_SOUND_8:Int = 1
Rem
bbdoc: DirectSound sound output driver, windows only.
about: This uses DirectSound 3 or above, if available. If DX8 sound effects are needed,
use ESOD_DIRECT_SOUND_8 instead. The ESOD_DIRECT_SOUND driver may be available on more and older windows 
versions than ESOD_DIRECT_SOUND_8.
End Rem
Const ESOD_DIRECT_SOUND:Int = 2
Rem
bbdoc: WinMM sound output driver, windows only.
about: Supports the ISoundMixedOutputReceiver interface using setMixedDataOutputReceiver.
End Rem
Const ESOD_WIN_MM:Int = 3
Rem
bbdoc: ALSA sound output driver, linux only.
about: When using ESOD_ALSA in createIrrKlangDevice(), it is possible to set the third parameter,
'deviceID' to the name of specific ALSA pcm device, to the irrKlang force to use this one.
Set it to 'default', or 'plug:hw' or whatever you need it to be. 
Supports the ISoundMixedOutputReceiver interface using setMixedDataOutputReceiver.
End Rem
Const ESOD_ALSA:Int = 4
Rem
bbdoc: Core Audio sound output driver, mac os only.
about: Supports the ISoundMixedOutputReceiver interface using setMixedDataOutputReceiver.
End Rem
Const ESOD_CORE_AUDIO:Int = 5
Rem
bbdoc: Null driver, creating no sound output.
End Rem
Const ESOD_NULL:Int = 6

Rem
bbdoc: If specified (default), it will make irrKlang run in a separate thread.
about: Using this flag, irrKlang will update all streams, sounds, 3d positions and whatever automaticly.
You also don't need to call ISoundEngine.update() if irrKlang is running multithreaded. However, if you
want to run irrKlang in the same thread as your application (for easier debugging for example),
don't set this. But you need to call ISoundEngine::update()	as often as you can (at least about 2-3
times per second) to make irrKlang update everything correctly then.
End Rem
Const ESEO_MULTI_THREADED:Int = $01
Rem
bbdoc: If the window of the application doesn't have the focus, irrKlang will be silent if this has been set.
about: This will only work when irrKlang is using the DirectSound output driver.
End Rem
Const ESEO_MUTE_IF_NOT_FOCUSED:Int = $02
Rem
bbdoc: Automaticly loads external plugins when starting up.
about: Plugins usually are .dll, .so or .dylib files named for example ikpMP3.dll
(= short for irrKlangPluginMP3) which are executed after the startup of the sound engine and modify
it for example to make it possible to play back mp3 files. Plugins are being loaded from
the current working directory as well as from the position where the .exe using the irrKlang
library resides.  It is also possible to load the plugins after the engine has started up using 
ISoundEngine::loadPlugins().
End Rem
Const ESEO_LOAD_PLUGINS:Int = $04
Rem
bbdoc: Uses 3D sound buffers instead of emulating them when playing 3d sounds (default).
about: If this flag is not specified, all buffers will by created in 2D only and 3D positioning
will be emulated in software, making the engine run faster if hardware 3d audio is slow on the system.
End Rem
Const ESEO_USE_3D_BUFFERS:Int = $08
Rem
bbdoc: Prints debug messages to the debugger window.
about: irrKlang will print debug info and status messages to any windows debugger supporting OutputDebugString() (like VisualStudio).
This is useful if your application does not capture any console output (see ESEO_PRINT_DEBUG_INFO_TO_STDOUT).
End Rem
Const ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER:Int = $10
Rem
bbdoc: Prints debug messages to stdout (the ConsoleWindow).
about: irrKlang will print debug info and status messages stdout, the console window in Windows.
End Rem
Const ESEO_PRINT_DEBUG_INFO_TO_STDOUT:Int = $20
Rem
bbdoc: Uses linear rolloff for 3D sound.
about: If specified, instead of the default logarithmic one, irrKlang will 
use a linear rolloff model which influences the attenuation of the sounds over distance. The volume
is interpolated linearly between the MinDistance and MaxDistance, making it possible to adjust sounds
more easily although this is not physically correct.
Note that this option may not work when used together with the ESEO_USE_3D_BUFFERS
option when using Direct3D for example, irrKlang will then turn off ESEO_USE_3D_BUFFERS
automaticly to be able to use this option and write out a warning.
End Rem
Const ESEO_LINEAR_ROLLOFF:Int = $40
Rem
bbdoc: Default parameters when starting up the engine.
End Rem
Const ESEO_DEFAULT_OPTIONS:Int = ESEO_MULTI_THREADED | ESEO_LOAD_PLUGINS | ESEO_USE_3D_BUFFERS | ..
	ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER | ESEO_PRINT_DEBUG_INFO_TO_STDOUT

Rem
bbdoc: Autodetects the best stream mode for a specified audio data.
End Rem
Const ESM_AUTO_DETECT:Int = 0
Rem
bbdoc: Streams the audio data when needed.
End Rem
Const ESM_STREAMING:Int = 1
Rem
bbdoc: Loads the whole audio data into the memory.
End Rem
Const ESM_NO_STREAMING:Int = 2

Rem
bbdoc: 
End Rem
Const ESF_U8:Int = 0
Rem
bbdoc: 
End Rem
Const ESF_S16:Int = 1
