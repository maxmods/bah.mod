SuperStrict

Import BaH.CEGUI


Type CEGuiBaseApplication

	Global quitting:Int

	Method getDataPathPrefix:String()
		Return "../datafiles"
	End Method

	Method initialiseResourceGroupDirectories()
		' initialise the required dirs for the DefaultResourceProvider
		Local rp:TCEDefaultResourceProvider = TCEDefaultResourceProvider(TCESystem.getResourceProvider())
		
		Local dataPathPrefix:String = getDataPathPrefix()
		
		' for each resource type, set a resource group directory
		rp.setResourceGroupDirectory("schemes", dataPathPrefix + "/schemes/")
		rp.setResourceGroupDirectory("imagesets", dataPathPrefix + "/imagesets/")
		rp.setResourceGroupDirectory("fonts", dataPathPrefix + "/fonts/")
		rp.setResourceGroupDirectory("layouts", dataPathPrefix + "/layouts/")
		rp.setResourceGroupDirectory("looknfeels", dataPathPrefix + "/looknfeel/")
		rp.setResourceGroupDirectory("lua_scripts", dataPathPrefix + "/lua_scripts/")
		rp.setResourceGroupDirectory("schemas", dataPathPrefix + "/xml_schemas/")
	End Method

	Method initialiseDefaultResourceGroups()
		' set the default resource groups to be used
		TCEImageset.setDefaultResourceGroup("imagesets")
		TCEFont.setDefaultResourceGroup("fonts")
		TCEScheme.setDefaultResourceGroup("schemes")
		TCEWidgetLookManager.setDefaultResourceGroup("looknfeels")
		TCEWindowManager.setDefaultResourceGroup("layouts")
		'TCEScriptModule.setDefaultResourceGroup("lua_scripts")
	End Method
	
	Method initialise:Int() Abstract
	
	Method run()
	
		initialiseResourceGroupDirectories()
		initialiseDefaultResourceGroups()
	
		If initialise() Then
			mainloop()
		End If

		cleanup()	
	End Method
	
	Method mainloop()
		
		While Not quitting And Not KeyDown(KEY_ESCAPE)
		
			Cls
			
			TCEEvent.mouseEvents(MouseX(), MouseY(), MouseZ())
			TCESystem.renderGUI()
		
			Flip
		
		Wend
	
	End Method
	
	Method cleanup()
	End Method
	
End Type


