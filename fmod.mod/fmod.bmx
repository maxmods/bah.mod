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

Rem
bbdoc: FMOD
about: FMOD Ex sound system.
End Rem
Module BaH.FMOD

ModuleInfo "Version: 1.01"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: FMOD - See http://www.fmod.org"
ModuleInfo "Copyright: Wrapper - 2008-2013 Bruce A Henderson"
ModuleInfo "Copyright: FMOD - 2004-2011 Firelight Technologies, Pty, Ltd"

ModuleInfo "History: 1.01"
ModuleInfo "History: Re-synchronised consts to match header values."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (FMOD 4.44.06)"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?macos
' -bind_at_load removes linker warnings on 10.4
ModuleInfo "LD_OPTS: -bind_at_load -L%PWD%/lib/macos"
?linux
ModuleInfo "LD_OPTS: -L%PWD%/lib/linux"
?

Import "common.bmx"

' for documenting :
Rem
	<p>Parameters: 
	<ul>
	<li><b>XXXXXXXXXX</b> : xxxxxxxxxxxxxxxxx</li>
	</ul>
	<p>

	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>XXXXXXXXXXXXX</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	</table>

End Rem

Rem
bbdoc: The current version of FMOD Ex being used.
about: The version is a 32bit hexadecimal value formated as 16:8:8, with the upper 16bits being the major version,
the middle 8bits being the minor version and the bottom 8bits being the development version. For example a value of 00040106h is equal to 4.01.06
End Rem
Global FMOD_VERSION:Int = bmx_fmod_getversion()

Rem
bbdoc: Returns a string representation of the error code.
End Rem
Function FMOD_ErrorString:String(result:Int)
	Return String.FromCString(bmx_FMOD_ErrorString(result))
End Function

Rem
bbdoc: Mutex function to synchronize user file reads with FMOD's file reads.
about: Parameters: 
<ul>
<li><b>busy</b> : 1 = you are about to perform a disk access. 0 = you are finished with the disk.</li>
</ul>
<p>
This function tells fmod that you are using the disk so that it will  block until you are finished with it.
</p>
<p>
This function also blocks if FMOD is already using the disk, so that you cannot do a read at the same time FMOD is reading.
</p>
End Rem
Function FMOD_SetDiskBusy:Int(busy:Int)
	Return FMOD_File_SetDiskBusy(busy)
End Function

Rem
bbdoc: Information function to retreive the state of fmod's disk access.
about: Parameters: 
<ul>
<li><b>busy</b> : A variable to receive the busy state of the disk at the current time.</li>
</ul>
<p>
Do not use this function to syncrhonize your own reads with, as due to timing, you might call this function
and it says false = it is not busy, but the split second after call this function, internally FMOD might set it
to busy. Use #FMOD_SetDiskBusy for proper mutual exclusion as it uses semaphores.
</p>
End Rem
Function FMOD_GetDiskBusy:Int(busy:Int Var)
	Return FMOD_File_GetDiskBusy(Varptr busy)
End Function

