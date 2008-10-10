SuperStrict

Import Pub.FreeType
Import BaH.RegEx
Import BaH.FreeImage

Import BRL.Map
Import BRL.PolledInput

Import "common.bmx"

' some global stuff
Global cegui_rendererPtr:Byte Ptr
Global cegui_startTime:Int

Global ce_event_handler:TCEEventHandler = New TCEEventHandler

' TODO : not sure if this gets called?
OnEnd cegui_cleanup


Function cegui_cleanup()

	' remove the event hook!
	RemoveHook EmitEventHook,TCEEvent.Keyhook

	If TCESystem.cegui_systemPtr Then
		bmx_cegui_delete_system(TCESystem.cegui_systemPtr)
		TCESystem.cegui_systemPtr = Null
	End If
	
	If cegui_rendererPtr Then
		bmx_cegui_delete_renderer(cegui_rendererPtr)
		cegui_rendererPtr = Null
	End If
	
End Function

Rem
bbdoc: Initializes CEGUI
End Rem
Function Init_CEGUI()
	If Not cegui_rendererPtr Then
		' creates a new opengle renderer
		cegui_rendererPtr = bmx_cegui_new_oglrenderer()

		If cegui_rendererPtr Then
			' creates a new CEGUI system
			TCESystem.cegui_systemPtr = bmx_cegui_new_system(cegui_rendererPtr)
			
			TCEWindowManager.windowManagerPtr = bmx_cegui_windowmanager_getsingleton()
		End If

		' TODO : there are probably better ways to do this
		cegui_startTime = MilliSecs()
		
		AddHook EmitEventHook,TCEEvent.Keyhook,Null,0
	End If
End Function


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
bbdoc: 
End Rem
Type TCESystem Extends TCEEventSet

	Global cegui_systemPtr:Byte Ptr

	
	Rem
	bbdoc: 
	End Rem
	Function getRenderer:TCERenderer()
		
	End Function
	
	Rem
	bbdoc: Render the GUI.
	End Rem
	Function renderGUI()
		bmx_cegui_system_renderGUI(cegui_systemPtr)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function setDefaultFont(font:Object)
		If TCEFont(font) Then
			
		ElseIf String(font) Then
			bmx_cegui_system_setDefaultFont(cegui_systemPtr, _convertMaxToUTF8(String(font)))
		End If
	End Function

	Method getDefaultFont:TCEFont()
		Return TCEFont._create(bmx_cegui_system_getDefaultFont(cegui_systemPtr))
	End Method
	
	Method signalRedraw()
		bmx_cegui_system_signalredraw(cegui_systemPtr)
	End Method
	
	Method isRedrawRequested:Int()
		Return bmx_cegui_system_isredrawrequested(cegui_systemPtr)
	End Method
	
	Method getSingleClickTimeout:Double()
		Return bmx_cegui_system_getsingleclicktimeout(cegui_systemPtr)
	End Method
	
	Method getMultiClickTimeout:Double()
		Return bmx_cegui_system_getmulticlicktimeout(cegui_systemPtr)
	End Method
	
	Method getMultiClickToleranceAreaSize(width:Float Var, height:Float Var)
		bmx_cegui_system_getmulticlicktoleranceareasize(cegui_systemPtr, Varptr width, Varptr height)
	End Method
	
	Method setSingleClickTimeout(timeout:Double)
		bmx_cegui_system_setsingleclicktimeout(cegui_systemPtr, timeout)
	End Method
	
	Method setMultiClickTimeout(timeout:Double)
		bmx_cegui_system_setmulticlicktimeout(cegui_systemPtr, timeout)
	End Method
	
	Method setMultiClickToleranceAreaSize(width:Float, height:Float)
		bmx_cegui_system_setmulticlicktoleranceareasize(cegui_systemPtr, width, height)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function setDefaultMouseCursor(a:String, b:String)
		bmx_cegui_system_setDefaultMouseCursor(cegui_systemPtr, _convertMaxToUTF8(a), _convertMaxToUTF8(b))
	End Function
	
	Rem
	bbdoc: Set the active GUI sheet (root) window.
	End Rem
	Function setGUISheet(window:TCEWindow)
		bmx_cegui_system_setGUISheet(cegui_systemPtr, window.objectPtr)
	End Function
	
	Rem
	bbdoc: Inject time pulses into the system.
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

