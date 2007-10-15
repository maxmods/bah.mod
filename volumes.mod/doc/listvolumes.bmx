SuperStrict

Framework bah.volumes
Import brl.standardio

Local list:TList = ListVolumes()

If list Then
	Print "Volumes :"

	For Local v:TVolume = EachIn list
	
		'If v.available Then
			Print "~t" + v.volumeName + "  -  " + v.volumeDevice + " (" +  v.volumeType +  ") -  " + ((v.volumeFree / 1024) / 1024) + "mb"
		'End If
	
	Next
End If