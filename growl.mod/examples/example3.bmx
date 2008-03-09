'
' Example to show changing icons for a specific notification
'
SuperStrict

Framework BaH.Growl

Incbin "process-stop.png"

Local delegate:TGrowlDelegate = New TGrowlDelegate.Create()

delegate.SetApplicationName("GrowlExample") ' the name that appears in the Growl Preferences
delegate.SetAllNotifications(["Example Notification"]) ' our list of supported notifications

If delegate.Install() Then

	Local notification:TGrowlNotification = New TGrowlNotification.Create("Example Notification", ..
		"The First Notification", "Hey look! We've changed the icon!")
	notification.SetIconData(IncbinPtr("process-stop.png"), IncbinLen("process-stop.png"))
	notification.Post()

	' wait a while
	Delay(3000)

	notification.SetTitle("The Second Notification")
	notification.SetDescription("This icon shows the application default.")
	notification.SetIconData() ' set the icon back to the app default
	notification.Post()

End If

