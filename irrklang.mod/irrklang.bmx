' Copyright (c) 2008-2009 Bruce A Henderson
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

ModuleInfo "Version: 1.01"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: irrKlang - See http://www.ambiera.com/irrklang/license.html"
ModuleInfo "Copyright: Wrapper - 2008-2009 Bruce A Henderson"
ModuleInfo "Copyright: irrKlang - Nikolaus Gebhardt / Ambiera 2002-2009"

ModuleInfo "History: 1.01"
ModuleInfo "History: Updated to irrKlang 1.2.0"
ModuleInfo "History: Loading the same sound twice now returns a valid object."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (irrKlang 1.04)"

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
	returns: The added sound source or Null if not sucessful because for example a sound already existed with that name. If not successful, the reason will be printed into the log. Note: Don't call Drop() to this pointer, it will be managed by irrKlang and 		exist as long as you don't delete irrKlang or call RemoveSoundSource(). However, you are free to call Grab() if you want and Drop() it then later of course.
	about: Parameters:
	<ul>
	<li><b>fileName </b> : Name of the sound file (e.g. "sounds/something.mp3"). You can also use this
		name when calling Play3D() or Play2D().</li>
	<li><b>mode </b> : Streaming mode for this sound source</li>
	<li><b>preload </b> : If this flag is set to false (which is default) the sound engine will
		not try to load the sound file when calling this method, but only when Play() is called
		with this sound source as parameter. Otherwise the sound will be preloaded.</li>
	</ul>
	End Rem
	Method AddSoundSourceFromFile:TISoundSource(fileName:String, Mode:Int = ESM_AUTO_DETECT, preload:Int = False)
		Local s:Byte Ptr = fileName.ToCString()
		Local source:Byte Ptr = bmx_soundengine_addsoundsourcefromfile(refPtr, s, Mode, preload)
		MemFree(s)
		Return TISoundSource._create(source)
	End Method
	
	Rem
	bbdoc: Adds a sound source into the sound engine as memory source.
	returns: The added sound source or Null if not sucessful because for example a sound already existed with that name. If not successful, the reason will be printed into the log.
	about: Note: This method only accepts a file (.wav, .ogg, etc) which is totally loaded into memory.
	If you want to add a sound source from decoded plain PCM data in memory, use addSoundSourceFromPCMData() instead.
	<p>Parameters:
	<ul>
	<li><b>memory </b> : Pointer to the memory to be treated as loaded sound file.</li>
	<li><b>sizeInBytes </b> : Size of the memory chunk, in bytes.</li>
	<li><b>soundName </b> : Name of the virtual sound file (e.g. "sounds/something.mp3"). You can also use this
	name when calling play3D() or play2D(). Hint: If you include the extension of the original file
	like .ogg, .mp3 or .wav at the end of the filename, irrKlang will be able to decide better what
	file format it is and might be able to start playback faster.</li>
	<li><b>copyMemory </b> : If set to true which is default, the memory block is copied 
	and stored in the engine, after	calling AddSoundSourceFromMemory() the memory pointer can be deleted
	safely. If set to true, the memory is not copied and the user takes the responsibility that 
	the memory block pointed to remains there as long as the sound engine or at least this sound
	source exists.</li>
	</ul>
	</p>
	End Rem
	Method AddSoundSourceFromMemory:TISoundSource(memory:Byte Ptr, sizeInBytes:Int, soundName:String, copyMemory:Int = True)
		Local s:Byte Ptr = soundName.ToCString()
		Local source:Byte Ptr = bmx_soundengine_addsoundsourcefrommemory(refPtr, memory, sizeInBytes, s, copyMemory)
		MemFree(s)
		Return TISoundSource._create(source)
	End Method
	
	Rem
	bbdoc: Adds a sound source as alias for an existing sound source, but with a different name or optional different default settings. 
	returns: The added sound source or Null if not sucessful because for example a sound already existed with that name. If not successful, the reason will be printed into the log. 
	about: This is useful if you want to play multiple sounds but each sound isn't necessarily one single file.
	Also useful if you want to or play the same sound using different names, volumes or min and max 3D distances. 
	<p>Parameters:
	<ul>
	<li><b>baseSource </b> : The sound source where this sound source should be based on. This sound source will use the
	baseSource as base to access the file and similar, but it will have its own name and its own default settings. </li>
	<li><b>soundName </b> : Name of the new sound source to be added. </li>
	</ul>
	</p>
	End Rem
	Method AddSoundSourceAlias:TISoundSource(baseSource:TISoundSource, soundName:String)
		Local s:Byte Ptr = soundName.ToCString()
		Local source:Byte Ptr = bmx_soundengine_addsoundsourcealias(refPtr, baseSource.soundSourcePtr, s)
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
	bbdoc: Returns the name of the sound driver, like 'ALSA' for the alsa device.
	End Rem
	Method GetDriverName:String()
		Return String.fromCString(bmx_soundengine_getdrivername(refPtr))
	End Method
	
	Rem
	bbdoc: Loads a sound source (if not loaded already) from a file and plays it. 
	about: Parameters:
	<ul>
	<li><b>sourceFileName  </b> : Filename of sound, like "sounds/test.wav" or "foobar.ogg".</li>
	<li><b>playLooped  </b> : plays the sound in loop mode. If set to 'false', the sound is played once, then stopped and
	deleted from the internal playing list. Calls to ISound have no effect after such a non looped sound has been stopped
	automaticly.  </li>
	<li><b>startPaused  </b> : starts the sound paused. This implies that track=true. Use this if you want to modify some of
	the playing parameters before the sound actually plays. Usually you would set this parameter to true, then use the ISound
	interface to modify some of the sound parameters and then call ISound::setPaused(false); Note: You need to call
	ISound::drop() when setting this parameter to true and you don't need the ISound object anymore. See 'return' for details.  </li>
	<li><b>track  </b> : Makes it possible to track the sound. Causes the method to return an ISound interface. See 'return'
	for details.  </li>
	<li><b>streamMode  </b> : Specifies if the file should be streamed or loaded completely into memory for playing.
	 ESM_AUTO_DETECT sets this to autodetection. Note: if the sound has been loaded or played before into the engine,
	this parameter has no effect.  </li>
	<li><b>enableSoundEffects  </b> : Makes it possible to use sound effects such as chorus, distorsions, echo, reverb
	and similar for this sound. Sound effects can then be controlled via ISound::getSoundEffectControl(). Only enable
	if necessary.  </li>
	</ul>
	End Rem
	Method Play2D:TISound(soundfileName:String, playLooped:Int = False, startPaused:Int = False, ..  
			track:Int = False, streamMode:Int = ESM_AUTO_DETECT, enableSoundEffects:Int = False)

		Local s:Byte Ptr = soundfileName.ToCString()
		Local sound:Byte Ptr = bmx_soundengine_play2d(refPtr, s, playLooped, startPaused, track, streamMode, enableSoundEffects)
		MemFree(s)

		Return TISound._create(sound)
	End Method
	
	Rem
	bbdoc: Plays a sound source as 2D sound with its default settings stored in TISoundSource. 
	returns: A TISound object only if the parameters 'track', 'startPaused' or 'enableSoundEffects' have been set to true.
	End Rem
	Method Play2DSource:TISound(source:TISoundSource, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, enableSoundEffects:Int = False)
		Return TISound._create(bmx_soundengine_play2dsource(refPtr, source.soundSourcePtr, playLooped, startPaused, track, enableSoundEffects))
	End Method
	
	Rem
	bbdoc: Loads a sound source (if not loaded already) from a file and plays it. 
	about: Parameters:
	<ul>
	<li><b>sourceFileName</b> : Filename of sound, like "sounds/test.wav" or "foobar.ogg".  </li>
	<li><b>playLooped</b> : Plays the sound in loop mode. If set to 'false', the sound is played
	once, then stopped and deleted from the internal playing list. Calls to ISound have no
	effect after such a non looped sound has been stopped automaticly.  </li>
	<li><b>startPaused</b> : Starts the sound paused. This implies that track=true. Use this if
	you want to modify some of the playing parameters before the sound actually plays. Usually
	you would set this parameter to true, then use the ISound interface to modify some of the
	sound parameters and then call ISound::setPaused(false); Note: You need to call ISound::drop()
	when setting this parameter to true and you don't need the ISound object anymore. See
	'return' for details.  </li>
	<li><b>track</b> : Makes it possible to track the sound. Causes the method to return an
	ISound interface. See 'return' for details.  </li>
	<li><b>streamMode</b> : Specifies if the file should be streamed or loaded completely into
	memory for playing. ESM_AUTO_DETECT sets this to autodetection. Note: if the sound has been
	loaded or played before into the engine, this parameter has no effect.  </li>
	<li><b>enableSoundEffects</b> : Makes it possible to use sound effects such as chorus,
	distorsions, echo, reverb and similar for this sound. Sound effects can then be controlled
	via ISound::getSoundEffectControl(). Only enable if necessary.  </li>
	</ul>
	End Rem
	Method Play3D:TISound(soundFileName:String, pos:TIVec3D, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, streamMode:Int = ESM_AUTO_DETECT, enableSoundEffects:Int = False)
		Assert pos, "Please provide a position vector"
			
		Local s:Byte Ptr = soundfileName.ToCString()
		Local sound:Byte Ptr = bmx_soundengine_play3d(refPtr, s, pos.vecPtr, playLooped, startPaused, track, streamMode, enableSoundEffects)
		MemFree(s)

		Return TISound._create(sound)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Play3DSource:TISound(source:TISoundSource, pos:TIVec3D, playLooped:Int = False, startPaused:Int = False, ..
			track:Int = False, enableSoundEffects:Int = False)
		Return TISound._create(bmx_soundengine_play3dsource(refPtr, source.soundSourcePtr, pos.vecPtr, playLooped, startPaused, track, enableSoundEffects))
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
	bbdoc: Returns amount of loaded sound sources.
	End Rem
	Method GetSoundSourceCount:Int()
		Return bmx_soundengine_getsoundsourcecount(refPtr)
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
	bbdoc: Removes a sound source from the engine, freeing the memory it occupies.
	End Rem
	Method RemoveSoundSource(name:String)
		Local s:Byte Ptr = name.ToCString()
		bmx_soundengine_removesoundsource(refPtr, s)
		MemFree(s)
	End Method

	Rem
	bbdoc: Removes a sound source from the engine, freeing the memory it occupies.
	End Rem
	Method RemoveSoundSourceSource(source:TISoundSource)
		bmx_soundengine_removesoundsourcesource(refPtr, source.soundSourcePtr)
	End Method

	Rem
	bbdoc: Sets master sound volume. This value is multiplied with all sounds played.
	about: \param volume 0 (silent) to 1.0f (full volume)
	End Rem
	Method SetSoundVolume(volume:Float)
		bmx_soundengine_setsoundvolume(refPtr, volume)
	End Method

	Rem
	bbdoc: Returns master sound volume.
	about: A value between 0.0 and 1.0. Default is 1.0. Can be changed using setSoundVolume().
	End Rem
	Method GetSoundVolume:Float()
		Return bmx_soundengine_getsoundvolume(refPtr)
	End Method

	Rem
	bbdoc: Sets the current listener 3d position.
	about: When playing sounds in 3D, updating the position of the listener every frame should be
	done using this method.
	<p>Parameters:
	<ul>
	<li><b>pos </b> : Position of the camera or listener.</li>
	<li><b>lookDir </b> : Direction vector where the camera or listener is looking into. If you have a 
	camera position and a target 3d point where it is looking at, this would be cam->getTarget() - cam->getAbsolutePosition().</li>
	<li><b>velPerSecond </b> : The velocity per second describes the speed of the listener and is only needed for doppler effects.</li>
	<li><b>upVector </b> : Vector pointing 'up', so the engine can decide where is left and right. 
	This vector is usually (0,1,0).</li>
	</ul>
	</p>
	End Rem
	Method SetListenerPosition(pos:TIVec3D, lookDir:TIVec3D, velPerSecond:TIVec3D = Null, upVector:TIVec3D = Null)
		If velPerSecond Then
			If upVector Then
				bmx_soundengine_setlistenerposition(refPtr, pos.vecPtr, lookDir.vecPtr, velPerSecond.vecPtr, upVector.vecPtr)
			Else
				bmx_soundengine_setlistenerposition(refPtr, pos.vecPtr, lookDir.vecPtr, velPerSecond.vecPtr, Null)
			End If
		Else
			If upVector Then
				bmx_soundengine_setlistenerposition(refPtr, pos.vecPtr, lookDir.vecPtr, Null, upVector.vecPtr)
			Else
				bmx_soundengine_setlistenerposition(refPtr, pos.vecPtr, lookDir.vecPtr, Null, Null)
			End If
		End If
	End Method

	Rem
	bbdoc: Updates the audio engine.
	about: This should be called several times per frame if irrKlang was started in single thread mode.
	This updates the 3d positions of the sounds as well as their volumes, effects,
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
		Local s:Byte Ptr = soundName.ToCString()
		Local ret:Int = bmx_soundengine_iscurrentlyplaying(refPtr, s)
		MemFree(s)
		Return ret
	End Method

	Rem
	bbdoc: Returns if a sound with the specified source is currently playing.
	End Rem
	Method IsCurrentlyPlayingSource:Int(source:TISoundSource)
		Return bmx_soundengine_iscurrentlyplayingsource(refPtr, source.soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Sets the default minimal distance for 3D sounds.
	about: This value influences how loud a sound is heard based on its distance.
	See ISound::setMinDistance() for details about what the min distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMinDistance().
	This method only influences the initial distance value of sounds. For changing the
	distance after the sound has been started to play, use ISound::setMinDistance() and ISound::setMaxDistance().
	<p>Parameters:
	<ul>
	<li><b>minDistance </b> : Default minimal distance for 3d sounds. The default value is 1.0</li>
	</ul>
	</p>
	End Rem
	Method SetDefault3DSoundMinDistance(minDistance:Float)
		bmx_soundengine_setdefault3dsoundmindistance(refPtr, minDistance)
	End Method

	Rem
	bbdoc: Returns the default minimal distance for 3D sounds.
	returns: The default minimal distance for 3d sounds. The default value is 1.0
	about: This value influences how loud a sound is heard based on its distance.
	You can change it using setDefault3DSoundMinDistance().
	See ISound::setMinDistance() for details about what the min distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMinDistance().
	End Rem
	Method GetDefault3DSoundMinDistance:Float()
		Return bmx_soundengine_getdefault3dsoundmindistance(refPtr)
	End Method

	Rem
	bbdoc: Sets the default maximal distance for 3D sounds.
	about: Changing this value is usually not necessary. Use setDefault3DSoundMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefault3DSoundMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMaxDistance().
	This method only influences the initial distance value of sounds. For changing the
	distance after the sound has been started to play, use ISound::setMinDistance() and ISound::setMaxDistance().
	<p>Parameters:
	<ul>
	<li><b>maxDistance </b> : Default maximal distance for 3d sounds. The default value is 1000000000.0</li>
	</ul>
	</p>
	End Rem
	Method SetDefault3DSoundMaxDistance(maxDistance:Float)
		bmx_soundengine_setdefault3dsoundmaxdistance(refPtr, maxDistance)
	End Method

	Rem
	bbdoc: Returns the default maximal distance for 3D sounds.
	returns: The default maximal distance for 3d sounds. The default value is 1000000000.0
	about: This value influences how loud a sound is heard based on its distance.
	You can change it using setDefault3DSoundmaxDistance(), but 
	changing this value is usually not necessary. This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefault3DSoundMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	It is also possible to influence this default value for every sound file 
	using ISoundSource::setDefaultMaxDistance().
	End Rem
	Method GetDefault3DSoundMaxDistance:Float()
		Return bmx_soundengine_getdefault3dsoundmaxdistance(refPtr)
	End Method

	Rem
	bbdoc: Sets a rolloff factor which influences the amount of attenuation that is applied to 3D sounds.
	about: The rolloff factor can range from 0.0 to 10.0, where 0 is no rolloff. 1.0 is the default 
	rolloff factor set, the value which we also experience in the real world. A value of 2 would mean
	twice the real-world rolloff.
	End Rem
	Method SetRolloffFactor(rolloff:Float)
		bmx_soundengine_setrollofffactor(refPtr, rolloff)
	End Method

	Rem
	bbdoc: Sets parameters affecting the doppler effect.
	about: Parameters:
	<ul>
	<li><b>dopplerFactor </b> : A value between 0 and 10 which multiplies the doppler 
	effect. Default value is 1.0, which is the real world doppler effect, and 10.0 
	would be ten times the real world doppler effect.</li>
	<li><b>distanceFactor </b> : the number of meters in a vector unit. The default value
	is 1.0. Doppler effects are calculated in meters per second, with this parameter,
	this can be changed, all velocities and positions are influenced by this. If
	the measurement should be in foot instead of meters, set this value to 0.3048
	for example.</li>
	</ul>
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
	bbdoc: Sets the paused status of the sound.
	End Rem
	Method SetPaused(paused:Int = True)
		bmx_sound_setispaused(soundPtr, paused)
	End Method

	Rem
	bbdoc: returns if the sound is paused
	End Rem
	Method IsPaused:Int()
		Return bmx_sound_getispaused(soundPtr)
	End Method

	Rem
	bbdoc: Will stop the sound and free its resources.
	about: If you just want to pause the sound, use setIsPaused().
	After calling stop(), isFinished() will usually return true.
	End Rem
	Method Stop()
		bmx_sound_stop(soundPtr)
	End Method

	Rem
	bbdoc: Returns volume of the sound, a value between 0 (mute) and 1 (full volume).
	about: This volume gets multiplied with the master volume of the sound engine
	and other parameters like distance to listener when played as 3d sound.
	End Rem
	Method GetVolume:Float()
		Return bmx_sound_getvolume(soundPtr)
	End Method

	Rem
	bbdoc: Sets the volume of the sound, a value between 0 (mute) and 1 (full volume).
	about: This volume gets multiplied with the master volume of the sound engine
	and other parameters like distance to listener when played as 3d sound. 
	End Rem
	Method SetVolume(volume:Float)
		bmx_sound_setvolume(soundPtr, volume)
	End Method

	Rem
	bbdoc: Sets the pan of the sound.
	about: Parameters: 
	<ul>
	<li><b>pan</b> : A value between -1 and 1, 0 is center.</li>
	</ul>
	End Rem
	Method SetPan(pan:Float)
		bmx_sound_setpan(soundPtr, pan)
	End Method

	Rem
	bbdoc: Returns the pan of the sound.
	about: A value between -1 and 1, 0 is center.
	End Rem
	Method GetPan:Float()
		Return bmx_sound_getpan(soundPtr)
	End Method

	Rem
	bbdoc: Returns True if the sound has been started to play looped.
	End Rem
	Method IsLooped:Int()
		Return bmx_sound_islooped(soundPtr)
	End Method

	Rem
	bbdoc: Changes the loop mode of the sound. 
	about: Parameters: 
	<ul>
	<li><b>looped</b> : The looping status. Set to False not to loop.</li>
	</ul>
	<p>
	If the sound is playing looped and it is changed to not-looped, then it will stop playing after the
	loop has finished.
	</p>
	<p>
	If it is not looped and changed to looped, the sound will start repeating to be played when it reaches its end. 
	</p>
	<p>
	Invoking this method will not have an effect when the sound already has stopped.
	</p>
	End Rem
	Method SetIsLooped(looped:Int)
		bmx_sound_setislooped(soundPtr, looped)
	End Method

	Rem
	bbdoc: Returns if the sound has finished playing.
	about: Don't mix this up with isPaused(). isFinished() returns if the sound has been
	finished playing. If it has, is maybe already have been removed from the playing list of the
	sound engine and calls to any other of the methods of ISound will not have any result.
	If you call stop() to a playing sound will result that this function will return true
	when invoked.
	End Rem
	Method IsFinished:Int()
		Return bmx_sound_isfinished(soundPtr)
	End Method

	Rem
	bbdoc: Sets the minimal distance if this is a 3D sound.
	about: Changes the distance at which the 3D sound stops getting louder. This works
	like this: As a listener approaches a 3D sound source, the sound gets louder.
	Past a certain point, it is not reasonable for the volume to continue to increase.
	Either the maximum (zero) has been reached, or the nature of the sound source
	imposes a logical limit. This is the minimum distance for the sound source.
	Similarly, the maximum distance for a sound source is the distance beyond
	which the sound does not get any quieter.
	The default minimum distance is 1, the default max distance is a huge number like 1000000000.0.
	End Rem
	Method SetMinDistance(minDistance:Float)
		bmx_sound_setmindistance(soundPtr, minDistance)
	End Method

	Rem
	bbdoc: Returns the minimal distance if this is a 3D sound.
	about: See SetMinDistance() for details.
	End Rem
	Method GetMinDistance:Float()
		Return bmx_sound_getmindistance(soundPtr)
	End Method

	Rem
	bbdoc: Sets the maximal distance if this is a 3D sound.
	about: Changing this value is usually not necessary. Use setMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setMinDistance()) to influence this.
	The maximum distance for a sound source is the distance beyond which the sound does not get any quieter.
	The default minimum distance is 1, the default max distance is a huge number like 1000000000.0.
	End Rem
	Method SetMaxDistance(maxDistance:Float)
		bmx_sound_setmaxdistance(soundPtr, maxDistance)
	End Method

	Rem
	bbdoc: Returns the maximal distance if this is a 3D sound.
	about: See setMaxDistance() for details.
	End Rem
	Method GetMaxDistance:Float()
		Return bmx_sound_getmaxdistance(soundPtr)
	End Method

	Rem
	bbdoc: Sets the position of the sound in 3d space
	End Rem
	Method SetPosition(position:TIVec3D)
		bmx_sound_setposition(soundPtr, position.vecPtr)
	End Method

	Rem
	bbdoc: Returns the position of the sound in 3d space
	End Rem
	Method GetPosition:TIVec3D()
		Return TIVec3D._create(bmx_sound_getposition(soundPtr))
	End Method

	Rem
	bbdoc: Sets the position of the sound in 3d space, needed for Doppler effects.
	about: To use doppler effects use ISound::setVelocity to set a sounds velocity, 
	ISoundEngine::setListenerPosition() to set the listeners velocity and 
	ISoundEngine::setDopplerEffectParameters() to adjust two parameters influencing 
	the doppler effects intensity.
	End Rem
	Method SetVelocity(vel:TIVec3D)
		bmx_sound_setvelocity(soundPtr, vel.vecPtr)
	End Method

	Rem
	bbdoc: Returns the velocity of the sound in 3d space, needed for Doppler effects.
	about: To use doppler effects use ISound::setVelocity to set a sounds velocity, 
	ISoundEngine::setListenerPosition() to set the listeners velocity and 
	ISoundEngine::setDopplerEffectParameters() to adjust two parameters influencing 
	the doppler effects intensity.
	End Rem
	Method GetVelocity:TIVec3D()
		Return TIVec3D._create(bmx_sound_getvelocity(soundPtr))
	End Method

	Rem
	bbdoc: Returns the current play position of the sound in milliseconds.
	returns: -1 if not implemented or possible for this sound for example because it already has been stopped and freed internally or similar.
	End Rem
	Method GetPlayPosition:Int()
		Return bmx_sound_getplayposition(soundPtr)
	End Method
	
	Rem
	bbdoc:sets the current play position of the sound in milliseconds.
	returns: True if successful. False is returned for example if the sound already finished playing and is stopped or the audio source is not seekable, for example if it is an internet stream or a a file format not supporting seeking (a .mod file for example).
	about: Parameters: 
	<ul>
	<li><b>pos</b> : Position in milliseconds. Must be between 0 And the value returned
	by getPlayPosition().</li>
	</ul>
	A file can be tested if it can be seeked using ISoundSource::getIsSeekingSupported().
	End Rem
	Method SetPlayPosition:Int(pos:Int)
		Return bmx_sound_setplayposition(soundPtr, pos)
	End Method

	Rem
	bbdoc: Sets the playback speed (frequency) of the sound.
	returns: true if sucessful, false if not. The current sound driver might not support changing the playBack speed, or the sound was started with the 'enableSoundEffects' parameter.
	about: Plays the sound at a higher or lower speed, increasing or decreasing its
	frequency which makes it sound lower or higher.
	Note that this feature is not available on all sound output drivers (it is on the
	DirectSound drivers at least), and it does not work together with the 
	'enableSoundEffects' parameter of ISoundEngine::play2D and ISoundEngine::play3D when
	using DirectSound.
	<p>Parameters: 
	<ul>
	<li><b>speed </b> : Factor of the speed increase or decrease. 2 is twice as fast, 
	0.5 is only half as fast. The default is 1.0.</li>
	</ul>
	</p>
	End Rem
	Method SetPlaybackSpeed:Int(speed:Float = 1.0)
		Return bmx_sound_setplaybackspeed(soundPtr, speed)
	End Method

	Rem
	bbdoc: Returns the playback speed set by setPlaybackSpeed(). Default: 1.0.
	about: See SetPlaybackSpeed() for details
	End Rem
	Method GetPlaybackSpeed:Float()
		Return bmx_sound_getplaybackspeed(soundPtr)
	End Method

	Rem
	bbdoc: Returns the play length of the sound in milliseconds.
	about: Returns -1 if not known for this sound for example because its decoder
	does not support length reporting or it is a file stream of unknown size.
	Note: You can also use ISoundSource::GetPlayLength() to get the length of 
	a sound without actually needing to play it.
	End Rem
	Method GetPlayLength:Int()
		Return bmx_sound_getplaylength(soundPtr)
	End Method
    
	Rem
	bbdoc: Returns the sound effect control interface for this sound.
	returns: The sound effects interface if available. The sound has to be started via ISoundEngine::Play2D() or ISoundEngine::Play3D(), with the flag enableSoundEffects=true, otherwise 0 will be returned. Note that if the output driver does not support sound effects, Null will be returned as well.
	about: Sound effects such as Chorus, Distorsions, Echo, Reverb and similar can
	be controlled using this. The interface pointer is only valid as long as the ISound pointer is valid.
	If the ISound pointer gets dropped (IVirtualRefCounted::Drop()), the ISoundEffects
	may not be used any more. 
	End Rem
	Method GetSoundEffectControl:TISoundEffectControl()
		Return TISoundEffectControl._create(bmx_sound_getsoundeffectcontrol(soundPtr))
	End Method
	
	Rem
	bbdoc: Sets the sound stop event receiver, an interface which gets called if a sound has finished playing. 
	about: This event is guaranteed to be called when the sound or sound stream is finished, either because the sound
	reached its playback end, its sound source was removed, ISoundEngine::StopAllSounds() has been called or the whole
	engine was deleted.
	<p>Parameters:
	<ul>
	<li><b>receiver </b> : Interface to a user implementation of the sound receiver. This interface should be valid for as
	long as the sound exists or another stop event receiver is set. Set this to Null to set no sound stop event receiver. </li>
	<li><b>userData</b> : Reference to optional user data.</li>
	</ul>
	</p>
	End Rem
	Method SetSoundStopEventReceiver(receiver:TISoundStopEventReceiver, userData:Object = Null)
		If receiver Then
			receiver.userData = userData
			bmx_sound_Setsoundstopeventreceiver(soundPtr, receiver.recPtr)
		Else
			bmx_sound_Setsoundstopeventreceiver(soundPtr, Null)
		End If
	End Method
	
	Rem
	bbdoc: Drops the object. Decrements the reference counter by one.
	End Rem
	Method Drop()
		bmx_sound_drop(soundPtr)
	End Method
	
	' we kind of need to ensure that the sound is dropped once we go out of scope - just in case
	' the user forgot to drop us.
	Method Delete()
		If soundPtr Then
			Drop()
			soundPtr = Null
		End If
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
	Method SetStreamMode(Mode:Int)
		bmx_soundsource_setstreammode(soundSourcePtr, Mode)
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
	<p>Parameters:
	<ul>
	<li><b>volume </b> : 0 (silent) to 1.0 (full volume). Default value is 1.0</li>
	</ul>
	</p>
	End Rem
	Method SetDefaultVolume(volume:Float = 1.0)
		bmx_soundsource_setdefaultvolume(soundSourcePtr, volume)
	End Method
	
	Rem
	bbdoc: Returns the default volume for a sound played from this source.
	returns: 0 (silent) to 1.0f (full volume). Default value is 1.0
	about: You can influence this default volume value using setDefaultVolume().
	Note that the default volume is being multiplied with the master volume
	of ISoundEngine, change this via ISoundEngine::SetSoundVolume(). 
	End Rem
	Method GetDefaultVolume:Float()
		Return bmx_soundsource_getdefaultvolume(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Sets the default minimal distance for 3D sounds played from this source.
	about: This value influences how loud a sound is heard based on its distance.
	See ISound::setMinDistance() for details about what the min distance is.
	This method only influences the initial distance value of sounds. For changing the
	distance while the sound is playing, use ISound::setMinDistance() and ISound::setMaxDistance().
	<p>Parameters:
	<ul>
	<li><b>minDistance</b> : Default minimal distance for 3D sounds from this source. Set it to a negative
	value to let sounds of this source use the engine level default min distance, which
	can be set via ISoundEngine::setDefault3DSoundMinDistance(). Default value is -1, causing
	the default min distance of the sound engine to take effect.</li>
	</ul>
	</p>
	End Rem
	Method SetDefaultMinDistance(minDistance:Float)
		bmx_soundsource_setdefaultmindistance(soundSourcePtr, minDistance)
	End Method
	
	Rem
	bbdoc: Returns the default minimal distance for 3D sounds played from this source.
	returns: The default minimal distance for 3d sounds from this source. If setDefaultMinDistance() was set to a negative value, it will return the default value set in the engine, using ISoundEngine::setDefault3DSoundMinDistance(). Default value is -1, causing the default min distance of the sound engine to take effect.
	about: This value influences how loud a sound is heard based on its distance.
	See ISound::SetMinDistance() for details about what the minimal distance is.
	End Rem
	Method GetDefaultMinDistance:Float()
		Return bmx_soundsource_getdefaultmindistance(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Sets the default maximum distance for 3D sounds played from this source.
	about: Changing this value is usually not necessary. Use setDefaultMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefaultMinDistance()) to influence this.
	See ISound::setMaxDistance() for details about what the max distance is.
	This method only influences the initial distance value of sounds. For changing the
	distance while the sound is played, use ISound::setMinDistance() 
	and ISound::setMaxDistance().
	<p>Parameters:
	<ul>
	<li><b>maxDistance </b> : Default maximal distance for 3D sounds from this source. Set it to a negative
	value to let sounds of this source use the engine level default max distance, which
	can be set via ISoundEngine::SetDefault3DSoundMaxDistance(). Default value is -1, causing
	the default max distance of the sound engine to take effect.</li>
	</ul>
	</p>
	End Rem
	Method SetDefaultMaxDistance(maxDistance:Float)
		bmx_soundsource_setdefaultmaxdistance(soundSourcePtr, maxDistance)
	End Method
	
	Rem
	bbdoc: Returns the default maximum distance for 3D sounds played from this source.
	returns: The default maximal distance for 3D sounds from this source. If setDefaultMaxDistance() was set to a negative value, it will return the default value set in the engine, using ISoundEngine::setDefault3DSoundMaxDistance(). Default value is -1, causing the default max distance of the sound engine to take effect.
	about: This value influences how loud a sound is heard based on its distance.
	Changing this value is usually not necessary. Use setDefaultMinDistance() instead.
	Don't change this value if you don't know what you are doing: This value causes the sound
	to stop attenuating after it reaches the max distance. Most people think that this sets the
	volume of the sound to 0 after this distance, but this is not true. Only change the
	minimal distance (using for example setDefaultMinDistance()) to influence this.
	See ISound::SetMaxDistance() for details about what the max distance is.
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
	the next time.
	End Rem
	Method ForceReloadAtNextUse()
		bmx_soundsource_forcereloadatnextuse(soundSourcePtr)
	End Method
	
	Rem
	bbdoc: Sets the threshold size where irrKlang decides to force streaming a file independent of the user specified setting.
	about: When specifying ESM_NO_STREAMING for playing back a sound file, irrKlang will
	ignore this setting if the file is bigger than this threshold and stream the file
	anyway. Please note that if an audio format loader is not able to return the 
	size of a sound source and returns -1 as length, this will be ignored as well 
	and streaming has to be forced.
	<p>Parameters:
	<ul>
	<li><b>threshold</b> : New threshold. The value is specified in uncompressed bytes and its default value is 
	about one Megabyte. Set to 0 or a negative value to disable stream forcing.</li>
	</ul>
	</p>
	End Rem
	Method SetForcedStreamingThreshold(thresholdBytes:Int)
		bmx_soundsource_setforcedstreamingthreshold(soundSourcePtr, thresholdBytes)
	End Method
	
	Rem
	bbdoc: Returns the threshold size where irrKlang decides to force streaming a file independent of the user specified setting.
	about: The value is specified in uncompressed bytes and its default value is about one Megabyte. See
	SetForcedStreamingThreshold() for details.
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

	Rem
	bbdoc: Disables all active sound effects
	End Rem
	Method DisableAllEffects()
		bmx_soundeffect_disablealleffects(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the chorus sound effect or adjusts its values.
	returns: True if successful.
	about: Chorus is a voice-doubling effect created by echoing the
	original sound with a slight delay and slightly modulating the delay of the echo. 
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters:
	<ul>
	<li><b>fWetDryMix </b> : Ratio of wet (processed) signal to dry (unprocessed) signal. Minimal Value:0, Maximal Value:100.0</li>
	<li><b>fDepth </b> : Percentage by which the delay time is modulated by the low-frequency oscillator, in hundredths of a percentage point. Minimal Value:0, Maximal Value:100.0</li>
	<li><b>fFeedback </b> : Percentage of output signal to feed back into the effect's input. Minimal Value:-99, Maximal Value:99.0</li>
	<li><b>fFrequency </b> : Frequency of the LFO. Minimal Value:0, Maximal Value:10.0</li>
	<li><b>sinusWaveForm </b> : True for sinus wave form, false for triangle.</li>
	<li><b>fDelay </b> : Number of milliseconds the input is delayed before it is played back. Minimal Value:0, Maximal Value:20.0</li>
	<li><b>lPhase </b> : Phase differential between left and right LFOs. Possible values: -180, -90, 0, 90, 180</li>
	</ul>
	</p>
	End Rem
	Method EnableChorusSoundEffect:Int(fWetDryMix:Float = 50, fDepth:Float = 10, fFeedback:Float = 25, ..
			fFrequency:Float = 1.1, sinusWaveForm:Int = True, fDelay:Float = 16, lPhase:Int = 90)
		Return bmx_soundeffect_enablechorussoundeffect(soundEffectPtr, fWetDryMix, fDepth, fFeedback, ..
			fFrequency, sinusWaveForm, fDelay, lPhase)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableChorusSoundEffect()
		bmx_soundeffect_disablechorussoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsChorusSoundEffectEnabled:Int()
		Return bmx_soundeffect_ischorussoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Compressor sound effect or adjusts its values.
	returns: True if successful.
	about: Compressor is a reduction in the fluctuation of a signal above a certain amplitude. 
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters:
	<ul>
	<li><b>fGain </b> : Output gain of signal after Compressor. Minimal Value:-60, Maximal Value:60.0</li>
	<li><b>fAttack </b> : Time before Compressor reaches its full value. Minimal Value:0.01, Maximal Value:500.0</li>
	<li><b>fRelease </b> : Speed at which Compressor is stopped after input drops below fThreshold. Minimal Value:50, Maximal Value:3000.0</li>
	<li><b>fThreshold </b> : Point at which Compressor begins, in decibels. Minimal Value:-60, Maximal Value:0.0</li>
	<li><b>fRatio </b> : Compressor ratio. Minimal Value:1, Maximal Value:100.0</li>
	<li><b>fPredelay </b> : Time after lThreshold is reached before attack phase is started, in milliseconds. Minimal Value:0, Maximal Value:4.0</li>
	</ul>
	</p>
	End Rem
	Method EnableCompressorSoundEffect:Int(fGain:Float = 0, fAttack:Float = 10, fRelease:Float = 200, ..
			fThreshold:Float = -20, fRatio:Float = 3, fPredelay:Float = 4)
		Return bmx_soundeffect_enablecompressorsoundeffect(soundEffectPtr, fGain, fAttack, fRelease, ..
			fThreshold, fRatio, fPredelay)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableCompressorSoundEffect()
		bmx_soundeffect_disablecompressorsoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsCompressorSoundEffectEnabled:Int()
		Return bmx_soundeffect_iscompressorsoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Distortion sound effect or adjusts its values.
	returns: True if successful.
	about: Distortion is achieved by adding harmonics to the signal in such a way that,
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	as the level increases, the top of the waveform becomes squared off or clipped.
	<p>Parameters:
	<ul>
	<li><b>fGain </b> : Amount of signal change after distortion. Minimal Value:-60, Maximal Value:0</li>
	<li><b>fEdge </b> : Percentage of distortion intensity. Minimal Value:0, Maximal Value:100</li>
	<li><b>fPostEQCenterFrequency </b> : Center frequency of harmonic content addition. Minimal Value:100, Maximal Value:8000</li>
	<li><b>fPostEQBandwidth </b> : Width of frequency band that determines range of harmonic content addition. Minimal Value:100, Maximal Value:8000</li>
	<li><b>fPreLowpassCutoff </b> : Filter cutoff for high-frequency harmonics attenuation. Minimal Value:100, Maximal Value:8000</li>
	</ul>
	</p>
	End Rem
	Method EnableDistortionSoundEffect:Int(fGain:Float = -18, fEdge:Float = 15, fPostEQCenterFrequency:Float = 2400, ..
			fPostEQBandwidth:Float = 2400, fPreLowpassCutoff:Float = 8000)
		Return bmx_soundeffect_enabledistortionsoundeffect(soundEffectPtr, fGain, fEdge, fPostEQCenterFrequency, ..
			fPostEQBandwidth, fPreLowpassCutoff)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableDistortionSoundEffect()
		bmx_soundeffect_disabledistortionsoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsDistortionSoundEffectEnabled:Int()
		Return bmx_soundeffect_isdistortionsoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Echo sound effect or adjusts its values.
	returns: True if successful.
	about: An echo effect causes an entire sound to be repeated after a fixed delay.
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters:
	<ul>
	<li><b>fWetDryMix </b> : Ratio of wet (processed) signal to dry (unprocessed) signal. Minimal Value:0, Maximal Value:100.0</li>
	<li><b>fFeedback </b> : Percentage of output fed back into input. Minimal Value:0, Maximal Value:100.0</li>
	<li><b>fLeftDelay </b> : Delay for left channel, in milliseconds. Minimal Value:1, Maximal Value:2000.0</li>
	<li><b>fRightDelay </b> : Delay for right channel, in milliseconds. Minimal Value:1, Maximal Value:2000.0</li>
	<li><b>lPanDelay </b> : Value that specifies whether to swap left and right delays with each successive echo. Minimal Value:0, Maximal Value:1</li>
	</ul>
	</p>
	End Rem
	Method EnableEchoSoundEffect:Int(fWetDryMix:Float = 50, fFeedback:Float = 50, fLeftDelay:Float = 500, ..
			fRightDelay:Float = 500, lPanDelay:Int = 0)
		Return bmx_soundeffect_enableechosoundeffect(soundEffectPtr, fWetDryMix, fFeedback, fLeftDelay, ..
			fRightDelay, lPanDelay)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableEchoSoundEffect()
		bmx_soundeffect_disableechosoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsEchoSoundEffectEnabled:Int()
		Return bmx_soundeffect_isechosoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Flanger sound effect or adjusts its values.
	returns: True if successful.
	about: Flange is an echo effect in which the delay between the original 
	signal and its echo is very short and varies over time. The result is 
	sometimes referred to as a sweeping sound. The term flange originated
	with the practice of grabbing the flanges of a tape reel to change the speed. 
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters: 
	<ul>
	<li><b>fWetDryMix </b> : Ratio of wet (processed) signal to dry (unprocessed) signal. Minimal Value:0, Maximal Value:100.0</li>
	<li><b>fDepth </b> : Percentage by which the delay time is modulated by the low-frequency oscillator, in hundredths of a percentage point. Minimal Value:0, Maximal Value:100.0f;</li>
	<li><b>fFeedback </b> : Percentage of output signal to feed back into the effect's input. Minimal Value:-99, Maximal Value:99.0</li>
	<li><b>fFrequency </b> : Frequency of the LFO. Minimal Value:0, Maximal Value:10.0</li>
	<li><b>triangleWaveForm </b> : True for triangle wave form, false for square.</li>
	<li><b>fDelay </b> : Number of milliseconds the input is delayed before it is played back. Minimal Value:0, Maximal Value:20.0</li>
	<li><b>lPhase </b> : Phase differential between left and right LFOs. Possible values: -180, -90, 0, 90, 180</li>
	</ul>
	</p>
	End Rem
	Method EnableFlangerSoundEffect:Int(fWetDryMix:Float = 50, fDepth:Float = 100, fFeedback:Float = -50, ..
			fFrequency:Float = 0.25, triangleWaveForm:Int = True, fDelay:Float = 2, lPhase:Int = 0)
		Return bmx_soundeffect_enableflangersoundeffect(soundEffectPtr, fWetDryMix, fDepth, fFeedback, ..
			fFrequency, triangleWaveForm, fDelay, lPhase)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableFlangerSoundEffect()
		bmx_soundeffect_disableflangersoundeffect(soundEffectPtr)
	End Method
	
	Rem
	returns if the sound effect is active on the sound
	end rem
	Method IsFlangerSoundEffectEnabled:Int()
		Return bmx_soundeffect_isflangersoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Gargle sound effect or adjusts its values.
	returns: True if successful.
	about: The gargle effect modulates the amplitude of the signal. 
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters: 
	<ul>
	<li><b>rateHz </b> : Rate of modulation, in Hertz. Minimal Value:1, Maximal Value:1000</li>
	<li><b>sinusWaveForm </b> : True for sinus wave form, false for triangle.</li>
	</ul>
	</p>
	End Rem
	Method EnableGargleSoundEffect:Int(rateHz:Int = 20, sinusWaveForm:Int = True)
		Return bmx_soundeffect_enablegarglesoundeffect(soundEffectPtr, rateHz, sinusWaveForm)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableGargleSoundEffect()
		bmx_soundeffect_disablegarglesoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsGargleSoundEffectEnabled:Int()
		Return bmx_soundeffect_isgarglesoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Interactive 3D Level 2 reverb sound effect or adjusts its values.
	returns: True if successful.
	about: An implementation of the listener properties in the I3DL2 specification. Source properties are not supported.
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters: 
	<ul>
	<li><b>lRoom </b> : Attenuation of the room effect, in millibels (mB). Interval: [-10000, 0] Default: -1000 mB</li>
	<li><b>lRoomHF </b> : Attenuation of the room high-frequency effect. Interval: [-10000, 0]      default: 0 mB</li>
	<li><b>flRoomRolloffFactor </b> : Rolloff factor for the reflected signals. Interval: [0.0, 10.0]      default: 0.0</li>
	<li><b>flDecayTime </b> : Decay time, in seconds. Interval: [0.1, 20.0]      default: 1.49s</li>
	<li><b>flDecayHFRatio </b> : Ratio of the decay time at high frequencies to the decay time at low frequencies. Interval: [0.1, 2.0]       default: 0.83</li>
	<li><b>lReflections </b> : Attenuation of early reflections relative to lRoom. Interval: [-10000, 1000]   default: -2602 mB</li>
	<li><b>flReflectionsDelay </b> : Delay time of the first reflection relative to the direct path in seconds. Interval: [0.0, 0.3]       default: 0.007 s</li>
	<li><b>lReverb </b> : Attenuation of late reverberation relative to lRoom, in mB. Interval: [-10000, 2000]   default: 200 mB</li>
	<li><b>flReverbDelay </b> : Time limit between the early reflections and the late reverberation relative to the time of the first reflection. Interval: [0.0, 0.1]       default: 0.011 s</li>
	<li><b>flDiffusion </b> : Echo density in the late reverberation decay in percent. Interval: [0.0, 100.0]     default: 100.0 %</li>
	<li><b>flDensity </b> : Modal density in the late reverberation decay, in percent. Interval: [0.0, 100.0]     default: 100.0 %</li>
	<li><b>flHFReference </b> : Reference high frequency, in hertz. Interval: [20.0, 20000.0]  default: 5000.0 Hz </li>
	</ul>
	</p>
	End Rem
	Method EnableI3DL2ReverbSoundEffect:Int(lRoom:Int = -1000, lRoomHF:Int = -100, flRoomRolloffFactor:Float = 0, ..
			flDecayTime:Float = 1.49, flDecayHFRatio:Float = 0.83, lReflections:Int = -2602, flReflectionsDelay:Float = 0.007, ..
			lReverb:Int = 200, flReverbDelay:Float = 0.011, flDiffusion:Float = 100.0, ..
			flDensity:Float = 100.0, flHFReference:Float = 5000.0 )
		Return bmx_soundeffect_enablei3dl2reverbsoundeffect(soundEffectPtr, lRoom, lRoomHF, flRoomRolloffFactor, ..
			flDecayTime, flDecayHFRatio, lReflections, flReflectionsDelay, ..
			lReverb, flReverbDelay, flDiffusion, flDensity, flHFReference)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableI3DL2ReverbSoundEffect()
		bmx_soundeffect_disablei3dl2reverbsoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsI3DL2ReverbSoundEffectEnabled:Int()
		Return bmx_soundeffect_isi3dl2reverbsoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the ParamEq sound effect or adjusts its values.
	returns: True if successful.
	about: Parametric equalizer amplifies or attenuates signals of a given frequency. 
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.
	<p>Parameters: 
	<ul>
	<li><b>fCenter </b> : Center frequency, in hertz, The default value is 8000. Minimal Value:80, Maximal Value:16000.0</li>
	<li><b>fBandwidth </b> : Bandwidth, in semitones, The default value is 12. Minimal Value:1.0, Maximal Value:36.0</li>
	<li><b>fGain </b> : Default value is 0. Minimal Value:-15.0, Maximal Value:15.0</li>
	</ul>
	</p>
	End Rem
	Method EnableParamEqSoundEffect:Int(fCenter:Float = 8000, fBandwidth:Float = 12, fGain:Float = 0)
		Return bmx_soundeffect_enableparameqsoundeffect(soundEffectPtr, fCenter, fBandwidth, fGain)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableParamEqSoundEffect()
		bmx_soundeffect_disableparameqsoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsParamEqSoundEffectEnabled:Int()
		Return bmx_soundeffect_isparameqsoundeffectenabled(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Enables the Waves Reverb sound effect or adjusts its values.
	returns: True if successful.
	about: Parameters: 
	<ul>
	<li><b>fInGain </b> : Input gain of signal, in decibels (dB). Min/Max: [-96.0,0.0] Default: 0.0 dB.
	If this sound effect is already enabled, calling this only modifies the parameters of the active effect.</li>
	<li><b>fReverbMix </b> : Reverb mix, in dB. Min/Max: [-96.0,0.0] Default: 0.0 dB</li>
	<li><b>fReverbTime </b> : Reverb time, in milliseconds. Min/Max: [0.001,3000.0] Default: 1000.0 ms</li>
	<li><b>fHighFreqRTRatio </b> : High-frequency reverb time ratio. Min/Max: [0.001,0.999] Default: 0.001</li>
	</ul>
	End Rem
	Method EnableWavesReverbSoundEffect:Int(fInGain:Float = 0, fReverbMix:Float = 0, ..
			fReverbTime:Float = 1000, fHighFreqRTRatio:Float = 0.001)
		Return bmx_soundeffect_enablewavesreverdsoundeffect(soundEffectPtr, fInGain, fReverbMix, fReverbTime, fHighFreqRTRatio)
	End Method
	
	Rem
	bbdoc: Removes the sound effect from the sound
	End Rem
	Method DisableWavesReverbSoundEffect()
		bmx_soundeffect_disablewavesreverbsoundeffect(soundEffectPtr)
	End Method
	
	Rem
	bbdoc: Returns if the sound effect is active on the sound
	End Rem
	Method IsWavesReverbSoundEffectEnabled:Int()
		Return bmx_soundeffect_iswavesreverbsoundeffectenabled(soundEffectPtr)
	End Method

End Type

Rem
bbdoc: A 3d vector template type for representing vectors and points in 3d.
End Rem
Type TIVec3D

	Field vecPtr:Byte Ptr
	
	Function _create:TIVec3D(vecPtr:Byte Ptr)
		If vecPtr Then
			Local this:TIVec3D = New TIVec3D
			this.vecPtr = vecPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CreateVec3D:TIVec3D(x:Float = 0.0, y:Float = 0.0, z:Float = 0.0)
		Return New TIVec3D.Create(x, y, z)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function Set:TIVec3D(x:Float = 0.0, y:Float = 0.0, z:Float = 0.0)
		Return New TIVec3D.Create(x, y, z)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TIVec3D(_x:Float = 0.0, _y:Float = 0.0, _z:Float = 0.0)
		vecPtr = bmx_vec3df_create(_x, _y, _z)
		Return Self
	End Method

	Method Delete()
		If vecPtr Then
			bmx_vec3df_delete(vecPtr)
			vecPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function Zero:TIVec3D()
		Return New TIVec3D.Create()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method X:Float()
		Return bmx_vec3df_x(vecPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Y:Float()
		Return bmx_vec3df_y(vecPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Z:Float()
		Return bmx_vec3df_z(vecPtr)
	End Method
	
	Rem
	bbdoc: Inverts the vector. 
	End Rem
	Method Invert()
	End Method
	
	Rem
	bbdoc: Calculates the cross product with another vector.
	End Rem
	Method CrossProduct:TIVec3D(p:TIVec3D)
	End Method
	
	Rem
	bbdoc: Returns the dot product with another vector.
	End Rem
	Method DotProduct:Float(other:TIVec3D)
	End Method
	 
	Rem
	bbdoc: Returns true if this vector is the same as other.
	End Rem
	Method Equals:Int(other:TIVec3D) 
	End Method
	 
	Rem
	bbdoc: Returns distance from an other point. 
	about: Here, the vector is interpreted as point in 3 dimensional space. 
	End Rem
	Method GetDistanceFrom:Double(other:TIVec3D)
	End Method
	 
	Rem
	bbdoc: Returns squared distance from an other point.
	about: Here, the vector is interpreted as point in 3 dimensional space. 
	End Rem
	Method GetDistanceFromSQ:Float(other:TIVec3D)
	End Method
	 
	Rem
	bbdoc: Gets the Y and Z rotations of a vector.
	End Rem
	Method GetHorizontalAngle:TIVec3D() 
	End Method
	 
	Rem
	bbdoc: Returns interpolated vector. 
	End Rem
	Method GetInterpolated:TIVec3D(other:TIVec3D, d:Float)
	End Method
	 
	Rem
	bbdoc: Returns length of the vector.
	End Rem
	Method GetLength:Double()  
	End Method
	 
	Rem
	bbdoc: Returns squared length of the vector. 
	about: This is useful because it is much faster than GetLength(). 
	End Rem
	Method GetLengthSQ:Double()
	End Method
	 
	Rem
	bbdoc: Returns if this vector interpreted as a point is on a line between two other points. 
	about: It is assumed that the point is on the line. 
	End Rem
	Method IsBetweenPoints (startPoint:TIVec3D, endPoint:TIVec3D)
	End Method
	 
	Rem
	bbdoc: Normalizes the vector. 
	End Rem
	Method Normalize:TIVec3D() 
	End Method

	Rem
	bbdoc: Rotates the vector by a specified number of degrees around the Z axis and the specified center.
	End Rem
	Method RotateXYBy(degrees:Double, center:TIVec3D)
	End Method
	 
	Rem
	bbdoc: Rotates the vector by a specified number of degrees around the Y axis and the specified center. 
	End Rem
	Method RotateXZBy(degrees:Double, center:TIVec3D) 
	End Method

	Rem
	bbdoc: Rotates the vector by a specified number of degrees around the X axis and the specified center. 
	End Rem
	Method RotateYZBy(degrees:Double, center:TIVec3D) 
	End Method

End Type

Rem
bbdoc: Type describing an audio stream format with helper functions 
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

	Method Delete()
		If formatPtr Then
			bmx_audiostreamformat_delete(formatPtr)
			formatPtr = Null
		End If
	End Method

	Rem
	bbdoc: Number of channels, 1 for mono, 2 for stereo
	End Rem
	Method GetChannelCount:Int()
		Return bmx_audiostreamformat_getchannelcount(formatPtr)
	End Method

	Rem
	bbdoc: Amount of frames in the sample data or stream. 
	about: If the stream has an unknown lenght, this is -1
	End Rem
	Method GetFrameCount:Int()
		Return bmx_audiostreamformat_getframecount(formatPtr)
	End Method

	Rem
	bbdoc: Samples per second
	End Rem
	Method GetSampleRate:Int()
		Return bmx_audiostreamformat_getsamplerate(formatPtr)
	End Method
	
	Rem
	bbdoc: Format of the sample data.
	about: One of ESF_U8 or ESF_S16.
	End Rem
	Method GetSampleFormat:Int()
		Return bmx_audiostreamformat_getsampleformat(formatPtr)
	End Method

	Rem
	bbdoc: Returns the size of a sample of the data described by the stream data in bytes
	End Rem
	Method GetSampleSize:Int()
		Return bmx_audiostreamformat_getsamplesize(formatPtr)
	End Method
	
	Rem
	bbdoc: Returns the frame size of the stream data in bytes
	End Rem
	Method GetFrameSize:Int()
		Return bmx_audiostreamformat_getframesize(formatPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the sample data in bytes
	about: Returns an invalid negative value when the stream has an unknown length
	End Rem
	Method GetSampleDataSize:Int()
		Return bmx_audiostreamformat_getsampledatasize(formatPtr)
	End Method
	
	Rem
	bbdoc: Returns amount of bytes per second
	End Rem
	Method GetBytesPerSecond:Int()
		Return bmx_audiostreamformat_getbytespersecond(formatPtr)
	End Method

End Type

Rem
bbdoc: A type for receiving notifications of sound stop events.
about: The sound stop event is guaranteed to be called when a sound or sound stream is finished, either because
the sound reached its playback end, its sound source was removed, ISoundEngine::stopAllSounds() has been called
or the whole engine was deleted.
<p>
This type should be extended, and the OnSoundStopped method implemented.
</p>
End Rem
Type TISoundStopEventReceiver

	Field recPtr:Byte Ptr
	Field userData:Object

	Rem
	bbdoc: Creates a new instance of TISoundStopEventReceiver
	End Rem
	Function CreateSoundStopEventReceiver:TISoundStopEventReceiver()
		Return New TISoundStopEventReceiver.Create()
	End Function
	
	Rem
	bbdoc: Creates a new instance of TISoundStopEventReceiver
	End Rem
	Method Create:TISoundStopEventReceiver()
		recPtr = bmx_soundstopeventreceiver_create(Self)
		Return Self
	End Method

	Function _OnSoundStopped(sound:Byte Ptr, reason:Int, rec:TISoundStopEventReceiver)
		rec.OnSoundStopped(TISound._create(sound), reason, rec.userData)
	End Function
	
	Rem
	bbdoc: Called when a sound has stopped playing. 
	about: This is the only method to be implemented by the user. The sound stop event is guaranteed to be called
	when a sound or sound stream is finished, either because the sound reached its playback end, its sound
	source was removed, ISoundEngine::stopAllSounds() has been called or the whole engine was deleted.
	End Rem
	Method OnSoundStopped(sound:TISound, reason:Int, userData:Object)
	End Method
	
	Method Delete()
		If recPtr Then
			bmx_soundstopeventreceiver_delete(recPtr)
			recPtr = Null
		End If
	End Method
	
End Type

