' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: libnotify
End Rem
Module BaH.libnotify

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Changed to use FromUTF8String instead of FromCString."
ModuleInfo "History: 1.00 Initial Release"

?linux

Import "common.bmx"

Rem
bbdoc: Notification API
End Rem
Type TNotify

	Rem
	bbdoc: Initialize libnotify.
	about: This must be called before any other functions.
	End Rem
	Function Initialize:Int(appName:String)
		Local s:Byte Ptr = appName.ToUTF8String()
		Local ret:Int = notify_init(s)
		MemFree(s)
		Return ret
	End Function
	
	Rem
	bbdoc: Gets whether or not libnotify is initialized.
	returns: TRUE if libnotify is initialized, or FALSE otherwise.
	End Rem
	Function IsInitted:Int()
		Return notify_is_initted()
	End Function
	
	Rem
	bbdoc: Uninitialize libnotify.
	about: This should be called when the program no longer needs libnotify for the rest of its lifecycle,
	typically just before exitting.
	End Rem
	Function UnInit()
		notify_uninit()
	End Function
	
	Rem
	bbdoc: Gets the application name registered.
	End Rem
	Function GetAppName:String()
		Return String.FromUTF8String(notify_get_app_name())
	End Function
	
	Rem
	bbdoc: Queries the server for its information, specifically, the name, vendor, server version, and the version of the notifications specification that it is compliant with.
	returns: TRUE if successful, and the variables passed will be set. FALSE  on failure.
	End Rem
	Function GetServerInfo:Int(name:String Var, vendor:String Var, version:String Var, specVersion:String Var)
		Local n:Byte Ptr, ven:Byte Ptr, ver:Byte Ptr, spec:Byte Ptr
		Local ret:Int = notify_get_server_info(Varptr n, Varptr ven, Varptr ver, Varptr spec)
		
		name = String.FromUTF8String(n)
		vendor = String.FromUTF8String(ven)
		version = String.FromUTF8String(ver)
		specVersion = String.FromUTF8String(spec)
		
		g_free(n)
		g_free(ven)
		g_free(ver)
		g_free(spec)
		
		Return ret
	End Function

End Type



Rem
bbdoc: A passive pop-up notification.
about: It can contain summary text, body text, and an icon, as well as hints specifying how the notification
should be presented. The notification is rendered by a notification daemon, and may present the notification in any number
of ways. As such, there is a clear separation of content and presentation, and this API enforces that.
End Rem
Type TNotification

	Field notificationPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new TNotification.
	about: The summary text is required, but all other parameters are optional.
	End Rem
	Function CreateNotification:TNotification(summary:String, body:String = "", icon:String = "")
		Return New TNotification.Create(summary, body, icon)
	End Function
	
	Rem
	bbdoc: Creates a new TNotification.
	about: The summary text is required, but all other parameters are optional.
	End Rem
	Method Create:TNotification(summary:String, body:String = "", icon:String = "")
		Local s:Byte Ptr, b:Byte Ptr, i:Byte Ptr
		
		Assert summary, "Summary is required"
		
		s = summary.ToUTF8String()
		
		If body Then
			b = body.ToUTF8String()
		End If
		
		If icon Then
			i = icon.ToUTF8String()
		End If
		
		If b Then
			If i Then
				notificationPtr = notify_notification_new(s, b, i, Null)
			Else
				notificationPtr = notify_notification_new(s, b, Null, Null)
			End If
		Else
			If i Then
				notificationPtr = notify_notification_new(s, Null, i, Null)
			Else
				notificationPtr = notify_notification_new(s, Null, Null, Null)
			End If
		End If
		
		MemFree(s)
		If b Then
			MemFree(b)
		End If
		If i Then
			MemFree(i)
		End If
		
		Return Self
	End Method
	
	Rem
	bbdoc: Updates the notification text and icon.
	about: This won't send the update out and display it on the screen. For that, you will need to call #Show().
	End Rem
	Method Update:Int(summary:String, body:String = "", icon:String = "")
		Local s:Byte Ptr, b:Byte Ptr, i:Byte Ptr
		
		Assert summary, "Summary is required"
		
		s = summary.ToUTF8String()
		
		If body Then
			b = body.ToUTF8String()
		End If
		
		If icon Then
			i = icon.ToUTF8String()
		End If
		
		Local ret:Int
		If b Then
			If i Then
				ret = notify_notification_update(notificationPtr, s, b, i)
			Else
				ret = notify_notification_update(notificationPtr, s, b, Null)
			End If
		Else
			If i Then
				ret = notify_notification_update(notificationPtr, s, Null, i)
			Else
				ret = notify_notification_update(notificationPtr, s, Null, Null)
			End If
		End If
		
		MemFree(s)
		If b Then
			MemFree(b)
		End If
		If i Then
			MemFree(i)
		End If
		
		Return ret
	End Method
	
	Rem
	bbdoc: Tells the notification server to display the notification on the screen.
	End Rem
	Method Show:Int()
		Return notify_notification_show(notificationPtr, Null)
	End Method
	
	Rem
	bbdoc: Sets the timeout of the notification.
	about: To set the default time, pass #NOTIFY_EXPIRES_DEFAULT as timeout. To set the notification to never expire,
	pass #NOTIFY_EXPIRES_NEVER.
	End Rem
	Method SetTimeout(timeout:Int)
		notify_notification_set_timeout(notificationPtr, timeout)
	End Method
	
	Rem
	bbdoc: Sets the category of this notification.
	about: This can be used by the notification server to filter or display the data in a certain way.
	End Rem
	Method SetCategory(category:String)
		Local s:Byte Ptr = category.ToUTF8String()
		notify_notification_set_category(notificationPtr, s)
		MemFree(s)
	End Method
	
	Rem
	bbdoc: Sets the urgency level of this notification.
	about: One of, #NOTIFY_URGENCY_LOW, #NOTIFY_URGENCY_NORMAL, or #NOTIFY_URGENCY_CRITICAL.
	End Rem
	Method SetUrgency(urgency:Int)
		notify_notification_set_urgency(notificationPtr, urgency)
	End Method
	
	Rem
	bbdoc: TODO
	End Rem
	Method SetIconFromPixmap()
	End Method
	
	Rem
	bbdoc: Tells the notification server to hide the notification on the screen.
	End Rem
	Method Close:Int()
		Return notify_notification_close(notificationPtr, Null)
	End Method

End Type





?