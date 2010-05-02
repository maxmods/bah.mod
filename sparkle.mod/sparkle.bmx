' Copyright (c) 2010 Bruce A Henderson
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
bbdoc: Software update framework for Mac.
End Rem
Module BaH.Sparkle

?macos
ModuleInfo "LD_OPTS: -F%PWD%/Frameworks"

Import "common.bmx"

Rem
bbdoc: Initialises Sparkle.
about: You should instantiate your custom TSUUpdater before calling this.
End Rem
Function SparkleInit()
	If Not TSUUpdater.instance Then
		New TSUUpdater
	End If
	
	bmx_sparkle_init(TSUUpdater.instance)
End Function

Rem
bbdoc: Controls the updating system for the application.
End Rem
Type TSUUpdater

	Global instance:TSUUpdater

	Field updPtr:Byte Ptr
	
	Method New()
		If Not instance Then
			instance = Self
		End If
	End Method
	
	Function _callback(updater:TSUUpdater, updPtr:Byte Ptr)
		updater.updPtr = updPtr
		updater.init()
	End Function

	Rem
	bbdoc: Override to provide pre-initialistion configuraion.
	End Rem
	Method init()
	End Method
	
	Rem
	bbdoc: Set to True to automatically check for updates.
	End Rem
	Method setAutomaticallyChecksForUpdates(value:Int)
		bmx_sparkle_setAutomaticallyChecksForUpdates(updPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether the application automatically checks for updates.
	End Rem
	Method automaticallyChecksForUpdates:Int()
		Return bmx_sparkle_automaticallyChecksForUpdates(updPtr)
	End Method
	
	Rem
	bbdoc: Set the update interval, in seconds.
	End Rem
	Method setUpdateCheckInterval(value:Double)
		bmx_sparkle_setUpdateCheckInterval(updPtr, value)
	End Method
	
	Rem
	bbdoc: Returns the current update interval, in seconds.
	End Rem
	Method updateCheckInterval:Double()
		Return bmx_sparkle_updateCheckInterval(updPtr)
	End Method
	
	Rem
	bbdoc: Sets the feed URL.
	End Rem
	Method setFeedURL(url:String)
		bmx_sparkle_setFeedURL(updPtr, url)
	End Method
	
	Rem
	bbdoc: Returns the feed URL.
	End Rem
	Method feedURL:String()
		Return bmx_sparkle_feedURL(updPtr)
	End Method
	
	Rem
	bbdoc: Set to True to send system profile information.
	End Rem
	Method setSendsSystemProfile(value:Int)
		bmx_sparkle_setSendsSystemProfile(updPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether system profile information is to be sent.
	End Rem
	Method sendsSystemProfile:Int()
		Return bmx_sparkle_sendsSystemProfile(updPtr)
	End Method
	
	Rem
	bbdoc: Set to True to automatically download updates.
	End Rem
	Method setAutomaticallyDownloadsUpdates(value:Int)
		bmx_sparkle_setAutomaticallyDownloadsUpdates(updPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether updates are automatically downloaded.
	End Rem
	Method automaticallyDownloadsUpdates:Int()
		Return bmx_sparkle_automaticallyDownloadsUpdates(updPtr)
	End Method
	
	Rem
	bbdoc: Kicks off an update meant to be programmatically initiated.
	about: It will display no UI unless it actually finds an update, in which case it
	proceeds as usual. If the automated downloading is turned on, however,
	this will invoke that behavior, and if an update is found, it will be
	downloaded and prepped for installation.
	End Rem
	Method checkForUpdatesInBackground()
		bmx_sparkle_checkForUpdatesInBackground(updPtr)
	End Method
	
	Rem
	bbdoc: Begins a "probing" check for updates which will not actually offer to update to that version.
	bout: The delegate methods, though, (up to updater:didFindValidUpdate: and updaterDidNotFindUpdate:), are called, so you
	can use that information in your UI. Essentially, you can use this to UI-lessly determine if there's an update.
	End Rem
	Method checkForUpdateInformation()
		bmx_sparkle_checkForUpdateInformation(updPtr)
	End Method
	
	Rem
	bbdoc: Returns True if an update is in progress.
	End Rem
	Method updateInProgress:Int()
		Return bmx_sparkle_updateInProgress(updPtr)
	End Method
	
	Rem
	bbdoc: Returns the date of last update check, in seconds from the epoch.
	End Rem
	Method lastUpdateCheckDate:Double()
		Return bmx_sparkle_lastUpdateCheckDate(updPtr)
	End Method
	
End Type


?
