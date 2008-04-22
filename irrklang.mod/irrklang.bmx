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
bbdoc: irrKlang
End Rem
Module BaH.irrKlang

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: irrKlang - See http://www.ambiera.com/irrklang/license.html"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Copyright: irrKlang - Nikolaus Gebhardt / Ambiera 2001-2007"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?macos
ModuleInfo "LD_OPTS:  -bind_at_load -L%PWD%/lib/macos"
?linux
ModuleInfo "LD_OPTS: -L%PWD%/lib/linux"
?

Import "common.bmx"


Rem
bbdoc: Creates an irrKlang device.
returns: The created irrKlang device or Null if the device could not be created.
about: The irrKlang device is the root object for using the sound engine. 
End Rem
Function CreateIrrKlangDevice:TISoundEngine(driver:Int = ESOD_AUTO_DETECT, options:Int = ESEO_DEFAULT_OPTIONS, ..
		deviceID:String = Null)
	Local eng:Byte Ptr
	If deviceID Then
		Local s:Byte Ptr = deviceID.ToCString()
		eng = bmx_createIrrKlangDevice(driver, options, s)
		MemFree(s)
	Else
		eng = bmx_createIrrKlangDevice(driver, options, Null)
	End If
	Return TISoundEngine._create(eng)
End Function

