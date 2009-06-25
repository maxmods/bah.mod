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


?linux
Import "-L/usr/lib"
Import "-L/opt/gnome/lib" ' some distros keep libs here
Import "-lglib-2.0"
Import "-lgobject-2.0"
Import "-lgmodule-2.0"
Import "-lnotify"
?


Extern
	Function g_free(handle:Byte Ptr)

	Function notify_init:Int(appname:Byte Ptr)
	Function notify_is_initted:Int()
	Function notify_uninit()
	Function notify_get_app_name:Byte Ptr()
	Function notify_get_server_info:Int(n:Byte Ptr, ven:Byte Ptr, ver:Byte Ptr, spec:Byte Ptr)
	
	Function notify_notification_new:Byte Ptr(summary:Byte Ptr, body:Byte Ptr, icon:Byte Ptr, attach:Byte Ptr)
	Function notify_notification_show:Int(handle:Byte Ptr, err:Byte Ptr)
	Function notify_notification_update:Int(handle:Byte Ptr, summary:Byte Ptr, body:Byte Ptr, icon:Byte Ptr)
	Function notify_notification_set_timeout(handle:Byte Ptr, timeout:Int)
	Function notify_notification_set_category(handle:Byte Ptr, s:Byte Ptr)
	Function notify_notification_set_urgency(handle:Byte Ptr, urgency:Int)
	Function notify_notification_close:Int(handle:Byte Ptr, err:Byte Ptr)
	
End Extern

Rem
bbdoc: Low urgency.
about: Used for unimportant notifications.
End Rem
Const NOTIFY_URGENCY_LOW:Int = 0
Rem
bbdoc: Normal urgency.
about: Used for most standard notifications.
End Rem
Const NOTIFY_URGENCY_NORMAL:Int = 1
Rem
bbdoc: Critical urgency.
about: Used for very important notifications.
End Rem
Const NOTIFY_URGENCY_CRITICAL:Int = 2

Rem
bbdoc: The Default expiration time on a notification.
End Rem
Const NOTIFY_EXPIRES_DEFAULT:Int = -1
Rem
bbdoc: The notification never expires.
about: It stays open until closed by the calling API or the user. 
End Rem
Const NOTIFY_EXPIRES_NEVER:Int = 0


