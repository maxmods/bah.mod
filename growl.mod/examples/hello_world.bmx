SuperStrict

Framework BaH.Growl

Local delegate:TGrowlDelegate = New TGrowlDelegate.Create()

delegate.SetApplicationName("HelloWorld")

delegate.SetAllNotifications(["Hello World"]) ' Supported notifications list

If delegate.Install() Then ' install the delegate, so that we can send notifications

	Local noti:TGrowlNotification = New TGrowlNotification.Create("Hello World", ..
		"Hello World!", "A small Hello World example from BlitzMax!")
		
	noti.Post() ' post the notification

End If
