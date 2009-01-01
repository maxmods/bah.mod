'
' Display tag clusters for a given tag.
'
'
SuperStrict

Framework BaH.Flickcurl
Import BRL.System
Import BRL.StandardIO

InitFlickcurl

Local fc:TFlickcurl = New TFlickcurl

fc.SetAPIKey("xxxxxxxxxxxx")
fc.SetSharedSecret("xxxxxxxxxx")
fc.SetAuthToken(fc.GetFullToken("xxxxxxxxxxx")) ' frob

Local clusters:TFCTagCluster[] = fc.GetClusters("cow")

If clusters Then

	For Local cluster:TFCTagCluster = EachIn clusters
		
		Print "*"

		If cluster.tags Then
		
			For Local tag:String = EachIn cluster.tags
				Print "  - " + tag
			Next
			
		End If
		
	Next


End If



fc.Free()
FinishFlickcurl