Rem
bbdoc: The FMOD System object.
End Rem
Type TFMODSystem

	Field systemPtr:Byte Ptr
	
	Rem
	bbdoc: Creates an instance of an FMOD system object.
	End Rem
	Function CreateSystem:TFMODSystem()
		Return New TFMODSystem.Create()
	End Function

	Rem
	bbdoc: Creates an instance of an FMOD system object.
	End Rem
	Method Create:TFMODSystem()
		Local res:Int
		systemPtr = bmx_FMOD_System_Create(Varptr res)
		If res Then
			Return Null
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: Adds a pre-created DSP unit or effect to the head of the System DSP chain.  
	about: This is a wrapper function to insert a DSP unit at the top of the System DSP chain.
	<p>
	It disconnects the head unit from its input, then inserts the unit at the head and reconnects the previously
	disconnected input back as as an input to the new unit.
	</p>
	End Rem
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
	bbdoc: Updates the FMOD system.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: This should be called once per 'game' tick, or once per frame in your application.  
	<p>
	This updates the following things :
	</p>
	<p>
	3D Sound. 3D positioning will not update if this function is not called. Virtual voices. If more voices are played
	than there are real hardware/software voices, this function must be called to handle the virtualization. *_NRT
	output modes. This function must be called to drive the output for these output modes. FMOD_INIT_STREAM_FROM_UPDATE.
	This function must be called to update the streamer if this flag has been used. Callbacks. This function must be
	called to fire callbacks if they are specified. FMOD_NONBLOCKING. This function must be called to make sounds
	opened with FMOD_NONBLOCKING flag to work properly. 
	</p>
	<p>
	If FMOD_OUTPUTTYPE_NOSOUND_NRT or FMOD_OUTPUTTYPE_WAVWRITER_NRT output modes are used, this function also drives
	the software / DSP engine, instead of it running asynchronously in a thread as is the default behaviour.
	</p>
	<p>
	This can be used for faster than realtime updates to the decoding or DSP engine which might be useful if the output
	is the wav writer for example.
	</p>
	<p>
	If FMOD_INIT_STREAM_FROM_UPDATE is used, this function will update the stream engine. Combining this with the non
	realtime output will mean smoother captured output.
	</p>
	End Rem
	Method Update:Int()
		Return FMOD_System_Update(systemPtr)
	End Method
	
	Rem
	bbdoc: Creates a 'virtual reverb' object.
	about: This object reacts to 3d location and morphs the reverb environment based on how close it is to the
	reverb object's center.
	<p>
	Multiple reverb objects can be created to achieve a multi-reverb environment.  
	</p>
	<p>The 3D reverb object is a sphere having 3D attributes (position, minimum distance, maximum distance) and
	reverb properties.
	</p>
	<p>
	The properties and 3D attributes of all reverb objects collectively determine, along with the listener's position,
	the settings of and input gains into a single 3D reverb DSP.
	</p>
	<p>
	Please note that this only applies to software channels. When the listener is within the sphere of effect of one
	or more 3d reverbs, the listener's 3D reverb properties are a weighted combination of such 3d reverbs. When the
	listener is outside all of the reverbs, the 3D reverb setting is set to the default ambient reverb setting.
	</p>
	<p>
	Use System.SetReverbAmbientProperties to set a 'background' default reverb environment. This is a reverb that
	will be morphed to if the listener is not within any virtual reverb zones.
	</p>
	<p>
	By default the ambient reverb is set to 'off'. 
	</p>
	<p>
	Creating multiple reverb objects does not impact performance. These are 'virtual reverbs'. There will still be
	only 1 physical reverb DSP running that just morphs between the different virtual reverbs. 
	</p>
	<p>
	System.SetReverbProperties can still be used in conjunction with the 3d based virtual reverb system. This allows
	2d sounds to have reverb. If this call is used at the same time virtual reverb objects are active, 2 physical
	reverb dsps will be used, incurring a small memory and cpu hit. 
	</p>
	End Rem
	Method CreateReverb:TFMODReverb()
		Return TFMODReverb._create(bmx_FMOD_System_CreateReverb(systemPtr))
	End Method
	
	Rem
	bbdoc: Loads a sound into memory, or opens it for streaming. 
	End Rem
	Method CreateSoundURL:TFMODSound(filename:String, Mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		Local s:Byte Ptr
		' 16-bit chars?
		If Mode & FMOD_UNICODE Then
			s = filename.ToWString()
		Else
			s = filename.ToCString()
		End If
	
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, s, Mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, s, Mode, Null, Varptr ret))
		End If
		MemFree(s)
		
		Return sound
	End Method
	
	Rem
	bbdoc: Loads a sound into memory, or opens it for streaming. 
	End Rem
	Method CreateSoundPtr:TFMODSound(mem:Byte Ptr, Mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, mem, Mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, mem, Mode, Null, Varptr ret))
		End If
		
		Return sound
	End Method

	Rem
	bbdoc: Loads a sound into memory, or opens it for streaming. 
	End Rem
	Method CreateSound:TFMODSound(Mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, Null, Mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateSound(systemPtr, Null, Mode, Null, Varptr ret))
		End If
		
		Return sound
	End Method

	Rem
	bbdoc: Opens a sound for streaming.
	about: This is a helper method that is the same as System.CreateSound but has the FMOD_CREATESTREAM flag added internally.  
	End Rem
	Method CreateStreamURL:TFMODSound(filename:String, Mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		Local s:Byte Ptr
		' 16-bit chars?
		If Mode & FMOD_UNICODE Then
			s = filename.ToWString()
		Else
			s = filename.ToCString()
		End If
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, s, Mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, s, Mode, Null, Varptr ret))
		End If
		MemFree(s)
		
		Return sound
	End Method

	Rem
	bbdoc: Opens a sound for streaming.
	about: This is a helper method that is the same as System.CreateSound but has the FMOD_CREATESTREAM flag added internally.  
	End Rem
	Method CreateStreamPtr:TFMODSound(mem:Byte Ptr, Mode:Int, exInfo:TFMODCreateSoundExInfo = Null)
		Local sound:TFMODSound
		Local ret:Int
		
		If exInfo Then
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, mem, Mode, exInfo.soundExInfoPtr, Varptr ret))
		Else
			sound = TFMODSound._create(bmx_FMOD_System_CreateStream(systemPtr, mem, Mode, Null, Varptr ret))
		End If
		
		Return sound
	End Method
	
	Rem
	bbdoc: Creates a channel group object.
	about: These objects can be used to assign channels to for group channel settings, such as volume.
	<p>
	Channel groups are also used for sub-mixing. Any channels that are assigned to a channel group get submixed
	into that channel group's DSP.  
	</p>
	<p>
	See the channel group type definition for the types of operations that can be performed on 'groups' of channels.
	</p>
	<p>
	The channel group can for example be used to have 2 seperate groups of master volume, instead of one global
	master volume.
	</p>
	<p>
	A channel group can be used for sub-mixing, ie so that a set of channels can be mixed into a channel group,
	then can have effects applied to it without affecting other channels.
	</p>
	End Rem
	Method CreateChannelGroup:TFMODChannelGroup(name:String)
		Local s:Byte Ptr = name.ToCString()
		Local group:TFMODChannelGroup = TFMODChannelGroup._create(bmx_FMOD_System_CreateChannelGroup(systemPtr, s))
		MemFree(s)
		Return group
	End Method
	
	Rem
	bbdoc: Retrieves a handle to the internal master channel group.
	about: This is the default channel group that all channels play on.
	<p>
	This channel group can be used to do things like set the master volume for all playing sounds. See the
	TFMODChannelGroup API for more functionality. 
	</p>
	End Rem
	Method GetMasterChannelGroup:TFMODChannelGroup()
		Return TFMODChannelGroup._create(bmx_FMOD_System_GetMasterChannelGroup(systemPtr))
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
	about: Parameters: 
	<ul>
	<li><b>channelId</b> : Use the value #FMOD_CHANNEL_FREE to get FMOD to pick a free channel.
	Otherwise specify a channel number from 0 to the 'maxchannels' value specified in System::init minus 1.</li>
	<li><b>sound</b> : The sound to play. This is opened with System::createSound. </li>
	<li><b>paused</b> : True or False flag to specify whether to start the channel paused or not. Starting a
	channel paused allows the user to alter its attributes without it being audible, and unpausing with
	Channel::setPaused actually starts the sound.</li>
	<li><b>reuse</b> : An option channel that receives the newly playing channel. If #FMOD_CHANNEL_REUSE is used, this
	can contain a previously used channel handle and FMOD will re-use it to play a sound on.</li>
	</ul>
	<p>
	When a sound is played, it will use the sound's default frequency, volume, pan, levels and priority.
	</p>
	<p>
	A sound defined as #FMOD_3D will by default play at the position of the listener.
	</p>
	<p>
	To change channel attributes before the sound is audible, start the channel paused by setting the paused flag to true, and calling
	the relevant channel based methods. Following that, unpause the channel with Channel::setPaused.
	</p>
	<p>
	If #FMOD_CHANNEL_FREE is used as the channel index, it will pick an arbitrary free channel and use channel management. (As described below).<br>
	If #FMOD_CHANNEL_REUSE is used as the channel index, FMOD Ex will re-use the channel handle that is passed in as the 'channel' parameter.
	If NULL or 0 is passed in as the channel handle it will use the same logic as FMOD_CHANNEL_FREE and pick an arbitrary channel.
	</p>
	<p>
	Channels are reference counted. If a channel is stolen by the FMOD priority system, then the handle to the stolen voice becomes
	invalid, and Channel based commands will not affect the new sound playing in its place.<br>
	If all channels are currently full playing a sound, FMOD will steal a channel with the lowest priority sound.<br>
	If more channels are playing than are currently available on the soundcard/sound device or software mixer, then FMOD will
	'virtualize' the channel. This type of channel is not heard, but it is updated as if it was playing. When its priority
	becomes high enough or another sound stops that was using a real hardware/software channel, it will start playing from where
	it should be. This technique saves CPU time (thousands of sounds can be played at once without actually being mixed or taking
	up resources), and also removes the need for the user to manage voices themselves.<br>
	An example of virtual channel usage is a dungeon with 100 torches burning, all with a looping crackling sound, but with a soundcard
	that only supports 32 hardware voices. If the 3D positions and priorities for each torch are set correctly, FMOD will play all 100
	sounds without any 'out of channels' errors, and swap the real voices in and out according to which torches are closest in 3D space.<br>
	Priority for virtual channels can be changed in the sound's defaults, or at runtime with Channel::setPriority.
	</p>
	End Rem
	Method PlaySound:TFMODChannel(channelId:Int, sound:TFMODSound, paused:Int = False, reuse:TFMODChannel = Null)
		If reuse Then
			Return TFMODChannel._create(bmx_FMOD_System_PlaySound(systemPtr, channelId, sound.soundPtr, paused, reuse.channelPtr))
		Else
			Return TFMODChannel._create(bmx_FMOD_System_PlaySound(systemPtr, channelId, sound.soundPtr, paused, Null))
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method PlayDSP:TFMODChannel(channelId:Int, dsp:TFMODDsp, paused:Int = False, reuse:TFMODChannel = Null)
		If reuse Then
			bmx_FMOD_System_PlayDSP(systemPtr, channelId, dsp.dspPtr, paused, reuse.channelPtr)
			Return reuse
		Else
			Return TFMODChannel._create(bmx_FMOD_System_PlayDSP(systemPtr, channelId, dsp.dspPtr, paused, Null))
		End If
	End Method
	
	Rem
	bbdoc: Retrieves the position, velocity and orientation of the specified 3D sound listener.
	about: Parameters: 
	<ul>
	<li><b>listener</b> : Listener ID in a multi-listener environment. Specify 0 if there is only 1 listener. </li>
	<li><b>pos</b> : A variable that receives the position of the listener in world space, measured in distance units. Optional. Specify Null to ignore.</li>
	<li><b>vel</b> : A variable that receives the velocity of the listener measured in distance units per second. Optional. Specify Null to ignore </li>
	<li><b>forward</b> : A variable that receives the forwards orientation of the listener. Optional. Specify Null to ignore.</li>
	<li><b>up</b> : A variable that receives the upwards orientation of the listener. Optional. Specify Null to ignore.</li>
	</ul>
	End Rem
	Method Get3DListenerAttributes:Int(listener:Int, pos:TFMODVector = Null, vel:TFMODVector = Null, ..
			forward:TFMODVector = Null, up:TFMODVector = Null)
		Return FMOD_System_Get3DListenerAttributes(systemPtr, listener, Varptr pos, Varptr vel, Varptr forward, Varptr up)
	End Method
	
	Rem
	bbdoc: Retrieves the number of 3D listeners.
	about: Parameters: 
	<ul>
	<li><b>listeners</b> : A variable that receives the current number of 3D listeners in the 3D scene.</li>
	</ul>
	End Rem
	Method Get3DNumListeners:Int(listeners:Int Var)
		Return FMOD_System_Get3DNumListeners(systemPtr, Varptr listeners)
	End Method
	
	Rem
	bbdoc: Retrieves the global doppler scale, distance factor and rolloff scale for all 3D sound in FMOD.
	about: Parameters: 
	<ul>
	<li><b>dopplerScale</b> : A variable that receives the scaling factor for doppler shift.</li>
	<li><b>distanceFactor</b> : A variable that receives the relative distance factor to FMOD's units.</li>
	<li><b>rollOffScale</b> : A variable that receives the scaling factor for 3D sound rolloff or attenuation.</li>
	</ul>
	End Rem
	Method Get3DSettings:Int(dopplerScale:Float Var, distanceFactor:Float Var, rollOffScale:Float Var)
		Return FMOD_System_Get3DSettings(systemPtr, Varptr dopplerScale, Varptr distanceFactor, Varptr rollOffScale)
	End Method
	
	Rem
	bbdoc: Retrieves the current speaker position information for the selected speaker.
	about: Parameters: 
	<ul>
	<li><b>speaker</b> : The selected speaker of interest to return the x and y position. </li>
	<li><b>x</b> : A variable that receives the 2D X position relative to the listener.</li>
	<li><b>y</b> : A variable that receives the 2D Y position relative to the listener.</li>
	<li><b>active</b> : A variable that receives the active state of a speaker.</li>
	</ul>
	<p>
	See #set3DSpeakerPosition for more information on speaker positioning.
	</p>
	End Rem
	Method Get3DSpeakerPosition:Int(speaker:Int, x:Float Var, y:Float Var, active:Int Var)
		Return FMOD_System_Get3DSpeakerPosition(systemPtr, speaker, Varptr x, Varptr y, Varptr active)
	End Method
	
	?win32
	Rem
	bbdoc:
	End Rem
	Method GetCDROMDriveName:String[](drive:Int)
		' TODO
	End Method
	?
	
	Rem
	bbdoc: Retrieves in percent of CPU time - the amount of cpu usage that FMOD is taking for streaming/mixing and System::Update combined.
	about: Parameters: 
	<ul>
	<li><b>dsp</b> : A variable that receives the current dsp mixing engine cpu usage. Result will be from 0 to 100.0.</li>
	<li><b>stream</b> : A variable that receives the current streaming engine cpu usage. Result will be from 0 to 100.0.</li>
	<li><b>geometry</b> : A variable that receives the current geometry engine cpu usage. Result will be from 0 to 100.0.</li>
	<li><b>update</b> : A variable that receives the current #update cpu usage. Result will be from 0 to 100.0.</li>
	<li><b>total</b> : A variable that receives the current total cpu usage. Result will be from 0 to 100.0.</li>
	</ul>
	<p>
	This value is slightly smoothed to provide more stable readout (and to round off spikes that occur due to multitasking/operating system issues).
	</p>
	End Rem
	Method GetCPUUsage:Int(dsp:Float Var, stream:Float Var, geometry:Float Var, update:Float Var, total:Float Var)
		Return FMOD_System_GetCPUUsage(systemPtr, Varptr dsp, Varptr stream, Varptr geometry, Varptr update, Varptr total)
	End Method

	Rem
	bbdoc: Retrieves a handle to a channel by ID.  
	about: Parameters: 
	<ul>
	<li><b>channelId</b> : Index in the FMOD channel pool. Specify a channel number from 0 to the 'maxchannels' value specified in #init minus 1. </li>
	</ul>
	<p>
	This method is mainly for getting handles to existing (playing) channels and setting their attributes.
	</p>
	End Rem
	Method GetChannel:TFMODChannel(channelId:Int)
		Return TFMODChannel._create(bmx_FMOD_System_GetChannel(systemPtr, channelId))
	End Method
	
	Rem
	bbdoc: Retrieves the number of currently playing channels.
	about: Parameters: 
	<ul>
	<li><b>channels</b> : A variable that receives the number of currently playing channels.</li>
	</ul>
	End Rem
	Method GetChannelsPlaying:Int(channels:Int Var)
		Return FMOD_System_GetChannelsPlaying(systemPtr, Varptr channels)
	End Method
	
	Rem
	bbdoc: Retrieves the buffer size settings for the FMOD software mixing engine.  
	End Rem
	Method GetDSPBufferSize:Int(bufferLength:Int Var, numBuffers:Int Var)
		Return FMOD_System_GetDSPBufferSize(systemPtr, Varptr bufferLength, Varptr numBuffers)
	End Method
	
	Rem
	bbdoc: Return the current 64bit DSP clock value which counts up by the number of samples per second in the software mixer, every second.
	about: ie. if the default sample rate is 48khz, the DSP clock increments by 48000 per second.  
	End Rem
	Method GetDSPClock:Int(hi:Int Var, lo:Int Var)
		Return FMOD_System_GetDSPClock(systemPtr, Varptr hi, Varptr lo)
	End Method
	
	Rem
	bbdoc: Returns the currently selected driver number.
	about: Drivers are enumerated when selecting a driver with System::setDriver or other driver related functions such as
	System::getNumDrivers or System::getDriverInfo  
	End Rem
	Method GetDriver:Int(driver:Int Var)
		Return FMOD_System_GetDriver(systemPtr, Varptr driver)
	End Method
	
	Rem
	bbdoc: Returns information on capabilities of the current output mode for the selected sound device.
	End Rem
	Method GetDriverCaps:Int(id:Int, caps:Int Var, controlpanelOutputRate:Int Var, controlPanelSpeakerMode:Int Var)
		Return FMOD_System_GetDriverCaps(systemPtr, id, Varptr caps, Varptr controlpanelOutputRate, Varptr controlPanelSpeakerMode)
	End Method
	
	'Method GetDriverInfo()
	'End Method
	
	Rem
	bbdoc: Retrieves the maximum world size for the geometry engine.
	End Rem
	Method GetGeometrySettings:Int(maxWorldSize:Float Var)
		Return FMOD_System_GetGeometrySettings(systemPtr, Varptr maxWorldSize)
	End Method
	
	Rem
	bbdoc: Returns the number of available hardware mixed 2d and 3d channels.  
	End Rem
	Method GetHardwareChannels:Int(numHardwareChannels:Int Var)
		Return FMOD_System_GetHardwareChannels(systemPtr, Varptr numHardwareChannels)
	End Method
	
	Rem
	bbdoc: Retrieves the number of available CDROM drives on the user's machine.  
	End Rem
	Method GetNumCDROMDrives:Int(numDrives:Int Var)
		Return FMOD_System_GetNumCDROMDrives(systemPtr, Varptr numDrives)
	End Method
	
	Rem
	bbdoc: Retrieves the amount of dedicated sound ram available if the platform supports it.
	about: Currently only support on GameCube.  Most platforms use main ram to store audio data, so this function usually isn't necessary.  
	End Rem
	Method GetSoundRAM:Int(currentAlloced:Int Var, maxAlloced:Int Var, total:Int Var)
		Return FMOD_System_GetSoundRAM(systemPtr, Varptr currentAlloced, Varptr maxAlloced, Varptr total)
	End Method
	
	Rem
	bbdoc: Retrieves the spectrum from the currently playing output signal.  
	End Rem
	Method GetSpectrum:Int(spectrumArray:Float[], channelOffset:Int, windowType:Int)
		Return bmx_FMOD_System_GetSpectrum(systemPtr, spectrumArray, channelOffset, windowType)
	End Method
	
	Rem
	bbdoc: Retrieves the current speaker mode.  
	End Rem
	Method GetSpeakerMode:Int(speakerMode:Int Var)
		Return FMOD_System_GetSpeakerMode(systemPtr, Varptr speakerMode)
	End Method
	
	Rem
	bbdoc: Returns the current internal buffersize settings for streamable sounds.
	End Rem
	Method GetStreamBufferSize:Int(fileBufferSize:Int Var, fileBufferSizeType:Int Var)
		Return FMOD_System_GetStreamBufferSize(systemPtr, Varptr fileBufferSize, Varptr fileBufferSizeType)
	End Method
	
	Rem
	bbdoc: Retrieves the output format for the software mixer.  
	End Rem
	Method GetSoftwareFormat:Int(sampleRate:Int Var, format:Int Var, numOutputChannels:Int Var, ..
			maxInputChannels:Int Var, resampleMethod:Int Var, bits:Int Var)
		Return FMOD_System_GetSoftwareFormat(systemPtr, Varptr sampleRate, Varptr format, Varptr numOutputChannels, ..
				Varptr maxInputChannels, Varptr resampleMethod, Varptr bits)
	End Method
	
	Rem
	bbdoc: Retrieves the maximum number of software mixed channels possible.
	about: Software mixed voices are used by sounds loaded with FMOD_SOFTWARE.  
	End Rem
	Method GetSoftwareChannels:Int(numSoftwareChannels:Int Var)
		Return FMOD_System_GetSoftwareChannels(systemPtr, Varptr numSoftwareChannels)
	End Method
	
	'Rem
	'bbdoc: Retrieves the currently selected recording driver, usually set with System::setRecordDriver.  
	'End Rem
	'Method GetRecordDriver:Int(driver:Int Var)
	'	Return FMOD_System_GetRecordDriver(systemPtr, Varptr driver)
	'End Method
	
	Rem
	bbdoc: Returns information on capabilities of the Current output Mode For the selected recording sound device.
	End Rem
	Method GetRecordDriverCaps:Int(id:Int, caps:Int Var, minFrequency:Int Var, maxFrequency:Int Var)
		Return FMOD_System_GetRecordDriverCaps(systemPtr, id, Varptr caps, Varptr minFrequency, Varptr maxFrequency)
	End Method
	
	Rem
	bbdoc: Retrieves identification information about a sound device specified by its index, And specific To the output mode set with System::setOutput.
	End Rem
	Method GetRecordDriverInfo:Int(id:Int, name:String Var, guid:Int Var)
		Local n:Byte[256]
		Local ret:Int = FMOD_System_GetRecordDriverInfo(systemPtr, id, n, 256, Varptr guid)
		name = String.FromCString(n)
		Return ret
	End Method
	
	Rem
	bbdoc: Retrieves the number of recording devices available for this output mode.
	about: Use this to enumerate all recording devices possible so that the user can select one. 
	<p>Parameters: 
	<ul>
	<li><b>numDrivers</b> : A variable that receives the number of recording drivers available for this output mode.</li>
	</ul>
	</p>
	End Rem
	Method GetRecordNumDrivers:Int(numDrivers:Int Var)
		Return FMOD_System_GetRecordNumDrivers(systemPtr, Varptr numDrivers)
	End Method
	
	Rem
	bbdoc: Retrieves the current recording position of the record buffer in PCM samples.
	about: Parameters: 
	<ul>
	<li><b>id</b> : Enumerated driver ID. This must be in a valid range delimited by #getRecordNumDrivers. </li>
	<li><b>position</b> : A variable to receieve the current recording position in PCM samples.</li>
	</ul>
	End Rem
	Method GetRecordPosition:Int(id:Int, position:Int Var)
		Return FMOD_System_GetRecordPosition(systemPtr, id, Varptr position)
	End Method
	
	Rem
	bbdoc: Returns the current version of FMOD Ex being used.  
	End Rem
	Method GetVersion:Int(version:Int Var)
		Return FMOD_System_GetVersion(systemPtr, Varptr version)
	End Method
	
	Rem
	bbdoc: Retrieves a pointer to a block of PCM data that represents the currently playing audio mix.
	about: Parameters: 
	<ul>
	<li><b>waveArray</b> : An array of floats that receives the currently playing waveform data. This is an array of floating point values.</li>
	<li><b>numValues</b> : Number of floats to write to the array. Maximum value = 16384. </li>
	<li><b>channelOffset</b> : Offset into multichannel data. For mono output use 0. Stereo output will use 0 = left, 1 = right.
	More than stereo output - use the appropriate index. </li>
	</ul>
	<p>
	This method is useful for a very easy way to plot an oscilliscope.
	</p>
	<p>
	This is the actual resampled, filtered and volume scaled data of the final output, at the time this function is called.
	</p>
	<p>
	Do not use this method to try and display the whole waveform of the sound, as this is more of a 'snapshot' of the current
	waveform at the time it is called, and could return the same data if it is called very quickly in succession.<br>
	See the DSP API to capture a continual stream of wave data as it plays, or see Sound::lock / Sound::unlock if you want to simply
	display the waveform of a sound.
	</p>
	<p>
	This method allows retrieval of left and right data for a stereo sound individually. To combine them into one signal, simply add the
	entries of each seperate buffer together and then divide them by 2.
	</p>
	<p>
	Note: This method only displays data for sounds playing that were created with FMOD_SOFTWARE. FMOD_HARDWARE based sounds
	are played using the sound card driver and are not accessable.
	</p>
	End Rem
	Method GetWaveData:Int(waveArray:Float[], numValues:Int, channelOffset:Int)
		Return FMOD_System_GetWaveData(systemPtr, waveArray, numValues, channelOffset)
	End Method
	
	Rem
	bbdoc: Retrieves the state of the FMOD recording API, ie if it is currently recording or not. 
	about: Parameters: 
	<ul>
	<li><b>id</b> : Enumerated driver ID. This must be in a valid range delimited by #getRecordNumDrivers.</li>
	<li><b>recording</b> : A variable to receive the current recording state. True or non zero if the FMOD
	recording api is currently in the middle of recording, False or zero if the recording api is stopped / not recording.</li>
	</ul>
	<p>
	Recording can be started with #recordStart.	
	</p>
	End Rem
	Method IsRecording:Int(id:Int, recording:Int Var)
		Return FMOD_System_IsRecording(systemPtr, id:Int, Varptr recording)
	End Method
	
	Rem
	bbdoc: Starts the recording engine recording to the specified recording sound.
	about: Parameters: 
	<ul>
	<li><b>id</b> : Enumerated driver ID. This must be in a valid range delimited by #getRecordNumDrivers().</li>
	<li><b>sound</b> : User created sound for the user to record to.</li>
	<li><b>loop</b> : Boolean flag to tell the recording engine whether to continue recording to the provided sound from the start
	again, after it has reached the end. If this is set to true the data will be continually be overwritten once every loop.</li>
	</ul>
	End Rem
	Method RecordStart:Int(id:Int, sound:TFMODSound, loop:Int)
		Return FMOD_System_RecordStart(systemPtr, id, sound.soundPtr, loop)
	End Method
	
	Rem
	bbdoc: Stops the recording engine from recording to the specified recording sound.  
	End Rem
	Method RecordStop:Int(id:Int)
		Return FMOD_System_RecordStop(systemPtr, id)
	End Method
	
	Rem
	bbdoc: This updates the position, velocity and orientation of the specified 3D sound listener.  
	End Rem
	Method Set3DListenerAttributes:Int(listener:Int, pos:TFMODVector = Null, vel:TFMODVector = Null, forward:TFMODVector = Null, up:TFMODVector = Null)
		Return FMOD_System_Set3DListenerAttributes(systemPtr, listener, pos, vel, forward, up)
	End Method
	
	Rem
	bbdoc: Sets the number of 3D 'listeners' in the 3D sound scene.
	about: This method is useful mainly for split-screen game purposes.
	End Rem
	Method Set3DNumListeners:Int(numListeners:Int)
		Return FMOD_System_Set3DNumListeners(systemPtr, numListeners)
	End Method
	
	Rem
	bbdoc: Sets the global doppler scale, distance factor and log rolloff scale for all 3D sound in FMOD. 
	End Rem
	Method Set3DSettings:Int(dopplerScale:Float, distanceFactor:Float, rolloffScale:Float)
		Return FMOD_System_Set3DSettings(systemPtr, dopplerScale, distanceFactor, rolloffScale)
	End Method
	
	Rem
	bbdoc: Allows the user to specify the position of their actual physical speaker to account for non standard setups.
	about: It also allows the user to disable speakers from 3D consideration in a game.
	<p>
	The method is for describing the 'real world' speaker placement to provide a more natural panning solution for 3d sound. Graphical
	configuration screens in an application could draw icons for speaker placement that the user could position at their will.  
	</p>
	End Rem
	Method Set3DSpeakerPosition:Int(speaker:Int, x:Float, y:Float, active:Int)
		Return FMOD_System_Set3DSpeakerPosition(systemPtr, speaker, x, y, active)
	End Method
	
	'Method SetAdvancedSettings()
	'End Method
	
	Rem
	bbdoc: Sets the FMOD internal mixing buffer size.
	about: This method is used if you need to control mixer latency or granularity.  Smaller buffersizes lead to smaller latency,
	but can lead to stuttering/skipping/instable sound on slower machines or soundcards with bad drivers.  
	End Rem
	Method SetDSPBufferSize:Int(bufferLength:Int, numBuffers:Int)
		Return FMOD_System_SetDSPBufferSize(systemPtr, bufferLength, numBuffers)
	End Method
	
	Rem
	bbdoc: Selects a soundcard driver.
	about: Parameters: 
	<ul>
	<li><b>driver</b> : Driver number to select. 0 = primary or main sound device as selected by the operating
	system settings. Use #getNumDrivers to select a specific device.</li>
	</ul>
	<p>
	This method is used when an output mode has enumerated more than one output device, and you need to select between them. 
	</p>
	If this method is called after FMOD is already initialized with #init, the current driver will be shutdown and the
	newly selected driver will be initialized / started.
	</p>
	<p>
	When switching output driver after System::init there are a few considerations to make:
	</p>
	<p>
	All sounds must be created with FMOD_SOFTWARE, creating even one FMOD_HARDWARE sound will cause this function to return FMOD_ERR_NEEDSSOFTWARE.
	</p>
	<p>
	The driver that you wish to change to must support the current output format, sample rate, and number of
	channels. If it does not, FMOD_ERR_OUTPUT_INIT is returned and driver state is cleared. You
	should now call #setDriver with your original driver index to restore driver state
	(providing that driver is still available / connected) or make another selection.
	</p>
	End Rem
	Method SetDriver:Int(driver:Int)
		Return FMOD_System_SetDriver(systemPtr, driver)
	End Method
	
	Rem
	bbdoc: Sets the maximum world size for the geometry engine for performance / precision reasons.  
	about: Parameters: 
	<ul>
	<li><b>maxWorldSize</b> : Maximum size of the world from the centerpoint to the edge using the same units used in other 3D methods.</li>
	</ul>
	<p>
	Setting maxworldsize should be done first before creating any geometry.<br>
	It can be done any time afterwards but may be slow in this case.
	</p>
	<p>
	Objects or polygons outside the range of maxworldsize will not be handled efficiently.<br>
	Conversely, if maxworldsize is excessively large, the structure may loose precision and efficiency may drop.
	</p>
	End Rem
	Method SetGeometrySettings:Int(maxWorldSize:Float)
		Return FMOD_System_SetGeometrySettings(systemPtr, maxWorldSize)
	End Method
	
	Rem
	bbdoc: Allows the user to request a minimum number of hardware voices to be present on the soundcard to allow hardware 3D sound acceleration, or clamp the number of hardware 3D voices to a maximum value.
	End Rem
	Method SetHardwareChannels:Int(numHardwareChannels:Int)
		Return FMOD_System_SetHardwareChannels(systemPtr, numHardwareChannels)
	End Method
	
	Rem
	bbdoc: Sets a proxy server to use for all subsequent internet connections.  
	End Rem
	Method SetNetworkProxy:Int(proxy:String)
		Local s:Byte Ptr = proxy.ToCString()
		Local res:Int = FMOD_System_SetNetworkProxy(systemPtr, s)
		MemFree(s)
		Return res
	End Method
	
	Rem
	bbdoc: Sets the timeout for network streams.  
	End Rem
	Method SetNetworkTimeout:Int(timeout:Int)
		Return FMOD_System_SetNetworkTimeout(systemPtr, timeout)
	End Method
	
	Rem
	bbdoc: Selects the output mode for the platform.
	about: This is for selecting different OS specific API's which might have different features. 
	End Rem
	Method SetOutput:Int(output:Int)
		Return FMOD_System_SetOutput(systemPtr, output)
	End Method
	
	Rem
	bbdoc: Specify a base search path for plugins so they can be placed somewhere else than the directory of the main executable.  
	End Rem
	Method SetPluginPath:Int(path:String)
		Local s:Byte Ptr = path.ToCString()
		Local res:Int = FMOD_System_SetPluginPath(systemPtr, s)
		MemFree(s)
		Return res
	End Method
	
	'Rem
	'bbdoc: Selects a recording driver.
	'about: This Method is used when an output Mode has enumerated more than one record device, And you need To Select between them.
	'End Rem
	'Method SetRecordDriver:Int(driver:Int)
	'	Return FMOD_System_SetRecordDriver(systemPtr, driver)
	'End Method
	
	'Method SetReverbAmbientProperties:Int()
	'End Method
	
	'Method SetReverbProperties:Int()
	'End Method
	
	Rem
	bbdoc: Sets the maximum number of software mixed channels possible.
	about: Software mixed voices are used by sounds loaded with FMOD_SOFTWARE.
	End Rem
	Method SetSoftwareChannels:Int(numSoftwareChannels:Int)
		Return FMOD_System_SetSoftwareChannels(systemPtr, numSoftwareChannels)
	End Method
	
	Rem
	bbdoc: Sets the output format for the software mixer.
	about: This includes the bitdepth, sample rate and number of output channels.
	<p>
	Do not call this unless you explicity want to change something. Calling this could have adverse impact on the performance
	and panning behaviour. 
	</p>
	End Rem
	Method SetSoftwareFormat:Int(samplerate:Int, format:Int, numOutputChannels:Int, maxInputChannels:Int, resampleMethod:Int)
		Return FMOD_System_SetSoftwareFormat(systemPtr, samplerate, format, numOutputChannels, maxInputChannels, resampleMethod)
	End Method
	
	Rem
	bbdoc: Sets the speaker mode in the hardware and FMOD software mixing engine.  
	End Rem
	Method SetSpeakerMode:Int(Mode:Int)
		Return FMOD_System_SetSpeakerMode(systemPtr, Mode)
	End Method
	
	Rem
	bbdoc: Sets the internal buffersize for streams opened after this call.
	about: Larger values will consume more memory (see remarks), whereas smaller values may cause buffer under-run/starvation/stuttering
	caused by large delays in disk access (ie CDROM or netstream), or cpu usage in slow machines, or by trying to play too many
	streams at once.
	End Rem
	Method SetStreamBufferSize:Int(fileBufferSize:Int, fileBufferSizeType:Int)
		Return FMOD_System_SetStreamBufferSize(systemPtr, fileBufferSize, fileBufferSizeType)
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
	bbdoc: Retrieves a sound's default attributes for when it is played on a channel with System::playSound.  
	End Rem
	Method GetDefaults:Int(frequency:Float Var, volume:Float Var, pan:Float Var, priority:Int Var)
		Return FMOD_Sound_GetDefaults(soundPtr, Varptr frequency, Varptr volume, Varptr pan, Varptr priority)
	End Method

	Rem
	bbdoc: Sets a sounds's default attributes, so when it is played it uses these values without having to specify them later for each channel each time the sound is played.
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
	Method GetMode:Int(Mode:Int Var)
		Return FMOD_Sound_GetMode(soundPtr, Varptr Mode)
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
	Method SetMode:Int(Mode:Int)
		Return FMOD_Sound_SetMode(soundPtr, Mode)
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
	bbdoc: Retrieves the number of subsounds stored within a sound. 
	about: Parameters: 
	<ul>
	<li><b>numSubSounds</b> : A variable that receives the number of subsounds stored within this sound.</li>
	</ul>
	<p>
	A format that has subsounds is usually a container format, such as FSB, DLS, MOD, S3M, XM, IT.
	</p>
	End Rem
	Method GetNumSubSounds:Int(numSubSounds:Int Var)
		Return FMOD_Sound_GetNumSubSounds(soundPtr, Varptr numSubSounds)
	End Method
	
	Rem
	bbdoc: Retrieves the number of tags belonging to a sound.
	about: Parameters: 
	<ul>
	<li><b>numTags</b> : A variable that receives the number of tags in the sound.</li>
	<li><b>numTagsUpdated</b> : A variable that receives the number of tags updated since this method was last called.</li>
	</ul>
	<p>
	The 'numtagsupdated' parameter can be used to check if any tags have been updated since last calling this method.
	This can be useful to update tag fields, for example from internet based streams, such as shoutcast or icecast
	where the name of the song might change.
	</p>
	End Rem
	Method GetNumTags:Int(numTags:Int Var, numTagsUpdated:Int Var)
		Return FMOD_Sound_GetNumTags(soundPtr, Varptr numTags, Varptr numTagsUpdated)
	End Method
	
	Rem
	bbdoc: Retrieves the state a sound is in after FMOD_NONBLOCKING has been used to open it, or the state of the streaming buffer.  
	about: Parameters: 
	<ul>
	<li><b>openState</b> : A variable that receives the open state of a sound.</li>
	<li><b>percentBuffered</b> : A variable that receives the percentage of the file buffer filled progress of a stream.</li>
	<li><b>starving</b> : A variable that receives the starving state of a sound. If a stream has decoded more than
	the stream file buffer has ready for it, it will return TRUE. </li>
	<li><b>diskBusy</b> : A variable that receives the disk busy state of a sound. That is, whether the disk is currently being accessed for the sound.</li>
	</ul>
	<p>
	When a sound is opened with FMOD_NONBLOCKING, it is opened and prepared in the background, or asynchronously.
	</p>
	<p>
	This allows the main application to execute without stalling on audio loads.
	</p>
	<p>
	This method will describe the state of the asynchronous load routine i.e. whether it has succeeded, failed or is still in progress.
	</p>
	<p>
	If 'starving' is true, then you will most likely hear a stuttering/repeating sound as the decode buffer loops on itself and replays old
	data.
	Now that this variable exists, you can detect buffer underrun and use something like Channel::setMute to keep it quiet until it is not
	starving any more. 
	</p>
	End Rem
	Method GetOpenState:Int(openState:Int Var, percentBuffered:Int Var, starving:Int Var, diskBusy:Int Var)
		Return FMOD_Sound_GetOpenState(soundPtr, Varptr openState, Varptr percentBuffered, Varptr starving, Varptr diskBusy)
	End Method
	
	Rem
	bbdoc: Retrieves the name of a sound.
	returns: The name of the sound.
	about: If FMOD_LOWMEM has been specified in TFMODSystem.CreateSound, this method will return "(null)". 
	End Rem
	Method GetName:String()
		Local vstr:Byte[] = New Byte[256]
		Local ret:Int = FMOD_Sound_GetName(soundPtr, vstr, 256)
		Return String.FromCString(vstr)
	End Method
	
	Rem
	bbdoc: Retrieves the current loop count value for the specified sound. 
	about: Parameters: 
	<ul>
	<li><b>loopCount</b> : A variable that receives the number of times a sound will loop by default before stopping.
	0 = oneshot. 1 = loop once then stop. -1 = loop forever. Default = -1</li>
	</ul>
	<p>
	Unlike the channel loop count method, this method simply returns the value set with #setLoopCount. It
	does not decrement as it plays (especially seeing as one sound can be played multiple times).
	</p>
	End Rem
	Method GetLoopCount:Int(loopCount:Int Var)
		Return FMOD_Sound_GetLoopCount(soundPtr, Varptr loopCount)
	End Method
	
	Rem
	bbdoc: Sets a sound, by default, to loop a specified number of times before stopping if its mode is set to #FMOD_LOOP_NORMAL or #FMOD_LOOP_BIDI.
	about: Parameters: 
	<ul>
	<li><b>loopCount</b> : Number of times to loop before stopping. 0 = oneshot. 1 = loop once then stop. -1 = loop forever. Default = -1</li>
	</ul>
	<p>
	This method does not affect FMOD_HARDWARE based sounds that are not streamable.
	FMOD_SOFTWARE based sounds or any type of sound created with System::CreateStream or FMOD_CREATESTREAM will support this method.
	</p>
	<p>
	Issues with streamed audio. (Sounds created with with System::createStream or FMOD_CREATESTREAM). When changing the loop count,
	sounds created with System::createStream or FMOD_CREATESTREAM may already have been pre-buffered and executed their loop logic
	ahead of time, before this call was even made.
	This is dependent on the size of the sound versus the size of the stream decode buffer. See FMOD_CREATESOUNDEXINFO.
	If this happens, you may need to reflush the stream buffer. To do this, you can call Channel::setPosition which forces a reflush of
	the stream buffer.
	Note this will usually only happen if you have sounds or looppoints that are smaller than the stream decode buffer size. Otherwise you
	will not normally encounter any problems.
	</p>
	End Rem
	Method SetLoopCount:Int(loopCount:Int)
		Return FMOD_Sound_SetLoopCount(soundPtr, loopCount)
	End Method

	Rem
	bbdoc: Retrieves the loop points for a sound.
	about: Parameters: 
	<ul>
	<li><b>loopStart</b> : A variable to receive the loop start point. This point in time is played, so it is inclusive.</li>
	<li><b>loopStartType</b> : The time format used for the returned loop start point. One of #FMOD_TIMEUNIT_MS, #FMOD_TIMEUNIT_PCM,
	#FMOD_TIMEUNIT_PCMBYTES, #FMOD_TIMEUNIT_RAWBYTES, #FMOD_TIMEUNIT_MODORDER, #FMOD_TIMEUNIT_MODROW, #FMOD_TIMEUNIT_MODPATTERN,
	#FMOD_TIMEUNIT_SENTENCE_MS, #FMOD_TIMEUNIT_SENTENCE_PCM, #FMOD_TIMEUNIT_SENTENCE_PCMBYTES, #FMOD_TIMEUNIT_SENTENCE,
	#FMOD_TIMEUNIT_SENTENCE_SUBSOUND or #FMOD_TIMEUNIT_BUFFERED</li>
	<li><b>loopEnd</b> : A variable to receive the loop end point. This point in time is played, so it is inclusive.</li>
	<li><b>loopEndType</b> : The time format used for the returned loop end point. One of #FMOD_TIMEUNIT_MS, #FMOD_TIMEUNIT_PCM,
	#FMOD_TIMEUNIT_PCMBYTES, #FMOD_TIMEUNIT_RAWBYTES, #FMOD_TIMEUNIT_MODORDER, #FMOD_TIMEUNIT_MODROW, #FMOD_TIMEUNIT_MODPATTERN,
	#FMOD_TIMEUNIT_SENTENCE_MS, #FMOD_TIMEUNIT_SENTENCE_PCM, #FMOD_TIMEUNIT_SENTENCE_PCMBYTES, #FMOD_TIMEUNIT_SENTENCE,
	#FMOD_TIMEUNIT_SENTENCE_SUBSOUND or #FMOD_TIMEUNIT_BUFFERED</li>
	</ul>
	End Rem
	Method GetLoopPoints:Int(loopStart:Int Var, loopStartType:Int Var, loopEnd:Int Var, loopEndType:Int Var)
		Return FMOD_Sound_GetLoopPoints(soundPtr, Varptr loopStart, Varptr loopStartType, Varptr loopEnd, Varptr loopEndType)
	End Method
		
	Rem
	bbdoc: Sets the loop points within a sound.
	about: Parameters: 
	<ul>
	<li><b>loopStart</b> : The loop start point. This point in time is played, so it is inclusive. </li>
	<li><b>loopStartType</b> : The time format used for the loop start point. One of #FMOD_TIMEUNIT_MS, #FMOD_TIMEUNIT_PCM,
	#FMOD_TIMEUNIT_PCMBYTES, #FMOD_TIMEUNIT_RAWBYTES, #FMOD_TIMEUNIT_MODORDER, #FMOD_TIMEUNIT_MODROW, #FMOD_TIMEUNIT_MODPATTERN,
	#FMOD_TIMEUNIT_SENTENCE_MS, #FMOD_TIMEUNIT_SENTENCE_PCM, #FMOD_TIMEUNIT_SENTENCE_PCMBYTES, #FMOD_TIMEUNIT_SENTENCE,
	#FMOD_TIMEUNIT_SENTENCE_SUBSOUND or #FMOD_TIMEUNIT_BUFFERED</li>
	<li><b>loopEnd</b> : The loop end point. This point in time is played, so it is inclusive. </li>
	<li><b>loopEndType</b> : The time format used for the loop end point. One of #FMOD_TIMEUNIT_MS, #FMOD_TIMEUNIT_PCM,
	#FMOD_TIMEUNIT_PCMBYTES, #FMOD_TIMEUNIT_RAWBYTES, #FMOD_TIMEUNIT_MODORDER, #FMOD_TIMEUNIT_MODROW, #FMOD_TIMEUNIT_MODPATTERN,
	#FMOD_TIMEUNIT_SENTENCE_MS, #FMOD_TIMEUNIT_SENTENCE_PCM, #FMOD_TIMEUNIT_SENTENCE_PCMBYTES, #FMOD_TIMEUNIT_SENTENCE,
	#FMOD_TIMEUNIT_SENTENCE_SUBSOUND or #FMOD_TIMEUNIT_BUFFERED</li>
	</ul>
	<p>
	Not supported by static sounds created with FMOD_HARDWARE.
	Supported by sounds created with FMOD_SOFTWARE, or sounds of any type (hardware or software) created with System::createStream or FMOD_CREATESTREAM. 
	If a sound was 44100 samples long and you wanted to loop the whole sound, loopstart would be 0, and loopend would be 44099, 
	not 44100. You wouldn't use milliseconds in this case because they are not sample accurate.
	If loop end is smaller or equal to loop start, it will result in an error.
	If loop start or loop end is larger than the length of the sound, it will result in an error.
	</p>
	<p>
	Issues with streamed audio. (Sounds created with with System::createStream or FMOD_CREATESTREAM). 
	When changing the loop points, sounds created with System::createStream or FMOD_CREATESTREAM may already have been pre-buffered
	and executed their loop logic ahead of time, before this call was even made.
	This is dependant on the size of the sound versus the size of the stream decode buffer. See FMOD_CREATESOUNDEXINFO.
	If this happens, you may need to reflush the stream buffer. To do this, you can call Channel::setPosition which forces a
	reflush of the stream buffer.
	Note this will usually only happen if you have sounds or looppoints that are smaller than the stream decode buffer size.
	Otherwise you will not normally encounter any problems.
	</p>
	End Rem
	Method SetLoopPoints:Int(loopStart:Int, loopStartType:Int, loopEnd:Int, loopEndType:Int)
		Return FMOD_Sound_SetLoopPoints(soundPtr, loopStart, loopStartType, loopEnd, loopEndType)
	End Method
	
	Rem
	bbdoc: Retrieves the inside and outside angles of the sound projection cone.
	End Rem
	Method Get3DConeSettings:Int(insideConeAngle:Float Var, outsideConeAngle:Float Var, outsideVolume:Float Var)
		Return FMOD_Sound_Get3DConeSettings(soundPtr, Varptr insideConeAngle, Varptr outsideConeAngle, Varptr outsideVolume)
	End Method
	
	Rem
	bbdoc: Sets the inside and outside angles of the sound projection cone, as well as the volume of the sound outside the outside angle of the sound projection cone.  
	End Rem
	Method Set3DConeSettings:Int(insideConeAngle:Float, outsideConeAngle:Float, outsideVolume:Float)
		Return FMOD_Sound_Set3DConeSettings(soundPtr, insideConeAngle, outsideConeAngle, outsideVolume)
	End Method
	
	Rem
	bbdoc: Retrieve the minimum and maximum audible distance for a sound.
	End Rem
	Method Get3DMinMaxDistance:Int(minDist:Float Var, maxDist:Float Var)
		Return FMOD_Sound_Get3DMinMaxDistance(soundPtr, Varptr minDist, Varptr maxDist)
	End Method
	
	Rem
	bbdoc: Sets the minimum and maximum audible distance for a sound.
	about:  MinDistance is the minimum distance that the sound emitter will cease to continue growing louder at (as it approaches the listener).
	<p>
	Within the mindistance it stays at the constant loudest volume possible. Outside of this mindistance it begins to attenuate.
	</p>
	<p>
  	MaxDistance is the distance a sound stops attenuating at. Beyond this point it will stay at the volume it would be at maxdistance
	units from the listener and will not attenuate any more.
	</p>
	<p>
  	MinDistance is useful to give the impression that the sound is loud or soft in 3d space. An example of this is a small quiet object,
	such as a bumblebee, which you could set a mindistance of to 0.1 for example, which would cause it to attenuate quickly and dissapear when
	only a few meters away from the listener.
	</p>
	<p>
  	Another example is a jumbo jet, which you could set to a mindistance of 100.0, which would keep the sound volume at max until the
	listener was 100 meters away, then it would be hundreds of meters more before it would fade out.
	</p>
	<p>
  	In summary, increase the mindistance of a sound to make it 'louder' in a 3d world, and decrease it to make it 'quieter' in a 3d world.
  	Maxdistance is effectively obsolete unless you need the sound to stop fading out at a certain point. Do not adjust this from the default
	if you dont need to.
  	Some people have the confusion that maxdistance is the point the sound will fade out to, this is not the case.
	</p>
	End Rem
	Method Set3DMinMaxDistance:Int(minDist:Float, maxDist:Float)
		Return FMOD_Sound_Set3DMinMaxDistance(soundPtr, minDist, maxDist)
	End Method
	
	Rem
	bbdoc: Gets the number of music channels inside a MOD/S3M/XM/IT/MIDI file.  
	End Rem
	Method GetMusicNumChannels:Int(numChannels:Int Var)
		Return FMOD_Sound_GetMusicNumChannels:Int(soundPtr, Varptr numChannels)
	End Method
	
	Rem
	bbdoc: Retrieves the volume of a MOD/S3M/XM/IT/MIDI music channel volume.
	End Rem
	Method GetMusicChannelVolume:Int(channel:Int, volume:Int Var)
		Return FMOD_Sound_GetMusicChannelVolume(soundPtr, channel, Varptr volume)
	End Method
	
	Rem
	bbdoc: Sets the volume of a MOD/S3M/XM/IT/MIDI music channel volume.  
	End Rem
	Method SetMusicChannelVolume:Int(channel:Int, volume:Int)
		Return FMOD_Sound_SetMusicChannelVolume(soundPtr, channel, volume)
	End Method
	
	Rem
	bbdoc: Retrieves a descriptive tag stored by the sound, to describe things like the song name, author etc.
	End Rem
	Method GetTag:TFMODTag(name:String, index:Int)
		Local tag:TFMODTag
		If name Then
			Local s:Byte Ptr = name.ToCString()
			tag = TFMODTag._create(bmx_FMOD_Sound_GetTag(soundPtr, s, index))
			MemFree(s)
		Else
			tag = TFMODTag._create(bmx_FMOD_Sound_GetTag(soundPtr, Null, index))
		End If
		Return tag
	End Method
	
	Rem
	bbdoc: Retrieves the current playback behaviour variations of a sound.  
	End Rem
	Method GetVariations:Int(frequencyVar:Float Var, volumeVar:Float Var, panVar:Float Var)
		Return FMOD_Sound_GetVariations(soundPtr, Varptr frequencyVar, Varptr volumeVar, Varptr panVar)
	End Method
	
	Rem
	bbdoc: Changes the playback behaviour of a sound by allowing random variations to playback parameters to be set.  
	End Rem
	Method SetVariations:Int(frequencyVar:Float, volumeVar:Float, panVar:Float)
		Return FMOD_Sound_SetVariations(soundPtr, frequencyVar, volumeVar, panVar)
	End Method
	
	Rem
	bbdoc: Assigns a sound as a 'subsound' of another sound. A sound can contain other sounds.
	about: The sound object that is issuing the command will be the 'parent' sound.  
	End Rem
	Method SetSubSound:Int(index:Int, subsound:TFMODSound)
		Return FMOD_Sound_SetSubSound(soundPtr, index, subsound.soundPtr)
	End Method
	
	Rem
	bbdoc: For any sound that has subsounds, this function will determine the order of playback of these subsounds, and it will play / stitch together the subsounds without gaps.
	about: This is a very useful feature for those users wanting to do seamless / gapless stream playback. (ie sports commentary,
	gapless playback media players etc).  
	End Rem
	Method SetSubSoundSentence:Int(soundList:Int[])
		Return bmx_FMOD_Sound_SetSubSoundSentence(soundPtr, soundList)
	End Method
	
	Rem
	bbdoc: Retrieves a handle to a Sound object that is contained within the parent sound.  
	End Rem
	Method GetSubSound:TFMODSound(index:Int)
		Return TFMODSound._create(bmx_FMOD_Sound_GetSubSound(soundPtr, index))
	End Method
	
	Rem
	bbdoc: Frees a sound object.  
	End Rem
	Method SoundRelease:Int()
		If soundPtr Then
			Local res:Int = FMOD_Sound_Release(soundPtr)
			soundPtr = Null
			Return res
		End If
	End Method
	
	Method GetUserData:Object()
		Return bmx_FMOD_Sound_GetUserData(soundPtr)
	End Method
	
