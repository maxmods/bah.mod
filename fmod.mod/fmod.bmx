' Copyright (c) 2008 Bruce A Henderson
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

Rem
bbdoc: 
End Rem
Module BaH.FMOD

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: FMOD - See http://www.fmod.org"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Copyright: FMOD - 2004-2008 Firelight Technologies, Pty, Ltd"


?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?macos
' -bind_at_load removes linker warnings on 10.4
ModuleInfo "LD_OPTS: -bind_at_load -L%PWD%/lib/macos"
?linux
ModuleInfo "LD_OPTS: -L%PWD%/lib/linux"
?

Import "common.bmx"

Rem
bbdoc: 
End Rem
Global FMOD_VERSION:Int = bmx_fmod_getversion()

Rem
bbdoc: 
End Rem
Type TFMODSystem

	Field systemPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateSystem:TFMODSystem()
		Return New TFMODSystem.Create()
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:TFMODSystem()
		Local res:Int
		systemPtr = bmx_FMOD_System_Create(Varptr res)
		If res Then
			Return Null
		End If
		Return Self
	End Method
	
	Method AddDSP:Int(dsp:TFMODDsp)
		Return FMOD_System_AddDSP(systemPtr, dsp.dspPtr, Null)
	End Method
	
	Rem
	bbdoc: Closes the system object without freeing the object's memory, so the system handle will still be valid.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Closing the output renders objects created with this system object invalid. Make sure any sounds,
	channelgroups, geometry and dsp objects are released before closing the system object.
	End Rem
	Method Close:Int()
		Return FMOD_System_Close(systemPtr)
	End Method
	
	Rem
	bbdoc: Initializes the system object, and the sound device.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: This has to be called at the start of the user's program.
	End Rem
	Method Init:Int(maxChannels:Int, flags:Int = FMOD_INIT_NORMAL, extraDriverData:Byte Ptr = Null)
		Return FMOD_System_Init(systemPtr, maxChannels, flags, extraDriverData)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Update:Int()
		Return FMOD_System_Update(systemPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method CreateSoundURL:TFMODSound(filename:String, mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		Local s:Byte Ptr = filename.ToCString()
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, filename, mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, filename, mode, Null, Varptr ret))
		End If
		MemFree(s)
		
		Return sound
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateSoundPtr:TFMODSound(mem:Byte Ptr, mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, mem, mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, mem, mode, Null, Varptr ret))
		End If
		
		Return sound
	End Method

	Rem
	bbdoc:
	End Rem
	Method CreateStreamURL:TFMODSound(filename:String, mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		Local s:Byte Ptr = filename.ToCString()
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, filename, mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, filename, mode, Null, Varptr ret))
		End If
		MemFree(s)
		
		Return sound
	End Method

	Rem
	bbdoc: 
	End Rem
	Method CreateStreamPtr:TFMODSound(mem:Byte Ptr, mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, mem, mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, mem, mode, Null, Varptr ret))
		End If
		
		Return sound
	End Method

	Rem
	bbdoc: Creates an FMOD defined built in DSP unit object to be inserted into a DSP network, for the purposes of sound filtering or sound generation.
	about: This method is used to create special effects that come built into FMOD. 
	End Rem
	Method CreateDSPByType:TFMODDsp(dspType:Int)
		Local dsp:TFMODDsp
		Local ret:Int
		
		dsp = TFMODDsp._create(bmx_FMOD_System_CreateDSPByType(systemPtr, dspType, Varptr ret))
		
		Return dsp
	End Method
	
	Rem
	bbdoc: Plays a sound object on a particular channel.  
	returns:  The newly playing channel.
	about: 
	End Rem
	Method PlaySound:TFMODChannel(channelId:Int, sound:TFMODSound, paused:Int = False, reuse:TFMODChannel = Null)
		If reuse Then
			Return TFMODChannel._create(bmx_FMOD_System_PlaySound(systemPtr, channelId, sound.soundPtr, paused, reuse.channelPtr))
		Else
			Return TFMODChannel._create(bmx_FMOD_System_PlaySound(systemPtr, channelId, sound.soundPtr, paused, Null))
		End If
	End Method
	
	Method PlayDSP:TFMODChannel(channelId:Int, dsp:TFMODDsp, paused:Int = False, reuse:TFMODChannel = Null)
		If reuse Then
			bmx_FMOD_System_PlayDSP(systemPtr, channelId, dsp.dspPtr, paused, reuse.channelPtr)
			Return reuse
		Else
			Return TFMODChannel._create(bmx_FMOD_System_PlayDSP(systemPtr, channelId, dsp.dspPtr, paused, Null))
		End If
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method Get3DListenerAttributes:Int(listener:Int, pos:TFMODVector = Null, vel:TFMODVector = Null, ..
			forward:TFMODVector = Null, up:TFMODVector = Null)
		Return FMOD_System_Get3DListenerAttributes(systemPtr, listener, Varptr pos, Varptr vel, Varptr forward, Varptr up)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method Get3DNumListeners:Int(listeners:Int Var)
		Return FMOD_System_Get3DNumListeners(systemPtr, Varptr listeners)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method Get3DSettings:Int(dopplerScale:Float Var, distanceFactor:Float Var, rollOffScale:Float Var)
		Return FMOD_System_Get3DSettings(systemPtr, Varptr dopplerScale, Varptr distanceFactor, Varptr rollOffScale)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method Get3DSpeakerPosition:Int(speaker:Int, x:Float Var, y:Float Var, active:Int Var)
		Return FMOD_System_Get3DSpeakerPosition(systemPtr, speaker, Varptr x, Varptr y, Varptr active)
	End Method
	
	?win32
	Rem
	bbdoc:
	End Rem
	Method GetCDROMDriveName:String[](drive:Int)
	End Method
	?
	
	Rem
	bbdoc:
	End Rem
	Method GetCPUUsage:Int(dsp:Float Var, stream:Float Var, update:Float Var, total:Float Var)
		Return FMOD_System_GetCPUUsage(systemPtr, Varptr dsp, Varptr stream, Varptr update, Varptr total)
	End Method

	Rem
	bbdoc:
	End Rem
	Method GetChannel:TFMODChannel(channelId:Int)
		Return TFMODChannel._create(bmx_FMOD_System_GetChannel(systemPtr, channelId))
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method GetChannelsPlaying:Int(channels:Int Var)
		Return FMOD_System_GetChannelsPlaying(systemPtr, Varptr channels)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method GetHardwareChannels:Int(num2D:Int Var, num3D:Int Var, total:Int Var)
		Return FMOD_System_GetHardwareChannels(systemPtr, Varptr num2D, Varptr num3D, Varptr total)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method GetNumCDROMDrives:Int(numDrives:Int Var)
		Return FMOD_System_GetNumCDROMDrives(systemPtr, Varptr numDrives)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method GetSoundRAM:Int(currentAlloced:Int Var, maxAlloced:Int Var, total:Int Var)
		Return FMOD_System_GetSoundRAM(systemPtr, Varptr currentAlloced, Varptr maxAlloced, Varptr total)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method GetVersion:Int(version:Int Var)
		Return FMOD_System_GetVersion(systemPtr, Varptr version)
	End Method

	Rem
	bbdoc: Closes and frees a system object and its resources. 
	End Rem
	Method SystemRelease:Int()
		If systemPtr Then
			Local res:Int = FMOD_System_Release(systemPtr)
			systemPtr = Null
			Return res
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TFMODSound

	Field soundPtr:Byte Ptr
	
	Function _create:TFMODSound(soundPtr:Byte Ptr)
		If soundPtr Then
			Local this:TFMODSound = New TFMODSound
			this.soundPtr = soundPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetDefaults:Int(frequency:Float Var, volume:Float Var, pan:Float Var, priority:Int Var)
		Return FMOD_Sound_GetDefaults(soundPtr, Varptr frequency, Varptr volume, Varptr pan, Varptr priority)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetDefaults:Int(frequency:Float, volume:Float, pan:Float, priority:Int)
		Return FMOD_Sound_SetDefaults(soundPtr, frequency, volume, pan, priority)
	End Method

	Rem
	bbdoc: Retrieves the mode bits set by the codec and the user when opening the sound.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>mode</b> : Variable that receives the current mode for this sound.</li>
	</ul>
	End Rem
	Method GetMode:Int(mode:Int Var)
		Return FMOD_Sound_GetMode(soundPtr, Varptr mode)
	End Method
	
	Rem
	bbdoc: Sets or alters the mode of a sound. 
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>mode</b> : Mode bits to set.</li>
	</ul>
	<p>
	When calling this method, note that it will only take effect when the sound is played again with
	TFMODSystem.PlaySound. Consider this mode the 'default mode' for when the sound plays, not a mode that will
	suddenly change all currently playing instances of this sound. 
	</p>
	<p>
	Flags supported:
	<ul>
	<li>FMOD_LOOP_OFF</li>
	<li>FMOD_LOOP_NORMAL</li>
	<li>FMOD_LOOP_BIDI (only works with sounds created with FMOD_SOFTWARE. Otherwise it will behave as
	FMOD_LOOP_NORMAL) </li>
	<li>FMOD_3D_HEADRELATIVE</li>
	<li>FMOD_3D_WORLDRELATIVE</li>
	<li>FMOD_2D (see notes for win32 hardware voices)</li>
	<li>FMOD_3D (see notes for win32 hardware voices)</li>
	<li>FMOD_3D_LOGROLLOFF</li>
	<li>FMOD_3D_LINEARROLLOFF</li>
	<li>FMOD_3D_CUSTOMROLLOFF</li>
	<li>FMOD_3D_IGNOREGEOMETRY</li>
	<li>FMOD_DONTRESTOREVIRTUAL</li>
	</ul>
	</p>
	<p>
	Issues with streamed audio. (Sounds created with with TFMODSystem.CreateStream or FMOD_CREATESTREAM). When
	changing the loop mode, sounds created with TFMODSystem.CreateStream or FMOD_CREATESTREAM may already have been
	pre-buffered and executed their loop logic ahead of time, before this call was even made.
	</p>
	<p>
	This is dependant on the size of the sound versus the size of the stream decode buffer. See
	FMOD_CREATESOUNDEXINFO.
	</p>
	<p>
	If this happens, you may need to reflush the stream buffer. To do this, you can call TFMODChannel.SetPosition
	which forces a reflush of the stream buffer.
	</p>
	<p>
	Note this will usually only happen if you have sounds or looppoints that are smaller than the stream decode
	buffer size. Otherwise you will not normally encounter any problems.
	</p>
	End Rem
	Method SetMode:Int(mode:Int)
		Return FMOD_Sound_SetMode(soundPtr, mode)
	End Method
	
	Rem
	bbdoc: Retrieves the length of the sound using the specified time unit.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>length</b> : Variable that receives the length of the sound. </li>
	<li><b>lengthType</b> : Time unit retrieve into the length parameter. See FMOD_TIMEUNIT.</li>
	</ul>
	<p>
	Certain timeunits do not work depending on the file format. For example FMOD_TIMEUNIT_MODORDER will not work
	with an mp3 file.
	</p>
	<p>
	A length of $FFFFFFFF usually means it is of unlimited length, such as an internet radio stream or
	MOD/S3M/XM/IT file which may loop forever.
	</p>
	<p>
	Warning! Using a VBR source that does not have an associated length information in milliseconds or pcm samples
	(such as MP3 or MOD/S3M/XM/IT) may return inaccurate lengths specify FMOD_TIMEUNIT_MS or FMOD_TIMEUNIT_PCM.
	</p>
	<p>
	If you want FMOD to retrieve an accurate length it will have to pre-scan the file first in this case. You will
	have to specify FMOD_ACCURATETIME when loading or opening the sound. This means there is a slight delay as
	FMOD scans the whole file when loading the sound to find the right length in millseconds or pcm samples, and
	this also creates a seek table as it does this for seeking purposes.
	</p>
	End Rem
	Method GetLength:Int(length:Int Var, lengthType:Int)
		Return FMOD_Sound_GetLength(soundPtr, Varptr length, lengthType)
	End Method
	
	Rem
	bbdoc: Returns format information about the sound.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>soundType</b> : Variable that receives the type of sound.</li>
	<li><b>soundFormat</b> : Variable that receives the format of the sound.</li>
	<li><b>channels</b> : Variable that receives the number of channels for the sound.</li>
	<li><b>bits</b> : Variable that receives the number of bits per sample for the sound. This corresponds to
	FMOD_SOUND_FORMAT but is provided as an integer format for convenience. Hardware compressed formats such as
	VAG, XADPCM, GCADPCM that stay compressed in memory will return 0.</li>
	</ul>
	End Rem
	Method GetFormat:Int(soundType:Int Var, soundFormat:Int Var, channels:Int Var, bits:Int Var)
		Return FMOD_Sound_GetFormat(soundPtr, Varptr soundType, Varptr soundFormat, Varptr channels, Varptr bits)
	End Method

	Rem
	bbdoc: Returns the sound type of the sound.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>soundType</b> : Variable that receives the type of sound.</li>
	</ul>
	End Rem
	Method GetSoundType:Int(soundType:Int Var)
		Return FMOD_Sound_GetFormat(soundPtr, Varptr soundType, Null, Null, Null)
	End Method

	Rem
	bbdoc: Returns the sound format of the sound.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>soundFormat</b> : Variable that receives the format of the sound.</li>
	</ul>
	End Rem
	Method GetSoundFormat:Int(soundFormat:Int Var)
		Return FMOD_Sound_GetFormat(soundPtr, Null, Varptr soundFormat, Null, Null)
	End Method

	Rem
	bbdoc: Returns the number of channels for the sound.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>channels</b> : Variable that receives the number of channels for the sound.</li>
	</ul>
	End Rem
	Method GetSoundChannels:Int(channels:Int Var)
		Return FMOD_Sound_GetFormat(soundPtr, Null, Null, Varptr channels, Null)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetNumSubSounds:Int(numSubSounds:Int Var)
		Return FMOD_Sound_GetNumSubSounds(soundPtr, Varptr numSubSounds)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNumTags:Int(numTags:Int Var, numTagsUpdated:Int Var)
		Return FMOD_Sound_GetNumTags(soundPtr, Varptr numTags, Varptr numTagsUpdated)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetOpenState:Int(openState:Int Var, percentBuffered:Int Var, starving:Int Var)
		Return FMOD_Sound_GetOpenState(soundPtr, Varptr openState, Varptr percentBuffered, Varptr starving)
	End Method
	
	Rem
	bbdoc: Retrieves the name of a sound.
	returns: The name of the sound.
	about: If FMOD_LOWMEM has been specified in TFMODSystem.CreateSound, this method will return "(null)". 
	End Rem
	Method GetName:String()
		Local vstr:Byte[] = New Byte[256]
		Local ret:Int = FMOD_Sound_GetName(soundPtr, Varptr vstr, 256)
		Return String.FromCString(vstr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLoopCount:Int(loopCount:Int Var)
		Return FMOD_Sound_GetLoopCount(soundPtr, Varptr loopCount)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLoopCount:Int(loopCount:Int)
		Return FMOD_Sound_SetLoopCount(soundPtr, loopCount)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetLoopPoints:Int(loopStart:Int Var, loopStartType:Int Var, loopEnd:Int Var, loopEndType:Int Var)
		Return FMOD_Sound_GetLoopPoints(soundPtr, Varptr loopStart, Varptr loopStartType, Varptr loopEnd, Varptr loopEndType)
	End Method
		
	Rem
	bbdoc: 
	End Rem
	Method SetLoopPoints:Int(loopStart:Int, loopStartType:Int, loopEnd:Int, loopEndType:Int)
		Return FMOD_Sound_SetLoopPoints(soundPtr, loopStart, loopStartType, loopEnd, loopEndType)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Get3DConeSettings:Int(insideConeAngle:Float Var, outsideConeAngle:Float Var, outsideVolume:Float Var)
		Return FMOD_Sound_Get3DConeSettings(soundPtr, Varptr insideConeAngle, Varptr outsideConeAngle, Varptr outsideVolume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Set3DConeSettings:Int(insideConeAngle:Float, outsideConeAngle:Float, outsideVolume:Float)
		Return FMOD_Sound_Set3DConeSettings(soundPtr, insideConeAngle, outsideConeAngle, outsideVolume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Get3DMinMaxDistance:Int(minDist:Float Var, maxDist:Float Var)
		Return FMOD_Sound_Get3DMinMaxDistance(soundPtr, Varptr minDist, Varptr maxDist)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Set3DMinMaxDistance:Int(minDist:Float, maxDist:Float)
		Return FMOD_Sound_Set3DMinMaxDistance(soundPtr, minDist, maxDist)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMusicNumChannels:Int(numChannels:Int Var)
		Return FMOD_Sound_GetMusicNumChannels:Int(soundPtr, Varptr numChannels)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMusicChannelVolume:Int(channel:Int, volume:Int Var)
		Return FMOD_Sound_GetMusicChannelVolume(soundPtr, channel, Varptr volume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetMusicChannelVolume:Int(channel:Int, volume:Int)
		Return FMOD_Sound_SetMusicChannelVolume(soundPtr, channel, volume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetTag:TFMODTag(name:String, index:Int)
		Local s:Byte Ptr = name.ToCString()
		Local tag:TFMODTag = TFMODTag._create(bmx_FMOD_Sound_GetTag(soundPtr, s, index))
		MemFree(s)
		Return tag
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetVariations:Int(frequencyVar:Float Var, volumeVar:Float Var, panVar:Float Var)
		Return FMOD_Sound_GetVariations(soundPtr, Varptr frequencyVar, Varptr volumeVar, Varptr panVar)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetVariations:Int(frequencyVar:Float, volumeVar:Float, panVar:Float)
		Return FMOD_Sound_SetVariations(soundPtr, frequencyVar, volumeVar, panVar)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetSubSound:Int(index:Int, subsound:TFMODSound)
		Return FMOD_Sound_SetSubSound(soundPtr, index, subsound.soundPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetSubSoundSentence:Int(soundList:Int[])
		Return bmx_FMOD_Sound_SetSubSoundSentence(soundPtr, soundList)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SoundRelease:Int()
		If soundPtr Then
			Local res:Int = FMOD_Sound_Release(soundPtr)
			soundPtr = Null
			Return res
		End If
	End Method
	
End Type

Rem
bbdoc: An audio channel.
End Rem
Type TFMODChannel

	Field channelPtr:Byte Ptr
	
	Function _create:TFMODChannel(channelPtr:Byte Ptr)
		If channelPtr Then
			Local this:TFMODChannel = New TFMODChannel
			this.channelPtr = channelPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Retrieves the position and velocity of a 3d channel.  
	End Rem
	Method Get3DAttributes:Int(pos:TFMODVector, vel:TFMODVector)
		Return bmx_FMOD_Channel_Get3DAttributes(channelPtr, Varptr pos, Varptr vel)
	End Method

	Rem
	bbdoc: 
	End Rem	
	Method Get3DConeOrientation:Int(orientation:TFMODVector)
		Return bmx_FMOD_Channel_Get3DConeOrientation(channelPtr, Varptr orientation)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Get3DConeSettings:Int(insideConeAngle:Float Var, outsideConeAngle:Float Var, outsideVolume:Float Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Get3DCustomRollOff:Int(points:TFMODVector[])
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Get3DDopplerLevel:Int(level:Float Var)
		Return bmx_FMOD_Channel_Get3DDopplerLevel(channelPtr, Varptr level)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Get3DMinMaxDistance:Int(minDistance:Float Var, maxDistance:Float Var)
		Return bmx_FMOD_Channel_Get3DMinMaxDistance(channelPtr, Varptr minDistance, Varptr maxDistance)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Get3DOcclusion:Int(directOcclusion:Float Var, reverbOcclusion:Float Var)
		Return bmx_FMOD_Channel_Get3DOcclusion(channelPtr, Varptr directOcclusion, Varptr reverbOcclusion)
	End Method
	
	Rem
	bbdoc: Retrieves the current 3D mix level for the channel set by Channel::set3DPanLevel. 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DPanLevel:Int(level:Float Var)
		Return bmx_FMOD_Channel_Get3DPanLevel(channelPtr, Varptr level)
	End Method
	
	Rem
	bbdoc: Retrieves the stereo (and above) spread angle specified by Channel::set3DSpread.  
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DSpread:Int(angle:Float Var)
		Return bmx_FMOD_Channel_Get3DSpread(channelPtr, Varptr angle)
	End Method
	
	
	Rem
	bbdoc: Returns the playing state for the current channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: 
	End Rem
	Method IsPlaying:Int(playing:Int Var)
		Return bmx_FMOD_Channel_IsPlaying(channelPtr, Varptr playing)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPaused:Int(paused:Int Var)
		Return bmx_FMOD_Channel_GetPaused(channelPtr, Varptr paused)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Stop:Int()
		Return bmx_FMOD_Channel_Stop:Int(channelPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetVolume:Int(volume:Float Var)
		Return bmx_FMOD_Channel_GetVolume(channelPtr, Varptr volume)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetVolume:Int(volume:Float)
		Return bmx_FMOD_Channel_SetVolume(channelPtr, volume)
	End Method
	
	Rem
	bbdoc: Returns the frequency in HZ of the channel.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: 
	End Rem
	Method GetFrequency:Int(frequency:Float Var)
		Return bmx_FMOD_Channel_GetFrequency(channelPtr, Varptr frequency)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFrequency:Int(frequency:Float)
		Return bmx_FMOD_Channel_SetFrequency(channelPtr, frequency)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetIndex:Int(index:Int Var)
		Return bmx_FMOD_Channel_GetIndex(channelPtr, Varptr index)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLoopCount:Int(loopCount:Int Var)
		Return bmx_FMOD_Channel_GetLoopCount(channelPtr, Varptr loopCount)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLoopPoints:Int(loopStart:Int Var, loopStartType:Int, loopEnd:Int Var, loopEndType:Int)
		Return bmx_FMOD_Channel_GetLoopPoints(channelPtr, Varptr loopStart, loopStartType, Varptr loopEnd, loopEndType)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPosition:Int(position:Int Var, posType:Int)
		Return bmx_FMOD_Channel_GetPosition(channelPtr, Varptr position, posType)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method IsVirtual:Int(virtual:Int Var)
		Return bmx_FMOD_Channel_IsVirtual(channelPtr, Varptr virtual)
	End Method
	
	Rem
	bbdoc: Returns the combined volume of the channel after 3d sound, volume, channel group volume and geometry occlusion calculations have been performed on it.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: This does not represent the waveform, just the calculated volume based on 3d distance, occlusion,
	volume and channel group volume. This value is used by the FMOD Ex virtual channel system to order its
	channels between real and virtual. 
	End Rem
	Method GetAudibility:Int(audibility:Float Var)
		Return bmx_FMOD_Channel_GetAudibility(channelPtr, Varptr audibility)
	End Method
	
	Rem
	bbdoc: Returns the currently playing sound for this channel, or Null if no sound is playing.
	End Rem
	Method GetCurrentSound:TFMODSound()
		Return TFMODSound._create(bmx_FMOD_Channel_GetCurrentSound(channelPtr))
	End Method
	
	Rem
	bbdoc: Retrieves the spectrum from the currently playing output signal for the current channel only.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: 
	End Rem
	Method GetSpectrum:Int(spectrumArray:Float[], channelOffset:Int, windowType:Int)
		Return bmx_FMOD_Channel_GetSpectrum(channelPtr, spectrumArray, channelOffset, windowType)
	End Method
	
	Rem
	bbdoc: Gets the currently set delay values.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: If FMOD_DELAYTYPE_DSPCLOCK_START is used, this will be the value of the DSP clock time at the time
	System::playSound was called, if the user has not called Channel::setDelay.. 
	<p>
	What is the 'dsp clock'? The DSP clock represents the output stream to the soundcard, and is incremented
	by the output rate every second (though of course with much finer granularity than this). So if your output
	rate is 48khz, the DSP clock will increment by 48000 per second.
	</p>
	<p>
	The hi and lo values represent this 64bit number, with the delaylo representing the least significant
	32bits and the delayhi value representing the most significant 32bits.
	</p>
	End Rem
	Method GetDelay:Int(delayType:Int, delayHi:Int Var, delayLo:Int Var)
		Return bmx_FMOD_Channel_GetDelay(channelPtr, delayType, Varptr delayHi, Varptr delayLo)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPaused:Int(paused:Int)
		Return bmx_FMOD_Channel_SetPaused(channelPtr, paused)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPan:Int(pan:Float Var)
		Return bmx_FMOD_Channel_GetPan(channelPtr, Varptr pan)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPan:Int(pan:Float)
		Return bmx_FMOD_Channel_SetPan(channelPtr, pan)
	End Method
	
	Method Delete()
		If channelPtr Then
			bmx_fmodchannel_delete(channelPtr)
			channelPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFMODCreateSoundExInfo

	Field soundExInfoPtr:Byte Ptr

	Method New()
		soundExInfoPtr = bmx_soundexinfo_create()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLength(length:Int)
		bmx_soundexinfo_setlength(soundExInfoPtr, length)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetNumChannels(numChannels:Int)
		bmx_soundexinfo_setnumchannels(soundExInfoPtr, numChannels)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetDefaultFrequency(frequency:Int)
		bmx_soundexinfo_setdefaultfrequency(soundExInfoPtr, frequency)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFormat(format:Int)
		bmx_soundexinfo_setformat(soundExInfoPtr, format)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetDecodeBufferSize(bufferSize:Int)
		bmx_soundexinfo_setdecodebuffersize(soundExInfoPtr, bufferSize)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetInitialSubSound(initial:Int)
		bmx_soundexinfo_setinitialsubsound(soundExInfoPtr, initial)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetNumSubSounds(num:Int)
		bmx_soundexinfo_setnumsubsounds(soundExInfoPtr, num)
	End Method
	

	Method Delete()
		If soundExInfoPtr Then
			bmx_soundexinfo_delete(soundExInfoPtr)
			soundExInfoPtr = Null
		End If
	End Method	
	
End Type

Rem
bbdoc: 
End Rem
Type TFMODTag

	Field tagPtr:Byte Ptr

	Function _create:TFMODTag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TFMODTag = New TFMODTag
			this.tagPtr = tagPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetTagType:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetDataType:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetName:String()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetDataAsString:String()
		Local s:Byte Ptr = bmx_fmodtag_getdata(tagPtr)
		Return String.FromCString(s)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetData:Byte Ptr()
		Return bmx_fmodtag_getdata(tagPtr)
	End Method

	Method Delete()
		If tagPtr Then
			bmx_fmodtag_delete(tagPtr)
			tagPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFMODDsp

	Field dspPtr:Byte Ptr
	
	Function _create:TFMODDsp(dspPtr:Byte Ptr)
		If dspPtr Then
			Local this:TFMODDsp = New TFMODDsp
			this.dspPtr = dspPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetActive:Int(active:Int Var)
		Return FMOD_DSP_GetActive(dspPtr, Varptr active)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetActive:Int(active:Int)
		Return FMOD_DSP_SetActive(dspPtr, active)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Remove:Int()
		Return FMOD_DSP_Remove(dspPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Reset:Int()
		Return FMOD_DSP_Reset(dspPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetBypass:Int(bypass:Int Var)
		Return FMOD_DSP_GetBypass(dspPtr, Varptr bypass)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBypass:Int(bypass:Int)
		Return FMOD_DSP_SetBypass(dspPtr, bypass)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNumParameters:Int(numParams:Int Var)
		Return FMOD_DSP_GetNumParameters(dspPtr, Varptr numParams)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetParameter:Int(index:Int, value:Float)
		Return FMOD_DSP_SetParameter(dspPtr, index, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetParameter:String(index:Int, value:Float Var)
		Local vstr:Byte[] = New Byte[16]
		Local ret:Int = FMOD_DSP_GetParameter(dspPtr, index, Varptr value, Varptr vstr, 16)
		Return String.FromCString(vstr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetInfo:String(version:Int Var, channels:Int Var)
		Local vstr:Byte[] = New Byte[32]
		Local ret:Int = FMOD_DSP_GetInfo(dspPtr, Varptr vstr, Varptr version, Varptr channels, Null, Null)
		Return String.FromCString(vstr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetType:Int(dspType:Int Var)
		Return FMOD_DSP_GetType(dspPtr, Varptr dspType)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetDefaults:Int(frequency:Float Var, volume:Float Var, pan:Float Var, priority:Int Var)
		Return FMOD_DSP_GetDefaults(dspPtr, Varptr frequency, Varptr volume, Varptr pan, Varptr priority)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DSPRelease:Int()
		Return FMOD_DSP_Release(dspPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFMODSoundGroup

	Field soundGroupPtr:Byte Ptr
	
	Function _create:TFMODSoundGroup(soundGroupPtr:Byte Ptr)
		If soundGroupPtr Then
			Local this:TFMODSoundGroup = New TFMODSoundGroup
			this.soundGroupPtr = soundGroupPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetMaxAudible:Int(maxAudible:Int Var)
		Return FMOD_SoundGroup_GetMaxAudible(soundGroupPtr, Varptr maxAudible)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMaxAudibleBehavior:Int(behavior:Int Var)
		Return FMOD_SoundGroup_GetMaxAudibleBehavior(soundGroupPtr, Varptr behavior)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMuteFadeSpeed:Int(speed:Float Var)
		Return FMOD_SoundGroup_GetMuteFadeSpeed(soundGroupPtr, Varptr speed)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetName:String()
		Local vstr:Byte[] = New Byte[256]
		Local ret:Int = FMOD_SoundGroup_GetName(soundGroupPtr, Varptr vstr, 256)
		Return String.FromCString(vstr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNumPlaying:Int(numPlaying:Int Var)
		Return FMOD_SoundGroup_GetNumPlaying(soundGroupPtr, Varptr numPlaying)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNumSounds:Int(numSounds:Int Var)
		Return FMOD_SoundGroup_GetNumSounds(soundGroupPtr, Varptr numSounds)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetSound:TFMODSound(index:Int)
		Return TFMODSound._create(bmx_FMOD_SoundGroup_GetSound(soundGroupPtr, index))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetVolume:Int(volume:Float Var)
		Return FMOD_SoundGroup_GetVolume(soundGroupPtr, Varptr volume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SoundGroupRelease:Int()
		Return FMOD_SoundGroup_Release(soundGroupPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetMaxAudible:Int(maxAudible:Int)
		Return FMOD_SoundGroup_SetMaxAudible(soundGroupPtr, maxAudible)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetMaxAudibleBehavior:Int(behavior:Int)
		Return FMOD_SoundGroup_SetMaxAudibleBehavior(soundGroupPtr, behavior)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetMuteFadeSpeed:Int(speed:Float)
		Return FMOD_SoundGroup_SetMuteFadeSpeed(soundGroupPtr, speed)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetVolume:Int(volume:Float)
		Return FMOD_SoundGroup_SetVolume(soundGroupPtr, volume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Stop:Int()
		Return FMOD_SoundGroup_Stop(soundGroupPtr)
	End Method

End Type
