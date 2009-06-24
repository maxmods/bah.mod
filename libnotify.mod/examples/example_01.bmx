SuperStrict

Framework BaH.libnotify


If TNotify.Initialize("My BlitzMax App") Then

	Local notification:TNotification = New TNotification.Create("Hello from BlitzMax!", ..
		"What's this, notifications in Linux?~nCool ! :-)")

	notification.Show()

End If

