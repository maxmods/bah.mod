' Copyright (c) 2008, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz

Import "-framework Growl"
Import "Frameworks/Growl.framework/Headers/*.h"

Import "glue.cpp"


Extern

	Function bmx_Growl_Delegate_create:Byte Ptr()
	Function bmx_Growl_SetDelegate(handle:Byte Ptr)
	Function bmx_growldelegate_setapplicationname(handle:Byte Ptr, name:String)
	Function bmx_growldelegate_setregistrationdictionary_array(handle:Byte Ptr, allNot:Object, defNot:Object, id:String)
	Function bmx_growldelegate_setapplicationicondata(handle:Byte Ptr, image:Byte Ptr, size:Int)

	Function bmx_Growl_Notification_create:Byte Ptr(name:String, title:String, description:String)
	Function bmx_Growl_PostNotification(handle:Byte Ptr)
	Function bmx_Growl_Notification_delete(handle:Byte Ptr)
	Function bmx_Growl_Notification_setname(handle:Byte Ptr, name:String)
	Function bmx_Growl_Notification_settitle(handle:Byte Ptr, title:String)
	Function bmx_Growl_Notification_setdescription(handle:Byte Ptr, description:String)
	Function bmx_Growl_Notification_setpriority(handle:Byte Ptr, priority:Int)
	Function bmx_Growl_Notification_seticondata(handle:Byte Ptr, pix:Byte Ptr, size:Int)

	Function bmx_Growl_IsInstalled:Int()
	Function bmx_Growl_IsRunning:Int()

End Extern

Rem
bbdoc: The name of your application.
about: The name of your application. This should remain stable between
different versions And incarnations of your application.
For example, "SurfWriter" is a good app name, whereas "SurfWriter 2.0" And "SurfWriter Lite" are Not.
end rem
Const GROWL_APP_NAME:String = "ApplicationName"


Rem
bbdoc: The bundle identifier of your application.
about: The bundle identifier of your application. This key should
be unique For your application While there May be several applications
with the same GROWL_APP_NAME.
This key is optional.
End Rem
Const GROWL_APP_ID:String = "ApplicationId"


Rem
bbdoc: The image data For your application's icon.
about: Image data representing your application's icon. This may be
superimposed on a notification icon as a badge, used as the notification
icon when a notification-specific icon is not supplied, or ignored
altogether, depending on the display. Must be in a format supported by
NSImage, such as TIFF, PNG, GIF, JPEG, BMP, PICT, or PDF.
Optional. Not supported by all display plugins.

End Rem
Const GROWL_APP_ICON:String = "ApplicationIcon"


Rem
bbdoc:  The array of notifications to turn on by Default.
about: These are the names of the notifications that should be enabled
by Default when your application registers For the First time. If your
application reregisters, Growl will look here For any New notification
names found in GROWL_NOTIFICATIONS_ALL, but ignore any others.
End Rem

Const GROWL_NOTIFICATIONS_DEFAULT:String = "DefaultNotifications"

Rem 
bbdoc: The array of all notifications your application can send.
about: These are the names of all of the notifications that your
application May post. See GROWL_NOTIFICATION_NAME For a discussion of good
notification names.
end rem
Const GROWL_NOTIFICATIONS_ALL:String = "AllNotifications"

Rem 
bbdoc: A dictionary of human-readable names For your notifications.
about: By Default, the Growl UI will display notifications by the names given in GROWL_NOTIFICATIONS_ALL
 which correspond To the GROWL_NOTIFICATION_NAME. This dictionary specifies the human-readable name To display.
 The keys of the dictionary are GROWL_NOTIFICATION_NAME strings; the objects are the human-readable versions.
 For any GROWL_NOTIFICATION_NAME Not specific in this dictionary, the GROWL_NOTIFICATION_NAME will be displayed.
 This key is optional.
end rem
Const GROWL_NOTIFICATIONS_HUMAN_READABLE_NAMES:String = "HumanReadableNames"



Rem 
bbdoc: A dictionary of descriptions of _when_ each notification occurs
about: This is an NSDictionary whose keys are GROWL_NOTIFICATION_NAME strings And whose objects are
descriptions of _when_ each notification occurs, such as "You received a new mail message" Or
"A file finished downloading".
This key is optional.
end rem
Const GROWL_NOTIFICATIONS_DESCRIPTIONS:String = "NotificationDescriptions"


