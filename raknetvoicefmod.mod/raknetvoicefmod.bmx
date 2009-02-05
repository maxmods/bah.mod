SuperStrict

Module BaH.RaknetVoiceFMOD

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: Raknet - The Creative Commons Attribution - NonCommercial 2.5, or see http://www.rakkarsoft.com/SingleApplicationLicense.html"
ModuleInfo "Copyright: 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Version"



Import BaH.RaknetVoice

Import "common.bmx"


New TRKFMODVoiceAdapter

Type TRKFMODVoiceAdapter Extends TRKVoiceAdapter

	Field system:TFMODSystem
	
	Field adapterPtr:Byte Ptr

	Method New()
		If Not GetAdapter() Then
			SetAdapter(Self)
		End If
	End Method

	Method Init:Int(audioInterface:Object, rakVoice:TRKRakVoice)
		system = TFMODSystem(audioInterface)

		If system Then
		
			Self.rakVoice = rakVoice
		
			adapterPtr = bmx_raknetvoicefmod_adapter_new(system.systemPtr, rakVoice.pluginPtr)
		
			Return True
		End If
		
		Return False
	End Method
	
	Method Update()
		If adapterPtr Then
			bmx_raknetvoicefmod_adapter_update(adapterPtr)
		End If
	End Method

End Type

