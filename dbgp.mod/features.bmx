SuperStrict

Import BRL.Map

Type TBlitzMaxFeatures

	Field features:TMap
	
	Method New()
		init()
	End Method
	
	Method init()
		features = New TMap
		
		addFeature(False, "language_supports_threads", "1")
		addFeature(False, "language_name", "BlitzMax")
		addFeature(False, "language_version", "1.33")
		addFeature(False, "encoding", "UTF-8")
		addFeature(False, "protocol_version", "1")
		addFeature(False, "supports_async", "0")
		addFeature(False, "data_encoding", "base64")
		addFeature(False, "breakpoint_languages", "")
		
		addFeature(True, "multiple_sessions", "0")
		addFeature(True, "max_children", "100")
		addFeature(True, "max_data", "4096")
		addFeature(True, "max_depth", "3")
		
		addFeature(False, "supports_postmortem", "0")
		addFeature(True, "show_hidden", "0")
		addFeature(True, "notify_ok", "0")

	End Method

	Method isSupported:Int(name:String)
		Return features.Contains(name)
	End Method
	
	Method getValue:String(name:String)
		If isSupported(name) Then
			Return TFeature(features.valueForKey(name)).value
		Else
			Return Null
		End If
	End Method
	
	Method addFeature(modifiable:Int, name:String, value:String)
		features.Insert(name, New TFeature.Create(modifiable, name, value))
	End Method

	Method setValue:Int(name:String, value:String)
		If isSupported(name) Then
			Local feature:TFeature = TFeature(features.ValueForKey(name))
			
			If feature.modifiable Then
				feature.value = value
				Return True
			End If
		End If
		
		Return False
	End Method

End Type

Type TFeature

	Field modifiable:Int
	Field name:String
	Field value:String
	
	Method Create:TFeature(modifiable:Int, name:String, value:String)
		Self.modifiable = modifiable
		Self.name = name
		Self.value = value
		
		Return Self
	End Method
	
	
End Type

