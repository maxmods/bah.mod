' Copyright (c) 2008-2009 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import Pub.FreeType
Import BaH.RegEx
Import BaH.FreeImage

Import BRL.Map
Import BRL.PolledInput

Import "common.bmx"

' some global stuff
Global cegui_renderer:TCERenderer
Global cegui_startTime:Int

Global ce_event_handler:TCEEventHandler = New TCEEventHandler

' TODO : not sure if this gets called?
OnEnd cegui_cleanup

Rem
bbdoc: 
End Rem
Function cegui_cleanup()

	' remove the event hook!
	RemoveHook EmitEventHook,TCEEvent.Keyhook

	If TCESystem.cegui_systemPtr Then
		'bmx_cegui_system_destroy()
		TCESystem.cegui_systemPtr = Null
	End If
	
	If cegui_renderer Then
		cegui_renderer.free()
		cegui_renderer = Null
	End If
	
End Function

Rem
bbdoc: Initializes CEGUI
about: Optionally, pass in a custom Resource Provider.
<p>
You may also provide your own customEventHandler, and not use the default Blitz event hook system. 
</p>
End Rem
Function Init_CEGUI(renderer:TCERenderer, ResourceProvider:TCEResourceProvider = Null, customEventHandler:Int = False)
	If Not cegui_renderer Then
		' attaches the renderer
		cegui_renderer = renderer

		'If cegui_rendererPtr Then
			' creates a new CEGUI system
		If resourceProvider Then
			TCESystem.resourceProvider = resourceProvider
			TCESystem.cegui_systemPtr = bmx_cegui_new_system(renderer.objectPtr, resourceProvider.objectPtr)
		Else
			TCESystem.cegui_systemPtr = bmx_cegui_new_system(renderer.objectPtr, Null)
		End If
		
		TCEWindowManager.windowManagerPtr = bmx_cegui_windowmanager_getsingleton()
		TCEMouseCursor.mouseCursorPtr = bmx_cegui_mousecursor_getsingleton()
		'End If

		' TODO : there are probably better ways to do this
		cegui_startTime = MilliSecs()
		
		If Not customEventHandler Then
			AddHook EmitEventHook,TCEEvent.Keyhook,Null,0
		End If
	End If
End Function

