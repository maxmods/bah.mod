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

Import BRL.Blitz
Import Pub.Macos

Import "-framework Sparkle"

Import "include/*.h"

Import "glue.m"

Extern

	Function bmx_sparkle_init(handle:Object)
	Function bmx_sparkle_setAutomaticallyChecksForUpdates(handle:Byte Ptr, value:Int)
	Function bmx_sparkle_automaticallyChecksForUpdates:Int(handle:Byte Ptr)
	Function bmx_sparkle_setUpdateCheckInterval(handle:Byte Ptr, value:Double)
	Function bmx_sparkle_updateCheckInterval:Double(handle:Byte Ptr)
	Function bmx_sparkle_setFeedURL(handle:Byte Ptr, url:String)
	Function bmx_sparkle_feedURL:String(handle:Byte Ptr)
	Function bmx_sparkle_setSendsSystemProfile(handle:Byte Ptr, value:Int)
	Function bmx_sparkle_sendsSystemProfile:Int(handle:Byte Ptr)
	Function bmx_sparkle_setAutomaticallyDownloadsUpdates(handle:Byte Ptr, value:Int)
	Function bmx_sparkle_automaticallyDownloadsUpdates:Int(handle:Byte Ptr)
	
	Function bmx_sparkle_checkForUpdatesInBackground(handle:Byte Ptr)
	Function bmx_sparkle_checkForUpdateInformation(handle:Byte Ptr)
	Function bmx_sparkle_updateInProgress:Int(handle:Byte Ptr)
	Function bmx_sparkle_lastUpdateCheckDate:Double(handle:Byte Ptr)

End Extern