Rem
bbdoc: Interface to the sound engine, for playing 3d and 2d sound and music. 
about: This is the main interface of irrKlang. You usually would create this using the createIrrKlangDevice() function. 
End Rem
Type TISoundEngine

	Field refPtr:Byte Ptr

	Function _create:TISoundEngine(refPtr:Byte Ptr)
		If refPtr Then
			Local this:TISoundEngine = New TISoundEngine
			this.refPtr = refPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Adds sound source into the sound engine as file.
	about: \param fileName Name of the sound file (e.g. "sounds/something.mp3"). You can also use this
		name when calling play3D() or play2D().
		\param mode Streaming mode for this sound source
		\param preload If this flag is set to false (which is default) the sound engine will
		not try to load the sound file when calling this method, but only when play() is called
		with this sound source as parameter. Otherwise the sound will be preloaded.
		\return Returns the pointer to the added sound source or 0 if not sucessful because for
		example a sound already existed with that name. If not successful, the reason will be printed
		into the log. Note: Don't call drop() to this pointer, it will be managed by irrKlang and
		exist as long as you don't delete irrKlang or call removeSoundSource(). However,
		you are free to call grab() if you want and drop() it then later of course. */
	End Rem
	Method AddSoundSourceFromFile:TISoundSource(fileName:String, mode:Int = ESM_AUTO_DETECT, preload:Int = False)
		Local s:Byte Ptr = fileName.ToCString()
		Local source:Byte Ptr = bmx_soundengine_addsoundsourcefromfile(refPtr, s, mode, preload)
		MemFree(s)
		Return TISoundSource._create(source)
	End Method
	
	Rem
	bbdoc: Adds a sound source into the sound engine as memory source.
	about: Note: This method only accepts a file (.wav, .ogg, etc) which is totally loaded into memory.
		If you want to add a sound source from decoded plain PCM data in memory, use addSoundSourceFromPCMData() instead.
		\param memory Pointer to the memory to be treated as loaded sound file.
		\param sizeInBytes Size of the memory chunk, in bytes.
		\param soundName Name of the virtual sound file (e.g. "sounds/something.mp3"). You can also use this
		name when calling play3D() or play2D(). Hint: If you include the extension of the original file
		like .ogg, .mp3 or .wav at the end of the filename, irrKlang will be able to decide better what
		file format it is and might be able to start playback faster.
		\param copyMemory If set to true which is default, the memory block is copied 
		and stored in the engine, after	calling addSoundSourceFromMemory() the memory pointer can be deleted
		savely. If set to true, the memory is not copied and the user takes the responsibility that 
		the memory block pointed to remains there as long as the sound engine or at least this sound
		source exists.
		\return Returns the pointer to the added sound source or 0 if not sucessful because for
		example a sound already existed with that name. If not successful, the reason will be printed
		into the log. */
	End Rem
	Method AddSoundSourceFromMemory:TISoundSource(memory:Byte Ptr, sizeInBytes:Int, soundName:String, copyMemory:Int = True)
		Local s:Byte Ptr = soundName.ToCString()
		Local source:Byte Ptr = bmx_soundengine_addsoundsourcefrommemory(refPtr, memory, sizeInBytes, s, copyMemory)
		MemFree(s)
		Return TISoundSource._create(source)
	End Method

	Rem
	bbdoc: Drops the object.
	End Rem
	Method Drop:Int()
		Return bmx_soundengine_drop(refPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Play2D:TISound(soundfileName:String, playLooped:Int = False, startPaused:Int = False, ..  
			track:Int = False, streamMode:Int = ESM_AUTO_DETECT, enableSoundEffects:Int = False)

		Local s:Byte Ptr = soundfileName.ToCString()
		Local sound:Byte Ptr = bmx_soundengine_play2d(refPtr, s, playLooped, startPaused, track, streamMode, enableSoundEffects)
		MemFree(s)

		Return TISound._create(sound)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Play2DSource:TISound(source:TISoundSource, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, enableSoundEffects:Int = False)
		Return TISound._create(bmx_soundengine_play2dsource(refPtr, source.soundSourcePtr, playLooped, startPaused, track, enableSoundEffects))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Play3D:TISound(soundFileName:String, pos:TVec3D, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, streamMode:Int = ESM_AUTO_DETECT, enableSoundEffects:Int = False)
		
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Play3DSource:TISound(source:TISoundSource, pos:TVec3D, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, enableSoundEffects:Int = False)
		
	End Method

	Rem
	bbdoc: Stops all currently playing sounds.
	End Rem
	Method StopAllSounds()
		bmx_soundengine_stopallsounds(refPtr)
	End Method

	Rem
	bbdoc: Pauses or unpauses all currently playing sounds.
	End Rem
	Method SetAllSoundsPaused(paused:Int = True)
		bmx_soundengine_setallsoundspaused(refPtr, paused)
	End Method

	Rem
	bbdoc: Removes all sound sources from the engine
	about: This will also cause all sounds to be stopped. 
	Removing sound sources is only necessary if you know you won't use a lot of non-streamed
	sounds again. Sound sources of streamed sounds do not cost a lot of memory.
	End Rem
	Method RemoveAllSoundSources()
		bmx_soundengine_removeallsoundsources(refPtr)
	End Method

	Rem
	bbdoc: Sets master sound volume. This value is multiplied with all sounds played.
	/** \param volume 0 (silent) to 1.0f (full volume)
	End Rem
	Method SetSoundVolume(volume:Float)
		bmx_soundengine_setsoundvolume(refPtr, volume)
	End Method

	Rem
	Returns master sound volume.
	/* A value between 0.0 and 1.0. Default is 1.0. Can be changed using setSoundVolume().
	End Rem
	Method GetSoundVolume:Float()
		Return bmx_soundengine_getsoundvolume(refPtr)
	End Method

	Rem
	Sets the current listener 3d position.
	/** When playing sounds in 3D, updating the position of the listener every frame should be
	done using this function.
	\param pos Position of the camera or listener.
	\param lookdir Direction vector where the camera or listener is looking into. If you have a 
	camera position and a target 3d point where it is looking at, this would be cam->getTarget() - cam->getAbsolutePosition().
	\param velPerSecond The velocity per second describes the speed of the listener and 
	is only needed for doppler effects.
	\param upvector Vector pointing 'up', so the engine can decide where is left and right. 
	This vector is usually (0,1,0).*/
	End Rem
	Method SetListenerPosition(pos:TVec3D, lookDir:TVec3D, velPerSecond:TVec3D = Null, upVector:TVec3D = Null)
	End Method

	Rem
	Updates the audio engine. This should be called several times per frame if irrKlang was started in single thread mode.
	/** This updates the 3d positions of the sounds as well as their volumes, effects,
	streams and other stuff. Call this several times per frame (the more the better) if you
	specified irrKlang to run single threaded. Otherwise it is not necessary to use this method.
	This method is being called by the scene manager automaticly if you are using one, so
	you might want to ignore this.
	End Rem
	Method Update()
		bmx_soundengine_update(refPtr)
	End Method

	Rem
	bbdoc: Returns if a sound with the specified name is currently playing.
	End Rem
	Method IsCurrentlyPlaying:Int(soundName:String)
	End Method

	

	Rem
	bbdoc: Returns if a sound with the specified source is currently playing.
	End Rem
	Method IsCurrentlyPlayingSource:Int(source:TISoundSource)
	End Method
	
	Rem
	 Sets the default minimal distance for 3D sounds.
	/**  This value influences how loud a sound is heard based on its distance.
	See ISound::setMinDistance() for details about what the min distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMinDistance().
	This method only influences the initial distance value of sounds. For changing the
	distance after the sound has been started to play, use ISound::setMinDistance() and ISound::setMaxDistance().
	\param minDistance Default minimal distance for 3d sounds. The default value is 1.0f.*/
	End Rem
	Method SetDefault3DSoundMinDistance(minDistance:Float)
		bmx_soundengine_setdefault3dsoundmindistance(refPtr, minDistance)
	End Method

	Rem
	 Returns the default minimal distance for 3D sounds.
	/**  This value influences how loud a sound is heard based on its distance.
	You can change it using setDefault3DSoundMinDistance().
	See ISound::setMinDistance() for details about what the min distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMinDistance().
	\return Default minimal distance for 3d sounds. The default value is 1.0f.
	End Rem
	Method GetDefault3DSoundMinDistance:Float()
		Return bmx_soundengine_getdefault3dsoundmindistance(refPtr)
	End Method

	Rem
	 Sets the default maximal distance for 3D sounds.
	/** Changing this value is usually not necessary. Use setDefault3DSoundMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefault3DSoundMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMaxDistance().
	This method only influences the initial distance value of sounds. For changing the
	distance after the sound has been started to play, use ISound::setMinDistance() and ISound::setMaxDistance().
	\param maxDistance Default maximal distance for 3d sounds. The default value is 1000000000.0f.
	End Rem
	Method SetDefault3DSoundMaxDistance(maxDistance:Float)
		bmx_soundengine_setdefault3dsoundmaxdistance(refPtr, maxDistance)
	End Method

	Rem
	 Returns the default maximal distance for 3D sounds.
	/**  This value influences how loud a sound is heard based on its distance.
	You can change it using setDefault3DSoundmaxDistance(), but 
	changing this value is usually not necessary. This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefault3DSoundMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMaxDistance().
	\return Default maximal distance for 3d sounds. The default value is 1000000000.0f.
	End Rem
	Method GetDefault3DSoundMaxDistance:Float()
		Return bmx_soundengine_getdefault3dsoundmaxdistance(refPtr)
	End Method

	Rem
	 Sets a rolloff factor which influences the amount of attenuation that is applied to 3D sounds.
	/** The rolloff factor can range from 0.0 to 10.0, where 0 is no rolloff. 1.0 is the default 
	rolloff factor set, the value which we also experience in the real world. A value of 2 would mean
	twice the real-world rolloff.
	End Rem
	Method SetRolloffFactor(rolloff:Float)
		bmx_soundengine_setrollofffactor(refPtr, rolloff)
	End Method

	Rem
	 Sets parameters affecting the doppler effect.
	/** \param dopplerFactor is a value between 0 and 10 which multiplies the doppler 
	effect. Default value is 1.0, which is the real world doppler effect, and 10.0f 
	would be ten times the real world doppler effect.
	\param distanceFactor is the number of meters in a vector unit. The default value
	is 1.0. Doppler effects are calculated in meters per second, with this parameter,
	this can be changed, all velocities and positions are influenced by this. If
	the measurement should be in foot instead of meters, set this value to 0.3048f
	for example.*/
	End Rem
	Method SetDopplerEffectParameters(dopplerFactor:Float = 1.0, distanceFactor:Float = 1.0)
		bmx_soundengine_setdopplereffectparameters(refPtr, dopplerFactor, distanceFactor)
	End Method

End Type

Rem
bbdoc: Represents a sound which is currently played. 
about: The sound can be stopped, its volume or pan changed, effects added/removed and similar using this
interface. Creating sounds is done using TISoundEngine.Play2D() or TISoundEngine.Play3D(). More informations
about the source of a sound can be obtained from the ISoundSource interface. 
End Rem
Type TISound
	
	Field soundPtr:Byte Ptr

	Function _create:TISound(soundPtr:Byte Ptr)
		If soundPtr Then
			Local this:TISound = New TISound
			this.soundPtr = soundPtr
			Return this
		End If
	End Function

	Rem
	returns if the sound is paused
	End Rem
	Method SetIsPaused(paused:Int = True)
		bmx_sound_setispaused(soundPtr, paused)
	End Method

	Rem
	returns if the sound is paused
	end rem
	Method GetIsPaused:Int()
		Return bmx_sound_getispaused(soundPtr)
	End Method

	Rem
	Will stop the sound and free its resources.
	/** If you just want to pause the sound, use setIsPaused().
	After calling stop(), isFinished() will usually return true.
	End Rem
	Method Stop()
		bmx_sound_stop(soundPtr)
	End Method

	Rem
	returns volume of the sound, a value between 0 (mute) and 1 (full volume).
	/** (this volume gets multiplied with the master volume of the sound engine
	and other parameters like distance to listener when played as 3d sound) 
	End Rem
	Method GetVolume:Float()
		Return bmx_sound_getvolume(soundPtr)
	End Method

	Rem
	sets the volume of the sound, a value between 0 (mute) and 1 (full volume).
	/** This volume gets multiplied with the master volume of the sound engine
	and other parameters like distance to listener when played as 3d sound. 
	End Rem
	Method SetVolume(volume:Float)
		bmx_sound_setvolume(soundPtr, volume)
	End Method

	Rem
	sets the pan of the sound. Takes a value between -1 and 1, 0 is center.
	end rem
	Method SetPan(pan:Float)
		bmx_sound_setpan(soundPtr, pan)
	End Method

	Rem
	returns the pan of the sound. Takes a value between -1 and 1, 0 is center.
	end rem
	Method GetPan:Float()
		Return bmx_sound_getpan(soundPtr)
	End Method

	Rem
	returns if the sound has been started to play looped
	end rem
	Method IsLooped:Int()
		Return bmx_sound_islooped(soundPtr)
	End Method

	Rem
	changes the loop mode of the sound. 
	/** If the sound is playing looped and it is changed to not-looped, then it 
	will stop playing after the loop has finished. 
	If it is not looped and changed to looped, the sound will start repeating to be 
	played when it reaches its end. 
	Invoking this method will not have an effect when the sound already has stopped.
	End Rem
	Method SetIsLooped(looped:Int)
		bmx_sound_setislooped(soundPtr, looped)
	End Method

	Rem
	returns if the sound has finished playing.
	/** Don't mix this up with isPaused(). isFinished() returns if the sound has been
	finished playing. If it has, is maybe already have been removed from the playing list of the
	sound engine and calls to any other of the methods of ISound will not have any result.
	If you call stop() to a playing sound will result that this function will return true
	when invoked.
	End Rem
	Method IsFinished:Int()
		Return bmx_sound_isfinished(soundPtr)
	End Method

	Rem
	Sets the minimal distance if this is a 3D sound.
	/** Changes the distance at which the 3D sound stops getting louder. This works
	like this: As a listener approaches a 3D sound source, the sound gets louder.
	Past a certain point, it is not reasonable for the volume to continue to increase.
	Either the maximum (zero) has been reached, or the nature of the sound source
	imposes a logical limit. This is the minimum distance for the sound source.
	Similarly, the maximum distance for a sound source is the distance beyond
	which the sound does not get any quieter.
	The default minimum distance is 1, the default max distance is a huge number like 1000000000.0f.
	End Rem
	Method SetMinDistance(minDistance:Float)
		bmx_sound_setmindistance(soundPtr, minDistance)
	End Method

	Rem
	Returns the minimal distance if this is a 3D sound.
	/** See setMinDistance() for details.
	End Rem
	Method GetMinDistance:Float()
		Return bmx_sound_getmindistance(soundPtr)
	End Method

	Rem
	Sets the maximal distance if this is a 3D sound.
	/** Changing this value is usually not necessary. Use setMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setMinDistance()) to influence this.
	The maximum distance for a sound source is the distance beyond which the sound does not get any quieter.
	The default minimum distance is 1, the default max distance is a huge number like 1000000000.0f.
	End Rem
	Method SetMaxDistance(maxDistance:Float)
		bmx_sound_setmaxdistance(soundPtr, maxDistance)
	End Method

	Rem
	Returns the maximal distance if this is a 3D sound.
	/** See setMaxDistance() for details.
	End Rem
	Method GetMaxDistance:Float()
		Return bmx_sound_getmaxdistance(soundPtr)
	End Method

	

	Rem
	sets the position of the sound in 3d space
	end rem
	Method SetPosition(position:TVec3D)
	End Method

	Rem
	returns the position of the sound in 3d space
	end rem
	Method GetPosition:TVec3D()
	End Method

	Rem
	sets the position of the sound in 3d space, needed for Doppler effects.
	/** To use doppler effects use ISound::setVelocity to set a sounds velocity, 
	ISoundEngine::setListenerPosition() to set the listeners velocity and 
	ISoundEngine::setDopplerEffectParameters() to adjust two parameters influencing 
	the doppler effects intensity.
	End Rem
	Method SetVelocity(vel:TVec3D)
	End Method

	Rem
	returns the velocity of the sound in 3d space, needed for Doppler effects.
	/** To use doppler effects use ISound::setVelocity to set a sounds velocity, 
	ISoundEngine::setListenerPosition() to set the listeners velocity and 
	ISoundEngine::setDopplerEffectParameters() to adjust two parameters influencing 
	the doppler effects intensity.
	End Rem
	Method GetVelocity:TVec3D()
	End Method

	Rem
	returns the current play position of the sound in milliseconds.
	/** \return Returns -1 if not implemented or possible for this sound for example
	because it already has been stopped and freed internally or similar.
	End Rem
	Method GetPlayPosition:Int()
		Return bmx_sound_getplayposition(soundPtr)
	End Method
	
	Rem sets the current play position of the sound in milliseconds.
	/** \param pos Position in milliseconds. Must be between 0 and the value returned
	by getPlayPosition().
	\return Returns true successful. False is returned for example if the sound already finished
	playing and is stopped or the audio source is not seekable, for example if it 
	is an internet stream or a a file format not supporting seeking (a .MOD file for example).
	A file can be tested if it can bee seeking using ISoundSource::getIsSeekingSupported().
	End Rem
	Method SetPlayPosition:Int(pos:Int)
		Return bmx_sound_setplayposition(soundPtr, pos)
	End Method

	Rem
	Sets the playback speed (frequency) of the sound.
	/** Plays the sound at a higher or lower speed, increasing or decreasing its
	frequency which makes it sound lower or higher.
	Note that this feature is not available on all sound output drivers (it is on the
	DirectSound drivers at least), and it does not work together with the 
	'enableSoundEffects' parameter of ISoundEngine::play2D and ISoundEngine::play3D when
	using DirectSound.
	\param speed Factor of the speed increase or decrease. 2 is twice as fast, 
	0.5 is only half as fast. The default is 1.0.
	\return Returns true if sucessful, false if not. The current sound driver might not
	support changing the playBack speed, or the sound was started with the 
	'enableSoundEffects' parameter.
	End Rem
	Method SetPlaybackSpeed:Int(speed:Float = 1.0)
		Return bmx_sound_setplaybackspeed(soundPtr, speed)
	End Method

	Rem
	Returns the playback speed set by setPlaybackSpeed(). Default: 1.0f.
	/** See setPlaybackSpeed() for details
	End Rem
	Method GetPlaybackSpeed:Float()
		Return bmx_sound_getplaybackspeed(soundPtr)
	End Method

	Rem
	returns the play length of the sound in milliseconds.
	/** Returns -1 if not known for this sound for example because its decoder
	does not support length reporting or it is a file stream of unknown size.
	Note: You can also use ISoundSource::getPlayLength() to get the length of 
	a sound without actually needing to play it.
	End Rem
	Method GetPlayLength:Int()
		Return bmx_sound_getplaylength(soundPtr)
	End Method
    
	Rem
	bbdoc: Returns the sound effect control interface for this sound.
	about: Sound effects such as Chorus, Distorsions, Echo, Reverb and similar can
		be controlled using this. The interface pointer is only valid as long as the ISound pointer is valid.
		If the ISound pointer gets dropped (IVirtualRefCounted::drop()), the ISoundEffects
		may not be used any more. 
		\return Returns a pointer to the sound effects interface if available. The sound
		has to be started via ISoundEngine::play2D() or ISoundEngine::play3D(),
		with the flag enableSoundEffects=true, otherwise 0 will be returned. Note that
		if the output driver does not support sound effects, 0 will be returned as well.*/
	End Rem
	Method GetSoundEffectControl:TISoundEffectControl()
		Return TISoundEffectControl._create(bmx_sound_getsoundeffectcontrol(soundPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Drop()
		bmx_sound_drop(soundPtr)
	End Method
	
End Type

Rem
bbdoc: A sound source describes an input file (.ogg, .mp3, .wav or similar) and its default settings. 
about: It provides some information about the sound source like the play length and can have default settings
for volume, distances for 3d etc.
End Rem
Type TISoundSource

	Field soundSourcePtr:Byte Ptr

	Function _create:TISoundSource(soundSourcePtr:Byte Ptr)
		If soundSourcePtr Then
			Local this:TISoundSource = New TISoundSource
			this.soundSourcePtr = soundSourcePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the name of the sound source (usually, this is the file name)
	End Rem
	Method GetName:String()
		Return String.FromCString(bmx_soundsource_getname(soundSourcePtr))
	End Method
	
	Rem
	bbdoc: Sets the stream mode which should be used for a sound played from this source.
	about: Note that if this is set to ESM_NO_STREAMING, the engine still might decide
	to stream the sound if it is too big. The threashold for this can be 
	adjusted using ISoundSource::setForcedStreamingThreshold().
	End Rem
	Method SetStreamMode(mode:Int)
		bmx_soundsource_setstreammode(soundSourcePtr, mode)
	End Method
	
	Rem
	bbdoc: Returns the detected or set type of the sound with wich the sound will be played.
	about: Note: If the returned type is ESM_AUTO_DETECT, this mode will change after the
	sound has been played the first time.
	End Rem
	Method GetStreamMode:Int()
		Return bmx_soundsource_getstreammode(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Returns the play length of the sound in milliseconds.
	about: Returns -1 if not known for this sound for example because its decoder
	does not support length reporting or it is a file stream of unknown size.
	Note: If the sound never has been played before, the sound engine will have to open
	the file and try to get the play length from there, so this call could take a bit depending
	on the type of file.
	End Rem
	Method GetPlayLength:Int()
		Return bmx_soundsource_getplaylength(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Returns informations about the sound source: channel count (mono/stereo), frame count, sample rate, etc.
	about: Returns the structure filled with 0 or negative values if not known for this sound for example because 
	because the file could not be opened or similar.
	Note: If the sound never has been played before, the sound engine will have to open
	the file and try to get the play length from there, so this call could take a bit depending
	on the type of file.
	End Rem
	Method GetAudioFormat:TAudioStreamFormat()
		Return TAudioStreamFormat._create(bmx_soundsource_getaudioformat(soundSourcePtr))
	End Method
	
	Rem
	bbdoc: Returns if sounds played from this source will support seeking via ISound::setPlayPosition().
	returns: True of the sound source supports SetPlayPosition() and false if not. 
	about: If a sound is seekable depends on the file type and the audio format. For example MOD files
	cannot be seeked currently.
	Note: If the sound never has been played before, the sound engine will have to open
	the file and try to get the information from there, so this call could take a bit depending
	on the type of file.
	End Rem
	Method IsSeekingSupported:Int()
		Return bmx_soundsource_isseekingsupported(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Sets the default volume for a sound played from this source.
	about: The default value of this is 1.0f. 
	Note that the default volume is being multiplied with the master volume
	of ISoundEngine, change this via ISoundEngine::setSoundVolume(). 
	//! \param volume 0 (silent) to 1.0f (full volume). Default value is 1.0f.
	End Rem
	Method SetDefaultVolume(volume:Float = 1.0)
		bmx_soundsource_setdefaultvolume(soundSourcePtr, volume)
	End Method
	
	Rem
	bbdoc: Returns the default volume for a sound played from this source.
	/** You can influence this default volume value using setDefaultVolume().
	Note that the default volume is being multiplied with the master volume
	of ISoundEngine, change this via ISoundEngine::setSoundVolume(). 
	//! \return 0 (silent) to 1.0f (full volume). Default value is 1.0f.
	End Rem
	Method GetDefaultVolume:Float()
		Return bmx_soundsource_getdefaultvolume(soundSourcePtr)
	End Method
	
	Rem
	 sets the default minimal distance for 3D sounds played from this source.
	/**  This value influences how loud a sound is heard based on its distance.
	See ISound::setMinDistance() for details about what the min distance is.
	This method only influences the initial distance value of sounds. For changing the
	distance while the sound is playing, use ISound::setMinDistance() and ISound::setMaxDistance().
	\param minDistance: Default minimal distance for 3D sounds from this source. Set it to a negative
	value to let sounds of this source use the engine level default min distance, which
	can be set via ISoundEngine::setDefault3DSoundMinDistance(). Default value is -1, causing
	the default min distance of the sound engine to take effect.
	End Rem
	Method SetDefaultMinDistance(minDistance:Float)
		bmx_soundsource_setdefaultmindistance(soundSourcePtr, minDistance)
	End Method
	
	Rem
	 Returns the default minimal distance for 3D sounds played from this source.
	/**  This value influences how loud a sound is heard based on its distance.
	See ISound::setMinDistance() for details about what the minimal distance is.
	\return Default minimal distance for 3d sounds from this source. If setDefaultMinDistance()
	was set to a negative value, it will return the default value set in the engine,
	using ISoundEngine::setDefault3DSoundMinDistance(). Default value is -1, causing
	the default min distance of the sound engine to take effect.
	End Rem
	Method GetDefaultMinDistance:Float()
		Return bmx_soundsource_getdefaultmindistance(soundSourcePtr)
	End Method
	
	Rem
	 Sets the default maximum distance for 3D sounds played from this source.
	/**  Changing this value is usually not necessary. Use setDefaultMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefaultMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	This method only influences the initial distance value of sounds. For changing the
	distance while the sound is played, use ISound::setMinDistance() 
	and ISound::setMaxDistance().
	\param maxDistance Default maximal distance for 3D sounds from this source. Set it to a negative
	value to let sounds of this source use the engine level default max distance, which
	can be set via ISoundEngine::setDefault3DSoundMaxDistance(). Default value is -1, causing
	the default max distance of the sound engine to take effect.
	End Rem
	Method SetDefaultMaxDistance(maxDistance:Float)
		bmx_soundsource_setdefaultmaxdistance(soundSourcePtr, maxDistance)
	End Method
	
	Rem
	 returns the default maximum distance for 3D sounds played from this source.
	/** This value influences how loud a sound is heard based on its distance.
	Changing this value is usually not necessary. Use setDefaultMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefaultMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	\return Default maximal distance for 3D sounds from this source. If setDefaultMaxDistance()
	was set to a negative value, it will return the default value set in the engine,
	using ISoundEngine::setDefault3DSoundMaxDistance(). Default value is -1, causing
	the default max distance of the sound engine to take effect.
	End Rem
	Method GetDefaultMaxDistance:Float()
		Return bmx_soundsource_getdefaultmaxdistance(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Forces the sound to be reloaded at next replay.
	about: Sounds which are not played as streams are buffered to make it possible to
	replay them without much overhead. If the sound file is altered after the sound
	has been played the first time, the engine won't play the changed file then.
	Calling this method makes the engine reload the file before the file is played
	the next time.*/
	End Rem
	Method ForceReloadAtNextUse()
		bmx_soundsource_forcereloadatnextuse(soundSourcePtr)
	End Method
	
	Rem
	 Sets the threshold size where irrKlang decides to force streaming a file independent of the user specified setting.
	/** When specifying ESM_NO_STREAMING for playing back a sound file, irrKlang will
	ignore this setting if the file is bigger than this threshold and stream the file
	anyway. Please note that if an audio format loader is not able to return the 
	size of a sound source and returns -1 as length, this will be ignored as well 
	and streaming has to be forced.
	\param threshold: New threshold. The value is specified in uncompressed bytes and its default value is 
	about one Megabyte. Set to 0 or a negative value to disable stream forcing.
	End Rem
	Method SetForcedStreamingThreshold(thresholdBytes:Int)
		bmx_soundsource_setforcedstreamingthreshold(soundSourcePtr, thresholdBytes)
	End Method
	
	Rem
	 Returns the threshold size where irrKlang decides to force streaming a file independent of the user specified setting.
	/**  The value is specified in uncompressed bytes and its default value is 
	about one Megabyte. See setForcedStreamingThreshold() for details.
	End Rem
	Method GetForcedStreamingThreshold:Int()
		Return bmx_soundsource_getforcedstreamingthreshold(soundSourcePtr)
	End Method

End Type

Rem
bbdoc: Interface to control the active sound effects (echo, reverb,...) of an ISound object, a playing sound. 
about: Sound effects such as chorus, distorsions, echo, reverb and similar can be controlled using this. An
instance of this interface can be obtained via ISound::getSoundEffectControl(). The sound containing this
interface has to be started via ISoundEngine::play2D() or ISoundEngine::play3D() with the flag
enableSoundEffects=true, otherwise no acccess to this interface will be available. For the DirectSound
driver, these are effects available since DirectSound8. For most effects, sounds should have a sample rate
of 44 khz and should be at least 150 milli seconds long for optimal quality when using the DirectSound
driver. Note that the interface pointer is only valid as long as the ISound pointer is valid. If the ISound
pointer gets dropped (IVirtualRefCounted::drop()), the ISoundEffects may not be used any more. 
End Rem
Type TISoundEffectControl

	Field soundEffectPtr:Byte Ptr

	Function _create:TISoundEffectControl(soundEffectPtr:Byte Ptr)
		If soundEffectPtr Then
			Local this:TISoundEffectControl = New TISoundEffectControl
			this.soundEffectPtr = soundEffectPtr
			Return this
		End If
	End Function

	
	
End Type

Rem
bbdoc: 
End Rem
Type TVec3D
End Type

Rem
bbdoc: 
End Rem
Type TAudioStreamFormat

	Field formatPtr:Byte Ptr

	Function _create:TAudioStreamFormat(formatPtr:Byte Ptr)
		If formatPtr Then
			Local this:TAudioStreamFormat = New TAudioStreamFormat
			this.formatPtr = formatPtr
			Return this
		End If
	End Function

	
End Type