End Type

Rem
bbdoc: An audio channel.
End Rem
Type TFMODChannel

	' NOTE : Remember Channel uses a custom class, so all calls NEED to be filtered through
	' the glue code ! ! ! !

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
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method Get3DAttributes:Int(pos:TFMODVector, vel:TFMODVector)
		Return bmx_FMOD_Channel_Get3DAttributes(channelPtr, Varptr pos, Varptr vel)
	End Method

	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DConeOrientation:Int(orientation:TFMODVector)
		Return bmx_FMOD_Channel_Get3DConeOrientation(channelPtr, Varptr orientation)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DConeSettings:Int(insideConeAngle:Float Var, outsideConeAngle:Float Var, outsideVolume:Float Var)
		Return bmx_FMOD_Channel_Get3DConeSettings(channelPtr, Varptr insideConeAngle, Varptr outsideConeAngle, Varptr outsideVolume)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DCustomRollOff:Int(points:TFMODVector[])
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DDopplerLevel:Int(level:Float Var)
		Return bmx_FMOD_Channel_Get3DDopplerLevel(channelPtr, Varptr level)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DMinMaxDistance:Int(minDistance:Float Var, maxDistance:Float Var)
		Return bmx_FMOD_Channel_Get3DMinMaxDistance(channelPtr, Varptr minDistance, Varptr maxDistance)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DOcclusion:Int(directOcclusion:Float Var, reverbOcclusion:Float Var)
		Return bmx_FMOD_Channel_Get3DOcclusion(channelPtr, Varptr directOcclusion, Varptr reverbOcclusion)
	End Method
	
	Rem
	bbdoc: Retrieves the current 3D mix level for the channel set by TFMODChannel.Set3DPanLevel. 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DPanLevel:Int(level:Float Var)
		Return bmx_FMOD_Channel_Get3DPanLevel(channelPtr, Varptr level)
	End Method
	
	Rem
	bbdoc: Retrieves the stereo (and above) spread angle specified by TFMODChannel.Set3DSpread.  
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem	
	Method Get3DSpread:Int(angle:Float Var)
		Return bmx_FMOD_Channel_Get3DSpread(channelPtr, Varptr angle)
	End Method
	
	
	Rem
	bbdoc: Returns the playing state for the current channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>playing</b> : Variable that receives the current channel's playing status.
	true = the channel is currently playing a sound. false = the channel is not playing a sound.  </li>
	</ul>
	End Rem
	Method IsPlaying:Int(playing:Int Var)
		Return bmx_FMOD_Channel_IsPlaying(channelPtr, Varptr playing)
	End Method
	
	Rem
	bbdoc: Retrieves the paused state of the channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>paused</b> : Variable that receives the current paused state. True = the sound is
	paused. False = the sound is not paused.  </li>
	</ul>
	End Rem
	Method GetPaused:Int(paused:Int Var)
		Return bmx_FMOD_Channel_GetPaused(channelPtr, Varptr paused)
	End Method
	
	Rem
	bbdoc: Stops the channel from playing.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Makes it available for re-use by the priority system. 
	End Rem
	Method Stop:Int()
		Return bmx_FMOD_Channel_Stop:Int(channelPtr)
	End Method
	
	Rem
	bbdoc: Retrieves the volume level for the channel. 
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>volume</b> : Variable to receive the channel volume level, from 0.0 to 1.0 inclusive.
	0.0 = silent, 1.0 = full volume. Default = 1.0.</li>
	</ul>
	End Rem
	Method GetVolume:Int(volume:Float Var)
		Return bmx_FMOD_Channel_GetVolume(channelPtr, Varptr volume)
	End Method

	Rem
	bbdoc: Sets the volume for the channel linearly.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>volume</b> : A linear volume level, from 0.0 to 1.0 inclusive. 0.0 = silent,
	1.0 = full volume. Default = 1.0. </li>
	</ul>
	<p>
	When a sound is played, it plays at the default volume of the sound which can be set by
	TFMODSound.SetDefaults.
	</p>
	<p>
	For most file formats, the volume is determined by the audio format. 
	</p>
	End Rem
	Method SetVolume:Int(volume:Float)
		Return bmx_FMOD_Channel_SetVolume(channelPtr, volume)
	End Method
	
	Rem
	bbdoc: Returns the frequency in HZ of the channel.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>frequency</b> : Variable that receives the current frequency of the channel in HZ.  </li>
	</ul>
	End Rem
	Method GetFrequency:Int(frequency:Float Var)
		Return bmx_FMOD_Channel_GetFrequency(channelPtr, Varptr frequency)
	End Method

	Rem
	bbdoc: Sets the channel's frequency or playback rate, in HZ.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>frequency</b> : A frequency value in HZ. This value can also be negative to play the
	sound backwards (negative frequencies allowed with FMOD_SOFTWARE based non-stream sounds
	only). DirectSound hardware voices have limited frequency range on some soundcards.</li>
	</ul>
	<p>
	When a sound is played, it plays at the default frequency of the sound which can be set by
	TFMODSound.SetDefaults.
	</p>
	<p>
	For most file formats, the volume is determined by the audio format.
	</p>
	<h3>Frequency limitations for sounds created with FMOD_HARDWARE in DirectSound.</h3>
	<p>
	Every hardware device has a minimum and maximum frequency. This means setting the frequency
	above the maximum and below the minimum will have no effect.
	</p>
	<p>
	FMOD clamps frequencies to these values when playing back on hardware, so if you are
	setting the frequency outside of this range, the frequency will stay at either the minimum
	or maximum.
	</p>
	<p>
	Note that FMOD_SOFTWARE based sounds do not have this limitation.
	</p>
	<p>
	To find out the minimum and maximum value before initializing FMOD (maybe to decide whether to use a
	different soundcard, output mode, or drop back fully to software mixing), you can use the
	TFMODSystem.GetDriverCaps method. 
	</p>
	End Rem
	Method SetFrequency:Int(frequency:Float)
		Return bmx_FMOD_Channel_SetFrequency(channelPtr, frequency)
	End Method
	
	Rem
	bbdoc: Retrieves the internal channel index for a channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>index</b> : Variable to receive the channel index. This will be from 0 to the value specified in
	TFMODSystem.Init minus 1.</li>
	</ul>
	<p>
	Note that working with channel indicies directly is not recommended. It is recommended that you use
	FMOD_CHANNEL_FREE for the index in TFMODSystem.PlaySound to use FMOD's channel manager. 
	</p>
	End Rem
	Method GetIndex:Int(index:Int Var)
		Return bmx_FMOD_Channel_GetIndex(channelPtr, Varptr index)
	End Method
	
	Rem
	bbdoc: Retrieves the current loop count for the specified channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>loopCount</b> : Variable that receives the number of times a channel will loop before stopping.
	0 = oneshot. 1 = loop once then stop. -1 = loop forever. Default = -1  </li>
	</ul>
	<p>
	This function retrieves the <b>current</b> loop countdown value for the channel being played.
	</p>
	<p>
	This means it will decrement until reaching 0, as it plays. To reset the value, use TFMODChannel.SetLoopCount.
	</p>
	End Rem
	Method GetLoopCount:Int(loopCount:Int Var)
		Return bmx_FMOD_Channel_GetLoopCount(channelPtr, Varptr loopCount)
	End Method
	
	Rem
	bbdoc: Retrieves the loop points for a channel. 
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>loopStart</b> : Variable to receive the loop start point. This point in time is played, so it is inclusive.</li>
	<li><b>loopStartType</b> : The time format used for the returned loop start point. See FMOD_TIMEUNIT.</li>
	<li><b>loopEnd</b> : Variable to receive the loop end point. This point in time is played, so it is inclusive.</li>
	<li><b>loopEndType</b> : The time format used for the returned loop end point. See FMOD_TIMEUNIT.  </li>
	</ul>
	End Rem
	Method GetLoopPoints:Int(loopStart:Int Var, loopStartType:Int, loopEnd:Int Var, loopEndType:Int)
		Return bmx_FMOD_Channel_GetLoopPoints(channelPtr, Varptr loopStart, loopStartType, Varptr loopEnd, loopEndType)
	End Method
	
	Rem
	bbdoc: Returns the current PCM offset or playback position for the specified channel.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>position</b> : Variable that receives the position of the sound. </li>
	<li><b>posType</b> : Time unit to retrieve into the position parameter. See FMOD_TIMEUNIT.  </li>
	</ul>
	<p>
	Certain timeunits do not work depending on the file format. For example FMOD_TIMEUNIT_MODORDER will not
	work with an mp3 file.
	</p>
	<p>
	A PCM sample is a unit of measurement in audio that contains the data for one audible element of sound.
	1 sample might be 16bit stereo, so 1 sample contains 4 bytes. 44,100 samples of a 44khz sound would
	represent 1 second of data. 
	</p>
	End Rem
	Method GetPosition:Int(position:Int Var, posType:Int)
		Return bmx_FMOD_Channel_GetPosition(channelPtr, Varptr position, posType)
	End Method
	
	Rem
	bbdoc: Returns the current channel's status of whether it is virtual (emulated) or not due to FMOD Ex's virtual channel management system.  
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>virtual</b> : variable that receives the current channel's virtual status. true = the channel is
	inaudible and currently being emulated at no cpu cost. false = the channel is a real hardware or software
	voice and should be audible. </li>
	</ul>
	<p>
	Virtual channels are not audible, because there are no more real hardware or software channels available.
	</p>
	<p>
	If you are plotting virtual voices vs real voices graphically, and wondering why FMOD sometimes chooses
	seemingly random channels to be virtual that are usually far away, that is because they are probably
	silent. It doesn't matter which are virtual and which are not if they are silent. Virtual voices are not
	calculation on 'closest to listener' calculation, they are based on audibility. See the tutorial in the
	FMOD Ex documentation for more information on virtual channels. 
	</p>
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
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method SetPaused:Int(paused:Int)
		Return bmx_FMOD_Channel_SetPaused(channelPtr, paused)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method GetPan:Int(pan:Float Var)
		Return bmx_FMOD_Channel_GetPan(channelPtr, Varptr pan)
	End Method
	
	Rem
	bbdoc: 
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method SetPan:Int(pan:Float)
		Return bmx_FMOD_Channel_SetPan(channelPtr, pan)
	End Method
	
	Rem
	bbdoc: Sets a channel to belong to a specified channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: A channelgroup can contain many channels.
	End Rem
	Method SetChannelGroup:Int(group:TFMODChannelGroup)
		Return bmx_FMOD_Channel_SetChannelGroup(channelPtr, group.channelGroupPtr)
	End Method
	
	Rem
	bbdoc: Retrieves the currently assigned channel group for the channel.
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method GetChannelGroup:TFMODChannelGroup()
		Return TFMODChannelGroup._create(bmx_FMOD_Channel_GetChannelGroup(channelPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetInputChannelMix:Int(levels:Float[])
	' TODO
	End Method
	
	Rem
	bbdoc: Retrieves the current mode bit flags for the current channel.  
	End Rem
	Method GetMode:Int(Mode:Int Var)
		Return bmx_FMOD_Channel_GetMode(channelPtr, Varptr Mode)
	End Method
	
	Rem
	bbdoc: Returns the current mute status of the channel. 
	End Rem
	Method GetMute:Int(mute:Int Var)
		Return bmx_FMOD_Channel_GetMute(channelPtr, Varptr mute)
	End Method
	
	Rem
	bbdoc: Retrieves the current priority for this channel.
	End Rem
	Method GetPriority:Int(priority:Int Var)
		Return bmx_FMOD_Channel_GetPriority(channelPtr, Varptr priority)
	End Method
	
	Rem
	bbdoc: Sets the priority for a channel after it has been played.
	about: A sound with a higher priority than another sound will not be stolen or made virtual by that sound. 
	End Rem
	Method SetPriority:Int(priority:Int)
		Return bmx_FMOD_Channel_SetPriority(channelPtr, priority)
	End Method
	
	Rem
	bbdoc: Retrieves the current reverb properties for this channel.
	End Rem
	Method GetReverbProperties:Int(properties:TFMODReverbChannelProperties)
		Return bmx_FMOD_Channel_GetReverbProperties(channelPtr, Varptr properties)
	End Method

	Rem
	bbdoc: Sets the channel specific reverb properties, including things like wet/dry mix.  
	End Rem
	Method SetReverbProperties:Int(properties:TFMODReverbChannelProperties)
		Return bmx_FMOD_Channel_GetReverbProperties(channelPtr, Varptr properties)
	End Method
	
	Rem
	bbdoc: Retrieves the current level settings from TFMODChannel::SetSpeakerLevels
	about: @levels should be a pre-created array, which will be populated by this method.
	<p>
	This method does not return level values reflecting TFMODChannel::SetPan or TFMODChannel::SetVolume.
	</p>
	End Rem
	Method GetSpeakerLevels:Int(speaker:Int, levels:Float[])
		Return bmx_FMOD_Channel_GetSpeakerLevels(channelPtr, speaker, levels)
	End Method	
	
	Rem
	bbdoc: Gets the channel's speaker volume levels for each speaker individually.
	End Rem
	Method GetSpeakerMix:Int(frontleft:Float Var, frontright:Float Var, center:Float Var, ..
			lfe:Float Var, backleft:Float Var, backright:Float Var, sideleft:Float Var, sideright:Float Var)
		Return bmx_FMOD_Channel_GetSpeakerMix(channelPtr, Varptr frontleft, Varptr frontright, Varptr center, ..
			Varptr lfe, Varptr backleft, Varptr backright, Varptr sideleft, Varptr sideright)
	End Method

	Rem
	bbdoc: Retrieves the gain of the dry signal when lowpass filtering is applied.
	End Rem	
	Method GetLowPassGain:Int(gain:Float Var)
		Return bmx_FMOD_Channel_GetLowPassGain(channelPtr, Varptr gain)
	End Method
	
	Rem
	bbdoc: Retrieves a block of PCM data that represents the currently playing waveform on this channel.
	about: This method is useful for a very easy way to plot an oscilliscope.
	<p>
	This is the actual resampled pcm data window at the time the method is called.
	</p>
	<p>
	Do not use this method to try and display the whole waveform of the sound, as this is more of a 'snapshot' of the current waveform
	at the time it is called, and could return the same data if it is called very quickly in succession.
	</p>
	<p>
	See the DSP API to capture a continual stream of wave data as it plays, or see Sound::lock / Sound::unlock if you want to simply
	display the waveform of a sound.
	</p>
	<p>
	This method allows retrieval of left and right data for a stereo sound individually. To combine them into one signal, simply add
	the entries of each seperate buffer together and then divide them by 2. 
	</p>
	<p>
	Note: This method only displays data for sounds playing that were created with FMOD_SOFTWARE. FMOD_HARDWARE based sounds are played
	using the sound card driver and are not accessable.
	</p>
	End Rem
	Method GetWaveData:Int(waveArray:Float[], channelOffset:Int)
		Return bmx_FMOD_Channel_GetWaveData(channelPtr, waveArray, channelOffset)
	End Method
	
	Rem
	bbdoc: Sets an end delay for a sound (so that dsp can continue to process the finished sound).
	about: Set the start of the sound according to the global DSP clock value which represents the time in the mixer timeline. 
	End Rem
	Method SetDelay:Int(delayType:Int, delayHi:Int, delayLo:Int)
		Return bmx_FMOD_Channel_SetDelay(channelPtr, delayType, delayHi, delayLo)
	End Method
	
	Rem
	bbdoc: Sets the incoming levels in a sound.
	about: This means if you have a multichannel sound you can turn channels on and off.
	<p>
	A mono sound has 1 input channel, a stereo has 2, etc. It depends on what type of sound is playing on the channel at the time. 
	</p>
	End Rem
	Method SetInputChannelMix:Int(levels:Float[])
		Return bmx_FMOD_Channel_SetInputChannelMix(channelPtr, levels)
	End Method
	
	Rem
	bbdoc: Sets a channel to loop a specified number of times before stopping. 
	End Rem
	Method SetLoopCount:Int(loopCount:Int)
		Return bmx_FMOD_Channel_SetLoopCount(channelPtr, loopCount)
	End Method

	Rem
	bbdoc: Sets the loop points within a channel.
	End Rem
	Method SetLoopPoints:Int(loopStart:Int, loopStartType:Int, loopEnd:Int, loopEndType:Int)
		Return bmx_FMOD_Channel_SetLoopPoints(channelPtr, loopStart, loopStartType, loopEnd, loopEndType)
	End Method

	Rem
	bbdoc: Sets the gain of the dry signal when lowpass filtering is applied. 
	End Rem
	Method SetLowPassGain:Int(gain:Float)
		Return bmx_FMOD_Channel_SetLowPassGain(channelPtr, gain)
	End Method

	Rem
	bbdoc: Changes some attributes for a channel based on the mode passed in. 
	End Rem
	Method SetMode:Int(Mode:Int)
		Return bmx_FMOD_Channel_SetMode(channelPtr, Mode)
	End Method

	Rem
	bbdoc: Mutes / un-mutes a channel, effectively silencing it or returning it to its normal volume. 
	End Rem
	Method SetMute:Int(mute:Int)
		Return bmx_FMOD_Channel_SetMute(channelPtr, mute)
	End Method

	Rem
	bbdoc: Sets the current playback position for the currently playing sound to the specified PCM offset. 
	End Rem
	Method SetPosition:Int(position:Int, postype:Int)
		Return bmx_FMOD_Channel_SetPosition(channelPtr, position, postype)
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
	
	Rem
	bbdoc: 
	End Rem
	Method SetPCMReadCallback(callback:Int(sound:TFMODSound, data:Byte Ptr, dataLen:Int))
		Local obj:TFMODCreateSoundCallbackHandler = _getorcreateuserdata()
		obj.pcmReadCallback = callback
		bmx_soundexinfo_setpcmreadcallback(soundExInfoPtr, TFMODCreateSoundCallbackHandler._PCMReadCallback)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetPCMSetPosCallback(callback:Int(sound:TFMODSound, subsound:Int, position:Int, posType:Int))
		Local obj:TFMODCreateSoundCallbackHandler = _getorcreateuserdata()
		obj.pcmSetPosCallback = callback
		bmx_soundexinfo_setpcmsetposcallback(soundExInfoPtr, TFMODCreateSoundCallbackHandler._PCMSetPosCallback)
	End Method
	
	Method _getorcreateuserdata:TFMODCreateSoundCallbackHandler()
		Local obj:TFMODCreateSoundCallbackHandler = TFMODCreateSoundCallbackHandler(bmx_soundexinfo_getuserdata(soundExInfoPtr))
		If Not obj Then
			obj = New TFMODCreateSoundCallbackHandler
			bmx_soundexinfo_setuserdata(soundExInfoPtr, obj)
		End If
		Return obj
	End Method

	Method Delete()
		If soundExInfoPtr Then
			bmx_soundexinfo_delete(soundExInfoPtr)
			soundExInfoPtr = Null
		End If
	End Method	
	
End Type

Type TFMODCreateSoundCallbackHandler

	Field pcmReadCallback:Int(sound:TFMODSound, data:Byte Ptr, dataLen:Int)
	Field pcmSetPosCallback:Int(sound:TFMODSound, subsound:Int, position:Int, posType:Int)
	Field nonBlockCallback:Int(sound:TFMODSound, result:Int)
	
	Function _PCMReadCallback:Int(sound:Byte Ptr, data:Byte Ptr, dataLen:Int)
		Local snd:TFMODSound = TFMODSound._create(sound)
		Local obj:TFMODCreateSoundCallbackHandler = TFMODCreateSoundCallbackHandler(snd.GetUserData())
		If obj Then
			Return obj.pcmReadCallback(snd, data, dataLen)
		End If
		Return FMOD_OK
	End Function

	Function _PCMSetPosCallback:Int(sound:Byte Ptr, subsound:Int, position:Int, posType:Int)
		Local snd:TFMODSound = TFMODSound._create(sound)
		Local obj:TFMODCreateSoundCallbackHandler = TFMODCreateSoundCallbackHandler(snd.GetUserData())
		If obj Then
			Return obj.pcmSetPosCallback(snd, subsound, position, posType)
		End If
		Return FMOD_OK
	End Function

	Function _NonBlockCallback:Int(sound:Byte Ptr, result:Int)
		Local snd:TFMODSound = TFMODSound._create(sound)
		Local obj:TFMODCreateSoundCallbackHandler = TFMODCreateSoundCallbackHandler(snd.GetUserData())
		If obj Then
			Return obj.nonBlockCallback(snd, result)
		End If
		Return FMOD_OK
	End Function

End Type


Rem
bbdoc: Structure describing a piece of tag data.
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
	bbdoc: The type of this tag. 
	End Rem
	Method GetTagType:Int()
		Return bmx_fmodtag_gettagtype(tagPtr)
	End Method
	
	Rem
	bbdoc: The type of data that this tag contains.
	End Rem
	Method GetDataType:Int()
		Return bmx_fmodtag_getdatatype(tagPtr)
	End Method
	
	Rem
	bbdoc: The name of this tag i.e. "TITLE", "ARTIST" etc.
	End Rem
	Method GetName:String()
		Return String.FromCString(bmx_fmodtag_getname(tagPtr))
	End Method
	
	Rem
	bbdoc: Length of the data contained in this tag.
	End Rem
	Method GetDataLength:Int()
		Return bmx_fmodtag_getdatalength(tagPtr)
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
	
	Rem
	bbdoc: True if this tag has been updated since last being accessed with TFMODSound.GetTag
	End Rem
	Method GetUpdated:Int()
		Return bmx_fmodtag_getupdated(tagPtr)
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
		Local ret:Int = FMOD_DSP_GetParameter(dspPtr, index, Varptr value, vstr, 16)
		Return String.FromCString(vstr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetInfo:String(version:Int Var, channels:Int Var)
		Local vstr:Byte[] = New Byte[32]
		Local ret:Int = FMOD_DSP_GetInfo(dspPtr, vstr, Varptr version, Varptr channels, Null, Null)
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
		Local ret:Int = FMOD_SoundGroup_GetName(soundGroupPtr, vstr, 256)
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

Rem
bbdoc: 
End Rem
Type TFMODReverb

	Field reverbPtr:Byte Ptr
	
	Function _create:TFMODReverb(reverbPtr:Byte Ptr)
		If reverbPtr Then
			Local this:TFMODReverb = New TFMODReverb
			this.reverbPtr = reverbPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetActive:Int(active:Int Var)
		Return FMOD_Reverb_GetActive(reverbPtr, Varptr active)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetActive:Int(active:Int)
		Return FMOD_Reverb_SetActive(reverbPtr, active)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ReverbRelease:Int()
		If reverbPtr Then
			Return FMOD_Reverb_Release(reverbPtr)
			reverbPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Get3DAttributes:Int(position:TFMODVector, minDistance:Float Var, maxDistance:Float Var)
		Return FMOD_Reverb_Get3DAttributes(reverbPtr, Varptr position, Varptr minDistance, Varptr maxDistance)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Set3DAttributes:Int(position:TFMODVector, minDistance:Float, maxDistance:Float)
		Return FMOD_Reverb_Set3DAttributes(reverbPtr, Varptr position, minDistance, maxDistance)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetProperties:Int(properties:TFMODReverbProperties)
		Return FMOD_Reverb_GetProperties(reverbPtr, Varptr properties)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetProperties:Int(properties:TFMODReverbProperties)
		Return FMOD_Reverb_SetProperties(reverbPtr, Varptr properties)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFMODChannelGroup

	Field channelGroupPtr:Byte Ptr
	
	Function _create:TFMODChannelGroup(channelGroupPtr:Byte Ptr)
		If channelGroupPtr Then
			Local this:TFMODChannelGroup = New TFMODChannelGroup
			this.channelGroupPtr = channelGroupPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Adds a DSP effect to this channelgroup, affecting all channels that belong to it.
	about: Because it is a submix, only one instance of the effect is added, and all subsequent channels are affected.  
	<p>
	This is a wrapper method to insert a DSP unit at the top of the channel group DSP chain.
	</p>
	<p>
	It disconnects the head unit from its input, then inserts the unit at the head and reconnects the previously
	disconnected input back as as an input to the new unit.
	</p>
	End Rem
	Method AddDSP:Int(dsp:TFMODDsp)
		Return FMOD_ChannelGroup_AddDSP(channelGroupPtr, dsp.dspPtr, Null)
	End Method
	
	Rem
	bbdoc: Adds a channel group as a child of the current channel group.
	End Rem
	Method AddGroup:Int(group:TFMODChannelGroup)
		Return FMOD_ChannelGroup_AddGroup(channelGroupPtr, group.channelGroupPtr)
	End Method
	
	Rem
	bbdoc: Retrieves the master volume level for the channel group. 
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>volume</b> : Variable to receive the channel group volume level, from 0.0 to 1.0 inclusive.
	0.0 = silent, 1.0 = full volume. Default = 1.0. </li>
	</ul>
	End Rem
	Method GetVolume:Int(volume:Float Var)
		Return FMOD_ChannelGroup_GetVolume(channelGroupPtr, Varptr volume)
	End Method
	
	Rem
	bbdoc: Sets the master volume for the channel group linearly.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>volume</b> : A linear volume level, from 0.0 to 1.0 inclusive. 0.0 = silent, 1.0 = full volume.
	Default = 1.0.</li>
	</ul>
	End Rem
	Method SetVolume:Int(volume:Float)
		Return FMOD_ChannelGroup_SetVolume(channelGroupPtr, volume)
	End Method
	
	Rem
	bbdoc: Stops all channels within the channelgroup
	returns: If the method succeeds then the return value is FMOD_OK.
	End Rem
	Method Stop:Int()
		Return FMOD_ChannelGroup_Stop(channelGroupPtr)
	End Method
	
	Rem
	bbdoc: Retrieves the mute state of a ChannelGroup.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>mute</b> : Variable to receive the pause state of the channelgroup. </li>
	</ul>
	End Rem
	Method GetMute:Int(mute:Int Var)
		Return FMOD_ChannelGroup_GetMute(channelGroupPtr, Varptr mute)
	End Method
	
	Rem
	bbdoc: Mutes a channelgroup, and the channels within it, or unmutes any unmuted channels if set to false.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>mute</b> : Mute state to set. True = channelgroup state is set to muted. False = channelgroup state is set to unmuted.</li>
	</ul>
	End Rem
	Method SetMute:Int(mute:Int)
		Return FMOD_ChannelGroup_SetMute(channelGroupPtr, mute)
	End Method
	
	Rem
	bbdoc: Retrieves the pause state of a ChannelGroup.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>paused</b> : Variable to receive the pause state of the channelgroup.</li>
	</ul>
	End Rem
	Method GetPaused:Int(paused:Int Var)
		Return FMOD_ChannelGroup_GetPaused(channelGroupPtr, Varptr paused)
	End Method
	
	Rem
	bbdoc: Pauses a channelgroup, and the channels within it, or unpauses any unpaused channels if set to false.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>paused</b> : Paused state to set. true = channelgroup state is set to paused. false = channelgroup state is set to unpaused.</li>
	</ul>
	End Rem
	Method SetPaused:Int(paused:Int)
		Return FMOD_ChannelGroup_SetPaused(channelGroupPtr, paused)
	End Method
	
	Rem
	bbdoc: Retrieves the master pitch level for the channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>pitch</b> : Variable to receive the channel group pitch value, from 0.0 to 10.0 inclusive.
	0.0 = silent, 1.0 = full volume. Default = 1.0.</li>
	</ul>
	End Rem
	Method GetPitch:Int(pitch:Float Var)
		Return FMOD_ChannelGroup_GetPitch(channelGroupPtr, Varptr pitch)
	End Method
	
	Rem
	bbdoc: Sets the master pitch for the channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>pitch</b> : A pitch level, from 0.0 to 10.0 inclusive. 0.5 = half pitch, 2.0 = double pitch. Default = 1.0.</li>
	</ul>
	<p>
	This method does not go through and overwrite the channel frequencies. It scales them by the channel group's
	pitch.
	</p>
	<p>
	That way when TFMODChannel.setFrequency / TFMODChannel.getFrequency is called the respective individual channel
	frequencies will still be preserved. 
	</p>
	End Rem
	Method SetPitch:Int(pitch:Float)
		Return FMOD_ChannelGroup_SetPitch(channelGroupPtr, pitch)
	End Method
	
	Rem
	bbdoc: Retrieves the current number of assigned channels to this channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>numChannels</b> : Variable to receive the current number of assigned channels in this channel group.</li>
	</ul>
	<p>
	Use this method to enumerate the channels within the channel group. You can then use TFMODChannelGroup.getChannel
	to retrieve each individual channel.
	</p>
	End Rem
	Method GetNumChannels:Int(numChannels:Int Var)
		Return FMOD_ChannelGroup_GetNumChannels(channelGroupPtr, Varptr numChannels)
	End Method
	
	Rem
	bbdoc: Retrieves the number of sub groups under this channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>numGroups</b> : Variable to receive the number of channel groups within this channel group.</li>
	</ul>
	End Rem
	Method GetNumGroups:Int(numGroups:Int Var)
		Return FMOD_ChannelGroup_GetNumGroups(channelGroupPtr, Varptr numGroups)
	End Method
	
	Rem
	bbdoc: Retrieves a channel from the current channel group.
	about: Parameters: 
	<ul>
	<li><b>index</b> : Index of the channel inside the channel group, from 0 to the number of channels returned by
	TFMODChannelGroup.getNumChannels.</li>
	</ul>
	End Rem
	Method GetChannel:TFMODChannel(index:Int)
		Return TFMODChannel._create(bmx_FMOD_ChannelGroup_GetChannel(channelGroupPtr, index))
	End Method
	
	Rem
	bbdoc: Retrieves the master occlusion factors for the channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>directOcclusion</b> : Variable that receives the occlusion factor for the direct path.
	0.0 = not occluded. 1.0 = fully occluded. Default = 0.0.</li>
	<li><b>reverbOcclusion</b> : Variable that receives the occlusion factor for the reverb mix.
	0.0 = not occluded. 1.0 = fully occluded. Default = 0.0.</li>
	</ul>
	End Rem
	Method Get3DOcclusion:Int(directOcclusion:Float Var, reverbOcclusion:Float Var)
		Return FMOD_ChannelGroup_Get3DOcclusion(channelGroupPtr, Varptr directOcclusion, Varptr reverbOcclusion)
	End Method
	
	Rem
	bbdoc: Sets the master occlusion factors for the channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>directOcclusion</b> : Occlusion factor for the direct path. 0.0 = not occluded. 1.0 = fully occluded. Default = 0.0.</li>
	<li><b>reverbOcclusion</b> : Occlusion factor for the reverb mix. 0.0 = not occluded. 1.0 = fully occluded. Default = 0.0.</li>
	</ul>
	<p>
	This method does not go through and overwrite the channel occlusion factors. It scales them by the channel
	group's occlusion factors.
	</p>
	<p>
	That way when TFMODChannel.set3DOcclusion / TFMODChannel.get3DOcclusion is called the respective individual channel
	occlusion factors will still be preserved. This means that final Channel occlusion values will be affected by
	both ChannelGroup occlusion and geometry (if any). 
	</p>
	End Rem
	Method Set3DOcclusion:Int(directOcclusion:Float, reverbOcclusion:Float)
		Return FMOD_ChannelGroup_Set3DOcclusion(channelGroupPtr, directOcclusion, reverbOcclusion)
	End Method
	
	Rem
	bbdoc: Overrides the position and velocity of all channels within this channel group and those of any sub channelgroups.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>pos</b> : Position in 3D space of the channels in the group.</li>
	<li><b>vel</b> : Velocity in 'distance units per second' in 3D space of the group of channels.</li>
	</ul>
	<p>
	A 'distance unit' is specified by TFMODSystem.set3DSettings. By default this is set to meters which is a distance
	scale of 1.0.
	</p>
	End Rem
	Method Override3DAttributes:Int(pos:TFMODVector, vel:TFMODVector)
		Return FMOD_ChannelGroup_Override3DAttributes(channelGroupPtr, Varptr pos, Varptr vel)
	End Method
	
	Rem
	bbdoc: Overrides the frequency or playback rate, in HZ of all channels within this channel group and those of any sub channelgroups.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>frequency</b> : A frequency value in HZ. This value can also be negative to play the sound backwards
	(negative frequencies allowed with FMOD_SOFTWARE based non-stream sounds only). DirectSound hardware voices
	have limited frequency range on some soundcards.</li>
	</ul>
	<p>
	When a sound is played, it plays at the default frequency of the sound which can be set by TFMODSound.setDefaults.
	</p>
	<p>
	For most file formats, the volume is determined by the audio format.
	</p>
	<p>
	Frequency limitations for sounds created with FMOD_HARDWARE in DirectSound.
	</p>
	<p>
	Every hardware device has a minimum and maximum frequency. This means setting the frequency above the maximum
	and below the minimum will have no effect.
	</p>
	<p>
	FMOD clamps frequencies to these values when playing back on hardware, so if you are setting the frequency
	outside of this range, the frequency will stay at either the minimum or maximum.
	</p>
	<p>
	Note that FMOD_SOFTWARE based sounds do not have this limitation.
	</p>
	<p>
	To find out the minimum and maximum value before initializing FMOD (maybe to decide whether to use a different
	soundcard, output mode, or drop back fully to software mixing), you can use the TFMODSystem.getDriverCaps
	method. 
	</p>
	End Rem
	Method OverrideFrequency:Int(frequency:Float)
		Return FMOD_ChannelGroup_OverrideFrequency(channelGroupPtr, frequency)
	End Method
	
	Rem
	bbdoc: Sets pan position linearly of all channels within this channel group and those of any sub channelgroups.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>pan</b> : A left/right pan level, from -1.0 to 1.0 inclusive. -1.0 = Full left, 0.0 = center, 1.0 = full right.
	Default = 0.0.</li>
	</ul>
	<p>
	Panning only works on sounds created with FMOD_2D. 3D sounds are not pannable.
	</p>
	<p>
	Only sounds that are mono or stereo can be panned. Multichannel sounds (ie >2 channels) cannot be panned.
	</p>
	<p>
	Mono sounds are panned from left to right using constant power panning. This means when pan = 0.0, the balance
	for the sound in each speaker is 71% left and 71% right, not 50% left and 50% right. This gives (audibly)
	smoother pans.
	</p>
	<p>
	Stereo sounds heave each left/right value faded up and down according to the specified pan position. This
	means when pan = 0.0, the balance for the sound in each speaker is 100% left and 100% right. When pan = -1.0,
	only the left channel of the stereo sound is audible, when pan = 1.0, only the right channel of the stereo
	sound is audible.
	</p>
	End Rem
	Method OverridePan:Int(pan:Float)
		Return FMOD_ChannelGroup_OverridePan(channelGroupPtr, pan)
	End Method
	
	Rem
	bbdoc: Overrides all channel speaker levels for each speaker individually.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>frontleft</b> : Level for this channel in the front left speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume. </li>
	<li><b>frontright</b> : Level for this channel in the front right speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	<li><b>center</b> : Level for this channel in the center speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume. </li>
	<li><b>lfe</b> : Level for this channel in the subwoofer speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	<li><b>backleft</b> : Level for this channel in the back left speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	<li><b>backright</b> : Level for this channel in the back right speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	<li><b>sideleft</b> : Level for this channel in the side left speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	<li><b>sideright</b> : Level for this channel in the side right speaker of a multichannel speaker setup. 0.0 = silent, 1.0 = full volume.</li>
	</ul>
	<p>
	This method only works on sounds created with FMOD_2D. 3D sounds are not pannable and will return
	FMOD_ERR_NEEDS2D.
	</p>
	<p>
	Only sounds create with FMOD_SOFTWARE playing on this channel will allow this functionality.
	</p>
	<p>
	Speakers specified that don't exist will simply be ignored.
	</p>
	<p>
	For more advanced speaker control, including sending the different channels of a stereo sound to arbitrary
	speakers, see TFMODChannel.SetSpeakerLevels.
	</p>
	End Rem
	Method OverrideSpeakerMix:Int(frontleft:Float, frontright:Float, center:Float, ..
			lfe:Float, backleft:Float, backright:Float, sideleft:Float, sideright:Float)
		Return FMOD_ChannelGroup_OverrideSpeakerMix(channelGroupPtr, frontleft, frontright, center, ..
			lfe, backleft, backright, sideleft, sideright)
	End Method
	
	Rem
	bbdoc: Overrides the volume of all channels within this channel group and those of any sub channelgroups.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: Parameters: 
	<ul>
	<li><b>volume</b> : A linear volume level, from 0.0 to 1.0 inclusive. 0.0 = silent, 1.0 = full volume. Default = 1.0.</li>
	</ul>
	<p>
	This is not to be used as a master volume for the group, as it will modify the volumes of the channels
	themselves.
	</p>
	<p>
	If you want to scale the volume of the group, use TFMODChannelGroup.SetVolume.
	</p>
	End Rem
	Method OverrideVolume:Int(volume:Float)
		Return FMOD_ChannelGroup_OverrideVolume(channelGroupPtr, volume)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method OverrideReverbProperties:Int(properties:TFMODReverbChannelProperties)
		Return FMOD_ChannelGroup_OverrideReverbProperties(channelGroupPtr, Varptr properties)
	End Method
	
	Rem
	bbdoc: Frees a channel group.
	returns: If the method succeeds then the return value is FMOD_OK.
	about: All channels assigned to this group are returned back to the master channel group owned by the
	System object. See TFMODSystem.GetMasterChannelGroup.
	<p>
	All child groups assigned to this group are returned back to the master channel group owned by the System
	object. See TFMODSystem.GetMasterChannelGroup.
	</p>
	End Rem
	Method ChannelGroupRelease:Int()
		Return FMOD_ChannelGroup_Release(channelGroupPtr)
	End Method

End Type

Rem
bbdoc: Settings for advanced features like configuring memory and cpu usage for the FMOD_CREATECOMPRESSEDSAMPLE feature.
End Rem
Type TFMODAdvancedSettings

	Field advancedSettingsPtr:Byte Ptr

	Method New()
		advancedSettingsPtr = bmx_advancedsettings_create()
	End Method

	Method SetmaxMPEGcodecs(value:Int)
		bmx_advancedsettings_setmaxMPEGcodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxMPEGcodecs:Int()
		Return bmx_advancedsettings_getmaxMPEGcodecs(advancedSettingsPtr)
	End Method
	
	Method SetmaxADPCMcodecs(value:Int)
		bmx_advancedsettings_setmaxADPCMcodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxADPCMcodecs:Int()
		Return bmx_advancedsettings_getmaxADPCMcodecs(advancedSettingsPtr)
	End Method
	
	Method SetmaxXMAcodecs(value:Int)
		bmx_advancedsettings_setmaxXMAcodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxXMAcodecs:Int()
		Return bmx_advancedsettings_getmaxXMAcodecs(advancedSettingsPtr)
	End Method
	
	Method SetmaxCELTcodecs(value:Int)
		bmx_advancedsettings_setmaxCELTcodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxCELTcodecs:Int()
		Return bmx_advancedsettings_getmaxCELTcodecs(advancedSettingsPtr)
	End Method
	
	Method SetmaxVORBIScodecs(value:Int)
		bmx_advancedsettings_setmaxVORBIScodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxVORBIScodecs:Int()
		Return bmx_advancedsettings_getmaxVORBIScodecs(advancedSettingsPtr)
	End Method
	
	Method SetmaxPCMcodecs(value:Int)
		bmx_advancedsettings_setmaxPCMcodecs(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxPCMcodecs:Int()
		Return bmx_advancedsettings_getmaxPCMcodecs(advancedSettingsPtr)
	End Method
	
	Method SetASIONumChannels(value:Int)
		bmx_advancedsettings_setASIONumChannels(advancedSettingsPtr, value)
	End Method
	
	Method GetASIONumChannels:Int()
		Return bmx_advancedsettings_getASIONumChannels(advancedSettingsPtr)
	End Method
	
	Method Setmax3DReverbDSPs(value:Int)
		bmx_advancedsettings_setmax3DReverbDSPs(advancedSettingsPtr, value)
	End Method
	
	Method Getmax3DReverbDSPs:Int()
		Return bmx_advancedsettings_getmax3DReverbDSPs(advancedSettingsPtr)
	End Method
	
	Method SetHRTFMinAngle(value:Float)
		bmx_advancedsettings_setHRTFMinAngle(advancedSettingsPtr, value)
	End Method
	
	Method GetHRTFMinAngle:Float()
		Return bmx_advancedsettings_getHRTFMinAngle(advancedSettingsPtr)
	End Method
	
	Method SetHRTFMaxAngle(value:Float)
		bmx_advancedsettings_setHRTFMaxAngle(advancedSettingsPtr, value)
	End Method
	
	Method GetHRTFMaxAngle:Float()
		Return bmx_advancedsettings_getHRTFMaxAngle(advancedSettingsPtr)
	End Method
	
	Method SetHRTFFreq(value:Float)
		bmx_advancedsettings_setHRTFFreq(advancedSettingsPtr, value)
	End Method
	
	Method GetHRTFFreq:Float()
		Return bmx_advancedsettings_getHRTFFreq(advancedSettingsPtr)
	End Method
	
	Method Setvol0virtualvol(value:Float)
		bmx_advancedsettings_setvol0virtualvol(advancedSettingsPtr, value)
	End Method
	
	Method Getvol0virtualvol:Float()
		Return bmx_advancedsettings_getvol0virtualvol(advancedSettingsPtr)
	End Method
	
	Method Seteventqueuesize(value:Int)
		bmx_advancedsettings_seteventqueuesize(advancedSettingsPtr, value)
	End Method
	
	Method Geteventqueuesize:Int()
		Return bmx_advancedsettings_geteventqueuesize(advancedSettingsPtr)
	End Method
	
	Method SetdefaultDecodeBufferSize(value:Int)
		bmx_advancedsettings_setdefaultDecodeBufferSize(advancedSettingsPtr, value)
	End Method
	
	Method GetdefaultDecodeBufferSize:Int()
		Return bmx_advancedsettings_getdefaultDecodeBufferSize(advancedSettingsPtr)
	End Method
	
	Method Setprofileport(value:Int)
		bmx_advancedsettings_setprofileport(advancedSettingsPtr, value)
	End Method
	
	Method Getprofileport:Int()
		Return bmx_advancedsettings_getprofileport(advancedSettingsPtr)
	End Method
	
	Method SetgeometryMaxFadeTime(value:Int)
		bmx_advancedsettings_setgeometryMaxFadeTime(advancedSettingsPtr, value)
	End Method
	
	Method GetgeometryMaxFadeTime:Int()
		Return bmx_advancedsettings_getgeometryMaxFadeTime(advancedSettingsPtr)
	End Method
	
	Method SetmaxSpectrumWaveDataBuffers(value:Int)
		bmx_advancedsettings_setmaxSpectrumWaveDataBuffers(advancedSettingsPtr, value)
	End Method
	
	Method GetmaxSpectrumWaveDataBuffers:Int()
		Return bmx_advancedsettings_getmaxSpectrumWaveDataBuffers(advancedSettingsPtr)
	End Method
	
	Method SetmusicSystemCacheDelay(value:Int)
		bmx_advancedsettings_setmusicSystemCacheDelay(advancedSettingsPtr, value)
	End Method
	
	Method GetmusicSystemCacheDelay:Int()
		Return bmx_advancedsettings_getmusicSystemCacheDelay(advancedSettingsPtr)
	End Method
	
	Method SetdistanceFilterCenterFreq(value:Float)
		bmx_advancedsettings_setdistanceFilterCenterFreq(advancedSettingsPtr, value)
	End Method
	
	Method GetdistanceFilterCenterFreq:Float()
		Return bmx_advancedsettings_getdistanceFilterCenterFreq(advancedSettingsPtr)
	End Method
	
	Method SetstackSizeStream(value:Int)
		bmx_advancedsettings_setstackSizeStream(advancedSettingsPtr, value)
	End Method
	
	Method GetstackSizeStream:Int()
		Return bmx_advancedsettings_getstackSizeStream(advancedSettingsPtr)
	End Method
	
	Method SetstackSizeNonBlocking(value:Int)
		bmx_advancedsettings_setstackSizeNonBlocking(advancedSettingsPtr, value)
	End Method
	
	Method GetstackSizeNonBlocking:Int()
		Return bmx_advancedsettings_getstackSizeNonBlocking(advancedSettingsPtr)
	End Method
	
	Method SetstackSizeMixer(value:Int)
		bmx_advancedsettings_setstackSizeMixer(advancedSettingsPtr, value)
	End Method
	
	Method GetstackSizeMixer:Int()
		Return bmx_advancedsettings_getstackSizeMixer(advancedSettingsPtr)
	End Method
	
	Method Delete()
		If advancedSettingsPtr Then
			bmx_advancedsettings_delete(advancedSettingsPtr)
			advancedSettingsPtr = Null
		End If
	End Method	

End Type

Rem
'bbdoc: Structure defining the properties For a reverb source, related To an FMOD channel.
'End Rem
Type TFMODReverbChannelProperties

	Field reverbPtr:Byte Ptr

	Function _create:TFMODReverbChannelProperties(reverbPtr:Byte Ptr)
		If reverbPtr Then
			Local this:TFMODReverbChannelProperties = New TFMODReverbChannelProperties
			this.reverbPtr = reverbPtr
			Return this
		End If
	End Function
	
	Method Delete()
		If reverbPtr Then
			bmx_fmodreverbchannelproperties_delete(reverbPtr)
			reverbPtr = Null
		End If
	End Method

End Type
End Rem
