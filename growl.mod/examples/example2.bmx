'
' Example to show re-using notification object
'
SuperStrict

Framework BaH.Growl

Local delegate:TGrowlDelegate = New TGrowlDelegate.Create()

delegate.SetApplicationName("GrowlExample") ' the name that appears in the Growl Preferences
delegate.SetAllNotifications(["Example Notification"]) ' our list of supported notifications

If delegate.Install() Then

	Local notification:TGrowlNotification = New TGrowlNotification.Create("Example Notification", ..
		"The First Notification", "This is only the first one!~nJust wait...")
	notification.Post()

	' wait a while
	Delay(3000)

	notification.SetTitle("The Second Notification")
	notification.SetDescription("...and this is the second one, re-using the TGrowlNotification object.")
	notification.Post()

End If
