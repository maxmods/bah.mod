'
' SID player example for BASS audio.
'
'
SuperStrict

Framework bah.sidbass

Import "../../sid.mod/examples/example_base.bmx"


' BASS audio initialisation
If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If


New TBASSPlayer.Run()


' Bass cleanup
TBass.Free()

End

Type TBASSPlayer Extends TPlayer


End Type