End Type

Rem
bbdoc: A GUI Scheme is a high-level construct that loads and initialises various lower-level objects and registers them within the system for usage.
about: So, for example, a Scheme might create some Imageset objects, some Font objects, and register
a collection of WindowFactory objects within the system which would then be in a state to serve those
elements to client code.
End Rem
Type TCEScheme

	Field schemePtr:Byte Ptr
	
	Function _create:TCEScheme(schemePtr:Byte Ptr)
		If schemePtr Then
			Local this:TCEScheme = New TCEScheme
			this.schemePtr = schemePtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Loads all resources for this scheme.
	End Rem
	Method loadResources()
		bmx_cegui_scheme_loadresources(schemePtr)
	End Method
	
	Rem
	bbdoc: Unloads all resources for this scheme.
	about: This should be used very carefully.
	End Rem
	Method unloadResources()
		bmx_cegui_scheme_unloadresources(schemePtr)
	End Method
	
	Rem
	bbdoc: Returns whether the resources for this Scheme are all loaded.
	End Rem
	Method resourcesLoaded:Int()
		Return bmx_cegui_scheme_resourcesloaded(schemePtr)
	End Method
	
	Rem
	bbdoc: Returns the name of this Scheme. 
	End Rem
	Method getName:String()
		Return _convertUTF8ToMax(bmx_cegui_scheme_getname(schemePtr))
	End Method

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
	Function CreateWindow:TCEWindow(windowType:String, name:String = "", prefix:String = "")
		Return TCEWindow(bmx_cegui_windowmanager_createwindow(windowManagerPtr, _convertMaxToUTF8(windowType), _convertMaxToUTF8(name), _convertMaxToUTF8(prefix)))
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
	bbdoc: 
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
	
	Method getType:String()
		Return _convertUTF8ToMax(bmx_cegui_window_gettype(objectPtr))
	End Method
	 
	Method getName:String()
	End Method
	 
	Method getPrefix:String()
	End Method
	
	Method isDestroyedByParent:Int()
	End Method
	 
	Method isAlwaysOnTop:Int()
	End Method
	 
	Method isDisabled:Int(localOnly:Int = False)
	End Method
	 
	Method isVisible:Int(localOnly:Int = False)
	End Method
	 
	Method isActive:Int()
	End Method
	 
	Method isClippedByParent:Int()
	End Method
	 
	Method getID:Int()
	End Method
	 
	Method getChildCount:Int()
	End Method
	 
	Method isChild:Int(name:String)
	End Method
	 
	Method isChildID:Int(ID:Int)
	End Method
	 
	Method isChildRecursive:Int(ID:Int)
	End Method
	 
	Method isChildWindow:Int(window:TCEWindow)
	End Method
	 
	Method GetChild:TCEWindow(name:String)
	End Method
	 
	Method recursiveChildSearch:TCEWindow(name:String)
	End Method
	
	Method GetChildID:TCEWindow(ID:Int)
	End Method
	 
	Method getChildRecursive:TCEWindow(ID:Int)
	End Method
	 
	Method getChildAtIdx:TCEWindow(idx:Int)
	End Method
	 
	Method getActiveChild:TCEWindow()
	End Method
	 
	Method isAncestor:Int(name:String)
	End Method
	 
	Method isAncestorID:Int(ID:Int)
	End Method
	 
	Method isAncestorWindow:Int(window:TCEWindow)
	End Method

	
	Method getFont:TCEFont(useDefault:Int = True)
	End Method
	
	Method getText:String()
	End Method
	 
	Method inheritsAlpha:Int()
	End Method
	 
	Method GetAlpha:Float()
	End Method
	 
	Method getEffectiveAlpha:Float()
	End Method
	 
	Method getPixelRect(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	 
	Method getInnerRect(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	 
	Method getUnclippedPixelRect(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	 
	Method getUnclippedInnerRect(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	 
	Method isCapturedByThis:Int()
	End Method
	 
	Method isCapturedByAncestor:Int()
	End Method
	 
	Method isCapturedByChild:Int()
	End Method
	 
	Method isHit:Int(x:Float, y:Float)
	End Method
	 
	Method getChildAtPosition:TCEWindow(x:Float, y:Float)
	End Method
	 
	Method getTargetChildAtPosition:TCEWindow(x:Float, y:Float)
	End Method
	 
	Method GetParent:TCEWindow()
	End Method
	 
	Method getMouseCursor:TCEImage(useDefault:Int = True)
	End Method
	 
	Method getPixelSize(width:Float Var, height:Float Var)
	End Method
	 
	Method getUserData:Object()
	End Method
	 
	Method restoresOldCapture:Int()
	End Method
	 
	Method isZOrderingEnabled:Int()
	End Method
	 
	Method wantsMultiClickEvents:Int()
	End Method
	 
	Method isMouseAutoRepeatEnabled:Int()
	End Method
	 
	Method getAutoRepeatDelay:Float()
	End Method
	 
	Method getAutoRepeatRate:Float()
	End Method
	 
	Method distributesCapturedInputs:Int()
	End Method
	 
	Method isUsingDefaultTooltip:Int()
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

	Method setClippedByParent(setting:Int)
	End Method
	
	Method setID(ID:Int)
	End Method
	
	Method setPrefix(prefix:String)
	End Method
	
	Method insertText(text:String, position:String)
	End Method
	
	Method appendText(text:String)
	End Method
	
	Method setFont(font:TCEFont)
	End Method
	
	Method setFontByName(name:String)
	End Method
	
	Method removeChildWindow(window:Object)
		If TCEWindow(window) Then
		ElseIf String(window) Then
		End If
	End Method
	
	Method removeChildWindowForID(ID:Int)
	End Method
	
	Method moveToFront()
	End Method
	
	Method moveToBack()
	End Method
	
	Method captureInput:Int()
	End Method
	
	Method releaseInput()
	End Method
	
	Method setRestoreCapture(setting:Int)
	End Method
	
	Method SetAlpha(alpha:Float)
	End Method
	
	Method setInheritsAlpha(setting:Int)
	End Method
	
	Method requestRedraw()
	End Method
	
	Method setMouseCursor(image:TCEImage)
	End Method
	
	Method setMouseCursorMode(image:Int)
	End Method
	
	Method setMouseCursorByName(imageset:String, imageName:String)
	End Method
	
	'method setUserData(void *user_data)
	'end method
	
	Method setZOrderingEnabled(setting:Int)
	End Method
	
	Method setWantsMultiClickEvents(setting:Int)
	End Method
	
	Method setMouseAutoRepeatEnabled(setting:Int)
	End Method
	
	Method setAutoRepeatDelay(Delay:Float)
	End Method
	
	Method setAutoRepeatRate(rate:Float)
	End Method
	
	Method setDistributesCapturedInputs(setting:Int)
	End Method
	
	Method notifyDragDropItemEnters(item:TCEDragContainer)
	End Method
	
	Method notifyDragDropItemLeaves(item:TCEDragContainer)
	End Method
	
	Method notifyDragDropItemDropped(item:TCEDragContainer)
	End Method
	
	Method destroy()
	End Method
	
	Method setTooltip(tooltip:TCETooltip)
	End Method
	
	Method setTooltipType(tooltipType:String)
	End Method
	
	Method setTooltipText(tip:String)
	End Method
	
	Method setInheritsTooltipText(setting:Int)
	End Method
	
	Method setRiseOnClickEnabled(setting:Int)
	End Method
	
	Method setVerticalAlignment(alignment:Int)
	End Method
	
	Method setHorizontalAlignment(alignment:Int)
	End Method
	
	Method setLookNFeel(look:String)
	End Method
	
	Method setModalState(state:Int)
	End Method
	
	Method performChildWindowLayout()
	End Method
	
	Method setUserString(name:String, value:String)
	End Method
	
	'method setArea(const UDim &xpos, UDim &ypos, UDim &width, UDim &height)
	'end method
	
	'method setArea(const UVector2 &pos, UVector2 &size)
	'end method
	
	'method setArea(const URect &area)
	'end method

	Rem
	bbdoc: Set the window's position.
	about: Sets the position of the area occupied by this window. The position is offset from the top-left corner
	of this windows parent window or from the top-left corner of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setPosition(x:Float, y:Float)
		bmx_cegui_window_setposition(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Set the window's X position.
	about: Sets the x position (left edge) of the area occupied by this window. The position is offset from
	the left edge of this windows parent window or from the left edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setXPosition(x:Float)
		bmx_cegui_window_setxposition(objectPtr, x)
	End Method
	 
	Rem
	bbdoc: Set the window's Y position.
	about: Sets the y position (top edge) of the area occupied by this window. The position is offset from
	the top edge of this windows parent window or from the top edge of the display if this window has no parent
	(i.e. it is the root window).
	End Rem
	Method setYPosition(y:Float)
		bmx_cegui_window_setyposition(objectPtr, y)
	End Method
	 
	Rem
	bbdoc: Set the window's size.
	about: Sets the size of the area occupied by this window.
	End Rem
	Method setSize(width:Float, height:Float)
		bmx_cegui_window_setsize(objectPtr, width, height)
	End Method
	 
	Rem
	bbdoc: Set the window's width.
	about: Sets the width of the area occupied by this window.
	End Rem
	Method setWidth(width:Float)
		bmx_cegui_window_setwidth(objectPtr, width)
	End Method
	 
	Rem
	bbdoc: Set the window's height.
	about: Sets the height of the area occupied by this window.
	End Rem
	Method setHeight(height:Float)
		bmx_cegui_window_setheight(objectPtr, height)
	End Method
	 
	Rem
	bbdoc: Set the window's maximum size.
	about: Sets the maximum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method setMaxSize(width:Float, height:Float)
		bmx_cegui_window_setmaxsize(objectPtr, width, height)
	End Method
	 
	Rem
	bbdoc: Set the window's minimum size.
	about: Sets the minimum size that this windows area may occupy (whether size changes occur by user interaction,
	general system operation, or by direct setting by client code).
	End Rem
	Method setMinSize(width:Float, height:Float)
		bmx_cegui_window_setminsize(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: 
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
	
	Method getTooltip:TCETooltip()
	End Method
	
	Method getTooltipType:String()
	End Method
	
	Method getTooltipText:String()
	End Method
	
	Method inheritsTooltipText:Int()
	End Method
	
	Method isRiseOnClickEnabled:Int()
	End Method
	
	Method testClassName:Int(className:String)
	End Method
	
	Method getVerticalAlignment:Int()
	End Method
	
	Method getHorizontalAlignment:Int()
	End Method
	
	Method getLookNFeel:String()
	End Method
	
	Method getModalState:Int()
	End Method
	
	Method getUserString:String(name:String)
	End Method
	
	Method isUserStringDefined(name:String)
	End Method
	
	Method getActiveSibling:TCEWindow()
	End Method
	
	Method getParentPixelSize(width:Float Var, height:Float Var)
	End Method
	
	Method getParentPixelWidth:Float()
	End Method
	
	Method getParentPixelHeight:Float()
	End Method
	
	Method isMousePassThroughEnabled:Int()
	End Method
	
	Method isAutoWindow:Int()
	End Method
	
	Method isWritingXMLAllowed:Int()
	End Method
	
	Method isDragDropTarget:Int()
	End Method
	
	Method rename(newName:String)
	End Method
	
	Method initialiseComponents()
	End Method
	
	Method setDestroyedByParent(setting:Int)
	End Method
	
	Method setAlwaysOnTop(setting:Int)
	End Method
	
	Method setEnabled(setting:Int)
	End Method
	
	Method enable()
	End Method
	
	Method disable()
	End Method
	
	Method setVisible(setting:Int)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method setText(text:String)
		bmx_cegui_window_settext(objectPtr, _convertMaxToUTF8(text))
	End Method

	Method getArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method getPosition(x:Float Var, y:Float Var)
	End Method
	
	Method getXPosition:Float()
	End Method
	
	Method getYPosition:Float()
	End Method
	
	Method getSize(w:Float Var, h:Float Var)
	End Method
	
	Method getWidth:Float()
	End Method
	
	Method getHeight:Float()
	End Method
	
	Method getMaxSize(w:Float Var, h:Float Var)
	End Method
	
	Method getMinSize(w:Float Var, h:Float Var)
	End Method
	
	Method render()
	End Method
	
	Method beginInitialisation()
	End Method
	
	Method endInitialisation()
	End Method
	
	Method setMousePassThroughEnabled(setting:Int)
	End Method
	
	Method setWindowRenderer(name:String)
	End Method
	
	Method getWindowRendererName:String()
	End Method
	
	Method setWritingXMLAllowed(allow:Int)
	End Method
	
	Method notifyScreenAreaChanged()
	End Method
	
	Method setFalagardType(_type:String, rendererType:String = "")
	End Method
	
	Method setDragDropTarget(setting:Int)
	End Method

	' properties
	Method removeProperty(name:String)
	End Method
	
	Method clearProperties()
	End Method
	
	Method isPropertyPresent:Int(name:String)
	End Method
	
	Method getPropertyHelp:String(name:String)
	End Method
	
	Method getProperty:String(name:String)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method setProperty(name:String, value:String)
		bmx_cegui_window_setproperty(objectPtr, _convertMaxToUTF8(name), _convertMaxToUTF8(value))
	End Method
	
	Method isPropertyDefault:Int(name:String)
	End Method
	
	Method getPropertyDefault:String(name:String)
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
	bbdoc: Set the level of logging information that will get out to the log file.
	about: One of LOG_ERRORS, LOG_WARNINGS, LOG_STANDARD, LOG_INFORMATIVE or LOG_INSANE.
	End Rem
	Method setLoggingLevel(level:Int)
		bmx_cegui_logger_setlogginglevel(level)
	End Method
	
	Rem
	bbdoc: Set the name of the log file where all subsequent log entries should be written.
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
	bbdoc: 
	End Rem
	Function CreateColourRect:TCEColourRect(colour:TCEColour)
		Return New TCEColourRect.Create(colour)
	End Function
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Function CreateColour:TCEColour(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		Return New TCEColour.Create(red, green, blue, alpha)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TCEColour(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		objectPtr = bmx_cegui_colour_create(red, green, blue, alpha)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getARGB:Int()
		Return bmx_cegui_colour_getargb(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAlpha:Float()
		Return bmx_cegui_colour_getalpha(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getRed:Float()
		Return bmx_cegui_colour_getred(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getGreen:Float()
		Return bmx_cegui_colour_getgreen(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getBlue:Float()
		Return bmx_cegui_colour_getblue(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getHue:Float()
		Return bmx_cegui_colour_gethue(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getSaturation:Float()
		Return bmx_cegui_colour_getsaturation(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getLumination:Float()
		Return bmx_cegui_colour_getlumination(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setARGB(argb:Int)
		bmx_cegui_colour_setargb(objectPtr, argb)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAlpha(alpha:Float)
		bmx_cegui_colour_setalpha(objectPtr, alpha)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setRed(red:Float)
		bmx_cegui_colour_setred(objectPtr, red)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setGreen(green:Float)
		bmx_cegui_colour_setgreen(objectPtr, green)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setBlue(blue:Float)
		bmx_cegui_colour_setblue(objectPtr, blue)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method set(red:Float, green:Float, blue:Float, alpha:Float = 1.0)
		bmx_cegui_colour_set(objectPtr, red, green, blue, alpha)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setRGB(red:Float, green:Float, blue:Float)
		bmx_cegui_colour_setrgb(objectPtr, red, green, blue)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setHSL(hue:Float, saturation:Float, luminance:Float, alpha:Float = 1.0)
		bmx_cegui_colour_sethsl(objectPtr, hue, saturation, luminance, alpha)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method invertColour()
		bmx_cegui_colour_inverColour(objectPtr)
	End Method
	
	Rem
	bbdoc: 
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

	Function createFont:TCEFont(filename:String, resourceGroup:String = "")
		Return TCEFont._create(bmx_cegui_fontmanager_createfont(_convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Function
	
	Function createFontType:TCEFont(fontType:String, name:String, FontName:String, resourceGroup:String = "")
	End Function
	
	Function destroyFont(font:Object)
		If TCEFont(font) Then
		ElseIf String(font) Then
		End If
	End Function
	
	Function destroyAllFonts()
	End Function
	
	Function isFontPresent:Int(name:String)
		Return bmx_cegui_fontmanager_isfontpresent(_convertMaxToUTF8(name))
	End Function
	
	Function getFont:TCEFont(name:String)
	End Function
	
	Function notifyScreenResolution(width:Float, height:Float)
	End Function

End Type


Rem
bbdoc: Manages the creation of, access to, and destruction of GUI Scheme objects.
End Rem
Type TCESchemeManager

	Rem
	bbdoc: Loads a scheme.
	End Rem
	Function loadScheme:TCEScheme(scheme:String, resourceGroup:String = "")
		Return TCEScheme._create(bmx_cegui_schememanager_loadScheme(_convertMaxToUTF8(scheme), _convertMaxToUTF8(resourceGroup)))
	End Function

	Rem
	bbdoc: Returns true if the named Scheme is present in the system (though the resources for the scheme may or may not be loaded).
	End Rem
	Function isSchemePresent:Int(scheme:String)
		Return bmx_cegui_schememanager_isschemepresent(_convertMaxToUTF8(scheme))
	End Function
	
	Rem
	bbdoc: Returns the Scheme object with the specified name.
	End Rem
	Function getScheme:TCEScheme(name:String)
		Return TCEScheme._create(bmx_cegui_schememanager_getscheme(_convertMaxToUTF8(name)))
	End Function
	
	Rem
	bbdoc: Unload all schemes currently defined within the system.
	about: Calling this method has the potential to be very dangerous; if any of the data that forms
	part of the scheme is still in use, you can expect fireworks shortly after!
	End Rem
	Function unloadAllSchemes()
		bmx_cegui_schememanager_unloadallschemes()
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

	Method isCodepointAvailable:Int(cp:Int)
	End Method
	
	Method getLineSpacing:Float(yScale:Float = 1.0)
	End Method

	Method getFontHeight:Float(yScale:Float = 1.0)
	End Method

	Method getBaseline:Float(yScale:Float = 1.0)
	End Method
	
End Type

Rem
bbdoc: 
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

	Method getSize(width:Float Var, height:Float Var)
	End Method
	
	Method getWidth:Float()
	End Method
	
	Method getHeight:Float()
	End Method
	
	Method getOffsets(x:Float Var, y:Float Var)
	End Method
	
	Method getOffsetX:Float()
	End Method
	
	Method getOffsetY:Float()
	End Method
	
	Method getName:String()
	End Method
	
	Method getImagesetName:String()
	End Method
	
	Method getImageset:TCEImageset()
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
	bbdoc: Create a Imageset object with the given name and Texture.
	about: The created Imageset will be of limited use, and will require one or more images to
	be defined for the set.
	End Rem
	Function createImageset:TCEImageset(filename:String, resourceGroup:String = "")
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimageset(_convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Function
	
	Rem
	bbdoc: Create an Imageset object from the specified file.
	End Rem
	Function createImagesetFromTexture:TCEImageset(name:String, texture:TCETexture)
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimagesetfromtexture(_convertMaxToUTF8(name), texture.objectPtr))
	End Function
	
	Rem
	bbdoc: Create an Imageset object from the specified image file.
	about: The Imageset will initially have a single image defined named "full_image" which is
	an image that represents the entire area of the loaded image.
	End Rem
	Function createImagesetFromImageFile:TCEImageset(name:String, filename:String, resourceGroup:String = "")
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimagesetfromimagefile(_convertMaxToUTF8(name), _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Function
	
	Rem
	bbdoc: Destroys the Imageset.
	about: Accepts either the imageset name, or an imageset object.
	End Rem
	Function destroyImageSet(imageset:Object)
		If TCEImageset(imageset) Then
			bmx_cegui_imagesetmanager_destroyimageset(TCEImageset(imageset).objectPtr)
		ElseIf String(imageset) Then
			bmx_cegui_imagesetmanager_destroyimagesetname(_convertMaxToUTF8(String(imageset)))
		End If
	End Function
	
	Rem
	bbdoc: Destroys all Imageset objects registered in the system.
	End Rem
	Function destroyAllImagesets()
		bmx_cegui_imagesetmanager_destroyallimagesets()
	End Function
	
	Rem
	bbdoc: Returns the Imageset object with the specified name.
	End Rem
	Function getImageset:TCEImageset(name:String)
		Return TCEImageset._create(bmx_cegui_imagesetmanager_getimageset(_convertMaxToUTF8(name)))
	End Function
	
	Rem
	bbdoc: Check for the existence of a named Imageset.
	End Rem
	Function isImagesetPresent:Int(name:String)	
		Return bmx_cegui_imagesetmanager_isimagesetpresent(_convertMaxToUTF8(name))
	End Function
	
	Rem
	bbdoc: Notify the ImagesetManager of the current (usually new) display resolution.
	End Rem
	Function notifyScreenResolution(width:Float, height:Float)
		bmx_cegui_imagesetmanager_notifyscreenresolution(width, height)
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
	bbdoc: Returns the current pixel width of the texture.
	End Rem
	Method getWidth:Int()
		Return bmx_cegui_texture_getwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the original pixel width of the data loaded into the texture.
	End Rem
	Method getOriginalWidth:Int()
		Return bmx_cegui_texture_getoriginalwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current scale used for the width of the texture.
	End Rem
	Method getXScale:Float()
		Return bmx_cegui_texture_getxscale(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current pixel height of the texture.
	End Rem
	Method getHeight:Int()
		Return bmx_cegui_texture_getheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the original pixel height of the data loaded into the texture.
	End Rem
	Method getOriginalHeight:Int()
		Return bmx_cegui_texture_getoriginalheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current scale used for the height of the texture.
	End Rem
	Method getYScale:Float()
		Return bmx_cegui_texture_getyscale(objectPtr)
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

	Method getName:String()
	End Method
	
	Method getImageCount:Int()
	End Method
	
	Method isImageDefined:Int(name:String)
	End Method
	
	'method getImage:TCEImage(name:String)
	'End method
	
	Method undefineImage(name:String)
	End Method
	
	Method undefinAllImages()
	End Method
	
	Method getImageSize(name:String, width:Float Var, height:Float Var)
	End Method
	
	Method getImageWidth:Float(name:String)
	End Method
	
	Method getImageHeight:Float(name:String)
	End Method
	
	Method getImageOffset(name:String, x:Float Var, y:Float Var)
	End Method
	
	Method getImageOffsetX:Float(name:String)
	End Method
	
	Method getImageOffsetY:Float(name:String)
	End Method
	
	Method defineImage(name:String, x:Float, y:Float, width:Float, height:Float, renderOffsetX:Float, renderOffsetY:Float)
	End Method
	
	Method isAutoScaled:Int()
	End Method
	
	Method getNativeResolution(width:Float Var, height:Float Var)
	End Method
	
	Method setAutoScalingEnabled(setting:Int)
	End Method

	Method setNativeResolution(width:Float, height:Float)
	End Method
	
	Method notifyScreenResolution(width:Float, height:Float)
	End Method
	
End Type

Rem
bbdoc: The means by which the GUI system interfaces with specific rendering technologies.
about: To use a rendering system or API to draw CEGUI imagery requires that an appropriate Renderer object be available.
End Rem
Type TCERenderer

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Create a Texture object using the given image file.
	about: Textures are always created with a size that is a power of 2. If the file you specify is
	of a size that is not a power of two, the final size will be rounded up. Additionally, textures are
	always square, so the ultimate size is governed by the larger of the width and height of the
	specified file. You can check the ultimate sizes by querying the texture after creation.
	End Rem
	Method CreateTexture:TCETexture(filename:String, resourceGroup:String)
		Return TCETexture(bmx_cegui_renderer_createtexture(objectPtr, _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
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
	Method createTextureWithSize:TCETexture(size:Float)
		Return TCETexture(bmx_cegui_renderer_createtexturewithsize(objectPtr, size))
	End Method
	
	Rem
	bbdoc: Destroys the given Texture.
	End Rem
	Method destroyTexture(texture:TCETexture)
		bmx_cegui_renderer_destroytexture(objectPtr, texture.objectPtr)
	End Method
	
	Rem
	bbdoc: Destroys all Texture objects.
	End Rem
	Method destroyAllTextures()
		bmx_cegui_renderer_destroyalltextures(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether queueing is enabled.
	End Rem
	Method isQueueingEnabled:Int()
		Return bmx_cegui_renderer_isqueueingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current width of the display in pixels.
	End Rem
	Method getWidth:Float()
		Return bmx_cegui_renderer_getwidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current height of the display in pixels.
	End Rem
	Method getHeight:Float()
		Return bmx_cegui_renderer_getheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the display in pixels.
	End Rem
	Method getSize(width:Float Var, height:Float Var)
		bmx_cegui_renderer_getsize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: Returns the maximum texture size available.
	End Rem
	Method getMaxTextureSize:Int()
		Return bmx_cegui_renderer_getmaxtexturesize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the horizontal display resolution dpi.
	End Rem
	Method getHorzScreenDPI:Int()
		Return bmx_cegui_renderer_gethorzscreendpi(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the vertical display resolution dpi.
	End Rem
	Method getVertScreenDPI:Int()
		Return bmx_cegui_renderer_getvertscreendpi(objectPtr)
	End Method
	
	Rem
	bbdoc: Resets the z co-ordinate for rendering.
	End Rem
	Method resetZValue()
		bmx_cegui_renderer_resetzvalue(objectPtr)
	End Method
	
	Rem
	bbdoc: Updates the z co-ordinate for the next major UI element (window).
	End Rem
	Method advanceZValue()
		bmx_cegui_renderer_advancezvalue(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the z co-ordinate to use for the requested layer on the current GUI element.
	End Rem
	Method getCurrentZ:Float()
		Return bmx_cegui_renderer_getcurrentz(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current Z value to use (equates to layer 0 for this UI element).
	End Rem
	Method getZLayer:Float(layer:Int)
		Return bmx_cegui_renderer_getzlayer(objectPtr, layer)
	End Method
	
	Rem
	bbdoc: Return identification string for the renderer module.
	about: If the internal id string has not been set by the Renderer module creator, a generic string
	of "Unknown renderer" will be returned.
	End Rem
	Method getIdentifierString:String()
		Return _convertUTF8ToMax(bmx_cegui_renderer_getidentifierstring(objectPtr))
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
bbdoc: 
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