Rem
bbdoc: Type that defines the required interface for all resource provider sub-types.
about: A ResourceProvider is used to load both XML and binary data from an external source. This could
be from a filesystem or the resource manager of a specific renderer. 
End Rem
Type TCEResourceProvider

	Field objectPtr:Byte Ptr
	
	Method New()
		objectPtr = bmx_cegui_resourceprovider_create(Self)
	End Method
	
	Rem
	bbdoc: Load data, populating dataContainer. 
	about: <i>This method should be overriden.</i>
	End Rem	
	Method loadRawDataContainer(filename:String, dataContainer:TCERawDataContainer, resourceGroup:String)
	End Method
	
	Function _loadRawDataContainer(provider:TCEResourceProvider, filename:String, container:Byte Ptr, resourceGroup:String)
		provider.loadRawDataContainer(filename, TCERawDataContainer._create(container), resourceGroup)
	End Function

	Rem
	bbdoc: Returns the current default resource group identifier.
	End Rem
	Method getDefaultResourceGroup:String()
		Return bmx_cegui_resourceprovider_getdefaultresourcegroup(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the default resource group identifier.
	End Rem
	Method setDefaultResourceGroup(group:String)
		bmx_cegui_resourceprovider_setdefaultresourcegroup(objectPtr, _convertMaxToUTF8(group))
	End Method
		
	Rem
	bbdoc: Unload raw binary data.
	about: This gives the resource provider a chance to unload the data in its own way before the data
	container object is destroyed. If it does nothing, then the object will release its memory. 
	<p>
	<i>This method should be overriden.</i>
	</p>
	End Rem	
	Method unloadRawDataContainer(dataContainer:TCERawDataContainer)
	End Method
	
	Function _unloadRawDataContainer(provider:TCEResourceProvider, container:Byte Ptr)
		provider.unloadRawDataContainer(TCERawDataContainer._create(container))
	End Function

	Method Delete()
		If objectPtr Then
			bmx_cegui_resourceprovider_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Type used as the databuffer for loading files throughout the library. 
End Rem
Type TCERawDataContainer

	Field objectPtr:Byte Ptr

	Function _create:TCERawDataContainer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCERawDataContainer = New TCERawDataContainer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Sets a pointer to the external data. 
	End Rem
	Method setData(data:Byte Ptr)
		bmx_cegui_rawdatacontainer_setdata(objectPtr, data)
	End Method
	
	Rem
	bbdoc: Returns a pointer to the external data. 
	End Rem
	Method getDataPtr:Byte Ptr()
		Return bmx_cegui_rawdatacontainer_getdataptr(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the size of the external data. 
	End Rem
	Method setSize(size:Int)
		bmx_cegui_rawdatacontainer_setsize(objectPtr, size)
	End Method
	
	Rem
	bbdoc: Gets the size of the external data. 
	End Rem
	Method getSize:Int()
		Return bmx_cegui_rawdatacontainer_getsize(objectPtr)
	End Method
	
	Method Delete()
		If objectPtr Then
			bmx_cegui_rawdatacontainer_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type


Type TCEConnection

	Field connectionPtr:Byte Ptr

	Function _create:TCEConnection(connectionPtr:Byte Ptr)
		If connectionPtr Then
			Local this:TCEConnection = New TCEConnection
			this.connectionPtr = connectionPtr
			Return this
		End If
	End Function

	Method Delete()
		If connectionPtr Then
			bmx_cegui_connection_delete(connectionPtr)
			connectionPtr = Null
		End If
	End Method
	
End Type

' keeps track of our subscribed events
Type TCEEventHandler

	Field callbacks:TMap = New TMap

	Method AddCallback:TCEEventCallback(owner:Object, name:String, callback:Int(args:TCEEventArgs))
		Local cb:TCEEventCallback = New TCEEventCallback
		cb.callback = callback
		cb.owner = owner
		cb.name = name

		Local cbList:TList = TList(callbacks.ValueForKey(owner))
		If Not cbList Then
			cbList = New TList
			callbacks.Insert(owner, cbList)
		End If
		
		cbList.AddLast(cb)
		
		Return cb
	End Method
	
	Method FreeCallbacks(owner:Object)
		Local cbList:TList = TList(callbacks.ValueForKey(owner))
		
		If cbList Then
			For Local cb:TCEEventCallback = EachIn cbList
				cb.Free()
			Next
			cbList.Clear()
			callbacks.Remove(owner)
		End If
		
	End Method
		
End Type

' an event callback hook
Type TCEEventCallback

	Field callbackPtr:Byte Ptr
	Field callback:Int(args:TCEEventArgs)
	
	Field owner:Object
	Field name:String
	
	Method New()
		callbackPtr = bmx_cegui_eventcallback_new(Self)
	End Method
	
	Function _callback:Int(instance:TCEEventCallback, args:TCEEventArgs)
		args.owner = instance.owner
		Return instance.callback(args)
	End Function
	
	Method Free()
		If callbackPtr Then
			owner = Null
			callback = Null
	
			' TODO 'bmx_cegui_eventcallback_delete(callbackPtr)
			callbackPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEEventArgs

	Field eventArgsPtr:Byte Ptr
	
	Field owner:Object
	
	Function _create:TCEEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEEventArgs = New TCEEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the event handled status.
	End Rem
	Method getHandled:Int()
		Return bmx_cegui_eventargs_gethandled(eventArgsPtr)
	End Method
	
	Method Delete()
		If eventArgsPtr Then
			bmx_cegui_eventargs_delete(eventArgsPtr)
			eventArgsPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Collects together a set of Event objects.
about: The EventSet is a means for code to attach a handler function to some named event, and later,
for that event to be fired and the subscribed handler(s) called.
End Rem
Type TCEEventSet

	Field objectPtr:Byte Ptr

	Method addEvent(name:String)
	End Method
	
	Method removeEvent(name:String)
	End Method
	
	Method removeAllEvents()
	End Method
	
	Method isEventPresent:Int(name:String)
	End Method
	
	Method isMuted:Int()
	End Method

	Method setMutedState(setting:Int)
	End Method

End Type

Rem
bbdoc: The GlobalEventSet singleton allows you to subscribe to an event for all instances of a type.
about: The GlobalEventSet effectively supports "late binding" to events; which means you can subscribe to some
event that does not actually exist (yet).
End Rem
Type TCEGlobalEventSet Extends TCEEventSet

	Global globalInstance:TCEGlobalEventSet

	Rem
	bbdoc: 
	End Rem
	Function getInstance:TCEGlobalEventSet()
		If Not globalInstance Then
			globalInstance = New TCEGlobalEventSet
			globalInstance.objectPtr = bmx_cegui_globaleventset_getinstance()
		End If
		Return globalInstance
	End Function

	Rem
	bbdoc: 
	End Rem
	Method subscribeEvent:TCEConnection(name:String, callback:Int(args:TCEEventArgs))
		' subscribe to the event
		Local cb:TCEEventCallback = ce_event_handler.AddCallback(Self, name, callback)
		Return TCEConnection._create(bmx_cegui_globaleventset_subscribeevent(objectPtr, _convertMaxToUTF8(name), cb.callbackPtr))
	End Method
	

End Type


Rem
bbdoc: The System type is the CEGUI type that provides access to all other elements in this system.
about: A global instance of TCESystem is created when Init_CEGUI() is called.
End Rem
Type TCESystem Extends TCEEventSet

	Global cegui_systemPtr:Byte Ptr

	Global resourceProvider:TCEResourceProvider

	' nothing to see here	
	Function getRenderer:TCERenderer()
		' maybe one day we'll handle renderers....
	End Function
	
	Rem
	bbdoc: Render the GUI.
	End Rem
	Function renderGUI()
		bmx_cegui_system_renderGUI(cegui_systemPtr)
	End Function
	
	Rem
	bbdoc: Set the default font to be used by the system.
	End Rem
	Function setDefaultFont(font:Object)
		If TCEFont(font) Then
			bmx_cegui_system_setdefaultfont(cegui_systemPtr, TCEFont(font).objectPtr)
		ElseIf String(font) Then
			bmx_cegui_system_setdefaultfonttxt(cegui_systemPtr, _convertMaxToUTF8(String(font)))
		End If
	End Function

	Rem
	bbdoc: Returns the default Font for the GUI system.
	End Rem
	Method getDefaultFont:TCEFont()
		Return TCEFont._create(bmx_cegui_system_getDefaultFont(cegui_systemPtr))
	End Method
	
	Rem
	bbdoc: Causes a full re-draw next time renderGUI() is called.
	End Rem
	Method signalRedraw()
		bmx_cegui_system_signalredraw(cegui_systemPtr)
	End Method
	
	Rem
	bbdoc: Returns a boolean value to indicate whether a full re-draw is requested next time renderGUI() is called.
	End Rem
	Method isRedrawRequested:Int()
		Return bmx_cegui_system_isredrawrequested(cegui_systemPtr)
	End Method
	
	Rem
	bbdoc: Returns the current timeout for generation of single-click events.
	about: A single-click is defined here as a button being pressed and then released.
	End Rem
	Method getSingleClickTimeout:Double()
		Return bmx_cegui_system_getsingleclicktimeout(cegui_systemPtr)
	End Method
	
	Rem
	bbdoc: Returns the current timeout for generation of multi-click events.
	about: A multi-click event is a double-click, or a triple-click. The value returned here is the maximum
	allowable time between mouse button down events for which a multi-click event will be generated.
	End Rem
	Method getMultiClickTimeout:Double()
		Return bmx_cegui_system_getmulticlicktimeout(cegui_systemPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the allowable mouse movement tolerance used when generating multi-click events.
	about: This size defines an area with the mouse at the centre. The mouse must stay within the tolerance
	defined for a multi-click (double click, or triple click) event to be generated.
	End Rem
	Method getMultiClickToleranceAreaSize(width:Float Var, height:Float Var)
		bmx_cegui_system_getmulticlicktoleranceareasize(cegui_systemPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Sets the timeout used for generation of single-click events.
	about: A single-click is defined here as a button being pressed and then released.
	<p>
	A timeout value of 0 indicates infinity and so no timeout occurrs; as long as the mouse is in
	the prescribed area, a mouse button 'clicked' event will therefore always be raised.
	</p>
	End Rem
	Method setSingleClickTimeout(timeout:Double)
		bmx_cegui_system_setsingleclicktimeout(cegui_systemPtr, timeout)
	End Method
	
	Rem
	bbdoc: Sets the timeout to be used for the generation of multi-click events.
	about: A multi-click event is a double-click, or a triple-click. The value returned here is the
	maximum allowable time between mouse button down events for which a multi-click event will be generated.
	<p>
	A timeout value of 0 indicates infinity and so no timeout occurrs; as long as the mouse is in the
	prescribed area, an appropriate mouse button event will therefore always be raised.
	</p>
	End Rem
	Method setMultiClickTimeout(timeout:Double)
		bmx_cegui_system_setmulticlicktimeout(cegui_systemPtr, timeout)
	End Method
	
	Rem
	bbdoc: Sets the size of the allowable mouse movement tolerance used when generating multi-click events.
	about: This size defines an area with the mouse at the centre. The mouse must stay within the tolerance
	defined for a multi-click (double click, or triple click) event to be generated.
	End Rem
	Method setMultiClickToleranceAreaSize(width:Float, height:Float)
		bmx_cegui_system_setmulticlicktoleranceareasize(cegui_systemPtr, width, height)
	End Method
	
	Rem
	bbdoc: Sets the image to be used as the default mouse cursor.
	End Rem
	Function setDefaultMouseCursor(a:String, b:String)
		bmx_cegui_system_setDefaultMouseCursor(cegui_systemPtr, _convertMaxToUTF8(a), _convertMaxToUTF8(b))
	End Function
	
	Rem
	bbdoc: Sets the active GUI sheet (root) window.
	returns: The window that was previously set as the GUI root.
	End Rem
	Function setGUISheet:TCEWindow(window:TCEWindow)
		Return TCEWindow(bmx_cegui_system_setGUISheet(cegui_systemPtr, window.objectPtr))
	End Function
	
	Rem
	bbdoc: Injects time pulses into the system.
	about: Accepts a value indicating the amount of time passed, in seconds, since the last time this method was called.
	End Rem 
	Function injectTimePulse(time:Float)
		bmx_cegui_system_injectTimePulse(cegui_systemPtr ,time)
	End Function
	
	Function injectMousePosition(x:Int, y:Int)
		bmx_cegui_system_injectMousePosition(cegui_systemPtr, x, y)
	End Function
	
	Function injectMouseButtonUp(button:Int)
		bmx_cegui_system_injectMouseButtonUp(cegui_systemPtr, button)
	End Function

	Function injectMouseButtonDown(button:Int)
		bmx_cegui_system_injectMouseButtonDown(cegui_systemPtr, button)
	End Function
	
	Function injectMouseWheelChange(delta:Int)
		bmx_cegui_system_injectMouseWheelChange(cegui_systemPtr, delta)
	End Function
	
	Function injectKeyDown:Int(key:Int)
		Return bmx_cegui_system_injectkeydown(cegui_systemPtr, key)
	End Function

	Function injectKeyUp:Int(key:Int)
		Return bmx_cegui_system_injectkeyup(cegui_systemPtr, key)
	End Function

	Function injectChar:Int(key:Int)
		Return bmx_cegui_system_injectchar(cegui_systemPtr, key)
	End Function

	Rem
	bbdoc: Sets the system default Tooltip object.
	about: This value may be NULL to indicate that no default Tooltip will be available.
	End Rem
	Function setDefaultTooltip(tooltip:Object)
		If TCETooltip(tooltip) Or Not tooltip Then
			If tooltip Then
				bmx_cegui_system_setdefaulttooltip(cegui_systemPtr, TCETooltip(tooltip).objectPtr)
			Else
				bmx_cegui_system_setdefaulttooltip(cegui_systemPtr, Null)
			End If
		Else If String(tooltip) Then
			bmx_cegui_system_setdefaulttooltiptext(cegui_systemPtr, _convertMaxToUTF8(String(tooltip)))
		End If
	End Function
	
	Rem
	bbdoc: Returns the system default tooltip.
	about: May return Null.
	End Rem
	Function getDefaultTooltip:TCETooltip()
		Return TCETooltip(bmx_cegui_system_getdefaulttooltip(cegui_systemPtr))
	End Function
	
	Rem
	bbdoc: Returns the current mouse movement scaling factor.
	End Rem
	Function getMouseMoveScaling:Float()
		Return bmx_cegui_system_getmousemovescaling(cegui_systemPtr)
	End Function
	
	Rem
	bbdoc: Sets the current mouse movement scaling factor.
	End Rem
	Function setMouseMoveScaling(scaling:Float)
		bmx_cegui_system_setmousemovescaling(cegui_systemPtr, scaling)
	End Function
	
	Function getResourceProvider:TCEResourceProvider()
		If resourceProvider Then
			Return resourceProvider
		End If
		
		Return TCEResourceProvider(bmx_cegui_system_getresourceprovider(cegui_systemPtr))
	End Function
	
End Type

Rem
bbdoc: The default resource provider.
End Rem
Type TCEDefaultResourceProvider Extends TCEResourceProvider

	Function _create:TCEDefaultResourceProvider(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEDefaultResourceProvider = New TCEDefaultResourceProvider
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method setResourceGroupDirectory(resourceGroup:String, directory:String)
		bmx_cegui_defaultresourceprovider_setresourcegroupdirectory(objectPtr, _convertMaxToUTF8(resourceGroup), _convertMaxToUTF8(directory))
	End Method
	
	Method getResourceGroupDirectory:String(resourceGroup:String)
		Return bmx_cegui_defaultresourceprovider_getresourcegroupdirectory(objectPtr, _convertMaxToUTF8(resourceGroup))
	End Method
	
	Method clearResourceGroupDirectory(resourceGroup:String)
		bmx_cegui_defaultresourceprovider_clearresourcegroupdirectory(objectPtr, _convertMaxToUTF8(resourceGroup))
	End Method
	
End Type

Rem
bbdoc: A GUI Scheme is a high-level construct that loads and initialises various lower-level objects and registers them within the system for usage.
about: So, for example, a Scheme might create some Imageset objects, some Font objects, and register
a collection of WindowFactory objects within the system which would then be in a state to serve those
elements to client code.
End Rem
Type TCEScheme

	Field objectPtr:Byte Ptr
	
	Function _create:TCEScheme(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScheme = New TCEScheme
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Loads all resources for this scheme.
	End Rem
	Method loadResources()
		bmx_cegui_scheme_loadresources(objectPtr)
	End Method
	
	Rem
	bbdoc: Unloads all resources for this scheme.
	about: This should be used very carefully.
	End Rem
	Method unloadResources()
		bmx_cegui_scheme_unloadresources(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the resources for this Scheme are all loaded.
	End Rem
	Method resourcesLoaded:Int()
		Return bmx_cegui_scheme_resourcesloaded(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the name of this Scheme. 
	End Rem
	Method getName:String()
		Return bmx_cegui_scheme_getname(objectPtr)
	End Method

	Rem
	bbdoc: Sets the default resource group to be used when loading scheme xml data.
	End Rem
	Function setDefaultResourceGroup(resourceGroup:String)
		bmx_cegui_scheme_setdefaultresourcegroup(_convertMaxToUTF8(resourceGroup))
	End Function
	
	Rem
	bbdoc: Returns the default resource group currently set for Schemes. 
	End Rem
	Function getDefaultResourceGroup:String()
		Return bmx_cegui_scheme_getdefaultresourcegroup()
	End Function

End Type

Rem
bbdoc: The WindowManager describes an object that manages creation and lifetime of Window objects.
about: The WindowManager is the means by which Window objects are created and destroyed. For each sub-type of
Window that is to be created, there must exist a WindowFactory object which is registered with the
WindowFactoryManager. Additionally, the WindowManager tracks every Window object created, and can be used to access
those Window objects by name.
End Rem
Type TCEWindowManager

	Global windowManagerPtr:Byte Ptr

	Rem
	bbdoc: Creates a set of windows (a Gui layout) from the information in the specified XML file.
	End Rem
	Function loadWindowLayout:TCEWindow(filename:String, namePrefix:String = "", resourceGroup:String = "")
		Return TCEWindow(bmx_cegui_windowmanager_loadWindowLayout(windowManagerPtr, _convertMaxToUTF8(filename), ..
			_convertMaxToUTF8(namePrefix), _convertMaxToUTF8(resourceGroup)))
	End Function

	Rem
	bbdoc: Returns a reference to the specified Window object.
	End Rem
	Function getWindow:TCEWindow(name:String)
		Return TCEWindow(bmx_cegui_windowmanager_getwindow(windowManagerPtr, _convertMaxToUTF8(name)))
	End Function
	
	Rem
	bbdoc: Examines the list of Window objects to see if one exists with the given name. 
	returns: True if a Window object was found with a name matching @name. False if no matching Window object was found.
	End Rem
	Function isWindowPresent:Int(name:String)
		Return bmx_cegui_windowmanager_iswindowpresent(windowManagerPtr, _convertMaxToUTF8(name))
	End Function
	
	Rem
	bbdoc: Destroys all Window objects within the system. 
	End Rem
	Function destroyAllWindows()
		bmx_cegui_windowmanager_destroyallwindows(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Destroys the specified Window. 
	about: This function accepts either the window name (String), or window object (TCEWindow).
	End Rem
	Function destroyWindow(window:Object)
		If TCEWindow(window) Then
			bmx_cegui_windowmanager_destroywindowwindow(windowManagerPtr, TCEWindow(window).objectPtr)
		ElseIf String(window) Then
			bmx_cegui_windowmanager_destroywindowname(windowManagerPtr, _convertMaxToUTF8(String(window)))
		End If
	End Function
	
	Rem
	bbdoc: Renames a window.
	about: This function accepts either the window name (String), or window object (TCEWindow).
	End Rem
	Function renameWindow(window:Object, newName:String)
		If TCEWindow(window) Then
			bmx_cegui_windowmanager_renamewindowwindow(windowManagerPtr, TCEWindow(window).objectPtr, _convertMaxToUTF8(newName))
		ElseIf String(window) Then
			bmx_cegui_windowmanager_renamewindowname(windowManagerPtr, _convertMaxToUTF8(String(window)), _convertMaxToUTF8(newName))
		End If
	End Function

	Rem
	bbdoc: Creates a new Window object of the specified type, and gives it the specified unique name.
	returns: The newly created Window object.
	End Rem
	Function CreateWindow:TCEWindow(windowType:String, name:String = "")
		Return TCEWindow(bmx_cegui_windowmanager_createwindow(windowManagerPtr, _convertMaxToUTF8(windowType), _convertMaxToUTF8(name)))
	End Function

	Rem
	bbdoc: Returns whether the window dead pool is empty.
	returns: True if there are no windows in the dead pool, or False if the dead pool contains 1 or more windows awaiting destruction.
	End Rem
	Function isDeadPoolEmpty:Int()
		Return bmx_cegui_windowmanager_isdeadpoolempty(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Permanently destroys any windows placed in the dead pool.
	about: It is probably not a good idea to call this from a Window based event handler if the specific window has been or is being destroyed.
	End Rem
	Function cleanDeadPool()
		bmx_cegui_windowmanager_cleandeadpool(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Puts WindowManager into the locked state.
	about: While WindowManager is in the locked state all attempts to create a Window of any type will fail with an InvalidRequestException being thrown.
	Calls to lock/unlock are recursive; if multiple calls to lock are made, WindowManager is only unlocked after a matching number of calls to unlock.
	<p>
	Note: This is primarily intended for internal use within the system.
	</p>
	End Rem
	Function lock()
		bmx_cegui_windowmanager_lock(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Puts WindowManager into the unlocked state.
	about: While WindowManager is in the locked state all attempts to create a Window of any type will fail with an InvalidRequestException being thrown.
	Calls to lock/unlock are recursive; if multiple calls to lock are made, WindowManager is only unlocked after a matching number of calls to unlock.
	<p>
	Note: This is primarily intended for internal use within the system.
	</p>
	End Rem
	Function unlock()
		bmx_cegui_windowmanager_unlock(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Returns whether WindowManager is currently in the locked state.
	returns: True to indicate WindowManager is locked and that any attempt to create Window objects will fail, or False to indicate WindowManager is unlocked and that Window objects may be created as normal.
	about: While WindowManager is in the locked state all attempts to create a Window of any type will fail with an InvalidRequestException being thrown.
	Calls to lock/unlock are recursive; if multiple calls to lock are made, WindowManager is only unlocked after a matching number of calls to unlock.
	End Rem
	Function isLocked:Int()
		Return bmx_cegui_windowmanager_islocked(windowManagerPtr)
	End Function
	
	Rem
	bbdoc: Sets the default resource group to be used when loading layouts. 
	End Rem
	Function setDefaultResourceGroup(resourceGroup:String)
		bmx_cegui_windowmanager_setdefaultresourcegroup(_convertMaxToUTF8(resourceGroup))
	End Function
	
	Rem
	bbdoc: Returns the default resource group currently set for layouts. 
	End Rem
	Function getDefaultResourceGroup:String()
		Return bmx_cegui_windowmanager_getdefaultresourcegroup()
	End Function

End Type

Rem
bbdoc: The core UI object that the the system knows about; for this reason, every other window, widget, or similar item within the system must be derived from Window.
about: This base type provides the common functionality required by all UI objects, and specifies
the minimal interface required to be implemented by derived types. 
End Rem
Type TCEWindow Extends TCEEventSet

	' window destruct callback - only enabled if we've subscribed to an event for this window.
	Field destructCB:TCEEventCallback

	Rem
	bbdoc: Window has been updated.
	End Rem
	Const EventWindowUpdated:String = "WindowUpdate"
	Rem
	bbdoc: Parent of this Window has been re-sized. 
	End Rem
	Const EventParentSized:String = "ParentSized"
	Rem
	bbdoc: Window size has changed. 
	End Rem
	Const EventSized:String = "Sized"
	Rem
	bbdoc: Window position has changed. 
	End Rem
	Const EventMoved:String = "Moved"
	Rem
	bbdoc: Text string for the Window has changed. 
	End Rem
	Const EventTextChanged:String = "TextChanged"
	Rem
	bbdoc: Font object for the Window has been changed. 
	End Rem
	Const EventFontChanged:String = "FontChanged"
	Rem
	bbdoc: Alpha blend value for the Window has changed. 
	End Rem
	Const EventAlphaChanged:String = "AlphaChanged"
	Rem
	bbdoc: Client assigned ID code for the Window has changed. 
	End Rem
	Const EventIDChanged:String = "IDChanged"
	Rem
	bbdoc: Window has been activated (has input focus). 
	End Rem
	Const EventActivated:String = "Activated"
	Rem
	bbdoc: Window has been deactivated (loses input focus). 
	End Rem
	Const EventDeactivated:String = "Deactivated"
	Rem
	bbdoc: Window has been made visible. 
	End Rem
	Const EventShown:String = "Shown"
	Rem
	bbdoc: Window has been hidden from view. 
	End Rem
	Const EventHidden:String = "Hidden"
	Rem
	bbdoc: Window has been enabled (interaction is possible). 
	End Rem
	Const EventEnabled:String = "Enabled"
	Rem
	bbdoc: Window has been disabled (interaction is no longer possible). 
	End Rem
	Const EventDisabled:String = "Disabled"
	Rem
	bbdoc: Clipping by parent mode has been modified. 
	End Rem
	Const EventClippedByParentChanged:String = "ClippingChanged"
	Rem
	bbdoc: Destruction by parent mode has been modified. 
	End Rem
	Const EventDestroyedByParentChanged:String = "DestroyedByParentChanged"
	Rem
	bbdoc: Alpha inherited from parent mode has been modified. 
	End Rem
	Const EventInheritsAlphaChanged:String = "InheritAlphaChanged"
	Rem
	bbdoc: Always on top mode has been modified. 
	End Rem
	Const EventAlwaysOnTopChanged:String = "AlwaysOnTopChanged"
	Rem
	bbdoc: Window has captured all inputs. 
	End Rem
	Const EventInputCaptureGained:String = "CaptureGained"
	Rem
	bbdoc: Window has lost it's capture on inputs. 
	End Rem
	Const EventInputCaptureLost:String = "CaptureLost"
	Rem
	bbdoc: Rendering of the Window has started. 
	End Rem
	Const EventRenderingStarted:String = "StartRender"
	Rem
	bbdoc: Rendering for the Window has finished. 
	End Rem
	Const EventRenderingEnded:String = "EndRender"
	Rem
	bbdoc: A child Window has been added. 
	End Rem
	Const EventChildAdded:String = "AddedChild"
	Rem
	bbdoc: A child window has been removed. 
	End Rem
	Const EventChildRemoved:String = "RemovedChild"
	Rem
	bbdoc: Destruction of the Window is about to begin. 
	End Rem
	Const EventDestructionStarted:String = "DestructStart"
	Rem
	bbdoc: The z-order of the window has changed. 
	End Rem
	Const EventZOrderChanged:String = "ZChanged"
	Rem
	bbdoc: A DragContainer has been dragged over this window. 
	End Rem
	Const EventDragDropItemEnters:String = "DragDropItemEnters"
	Rem
	bbdoc: A DragContainer has left this window. 
	End Rem
	Const EventDragDropItemLeaves:String = "DragDropItemLeaves"
	Rem
	bbdoc: A DragContainer was dropped on this Window. 
	End Rem
	Const EventDragDropItemDropped:String = "DragDropItemDropped"
	Rem
	bbdoc: The vertical alignment of the window has changed. 
	End Rem
	Const EventVerticalAlignmentChanged:String = "VerticalAlignmentChanged"
	Rem
	bbdoc: The vertical alignment of the window has changed. 
	End Rem
	Const EventHorizontalAlignmentChanged:String = "HorizontalAlignmentChanged"
	Rem
	bbdoc: The a new window renderer was attached. 
	End Rem
	Const EventWindowRendererAttached:String = "WindowRendererAttached"
	Rem
	bbdoc: The currently assigned window renderer was detached. 
	End Rem
	Const EventWindowRendererDetached:String = "WindowRendererDetached"
	Rem
	bbdoc: Mouse cursor has entered the Window. 
	End Rem
	Const EventMouseEnters:String = "MouseEnter"
	Rem
	bbdoc: Mouse cursor has left the Window. 
	End Rem
	Const EventMouseLeaves:String = "MouseLeave"
	Rem
	bbdoc: Mouse cursor was moved within the area of the Window. 
	End Rem
	Const EventMouseMove:String = "MouseMove"
	Rem
	bbdoc: Mouse wheel was scrolled within the Window. 
	End Rem
	Const EventMouseWheel:String = "MouseWheel"
	Rem
	bbdoc: A mouse button was pressed down within the Window. 
	End Rem
	Const EventMouseButtonDown:String = "MouseButtonDown"
	Rem
	bbdoc: A mouse button was released within the Window. 
	End Rem
	Const EventMouseButtonUp:String = "MouseButtonUp"
	Rem
	bbdoc: A mouse button was clicked (down then up) within the Window. 
	End Rem
	Const EventMouseClick:String = "MouseClick"
	Rem
	bbdoc: A mouse button was double-clicked within the Window. 
	End Rem
	Const EventMouseDoubleClick:String = "MouseDoubleClick"
	Rem
	bbdoc: A mouse button was triple-clicked within the Window. 
	End Rem
	Const EventMouseTripleClick:String = "MouseTripleClick"
	Rem
	bbdoc: A key on the keyboard was pressed. 
	End Rem
	Const EventKeyDown:String = "KeyDown"
	Rem
	bbdoc: A key on the keyboard was released. 
	End Rem
	Const EventKeyUp:String = "KeyUp"
	Rem
	bbdoc: A text character was typed on the keyboard. 
	End Rem
	Const EventCharacterKey:String = "CharacterKey"
	
	Function _create:TCEWindow(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEWindow = New TCEWindow
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Subscribes a handler to the named Event.
	returns: Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.
	about: If the named Event is not yet present in the EventSet, it is created and added.
	End Rem
	Method subscribeEvent:TCEConnection(name:String, callback:Int(args:TCEEventArgs))', handler:TCEEventHandler)
	
		' if necessary, add the destruct callback - for cleaning up our callback hooks when the window is killed.
		If Not destructCB Then
			destructCB = ce_event_handler.AddCallback(Self, EventDestructionStarted, _destructCallback)
			bmx_cegui_eventset_subscribeevent(objectPtr, _convertMaxToUTF8(EventDestructionStarted), destructCB.callbackPtr)
		End If

		' subscribe to the event
		Local cb:TCEEventCallback = ce_event_handler.AddCallback(Self, name, callback)
		Return TCEConnection._create(bmx_cegui_eventset_subscribeevent(objectPtr, _convertMaxToUTF8(name), cb.callbackPtr))
	End Method
	
	' callback for catching window destruction... we need to clean up!
	Function _destructCallback:Int(args:TCEEventArgs)
		TCEWindow(args.owner).Free()
	End Function
	
	' remove self from the callback cache, and any other tidy-up.
	Method Free()
		ce_event_handler.FreeCallbacks(Self)
		destructCB = Null
	End Method
	
	Rem
	bbdoc: Returns the type name for this Window.
	End Rem
	Method getType:String()
		Return bmx_cegui_window_gettype(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the name of this Window.
	End Rem
	Method getName:String()
		Return bmx_cegui_window_getname(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether or not this Window is set to be destroyed when its parent window is destroyed.
	End Rem
	Method isDestroyedByParent:Int()
		Return bmx_cegui_window_isdestroyedbyparent(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether or not this Window is an always on top Window. Also known as a top-most window.
	End Rem
	Method isAlwaysOnTop:Int()
		Return bmx_cegui_window_isalwaysontop(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the Window is currently disabled.
	End Rem
	Method isDisabled:Int(localOnly:Int = False)
		Return bmx_cegui_window_isdisabled(objectPtr, localOnly)
	End Method
	 
	Rem
	bbdoc: Returns true if the Window is currently visible.
	about: When true is returned from this method, it does not mean that the window is not completely obscured by other
	windows, just that the window will be processed when rendering, and is not explicitly marked as hidden.
	End Rem
	Method isVisible:Int(localOnly:Int = False)
		Return bmx_cegui_window_isvisible(objectPtr, localOnly	)
	End Method
	 
	Rem
	bbdoc: Returns true if this is the active Window.
	about: An active window is a window that may receive user inputs.
	<p>
	Mouse events are always sent to the window containing the mouse cursor regardless of what this method
	reports (unless a window has captured inputs). The active state mainly determines where send other, for
	example keyboard, inputs.
	</p>
	End Rem
	Method isActive:Int()
		Return bmx_cegui_window_isactive(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if this Window is clipped so that its rendering will not pass outside of its parent Window area.
	End Rem
	Method isClippedByParent:Int()
		Return bmx_cegui_window_isclippedbyparent(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the ID code currently assigned to this Window by client code.
	End Rem
	Method getID:Int()
		Return bmx_cegui_window_getid(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the number of child Window objects currently attached to this Window.
	End Rem
	Method getChildCount:Int()
		Return bmx_cegui_window_getchildcount(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether a Window with the specified name is currently attached to this Window as a child.
	End Rem
	Method isChild:Int(name:String)
		Return bmx_cegui_window_ischild(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Returns whether at least one window with the given ID code is attached to this Window as a child.
	End Rem
	Method isChildID:Int(ID:Int)
		Return bmx_cegui_window_ischildid(objectPtr, ID)
	End Method
	 
	Rem
	bbdoc: returns whether at least one window with the given ID code is attached to this Window or any of it's children as a child.
	about: Note: ID codes are client assigned and may or may not be unique, and as such, the return from this method
	will only have meaning to the client code.
	<p>
	WARNING! This method can be very expensive and should only be used when you have no other option available.
	If you decide to use it anyway, make sure the window hierarchy from the entry point is small.
	</p>
	End Rem
	Method isChildRecursive:Int(ID:Int)
		Return bmx_cegui_window_ischildrecursive(objectPtr, ID)
	End Method
	 
	Rem
	bbdoc: Returns true if the given Window is a child of this window.
	End Rem
	Method isChildWindow:Int(window:TCEWindow)
		Return bmx_cegui_window_ischildwindow(objectPtr, window.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the child window with the specified name.
	End Rem
	Method GetChild:TCEWindow(name:String)
		Return TCEWindow(bmx_cegui_window_getchild(objectPtr, _convertMaxToUTF8(name)))
	End Method
	 
	Rem
	bbdoc: Returns the first attached child window with the specified ID value.
	End Rem
	Method GetChildID:TCEWindow(ID:Int)
		Return TCEWindow(bmx_cegui_window_getchildid(objectPtr, ID))
	End Method
	 
	Rem
	bbdoc: Returns the first attached child window with the specified ID value. Children are traversed recursively.
	about: Contrary to the non recursive version of this method, this one will not throw an exception, but return 0 in case no child was found.
	<p>
	Note:
	WARNING! This method can be very expensive and should only be used when you have no other option available. If
	you decide to use it anyway, make sure the window hierarchy from the entry point is small.
	</p>
	End Rem
	Method getChildRecursive:TCEWindow(ID:Int)
		Return TCEWindow(bmx_cegui_window_getchildrecursive(objectPtr, ID))
	End Method
	 
	Rem
	bbdoc: Returns the child window that is attached to 'this' at the given index.
	End Rem
	Method getChildAtIdx:TCEWindow(idx:Int)
		Return TCEWindow(bmx_cegui_window_getchildatidx(objectPtr, idx))
	End Method
	 
	Rem
	bbdoc: Returns the Window that currently has input focus starting with this Window.
	End Rem
	Method getActiveChild:TCEWindow()
		Return TCEWindow(bmx_cegui_window_getactivechild(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns true if the specified Window is some ancestor of this Window.
	End Rem
	Method isAncestor:Int(name:String)
		Return bmx_cegui_window_isancestor(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Returns true if any Window with the given ID is some ancestor of this Window.
	End Rem
	Method isAncestorID:Int(ID:Int)
		Return bmx_cegui_window_isancestorid(objectPtr, ID)
	End Method
	 
	Rem
	bbdoc: Returns true if the specified Window is some ancestor of this Window.
	End Rem
	Method isAncestorWindow:Int(window:TCEWindow)
		Return bmx_cegui_window_isancestorwindow(objectPtr, window.objectPtr)
	End Method

	Rem
	bbdoc: Returns the active Font object for the Window.
	End Rem
	Method getFont:TCEFont(useDefault:Int = True)
		Return TCEFont._create(bmx_cegui_window_getfont(objectPtr, useDefault))
	End Method
	
	Rem
	bbdoc: Returns the current text for the Window
	End Rem
	Method getText:String()
		Return bmx_cegui_window_gettext(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if the Window inherits alpha from its parent(s).
	End Rem
	Method inheritsAlpha:Int()
		Return bmx_cegui_window_inheritsalpha(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current alpha value set for this Window.
	about: The alpha value set for any given window may or may not be the final alpha value that is used when
	rendering. All window objects, by default, inherit alpha from thier parent window(s) - this will blend
	child windows, relatively, down the line of inheritance. This behaviour can be overridden via the
	setInheritsAlpha() method. To return the true alpha value that will be applied when rendering, use the
	getEffectiveAlpha() method.
	End Rem
	Method GetAlpha:Float()
		Return bmx_cegui_window_getalpha(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the effective alpha value that will be used when rendering this window, taking into account inheritance of parent window(s) alpha.
	End Rem
	Method getEffectiveAlpha:Float()
		Return bmx_cegui_window_geteffectivealpha(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns a Rect that describes, unclipped, the inner rectangle for this window.
	about: The inner rectangle is typically an area that excludes some frame or other rendering that should not be
	touched by subsequent rendering.
	End Rem
	Method getUnclippedInnerRect(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_window_getunclippedinnerrect(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	 
	Rem
	bbdoc: Returns true if this Window has input captured.
	End Rem
	Method isCapturedByThis:Int()
		Return bmx_cegui_window_iscapturedbythis(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if an ancestor window has captured inputs.
	End Rem
	Method isCapturedByAncestor:Int()
		Return bmx_cegui_window_iscapturedbyancestor(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if a child window has captured inputs.
	End Rem
	Method isCapturedByChild:Int()
		Return bmx_cegui_window_iscapturedbychild(objectPtr)
	End Method
	 
	Rem
	bbdoc: Checks if the given pixel position would hit this window.
	End Rem
	Method isHit:Int(x:Float, y:Float, allowDisabled:Int = False)
		Return bmx_cegui_window_ishit(objectPtr, x, y, allowDisabled)
	End Method
	 
	Rem
	bbdoc: Returns the child Window that is hit by the given pixel position
	End Rem
	Method getChildAtPosition:TCEWindow(x:Float, y:Float)
		Return TCEWindow(bmx_cegui_window_getchildatposition(objectPtr, x, y))
	End Method
	 
	Rem
	bbdoc: Returns the child Window that is 'hit' by the given position, and is allowed to handle mouse events.
	End Rem
	Method getTargetChildAtPosition:TCEWindow(x:Float, y:Float)
		Return TCEWindow(bmx_cegui_window_gettargetchildatposition(objectPtr, x, y))
	End Method
	 
	Rem
	bbdoc: Returns the parent of this Window.
	End Rem
	Method GetParent:TCEWindow()
		Return TCEWindow(bmx_cegui_window_getparent(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the mouse cursor image to use when the mouse cursor is within this window's area.
	End Rem
	Method getMouseCursor:TCEImage(useDefault:Int = True)
		Return TCEImage(bmx_cegui_window_getmousecursor(objectPtr, useDefault))
	End Method
	 
	Rem
	bbdoc: Returns the window size in pixels.
	End Rem
	Method getPixelSize(width:Float Var, height:Float Var)
		bmx_cegui_window_getpixelsize(objectPtr, Varptr width, Varptr height)
	End Method

	' TODO
	Method getUserData:Object()
		' TODO
	End Method
	 
	Rem
	bbdoc: Return whether this window is set to restore old input capture when it loses input capture.
	about: This is only really useful for certain sub-components for widget writers.
	End Rem
	Method restoresOldCapture:Int()
		Return bmx_cegui_window_restoresoldcapture(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether z-order changes are enabled or disabled for this Window.
	End Rem
	Method isZOrderingEnabled:Int()
		Return bmx_cegui_window_iszorderingenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether this window will receive multi-click events or multiple 'down' events instead.
	End Rem
	Method wantsMultiClickEvents:Int()
		Return bmx_cegui_window_wantsmulticlickevents(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether mouse button down event autorepeat is enabled for this window.
	End Rem
	Method isMouseAutoRepeatEnabled:Int()
		Return bmx_cegui_window_ismouseautorepeatenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current auto-repeat delay setting for this window.
	End Rem
	Method getAutoRepeatDelay:Float()
		Return bmx_cegui_window_getautorepeatdelay(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current auto-repeat rate setting for this window.
	End Rem
	Method getAutoRepeatRate:Float()
		Return bmx_cegui_window_getautorepeatrate(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the window wants inputs passed to its attached child windows when the window has inputs captured.
	End Rem
	Method distributesCapturedInputs:Int()
		Return bmx_cegui_window_distributescapturedinputs(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether this Window is using the system default Tooltip for its Tooltip window.
	End Rem
	Method isUsingDefaultTooltip:Int()
		Return bmx_cegui_window_isusingdefaulttooltip(objectPtr)
	End Method
	
	Rem
	bbdoc: Activate the Window giving it input focus and bringing it to the top of all windows with the same always-on-top settig as this Window.
	End Rem
	Method activate()
		bmx_cegui_window_activate(objectPtr)
	End Method

	Rem
	bbdoc: Deactivate the window.
	about: No further inputs will be received by the window until it is re-activated either
	programmatically or by the user interacting with the gui.
	End Rem
	Method deactivate()
		bmx_cegui_window_deactivate(objectPtr)
	End Method

	Rem
	bbdoc: Sets whether this Window will be clipped by its parent window(s).
	End Rem
	Method setClippedByParent(setting:Int)
		bmx_cegui_window_setclippedbyparent(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Set the current ID for the Window.
	End Rem
	Method setID(ID:Int)
		bmx_cegui_window_setid(objectPtr, ID)
	End Method
	
	Rem
	bbdoc: Inserts the text string text into the current text string for the Window object at the position specified by position.
	End Rem
	Method insertText(text:String, position:Int)
		bmx_cegui_window_inserttext(objectPtr, _convertMaxToUTF8(text), position)
	End Method
	
	Rem
	bbdoc: Appends the string @text to the currect text string for the Window object.
	End Rem
	Method appendText(text:String)
		bmx_cegui_window_appendtext(objectPtr, _convertMaxToUTF8(text))
	End Method
	
	Rem
	bbdoc: Sets the font used by this Window.
	about: Accepts either a TCEFont object, or font name.
	End Rem
	Method setFont(font:Object)
		If TCEFont(font) Then
			bmx_cegui_window_setfont(objectPtr, TCEFont(Font).objectPtr)
		Else If String(font) Then
			bmx_cegui_window_setfontbyname(objectPtr, _convertMaxToUTF8(String(font)))
		End If
	End Method
	
	Rem
	bbdoc: Removes the named Window from this windows child list.
	about: Accepts either a TCEWindow object, or window name.
	End Rem
	Method removeChildWindow(window:Object)
		If TCEWindow(window) Then
			bmx_cegui_window_removechildwindow(objectPtr, TCEWindow(window).objectPtr)
		ElseIf String(window) Then
			bmx_cegui_window_removechildwindowname(objectPtr, _convertMaxToUTF8(String(window)))
		End If
	End Method
	
	Rem
	bbdoc: Removes the first child Window with the specified ID.
	about: If there is more than one attached Window objects with the specified ID, only the fist one encountered will be removed.
	End Rem
	Method removeChildWindowForID(ID:Int)
		bmx_cegui_window_removechildwindowforid(objectPtr, ID)
	End Method
	
	Rem
	bbdoc: Moves the Window to the top of the z order.
	about: If the Window is a non always-on-top window it is moved the the top of all other non always-on-top sibling
	windows, and the process repeated for all ancestors.
	<p>
	If the Window is an always-on-top window it is moved to the of of all sibling Windows, and the process repeated
	for all ancestors.
	</p>
	End Rem
	Method moveToFront()
		bmx_cegui_window_movetofront(objectPtr)
	End Method
	
	Rem
	bbdoc: Moves the Window to the bottom of the Z order.
	about: If the window is non always-on-top the Window is sent to the very bottom of its sibling windows and the
	process repeated for all ancestors.
	<p>
	If the window is always-on-top, the Window is sent to the bottom of all sibling always-on-top windows and the process
	repeated for all ancestors.
	</p>
	End Rem
	Method moveToBack()
		bmx_cegui_window_movetoback(objectPtr)
	End Method
	
	Rem
	bbdoc: Captures input to this window.
	End Rem
	Method captureInput:Int()
		Return bmx_cegui_window_captureinput(objectPtr)
	End Method
	
	Rem
	bbdoc: Releases input capture from this Window.
	about: If this Window does not have inputs captured, nothing happens.
	End Rem
	Method releaseInput()
		bmx_cegui_window_releaseinput(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this window will remember and restore the previous window that had inputs captured.
	End Rem
	Method setRestoreCapture(setting:Int)
		bmx_cegui_window_setrestorecapture(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the current alpha value for this window.
	about: The alpha value set for any given window may or may not be the final alpha value that is used when
	rendering. All window objects, by default, inherit alpha from thier parent window(s) - this will blend
	child windows, relatively, down the line of inheritance. This behaviour can be overridden via the
	setInheritsAlpha() method. To return the true alpha value that will be applied when rendering, use the
	getEffectiveAlpha() method.
	End Rem
	Method SetAlpha(alpha:Float)
		bmx_cegui_window_setalpha(objectPtr, alpha)
	End Method
	
	Rem
	bbdoc: Sets whether this Window will inherit alpha from its parent windows.
	End Rem
	Method setInheritsAlpha(setting:Int)
		bmx_cegui_window_setinheritsalpha(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Invalidate this window causing at least this window to be redrawn during the next rendering pass.
	End Rem
	Method invalidate()
		bmx_cegui_window_invalidate(objectPtr)
	End Method
	
	Rem
	bbdoc: Invalidate the chain of rendering surfaces from this window backwards to ensure they get properly redrawn - but doing the minimum amount of work possibe - next render. 
	End Rem
	Method invalidateRenderingSurface()
		bmx_cegui_window_invalidaterenderingsurface(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the mouse cursor image to be used when the mouse enters this window.
	End Rem
	Method setMouseCursor(image:TCEImage)
		bmx_cegui_window_setmousecursor(objectPtr, image.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the mouse cursor image to be used when the mouse enters this window.
	End Rem
	Method setMouseCursorMode(image:Int)
		bmx_cegui_window_setmousecursormode(objectPtr, image)
	End Method
	
	Rem
	bbdoc: Sets the mouse cursor image to be used when the mouse enters this window.
	End Rem
	Method setMouseCursorByName(imageset:String, imageName:String)
		bmx_cegui_window_setmousecursorbyname(objectPtr, _convertMaxToUTF8(imageset), _convertMaxToUTF8(imageName))
	End Method
	
	'method setUserData(void *user_data)
		' TODO
	'end method
	
	Rem
	bbdoc: Sets whether z-order changes are enabled or disabled for this Window.
	End Rem
	Method setZOrderingEnabled(setting:Int)
		bmx_cegui_window_setzorderingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this window will receive multi-click events or multiple 'down' events instead.
	End Rem
	Method setWantsMultiClickEvents(setting:Int)
		bmx_cegui_window_setwantsmulticlickevents(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether mouse button down event autorepeat is enabled for this window.
	End Rem
	Method setMouseAutoRepeatEnabled(setting:Int)
		bmx_cegui_window_setmouseautorepeateenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the current auto-repeat delay setting for this window.
	End Rem
	Method setAutoRepeatDelay(_delay:Float)
		bmx_cegui_window_setautorepeatdelay(objectPtr, _delay)
	End Method
	
	Rem
	bbdoc: Sets the current auto-repeat rate setting for this window.
	End Rem
	Method setAutoRepeatRate(rate:Float)
		bmx_cegui_window_setautorepeatrate(objectPtr, rate)
	End Method
	
	Rem
	bbdoc: Sets whether the window wants inputs passed to its attached child windows when the window has inputs captured.
	End Rem
	Method setDistributesCapturedInputs(setting:Int)
		bmx_cegui_window_setdistributescapturedinputs(objectPtr, setting)
	End Method
	
	' not usually called by client code
	Method notifyDragDropItemEnters(item:TCEDragContainer)
		bmx_cegui_window_notifydragdropitementers(objectPtr, item.objectPtr)
	End Method
	
	' not usually called by client code
	Method notifyDragDropItemLeaves(item:TCEDragContainer)
		bmx_cegui_window_notifydragdropitemleaves(objectPtr, item.objectPtr)
	End Method
	
	' not usually called by client code
	Method notifyDragDropItemDropped(item:TCEDragContainer)
		bmx_cegui_window_notifydragdropitemdropped(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Internal destroy method which actually just adds the window and any parent destructed child windows to the dead pool.
	End Rem
	Method destroy()
		bmx_cegui_window_destroy(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the custom Tooltip object for this Window.
	about: This value may be 0 to indicate that the Window should use the system default Tooltip object.
	End Rem
	Method setTooltip(tooltip:TCETooltip)
		bmx_cegui_window_settooltip(objectPtr, tooltip.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the custom Tooltip to be used by this Window by specifying a Window type.
	about: The Window will internally attempt to create an instance of the specified window type (which must be
	derived from the base Tooltip class). If the Tooltip creation fails, the error is logged and the Window will
	revert to using either the existing custom Tooltip or the system default Tooltip.
	End Rem
	Method setTooltipType(tooltipType:String)
		bmx_cegui_window_settooltiptype(objectPtr, _convertMaxToUTF8(tooltipType))
	End Method
	
	Rem
	bbdoc: Sets the tooltip text for this window.
	End Rem
	Method setTooltipText(tip:String)
		bmx_cegui_window_settooltiptext(objectPtr, _convertMaxToUTF8(tip))
	End Method
	
	Rem
	bbdoc: Sets whether this window inherits Tooltip text from its parent when its own tooltip text is not set.
	End Rem
	Method setInheritsTooltipText(setting:Int)
		bmx_cegui_window_setinheritstooltiptext(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this window will rise to the top of the z-order when clicked with the left mouse button.
	End Rem
	Method setRiseOnClickEnabled(setting:Int)
		bmx_cegui_window_setriseonclickenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the vertical alignment.
	about: Modifies the vertical alignment for the window. This setting affects how the windows position is
	interpreted relative to its parent. One of #VA_TOP, #VA_CENTRE, or #VA_BOTTOM.
	End Rem
	Method setVerticalAlignment(alignment:Int)
		bmx_cegui_window_setverticalaligmnent(objectPtr, alignment)
	End Method
	
	Rem
	bbdoc: Sets the horizontal alignment.
	about: Modifies the horizontal alignment for the window. This setting affects how the windows position is
	interpreted relative to its parent. One of #HA_LEFT, #HA_CENTRE or #HA_RIGHT.
	End Rem
	Method setHorizontalAlignment(alignment:Int)
		bmx_cegui_window_sethorizontalalignment(objectPtr, alignment)
	End Method
	
	Rem
	bbdoc: Sets the LookNFeel that shoule be used for this window.
	about: Once a look'n'feel has been assigned it is locked - as in cannot be changed.
	End Rem
	Method setLookNFeel(look:String)
		bmx_cegui_window_setlooknfeel(objectPtr, _convertMaxToUTF8(look))
	End Method
	
	Rem
	bbdoc: Sets the modal state for this Window.
	End Rem
	Method setModalState(state:Int)
		bmx_cegui_window_setmodalstate(objectPtr, state)
	End Method
	
	Rem
	bbdoc: Performs extended laying out of attached child windows.
	about: The system may call this at various times (like when it is resized for example), and it may be invoked
	directly where required.
	End Rem
	Method performChildWindowLayout()
		bmx_cegui_window_performchildwindowlayout(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the value a named user string, creating it as required.
	End Rem
	Method setUserString(name:String, value:String)
		bmx_cegui_window_setuserstring(objectPtr, _convertMaxToUTF8(name), _convertMaxToUTF8(value))
	End Method
	
	Rem
	bbdoc: Sets the window area.
	about: Sets the area occupied by this window. The defined area is offset from the top-left corner of this windows parent window or from
	the top-left corner of the display if this window has no parent (i.e. it is the root window).
	End Rem
	Method setAreaRel(xpos:Float, ypos:Float, width:Float, height:Float)
		bmx_cegui_window_setarearel(objectPtr, xpos, ypos, width, height)
	End Method
	
	'method setArea(const UVector2 &pos, UVector2 &size)
	'end method
	
	'method setArea(const URect &area)
	'end method

	Rem
	bbdoc: Sets the window's position.
	about: Sets the position of the area occupied by this window. The position is offset from the top-left corner
	of this windows parent window or from the top-left corner of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setPosition(x:Float, y:Float)
		bmx_cegui_window_setposition(objectPtr, x, y)
	End Method

	Rem
	bbdoc: Sets the window's position.
	about: Sets the position of the area occupied by this window. The position is offset from the top-left corner
	of this windows parent window or from the top-left corner of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setPositionU(x:TCEUDim, y:TCEUDim)
		bmx_cegui_window_setpositionu(objectPtr, x.objectPtr, y.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the window's X position.
	about: Sets the x position (left edge) of the area occupied by this window. The position is offset from
	the left edge of this windows parent window or from the left edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setXPosition(x:Float)
		bmx_cegui_window_setxposition(objectPtr, x)
	End Method

	Rem
	bbdoc: Sets the window's X position.
	about: Sets the x position (left edge) of the area occupied by this window. The position is offset from
	the left edge of this windows parent window or from the left edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setXPositionU(x:TCEUDim)
		bmx_cegui_window_setxpositionu(objectPtr, x.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the window's Y position.
	about: Sets the y position (top edge) of the area occupied by this window. The position is offset from
	the top edge of this windows parent window or from the top edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setYPosition(y:Float)
		bmx_cegui_window_setyposition(objectPtr, y)
	End Method

	Rem
	bbdoc: Sets the window's Y position.
	about: Sets the y position (top edge) of the area occupied by this window. The position is offset from
	the top edge of this windows parent window or from the top edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setYPositionU(y:TCEUDim)
		bmx_cegui_window_setypositionu(objectPtr, y.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the window's size.
	about: Sets the size of the area occupied by this window.
	End Rem
	Method setSize(width:Float, height:Float)
		bmx_cegui_window_setsize(objectPtr, width, height)
	End Method

	Rem
	bbdoc: Sets the window's size.
	about: Sets the size of the area occupied by this window.
	End Rem
	Method setSizeU(width:TCEUDim, height:TCEUDim)
		bmx_cegui_window_setsizeu(objectPtr, width.objectPtr, height.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the window's width.
	about: Sets the width of the area occupied by this window.
	End Rem
	Method setWidth(width:Float)
		bmx_cegui_window_setwidth(objectPtr, width)
	End Method

	Rem
	bbdoc: Sets the window's width.
	about: Sets the width of the area occupied by this window.
	End Rem
	Method setWidthU(width:TCEUDim)
		bmx_cegui_window_setwidthu(objectPtr, width.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the window's height.
	about: Sets the height of the area occupied by this window.
	End Rem
	Method setHeight(height:Float)
		bmx_cegui_window_setheight(objectPtr, height)
	End Method

	Rem
	bbdoc: Sets the window's height.
	about: Sets the height of the area occupied by this window.
	End Rem
	Method setHeightU(height:TCEUDim)
		bmx_cegui_window_setheightu(objectPtr, height.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the window's maximum size.
	about: Sets the maximum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method setMaxSize(width:Float, height:Float)
		bmx_cegui_window_setmaxsize(objectPtr, width, height)
	End Method
	 
	Rem
	bbdoc: Sets the window's minimum size.
	about: Sets the minimum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method setMinSize(width:Float, height:Float)
		bmx_cegui_window_setminsize(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Adds the specified Window as a child of this Window.
	about: If the Window window is already attached to a Window, it is detached before being added to this Window.
	End Rem
	Method addChildWindow(window:Object)
		If TCEWindow(window) Then
			bmx_cegui_window_addchildwindowwindow(objectPtr, TCEWindow(window).objectPtr)
		Else If String(window) Then
			bmx_cegui_window_addchildwindow(objectPtr, _convertMaxToUTF8(String(window)))
		End If
	End Method
	
	Rem
	bbdoc: Hides the Window.
	about: If the window is the active window, it will become deactivated as a result of being hidden.
	End Rem
	Method hide()
		bmx_cegui_window_hide(objectPtr)
	End Method
	
	Rem
	bbdoc: Shows the Window.
	about: Showing a window does not automatically activate the window. If you want the window to also become
	active you will need to call the Window::activate member also.
	End Rem
	Method show()
		bmx_cegui_window_show(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the Tooltip object used by this Window.
	about: The value returned may point to the system default Tooltip, a custom Window specific Tooltip, or be NULL.
	End Rem
	Method getTooltip:TCETooltip()
		Return TCETooltip(bmx_cegui_window_gettooltip(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the custom tooltip type.
	End Rem
	Method getTooltipType:String()
		Return bmx_cegui_window_gettooltiptype(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current tooltip text set for this Window.
	End Rem
	Method getTooltipText:String()
		Return bmx_cegui_window_gettooltiptext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window inherits Tooltip text from its parent when its own tooltip text is not set.
	End Rem
	Method inheritsTooltipText:Int()
		Return bmx_cegui_window_inheritstooltiptext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window will rise to the top of the z-order when clicked with the left mouse button.
	End Rem
	Method isRiseOnClickEnabled:Int()
		Return bmx_cegui_window_isriseonclickenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window was inherited from the given class name at some point in the inheritance hierarchy.
	End Rem
	Method testClassName:Int(className:String)
		Return bmx_cegui_window_testclassname(objectPtr, _convertMaxToUTF8(className))
	End Method
	
	Rem
	bbdoc: Gets the vertical alignment.
	returns: The vertical alignment for the window. This setting affects how the windows position is interpreted relative to its parent.
	End Rem
	Method getVerticalAlignment:Int()
		Return bmx_cegui_window_getverticalalignment(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the horizontal alignment.
	returns: the horizontal alignment for the window. This setting affects how the windows position is interpreted relative to its parent.
	End Rem
	Method getHorizontalAlignment:Int()
		Return bmx_cegui_window_gethorizontalalignment(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the name of the LookNFeel assigned to this window.
	End Rem
	Method getLookNFeel:String()
		Return bmx_cegui_window_getlooknfeel(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets whether or not this Window is the modal target.
	End Rem
	Method getModalState:Int()
		Return bmx_cegui_window_getmodalstate(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns a named user string.
	End Rem
	Method getUserString:String(name:String)
		Return bmx_cegui_window_getuserstring(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns whether a user string with the specified name exists.
	End Rem
	Method isUserStringDefined:Int(name:String)
		Return bmx_cegui_window_isuserstringdefined(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the active sibling window.
	about: This searches the immediate children of this window's parent, and returns the active window. The method will
	return this if we are the immediate child of our parent that is active. If our parent is not active, or if no
	immediate child of our parent is active then Null is returned. If this window has no parent, and this window is not
	active then Null is returned, else this is returned.
	End Rem
	Method getActiveSibling:TCEWindow()
		Return TCEWindow(bmx_cegui_window_getactivesibling(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the pixel size of the parent element.
	End Rem
	Method getParentPixelSize(width:Float Var, height:Float Var)
		bmx_cegui_window_getparentpixelsize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Returns the pixel Width of the parent element.
	End Rem
	Method getParentPixelWidth:Float()
		Return bmx_cegui_window_getparentpixelwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the pixel Height of the parent element.
	End Rem
	Method getParentPixelHeight:Float()
		Return bmx_cegui_window_getparentpixelheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window should ignore mouse event and pass them through to and other windows behind it.
	about: In effect making the window transparent to the mouse.
	End Rem
	Method isMousePassThroughEnabled:Int()
		Return bmx_cegui_window_ismousepassthroughenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window is an auto-child window.
	about: All auto-child windows have "__auto_" in their name, but this is faster. 
	End Rem
	Method isAutoWindow:Int()
		Return bmx_cegui_window_isautowindow(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window is allowed to write XML. 
	End Rem
	Method isWritingXMLAllowed:Int()
		Return bmx_cegui_window_iswritingxmlallowed(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this Window object will receive events generated by the drag and drop support in the system.
	End Rem
	Method isDragDropTarget:Int()
		Return bmx_cegui_window_isdragdroptarget(objectPtr)
	End Method
	
	Rem
	bbdoc: Renames the window.
	End Rem
	Method rename(newName:String)
		bmx_cegui_window_rename(objectPtr, _convertMaxToUTF8(newName))
	End Method
	
	Rem
	bbdoc: Sets whether or not this Window will automatically be destroyed when its parent Window is destroyed.
	End Rem
	Method setDestroyedByParent(setting:Int)
		bmx_cegui_window_setdestroyedbyparent(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this window is always on top, or not.
	End Rem
	Method setAlwaysOnTop(setting:Int)
		bmx_cegui_window_setalwaysontop(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this window is enabled or disabled.
	about: A disabled window normally can not be interacted with, and may have different rendering.
	End Rem
	Method setEnabled(setting:Int)
		bmx_cegui_window_setenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Enables the Window to allow interaction.
	End Rem
	Method enable()
		bmx_cegui_window_enable(objectPtr)
	End Method
	
	Rem
	bbdoc: Disables the Window to prevent interaction.
	End Rem
	Method disable()
		bmx_cegui_window_disable(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the Window is visible or hidden.
	about: Hiding the active window will cause that window to become deactivated. Showing a window does not,
	however, automatically cause that window to become the active window (call Window::activate after
	making the window visible to activate it).
	End Rem
	Method setVisible(setting:Int)
		bmx_cegui_window_setvisible(objectPtr, setting)
	End Method

	Rem
	bbdoc: Sets the current text string for the Window.
	End Rem
	Method setText(text:String)
		bmx_cegui_window_settext(objectPtr, _convertMaxToUTF8(text))
	End Method

	Method getArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		' TODO
	End Method
	
	Method getPosition(x:Float Var, y:Float Var)
		' TODO
	End Method
	
	Rem
	bbdoc: Gets the window's X position.
	about: Gets the x position (left edge) of the area occupied by this window. The position is offset from the
	left edge of this windows parent window or from the left edge of the display if this window has no
	parent (i.e. it is the root window).
	End Rem
	Method getXPosition:Float()
		Return bmx_cegui_window_getxposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the window's Y position.
	about: Gets the y position (top edge) of the area occupied by this window. The position is offset from the
	top edge of this windows parent window or from the top edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method getYPosition:Float()
		Return bmx_cegui_window_getyposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the window's size.
	about: Gets the size of the area occupied by this window.
	End Rem
	Method getSize(w:Float Var, h:Float Var)
		bmx_cegui_window_getsize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Gets the window's width.
	about: Gets the width of the area occupied by this window.
	End Rem
	Method getWidth:Float()
		Return bmx_cegui_window_getwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the window's height.
	about: Gets the height of the area occupied by this window.
	End Rem
	Method getHeight:Float()
		Return bmx_cegui_window_getheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the window's maximum size.
	about: Gets the maximum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method getMaxSize(w:Float Var, h:Float Var)
		bmx_cegui_window_getmaxsize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Gets the window's minimum size.
	about: Gets the minimum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method getMinSize(w:Float Var, h:Float Var)
		bmx_cegui_window_getminsize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Causes the Window object to render itself and all of it's attached children.
	End Rem
	Method render()
		bmx_cegui_window_render(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this window should ignore mouse events and pass them through to any windows behind it.
	about: In effect making the window transparent to the mouse.
	End Rem
	Method setMousePassThroughEnabled(setting:Int)
		bmx_cegui_window_setmousepassthroughenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Assigns the WindowRenderer to specify the Look'N'Feel specification to be used.
	about: Once a window renderer has been assigned it is locked - as in cannot be changed.
	End Rem
	Method setWindowRenderer(name:String)
		bmx_cegui_window_setwindowrenderer(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Gets the factory name of the currently assigned WindowRenderer. (Look'N'Feel specification).
	End Rem
	Method getWindowRendererName:String()
		Return bmx_cegui_window_getwindowrenderername(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this window is allowed to write XML. 
	End Rem
	Method setWritingXMLAllowed(allow:Int)
		bmx_cegui_window_setwritingxmlallowed(objectPtr, allow)
	End Method
	
	Rem
	bbdoc: Recursively informs all children that the screen area has changed, and needs to be re-cached. 
	End Rem
	Method notifyScreenAreaChanged()
		bmx_cegui_window_notifyscreenareachanged(objectPtr)
	End Method
	
	Rem
	bbdoc: Changes the widget's falagard type, thus changing its look'n'feel and optionally its renderer in the process.
	End Rem
	Method setFalagardType(_type:String, rendererType:String = "")
		bmx_cegui_window_setfalagardtype(objectPtr, _convertMaxToUTF8(_type), _convertMaxToUTF8(rendererType))
	End Method
	
	Rem
	bbdoc: Specifies whether this Window object will receive events generated by the drag and drop support in the system. 
	End Rem
	Method setDragDropTarget(setting:Int)
		bmx_cegui_window_setdragdroptarget(objectPtr, setting)
	End Method

	' properties
	Rem
	bbdoc: Removes a Property from the PropertySet.
	End Rem
	Method removeProperty(name:String)
		bmx_cegui_window_removeproperty(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Removes all Property objects from the PropertySet. 
	End Rem
	Method clearProperties()
		bmx_cegui_window_clearproperties(objectPtr)
	End Method
	
	Rem
	bbdoc: Checks to see if a Property with the given name is in the PropertySet. 
	End Rem
	Method isPropertyPresent:Int(name:String)
		Return bmx_cegui_window_ispropertypresent(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the help text for the specified Property. 
	End Rem
	Method getPropertyHelp:String(name:String)
		Return bmx_cegui_window_getpropertyhelp(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Gets the current value of the specified Property. 
	End Rem
	Method getProperty:String(name:String)
		Return bmx_cegui_window_getproperty(objectPtr, _convertMaxToUTF8(name))
	End Method

	Rem
	bbdoc: Sets the current value of a Property. 
	End Rem
	Method setProperty(name:String, value:String)
		bmx_cegui_window_setproperty(objectPtr, _convertMaxToUTF8(name), _convertMaxToUTF8(value))
	End Method
	
	Rem
	bbdoc: Returns whether a Property is at it's default value. 
	End Rem
	Method isPropertyDefault:Int(name:String)
		Return bmx_cegui_window_ispropertydefault(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the default value of a Property as a String. 
	End Rem
	Method getPropertyDefault:String(name:String)
		Return bmx_cegui_window_getpropertydefault(objectPtr, _convertMaxToUTF8(name))
	End Method
	

End Type

Type TCEEvent

	Global mx:Int, my:Int, mwheel:Int
	Global mb1:Int, mb2:Int, mb3:Int
	

	Function mouseEvents(x:Int, y:Int, wheel:Int = 0)
	
		Local elapsed:Float = (MilliSecs() - cegui_startTime)/1000.0
		
		If x <> mx Or y <> my Then
		
			TCESystem.injectTimePulse(elapsed)
			TCESystem.injectMousePosition(x, y)
			mx = x
			my = y
		
		End If
		
		If wheel <> mwheel Then
			If wheel < mwheel Then
				TCESystem.injectMouseWheelChange((mwheel - wheel) * -1)
			Else
				TCESystem.injectMouseWheelChange(wheel - mwheel)
			End If
			mwheel = wheel
		End If
		
		If mb1 Then
			If Not MouseDown(1) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonUp(1)
				mb1 = 0
			End If
		Else
			If MouseDown(1) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonDown(1)
				mb1 = 1
			End If
		End If
		
		If mb2 Then
			If Not MouseDown(2) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonUp(2)
				mb2 = 0
			End If
		Else
			If MouseDown(2) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonDown(2)
				mb2 = 1
			End If
		End If

		If mb3 Then
			If Not MouseDown(3) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonUp(3)
				mb3 = 0
			End If
		Else
			If MouseDown(3) Then
				TCESystem.injectTimePulse(elapsed)
				TCESystem.injectMouseButtonDown(3)
				mb3 = 1
			End If
		End If
		
	End Function


	Function Keyhook:Object( id:Int, data:Object, context:Object )

		Local ev:TEvent=TEvent(data)
		If Not ev Return data
		
		Select ev.id
			Case EVENT_KEYREPEAT
				TCESystem.injectKeyDown(ev.data)
			Case EVENT_KEYDOWN
				TCESystem.injectKeyDown(ev.data)
			Case EVENT_KEYUP
				TCESystem.injectKeyUp(ev.data)
			Case EVENT_KEYCHAR
				TCESystem.injectChar(ev.data)
		End Select
		
		Return data
	End Function
	
End Type



Rem
bbdoc: Default implementation for the Logger.
about: If you want to redirect CEGUI logs to some place other than a text file, implement your own TCECustomLogger implementation
and create a object of the your type before creating the CEGUI::System singleton.
End Rem
Type TCELogger

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Returns the current logger.
	End Rem
	Function GetLogger:TCELogger()
		If TCECustomLogger.logger Then
			Return TCECustomLogger.logger
		Else
			Return New TCELogger
		End If
	End Function
	
	Rem
	bbdoc: Sets the level of logging information that will get out to the log file.
	about: One of LOG_ERRORS, LOG_WARNINGS, LOG_STANDARD, LOG_INFORMATIVE or LOG_INSANE.
	End Rem
	Method setLoggingLevel(level:Int)
		bmx_cegui_logger_setlogginglevel(level)
	End Method
	
	Rem
	bbdoc: Sets the name of the log file where all subsequent log entries should be written.
	End Rem
	Method setLogFilename(filename:String, append:Int = False)
		bmx_cegui_logger_setlogfilename(_convertMaxToUTF8(filename), append)
	End Method

	Rem
	bbdoc: Returns the current logging level setting.
	about: One of LOG_ERRORS, LOG_WARNINGS, LOG_STANDARD, LOG_INFORMATIVE or LOG_INSANE.
	End Rem	
	Method getLoggingLevel:Int()
		Return bmx_cegui_logger_getlogginglevel()
	End Method

	Rem
	bbdoc: Adds an event to the log. 
	about: Where @level is one of LOG_ERRORS, LOG_WARNINGS, LOG_STANDARD, LOG_INFORMATIVE or LOG_INSANE.
	End Rem
	Method logEvent(message:String, level:Int = LOG_STANDARD)	
		bmx_cegui_logger_logevent(_convertMaxToUTF8(message), level)
	End Method
	
End Type

Rem
bbdoc: A custom logger.
about: Override setLogFilename() and logEvent() in your own implementation.
End Rem
Type TCECustomLogger Extends TCELogger

	Global logger:TCECustomLogger

	' creates a new instance of a custom logger
	Method New()
		If logger Then
			bmx_cegui_customlogger_delete(logger.objectPtr)
		End If
		
		logger = Self
		objectPtr = bmx_cegui_customlogger_create(Self)
	End Method
	
	Rem
	bbdoc: Set the name of the log file where all subsequent log entries should be written.
	about: The interpretation of file name may differ depending on the concrete logger implementation. 
	<p>
	Override this method to implement your own functionality.
	</p>
	End Rem
	Method setLogFilename(filename:String, append:Int = False)
		Super.setLogFilename(filename, append)
	End Method
	
	Rem
	bbdoc: Adds an event to the log.
	about: Override this method to implement your own functionality.
	End Rem
	Method logEvent(message:String, level:Int = LOG_STANDARD)	
		Super.logEvent(message, level)
	End Method
	
	' callback hook for custom logging
	Function _logEvent(instance:TCECustomLogger, message:Byte Ptr, level:Int)
		instance.logEvent(_convertUTF8ToMax(message), level)
	End Function
	
End Type

Rem
bbdoc: Holds details of colours for the four corners of a rectangle.
End Rem
Type TCEColourRect

	Field objectPtr:Byte Ptr

	Function _create:TCEColourRect(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEColourRect = New TCEColourRect
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new ColourRect using the provided colour.
	End Rem
	Function CreateColourRect:TCEColourRect(colour:TCEColour)
		Return New TCEColourRect.Create(colour)
	End Function
	
	Rem
	bbdoc: Creates a new ColourRect using the provided colour.
	End Rem
	Method Create:TCEColourRect(colour:TCEColour)
		bmx_cegui_colourrect_create(colour.objectPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Set the alpha value to use for all four corners of the ColourRect.
	End Rem
	Method SetAlpha(alpha:Float)
		bmx_cegui_colourrect_setalpha(objectPtr, alpha)
	End Method
	 
	Rem
	bbdoc: Set the alpha value to use for the top edge of the ColourRect.
	End Rem
	Method setTopAlpha(alpha:Float)
		bmx_cegui_colourrect_settopalpha(objectPtr, alpha)
	End Method
	 
	Rem
	bbdoc: Set the alpha value to use for the bottom edge of the ColourRect.
	End Rem
	Method setBottomAlpha(alpha:Float)
		bmx_cegui_colourrect_setbottomalpha(objectPtr, alpha)
	End Method
	 
	Rem
	bbdoc: Set the alpha value to use for the left edge of the ColourRect.
	End Rem
	Method setLeftAlpha(alpha:Float)
		bmx_cegui_colourrect_setleftalpha(objectPtr, alpha)
	End Method
	 
	Rem
	bbdoc: Set the alpha value to use for the right edge of the ColourRect.
	End Rem
	Method setRightAlpha(alpha:Float)
		bmx_cegui_colourrect_setrightalpha(objectPtr, alpha)
	End Method
	 
	Rem
	bbdoc: Determinate the ColourRect is monochromatic or variegated.
	End Rem
	Method isMonochromatic:Int()
		Return bmx_cegui_colourrect_ismonochromatic(objectPtr)
	End Method
	 
	Rem
	bbdoc: Gets a portion of this ColourRect as a subset ColourRect.
	End Rem
	Method getSubRectangle:TCEColourRect(_left:Float, _right:Float, _top:Float, _bottom:Float)
		Return TCEColourRect._create(bmx_cegui_colourrect_getsubrectangle(objectPtr, _left, _right, _top, _bottom))
	End Method
	 
	Rem
	bbdoc: Get the colour at a point in the rectangle.
	End Rem
	Method getColourAtPoint:TCEColour(x:Float, y:Float)
		Return TCEColour._create(bmx_cegui_colourrect_getcolouratpoint(objectPtr, x, y))
	End Method
	 
	Rem
	bbdoc: Set the colour of all four corners simultaneously.
	End Rem
	Method setColours(col:TCEColour)
		bmx_cegui_colourrect_setcolours(objectPtr, col.objectPtr)
	End Method
	 
	Rem
	bbdoc: Module the alpha components of each corner's colour by a constant.
	End Rem
	Method modulateAlpha(alpha:Float)
		bmx_cegui_colourrect_modulatealpha(objectPtr, alpha)
	End Method
	
	Rem
	bbdoc: Sets the top-left corner's colour.
	End Rem
	Method setTopLeft(colour:TCEColour)
		bmx_cegui_colourrect_settopleft(objectPtr, colour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the top-right corner's colour.
	End Rem
	Method setTopRight(colour:TCEColour)
		bmx_cegui_colourrect_settopright(objectPtr, colour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the bottom-left corner's colour.
	End Rem
	Method setBottomLeft(colour:TCEColour)
		bmx_cegui_colourrect_setbottomleft(objectPtr, colour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the bottom-right corner's colour.
	End Rem
	Method setBottomRight(colour:TCEColour)
		bmx_cegui_colourrect_setbottomright(objectPtr, colour.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the top-left corner's colour.
	End Rem
	Method getTopLeft:TCEColour()
		Return TCEColour._create(bmx_cegui_colourrect_gettopleft(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the top-right corner's colour.
	End Rem
	Method getTopRight:TCEColour()
		Return TCEColour._create(bmx_cegui_colourrect_gettopright(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the bottom-left corner's colour.
	End Rem
	Method getBottomLeft:TCEColour()
		Return TCEColour._create(bmx_cegui_colourrect_getbottomleft(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the bottom-right corner's colour.
	End Rem
	Method getBottomRight:TCEColour()
		Return TCEColour._create(bmx_cegui_colourrect_getbottomright(objectPtr))
	End Method
	

	Method Delete()
		If objectPtr Then
			bmx_cegui_colourrect_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Represents colour values within the system.
End Rem
Type TCEColour

	Field objectPtr:Byte Ptr

	Function _create:TCEColour(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEColour = New TCEColour
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a colour from red, green, blue and alpha components.
	End Rem
	Function CreateColour:TCEColour(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		Return New TCEColour.Create(red, green, blue, alpha)
	End Function
	
	Rem
	bbdoc: Creates a colour from red, green, blue and alpha components.
	End Rem
	Method Create:TCEColour(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		objectPtr = bmx_cegui_colour_create(red, green, blue, alpha)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the 32 bit ARGB representation of the colour. 
	End Rem
	Method getARGB:Int()
		Return bmx_cegui_colour_getargb(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the alpha component.
	End Rem
	Method GetAlpha:Float()
		Return bmx_cegui_colour_getalpha(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the red component.
	End Rem
	Method getRed:Float()
		Return bmx_cegui_colour_getred(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the green component.
	End Rem
	Method getGreen:Float()
		Return bmx_cegui_colour_getgreen(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the blue component.
	End Rem
	Method getBlue:Float()
		Return bmx_cegui_colour_getblue(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the hue level for this colour.
	End Rem
	Method getHue:Float()
		Return bmx_cegui_colour_gethue(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the saturation level for this colour.
	End Rem
	Method getSaturation:Float()
		Return bmx_cegui_colour_getsaturation(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the lumination level for this colour.
	End Rem
	Method getLumination:Float()
		Return bmx_cegui_colour_getlumination(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colour using a 32 bit RGBA represention.
	End Rem
	Method setARGB(argb:Int)
		bmx_cegui_colour_setargb(objectPtr, argb)
	End Method
	
	Rem
	bbdoc: Sets the alpha component.
	End Rem
	Method SetAlpha(alpha:Float)
		bmx_cegui_colour_setalpha(objectPtr, alpha)
	End Method
	
	Rem
	bbdoc: Sets the red component.
	End Rem
	Method setRed(red:Float)
		bmx_cegui_colour_setred(objectPtr, red)
	End Method
	
	Rem
	bbdoc: Sets the green component.
	End Rem
	Method setGreen(green:Float)
		bmx_cegui_colour_setgreen(objectPtr, green)
	End Method
	
	Rem
	bbdoc: Sets the blue component.
	End Rem
	Method setBlue(blue:Float)
		bmx_cegui_colour_setblue(objectPtr, blue)
	End Method
	
	Rem
	bbdoc: Sets the colour components.
	End Rem
	Method set(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		bmx_cegui_colour_set(objectPtr, red, green, blue, alpha)
	End Method
	
	Rem
	bbdoc: Sets the RGB components.
	End Rem
	Method setRGB(red:Float, green:Float, blue:Float)
		bmx_cegui_colour_setrgb(objectPtr, red, green, blue)
	End Method
	
	Rem
	bbdoc: Sets the hue, saturation and luminance components.
	End Rem
	Method setHSL(hue:Float, saturation:Float, luminance:Float, alpha:Float = 1.0)
		bmx_cegui_colour_sethsl(objectPtr, hue, saturation, luminance, alpha)
	End Method
	
	Rem
	bbdoc: Inverts the colour.
	End Rem
	Method invertColour()
		bmx_cegui_colour_inverColour(objectPtr)
	End Method
	
	Rem
	bbdoc: Inverts the colour, including the alpha component.
	End Rem
	Method invertColourWithAlpha()
		bmx_cegui_colour_invertColourWithAlpha(objectPtr)
	End Method
	
	Method Delete()
		If objectPtr Then
			bmx_cegui_colour_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Provides a shared library of Font objects to the system.
about: The FontManager is used to create, access, and destroy Font objects. The idea is that the
FontManager will function as a central repository for Font objects used within the GUI system, and that
those Font objects can be accessed, via a unique name, by any interested party within the system.
End Rem
Type TCEFontManager

	Rem
	bbdoc: Creates a new font from a font definition file, and returns the new Font object.
	End Rem
	Function createFreeTypeFont:TCEFont(FontName:String, pointSize:Float, antialiased:Int, fontFilename:String, resourceGroup:String = "", ..
			autoScaled:Int = False, nativeHorzRes:Float = 640.0, nativeVertRes:Float = 480.0, action:Int = XREA_RETURN)
		Return TCEFont(bmx_cegui_fontmanager_createfreetypefont(_convertMaxToUTF8(FontName), pointSize, antialiased, _convertMaxToUTF8(fontFilename), ..
			_convertMaxToUTF8(resourceGroup), autoScaled, nativeHorzRes, nativeVertRes, action))
	End Function
	
	Rem
	bbdoc: Creates a new Font based on a true-type font, and returns the new Font object.
	End Rem
	Function createPixmapFont:TCEFont(FontName:String, imagesetFilename:String, resourceGroup:String = "", autoScaled:Int = False, ..
			nativeHorzRes:Float = 640.0, nativeVertRes:Float = 480.0, action:Int = XREA_RETURN)
		Return TCEFont(bmx_cegui_fontmanager_createpixmapfont(_convertMaxToUTF8(FontName), _convertMaxToUTF8(imagesetFilename), ..
			_convertMaxToUTF8(resourceGroup), autoScaled, nativeHorzRes, nativeVertRes, action))

	End Function

	Function createFont:TCEFont(filename:String, resourceGroup:String = "", action:Int = XREA_RETURN)
		Return TCEFont(bmx_cegui_fontmanager_createfont(_convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup), action))
	End Function
	
	Rem
	bbdoc: Notifies the FontManager of the current (usually new) display resolution.
	End Rem
	Function notifyDisplaySizeChanged(width:Float, height:Float)
		bmx_cegui_fontmanager_notifydislaysizechanged(width, height)
	End Function

	Function destroy(font:Object)
		If String(font) Then
			bmx_cegui_fontmanager_destroyname(_convertMaxToUTF8(String(font)))
		Else If TCEFont(font) Then
			bmx_cegui_fontmanager_destroyobj(TCEFont(font).objectPtr)
		End If
	End Function
	
	Function destroyAll()
		bmx_cegui_fontmanager_destroyall()
	End Function
	
	Function get:TCEFont(name:String)
		Return TCEFont(bmx_cegui_fontmanager_get(_convertMaxToUTF8(name)))
	End Function
	
	Function isDefined:Int(name:String)
		Return bmx_cegui_fontmanager_isdefined(_convertMaxToUTF8(name))
	End Function
	
	Function createAll(pattern:String, resourceGroup:String)
		bmx_cegui_fontmanager_createall(_convertMaxToUTF8(pattern), _convertMaxToUTF8(resourceGroup))
	End Function

End Type


Rem
bbdoc: Manages the creation of, access to, and destruction of GUI Scheme objects.
End Rem
Type TCESchemeManager

	Function createScheme:TCEScheme(filename:String, resourceGroup:String = "", action:Int = XREA_RETURN)
		Return TCEScheme(bmx_cegui_schememanager_createscheme(_convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup), action))
	End Function

	Function destroy(scheme:Object)
		If String(scheme) Then
			bmx_cegui_schememanager_destroyname(_convertMaxToUTF8(String(scheme)))
		Else If TCEScheme(scheme) Then
			bmx_cegui_schememanager_destroyobj(TCEScheme(scheme).objectPtr)
		End If
	End Function
	
	Function destroyAll()
		bmx_cegui_schememanager_destroyall()
	End Function
	
	Function get:TCEScheme(name:String)
		Return TCEScheme(bmx_cegui_schememanager_get(_convertMaxToUTF8(name)))
	End Function
	
	Function isDefined:Int(name:String)
		Return bmx_cegui_schememanager_isdefined(_convertMaxToUTF8(name))
	End Function
	
	Function createAll(pattern:String, resourceGroup:String)
		bmx_cegui_schememanager_createall(_convertMaxToUTF8(pattern), _convertMaxToUTF8(resourceGroup))
	End Function
	
End Type


Rem
bbdoc: Encapsulates text rendering functionality for a typeface.
about: A Font object is created for each unique typeface required. The Font type provides methods for loading
typefaces from various sources, and then for outputting text via the Renderer object.
<p>
This type is not specific to any font renderer, it just provides the basic interfaces needed to manage fonts.
</p>
End Rem
Type TCEFont

	Field objectPtr:Byte Ptr

	Function _create:TCEFont(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEFont = New TCEFont
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns whether this Font can draw the specified code-point. 
	End Rem
	Method isCodepointAvailable:Int(cp:Int)
		Return bmx_cegui_font_iscodepointavailable(objectPtr, cp)
	End Method
	
	Rem
	bbdoc: Returns the pixel line spacing value for. 
	End Rem
	Method getLineSpacing:Float(yScale:Float = 1.0)
		Return bmx_cegui_font_getlinespacing(objectPtr, yScale)
	End Method

	Rem
	bbdoc: Returns the exact pixel height of the font. 
	End Rem
	Method getFontHeight:Float(yScale:Float = 1.0)
		Return bmx_cegui_font_getfontheight(objectPtr, yScale)
	End Method

	Rem
	bbdoc: Returns the number of pixels from the top of the highest glyph to the baseline. 
	End Rem
	Method getBaseline:Float(yScale:Float = 1.0)
		Return bmx_cegui_font_getbaseline(objectPtr, yScale)
	End Method
	
	Rem
	bbdoc: Removes a Property from the PropertySet.
	End Rem
	Method removeProperty(name:String)
		bmx_cegui_font_removeproperty(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Removes all Property objects from the PropertySet.
	End Rem
	Method clearProperties()
		bmx_cegui_font_clearproperties(objectPtr)
	End Method
	 
	Rem
	bbdoc: Checks to see if a Property with the given name is in the PropertySet.
	returns: True if a Property named name is in the PropertySet. False if no Property named name is in the PropertySet.
	End Rem
	Method isPropertyPresent:Int(name:String)
		Return bmx_cegui_font_ispropertypresent(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Returns the help text for the specified Property.
	End Rem
	Method getPropertyHelp:String(name:String)
		Return bmx_cegui_font_getpropertyhelp(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Gets the current value of the specified Property.
	End Rem
	Method getProperty:String(name:String)
		Return bmx_cegui_font_getproperty(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Sets the current value of a Property.
	End Rem
	Method setProperty(name:String, value:String)
		bmx_cegui_font_setproperty(objectPtr, _convertMaxToUTF8(name), _convertMaxToUTF8(value))
	End Method
	 
	Rem
	bbdoc: Returns whether a Property is at it's default value.
	returns: True if the property has it's default value, or False if the property has been modified from it's default value.
	End Rem
	Method isPropertyDefault:Int(name:String)
		Return bmx_cegui_font_ispropertydefault(objectPtr, _convertMaxToUTF8(name))
	End Method
	 
	Rem
	bbdoc: Returns the default value of a Property as a String.
	End Rem
	Method getPropertyDefault:String(name:String)
		Return bmx_cegui_font_getpropertydefault(objectPtr, _convertMaxToUTF8(name))
	End Method


	Rem
	bbdoc: Sets the default resource group to be used when loading font data. 
	End Rem
	Function setDefaultResourceGroup(resourceGroup:String)
		bmx_cegui_font_setdefaultresourcegroup(_convertMaxToUTF8(resourceGroup))
	End Function
	
	Rem
	bbdoc: Returns the default resource group currently set for Fonts. 
	End Rem
	Function getDefaultResourceGroup:String()
		Return bmx_cegui_font_getdefaultresourcegroup()
	End Function

End Type

Rem
bbdoc: Represents a single Image of an Imageset. 
End Rem
Type TCEImage

	Field objectPtr:Byte Ptr

	Function _create:TCEImage(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEImage = New TCEImage
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the dimensions of the Image. 
	End Rem
	Method getSize(width:Float Var, height:Float Var)
		bmx_cegui_image_getsize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Returns the pixel width of the image. 
	End Rem
	Method getWidth:Float()
		Return bmx_cegui_image_getwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the pixel height of the image. 
	End Rem
	Method getHeight:Float()
		Return bmx_cegui_image_getheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the offset applied when rendering this Image. 
	End Rem
	Method getOffsets(x:Float Var, y:Float Var)
		bmx_cegui_image_getoffsets(objectPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Returns the X rendering offset. 
	End Rem
	Method getOffsetX:Float()
		Return bmx_cegui_image_getoffsetx(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the Y rendering offset. 
	End Rem
	Method getOffsetY:Float()
		Return bmx_cegui_image_getoffsety(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the name of this Image object. 
	End Rem
	Method getName:String()
		Return bmx_cegui_image_getname(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the name of the Imageset that contains this Image. 
	End Rem
	Method getImagesetName:String()
		Return bmx_cegui_image_getimagesetname(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the parent Imageset object that contains this Image. 
	End Rem
	Method getImageset:TCEImageset()
		Return TCEImageset._create(bmx_cegui_image_getimageset(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns Rect describing the source texture area used by this Image. 
	End Rem
	Method getSourceTextureArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_image_getsourcetexturearea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
End Type

Rem
bbdoc: Provides a shared library of Imageset objects to the system.
abotu: The ImagesetManager is used to create, access, and destroy Imageset objects. The idea is
that the ImagesetManager will function as a central repository for imagery used within the GUI system,
and that such imagery can be accessed, via a unique name, by any interested party within the system.
End Rem
Type TCEImagesetManager

	Rem
	bbdoc: Create an Imageset object from the specified file.
	End Rem
	Function createImagesetFromTexture:TCEImageset(name:String, texture:TCETexture , action:Int = XREA_RETURN)
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimagesetfromtexture(_convertMaxToUTF8(name), texture.objectPtr, action))
	End Function
	
	Rem
	bbdoc: Create an Imageset object from the specified image file.
	about: The Imageset will initially have a single image defined named "full_image" which is
	an image that represents the entire area of the loaded image.
	End Rem
	Function createImagesetFromImageFile:TCEImageset(name:String, filename:String, resourceGroup:String = "", action:Int = XREA_RETURN)
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimagesetfromimagefile(_convertMaxToUTF8(name), _convertMaxToUTF8(filename), ..
			_convertMaxToUTF8(resourceGroup), action))
	End Function
	
	Rem
	bbdoc: Notify the ImagesetManager of the current (usually new) display resolution.
	End Rem
	Function notifyDisplaySizeChanged(width:Float, height:Float)
		bmx_cegui_imagesetmanager_notifydisplaysizechanged(width, height)
	End Function

	Function createImageset:TCEImageset(filename:String, resourceGroup:String = "", action:Int = XREA_RETURN)
		Return TCEImageset(bmx_cegui_imagesetmanager_createimageset(_convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup), action))
	End Function

	Function destroy(imageset:Object)
		If String(imageset) Then
			bmx_cegui_imagesetmanager_destroyname(_convertMaxToUTF8(String(imageset)))
		Else If TCEImageset(imageset) Then
			bmx_cegui_imagesetmanager_destroyobj(TCEImageset(imageset).objectPtr)
		End If
	End Function
	
	Function destroyAll()
		bmx_cegui_imagesetmanager_destroyall()
	End Function
	
	Function get:TCEImageset(name:String)
		Return TCEImageset(bmx_cegui_imagesetmanager_get(_convertMaxToUTF8(name)))
	End Function
	
	Function isDefined:Int(name:String)
		Return bmx_cegui_imagesetmanager_isdefined(_convertMaxToUTF8(name))
	End Function
	
	Function createAll(pattern:String, resourceGroup:String)
		bmx_cegui_imagesetmanager_createall(_convertMaxToUTF8(pattern), _convertMaxToUTF8(resourceGroup))
	End Function

End Type

Rem
bbdoc: A Texture object.
about: Texture objects are created via the Renderer. The actual inner workings of any Texture object
are dependant upon the Renderer (and underlying API) in use. This base type defines the minimal
set of functions that is required for the rest of the system to work. Texture objects are only
created through the Renderer object's texture creation methods.
End Rem
Type TCETexture

	Rem
	bbdoc: Each pixel is 3 bytes. RGB in that order.
	End Rem
	Const PF_RGB:Int = 0
	Rem
	bbdoc: Each pixel is 4 bytes. RGBA in that order.
	End Rem
	Const PF_RGBA:Int = 1

	Field objectPtr:Byte Ptr
	
	Function _create:TCETexture(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETexture = New TCETexture
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the current pixel size of the texture.
	End Rem
	Method getSize(w:Int Var, h:Int Var)
		bmx_cegui_texture_getsize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getOriginalDataSize(w:Int Var, h:Int Var)
		bmx_cegui_texture_getoriginaldatasize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Loads the specified image file into the texture.
	about: The texture is resized as required to hold the image.
	End Rem
	Method loadFromFile(filename:String, resourceGroup:String)
		bmx_cegui_texture_loadfromfile(objectPtr, _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup))
	End Method
	
	Rem
	bbdoc: Loads (copies) an image in memory into the texture.
	about: The texture is resized as required to hold the image.
	End Rem
	Method loadFromMemory(buffer:Byte Ptr, width:Int, height:Int, pixelFormat:Int)
		bmx_cegui_texture_loadfrommemory(objectPtr, buffer, width, height, pixelFormat)
	End Method
	
	Rem
	bbdoc: Saves / dumps the content of the texture to a memory buffer.
	about: The dumped pixel format is always RGBA (4 bytes per pixel).
	<p>
	You must make sure that this buffer is large enough to hold the dumped texture data, the required pixel dimensions can be established by calling getSize.
	</p>
	End Rem
	Method saveToMemory(buffer:Byte Ptr)
		bmx_cegui_texture_savetomemory(objectPtr, buffer)
	End Method
	
End Type

Rem
bbdoc: Offers functions to define, access, and draw, a set of image components on a single graphical surface or Texture.
about: Imageset objects are a means by which a single graphical image (file, Texture, etc), can be split into
a number of 'components' which can later be accessed via name. The components of an Imageset can queried for various
details, and sent to the Renderer object for drawing.
End Rem
Type TCEImageset

	Field objectPtr:Byte Ptr

	Function _create:TCEImageset(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEImageset = New TCEImageset
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the name of the Imageset.
	End Rem
	Method getName:String()
		Return bmx_cegui_imageset_getname(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns number of images defined for this Imageset.
	End Rem
	Method getImageCount:Int()
		Return bmx_cegui_imageset_getimagecount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns True if an Image with the specified name exists.
	End Rem
	Method isImageDefined:Int(name:String)
		Return bmx_cegui_imageset_isimagedefined(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	'method getImage:TCEImage(name:String)
	'End method
	
	Rem
	bbdoc: Removes the definition for the Image with the specified name.
	about: If no such Image exists, nothing happens.
	End Rem
	Method undefineImage(name:String)
		bmx_cegui_imageset_undefineimage(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Removes the definitions for all Image objects currently defined in the Imageset.
	End Rem
	Method undefinAllImages()
		bmx_cegui_imageset_undefineallimages(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the dimensions of the named image.
	End Rem
	Method getImageSize(name:String, width:Float Var, height:Float Var)
		bmx_cegui_imageset_getimagesize(objectPtr, _convertMaxToUTF8(name), Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Returns the width of the named image.
	End Rem
	Method getImageWidth:Float(name:String)
		Return bmx_cegui_imageset_getimagewidth(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the height of the named image.
	End Rem
	Method getImageHeight:Float(name:String)
		Return bmx_cegui_imageset_getimageheight(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the rendering offsets applied to the named image.
	End Rem
	Method getImageOffset(name:String, x:Float Var, y:Float Var)
		bmx_cegui_imageset_getimageoffset(objectPtr, _convertMaxToUTF8(name), Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Returns the x rendering offset for the named image.
	End Rem
	Method getImageOffsetX:Float(name:String)
		Return bmx_cegui_imageset_getimageoffsetx(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Returns the y rendering offset for the named image.
	End Rem
	Method getImageOffsetY:Float(name:String)
		Return bmx_cegui_imageset_getimageoffsety(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Defines a new Image for this Imageset.
	End Rem
	Method defineImage(name:String, x:Float, y:Float, width:Float, height:Float, renderOffsetX:Float, renderOffsetY:Float)
		bmx_cegui_imageset_defineimage(objectPtr, _convertMaxToUTF8(name), x, y, width, height, renderOffsetX, renderOffsetY)
	End Method
	
	Rem
	bbdoc: Returns whether this Imageset is auto-scaled.
	End Rem
	Method isAutoScaled:Int()
		Return bmx_cegui_imageset_isautoscaled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the native display size for this Imageset.
	about: This is only relevant if the Imageset is being auto-scaled.
	End Rem
	Method getNativeResolution(width:Float Var, height:Float Var)
		bmx_cegui_imageset_getnativeresoultion(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Enables or disables auto-scaling for this Imageset.
	End Rem
	Method setAutoScalingEnabled(setting:Int)
		bmx_cegui_imageset_setautoscalingenabled(objectPtr, setting)
	End Method

	Rem
	bbdoc: Sets the native resolution for this Imageset.
	End Rem
	Method setNativeResolution(width:Float, height:Float)
		bmx_cegui_imageset_setnativeresolution(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Notifies the Imageset of the current (usually new) display resolution.
	End Rem
	Method notifyDisplaySizeChanged(width:Float, height:Float)
		bmx_cegui_imageset_notifydisplaysizechanged(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Sets the default resource group to be used when loading imageset data.
	End Rem
	Function setDefaultResourceGroup(resourceGroup:String)
		bmx_cegui_imageset_setdefaultresourcegroup(_convertMaxToUTF8(resourceGroup))
	End Function
	
	Rem
	bbdoc: Returns the default resource group currently set for Imagesets.
	End Rem
	Function getDefaultResourceGroup:String()
		Return bmx_cegui_imageset_getdefaultresourcegroup()
	End Function
	
End Type

Rem
bbdoc: The means by which the GUI system interfaces with specific rendering technologies.
about: To use a rendering system or API to draw CEGUI imagery requires that an appropriate Renderer object be available.
End Rem
Type TCERenderer Abstract

	Field objectPtr:Byte Ptr
	
	Method createGeometryBuffer:TCEGeometryBuffer()
'		Return TCEGeometryBuffer(bmx_cegui_renderer_creategeometrybuffer(objectPtr))
	End Method
	
	Method destroyGeometryBuffer(buffer:TCEGeometryBuffer)
'		bmx_cegui_renderer_destroygeometrybuffer(objectPtr, buffer.objectPtr)
	End Method
	
	Method destroyAllGeometryBuffers()
'		bmx_cegui_renderer_destroyallgeometrybuffers(objectPtr)
	End Method
	
	Method createTextureTarget:TCETextureTarget()
'		Return TCETextureTarget(bmx_cegui_renderer_createtexturetarget(objectPtr))
	End Method
	
	Method destroyTextureTarget(target:TCETextureTarget)
'		bmx_cegui_renderer_destroytexturetarget(objectPtr, target.objectPtr)
	End Method
	
	Method destroyAllTextureTargets()
'		bmx_cegui_renderer_destroyalltexturetargets(objectPtr)
	End Method
	
	Rem
	bbdoc: Create a Texture object using the given image file.
	about: Textures are always created with a size that is a power of 2. If the file you specify is
	of a size that is not a power of two, the final size will be rounded up. Additionally, textures are
	always square, so the ultimate size is governed by the larger of the width and height of the
	specified file. You can check the ultimate sizes by querying the texture after creation.
	End Rem
	Method CreateTexture:TCETexture(filename:String, resourceGroup:String)
'		Return TCETexture(bmx_cegui_renderer_createtexture(objectPtr, _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Method
	
	Rem
	bbdoc: Create a Texture object with the given pixel dimensions as specified by size. 
	about: Textures are always square.
	<p>
	Textures are always created with a size that is a power of 2. If you specify a size that is not a
	power of two, the final size will be rounded up. So if you specify a size of 1024, the texture will
	be (1024 x 1024), however, if you specify a size of 1025, the texture will be (2048 x 2048). You
	can check the ultimate size by querying the texture after creation.
	</p>
	End Rem
	Method createTextureWithSize:TCETexture(w:Float, h:Float)
'		Return TCETexture(bmx_cegui_renderer_createtexturewithsize(objectPtr, size))
	End Method
	
	Rem
	bbdoc: Destroys the given Texture.
	End Rem
	Method destroyTexture(texture:TCETexture)
'		bmx_cegui_renderer_destroytexture(objectPtr, texture.objectPtr)
	End Method
	
	Rem
	bbdoc: Destroys all Texture objects.
	End Rem
	Method destroyAllTextures()
'		bmx_cegui_renderer_destroyalltextures(objectPtr)
	End Method

	Method beginRendering()
'		bmx_cegui_renderer_beginrendering(objectPtr)
	End Method

	Method endRendering()
'		bmx_cegui_renderer_endrendering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the display in pixels.
	End Rem
	Method getDisplaySize(width:Float Var, height:Float Var)
'		bmx_cegui_renderer_getdisplaysize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Method setDisplaySize(width:Float, height:Float)
'		bmx_cegui_renderer_setdisplaysize(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Returns the vertical display resolution dpi.
	End Rem
	Method getDisplayDPI(x:Float Var, y:Float Var)
'		bmx_cegui_renderer_getdisplaydpi(objectPtr, Varptr x, Varptr y)
	End Method
	
	Method getMaxTextureSize:Int()
'		Return bmx_cegui_renderer_getmaxtexturesize(objectPtr)
	End Method
	
	Rem
	bbdoc: Return identification string for the renderer module.
	about: If the internal id string has not been set by the Renderer module creator, a generic string
	of "Unknown renderer" will be returned.
	End Rem
	Method getIdentifierString:String()
'		Return bmx_cegui_renderer_getidentifierstring(objectPtr)
	End Method

	Method free()
	' TODO
	End Method
	
End Type

Rem
bbdoc: Base type for Tooltip widgets.
about: The Tooltip type shows a simple pop-up window around the mouse position with some text information.
The tool-tip fades in when the user hovers with the mouse over a window which has tool-tip text set,
and then fades out after some pre-set time.
<p>
For Tooltip to work properly, you must specify a default tool-tip widget type via TCESystem::setTooltip,
or by setting a custom tool-tip object for your Window(s). Additionally, you need to ensure that time
pulses are properly passed to the system via System::injectTimePulse. 
</p>
End Rem
Type TCETooltip Extends TCEWindow

	Function _create:TCETooltip(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETooltip = New TCETooltip
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the hover timeout gets changed. 
	End Rem
	Const EventHoverTimeChanged:String = "HoverTimeChanged"
	Rem
	bbdoc: Event fired when the display timeout gets changed. 
	End Rem
	Const EventDisplayTimeChanged:String = "DisplayTimeChanged"
	Rem
	bbdoc: Event fired when the fade timeout gets changed. 
	End Rem
	Const EventFadeTimeChanged:String = "FadeTimeChanged"
	Rem
	bbdoc: Event fired when the tooltip is about to get activated. 
	End Rem
	Const EventTooltipActive:String = "TooltipActive"
	Rem
	bbdoc: Event fired when the tooltip has been deactivated. 
	End Rem
	Const EventTooltipInactive:String = "TooltipInactive"

	Rem
	bbdoc: Returns the current target window for this Tooltip.
	End Rem
	Method getTargetWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_tooltip_gettargetwindow(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the number of seconds the mouse should hover stationary over the target window before the tooltip gets activated.
	End Rem
	Method getHoverTime:Float()
		Return bmx_cegui_tooltip_gethovertime(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the number of seconds the tooltip should be displayed for before it automatically de-activates itself.
	about: 0 indicates that the tooltip should never timesout and auto-deactivate.
	End Rem
	Method setDisplayTime(seconds:Float)
		bmx_cegui_tooltip_setdisplaytime(objectPtr, seconds)
	End Method
	
	Rem
	bbdoc: Returns the number of seconds that should be taken to fade the tooltip into and out of visibility.
	End Rem
	Method getFadeTime:Float()
		Return bmx_cegui_tooltip_getfadetime(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the number of seconds the mouse should hover stationary over the target window before the tooltip gets activated.
	End Rem
	Method setHoverTime(seconds:Float)
		bmx_cegui_tooltip_sethovertime(objectPtr, seconds)
	End Method
	
	Rem
	bbdoc: Returns the number of seconds the tooltip should be displayed for before it automatically de-activates itself.
	about: 0 indicates that the tooltip never timesout and auto-deactivates.
	End Rem
	Method getDisplayTime:Float()
		Return bmx_cegui_tooltip_getdisplaytime(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the number of seconds that should be taken to fade the tooltip into and out of visibility.
	End Rem
	Method setFadeTime(seconds:Float)
		bmx_cegui_tooltip_setfadetime(objectPtr, seconds)
	End Method
	
	Rem
	bbdoc: Causes the tooltip to position itself appropriately.
	End Rem
	Method positionSelf()
		bmx_cegui_tooltip_positionself(objectPtr)
	End Method
	
	Rem
	bbdoc: Causes the tooltip to resize itself appropriately.
	End Rem
	Method sizeSelf()
		bmx_cegui_tooltip_sizeself(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the area that will be occupied by the tooltip text, given any current formatting options.
	End Rem
	Method getTextSize(width:Float Var, height:Float Var) 
		bmx_cegui_tooltip_gettextsize(objectPtr, Varptr width, Varptr height)
	End Method

End Type

Rem
bbdoc: Generic drag and drop enabled window.
End Rem
Type TCEDragContainer Extends TCEWindow

	Function _create:TCEDragContainer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEDragContainer = New TCEDragContainer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the user begins dragging the thumb. 
	End Rem
	Const EventDragStarted:String = "DragStarted"
	Rem
	bbdoc: Event fired when the user releases the thumb. 
	End Rem
	Const EventDragEnded:String = "DragEnded"
	Rem
	bbdoc: Event fired when the drag position has changed. 
	End Rem
	Const EventDragPositionChanged:String = "DragPositionChanged"
	Rem
	bbdoc: Event fired when dragging is enabled or disabled. 
	End Rem
	Const EventDragEnabledChanged:String = "DragEnabledChanged"
	Rem
	bbdoc: Event fired when the alpha value used when dragging is changed. 
	End Rem
	Const EventDragAlphaChanged:String = "DragAlphaChanged"
	Rem
	bbdoc: Event fired when the mouse cursor used when dragging is changed. 
	End Rem
	Const EventDragMouseCursorChanged:String = "DragMouseCursorChanged"
	Rem
	bbdoc: Event fired when the drag pixel threshold is changed. 
	End Rem
	Const EventDragThresholdChanged:String = "DragThresholdChanged"
	Rem
	bbdoc: Event fired when the drop target changes. 
	End Rem
	Const EventDragDropTargetChanged:String = "DragDropTargetChanged"

	Rem
	bbdoc: Returns whether dragging is currently enabled for this DragContainer. 
	End Rem
	Method isDraggingEnabled:Int()
		Return bmx_cegui_dragcontainer_isdraggingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether dragging is currently enabled for this DragContainer. 
	End Rem
	Method setDraggingEnabled(setting:Int)
		bmx_cegui_dragcontainer_setdraggingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns whether the DragContainer is currently being dragged. 
	End Rem
	Method isBeingDragged:Int()
		Return bmx_cegui_dragcontainer_isbeingdragged(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current drag threshold in pixels.
	about: The drag threshold is the number of pixels that the mouse must be moved with the left button
	held down in order to commence a drag operation.
	End Rem
	Method getPixelDragThreshold:Float()
		Return bmx_cegui_dragcontainer_getpixeldragthreshold(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the current drag threshold in pixels.
	about: The drag threshold is the number of pixels that the mouse must be moved with the left button
	held down in order to commence a drag operation.
	End Rem
	Method setPixelDragThreshold(pixels:Float)
		bmx_cegui_dragcontainer_setpixeldragthreshold(objectPtr, pixels)
	End Method
	
	Rem
	bbdoc: Returns the alpha value that will be set on the DragContainer while a drag operation is in progress.
	End Rem
	Method getDragAlpha:Float()
		Return bmx_cegui_dragcontainer_getdragalpha(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the alpha value to be set on the DragContainer when a drag operation is in progress.
	about: This method can be used while a drag is in progress to update the alpha. Note that the normal
	setAlpha method does not affect alpha while a drag is in progress, but once the drag operation has
	ended, any value set via setAlpha will be restored.
	End Rem
	Method setDragAlpha(alpha:Float)
		bmx_cegui_dragcontainer_setdragalpha(objectPtr, alpha)
	End Method
	
	Rem
	bbdoc: Returns the Image currently set to be used for the mouse cursor when a drag operation is in progress. 
	End Rem
	Method getDragCursorImage:TCEImage()
		Return TCEImage(bmx_cegui_dragcontainer_getdragcursorimage(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the Image to be used for the mouse cursor when a drag operation is in progress.
	about: This method may be used during a drag operation to update the current mouse cursor image.
	End Rem
	Method setDragCursorImage(image:TCEImage)
		bmx_cegui_dragcontainer_setdragcursorimage(objectPtr, image.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the Image to be used for the mouse cursor when a drag operation is in progress.
	about: This method may be used during a drag operation to update the current mouse cursor image.
	End Rem
	Method setDragCursorImageMode(image:Int)
		bmx_cegui_dragcontainer_setdragcursorimagemode(objectPtr, image)
	End Method
	
	Rem
	bbdoc: Sets the Image to be used for the mouse cursor when a drag operation is in progress.
	about: This method may be used during a drag operation to update the current mouse cursor image.
	End Rem
	Method setDragCursorImageByName(imageset:String, image:String)
		bmx_cegui_dragcontainer_setdragcursorimagebyname(objectPtr, imageset,image)
	End Method
	
	Rem
	bbdoc: Return the Window object that is the current drop target for the DragContainer.
	about: The drop target for a DragContainer is basically the Window that the DragContainer is within
	while being dragged. The drop target may be Null to indicate no target.
	End Rem
	Method getCurrentDropTarget:TCEWindow() 
		Return TCEWindow(bmx_cegui_dragcontainer_getcurrentdroptarget(objectPtr))
	End Method

End Type

Rem
bbdoc: Defines the required interface for all WindowFactory objects.
abotu: A WindowFactory is used to create and destroy windows of a specific type. For every type of Window object wihin
the system (widgets, dialogs, movable windows etc) there must be an associated WindowFactory registered with the
WindowManager so that the system knows how to create and destroy those types of Window base object.
End Rem
Type TCEWindowFactory

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new TCEWindowFactory instance.
	End Rem
	Method Create:TCEWindowFactory()
		objectPtr = bmx_cegui_windowfactory_create(Self, _convertMaxToUTF8(getTypeName()))
		Return Self
	End Method
	
	Rem
	bbdoc: Creates a new Window object of whatever type this WindowFactory produces.
	End Rem
	Method CreateWindow:TCEWindow(name:String)
	End Method
	
	Function _createWindow:Byte Ptr(factory:TCEWindowFactory, name:String)
		Return factory.CreateWindow(name).objectPtr
	End Function
	
	Rem
	bbdoc: Destroys the given Window object.
	End Rem
	Method destroyWindow(window:TCEWindow)
	End Method
	
	Function _destroyWindow(factory:TCEWindowFactory, window:TCEWindow)
		factory.destroyWindow(window)
	End Function
	
	Rem
	bbdoc: Gets the string that describes the type of Window object this WindowFactory produces.
	End Rem
	Method getTypeName:String()
	End Method
	
	Function _getTypeName:String(factory:TCEWindowFactory)
		Return factory.getTypeName()
	End Function
	
End Type


Rem
bbdoc:
End Rem
Type TCEWindowFactoryManager

End Type

Rem
bbdoc: Allows access to the GUI system mouse cursor.
about: The MouseCursor provides functionality to access the position and imagery of the mouse cursor / pointer .
End Rem
Type TCEMouseCursor

	Global mouseCursorPtr:Byte Ptr

	Rem
	bbdoc: Sets the current mouse cursor image. 
	End Rem
	Function setImageWithName(imageset:String, imageName:String)
		bmx_cegui_mousecursor_setimagewithname(mouseCursorPtr, _convertMaxToUTF8(imageset), _convertMaxToUTF8(imageName))
	End Function
	
	Rem
	bbdoc: Sets the current mouse cursor image. 
	End Rem
	Function setImage(image:TCEImage)
		bmx_cegui_mousecursor_setimage(mouseCursorPtr, image.objectPtr)
	End Function
	
	Rem
	bbdoc: Gets the current mouse cursor image. 
	End Rem
	Function getImage:TCEImage()
		Return TCEImage(bmx_cegui_mousecursor_getimage(mouseCursorPtr))
	End Function
	
	Rem
	bbdoc: Makes the cursor draw itself. 
	End Rem
	Function draw()
		bmx_cegui_mousecursor_draw(mouseCursorPtr)
	End Function
	
	Rem
	bbdoc: Sets the current mouse cursor position. 
	End Rem
	Function setPosition(x:Float, y:Float)
		bmx_cegui_mousecursor_setposition(mouseCursorPtr, x, y)
	End Function
	
	Rem
	bbdoc: Offsets the mouse cursor position by the deltas specified in offset. 
	End Rem
	Function offsetPosition(x:Float, y:Float)
		bmx_cegui_mousecursor_offsetPosition(mouseCursorPtr, x, y)
	End Function
	
	Rem
	bbdoc: Sets the area that the mouse cursor is constrained to. 
	End Rem
	Function setConstraintArea(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_mousecursor_setconstraintarea(mouseCursorPtr, x, y, w, h)
	End Function
	
	Rem
	bbdoc: Hides the mouse cursor. 
	End Rem
	Function hide()
		bmx_cegui_mousecursor_hide(mouseCursorPtr)
	End Function
	
	Rem
	bbdoc: Shows the mouse cursor. 
	End Rem
	Function show()
		bmx_cegui_mousecursor_show(mouseCursorPtr)
	End Function
	
	Rem
	bbdoc: Sets the visibility of the mouse cursor. 
	End Rem
	Function setVisible(visible:Int)
		bmx_cegui_mousecursor_setvisible(mouseCursorPtr, visible)
	End Function
	
	Rem
	bbdoc: Returns whether the mouse cursor is visible. 
	End Rem
	Function isVisible:Int()
		Return bmx_cegui_mousecursor_isvisible(mouseCursorPtr)
	End Function
	
	Rem
	bbdoc: Returns the current mouse cursor position as a pixel offset from the top-left corner of the display. 
	End Rem
	Function getPosition(x:Float Var, y:Float Var)
		bmx_cegui_mousecursor_getposition(mouseCursorPtr, Varptr x, Varptr y)
	End Function
	
	Rem
	bbdoc: Returns the current constraint area of the mouse cursor. 
	End Rem
	Function getConstraintArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_mousecursor_getconstraintarea(mouseCursorPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Function
	
	Rem
	bbdoc: Returns the current mouse cursor position as display resolution independant values. 
	End Rem
	Function getDisplayIndependentPosition(x:Float Var, y:Float Var)
		bmx_cegui_mousecursor_getdisplayindependentposition(mouseCursorPtr, Varptr x, Varptr y)
	End Function

End Type

Rem
bbdoc: Utility type that helps in converting various types of co-ordinate between absolute screen positions and positions offset from the top-left corner of a given Window object. 
End Rem
Type TCECoordConverter

	Rem
	bbdoc: Converts a window pixel co-ordinate value, specified as a float, to a screen pixel co-ordinate. 
	End Rem
	Function windowToScreenX:Float(window:TCEWindow, x:Float)
		Return bmx_cegui_coordconverter_windowtoscreenx(window.objectPtr, x)
	End Function
	
	Rem
	bbdoc: Converts a window pixel co-ordinate value, specified as a float, to a screen pixel co-ordinate. 
	End Rem
	Function windowToScreenY:Float(window:TCEWindow, y:Float)
		Return bmx_cegui_coordconverter_windowtoscreeny(window.objectPtr, y)
	End Function
	
	Rem
	bbdoc: Converts a window pixel co-ordinate point, specified as an x,y vector, to a screen pixel co-ordinate point. 
	End Rem
	Function windowToScreen(window:TCEWindow, x:Float, y:Float, toX:Float Var, toY:Float Var)
		bmx_cegui_coordconverter_windowtoscreen(window.objectPtr, x, y, Varptr toX, Varptr toY)
	End Function
	
	Rem
	bbdoc: Converts a pixel window area, specified as a Rect, to a screen area. 
	End Rem
	Function windowToScreenRect(window:TCEWindow, x:Float, y:Float, w:Float, h:Float, toX:Float Var, toY:Float Var, toW:Float Var, toH:Float Var)
		bmx_cegui_coordconverter_windowtoscreenrect(window.objectPtr, x, y, w, h, Varptr toX, Varptr toY, Varptr toW, Varptr toH)
	End Function
	
	Rem
	bbdoc: Converts a screen pixel co-ordinate value to a window co-ordinate value, specified in pixels. 
	End Rem
	Function screenToWindowX:Float(window:TCEWindow, x:Float)
		Return bmx_cegui_coordconverter_screentowindowx(window.objectPtr, x)
	End Function
	
	Rem
	bbdoc: Converts a screen pixel co-ordinate value to a window co-ordinate value, specified in pixels. 
	End Rem
	Function screenToWindowY:Float(window:TCEWindow, y:Float)
		Return bmx_cegui_coordconverter_screentowindowy(window.objectPtr, y)
	End Function
	
	Rem
	bbdoc: Converts a screen x,y vector pixel point to a window co-ordinate point, specified in pixels. 
	End Rem
	Function screenToWindow(window:TCEWindow, x:Float, y:Float, toX:Float Var, toY:Float Var)
		bmx_cegui_coordconverter_screentowindow(window.objectPtr, x, y, Varptr x, Varptr y)
	End Function
	
	Rem
	bbdoc: Converts a Rect screen pixel area to a window area, specified in pixels. 
	End Rem
	Function screenToWindowRect(window:TCEWindow, x:Float, y:Float, w:Float, h:Float, toX:Float Var, toY:Float Var, toW:Float Var, toH:Float Var)
		bmx_cegui_coordconverter_screentowindowrect(window.objectPtr, x, y, w, h, Varptr toX, Varptr toY, Varptr toW, Varptr toH)
	End Function

End Type

Rem
bbdoc: A 
End Rem
Function UDim:TCEUDim(scale:Float = 0.0, offset:Float = 0.0)
	Return TCEUDim._create(scale, offset) 
End Function

Rem
bbdoc: 
End Rem
Type TCEUDim

	Field objectPtr:Byte Ptr
	
	Function _create:TCEUDim(scale:Float = 0.0, offset:Float = 0.0)
		Local this:TCEUDim = New TCEUDim
		this.objectPtr = bmx_cegui_udim_create(scale, offset)
		Return this
	End Function

	Method Delete()
		If objectPtr Then
			bmx_cegui_udim_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type


Type TCEGeometryBuffer

	Field objectPtr:Byte Ptr

End Type


Type TCETextureTarget

	Field objectPtr:Byte Ptr

End Type

Rem
bbdoc: Manager type that gives top-level access to widget data based "look and feel" specifications loaded into the system.
End Rem
Type TCEWidgetLookManager

	Rem
	bbdoc: 
	End Rem
	Function getDefaultResourceGroup:String()
		Return bmx_cegui_widgetlookmanager_getdefaultresourcegroup()
	End Function
	
	Rem
	bbdoc: Sets the default resource group identifier.
	End Rem
	Function setDefaultResourceGroup(group:String)
		bmx_cegui_widgetlookmanager_setdefaultresourcegroup(_convertMaxToUTF8(group))
	End Function
	
End Type

