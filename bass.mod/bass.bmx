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
bbdoc: BASS Audio Library
End Rem
Module BaH.Bass

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: BASS - see http://www.un4seen.com/bass.html#license"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Copyright: BASS - 1999-2008 Un4seen Developments Ltd."

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?

?Not Linux

Import BRL.Stream
Import "common.bmx"

Private
Global fx_factories:TBassFXFactory
Public


Rem
Parameters: 
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
bbdoc: Used with TBass.GetInfo to retrieve information on the current device. 
about: 
<p>
<b>OS X notes</b> - Only the latency, minbuf, initflags, speakers and freq members are used. Hardware mixing and
EAX are unavailable in OS X. The speakers and freq values will change as the device's output is changed, eg.
via its control panel.
</p>
End Rem
Type TBassInfo

	Field bassInfoPtr:Byte Ptr
	
	Function _create:TBassInfo(bassInfoPtr:Byte Ptr)
		If bassInfoPtr Then
			Local this:TBassInfo = New TBassInfo
			this.bassInfoPtr = bassInfoPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: flagsThe device's capabilities... a combination of these flags.
	about: 
	End Rem
	Method GetFlags:Int()
		Return bmx_bassinfo_getflags(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The device's total amount of hardware memory. 
	End Rem
	Method GetHWSize:Int()
		Return bmx_bassinfo_gethwsize(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The device's amount of free hardware memory.
	End Rem
	Method GetHWFree:Int()
		Return bmx_bassinfo_gethwfree(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The number of free sample slots in the hardware.
	End Rem
	Method GetFreeSam:Int()
		Return bmx_bassinfo_getfreesam(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The number of free 3D sample slots in the hardware.
	End Rem
	Method GetFree3d:Int()
		Return bmx_bassinfo_getfree3d(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The minimum sample rate supported by the hardware.
	End Rem
	Method GetMinRate:Int()
		Return bmx_bassinfo_getminrate(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The maximum sample rate supported by the hardware.
	End Rem
	Method GetMaxRate:Int()
		Return bmx_bassinfo_getmaxrate(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The device supports EAX and has it enabled?
	about: The device's "Hardware acceleration" needs to be set to "Full" in its "Advanced Properties" setup,
	else EAX is disabled. This is always FALSE if BASS_DEVICE_3D was not specified when TBass.Init was called. 
	End Rem
	Method GetEAX:Int()
		Return bmx_bassinfo_geteax(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The minimum buffer length (rounded up to the nearest millisecond) recommended for use (with the BASS_CONFIG_BUFFER config option).
	about: Requires that BASS_DEVICE_LATENCY was used when TBass.Init was called
	End Rem
	Method GetMinBuf:Int()
		Return bmx_bassinfo_getminbuf(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: DirectSound version...
	about: 9 = DX9/8/7/5 features are available, 8 = DX8/7/5 features are available,
	7 = DX7/5 features are available, 5 = DX5 features are available.
	0 = none of the DX9/8/7/5 features are available.
	End Rem
	Method GetDSVer:Int()
		Return bmx_bassinfo_getdsver(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The average delay (rounded up to the nearest millisecond) for playback of TBassStream/TBassMusic channels to start and be heard.
	about: Requires that BASS_DEVICE_LATENCY was used when TBass.Init was called.
	End Rem
	Method GetLatency:Int()
		Return bmx_bassinfo_getlatency(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The flags parameter of the TBass.Init call.
	End Rem
	Method GetInitFlags:Int()
		Return bmx_bassinfo_getinitflags(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The number of speakers the device/drivers supports...
	about: 2 means that there is no support for speaker assignment (this will always be the case with VxD
	drivers). It's also possible that it could mistakenly be 2 with some devices/drivers, when the device in 
	fact supports more speakers. In that case the BASS_DEVICE_CPSPEAKERS flag can be used in the TBass.Init call
	to use the Windows control panel setting, or the BASS_DEVICE_SPEAKERS flag can be used to force the
	enabling of speaker assignment.
	End Rem
	Method GetSpeakers:Int()
		Return bmx_bassinfo_getspeakers(bassInfoPtr)
	End Method
	
	Rem
	bbdoc: The device's current output sample rate.
	about: This is only available on Windows Vista and OS X.
	End Rem
	Method GetFreq:Int()
		Return bmx_bassinfo_getfreq(bassInfoPtr)
	End Method
	
	Method Delete()
		If bassInfoPtr Then
			bmx_bassinfo_delete(bassInfoPtr)
			bassInfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A "channel" can be a sample playback channel (TBassSample), a sample stream (TBassStream), a MOD music (TBassMusic), or a recording (TBassRecord).
about: Each "Channel" function can be used with one or more of these channel types.
End Rem
Type TBassChannel

	Field handle:Int

	Function _create:TBassChannel(handle:Int)
		If handle Then
			Local this:TBassChannel = New TBassChannel
			this.handle = handle
			Return this
		End If
	End Function

	Rem
	bbdoc: Starts (or resumes) playback of a sample, stream, MOD music, or recording.
	returns: If successful, TRUE is returned, else FALSE is returned. Use #BASS_ErrorGetCode to get the error code. 
	about: Parameters:
	<ul>
	<li><b>restart</b> : Restart playback from the beginning? If channel is a user stream, its current buffer contents are cleared.
	If it is a MOD music, its BPM/etc are reset to their initial values.</li>
	</ul>
	When streaming in blocks (BASS_STREAM_BLOCK), the restart parameter is ignored as it's not possible
	to go back to the start. The restart parameter is also of no consequence with recording channels. 
	End Rem
	Method Play:Int(restart:Int = True)
		Return BASS_ChannelPlay(handle, restart)
	End Method
	
	Rem
	bbdoc: Pauses a sample, stream, MOD music, or recording.
	returns: If successful, TRUE is returned, else FALSE is returned. Use #BASS_ErrorGetCode to get the error code.
	about: Use #Play to resume a paused channel. #Stop can be used to stop a paused channel.
	End Rem
	Method Pause:Int()
		Return BASS_ChannelPause(handle)
	End Method
	
	Rem
	bbdoc: Stops a sample, stream, MOD music, or recording. 
	End Rem
	Method Stop:Int()
		Return BASS_ChannelStop(handle)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLength:Long(mode:Int)
		Local length:Long
		bmx_bass_channelgetlength(handle, Varptr length, mode)
		Return length
	End Method
	
	Rem
	bbdoc: Retrieves the level (peak amplitude) of a stream, MOD music or recording channel.
	about: This method measures the level of the channel's sample data, not the level of the channel in the
	final output mix, so the channel's volume (BASS_ATTRIB_VOL attribute) does not affect it. 
	<p>
	For channels that are more than stereo, the left level will include all left channels (eg. front-left,
	rear-left, center), and the right will include all right (front-right, rear-right, LFE). If there are an
	odd number of channels then the left and right levels will include all channels. If the level of each
	individual channel is required, that can be calculated from the sample data that #GetData
	provides. 
	</p>
	<p>
	When used with a decoding channel, this method has to decode data (20ms worth) from the channel in order
	to calculate the level. Which means that data is gone, eg. it is not available to #GetData. 
	</p>
	<p>
	If an error occurs, -1 is returned, use TBass.ErrorGetCode to get the error code. If successful, the level
	of the left channel is returned in the low word (low 16-bits), and the level of the right channel is
	returned in the high word (high 16-bits). If the channel is mono, then the low word is duplicated in the
	high word. The level ranges linearly from 0 (silent) to 32768 (max). 0 will be returned when a channel is
	stalled. 
	</p>
	End Rem
	Method GetLevel:Int(leftLevel:Int Var, rightLevel:Int Var)
		Return bmx_bass_channelgetlevel(handle, Varptr leftLevel, Varptr rightLevel)
	End Method
	
	Rem
	bbdoc: Retrieves tags/headers from a channel. 
	returns: If successful, the requested tags are returned, else NULL is returned. Use TBass.ErrorGetCode to get the error code. 
	about: Parameters: 
	<ul>
	<li><b>tags</b> : The tags/headers wanted... one of the following.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_TAG_ID3</td><td>BASS_TAG_ID3ID3v1 tags. A pointer to a 128 byte block is returned.
	See http://www.id3.org/ID3v1 for details of the block's structure. </td></tr>
	<tr><td>BASS_TAG_ID3V2</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_LYRICS3</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_OGG</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_VENDOR</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_HTTP</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_ICY</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_META</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_RIFF_INFO</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_MUSIC_NAME</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_MUSIC_MESSAGE</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_MUSIC_INST</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	<tr><td>BASS_TAG_MUSIC_SAMPLE</td><td>xxxxxxxxxxxxxxxxxxx</td></tr>
	</table>	
	</li>
	</ul>
	<p>
	ID3v1 tags are located at the end of the file, so when streaming a file with ID3v1 tags from the internet,
	the tags will not be available until the download is complete. A BASS_SYNC_DOWNLOAD sync can be set via
	BASS_ChannelSetSync, to be informed of when the download is complete. A BASS_SYNC_META sync can be used to
	be informed of new Shoutcast metadata, and a BASS_SYNC_OGG_CHANGE sync for when a new logical bitstream
	begins in a chained OGG stream, which generally brings new OGG tags. 
	</p>
	End Rem
	Method GetTags:String[](tags:Int)
		Return bmx_bass_channelgettags(handle, tags)
	End Method
	
	Rem
	bbdoc: Checks if a sample, stream, or MOD music is active (playing) or stalled.
	about: Can also check if a recording is in progress.
	<p>
	When using this method with a decoding channel, BASS_ACTIVE_PLAYING will be returned while there
	is still data to decode. Once the end has been reached, BASS_ACTIVE_STOPPED will be returned.
	BASS_ACTIVE_STALLED is never returned for decoding channels; you can tell a decoding channel is
	stalled if BASS_ChannelGetData returns less data than requested, and this function still returns
	BASS_ACTIVE_PLAYING. 
	</p>
	End Rem
	Method IsActive:Int()
		Return BASS_ChannelIsActive(handle)
	End Method
	
	Rem
	bbdoc: Retrieves the playback position of a sample, stream, or MOD music.
	returns: If successful, then the channel's position is returned, else -1 is returned. Use TBass.ErrorGetCode to get the error code. 
	about: Can also be used with a recording channel.
	<p>Parameters: 
	<ul>
	<li><b>mode</b> : How to retrieve the position. One of the following.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_POS_BYTE</td><td>Get the position in bytes.</td></tr>
	<tr><td>BASS_POS_MUSIC_ORDER</td><td>Get the position in orders and rows... LOWORD = order, HIWORD = row * scaler (BASS_ATTRIB_MUSIC_PSCALER). (TBassMusic only)</td></tr>
	</table>
	<i>Other modes may be supported by add-ons, see the documentation.</i>
	</li>
	</ul>
	</p>
	End Rem
	Method GetPosition:Long(mode:Int)
		Local pos:Long
		bmx_bass_channelgetposition(handle, Varptr pos, mode)
		Return pos
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetPositionLowHigh:Long(mode:Int, low:Int Var, high:Int Var)
		Local pos:Long
		bmx_bass_channelgetpositionlowhigh(handle, Varptr pos, mode, Varptr low, Varptr high)
		Return pos
	End Method
	
	Rem
	bbdoc: Updates the playback buffer of a stream or MOD music. 
	returns: If successful, TRUE is returned, else FALSE is returned. Use #ErrorGetCode to get the error code. 
	about: Parameters: 
	<ul>
	<li><b>length</b> : The amount to render, in milliseconds... 0 = default (2 x update period). This is capped
	at the space available in the buffer. </li>
	</ul>
	<p>
	When starting playback of a stream or MOD music, after creating it or changing its position, there
	will be a slight delay while the initial data is decoded for playback. Usually the delay is not noticeable
	or important, but if you need playback to start instantly when you call #Play, then use this
	method first. The length parameter should be at least equal to the update period. 
	</p>
	<p>
	It may not always be possible to render the requested amount of data, in which case this method will
	still succeed. #GetData(BASS_DATA_AVAILABLE) can be used to check how much data a channel has
	buffered for playback. 
	</p>
	<p>
	When automatic updating is disabled (BASS_CONFIG_UPDATEPERIOD = 0), this method could be used instead of
	TBass.Update to implement different update periods for different channels, instead of a single update period
	for all. Unlike TBass.Update, this method can also be used while automatic updating is enabled. 
	</p>
	<p>
	The CPU usage of this method is not included in the TBass.GetCPU reading.
	</p>
	End Rem
	Method Update:Int(length:Int)
		Return BASS_ChannelUpdate(handle, length)
	End Method
	
	Rem
	bbdoc: Slides a channel's attribute from its current value to a new value. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: This method is similar to #SetAttribute, except that the attribute is ramped to the value over the
	specified period of time. Another difference is that the value is not pre-checked. If it is invalid, the
	slide will simply end early. 
	<p>
	</p>
	If an attribute is already sliding, then the old slide is stopped and replaced by the new one. 
	<p>
	</p>
	#IsSliding can be used to check if an attribute is currently sliding. A BASS_SYNC_SLIDE sync can
	also be set via #SetSync, to be triggered at the end of a slide. The sync will not be triggered
	in the case of an existing slide being replaced by a new one. 
	<p>
	</p>
	Attribute slides are unaffected by whether the channel is playing, paused or stopped. They carry on
	regardless. 
	</p>
	End Rem
	Method SlideAttribute:Int(attrib:Int, value:Float, time:Int)
		Return BASS_ChannelSlideAttribute(handle, attrib, value, time)
	End Method
	
	Rem
	bbdoc: Checks if an attribute (or any attribute) of a sample, stream, or MOD music is sliding. 
	returns: If the attribute is sliding, then TRUE is returned, else FALSE is returned.
	about: 
	End Rem
	Method IsSliding:Int(attrib:Int)
		Return BASS_ChannelIsSliding(handle, attrib)
	End Method
	
	Rem
	bbdoc: Translates a byte position into time (seconds), based on a channel's format.
	returns: If successful, then the translated length is returned, else a negative value is returned. Use TBass.ErrorGetCode to get the error code. 
	about: The translation is based on the channel's initial sample rate, when it was created. 
	End Rem
	Method Bytes2Seconds:Double(pos:Long)
		Return BASS_ChannelBytes2Seconds(handle, pos)
	End Method
	
	Rem
	bbdoc: Modifies and retrieves a channel's flags. 
	returns: If successful, the channel's updated flags are returned, else -1 is returned. Use BASS_ErrorGetCode to get the error code.
	about: Some flags may not be adjustable in some circumstances, so the return value should be checked to
	confirm any changes. The flags listed above are just the flags that can be modified, and there may be
	additional flags present in the return value. See the BASS_CHANNELINFO documentation for a full list of
	flags. 
	<p>
	Streams that are created by add-ons may have additional flags available. There is a limited number of
	possible flag values though, so some add-ons may use the same flag value for different things. This means
	that when using add-on specific flags with a stream created via the plugin system, it is a good idea to
	first confirm that the add-on is handling the stream, by checking its ctype via BASS_ChannelGetInfo. 
	</p>
	<p>
	During playback, the effects of flag changes are not heard instantaneously, due to buffering. To reduce the
	delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	End Rem
	Method Flags:Int(flags:Int, mask:Int)
		Return BASS_ChannelFlags(handle, flags, mask)
	End Method
	
	Rem
	bbdoc: Sets an effect on a stream, MOD music, or recording channel. 
	returns: If successful, then the new effect's handle is returned, else Null is returned. Use TBass.ErrorGetCode to get the error code.
	End Rem
	Method SetFX:TBassFX(fxType:Int, priority:Int)
		Return TBassFX._create(BASS_ChannelSetFX(handle, fxType, priority), fxType)
	End Method
	
	Rem
	bbdoc: Retrieves the value of a channel's attribute. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: Parameters: 
	<ul>
	<li><b>attrib</b> : The attribute to set the value of... one of the following.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_ATTRIB_EAXMIX</td><td>EAX wet/dry mix. (TBassSample/TBassMusic/TBassStream only)</td></tr>
	<tr><td>BASS_ATTRIB_FREQ</td><td>Sample rate.</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_AMPLIFY</td><td>Amplification level. (TBassMusic)</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_BPM</td><td>BPM. (TBassMusic) </td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_PANSEP</td><td>Pan separation level. (TBassMusic) </td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_PSCALER</td><td>Position scaler. (TBassMusic)</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_SPEED</td><td>Speed. (TBassMusic) </td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_CHAN</td><td>A channel volume level. (TBassMusic) </td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_GLOBAL</td><td>Global volume level. (TBassMusic) </td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_INST</td><td>An instrument/sample volume level. (TBassMusic)</td></tr>
	<tr><td>BASS_ATTRIB_PAN</td><td>Panning/balance position.</td></tr>
	<tr><td>BASS_ATTRIB_VOL</td><td>Volume level.</td></tr>
	</table>
	<i>Other attributes may be supported by add-ons, see the documentation.</i>
	</li>
	<li><b>value</b> : A variable to receive the attribute value.</li>
	</ul>
	End Rem
	Method GetAttribute:Int(attrib:Int, value:Float Var)
		Return BASS_ChannelGetAttribute(handle, attrib, Varptr value)
	End Method
	
	Rem
	bbdoc: Sets the value of a channel's attribute. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: Parameters: 
	<ul>
	<li><b>attrib</b> : The attribute to set the value of... one of the following.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_ATTRIB_EAXMIX</td><td>EAX wet/dry mix. (TBassSample/TBassMusic/TBassStream only)
	<p>
	The wet / dry ratio... 0 (full dry) to 1 (full wet), -1 = automatically calculate the mix based on the distance (the default).
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_FREQ</td><td>Sample rate.
	<p>
	The sample rate... 100 (min) to 100000 (max), 0 = original rate (when the channel was created). The value
	will be rounded down to a whole number. 
	</p>
	<p>
	This attribute applies to playback of the channel, and does not affect the channel's sample data, so has no
	real effect on decoding channels. It is still adjustable though, so that it can be used by the BASSmix
	add-on, and anything else that wants to use it. 
	</p>
	<p>
	Although the standard valid sample rate range is 100 to 100000, some devices/drivers may have a different
	valid range. See the minrate and maxrate members of the TBassInfo Type. 
	</p>
	<p>
	It is not possible to change the sample rate of a channel if the "with FX flag" DX8 effect implementation
	enabled on it, unless DirectX 9 or above is installed. 
	</p>
	<p>
	It requires an increased amount of CPU processing to play MOD musics and streams at increased sample rates.
	If you plan to play MOD musics or streams at greatly increased sample rates, then you should increase the
	buffer lengths (BASS_CONFIG_BUFFER) to avoid possible break-ups in the sound. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_AMPLIFY</td><td>Amplification level. (TBassMusic)
	<p>
	Amplification level... 0 (min) to 100 (max). This will be rounded down to a whole number. 
	</p>
	<p>
	As the amplification level get's higher, the sample data's range increases, and therefore, the resolution
	increases. But if the level is set too high, then clipping can occur, which can result in distortion of the
	sound. 
	</p>
	<p>
	You can check the current level of a MOD music at any time by using TBassChannel.GetLevel. By doing so, you
	can decide if a MOD music's amplification level needs adjusting. 
	</p>
	<p>
	The default amplification level is 50. 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_BPM</td><td>BPM. (TBassMusic)
	<p>
	The BPM... 1 (min) to 255 (max). This will be rounded down to a whole number. 
	</p>
	<p>
	This attribute is a direct mapping of the MOD's BPM, so the value can be changed via effects in the MOD
	itself. 
	</p>
	<p>
	Note that by changing this attribute, you are changing the playback length. 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_PANSEP</td><td>Pan separation level. (TBassMusic)
	<p>
	Pan separation... 0 (min) to 100 (max), 50 = linear. This will be rounded down to a whole number.
	</p>
	<p>
	By default BASS uses a linear panning "curve". If you want to use the panning of FT2, use a pan separation
	setting of around 35. To use the Amiga panning (ie. full left and right) set it to 100. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_PSCALER</td><td>Position scaler. (TBassMusic)
	<p>
	The scaler... 1 (min) to 256 (max). This will be rounded down to a whole number. 
	</p>
	<p>
	When getting the order position via TBassChannel.GetPosition, the row will be scaled by this value.
	By using a higher scaler, you can get a more precise position indication. 
	</p>
	<p>
	The default position scaler is 1. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_SPEED</td><td>Speed. (TBassMusic)
	<p>
	The speed... 0 (min) to 255 (max). This will be rounded down to a whole number. 
	</p>
	<p>
	This attribute is a direct mapping of the MOD's speed, so the value can be changed via effects in the MOD
	itself. 
	</p>
	<p>
	The "speed" is the number of ticks per row. Setting it to 0, stops and ends the music. Note that by
	changing this attribute, you are changing the playback length. 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_CHAN + channel</td><td>A channel volume level. (TBassMusic)
	<p>
	For channels, 0 = first channel.
	</p>
	<p>
	The volume curve used by this attribute is always linear, eg. 0.5 = 50%. The BASS_CONFIG_CURVE_VOL config
	option setting has no effect on this. The volume level of all channels is initially 1 (full). 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	<p>
	This attribute can also be used to count the number of channels in a MOD Music.
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_GLOBAL</td><td>Global volume level. (TBassMusic)
	<p>
	The global volume level... 0 (min) to 64 (max, 128 for IT format). This will be rounded down to a whole number. 
	</p>
	<p>
	This attribute is a direct mapping of the MOD's global volume, so the value can be changed via effects in
	the MOD itself. 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length.
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_MUSIC_VOL_INST + inst</td><td>An instrument/sample volume level. (TBassMusic)
	<p>
	For instruments, 0 = first instrument.
	</p>
	<p>
	The volume curve used by this attribute is always linear, eg. 0.5 = 50%. The BASS_CONFIG_CURVE_VOL config
	option setting has no effect on this. The volume level of all instruments is initially 1 (full). For MOD
	formats that do not use instruments, read "sample" for "instrument". 
	</p>
	<p>
	During playback, the effect of changes to this attribute are not heard instantaneously, due to buffering.
	To reduce the delay, use the BASS_CONFIG_BUFFER config option to reduce the buffer length. 
	</p>
	<p>
	This attribute can also be used to count the number of instruments in a MOD music. 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_PAN</td><td>Panning/balance position.
	<p>
	The pan position... -1 (full left) to +1 (full right), 0 = centre. 
	</p>
	<p>
	This attribute applies to playback of the channel, and does not affect the channel's sample data, so has no
	real effect on decoding channels. It is still adjustable though, so that it can be used by the BASSmix
	add-on, and anything else that wants to use it. 
	</p>
	<p>
	It is not possible to set the pan position of a 3D channel. It is also not possible to set the pan position
	when using speaker assignment, but if needed, it can be done via a DSP function instead (not on mono
	channels). 
	</p>
	</td></tr>
	<tr><td>BASS_ATTRIB_VOL</td><td>Volume level.
	<p>
	The volume level... 0 (silent) to 1 (full). 
	</p>
	<p>
	This attribute applies to playback of the channel, and does not affect the channel's sample data, so has no
	real effect on decoding channels. It is still adjustable though, so that it can be used by the BASSmix
	add-on, and anything else that wants to use it. 
	<p>
	</p>
	When using BASS_ChannelSlideAttribute to slide this attribute, a negative volume value can be used to
	fade-out and then stop the channel. 
	</p>
	</td></tr>
	</table>
	<i>Other attributes may be supported by add-ons, see the documentation.</i>
	</li>
	<li><b>value</b> : A variable to receive the attribute value.</li>
	</ul>
	End Rem
	Method SetAttribute:Int(attrib:Int, value:Float)
		Return BASS_ChannelSetAttribute(handle, attrib, value)
	End Method
	
	Rem
	bbdoc: Links two MOD music or stream channels together.
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: Linked channels are started/stopped/paused/resumed together. Links are one-way, for example, channel
	chan will be started by channel handle, but not vice versa unless another link has been set in that
	direction. 
	<p>
	If a linked channel has reached the end, it will not be restarted when a channel it is linked to is
	started. If you want a linked channel to be restarted, you need to have resetted its position using
	#SetPosition beforehand.
	</p>
	End Rem
	Method SetLink:Int(channel:TBassChannel)
		Return BASS_ChannelSetLink(handle, channel.handle)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Get3DPosition:Int(pos:TBass3DVector = Null, orient:TBass3DVector = Null, vel:TBass3DVector = Null)
		If pos Then
			If orient Then
				If vel Then
					Return BASS_ChannelGet3DPosition(handle, pos.vectorPtr, orient.vectorPtr, vel.vectorPtr)
				Else
					Return BASS_ChannelGet3DPosition(handle, pos.vectorPtr, orient.vectorPtr, Null)
				End If
			Else
				If vel Then
					Return BASS_ChannelGet3DPosition(handle, pos.vectorPtr, Null, vel.vectorPtr)
				Else
					Return BASS_ChannelGet3DPosition(handle, pos.vectorPtr, Null, Null)
				End If
			End If
		Else
			If orient Then
				If vel Then
					Return BASS_ChannelGet3DPosition(handle, Null, orient.vectorPtr, vel.vectorPtr)
				Else
					Return BASS_ChannelGet3DPosition(handle, Null, orient.vectorPtr, Null)
				End If
			Else
				If vel Then
					Return BASS_ChannelGet3DPosition(handle, Null, Null, vel.vectorPtr)
				Else
					Return BASS_ChannelGet3DPosition(handle, Null, Null, Null)
				End If
			End If
		End If
	End Method
	
	Rem
	bbdoc: Sets the 3D position of a sample, stream, or MOD music channel with 3D functionality. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: 
	End Rem
	Method Set3DPosition:Int(pos:TBass3DVector = Null, orient:TBass3DVector = Null, vel:TBass3DVector = Null)
		If pos Then
			If orient Then
				If vel Then
					Return BASS_ChannelSet3DPosition(handle, pos.vectorPtr, orient.vectorPtr, vel.vectorPtr)
				Else
					Return BASS_ChannelSet3DPosition(handle, pos.vectorPtr, orient.vectorPtr, Null)
				End If
			Else
				If vel Then
					Return BASS_ChannelSet3DPosition(handle, pos.vectorPtr, Null, vel.vectorPtr)
				Else
					Return BASS_ChannelSet3DPosition(handle, pos.vectorPtr, Null, Null)
				End If
			End If
		Else
			If orient Then
				If vel Then
					Return BASS_ChannelSet3DPosition(handle, Null, orient.vectorPtr, vel.vectorPtr)
				Else
					Return BASS_ChannelSet3DPosition(handle, Null, orient.vectorPtr, Null)
				End If
			Else
				If vel Then
					Return BASS_ChannelSet3DPosition(handle, Null, Null, vel.vectorPtr)
				Else
					Return BASS_ChannelSet3DPosition(handle, Null, Null, Null)
				End If
			End If
		End If
	End Method
	
	Rem
	bbdoc: Sets the 3D attributes of a sample, stream, or MOD music channel with 3D functionality. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: 
	End Rem
	Method Set3DAttributes:Int(mode:Int = -1, minDist:Float = 0, maxDist:Float = 0, iangle:Int = -1, ..
			oangle:Int = -1, outvol:Float = -1)
		Return BASS_ChannelSet3DAttributes(handle, mode, minDist, maxDist, iangle, oangle, outvol)
	End Method
	
	Rem
	bbdoc: Retrieves the 3D attributes of a sample, stream, or MOD music channel with 3D functionality.
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: 
	End Rem
	Method Get3DAttributes:Int(mode:Int Var, minDist:Float Var, maxDist:Float Var, iangle:Int Var, oangle:Int Var, ..
			outvol:Float Var)
		Return BASS_ChannelGet3DAttributes(handle, Varptr mode, Varptr minDist, Varptr maxDist, ..
			Varptr iangle, Varptr oangle, Varptr outvol)
	End Method

	Rem
	bbdoc: Retrieves the immediate sample data (or an FFT representation of it) of a stream or MOD music channel.
	returns: If an error occurs, -1 is returned, use BASS_ErrorGetCode to get the error code. When requesting FFT data, the number of bytes read from the channel (to perform the FFT) is returned. When requesting sample data, the number of bytes written to buffer will be returned (not necessarily the same as the number of bytes read when using the BASS_DATA_FLOAT flag). When using the BASS_DATA_AVAILABLE flag, the number of bytes in the channel's buffer is returned.
	about: Can also be used with a recording channel.
	<p>
	This method can only return as much data as has been written to the channel's buffer, so it may not always
	be possible to get the amount of data requested, especially if you request large amounts. If you really do
	need large amounts, then increase the buffer lengths (BASS_CONFIG_BUFFER). The BASS_DATA_AVAILABLE flag
	can be used to check how much data a channel's buffer contains at any time, including when stopped or stalled.
	</p>
	<p>
	When requesting data from a "decoding channel" (BASS_STREAM_DECODE or BASS_MUSIC_DECODE was used at creation),
	there are no intermediate buffers involved, so as much data as is available can be decoded in one go.
	</p>
	<p>
	When retrieving sample data, 8-bit samples are unsigned (0 to 255) , 16-bit samples are signed (-32768 to 32767),
	 32-bit floating-point samples range from -1 to +1 (not clipped, so can actually be outside this range).
	That's unless the BASS_DATA_FLOAT flag is used, in which case, the sample data will be converted to 32-bit
	floating-point (if it isn't already).
	</p>
	<p>
	When requesting FFT data, floating-point values ranging from 0 to 1 are returned. Only the first half of the
	FFT is useful, so that's what BASS returns. For example, with a 2048 sample FFT, it will return 1024 values;
	the 1st value being the DC component, the 2nd being the amplitude at 1/2048 of the channel's sample rate, then
	the amplitude at 2/2048, 3/2048, etc... A Hann window is applied to the sample data to reduce leakage, unless
	the BASS_DATA_FFT_NOWINDOW flag is used.
	</p>
	<p>
	Channels that have 2 or more sample channels (ie. stereo or above) may have FFT performed on each individual
	channel, using the BASS_DATA_FFT_INDIVIDUAL flag. Without this flag, all the channels are combined, and a
	single mono FFT is performed. Performing the extra individual FFTs of course increases the amount of processing
	required. The return values are interleaved in the same order as the channel's sample data, eg. stereo = left,
	right,left,etc...
	</p>
	<p>
	This method is most useful if you wish to visualize (eg. spectrum analyze) the sound.
	</p>
	End Rem
	Method GetData:Int(buffer:Byte Ptr, length:Int)
		Return BASS_ChannelGetData(handle, buffer, length)
	End Method
	
	Rem
	bbdoc: Retrieves the device that a channel is using.
	returns: If successful, the device number is returned, else -1 is returned. Use BASS_ErrorGetCode to get the error code.
	End Rem
	Method GetDevice:Int()
		Return BASS_ChannelGetDevice(handle)
	End Method
	
	Rem
	bbdoc: Retrieves information on a channel. 
	End Rem
	Method GetInfo:TBassChannelInfo()
		Return TBassChannelInfo._create(bmx_bass_getchannelinfo(handle))
	End Method
	
	Rem
	bbdoc: Locks a stream, MOD music or recording channel to the current thread.
	about: Locking a channel prevents other threads from performing most functions on it, including buffer updates.
	Other threads wanting to access a locked channel will block until it is unlocked, so a channel should only be
	locked very briefly. A channel must be unlocked in the same thread that it was locked.
	End Rem
	Method Lock:Int(value:Int)
		Return BASS_ChannelLock(handle, value)
	End Method
	
	Rem
	bbdoc: Translates a time (seconds) position into bytes, based on a channel's format.
	returns: If successful, then the translated length is returned, else -1 is returned. Use BASS_ErrorGetCode to get the error code.
	about: The translation is based on the channel's initial sample rate, when it was created.
	<p>
	The return value is rounded down to the position of the nearest sample.
	</p>
	End Rem
	Method Seconds2Bytes:Long(pos:Double)
		Local bytes:Long
		bmx_bass_channelseconds2bytes(handle, Varptr bytes, pos)
		Return bytes
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetSync(stype:Int, param:Long, syncData:TBassSyncData)
		bmx_bass_setsync(handle, stype, param, syncData.dataPtr, Varptr syncData.channel, Varptr syncData.data, Varptr syncData.set)
	End Method
	
	Rem
	bbdoc: Sets the playback position of a sample, MOD music, or stream. 
	about: Setting the position of a MOD music in bytes (other than 0) requires that the BASS_MUSIC_PRESCAN flag was
	used in the BASS_MusicLoad call. When setting the position in orders and rows, the channel's byte position (as
	reported by BASS_ChannelGetPosition) is reset to 0. This is because it is not possible to get the byte position
	of an order/row position; it is possible that a position may never be played in the normal course of events, or
	it may be played multiple times. 
	<p>
	When setting the position of a MOD music, and the BASS_MUSIC_POSRESET flag is active, all notes that were playing
	before the position changed will be stopped. Otherwise, the notes will continue playing until they are stopped in
	the MOD music. When setting the position in bytes, the BPM, speed and global volume are updated to what they would
	normally be at the new position. Otherwise they are left as they were prior to the position change, unless the seek
	position is 0 (the start), in which case they are also reset to the starting values (with the BASS_MUSIC_POSRESET
	flag). When the BASS_MUSIC_POSRESETEX flag is active, the BPM, speed and global volume are reset with every seek.
	</p>
	<p>
	For MP3/MP2/MP1 streams, if the BASS_STREAM_PRESCAN flag was used when BASS_StreamCreateFile was called to create the
	stream, seeking will be accurate to the exact requested byte. Otherwise it will be an approximation, which is generally
	still quite accurate. 
	</p>
	<p>
	Seeking in internet file (and "buffered" user file) streams is possible once the download has reached the requested
	position, so long as the file is not being streamed in blocks (BASS_STREAM_BLOCK flag). 
	</p>
	End Rem
	Method SetPosition:Int(pos:Long, mode:Int)
		Return BASS_ChannelSetPosition(handle, pos, mode)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBassSyncData

	Field dataPtr:Byte Ptr
	Field channel:Int
	Field data:Int
	Field set:Int
	Field user:Object
	
	Method New()
		dataPtr = bmx_bass_syncdata_new()
	End Method
	
	Method isSet:Int()
		If set Then
			set = False
			Return True
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TBassFX

	Field handle:Int
	Field fxType:Int

	Function _create:TBassFX(handle:Int, fxType:Int)
		If handle Then
			Local this:TBassFX = New TBassFX
			this.handle = handle
			this.fxType = fxType
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Reset:Int()
		Return BASS_FXReset(handle)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetParameters:TBassFXType()
		Local fx:TBassFXType
		
		Local factory:TBassFXFactory = fx_factories

		While factory
			fx = factory.FXForType(fxType)
			If fx Exit
			factory = factory._succ
		Wend

		Return fx
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetParameters:Int(fx:TBassFXType)
		Return BASS_FXSetParameters(handle, fx.fxPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBassFXType

	Field fxPtr:Byte Ptr

	Method Delete()
		If fxPtr Then
			bmx_bass_fxdelete(fxPtr)
			fxPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TBassDX8Chorus Extends TBassFXType
End Type


Rem
bbdoc: 
End Rem
Type TBassStream Extends TBassChannel

	Field _stream:TStream
	Field callback:Int(handle:TBassStream, buffer:Byte Ptr, length:Int, user:Object)
	Field userData:Object
	Field dlcallback(buffer:Byte Ptr, length:Int, user:Object)

	Function _create:TBassStream(handle:Int)
		If handle Then
			Local this:TBassStream = New TBassStream
			this.handle = handle
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function StreamCreate:TBassStream(freq:Int, chans:Int, flags:Int, proc:Int(handle:TBassStream, buffer:Byte Ptr, length:Int, user:Object), user:Object)
		Return New TBassStream.Create(freq, chans, flags, proc, user)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TBassStream(freq:Int, chans:Int, flags:Int, proc:Int(handle:TBassStream, buffer:Byte Ptr, length:Int, user:Object), user:Object)
		callback = proc
		userData = user
		handle = BASS_StreamCreate(freq, chans, flags, _streamcallback, Self)
		Return Self
	End Method
	
	Function _streamcallback:Int(handle:Int, buffer:Byte Ptr, length:Int, data:Object)
		Return TBassStream(data).callback(TBassStream(data), buffer, length, TBassStream(data).userData)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function StreamCreateFile:TBassStream(filename:String, offset:Int, length:Int, flags:Int)
		Return New TBassStream.CreateFile(filename, offset, length, flags)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method CreateFile:TBassStream(filename:String, offset:Long, length:Long, flags:Int)
		Local s:Byte Ptr = filename.ToCString()
		handle = BASS_StreamCreateFile(False, s, offset, length, flags)
		MemFree(s)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method

	Rem
	bbdoc: Creates a sample stream from an MP3, MP2, MP1, OGG, WAV, AIFF or plugin supported file in memory.
	End Rem
	Function StreamCreateMem:TBassStream(mem:Byte Ptr, length:Long, flags:Int)
		Return New TBassStream.CreateMem(mem, length, flags)
	End Function

	Rem
	bbdoc: Creates a sample stream from an MP3, MP2, MP1, OGG, WAV, AIFF or plugin supported file in memory.
	returns: If successful, the new stream's handle is returned, else 0 is returned. Use TBass.ErrorGetCode to get the error code.
	about: Parameters: 
	<ul>
	<li><b>mem</b> : Memory Location</li>
	<li><b>length</b> : Length of sample in memory.</li>
	<li><b>flags</b> : Any combination of these flags.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_SAMPLE_FLOAT</td><td>Use 32-bit floating-point sample data. See Floating-point channels for info. </td></tr>
	<tr><td>BASS_SAMPLE_MONO</td><td>Decode/play the stream (MP3/MP2/MP1 only) in mono, reducing the CPU usage (if
	it was originally stereo). This flag is automatically applied if BASS_DEVICE_MONO was specified when calling
	BASS_Init. </td></tr>
	<tr><td>BASS_SAMPLE_SOFTWARE</td><td>Force the stream to not use hardware mixing. </td></tr>
	<tr><td>BASS_SAMPLE_3D</td><td>Enable 3D functionality. This requires that the BASS_DEVICE_3D flag was
	specified when calling BASS_Init, and the stream must be mono. The SPEAKER flags can not be used together with
	this flag. </td></tr>
	<tr><td>BASS_SAMPLE_LOOP</td><td>Loop the file. This flag can be toggled at any time using BASS_ChannelFlags. </td></tr>
	<tr><td>BASS_SAMPLE_FX</td><td>Enable the old implementation of DirectX 8 effects. See the DX8 effect
	implementations section for details. Use BASS_ChannelSetFX to add effects to the stream. </td></tr>
	<tr><td>BASS_STREAM_PRESCAN</td><td>Enable pin-point accurate seeking (to the exact byte) on the MP3/MP2/MP1
	stream. This also increases the time taken to create the stream, due to the entire file being pre-scanned for
	the seek points. </td></tr>
	<tr><td>BASS_STREAM_AUTOFREE</td><td>Automatically free the stream when playback ends. </td></tr>
	<tr><td>BASS_STREAM_DECODE</td><td>Decode the sample data, without playing it. Use BASS_ChannelGetData to
	retrieve decoded sample data. The BASS_SAMPLE_3D, BASS_STREAM_AUTOFREE and SPEAKER flags can not be used
	together with this flag. The BASS_SAMPLE_SOFTWARE and BASS_SAMPLE_FX flags are also ignored. </td></tr>
	<tr><td>BASS_SPEAKER_xxx</td><td>Speaker assignment flags. These flags have no effect when the stream is more
	than stereo. </td></tr>
	<tr><td>BASS_UNICODE</td><td>file is a Unicode (UTF-16) filename. </td></tr>
	</table>
	</li>
	</ul>
	<p>
	BASS has built-in support for MPEG, OGG, WAV and AIFF files. Support for additional formats is available via
	add-ons, which can be downloaded from the BASS website: http://www.un4seen.com/
	</p>
	<p>
	MPEG 1.0, 2.0 and 2.5 layer 3 (MP3) files are supported, layers 1 (MP1) and 2 (MP2) are also supported. WAV
	files can be in standard PCM format or they can be compressed with any ACM codec, but the codec is required to
	be installed on the user's computer for the WAV to be decoded. So you should either distribute the codec with
	your software, or use a codec that comes with Windows (eg. Microsoft ADPCM). All PCM formats from 8 to 32-bit
	are supported in WAV and AIFF files, but the output will be restricted to 16-bit unless the BASS_SAMPLE_FLOAT
	flag is used. The file's original resolution is available via #GetInfo.
	</p>
	<p>
	Multi-channel (ie. more than stereo) OGG, WAV and AIFF files are supported.
	</p>
	<p>
	Use #GetInfo to retrieve information on the format (sample rate, resolution, channels) of the
	stream. The playback length of the stream can be retrieved using #GetLength.
	</p>
	<p>
	When streaming from memory, the memory generally must not be freed before the stream is.
	</p>
	<p>
	<b>OSX notes</b> - Filenames are in UTF-8 form, and the BASS_UNICODE flag has no effect. All mixing is done in
	software (by BASS), so the BASS_SAMPLE_SOFTWARE flag has no effect. Compressed WAV files are not supported,
	only standard PCM are.
	</p>
	End Rem
	Method CreateMem:TBassStream(mem:Byte Ptr, length:Long, flags:Int)
		handle = BASS_StreamCreateFile(True, mem, 0, length, flags)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function StreamCreateFileUser:TBassStream()
	End Function

	Rem
	bbdoc: 
	End Rem
	Method CreateFileUser:TBassStream()
	End Method
	
	Rem
	bbdoc: Creates a sample stream from an MP3, MP2, MP1, OGG, WAV, AIFF or plugin supported file on the internet, optionally receiving the downloaded data in a callback function. 
	End Rem
	Function StreamCreateURL:TBassStream(url:String, offset:Int, flags:Int, proc(buffer:Byte Ptr, length:Int, user:Object), data:Object)
		Return New TBassStream.CreateURL(url, offset, flags, proc, data)
	End Function
	
	Rem
	bbdoc: Creates a sample stream from an MP3, MP2, MP1, OGG, WAV, AIFF or plugin supported file on the internet, optionally receiving the downloaded data in a callback function. 
	End Rem
	Method CreateURL:TBassStream(url:String, offset:Int, flags:Int, proc(buffer:Byte Ptr, length:Int, data:Object), user:Object)
		userData = user
		Local s:Byte Ptr = url.ToCString()
		If proc Then
			dlcallback = proc
			handle = bmx_bass_streamcreateurl(s, offset, flags, Self)
		Else
			' a bit hacky... wouldn't let me pass NULL into the above call... so we do it in the glue instead...
			handle = bmx_bass_streamcreateurlncb(s, offset, flags, Self)
		End If
		MemFree(s)
		Return Self
	End Method
	
	Function _dlstreamcallback(buffer:Byte Ptr, length:Int, data:Object)
		TBassStream(data).dlcallback(buffer, length, TBassStream(data).userData)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function StreamCreateTStream:TBassStream(stream:TStream, system:Int, flags:Int)
		Return New TBassStream.CreateTStream(stream, system, flags)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method CreateTStream:TBassStream(stream:TStream, system:Int, flags:Int)
		_stream = stream
		handle = bmx_bass_streamcreatetstream(Self, system, flags)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method
	
	' callback wrapper for TStream
	Function _fileCloseProc(stream:TBassStream)
		stream._stream.Close()
	End Function
	
	' callback wrapper for TStream
	Function _fileLenProc(stream:TBassStream, length:Long Ptr)
		length[0] = Long(stream._stream.Size())
	End Function
	
	' callback wrapper for TStream
	Function _fileReadProc:Int(buffer:Byte Ptr, length:Int, stream:TBassStream)
		Return stream._stream.Read(buffer, length)
	End Function
	
	' callback wrapper for TStream
	Function _fileSeekProc:Int(offset:Long, stream:TBassStream)
		stream._stream.Seek(Int(offset))
		Return 1 ' TODO for now, we always return true...
	End Function
	
	Rem
	bbdoc: Retrieves the file position/status of a stream.
	End Rem
	Method GetFilePosition:Long(mode:Int)
		Local pos:Long
		bmx_bass_streamgetfileposition(handle, Varptr pos, mode)
		Return pos
	End Method

	Rem
	bbdoc: 
	End Rem
	Method PutData(buffer:Byte Ptr, length:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method PutFileData(buffer:Byte Ptr, length:Int)
	End Method

	Rem
	bbdoc: Frees a sample stream's resources, including any sync/DSP/FX it has.
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	End Rem
	Method Free:Int()
		Return BASS_StreamFree(handle)
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type TBassSample

	Field handle:Int

	Function _create:TBassSample(handle:Int)
		If handle Then
			Local this:TBassSample = New TBassSample
			this.handle = handle
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new sample.
	about: 
	End Rem
	Function SampleCreate:TBassSample(length:Int, freq:Int, channels:Int, maxPlaybacks:Int, flags:Int)
		Return New TBassSample.Create(length, freq, channels, maxPlaybacks, flags)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TBassSample(length:Int, freq:Int, channels:Int, maxPlaybacks:Int, flags:Int)
		Return TBassSample._create(BASS_SampleCreate(length, freq, channels, maxPlaybacks, flags))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function SampleFileLoad:TBassSample(file:String, offset:Long, length:Int, maxPlaybacks:Int, flags:Int)
		Return New TBassSample.FileLoad(file, offset, length, maxPlaybacks, flags)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method FileLoad:TBassSample(file:String, offset:Long, length:Int, maxPlaybacks:Int, flags:Int)
		Local s:Byte Ptr = file.ToCString()
		handle = BASS_SampleLoad(False, s, offset, length, maxPlaybacks, flags)
		MemFree(s)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function SampleMemLoad:TBassSample(mem:Byte Ptr, length:Int, maxPlaybacks:Int, flags:Int)
		Return New TBassSample.MemLoad(mem, length, maxPlaybacks, flags)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method MemLoad:TBassSample(mem:Byte Ptr, length:Int, maxPlaybacks:Int, flags:Int)
		handle = BASS_SampleLoad(True, mem, 0, length, maxPlaybacks, flags)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: Frees a sample's resources. 
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	End Rem
	Method Free:Int()
		Return BASS_SampleFree(handle)
	End Method
	
	Rem
	bbdoc: Creates/initializes a playback channel for a sample. 
	End Rem
	Method GetChannel:TBassChannel(onlyNew:Int)
		Return TBassChannel._create(BASS_SampleGetChannel(handle, onlyNew))
	End Method
	
	Rem
	bbdoc: Stops all instances of a sample.
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: If a sample is playing simultaneously multiple times, calling this function will stop them all,
	which is obviously simpler than calling BASS_ChannelStop multiple times. 
	End Rem
	Method Stop:Int()
		Return BASS_SampleStop(handle)
	End Method

	Rem
	bbdoc: Retrieves a sample's default attributes and other information. 
	End Rem
	Method GetInfo:TBassSampleInfo()
		Return TBassSampleInfo._create(bmx_bass_getsampleinfo(handle))
	End Method
	
	Rem
	bbdoc: Retrieves a copy of a sample's data. 
	End Rem
	Method GetData:Int(buffer:Byte Ptr)
		Return BASS_SampleGetData(handle, buffer)
	End Method
	
	Rem
	bbdoc: Sets a sample's data. 
	End Rem
	Method SetData:Int(buffer:Byte Ptr)
		Return BASS_SampleSetData(handle, buffer)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetChannels:TBassSample[]()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TBassSampleInfo

	Field sampleinfoPtr:Byte Ptr
	
	Function _create:TBassSampleInfo(sampleinfoPtr:Byte Ptr)
		If sampleinfoPtr Then
			Local this:TBassSampleInfo = New TBassSampleInfo
			this.sampleinfoPtr = sampleinfoPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Default sample rate. 
	End Rem
	Method GetFreq:Int()
	   Return bmx_sampleinfo_getfreq(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Default volume... 0 (silent) to 1 (full).
	End Rem
	Method GetVolume:Float()
	   Return bmx_sampleinfo_getvolume(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Default panning position... -1 (full left) to +1 (full right), 0 = centre. 
	End Rem
	Method GetPan:Float()
	   Return bmx_sampleinfo_getpan(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetFlags:Int()
	   Return bmx_sampleinfo_getflags(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The length in bytes.
	End Rem
	Method GetLength:Int()
	   Return bmx_sampleinfo_getlength(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Maximum number of simultaneous playbacks. 
	End Rem
	Method GetMax:Int()
	   Return bmx_sampleinfo_getmax(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The original resolution (bits per sample)... 0 = undefined.
	End Rem
	Method GetOrigRes:Int()
	   Return bmx_sampleinfo_getorigres(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Number of channels... 1 = mono, 2 = stereo, etc...
	End Rem
	Method GetChans:Int()
	   Return bmx_sampleinfo_getchans(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Minimum time gap in milliseconds between creating channels using TBassSample.GetChannel.
	about: This can be used to prevent flanging effects caused by playing a sample multiple times very close to
	each other. The default setting, after loading/creating a sample, is 0 (disabled).
	End Rem
	Method GetMinGap:Int()
	   Return bmx_sampleinfo_getmingap(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMode3D:Int()
	   Return bmx_sampleinfo_getmode3d(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The minimum distance.
	about: The sample's volume is at maximum when the listener is within this distance.
	End Rem
	Method GetMinDist:Float()
	   Return bmx_sampleinfo_getmindist(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The maximum distance.
	about: The sample's volume stops decreasing when the listener is beyond this distance. 
	End Rem
	Method GetMaxDist:Float()
	   Return bmx_sampleinfo_getmaxdist(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The angle of the inside projection cone in degrees... 0 (no cone) to 360 (sphere). 
	End Rem
	Method GetIAngle:Int()
	   Return bmx_sampleinfo_getiangle(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The angle of the outside projection cone in degrees... 0 (no cone) to 360 (sphere). 
	End Rem
	Method GetOAngle:Int()
	   Return bmx_sampleinfo_getoangle(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: The delta-volume outside the outer projection cone... 0 (silent) to 1 (full). 
	End Rem
	Method GetOutVol:Float()
	   Return bmx_sampleinfo_getoutvol(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetVAM:Int()
	   Return bmx_sampleinfo_getvam(sampleinfoPtr)
	End Method
	
	Rem
	bbdoc: Priority, used with the BASS_VAM_TERM_PRIO flag... 0 (min) to $FFFFFFFF (max). 
	End Rem
	Method GetPriority:Int()
	   Return bmx_sampleinfo_getpriority(sampleinfoPtr)
	End Method
	
	Method Delete()
		If sampleinfoPtr Then
			bmx_sampleinfo_delete(sampleinfoPtr)
			sampleinfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A Mod music file : MO3 / IT / XM / S3M / MTM / MOD / UMX formats.
End Rem
Type TBassMusic Extends TBassChannel
	
	Rem
	bbdoc: Loads a MOD music file; MO3 / IT / XM / S3M / MTM / MOD / UMX formats. 
	returns: If successful, the loaded MOD music's handle is returned, else Null is returned. Use TBass.ErrorGetCode to get the error code.
	about: Parameters: 
	<ul>
	<li><b>file</b> : Filename</li>
	<li><b>offset</b> : File offset to load the MOD music from</li>
	<li><b>length</b> : Data length... 0 = use all data up to the end of file</li>
	<li><b>flags</b> : A combination of these flags.
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_SAMPLE_8BITS</td><td>Use 8-bit resolution. If neither this or the BASS_SAMPLE_FLOAT flags are specified, then the sample data will be 16-bit. </td></tr>
	<tr><td>BASS_SAMPLE_FLOAT</td><td>Use 32-bit floating-point sample data. See Floating-point channels for info.</td></tr>
	<tr><td>BASS_SAMPLE_MONO</td><td>Decode/play the MOD music in mono (uses less CPU than stereo). This flag is automatically applied if BASS_DEVICE_MONO was specified when calling BASS_Init. </td></tr>
	<tr><td>BASS_SAMPLE_SOFTWARE</td><td>Force the MOD music to not use hardware mixing.</td></tr>
	<tr><td>BASS_SAMPLE_3D</td><td>Enable 3D functionality. This requires that the BASS_DEVICE_3D flag was specified when calling BASS_Init. 3D channels must also be mono, so BASS_SAMPLE_MONO is automatically applied. The SPEAKER flags can not be used together with this flag. 
</td></tr>
	<tr><td>BASS_SAMPLE_FX</td><td>Enable the old implementation of DirectX 8 effects. See the DX8 effect implementations section for details. Use BASS_ChannelSetFX to add effects to the music. 
</td></tr>
	<tr><td>BASS_SAMPLE_LOOP</td><td>Loop the music. </td></tr>
	<tr><td>BASS_MUSIC_NONINTER</td><td>Use non-interpolated sample mixing. This generally reduces the sound quality, but can be good for chip-tunes. </td></tr>
	<tr><td>BASS_MUSIC_SINCINTER</td><td>Use sinc interpolated sample mixing. This increases the sound quality, but also requires more processing. If neither this or the BASS_MUSIC_NONINTER flag is specified, linear interpolation is used. </td></tr>
	<tr><td>BASS_MUSIC_RAMP</td><td>Use "normal" ramping (as used in FastTracker 2).</td></tr>
	<tr><td>BASS_MUSIC_RAMPS</td><td>Use "sensitive" ramping.</td></tr>
	<tr><td>BASS_MUSIC_SURROUND</td><td>Apply XMPlay's surround sound to the music (ignored in mono). </td></tr>
	<tr><td>BASS_MUSIC_SURROUND2</td><td>Apply XMPlay's surround sound mode 2 to the music (ignored in mono).</td></tr>
	<tr><td>BASS_MUSIC_FT2MOD</td><td>Play .MOD file as FastTracker 2 would. </td></tr>
	<tr><td>BASS_MUSIC_PT1MOD</td><td>Play .MOD file as ProTracker 1 would.</td></tr>
	<tr><td>BASS_MUSIC_POSRESET</td><td>Stop all notes when seeking (BASS_ChannelSetPosition).</td></tr>
	<tr><td>BASS_MUSIC_POSRESETEX</td><td>Stop all notes and reset bpm/etc when seeking.</td></tr>
	<tr><td>BASS_MUSIC_STOPBACK</td><td>Stop the music when a backward jump effect is played. This stops musics that never reach the end from going into endless loops. Some MOD musics are designed to jump all over the place, so this flag would cause those to be stopped prematurely. If this flag is used together with the BASS_SAMPLE_LOOP flag, then the music would not be stopped but any BASS_SYNC_END sync would be triggered.</td></tr>
	<tr><td>BASS_MUSIC_PRESCAN</td><td>Calculate the playback length of the music, and enable seeking in bytes. This slightly increases the time taken to load the music, depending on how long it is. In the case of musics that loop, the length until the loop occurs is calculated. Use BASS_ChannelGetLength to retrieve the length. </td></tr>
	<tr><td>BASS_MUSIC_NOSAMPLE</td><td>Don't load the samples. This reduces the time (and memory) taken to load the music, notably with MO3 files, which is useful if you just want to get the text and/or length of the music without playing it.</td></tr>
	<tr><td>BASS_MUSIC_AUTOFREE</td><td>Automatically free the music when playback ends. Note that some musics have infinite loops, so never actually end on their own. </td></tr>
	<tr><td>BASS_MUSIC_DECODE</td><td>Decode/render the sample data, without playing it. Use BASS_ChannelGetData to retrieve decoded sample data. The BASS_SAMPLE_3D, BASS_STREAM_AUTOFREE and SPEAKER flags can not be used together with this flag. The BASS_SAMPLE_SOFTWARE and BASS_SAMPLE_FX flags are also ignored.</td></tr>
	<tr><td>BASS_SPEAKER_xxx</td><td>Speaker assignment flags. The BASS_SAMPLE_MONO flag is automatically applied when using a mono speaker assignment flag.</td></tr>
	<tr><td>BASS_UNICODE</td><td>file is a Unicode (UTF-16) filename. </td></tr>
	</table>
	</li>
	<li><b>freq</b> : Sample rate to render/play the MOD music at... 0 = the rate specified in the TBass.Init call.</li>
	</ul>
	<p>
	BASS uses the same code as XMPlay for its MOD music support, giving the most accurate reproduction of
	MO3 / IT / XM / S3M / MTM / MOD / UMX files available.
	<p>
	MO3s are treated and used in exactly the same way as normal MOD musics. The advantage of MO3s is that they
	can be a lot smaller with virtually identical quality. Playing a MO3 does not use any more CPU power than
	playing the original MOD version does. The only difference is a slightly longer load time as the samples
	are being decoded. MO3 files are created using the MO3 encoder available at the BASS website. 
	</p>
	<p>
	DMO effects (the same as available with BASS_ChannelSetFX) can be used in IT and XM files (and MO3 versions
	of them) created with Modplug Tracker. This allows effects to be added to a track without having to resort
	to an MP3 or OGG version, so it can remain small and still sound fancy. Of course, the effects require some
	CPU, so should not be used carelessly if performance is key. DirectX 8 (or above) is required for the
	effects to be heard; without that, the music can still be played, but the effects are disabled. 
	</p>
	<p>
	Ramping doesn't take a lot of extra processing and improves the sound quality by removing "clicks".
	Sensitive ramping leaves sharp attacked samples, while normal ramping can cause them to lose a bit of their
	impact. Generally, normal ramping is recommended for XMs, and sensitive ramping for the other formats. But,
	some XMs may also sound better using sensitive ramping. 
	</p>
	<p>
	After loading a MOD music from memory (mem = TRUE), the memory can safely be discarded.
	</p>
	<p>
	<b>OSX notes</b> - Filenames are in UTF-8 form, and the BASS_UNICODE flag has no effect. All mixing is done
	in software (by BASS), so the BASS_SAMPLE_SOFTWARE flag has no effect.
	</p>
	End Rem
	Function MusicFileLoad:TBassMusic(file:String, offset:Long, length:Int, flags:Int, freq:Int)
		Return New TBassMusic.FileLoad(file, offset, length, flags, freq)
	End Function 
	
	Rem
	bbdoc: Loads a MOD music file; MO3 / IT / XM / S3M / MTM / MOD / UMX formats. 
	returns: If successful, the loaded MOD music's handle is returned, else Null is returned. Use TBass.ErrorGetCode to get the error code.
	End Rem
	Method FileLoad:TBassMusic(file:String, offset:Long, length:Int, flags:Int, freq:Int)
		Local s:Byte Ptr = file.ToCString()
		handle = BASS_MusicLoad(False, s, offset, length, flags, freq)
		MemFree(s)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method

	Rem
	bbdoc: Loads a MOD music file from memory; MO3 / IT / XM / S3M / MTM / MOD / UMX formats.
	returns: If successful, the loaded MOD music's handle is returned, else Null is returned. Use TBass.ErrorGetCode to get the error code.
	End Rem
	Function MusicMemLoad:TBassMusic(mem:Byte Ptr, length:Int, flags:Int, freq:Int)
		Return New TBassMusic.MemLoad(mem, length, flags, freq)
	End Function 

	Rem
	bbdoc: Loads a MOD music file from memory; MO3 / IT / XM / S3M / MTM / MOD / UMX formats. 
	returns: If successful, the loaded MOD music's handle is returned, else Null is returned. Use TBass.ErrorGetCode to get the error code.
	End Rem
	Method MemLoad:TBassMusic(mem:Byte Ptr, length:Int, flags:Int, freq:Int)
		handle = BASS_MusicLoad(True, mem, 0, length, flags, freq)
		If Not handle Then
			Return Null
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: Frees a MOD music's resources, including any sync/DSP/FX it has. 
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	End Rem
	Method Free:Int()
		Return BASS_MusicFree(handle)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBassChannelInfo

	Field channelinfoPtr:Byte Ptr
	
	Function _create:TBassChannelInfo(channelinfoPtr:Byte Ptr)
		If channelinfoPtr Then
			Local this:TBassChannelInfo = New TBassChannelInfo
			this.channelinfoPtr = channelinfoPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetFreq:Int()
	   Return bmx_channelinfo_getfreq(channelinfoPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetChannels:Int()
	   Return bmx_channelinfo_getchannels(channelinfoPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetFlags:Int()
	   Return bmx_channelinfo_getflags(channelinfoPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetCType:Int()
	   Return bmx_channelinfo_getctype(channelinfoPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetOrigRes:Int()
	   Return bmx_channelinfo_getorigres(channelinfoPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetSample:TBassSample()
	   Return TBassSample._create(bmx_channelinfo_getsample(channelinfoPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetFilename:String()
	   Return String.FromCString(bmx_channelinfo_getfilename(channelinfoPtr))
	End Method

	Method Delete()
		If channelinfoPtr Then
			bmx_channelinfo_delete(channelinfoPtr)
			channelinfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBass

	Rem
	bbdoc: Frees all resources used by the output device, including all its samples, streams and MOD musics. 
	End Rem
	Function Free:Int()
		Return BASS_free()
	End Function

	Rem
	bbdoc: Initializes an output device. 
	returns: If the device was successfully initialized, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: This function must be successfully called before using any sample, stream or MOD music functions. The
	recording functions may be used without having called this function. 
	<p>
	The "no sound" device allows loading and playing of MOD musics, but most other functions will fail. This is
	so that you can still use MOD musics as synchronizers when there is no soundcard present (or without DirectX
	installed). Decoding channels can also still be used with the "no sound" device. When using the "no sound"
	device, you should still set the other parameters as you would do normally. 
	</p>
	<p>
	The format (sample rate, etc...) specified when calling this function only actually has effect on the
	device's output with VxD drivers. With WDM drivers, the output format is automatically set depending on the
	format of the channels that are played and what the device supports. While in Vista, the output format is
	determined by the user's choice in the control panel; BASS_GetInfo can be used to check what that is. For
	reference... Windows XP/2000 use only WDM drivers. Windows Me/98SE can use either WDM or the older VxD
	driver model. Windows 98/95 only use VxD drivers. 
	</p>
	<p>
	When specifying a class identifier (clsid), after successful initialization, you can use BASS_GetDSoundObject
	to retrieve the DirectSound object, and through that access any special interfaces that the object may
	provide. 
	</p>
	<p>
	Simultaneously using multiple devices is supported in the BASS API via a context switching system; instead
	of there being an extra "device" parameter in the function calls, the device to be used is set prior to
	calling the functions. #SetDevice is used to switch the current device. When successful, BASS_Init
	automatically sets the current thread's device to the one that was just initialized. 
	</p>
	<p>
	When using the default device (device = -1), #GetDevice can be used to find out which device it was
	mapped to.
	</p>
	<p>
	<b>OSX notes</b> : The win and clsid parameters are not used. The BASS_DEVICE_LATENCY, BASS_DEVICE_CPSPEAKERS
	and BASS_DEVICE_SPEAKERS flags are also ignored; the latency is calculated automatically and the number of
	speakers is always detected accurately. The sample format specified has no effect on the device output;
	BASS_GetInfo can be used to check what that is.
	</p>
	End Rem
	Function Init:Int(device:Int, freq:Int, flags:Int, win:Byte Ptr = Null, clsid:Byte Ptr = Null)
		Return BASS_Init(device, freq, flags, win, clsid)
	End Function
	
	Rem
	bbdoc: Retrieves the error code for the most recent BASS function call in the current thread. 
	End Rem
	Function ErrorGetCode:Int()
		Return BASS_ErrorGetCode()
	End Function
	
	Rem
	bbdoc: Retrieves the current master volume level.
	returns: If successful, the volume level is returned, else -1 is returned. Use #BASS_ErrorGetCode to get the error code. 
	End Rem
	Function GetVolume:Float()
		Return BASS_GetVolume()
	End Function

	Rem
	bbdoc: Sets the output master volume. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: The actual volume level may not be exactly the same as requested, due to underlying precision
	differences. BASS_GetVolume can be used to confirm what the volume is. 
	<p>
	This function affects the volume level of all applications using the same output device. If you wish to only
	affect the level of your application's sounds, the BASS_ATTRIB_VOL attribute and/or the
	BASS_CONFIG_GVOL_MUSIC / BASS_CONFIG_GVOL_SAMPLE / BASS_CONFIG_GVOL_STREAM config options should be used
	instead. 
	</p>
	<p>
	When using multiple devices, the current thread's device setting (as set with BASS_SetDevice) determines
	which device this function call applies to.
	</p>
	End Rem	
	Function SetVolume:Int(volume:Float)
		Return BASS_SetVolume(volume)
	End Function
	
	Rem
	bbdoc: Sets the device to use for subsequent calls.
	returns: If successful, then TRUE is returned, else FALSE is returned. Use #BASS_ErrorGetCode to get the error code. 
	about: Parameters: 
	<ul>
	<li><b>device</b> : The device to use... 0 = no sound, 1 = first real output device.</li>
	</ul>
	<p>
	Simultaneously using multiple devices is supported in the BASS API via a context switching system;
	instead of there being an extra "device" parameter in the function calls, the device to be used is set
	prior to calling the functions.
	<p>
	</p>
	The functions that use the device selection are the following: #Free, #GetDSoundObject,
	#GetInfo, #Start, #Stop, #Pause, #SetVolume, #GetVolume, #Set3DFactors,
	#Get3DFactors, #Set3DPosition, #Get3DPosition, #SetEAXParameters, #GetEAXParameters.
	It also determines which device is used by a new sample/stream/music: BASS_MusicLoad, BASS_SampleLoad,
	BASS_StreamCreateFile, etc... 
	<p>
	</p>
	When one of the above functions (or #GetDevice) is called, BASS will check the
	device setting, and if no device is selected (or the selected device is not initialized), BASS will
	automatically select the lowest device that is initialized. This means that when using a single device,
	there is no need to use this function; BASS will automatically use the device that's initialized. Even
	if you free the device, and initialize another, BASS will automatically switch to the one that is
	initialized.
	</p>
	End Rem
	Function SetDevice:Int(device:Int)
		Return BASS_SetDevice(device)
	End Function
	
	Rem
	bbdoc: Retrieves the device setting of the current thread.
	returns: If successful, the device number is returned, else -1 is returned. Use TBass.ErrorGetCode to get the error code.
	End Rem
	Function GetDevice:Int()
		Return BASS_GetDevice()
	End Function
	
	Rem
	bbdoc: Retrieves the current CPU usage of BASS.
	returns: The BASS CPU usage as a percentage of total CPU time.
	about: This function includes the time taken to render stream (TBassStream) and MOD music (TBassMusic) channels
	during playback, and any DSP functions set on those channels. It does not include sample (TBassSample)
	channels, which are mixed by the output device (hardware accelerated) or DirectSound/drivers. 
	<p>
	The rendering of some add-on stream formats may not be entirely included, if they use additional decoding
	threads. See the add-on documentation for details. 
	</p>
	<p>
	This function does not strictly tell the CPU usage, but rather how timely the buffer updates are. For
	example, if it takes 10ms to render 100ms of data, that would be 10%. If the reported usage gets to 100%,
	that means the channel data is being played faster than it can be rendered, and buffer underruns are likely
	to occur. 
	</p>
	<p>
	If automatic updating is disabled, then the value returned by this function is only updated after each call
	to Update. TBassChannel.Update usage is not included. 
	</p>
	<p><b>OSX Notes</b> : The CPU usage does include sample playback.</p>
	End Rem
	Function GetCPU:Float()
		Return BASS_GetCPU()
	End Function
	
	Rem
	bbdoc: Stops the output, pausing all musics/samples/streams.
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: Use #Start to resume the output and paused channels. 
	<p>
	When using multiple devices, the current thread's device setting (as set with SetDevice) determines
	which device this function call applies to. 
	</p>
	End Rem
	Function Pause:Int()
		Return BASS_Pause()
	End Function
	
	Rem
	bbdoc: Retrieves the version of BASS that is loaded. 
	returns: The BASS version. For example, 0x02040103 (hex), would be version 2.4.1.3 
	about: There is no guarantee that a previous or future version of BASS supports all the BASS functions that
	you are using, so you should always use this function to make sure the correct version is loaded. It is
	safe to assume that future revisions (indicated in the LOWORD) will be fully compatible. 
	<p>
	The BASS API includes a BASSVERSION constant, which can be used to check that the loaded BASS.DLL matches
	the API version used, ignoring revisions. 
	</p>
	End Rem
	Function GetVersion:Int()
		Return BASS_GetVersion()
	End Function
	
	Rem
	bbdoc: Manually updates the TBassStream and TBassMusic channel playback buffers.
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: Parameters: 
	<ul>
	<li><b>length</b> : The amount to render, in milliseconds. </li>
	</ul>
	When automatic updating is disabled, this function (or TBassChannel.Update) needs to be called to keep
	the playback buffers updated. The length parameter should include some safety margin, in case the next
	update cycle gets delayed. For example, if calling this function every 100ms, 200 would be a reasonable
	length parameter.
	End Rem
	Function Update:Int(length:Int)
		Return BASS_Update(length)
	End Function
	
	Rem
	bbdoc: Retrieves information on an output device. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code.
	about: This method can be used to enumerate the available devices for a setup dialog. Device 0 is always
	the "no sound" device, so if you should start at device 1 if you only want to list real devices. 
	End Rem
	Function GetDeviceInfo:TBassDeviceInfo(device:Int)
		Return TBassDeviceInfo._create(bmx_bass_getdeviceinfo(device))
	End Function
	
	Rem
	bbdoc: Retrieves the value of a config option.
	returns: If successful, the value of the requested config option is returned, else -1 is returned. Use TBass.ErrorGetCode to get the error code.
	about: Retrieves the value of a config option. 
	<p>Parameters: 
	<ul>
	<li><b>option</b> : 
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_CONFIG_3DALGORITHM</td><td>The 3D algorithm for software mixed 3D channels.</td></tr>
	<tr><td>BASS_CONFIG_BUFFER</td><td>Playback buffer length. </td></tr>
	<tr><td>BASS_CONFIG_CURVE_PAN</td><td>Panning translation curve. </td></tr>
	<tr><td>BASS_CONFIG_CURVE_VOL</td><td>Volume translation curve. </td></tr>
	<tr><td>BASS_CONFIG_FLOATDSP</td><td>Pass 32-bit floating-point sample data to all DSP functions?</td></tr>
	<tr><td>BASS_CONFIG_GVOL_MUSIC</td><td>Global MOD music volume. </td></tr>
	<tr><td>BASS_CONFIG_GVOL_SAMPLE</td><td>Global sample volume. </td></tr>
	<tr><td>BASS_CONFIG_GVOL_STREAM</td><td>Global stream volume. </td></tr>
	<tr><td>BASS_CONFIG_MUSIC_VIRTUAL</td><td>IT virtual channels. </td></tr>
	<tr><td>BASS_CONFIG_NET_BUFFER</td><td>Internet download buffer length.</td></tr>
	<tr><td>BASS_CONFIG_NET_PASSIVE</td><td>Use passive mode in FTP connections? </td></tr>
	<tr><td>BASS_CONFIG_NET_PLAYLIST</td><td>Process URLs in playlists?</td></tr>
	<tr><td>BASS_CONFIG_NET_PREBUF</td><td>Amount to pre-buffer when opening internet streams. </td></tr>
	<tr><td>BASS_CONFIG_NET_TIMEOUT</td><td>Time to wait for a server to respond to a connection request. </td></tr>
	<tr><td>BASS_CONFIG_PAUSE_NOPLAY</td><td>Prevent channels being played when the output is paused?</td></tr>
	<tr><td>BASS_CONFIG_REC_BUFFER</td><td>Recording buffer length.</td></tr>
	<tr><td>BASS_CONFIG_UPDATEPERIOD</td><td>Update period of playback buffers. </td></tr>
	<tr><td>BASS_CONFIG_UPDATETHREADS</td><td>Number of update threads.</td></tr>
	<tr><td>BASS_CONFIG_VERIFY</td><td>File format verification length. </td></tr>
	</table>
	</li>
	</ul>
	</p>
	End Rem
	Function GetConfig:Int(option:Int)
		Return BASS_GetConfig(option)
	End Function
	
	Rem
	bbdoc: Sets the value of a config option. 
	returns: If successful, TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: The option to set the value of... one of the following.
	<p>Parameters: 
	<ul>
	<li><b>option</b> : 
	<table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>BASS_CONFIG_3DALGORITHM</td><td>The 3D algorithm for software mixed 3D channels.</td></tr>
	<tr><td>BASS_CONFIG_BUFFER</td><td>Playback buffer length. </td></tr>
	<tr><td>BASS_CONFIG_CURVE_PAN</td><td>Panning translation curve. </td></tr>
	<tr><td>BASS_CONFIG_CURVE_VOL</td><td>Volume translation curve. </td></tr>
	<tr><td>BASS_CONFIG_FLOATDSP</td><td>Pass 32-bit floating-point sample data to all DSP functions?</td></tr>
	<tr><td>BASS_CONFIG_GVOL_MUSIC</td><td>Global MOD music volume. </td></tr>
	<tr><td>BASS_CONFIG_GVOL_SAMPLE</td><td>Global sample volume. </td></tr>
	<tr><td>BASS_CONFIG_GVOL_STREAM</td><td>Global stream volume. </td></tr>
	<tr><td>BASS_CONFIG_MUSIC_VIRTUAL</td><td>IT virtual channels. </td></tr>
	<tr><td>BASS_CONFIG_NET_BUFFER</td><td>Internet download buffer length.</td></tr>
	<tr><td>BASS_CONFIG_NET_PASSIVE</td><td>Use passive mode in FTP connections? </td></tr>
	<tr><td>BASS_CONFIG_NET_PLAYLIST</td><td>Process URLs in playlists?</td></tr>
	<tr><td>BASS_CONFIG_NET_PREBUF</td><td>Amount to pre-buffer when opening internet streams. </td></tr>
	<tr><td>BASS_CONFIG_NET_TIMEOUT</td><td>Time to wait for a server to respond to a connection request. </td></tr>
	<tr><td>BASS_CONFIG_PAUSE_NOPLAY</td><td>Prevent channels being played when the output is paused?</td></tr>
	<tr><td>BASS_CONFIG_REC_BUFFER</td><td>Recording buffer length.</td></tr>
	<tr><td>BASS_CONFIG_UPDATEPERIOD</td><td>Update period of playback buffers. </td></tr>
	<tr><td>BASS_CONFIG_UPDATETHREADS</td><td>Number of update threads.</td></tr>
	<tr><td>BASS_CONFIG_VERIFY</td><td>File format verification length. </td></tr>
	</table>
	</li>
	<li><b>value</b> : The new option setting. See the option's documentation for details on the possible values.</li>
	</ul>
	</p>
	End Rem
	Function SetConfig:Int(option:Int, value:Int)
		Return BASS_SetConfig(option, value)
	End Function
	
	Rem
	bbdoc: Applies changes made to the 3D system. 
	about: This function must be called to apply any changes made with TBass.Set3DFactors, TBass.Set3DPosition,
	TBassChannel.Set3DAttributes or TBassChannel.Set3DPosition. This allows multiple changes to be synchronized,
	and also improves performance. 
	<p>
	This function applies 3D changes on all the initialized devices. There is no need to re-call it for each
	individual device when using multiple devices.
	</p>
	End Rem
	Function Apply3D()
		BASS_Apply3D()
	End Function
?win32
	Rem
	bbdoc: Retrieves the current type of EAX environment and its parameters. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: 
	End Rem
	Function GetEAXParameters:Int(env:Int Var, vol:Float Var, decay:Float Var, damp:Float Var)
		Return BASS_GetEAXParameters(Varptr env, Varptr vol, Varptr decay, Varptr damp)
	End Function
	
	Rem
	bbdoc: Sets the type of EAX environment and its parameters. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: 
	End Rem
	Function SetEAXParameters:Int(env:Int, vol:Float, decay:Float, damp:Float)
		Return BASS_SetEAXParameters(env, vol, decay, damp)
	End Function
?
?Not Linux
	Rem
	bbdoc: Retrieves the factors that affect the calculations of 3D sound. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: Parameters: 
	<ul>
	<li><b>distf</b> : The distance factor</li>
	<li><b>rollf</b> : The rolloff factor</li>
	<li><b>doppf</b> : The doppler factor</li>
	</ul>
	<p>
	When using multiple devices, the current thread's device setting (as set with BASS_SetDevice)
	determines which device this function call applies to.
	</p>
	End Rem
	Function Get3DFactors:Int(distf:Float Var, rollf:Float Var, doppf:Float Var)
		Return BASS_Get3DFactors(Varptr distf, Varptr rollf, Varptr doppf)
	End Function
	
	Rem
	bbdoc: Sets the factors that affect the calculations of 3D sound. 
	returns: If successful, then TRUE is returned, else FALSE is returned. Use TBass.ErrorGetCode to get the error code. 
	about: Parameters
	<ul>
	<li><b>distf</b> : The distance factor... 0 or less = leave current... examples: 1.0 = use meters,
	0.9144 = use yards, 0.3048 = use feet. By default BASS measures distances in meters, you can change this
	setting if you are using a different unit of measurement. </li>
	<li><b>rollf</b> : The rolloff factor, how fast the sound quietens with distance... 0.0 (min) - 10.0 (max),
	less than 0.0 = leave current...
	examples: 0.0 = no rolloff, 1.0 = real world, 2.0 = 2x real. </li>
	<li><b>doppf</b> : The doppler factor... 0.0 (min) - 10.0 (max), less than 0.0 = leave current... examples:
	0.0 = no doppler, 1.0 = real world, 2.0 = 2x real. The doppler effect is the way a sound appears to change
	pitch when it is moving towards or away from you. The listener and sound velocity settings are used to
	calculate this effect, this doppf value can be used to lessen or exaggerate the effect.</li>
	</ul>
	<p>
	As with all 3D functions, use #Apply3D to apply the changes. 
	</p>
	<p>
	When using multiple devices, the current thread's device setting (as set with BASS_SetDevice) determines which device this function call applies to. 
	</p>
	End Rem
	Function Set3DFactors:Int(distf:Float, rollf:Float, doppf:Float)
		Return BASS_Set3DFactors(distf, rollf, doppf)
	End Function
	
	Rem
	bbdoc: Retrieves information on the device being used. 
	returns: If successful, a TBassInfo object is returned, otherwise Null.
	about: When using multiple devices, the current thread's device setting (as set with BASS_SetDevice) determines which device this function call applies to. 
	End Rem
	Function GetInfo:TBassInfo()
		Return TBassInfo._create(bmx_bass_getinfo())
	End Function
	
End Type

Rem
bbdoc: Used with TBass.GetDeviceInfo or TBassRecord.GetDeviceInfo to retrieve information on a device. 
about: When a device is disabled/disconnected, it is still retained in the device list, but the
BASS_DEVICE_ENABLED flag is removed from it. If the device is subsequently re-enabled, it may become available
again with the same device number, or the system may add a new entry for it. 
<p>
When a new device is connected, it can affect the other devices and result in the system moving them to new
device entries. If an affected device is initialized, it will stop working and will need to be reinitialized
using its new device number. 
</p>
<p>
<b>OSX notes</b> - driver is not used.
</p>
End Rem
Type TBassDeviceInfo

	Field deviceinfoPtr:Byte Ptr

	Function _create:TBassDeviceInfo(deviceinfoPtr:Byte Ptr)
		If deviceinfoPtr Then
			Local this:TBassDeviceInfo = New TBassDeviceInfo
			this.deviceinfoPtr = deviceinfoPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Description of the device.
	End Rem
	Method GetName:String()
		Return String.FromCString(bmx_deviceinfo_getname(deviceinfoPtr))
	End Method
	
	Rem
	bbdoc: The filename of the driver... NULL = no driver ("no sound" device).
	about: On systems that can use both VxD and WDM drivers (Windows Me/98SE), this will reveal which type of
	driver is being used. Further information can be obtained from the file using the GetFileVersionInfo Win32
	API function.
	End Rem
	Method GetDriver:String()
		Return String.FromCString(bmx_deviceinfo_getdriver(deviceinfoPtr))
	End Method
	
	Rem
	bbdoc: The device's current status... a combination of these flags. 
	about: 
	End Rem
	Method GetFlags:Int()
		Return bmx_deviceinfo_getflags(deviceinfoPtr)
	End Method

	Method Delete()
		If deviceinfoPtr Then
			bmx_deviceinfo_delete(deviceinfoPtr)
			deviceinfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBassRecord Extends TBassChannel

	Field callback:Int(handle:TBassRecord, buffer:Byte Ptr, length:Int, user:Object)
	Field userData:Object

	Rem
	bbdoc: 
	End Rem
	Function RecordStart:TBassRecord(freq:Int, chans:Int, flags:Int, proc:Int(handle:TBassRecord, buffer:Byte Ptr, length:Int, user:Object), user:Object)
		Return New TBassRecord.Start(freq, chans, flags, proc, user)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Start:TBassRecord(freq:Int, chans:Int, flags:Int, proc:Int(handle:TBassRecord, buffer:Byte Ptr, length:Int, user:Object), user:Object)
		callback = proc
		userData = user
		handle = BASS_RecordStart(freq, chans, flags, _proc, Self)
		Return Self
	End Method
	
	Function _proc:Int(handle:Int, buffer:Byte Ptr, length:Int, user:Object)
		TBassRecord(user).callback(TBassRecord(user), buffer, length, TBassRecord(user).userData)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Free:Int()
		Return BASS_RecordFree()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetRecordDevice:Int()
		Return BASS_RecordGetDevice()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function Init:Int(device:Int)
		Return BASS_RecordInit(device)
	End Function
	
	Rem
	bbdoc: Sets the recording device to use for subsequent calls in the current thread. 
	End Rem
	Function SetDevice:Int(device:Int)
		Return BASS_RecordSetDevice(device)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function SetInput:Int(inp:Int, flags:Int, volume:Float)
		Return BASS_RecordSetInput(inp, flags, volume)
	End Function
	
	Rem
	bbdoc: Retrieves the current settings of a recording input source. 
	End Rem
	Function GetInput:Int(inp:Int, volume:Float Var)
		Return BASS_RecordGetInput(inp, Varptr volume)
	End Function

	Rem
	bbdoc: Retrieves the text description of a recording input source. 
	End Rem
	Function GetInputName:String(inp:Int)
		Return String.FromCString(BASS_RecordGetInputName(inp))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetRecordInfo:TBassRecordInfo()
		Return TBassRecordInfo._create(bmx_bass_recordgetinfo())
	End Function
	
End Type

Rem
bbdoc: 
about: The formats member does not represent all the formats supported by the device, just the "standard" ones.
<p>
If there is no DirectSound driver for the device (ie. it's being emulated), then the driver member will contain
something like "WaveIn" instead of a filename. 
</p>
<p>
<b>OSX notes</b> - The flags and formats members are not used. singlein is always TRUE.
</p>
End Rem
Type TBassRecordInfo

	Field recordInfoPtr:Byte Ptr
	
	Function _create:TBassRecordInfo(recordInfoPtr:Byte Ptr)
		If recordInfoPtr Then
			Local this:TBassRecordInfo = New TBassRecordInfo
			this.recordInfoPtr = recordInfoPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The device's capabilities... a combination of these flags.
	about: <table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>DSCCAPS_EMULDRIVER</td><td>The device's drivers do NOT have DirectSound recording support, so it is
	being emulated.</td></tr>
	<tr><td>DSCCAPS_CERTIFIED</td><td>The device driver has been certified by Microsoft.</td></tr>
	</table>
	End Rem
	Method GetFlags:Int()
		Return bmx_recordinfo_getflags(recordInfoPtr)
	End Method
	
	Rem
	bbdoc: The standard formats supported by the device... a combination of these flags. 
	about: <table width="100%">
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>WAVE_FORMAT_1M08</td><td>11025hz, Mono, 8-bit</td></tr>
	<tr><td>WAVE_FORMAT_1S08</td><td>11025hz, Stereo, 8-bit</td></tr>
	<tr><td>WAVE_FORMAT_1M16</td><td>11025hz, Mono, 16-bit </td></tr>
	<tr><td>WAVE_FORMAT_1S16</td><td>11025hz, Stereo, 16-bit </td></tr>
	<tr><td>WAVE_FORMAT_2M08</td><td>22050hz, Mono, 8-bit</td></tr>
	<tr><td>WAVE_FORMAT_2S08</td><td>22050hz, Stereo, 8-bit </td></tr>
	<tr><td>WAVE_FORMAT_2M16</td><td>22050hz, Mono, 16-bit </td></tr>
	<tr><td>WAVE_FORMAT_2S16</td><td>22050hz, Stereo, 16-bit </td></tr>
	<tr><td>WAVE_FORMAT_4M08</td><td>44100hz, Mono, 8-bit </td></tr>
	<tr><td>WAVE_FORMAT_4S08</td><td>44100hz, Stereo, 8-bit </td></tr>
	<tr><td>WAVE_FORMAT_4M16</td><td>44100hz, Mono, 16-bit </td></tr>
	<tr><td>WAVE_FORMAT_4S16</td><td>44100hz, Stereo, 16-bit</td></tr>
	</table>
	End Rem
	Method GetFormats:Int()
		Return bmx_recordinfo_getformats(recordInfoPtr)
	End Method
	
	Rem
	bbdoc: The number of input sources available to the device.
	End Rem
	Method GetInputs:Int()
		Return bmx_recordinfo_getinputs(recordInfoPtr)
	End Method
	
	Rem
	bbdoc: TRUE = only one input may be active at a time. 
	End Rem
	Method IsSingleIn:Int()
		Return bmx_recordinfo_issinglein(recordInfoPtr)
	End Method
	
	Rem
	bbdoc: The device's current input sample rate (available only on Windows Vista and OSX).
	about: Recording at this rate will give the best quality and performance, as no resampling is required.
	End Rem
	Method GetFreq:Int()
		Return bmx_recordinfo_getfreq(recordInfoPtr)
	End Method
	
	Method Delete()
		If recordInfoPtr Then
			bmx_recordinfo_delete(recordInfoPtr)
			recordInfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Type used by the 3D functions to describe positions, velocities, and orientations. 
End Rem
Type TBass3DVector

	Field vectorPtr:Byte Ptr

	Function _create:TBass3DVector(vectorPtr:Byte Ptr)
		If vectorPtr Then
			Local this:TBass3DVector = New TBass3DVector
			this.vectorPtr = vectorPtr
			Return this
		End If
	End Function
	
	Method New()
		vectorPtr = bmx_bass_3dvector_new()
	End Method

	Rem
	bbdoc: Creates a new 3D Vector object.
	End Rem
	Function CreateVector:TBass3DVector(x:Float = 0, y:Float = 0, z:Float = 0)
		Return New TBass3DVector.Create(x, y, z)
	End Function
	
	Rem
	bbdoc: Creates a new 3D Vector object.
	End Rem
	Method Create:TBass3DVector(x:Float = 0, y:Float = 0, z:Float = 0)
		bmx_bass_3dvector_setxyz(vectorPtr, x, y, z)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the X vector.
	End Rem
	Method GetX:Float()
		Return bmx_bass_3dvector_getx(vectorPtr)
	End Method

	Rem
	bbdoc: Returns the Y vector.
	End Rem
	Method GetY:Float()
		Return bmx_bass_3dvector_gety(vectorPtr)
	End Method

	Rem
	bbdoc: Returns the Z vector.
	End Rem
	Method GetZ:Float()
		Return bmx_bass_3dvector_getz(vectorPtr)
	End Method
	
	Rem
	bbdoc: Sets the X vector.
	End Rem
	Method SetX(value:Float)
		bmx_bass_3dvector_setx(vectorPtr, value)
	End Method

	Rem
	bbdoc: Sets the Y vector.
	End Rem
	Method SetY(value:Float)
		bmx_bass_3dvector_sety(vectorPtr, value)
	End Method

	Rem
	bbdoc: Sets the Y vector.
	End Rem
	Method SetZ(value:Float)
		bmx_bass_3dvector_setz(vectorPtr, value)
	End Method
	
	Rem
	bbdoc: Returns X, Y and Z.
	End Rem
	Method GetXYZ(x:Float Var, y:Float Var, z:Float Var)
		bmx_bass_3dvector_getxyz(vectorPtr, Varptr x, Varptr y, Varptr z)
	End Method
	
	Rem
	bbdoc: Sets X, Y and Z.
	End Rem
	Method SetXYZ(x:Float, y:Float, z:Float)
		bmx_bass_3dvector_setxyz(vectorPtr, x, y, z)
	End Method
	
	
	Method Delete()
		If vectorPtr Then
			bmx_bass_3dvector_delete(vectorPtr)
			vectorPtr = Null
		End If
	End Method
End Type

Type TBassFXFactory
	Field _succ:TBassFXFactory
	
	Method New()
		_succ = fx_factories
		fx_factories = Self
	End Method
	
	Method FXForType:TBassFXType(fxType:Int) Abstract
		
End Type

New TBassCoreFXFactory

Type TBassCoreFXFactory Extends TBassFXFactory

	Method FXForType:TBassFXType(fxType:Int)
		Select fxType
			Case BASS_FX_DX8_CHORUS
			Case BASS_FX_DX8_COMPRESSOR
			Case BASS_FX_DX8_DISTORTION
			Case BASS_FX_DX8_ECHO
			Case BASS_FX_DX8_FLANGER
			Case BASS_FX_DX8_GARGLE
			Case BASS_FX_DX8_I3DL2REVERB
			Case BASS_FX_DX8_PARAMEQ
			Case BASS_FX_DX8_REVERB
		End Select
	End Method	

End Type


?
