'
' Example to show re-using notification object, including some basic markup.
'
SuperStrict

Framework BaH.libnotify


If TNotify.Initialize("My BlitzMax App") Then

	Local notification:TNotification = New TNotification.Create("The First Notification", ..
		"This is only the first one!~nJust wait...")
	notification.Show()

	' wait a while
	Delay(3000)
	
	notification.Update("The Second Notification", "...and this is the second one, re-using the TNotification object.~n" + ..
		"<a href=~qhttp://blitzmax.com~q>See BlitzMax!</a>~n")
	notification.SetTimeout(5000)
	notification.Show()

End If

