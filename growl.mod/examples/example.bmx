'
' Example to show customizing of the application icon
'
SuperStrict

Framework BaH.Growl


Incbin "process-stop.png"


Local delegate:TGrowlDelegate = New TGrowlDelegate.Create()

delegate.SetApplicationName("GrowlExample") ' the name that appears in the Growl Preferences
delegate.SetApplicationIconData(IncbinPtr("process-stop.png"), IncbinLen("process-stop.png"))

delegate.SetAllNotifications(["Example Notification"]) ' our list of supported notifications

If delegate.Install() Then

	Local notification:TGrowlNotification = New TGrowlNotification.Create("Example Notification", ..
		"An example notification", "Hey!~nWe changed the application icon!!")
	notification.Post()

End If
