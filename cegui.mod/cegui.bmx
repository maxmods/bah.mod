' Copyright (c) 2008 Bruce A Henderson
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

Rem
bbdoc: CEGUI - Graphics based GUI.
End Rem
Module BaH.CEGUI

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: CEGUI - 2004 - 2006 Paul D Turner & The CEGUI Development Team"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (CEGUI 0.6.1)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DPCRE_STATIC"
ModuleInfo "CC_OPTS: -DCEGUI_STATIC -DCEGUI_FALAGARD_RENDERER -DCEGUI_WITH_TINYXML -DCEGUI_CODEC_FREEIMAGE"

?win32
ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB -DPNG_iTXt_SUPPORTED -DHAVE_PROTOTYPES -DOPJ_STATIC"
ModuleInfo "CC_OPTS: -DGLEW_STATIC"
?

Import Pub.FreeType
Import BaH.RegEx
Import BaH.FreeImage

Import BRL.Map
Import BRL.PolledInput

Import "common.bmx"

'
' NOTES
'
' Modified CEGUIConfig.h
'  Changed to include options for all platforms.
'  Added macPlugins.h include... for Mac target.
'
' Modified macPlugins.cpp
'  Commented out "main" method. Looks like this isn't designed to build static on Mac... haw.
'
' CEGUIFreeImageImageCodec.cpp
'  Fixed endian colour issue on Mac.
'

' some global stuff
Global cegui_rendererPtr:Byte Ptr
Global cegui_startTime:Int

Global ce_event_handler:TCEEventHandler = New TCEEventHandler

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

' TODO : not sure if this gets called?
OnEnd cegui_cleanup


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
bbdoc: 
End Rem
Type TCEMouseCursorEventArgs Extends TCEEventArgs

	Function _create:TCEMouseCursorEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEMouseCursorEventArgs = New TCEMouseCursorEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCEWindowEventArgs Extends TCEEventArgs

	Function _create:TCEWindowEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEWindowEventArgs = New TCEWindowEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the Window object of relevance to the event. 
	End Rem
	Method getWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_windoweventargs_getwindow(eventArgsPtr))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEActivationEventArgs Extends TCEWindowEventArgs

	Function _create:TCEActivationEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEActivationEventArgs = New TCEActivationEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the other window involved in the activation change.
	End Rem
	Method getOtherWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_activationeventargs_getotherwindow(eventArgsPtr))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEDragDropEventArgs Extends TCEWindowEventArgs

	Function _create:TCEDragDropEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEDragDropEventArgs = New TCEDragDropEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method getDragDropItem:TCEDragContainer()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEHeaderSequenceEventArgs Extends TCEWindowEventArgs

	Function _create:TCEHeaderSequenceEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEHeaderSequenceEventArgs = New TCEHeaderSequenceEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Method getOldIndex:Int()
	End Method
	
	Method getNewIndex:Int()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEKeyEventArgs Extends TCEWindowEventArgs

	Function _create:TCEKeyEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEKeyEventArgs = New TCEKeyEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEMouseEventArgs Extends TCEWindowEventArgs

	Function _create:TCEMouseEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEMouseEventArgs = New TCEMouseEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCETreeEventArgs Extends TCEWindowEventArgs

	Function _create:TCETreeEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCETreeEventArgs = New TCETreeEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Method getTreeItem:TCETreeItem()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCEUpdateEventArgs Extends TCEWindowEventArgs

	Function _create:TCEUpdateEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEUpdateEventArgs = New TCEUpdateEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the time since the last frame update. 
	End Rem
	Method getTimeSinceLastFrame:Float()
		Return bmx_cegui_updateeventargs_gettimesincelastframe(eventArgsPtr)
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
	bbdoc: Render the GUI.
	End Rem
	Function renderGUI()
		bmx_cegui_system_renderGUI(cegui_systemPtr)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function setDefaultFont(font:String)
		bmx_cegui_system_setDefaultFont(cegui_systemPtr, _convertMaxToUTF8(font))
	End Function
	
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
	
	Function getRenderer:TCERenderer()
		
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

	Function isSchemePresent:Int(scheme:String)
	End Function
	
	Function getScheme:TCEScheme(name:String)
	End Function
	
	Function unloadAllSchemes()
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
	
	
	
	

	Rem
	bbdoc: 
	End Rem
	Method setText(text:String)
		bmx_cegui_window_settext(objectPtr, _convertMaxToUTF8(text))
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

Rem
bbdoc: 
End Rem
Type TCEFrameWindow Extends TCEWindow

	Const EventRollupToggled:String = "RollupToggled"
	Const EventCloseClicked:String = "CloseClicked"


	Function _create:TCEFrameWindow(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEFrameWindow = New TCEFrameWindow
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns whether this window is sizable.
	about: Note that this requires that the window have an enabled frame and that sizing itself is enabled.
	End Rem
	Method isSizingEnabled:Int()
		Return bmx_cegui_framewindow_issizingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the frame for this window is enabled. 
	End Rem
	Method isFrameEnabled:Int()
		Return bmx_cegui_framewindow_isframeenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the title bar for this window is enabled. 
	End Rem
	Method isTitleBarEnabled:Int()
		Return bmx_cegui_framewindow_istitlebarenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this close button for this window is enabled. 
	End Rem
	Method isCloseButtonEnabled:Int()
		Return bmx_cegui_framewindow_isclosebuttonenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether roll up (a.k.a shading) is enabled for this window. 
	End Rem
	Method isRollupEnabled:Int()
		Return bmx_cegui_framewindow_isrollupenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the window is currently rolled up (a.k.a shaded). 
	End Rem
	Method isRolledup:Int()
		Return bmx_cegui_framewindow_isrolledup(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the thickness of the sizing border. 
	End Rem
	Method getSizingBorderThickness:Float()
		Return bmx_cegui_framewindow_getsizingborderthickness(objectPtr)
	End Method
	
	Rem
	bbdoc: Enables or disables sizing for this window. 
	End Rem
	Method setSizingEnabled(setting:Int)
		bmx_cegui_framewindow_setsizingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Enables or disables the frame for this window. 
	End Rem
	Method setFrameEnabled(setting:Int)
		bmx_cegui_framewindow_setframeenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Enables or disables the title bar for the frame window. 
	End Rem
	Method setTitleBarEnabled(setting:Int)
		bmx_cegui_framewindow_settitlebarenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Enables or disables the close button for the frame window. 
	End Rem
	Method setCloseButtonEnabled(setting:Int)
		bmx_cegui_framewindow_setclosebuttonenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Enables or disables roll-up (shading) for this window. 
	End Rem
	Method setRollupEnabled(setting:Int)
		bmx_cegui_framewindow_rollupenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Toggles the state of the window between rolled-up (shaded) and normal sizes.
	about: This requires roll-up to be enabled. 
	End Rem
	Method toggleRollup()
		bmx_cegui_framewindow_togglerollup(objectPtr)
	End Method
	
	Rem
	bbdoc: Set the size of the sizing border for this window. 
	End Rem
	Method setSizingBorderThickness(pixels:Float)
		bmx_cegui_framewindow_setsizingborderthickness(objectPtr, pixels)
	End Method
	
	Rem
	bbdoc: Returns whether this FrameWindow can be moved by dragging the title bar. 
	End Rem
	Method isDragMovingEnabled:Int()
		Return bmx_cegui_framewindow_isdragmovingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this FrameWindow can be moved by dragging the title bar. 
	End Rem
	Method setDragMovingEnabled(setting:Int)
		bmx_cegui_framewindow_setdragmovingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Checks if the given pixel position would hit this window. 
	End Rem
	Method isHit:Int(x:Float, y:Float)
		Return bmx_cegui_framewindow_ishit(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the Titlebar component widget for this FrameWindow. 
	End Rem
	Method getTitlebar:TCETitlebar()
		Return TCETitlebar(bmx_cegui_framewindow_gettitlebar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the close button component widget for this FrameWindow. 
	End Rem
	Method getCloseButton:TCEPushButton()
		Return TCEPushButton(bmx_cegui_framewindow_getclosebutton(objectPtr))
	End Method
	
End Type

Rem
bbdoc: Type representing the title bar for Frame Windows.
End Rem
Type TCETitlebar Extends TCEWindow

	Function _create:TCETitlebar(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETitlebar = New TCETitlebar
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns whether this title bar will respond to dragging. 
	End Rem
	Method isDraggingEnabled:Int()
		Return bmx_cegui_titlebar_isdraggingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this title bar widget will respond to dragging. 
	End Rem
	Method setDraggingEnabled(setting:Int)
		bmx_cegui_titlebar_setdraggingenabled(objectPtr, setting)
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
	
	Method loadResources()
	End Method
	
	Method unloadResources()
	End Method
	
	Method resourcesLoaded:Int()
	End Method
	
	Method getName:String()
	End Method

End Type


Rem
bbdoc: Base type for all the 'button' type widgets (push button, radio button, check-box, etc).
End Rem
Type TCEButtonBase Extends TCEWindow

	Rem
	bbdoc: Returns true if user is hovering over this widget (or it's pushed and user is not over it for highlight).
	End Rem
	Method isHovering:Int()
		Return bmx_cegui_buttonbase_ishovering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the button widget is in the pushed state.
	End Rem
	Method isPushed:Int()
		Return bmx_cegui_buttonbase_ispushed(objectPtr)
	End Method
	
End Type

Rem
bbdoc: Base type providing logic for Check-box widgets.
End Rem
Type TCECheckbox Extends TCEButtonBase

	Function _create:TCECheckbox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCECheckbox = New TCECheckbox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The check-state of the widget has changed.
	End Rem
	Const EventCheckStateChanged:String = "CheckStateChanged"

	Rem
	bbdoc: Returns true if the check-box is selected (has the checkmark)
	returns: True if the widget is selected and has the check-mark, False if the widget is not selected and does not have the check-mark.
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_checkbox_isselected(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the check-box is selected or not.
	End Rem
	Method setSelected(selected:Int)
		bmx_cegui_checkbox_setselected(objectPtr, selected)
	End Method
	
End Type

Rem
bbdoc: Provides logic for push button type widgets.
End Rem
Type TCEPushButton Extends TCEButtonBase

	Function _create:TCEPushButton(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEPushButton = New TCEPushButton
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The button was clicked. 
	End Rem
	Const EventClicked:String = "Clicked"

End Type

Rem
bbdoc: 
End Rem
Type TCETabButton Extends TCEButtonBase

	Function _create:TCETabButton(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETabButton = New TCETabButton
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type to provide the logic for Radio Button widgets. 
End Rem
Type TCERadioButton Extends TCEButtonBase

	Function _create:TCERadioButton(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCERadioButton = New TCERadioButton
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The selected state of the widget has changed. 
	End Rem
	Const EventSelectStateChanged:String = "SelectStateChanged"
	
	Rem
	bbdoc: Returns true if the radio button is selected (has the checkmark).
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_radiobutton_isselected(objectPtr)
	End Method

	Rem
	bbdoc: Returns the groupID assigned to this radio button.
	End Rem
	Method getGroupID:Int()
		Return bmx_cegui_radiobutton_getgroupid(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the RadioButton object within the same group as this RadioButton, that is currently selected. 
	End Rem
	Method getSelectedButtonInGroup:TCERadioButton()
		Return TCERadioButton(bmx_cegui_radiobutton_getselectedbuttoningroup(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets whether the radio button is selected or not.
	End Rem
	Method setSelected(selected:Int)
		bmx_cegui_radiobutton_setselected(objectPtr, selected)
	End Method
	
	Rem
	bbdoc: Sets the groupID for this radio button.
	End Rem
	Method setGroupID(group:Int)
		bmx_cegui_radiobutton_setgroupid(objectPtr, group)
	End Method

End Type

Rem
bbdoc: Base type for standard Listbox widget.
End Rem
Type TCEListbox Extends TCEWindow

	Function _create:TCEListbox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListbox = New TCEListbox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event triggered when the contents of the list is changed. 
	End Rem
	Const EventListContentsChanged:String = "ListItemsChanged"
	Rem
	bbdoc: Event triggered when there is a change to the currently selected item(s). 
	End Rem
	Const EventSelectionChanged:String = "ItemSelectionChanged"
	Rem
	bbdoc: Event triggered when the sort mode setting changes. 
	End Rem
	Const EventSortModeChanged:String = "SortModeChanged"
	Rem
	bbdoc: Event triggered when the multi-select mode setting changes. 
	End Rem
	Const EventMultiselectModeChanged:String = "MuliselectModeChanged"
	Rem
	bbdoc: Event triggered when the vertical scroll bar 'force' setting changes.
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollModeChanged"
	Rem
	bbdoc: Event triggered when the horizontal scroll bar 'force' setting changes. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollModeChanged"



	Method getItemCount:Int()
	End Method
	
	Method getSelectedCount:Int()
	End Method
	 
	Method getFirstSelectedItem:TCEListboxItem()
	End Method
	 
	Method getNextSelected:TCEListboxItem(start_item:TCEListboxItem)
	End Method
	 
	Method getListboxItemFromIndex:TCEListboxItem(index:Int)
	End Method
	 
	Method getItemIndex:Int(item:TCEListboxItem)
	End Method
	 
	Method isSortEnabled:Int()
	End Method
	 
	Method isMultiselectEnabled:Int()
	End Method
	 
	Method isItemTooltipsEnabled:Int()
	End Method
	
	Method isItemSelected(index:Int)
	End Method
	 
	Method findItemWithText:TCEListboxItem(text:String, start_item:TCEListboxItem)
	End Method
	 
	Method isListboxItemInList:Int(item:TCEListboxItem)
	End Method
	 
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	 
	Method isHorzScrollbarAlwaysShown:Int()
	End Method
	 
	Method initialiseComponents()
	End Method
	 
	Method resetList()
	End Method
	 
	Method addItem(item:TCEListboxItem)
	End Method
	 
	Method insertItem(item:TCEListboxItem, position:TCEListboxItem)
	End Method
	 
	Method removeItem(item:TCEListboxItem)
	End Method
	 
	Method clearAllSelections()
	End Method
	 
	Method setSortingEnabled(setting:Int)
	End Method
	 
	Method setMultiselectEnabled(setting:Int)
	End Method
	 
	Method setShowVertScrollbar(setting:Int)
	End Method
	 
	Method setShowHorzScrollbar(setting:Int)
	End Method
	 
	Method setItemTooltipsEnabled(setting:Int)
	End Method
	
	Method setItemSelectState(item:TCEListboxItem, state:Int)
	End Method
	 
	Method setItemSelectStateForIndex(item_index:Int, state:Int)
	End Method
	 
	Method handleUpdatedItemData()
	End Method
	 
	Method ensureItemIsVisibleForIndex(item_index:Int)
	End Method
	 
	Method ensureItemIsVisible(item:TCEListboxItem)
	End Method
	 
	Method getListRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	 
	Method getVertScrollbar:TCEScrollbar()
	End Method
	 
	Method getHorzScrollbar:TCEScrollbar()
	End Method
	 
	Method getTotalItemsHeight:Float()
	End Method
	 
	Method getWidestItemWidth:Float()
	End Method

	
End Type

Rem
bbdoc: Base type for the combo box drop down list.
about: This is a specialisation of TCEListbox. 
End Rem
Type TCEComboDropList Extends TCEListbox

	Function _create:TCEComboDropList(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEComboDropList = New TCEComboDropList
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Event fired when the user confirms the selection by clicking the mouse. 
	End Rem
	Const EventListSelectionAccepted:String = "ListSelectionAccepted"

	Rem
	bbdoc: Sets whether the drop-list is 'armed' for selection. 
	End Rem
	Method setArmed(setting:Int)
		bmx_cegui_combodroplist_setarmed(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns the 'armed' state of the ComboDropList. 
	End Rem
	Method isArmed:Int()
		Return bmx_cegui_combodroplist_isarmed(objectPtr)
	End Method

	Rem
	bbdoc: Sets the mode of operation for the ComboDropList. 
	End Rem
	Method setAutoArmEnabled(setting:Int)
		bmx_cegui_combodroplist_setautoarmenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns the mode of operation for the drop-list.
	End Rem
	Method isAutoArmEnabled:Int()
		Return bmx_cegui_combodroplist_isautoarmenabled(objectPtr)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCEListboxItem

	Field objectPtr:Byte Ptr

	Function _create:TCEListboxItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListboxItem = New TCEListboxItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEListboxTextItem Extends TCEListboxItem

	Function _create:TCEListboxTextItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListboxTextItem = New TCEListboxTextItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for progress bars.
End Rem
Type TCEProgressBar Extends TCEWindow

	Function _create:TCEProgressBar(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEProgressBar = New TCEProgressBar
			this.objectPtr = objectPtr
			Return this
		End If
	End Function


	Rem
	bbdoc: Event fired whenever the progress value changes. 
	End Rem
	Const EventProgressChanged:String = "ProgressChanged"
	Rem
	bbdoc: Event fired when the progress bar reaches 100%. 
	End Rem
	Const EventProgressDone:String = "ProgressDone"


	Rem
	bbdoc: Returns the current progress value.
	End Rem
	Method getProgress:Float()
		Return bmx_cegui_progressbar_getprogress(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current step size.
	End Rem
	Method getStep:Float()
		Return bmx_cegui_progressbar_getstep(objectPtr)
	End Method
	
	Rem
	bbdoc: Set the current progress.
	End Rem
	Method setProgress(progress:Float)
		bmx_cegui_progressbar_setprogress(objectPtr, progress)
	End Method
	
	Rem
	bbdoc: Set the size of the 'step' in percentage points (default is 0.01 or 1%).
	End Rem
	Method setStepSize(stepVal:Float)
		bmx_cegui_progressbar_setstepsize(objectPtr, stepVal)
	End Method
	
	Rem
	bbdoc: Cause the progress to step.
	about: The amount the progress bar will step can be changed by calling the setStepSize method.
	The default step size is 0.01 which is equal to 1%.
	End Rem
	Method doStep()
		bmx_cegui_progressbar_dostep(objectPtr)
	End Method
	
	Rem
	bbdoc: Modify the progress level by a specified delta.
	End Rem
	Method adjustProgress(delta:Float)
		bmx_cegui_progressbar_adjustprogress(objectPtr, delta)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCECombobox Extends TCEWindow

	Function _create:TCECombobox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCECombobox = New TCECombobox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	' event names from edit box
	Rem
	bbdoc: 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventValidationStringChanged:String = "ValidationStringChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaximumTextLengthChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventTextInvalidated:String = "TextInvalidatedEvent"
	Rem
	bbdoc: 
	End Rem
	Const EventInvalidEntryAttempted:String = "InvalidEntryAttempted"
	Rem
	bbdoc: 
	End Rem
	Const EventCaratMoved:String = "CaratMoved"
	Rem
	bbdoc: 
	End Rem
	Const EventTextSelectionChanged:String = "TextSelectionChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventEditboxFull:String = "EditboxFullEvent"
	Rem
	bbdoc: 
	End Rem
	Const EventTextAccepted:String = "TextAcceptedEvent"
	
	' event names from list box
	Rem
	bbdoc: 
	End Rem
	Const EventListContentsChanged:String = "ListContentsChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventListSelectionChanged:String = "ListSelectionChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSortModeChanged:String = "SortModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"
	
	' events we produce / generate ourselves
	Rem
	bbdoc: 
	End Rem
	Const EventDropListDisplayed:String = "DropListDisplayed"
	Rem
	bbdoc: 
	End Rem
	Const EventDropListRemoved:String = "DropListRemoved"
	Rem
	bbdoc: 
	End Rem
	Const EventListSelectionAccepted:String = "ListSelectionAccepted"
	
	Method isHit:Int(x:Float, y:Float)
	End Method
	
	Method getSingleClickEnabled:Int()
	End Method
	 
	Method isDropDownListVisible:Int()
	End Method
	 
	Method getEditbox:TCEEditbox()
	End Method
	 
	Method getPushButton:TCEPushButton()
	End Method
	 
	Method getDropList:TCEComboDropList()
	End Method
	 
	Method hasInputFocus:Int()
	End Method
	 
	Method isReadOnly:Int()
	End Method
	 
	Method isTextValid:Int()
	End Method
	 
	Method getValidationString:String()
	End Method
	 
	Method getCaratIndex:Int()
	End Method
	 
	Method getSelectionStartIndex:Int()
	End Method
	 
	Method getSelectionEndIndex:Int()
	End Method
	 
	Method getSelectionLength:Int()
	End Method
	 
	Method getMaxTextLength:Int()
	End Method
	 
	Method getItemCount:Int()
	End Method
	 
	Method getSelectedItem:TCEListboxItem()
	End Method
	 
	Method getListboxItemFromIndex:TCEListboxItem(index:Int)
	End Method
	 
	Method getItemIndex:Int(item:TCEListboxItem)
	End Method
	 
	Method isSortEnabled:Int()
	End Method
	 
	Method isItemSelected:Int(index:Int)
	End Method
	 
	Method findItemWithText:TCEListboxItem(text:String, startItem:TCEListboxItem)
	End Method
	 
	Method isListboxItemInList:Int(item:TCEListboxItem)
	End Method
	 
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	 
	Method isHorzScrollbarAlwaysShown:Int()
	End Method
	 
	Method initialiseComponents()
	End Method
	 
	Method showDropList()
	End Method
	 
	Method hideDropList()
	End Method
	 
	Method setSingleClickEnabled(setting:Int)
	End Method
	 
	Method setReadOnly(setting:Int)
	End Method
	 
	Method setValidationString(validationString:String)
	End Method
	 
	Method setCaratIndex(caratPos:Int)
	End Method
	 
	Method setSelection(startPos:Int, endPos:Int)
	End Method
	 
	Method setMaxTextLength(maxLen:Int)
	End Method
	 
	Method activateEditbox()
	End Method
	 
	Method resetList()
	End Method
	 
	Method addItem(item:TCEListboxItem)
	End Method
	 
	Method insertItem(item:TCEListboxItem, position:TCEListboxItem)
	End Method
	 
	Method removeItem(item:TCEListboxItem)
	End Method
	 
	Method clearAllSelections()
	End Method
	 
	Method setSortingEnabled(setting:Int)
	End Method
	 
	Method setShowVertScrollbar(setting:Int)
	End Method
	 
	Method setShowHorzScrollbar(setting:Int)
	End Method
	 
	Method setItemSelectState(item:TCEListboxItem, state:Int)
	End Method
	 
	Method setItemSelectStateIndex(itemIndex:Int, state:Int)
	End Method
	 
	Method handleUpdatedListItemData()
	End Method
	 

End Type

Rem
bbdoc: Window type intended to be used as a simple, generic Window.
about: This type does no rendering and so appears totally transparent. This window defaults to
position 0.0, 0.0 with a size of 1.0 x 1.0.
End Rem
Type TCEGUISheet Extends TCEWindow

	Function _create:TCEGUISheet(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEGUISheet = New TCEGUISheet
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for standard GroupBox widget.
End Rem
Type TCEGroupBox Extends TCEWindow

	Function _create:TCEGroupBox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEGroupBox = New TCEGroupBox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for the Spinner widget.
about: The spinner widget has a text area where numbers may be entered and two buttons which may be used to
increase or decrease the value in the text area by a user specified amount.
<p>
<b>Note:</b>
While the Spinner widget has support for floating point values, the results of using this support in its current
state may not be satisfactory. The general advice, for the moment, is to avoid very large or very small values
in floating point mode, and to perform as little manipulation of the values as possible. The various issues you
may see range from scientific notation appearing in the box, to complete breakdown of 'expected' values upon
manipulation. This is something that we intend to address for a future release.
</p>
End Rem
Type TCESpinner Extends TCEWindow

	Rem
	bbdoc: Display mode - Floating point decimal.
	End Rem
	Const INPUT_FLOAT:Int = 0
	Rem
	bbdoc: Display mode - Integer decimal.
	End Rem
	Const INPUT_INTEGER:Int = 1
	Rem
	bbdoc: Display mode - Hexadecimal.
	End Rem
	Const INPUT_HEX:Int = 2
	Rem
	bbdoc: Display mode - Octal.
	End Rem
	Const INPUT_OCTAL:Int = 3

	Function _create:TCESpinner(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCESpinner = New TCESpinner
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the spinner value changes. 
	End Rem
	Const EventValueChanged:String = "ValueChanged"
	Rem
	bbdoc: Event fired when the step value changes. 
	End Rem
	Const EventStepChanged:String = "StepChanged"
	Rem
	bbdoc: Event fired when the maximum spinner value changes. 
	End Rem
	Const EventMaximumValueChanged:String = "MaximumValueChanged"
	Rem
	bbdoc: Event fired when the minimum spinner value changes. 
	End Rem
	Const EventMinimumValueChanged:String = "MinimumValueChanged"
	Rem
	bbdoc: Event fired when the input/display mode is changed. 
	End Rem
	Const EventTextInputModeChanged:String = "TextInputModeChanged"

	Rem
	bbdoc: Returns the current spinner value.
	End Rem
	Method getCurrentValue:Float()
		Return bmx_cegui_spinner_getcurrentvalue(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current step value.
	End Rem
	Method getStepSize:Float()
		Return bmx_cegui_spinner_getstepsize(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current maximum limit value for the Spinner.
	End Rem
	Method getMaximumValue:Float()
		Return bmx_cegui_spinner_getmaximumvalue(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current minimum limit value for the Spinner.
	End Rem
	Method getMinimumValue:Float()
		Return bmx_cegui_spinner_getminimumvalue(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current text input / display mode setting.
	End Rem
	Method getTextInputMode:Int()
		Return bmx_cegui_spinner_gettextinputmode(objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the current spinner value.
	End Rem
	Method setCurrentValue(value:Float)
		bmx_cegui_spinner_setcurrentvalue(objectPtr, value)
	End Method
	 
	Rem
	bbdoc: Sets the current step value.
	End Rem
	Method setStepSize(stepSize:Float)
		bmx_cegui_spinner_setstepsize(objectPtr, stepSize)
	End Method
	 
	Rem
	bbdoc: Sets the spinner maximum value.
	End Rem
	Method setMaximumValue(maxValue:Float)
		bmx_cegui_spinner_setmaximumvalue(objectPtr, maxValue)
	End Method
	 
	Rem
	bbdoc: Sets the spinner minimum value.
	End Rem
	Method setMinimumValue(minValue:Float)
		bmx_cegui_spinner_setminimumvalue(objectPtr, minValue)
	End Method
	 
	Rem
	bbdoc: Sets the spinner input / display mode.
	End Rem
	Method setTextInputMode(mode:Int)
		bmx_cegui_spinner_settextinputmode(objectPtr, mode)
	End Method

	
End Type


Rem
bbdoc: 
End Rem
Type TCEItemListBase Extends TCEWindow

	Function _create:TCEItemListBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemListBase = New TCEItemListBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEMenuBase Extends TCEItemListBase

	Function _create:TCEMenuBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMenuBase = New TCEMenuBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEMenubar Extends TCEMenuBase

	Function _create:TCEMenubar(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMenubar = New TCEMenubar
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEPopupMenu Extends TCEMenuBase

	Function _create:TCEPopupMenu(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEPopupMenu = New TCEPopupMenu
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEItemEntry Extends TCEWindow

	Function _create:TCEItemEntry(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemEntry = New TCEItemEntry
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method getItemPixelSize(w:Float Var, h:Float Var)
	End Method

	Method getOwnerList:TCEItemListBase()
	End Method
	
	Method isSelected:Int()
	End Method
	
	Method isSelectable:Int()
	End Method
	
	Method setSelected(setting:Int)
	End Method
	
	Method selectEntry()
	End Method
	
	Method deselect()
	End Method
	
	Method setSelectable(setting:Int)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCEMenuItem Extends TCEItemEntry

	Function _create:TCEMenuItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMenuItem = New TCEMenuItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method isHovering:Int()
	End Method
	
	Method isPushed:Int()
	End Method
	
	Method isOpened:Int()
	End Method
	
	Method getPopupMenu:TCEPopupMenu()
	End Method
	
	Method setPopupMenu(popup:TCEPopupMenu)
	End Method
	
	Method openPopupMenu(Notify:Int = True)
	End Method
	
	Method closePopupMenu(Notify:Int = True)
	End Method
	
	Method togglePopupMenu:Int()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCEScrollablePane Extends TCEWindow

	Function _create:TCEScrollablePane(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrollablePane = New TCEScrollablePane
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Const EventContentPaneChanged:String = "ContentPaneChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventAutoSizeSettingChanged:String = "AutoSizeSettingChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventContentPaneScrolled:String = "ContentPaneScrolled"

	Method getContentPane:TCEScrolledContainer()
	End Method
	
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	
	Method setShowVertScrollbar(setting:Int)
	End Method
	
	Method isHorzScrollbarAlwaysShown:Int()
	End Method
	
	Method setShowHorzScrollbar(setting:Int)
	End Method
	
	Method isContentPaneAutoSized:Int()
	End Method
	
	Method setContentPaneAutoSized(setting:Int)
	End Method
	
	Method getContentPaneArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method setContentPaneArea(x:Float, y:Float, w:Float, h:Float)
	End Method
	
	Method getHorizontalStepSize:Float()
	End Method
	
	Method setHorizontalStepSize(stepSize:Float)
	End Method
	
	Method getHorizontalOverlapSize:Float()
	End Method
	
	Method setHorizontalOverlapSize(overlap:Float)
	End Method
	
	Method getHorizontalScrollPosition:Float()
	End Method
	
	Method setHorizontalScrollPosition(position:Float)
	End Method
	
	Method getVerticalStepSize:Float()
	End Method
	
	Method setVerticalStepSize(stepSize:Float)
	End Method
	
	Method getVerticalOverlapSize:Float()
	End Method
	
	Method setVerticalOverlapSize(overlap:Float)
	End Method
	
	Method getVerticalScrollPosition:Float()
	End Method
	
	Method setVerticalScrollPosition(position:Float)
	End Method
	
	Method getViewableArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method getVertScrollbar:TCEScrollbar()
	End Method
	
	Method getHorzScrollbar:TCEScrollbar() 
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

End Type

Rem
bbdoc: Helper container window that has configurable clipping.
about: Used by the TCEItemListbox widget. 
End Rem
Type TCEClippedContainer Extends TCEWindow

	Function _create:TCEClippedContainer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEClippedContainer = New TCEClippedContainer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Helper container window type which is used in the implementation of the TCEScrollablePane widget. 
End Rem
Type TCEScrolledContainer Extends TCEWindow

	Function _create:TCEScrolledContainer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrolledContainer = New TCEScrolledContainer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for list header segment window. 
End Rem
Type TCEListHeaderSegment Extends TCEWindow

	Function _create:TCEListHeaderSegment(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListHeaderSegment = New TCEListHeaderSegment
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: Base type for the multi column list header widget. 
End Rem
Type TCEListHeader Extends TCEWindow

	Function _create:TCEListHeader(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListHeader = New TCEListHeader
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Const EventSortColumnChanged:String = "SortColumnChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSortDirectionChanged:String = "SortDirectionChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentSized:String = "SegmentSized"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentClicked:String = "SegmentClicked"
	Rem
	bbdoc: 
	End Rem
	Const EventSplitterDoubleClicked:String = "SplitterDoubleClicked"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentSequenceChanged:String = "SegmentSequenceChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentAdded:String = "SegmentAdded"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentRemoved:String = "SegmentRemoved"
	Rem
	bbdoc: 
	End Rem
	Const EventSortSettingChanged:String = "SortSettingChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventDragMoveSettingChanged:String = "DragMoveSettingChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventDragSizeSettingChanged:String = "DragSizeSettingChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSegmentRenderOffsetChanged:String = "SegmentOffsetChanged"


	Method getColumnCount:Int()
	End Method
	
	Method getSegmentFromColumn:TCEListHeaderSegment(column:Int)
	End Method
	 
	Method getSegmentFromID:TCEListHeaderSegment(id:Int)
	End Method
	 
	Method getSortSegment:TCEListHeaderSegment()
	End Method
	 
	Method getColumnFromSegment:Int(segment:TCEListHeaderSegment)
	End Method
	 
	Method getColumnFromID:Int(id:Int)
	End Method
	 
	Method getSortColumn:Int()
	End Method
	 
	Method getColumnWithText:Int(text:String)
	End Method
	 
	Method getPixelOffsetToSegment:Float(segment:TCEListHeaderSegment)
	End Method
	 
	Method getPixelOffsetToColumn:Float(column:Int)
	End Method
	 
	Method getTotalSegmentsPixelExtent:Float()
	End Method
	 
	Method getColumnWidth:Float(column:Int)
	End Method
	 
	Method getSortDirection:Int()
	End Method
	 
	Method isSortingEnabled:Int()
	End Method
	 
	Method isColumnSizingEnabled:Int()
	End Method
	 
	Method isColumnDraggingEnabled:Int()
	End Method
	 
	Method getSegmentOffset:Float()
	End Method
	 
	Method setSortingEnabled(setting:Int)
	End Method
	 
	Method setSortDirection(direction:Int)
	End Method
	 
	Method setSortSegment(segment:TCEListHeaderSegment)
	End Method
	 
	Method setSortColumn(column:Int)
	End Method
	 
	Method setSortColumnFromID(id:Int)
	End Method
	 
	Method setColumnSizingEnabled(setting:Int)
	End Method
	 
	Method setColumnDraggingEnabled(setting:Int)
	End Method
	 
	Method addColumn(text:String, id:Int, width:Float)
	End Method
	 
	Method insertColumn(text:String, id:Int, width:Float, position:Int)
	End Method
	 
	Method insertColumnAtSegment(text:String, id:Int, width:Float, position:TCEListHeaderSegment)
	End Method
	 
	Method removeColumn(column:Int)
	End Method
	 
	Method removeSegment(segment:TCEListHeaderSegment)
	End Method
	 
	Method moveColumn(column:Int, position:Int)
	End Method
	 
	Method moveColumnAtSegment(column:Int, position:TCEListHeaderSegment)
	End Method
	 
	Method moveSegment(segment:TCEListHeaderSegment, position:Int)
	End Method
	 
	Method moveSegmentAtSegment(segment:TCEListHeaderSegment, position:TCEListHeaderSegment)
	End Method
	 
	Method setSegmentOffset(offset:Float)
	End Method
	 
	Method setColumnWidth(column:Int, width:Float)
	End Method
		
End Type

Rem
bbdoc: Base scroll bar type.
about: This base type for scroll bars does not have any idea of direction - a derived type would add whatever meaning
is appropriate according to what that derived type represents to the user.
End Rem
Type TCEScrollbar Extends TCEWindow

	Function _create:TCEScrollbar(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrollbar = New TCEScrollbar
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the scroll bar position value changes. 
	End Rem
	Const EventScrollPositionChanged:String = "ScrollPosChanged"
	Rem
	bbdoc: Event fired when the user begins dragging the thumb. 
	End Rem
	Const EventThumbTrackStarted:String = "ThumbTrackStarted"
	Rem
	bbdoc: Event fired when the user releases the thumb. 
	End Rem
	Const EventThumbTrackEnded:String = "ThumbTrackEnded"
	Rem
	bbdoc: Event fired when the scroll bar configuration data changes. 
	End Rem
	Const EventScrollConfigChanged:String = "ScrollConfigChanged"

	Rem
	bbdoc: Returns the size of the document or data.
	about: The document size should be thought of as the total size of the data that is being scrolled
	through (the number of lines in a text file for example).
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method getDocumentSize:Float()
		Return bmx_cegui_scrollbar_getdocumentsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the page size for this scroll bar.
	about: The page size is typically the amount of data that can be displayed at one time. This value is also used
	when calculating the amount the position will change when you click either side of the scroll bar thumb - the
	amount the position changes will is (pageSize - overlapSize).
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method getPageSize:Float()
		Return bmx_cegui_scrollbar_getpagesize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the step size for this scroll bar.
	about: The step size is typically a single unit of data that can be displayed, this is the amount the position
	will change when you click either of the arrow buttons on the scroll bar. (this could be 1 for a single line
	of text, for example).
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method getStepSize:Float()
		Return bmx_cegui_scrollbar_getstepsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the overlap size for this scroll bar.
	about: The overlap size is the amount of data from the end of a 'page' that will remain visible when the position
	is moved by a page. This is usually used so that the user keeps some context of where they were within the
	document's data when jumping a page at a time.
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method getOverlapSize:Float()
		Return bmx_cegui_scrollbar_getoverlapsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current position of scroll bar within the document.
	about: The range of the scroll bar is from 0 to the size of the document minus the size of a page
	(0 <= position <= (documentSize - pageSize)).
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method getScrollPosition:Float()
		Return bmx_cegui_scrollbar_getscrollposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the 'increase' PushButtoncomponent widget for this Scrollbar.
	End Rem
	Method getIncreaseButton:TCEPushButton()
		Return TCEPushButton(bmx_cegui_scrollbar_getincreasebutton(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the 'decrease' PushButton component widget for this Scrollbar.
	End Rem
	Method getDecreaseButton:TCEPushButton()
		Return TCEPushButton(bmx_cegui_scrollbar_getdecreasebutton(objectPtr))
	End Method
	
	'Thumb * 	getThumb ()
	
	Rem
	bbdoc: Sets the size of the document or data.
	about: The document size should be thought of as the total size of the data that is being scrolled
	through (the number of lines in a text file for example).
	<p>
	The value set has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method setDocumentSize(documentSize:Float)
		bmx_cegui_scrollbar_setdocumentsize(objectPtr, documentSize)
	End Method
	
	Rem
	bbdoc: Sets the page size for this scroll bar.
	about: The page size is typically the amount of data that can be displayed at one time. This value is also
	used when calculating the amount the position will change when you click either side of the scroll bar thumb
	- the amount the position changes will is (pageSize - overlapSize).
	<p>
	The value set has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method setPageSize(pageSize:Float)
		bmx_cegui_scrollbar_setpagesize(objectPtr, pageSize)
	End Method
	
	Rem
	bbdoc: Sets the step size for this scroll bar.
	about: The step size is typically a single unit of data that can be displayed, this is the amount the position
	will change when you click either of the arrow buttons on the scroll bar. (this could be 1 for a single line
	of text, for example).
	<p>
	The value set has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method setStepSize(stepSize:Float)
		bmx_cegui_scrollbar_setstepsize(objectPtr, stepSize)
	End Method
	
	Rem
	bbdoc: Sets the overlap size for this scroll bar.
	about: The overlap size is the amount of data from the end of a 'page' that will remain visible when the position
	is moved by a page. This is usually used so that the user keeps some context of where they were within the
	document's data when jumping a page at a time.
	<p>
	The value set has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method setOverlapSize(overlapSize:Float)
		bmx_cegui_scrollbar_setoverlapsize(objectPtr, overlapSize)
	End Method
	
	Rem
	bbdoc: Sets the current position of scroll bar within the document.
	about: The range of the scroll bar is from 0 to the size of the document minus the size of a page
	(0 <= position <= (documentSize - pageSize)), any attempt to set the position outside this range will be adjusted
	so that it falls within the legal range.
	<p>
	The returned value has no meaning within the Gui system, it is left up to the application to assign appropriate
	values for the application specific use of the scroll bar.
	</p>
	End Rem
	Method setScrollPosition(position:Float)
		bmx_cegui_scrollbar_setscrollposition(objectPtr, position)
	End Method

End Type

Rem
bbdoc: Base type for Slider widgets.
about: The slider widget has a default range of 0.0 - 1.0. This enables use of the slider value to
scale any value needed by way of a simple multiplication. 
End Rem
Type TCESlider Extends TCEWindow

	Function _create:TCESlider(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCESlider = New TCESlider
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the slider value changes. 
	End Rem
	Const EventValueChanged:String = "ValueChanged"
	Rem
	bbdoc: Event fired when the user begins dragging the thumb. 
	End Rem
	Const EventThumbTrackStarted:String = "ThumbTrackStarted"
	Rem
	bbdoc: Event fired when the user releases the thumb.
	End Rem
	Const EventThumbTrackEnded:String = "ThumbTrackEnded"

	Rem
	bbdoc: Returns the current slider value.
	End Rem
	Method getCurrentValue:Float()
		Return bmx_cegui_slider_getcurrentvalue(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the maximum value set for this widget.
	End Rem
	Method getMaxValue:Float()
		Return bmx_cegui_slider_getmaxvalue(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current click step setting for the slider.
	about: The click step size is the amount the slider value will be adjusted when the widget is clicked
	either side of the slider thumb.
	End Rem
	Method getClickStep:Float()
		Return bmx_cegui_slider_getclickstep(objectPtr)
	End Method
	
	'Thumb * 	getThumb()
	
	Rem
	bbdoc: Sets the maximum value for the slider.
	about: Note that the minimum value is fixed at 0.
	End Rem
	Method setMaxValue(maxVal:Float)
		bmx_cegui_slider_setmaxvalue(objectPtr, maxVal)
	End Method
	
	Rem
	bbdoc: Sets the current slider value.
	End Rem
	Method setCurrentValue(value:Float)
		bmx_cegui_slider_setcurrentvalue(objectPtr, value)
	End Method
	
	Rem
	bbdoc: Sets the current click step setting for the slider.
	about: The click step size is the amount the slider value will be adjusted when the widget is clicked
	either side of the slider thumb.
	End Rem
	Method setClickStep(clickStep:Float)
		bmx_cegui_slider_setclickstep(objectPtr, clickStep)
	End Method

End Type

Rem
bbdoc: Base type for standard Tab Control widget. 
End Rem
Type TCETabControl Extends TCEWindow

	Function _create:TCETabControl(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETabControl = New TCETabControl
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event triggered when there is a change to the currently selected tab. 
	End Rem
	Const EventSelectionChanged:String = "TabSelectionChanged"

	Rem
	bbdoc: Returns number of tabs.
	End Rem
	Method getTabCount:Int()
		Return bmx_cegui_tabcontrol_gettabcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the positioning of the tab pane.
	End Rem
	Method getTabPanePosition:Int()
		Return bmx_cegui_tabcontrol_gettabpaneposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Changes the positioning of the tab button pane.
	End Rem
	Method setTabPanePosition(pos:Int)
		bmx_cegui_tabcontrol_settabpaneposition(objectPtr, pos)
	End Method
	
	Rem
	bbdoc: Sets the selected tab by the name of the root window within it.
	about: Also ensures that the tab is made visible (tab pane is scrolled if required).
	End Rem
	Method setSelectedTab(name:String)
		bmx_cegui_tabcontrol_setselectedtab(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Sets the selected tab by the ID of the root window within it.
	about: Also ensures that the tab is made visible (tab pane is scrolled if required).
	End Rem
	Method setSelectedTabForID(ID:Int)
		bmx_cegui_tabcontrol_setselectedtabforid(objectPtr, ID)
	End Method
	
	Rem
	bbdoc: Sets the selected tab by the index position in the tab control.
	about: Also ensures that the tab is made visible (tab pane is scrolled if required).
	End Rem
	Method setSelectedTabAtIndex(index:Int)
		bmx_cegui_tabcontrol_setselectedtabatindex(objectPtr, index)
	End Method
	
	Rem
	bbdoc: Ensures that the tab by the name of the root window within it is visible.
	End Rem
	Method makeTabVisible(name:String)
		bmx_cegui_tabcontrol_maketabvisible(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Ensures that the tab by the ID of the root window within it is visible.
	End Rem
	Method makeTabVisibleForID(ID:Int)
		bmx_cegui_tabcontrol_maketabvisibleforid(objectPtr, ID)
	End Method
	
	Rem
	bbdoc: Ensure that the tab by the index position in the tab control is visible.
	End Rem
	Method makeTabVisibleAtIndex(index:Int)
		bmx_cegui_tabcontrol_maketabvisibleatindex(objectPtr, index)
	End Method
	
	Rem
	bbdoc: Returns the Window which is the first child of the tab at index position index.
	End Rem
	Method getTabContentsAtIndex:TCEWindow(index:Int)
		Return TCEWindow(bmx_cegui_tabcontrol_gettabcontentsatindex(objectPtr, index))
	End Method
	
	Rem
	bbdoc: Returns the Window which is the tab content with the given name.
	End Rem
	Method getTabContents:TCEWindow(name:String)
		Return TCEWindow(bmx_cegui_tabcontrol_gettabcontents(objectPtr, _convertMaxToUTF8(name)))
	End Method
	
	Rem
	bbdoc: Returns the Window which is the tab content with the given ID.
	End Rem
	Method getTabContentsForID:TCEWindow(ID:Int)
		Return TCEWindow(bmx_cegui_tabcontrol_gettabcontentsforid(objectPtr, ID))
	End Method
	
	Rem
	bbdoc: Returns whether the tab contents window is currently selected. 
	End Rem
	Method isTabContentsSelected:Int(wnd:TCEWindow)
		Return bmx_cegui_tabcontrol_istabcontentsselected(objectPtr, wnd.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the index of the currently selected tab. 
	End Rem
	Method getSelectedTabIndex:Int()
		Return bmx_cegui_tabcontrol_getselectedtabindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the height of the tabs. 
	End Rem
	Method getTabHeight:Float()
		Return bmx_cegui_tabcontrol_gettabheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the amount of padding to add either side of the text in the tab. 
	End Rem
	Method getTabTextPadding:Float()
		Return bmx_cegui_tabcontrol_gettabtextpadding(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the height of the tabs. 
	End Rem
	Method setTabHeight(height:Float)
		bmx_cegui_tabcontrol_settabheight(objectPtr, height)
	End Method
	
	Rem
	bbdoc: Sets the amount of padding to add either side of the text in the tab. 
	End Rem
	Method setTabTextPadding(padding:Float)
		bmx_cegui_tabcontrol_settabtextpadding(objectPtr, padding)
	End Method
	
	Rem
	bbdoc: Adds a new tab to the tab control.
	about: The new tab will be added with the same text as the window passed in.
	End Rem
	Method addTab(wnd:TCEWindow)
		bmx_cegui_tabcontrol_addtab(objectPtr, wnd.objectPtr)
	End Method
	
	Rem
	bbdoc: Removes the named tab from the tab control.
	about: The tab content will be destroyed.
	End Rem
	Method removeTab(name:String)
		bmx_cegui_tabcontrol_removetab(objectPtr, _convertMaxToUTF8(name))
	End Method
	
	Rem
	bbdoc: Removes the tab with the given ID from the tab control.
	about: The tab content will be destroyed.
	End Rem
	Method removeTabForID(ID:Int)
		bmx_cegui_tabcontrol_removetabforid(objectPtr, ID)
	End Method

End Type

Rem
bbdoc: 
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
Type TCETree Extends TCEWindow

	Function _create:TCETree(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETree = New TCETree
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Const EventListContentsChanged:String = "ListItemsChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSelectionChanged:String = "ItemSelectionChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSortModeChanged:String = "SortModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMultiselectModeChanged:String = "MuliselectModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventBranchOpened:String = "BranchOpened"
	Rem
	bbdoc: 
	End Rem
	Const EventBranchClosed:String = "BranchClosed"


	Method getItemCount:Int()
	End Method
	
	Method getSelectedCount:Int()
	End Method
	
	Method getFirstSelectedItem:TCETreeItem()
	End Method
	
	Method getLastSelectedItem:TCETreeItem()
	End Method
	
	Method getNextSelected:TCETreeItem(startItem:TCETreeItem)
	End Method
	
	'Method getNextSelectedItemFromList:TCETreeItem(Const LBItemList &itemList, start_item:TCETreeItem, bool foundStartItem:Int)
	'End Method
	
	Method isSortEnabled:Int()
	End Method
	
	Method setItemRenderArea(x:Float, y:Float, w:Float, h:Float)
	End Method
	
	Method getVertScrollbar:TCEScrollbar()
	End Method
	
	Method getHorzScrollbar:TCEScrollbar()
	End Method
	
	Method isMultiselectEnabled:Int()
	End Method
	
	Method isItemTooltipsEnabled:Int()
	End Method
	
	Method findFirstItemWithText:TCETreeItem(text:String)
	End Method
	
	Method findNextItemWithText:TCETreeItem(text:String, startItem:TCETreeItem)
	End Method
	
	'Method findItemWithTextFromList:TCETreeItem(Const LBItemList &itemList, String &text:String, start_item:TCETreeItem, foundStartItem:Int)
	'End Method
	
	Method findFirstItemWithID:TCETreeItem(searchID:Int)
	End Method
	
	Method findNextItemWithID:TCETreeItem(searchID:Int, startItem:TCETreeItem)
	End Method
	
	'Method findItemWithIDFromList:TCETreeItem(Const LBItemList &itemList, uint searchID:Int, start_item:TCETreeItem, foundStartItem:Int)
	'End Method
	
	Method isTreeItemInList:Int(item:TCETreeItem)
	End Method
	
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	
	Method isHorzScrollbarAlwaysShown:Int()
	End Method
	
	Method initialise()
	End Method
	
	Method resetList()
	End Method
	
	Method addItem(item:TCETreeItem)
	End Method
	
	Method insertItem(item:TCETreeItem, position:TCETreeItem)
	End Method
	
	Method removeItem(item:TCETreeItem)
	End Method
	
	Method clearAllSelections()
	End Method
	
	'Method clearAllSelectionsFromList:Int(Const LBItemList &itemList)
	'End Method
	
	Method setSortingEnabled(setting:Int)
	End Method
	
	Method setMultiselectEnabled(setting:Int)
	End Method
	
	Method setShowVertScrollbar(setting:Int)
	End Method
	
	Method setShowHorzScrollbar(setting:Int)
	End Method
	
	Method setItemTooltipsEnabled(setting:Int)
	End Method
	
	Method setItemSelectState(item:TCETreeItem, state:Int)
	End Method
	
	Method setItemSelectStateIndex(itemIndex:Int, state:Int)
	End Method
	
	Method setLookNFeel(look:String)
	End Method
	
	Method handleUpdatedItemData()
	End Method
	
	Method ensureItemIsVisible(item:TCETreeItem)
	End Method
	

End Type

Rem
bbdoc: 
End Rem
Type TCETreeItem

	Field objectPtr:Byte Ptr

	Function _create:TCETreeItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETreeItem = New TCETreeItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method getFont:TCEFont()
	End Method
	
	Method getTextColours:TCEColourRect()
	End Method
	
	Method setFont(font:Object)
		If TCEFont(font) Then
		
		ElseIf String(font) Then
		
		End If
	End Method
	
	Method setTextColours(cols:TCEColourRect)
	End Method
	
	'Method setTextColours(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	'End Method
	
	'Method setTextColours(col:TCEColour)
	'End Method
	
	Method getText:String()
	End Method
	
	Method getTooltipText:String()
	End Method
	
	Method getID:Int()
	End Method
	
	Method getUserData:Object()
	End Method
	
	Method isSelected:Int()
	End Method
	
	Method isDisabled:Int()
	End Method
	
	Method isAutoDeleted:Int()
	End Method
	
	Method getOwnerWindow:TCEWindow()
	End Method
	
	Method getSelectionColours:TCEColourRect()
	End Method
	
	Method getSelectionBrushImage:TCEImage()
	End Method
	
	Method setText(text:String)
	End Method
	
	Method setTooltipText(text:String)
	End Method
	
	Method setID(itemId:Int)
	End Method
	
	Method setUserData(itemData:Object)
	End Method
	
	Method setSelected(setting:Int)
	End Method
	
	Method setDisabled(setting:Int)
	End Method
	
	Method setAutoDeleted(setting:Int)
	End Method
	
	Method setOwnerWindow(owner:TCEWindow)
	End Method
	
	Method setSelectionColours(cols:TCEColourRect)
	End Method
	
	'Method setSelectionColours(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	'End Method
	
	'Method setSelectionColours(col:TCEColour)
	'End Method
	
	Method setSelectionBrushImage(image:TCEImage)
	End Method
	
	Method setSelectionBrushImageByName(imageset:String, image:String)
	End Method
	
	Method setButtonLocation(x:Float, y:Float, w:Float, h:Float)
	End Method
	
	Method getButtonLocation(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method getIsOpen:Int()
	End Method
	
	Method toggleIsOpen()
	End Method
	
	Method getTreeItemFromIndex:TCETreeItem(itemIndex:Int)
	End Method
	
	Method getItemCount:Int()
	End Method
	
	'LBItemList & 	getItemList()
	
	Method addItem(item:TCETreeItem)
	End Method
	
	Method setIcon(theIcon:TCEImage)
	End Method
	
	Method getPixelSize(width:Float Var, height:Float Var) 
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCEMultiColumnList Extends TCEWindow

	Function _create:TCEMultiColumnList(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMultiColumnList = New TCEMultiColumnList
			this.objectPtr = objectPtr
			Return this
		End If
	End Function


	Rem
	bbdoc: 
	End Rem
	Const EventSelectionModeChanged:String = "SelectModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventNominatedSelectColumnChanged:String = "NomSelColChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventNominatedSelectRowChanged:String = "NomSelRowChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertBarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzBarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSelectionChanged:String = "SelectionChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventListContentsChanged:String = "ContentsChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSortColumnChanged:String = "SortColChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventSortDirectionChanged:String = "SortDirChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventListColumnSized:String = "ColSized"
	Rem
	bbdoc: 
	End Rem
	Const EventListColumnMoved:String = "ColMoved"


End Type

Rem
bbdoc: 
End Rem
Type TCEMultiLineEditbox Extends TCEWindow

	Function _create:TCEMultiLineEditbox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMultiLineEditbox = New TCEMultiLineEditbox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function


	Rem
	bbdoc: The read-only mode for the edit box has been changed. 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: The word wrap mode of the text box has been changed. 
	End Rem
	Const EventWordWrapModeChanged:String = "WordWrapModeChanged"
	Rem
	bbdoc: The maximum allowable string length has been changed. 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaximumTextLengthChanged"
	Rem
	bbdoc: The text carat (insert point) has changed. 
	End Rem
	Const EventCaratMoved:String = "CaratMoved"
	Rem
	bbdoc: The current text selection has changed.
	End Rem
	Const EventTextSelectionChanged:String = "TextSelectionChanged"
	Rem
	bbdoc: The number of characters in the edit box has reached the current maximum. 
	End Rem
	Const EventEditboxFull:String = "EditboxFullEvent"
	Rem
	bbdoc: Event triggered when the vertical scroll bar 'force' setting changes.
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: Event triggered when the horizontal scroll bar 'force' setting changes. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"

	Rem
	bbdoc: Returns true if the edit box has input focus.
	End Rem
	Method hasInputFocus:Int()
		Return bmx_cegui_multilineeditbox_hasinputfocus(objectPtr)
	End Method

	Rem
	bbdoc: Returns true if the edit box is read-only.
	End Rem
	Method isReadOnly:Int()
		Return bmx_cegui_multilineeditbox_isreadonly(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current position of the carat.
	End Rem
	Method getCaratIndex:Int()
		Return bmx_cegui_multilineeditbox_getcaratindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current selection start point.
	returns: ndex of the selection start point relative to the start of the text. If no selection is defined this function returns the position of the carat.
	End Rem
	Method getSelectionStartIndex:Int()
		Return bmx_cegui_multilineeditbox_getselectionstartindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current selection end point.
	returns: Index of the selection end point relative to the start of the text. If no selection is defined this function returns the position of the carat.
	End Rem
	Method getSelectionEndIndex:Int()
		Return bmx_cegui_multilineeditbox_getselectionendindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the length of the current selection (in code points / characters).
	End Rem
	Method getSelectionLength:Int()
		Return bmx_cegui_multilineeditbox_getselectionlength(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the maximum text length set for this edit box.
	End Rem
	Method getMaxTextLength:Int()
		Return bmx_cegui_multilineeditbox_getmaxtextlength(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the text in the edit box will be word-wrapped.
	End Rem
	Method isWordWrapped:Int()
		Return bmx_cegui_multilineeditbox_iswordwrapped(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the vertical scrollbar component widget for this MultiLineEditbox.
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_multilineeditbox_getvertscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown.
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_multilineeditbox_isvertscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the horizontal scrollbar component widget for this MultiLineEditbox.
	End Rem
	Method getHorzScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_multilineeditbox_gethorzscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns an area describing, in un-clipped pixels, the window relative area that the text should be rendered in to.
	End Rem
	Method getTextRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_multilineeditbox_gettextrenderarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	'Const LineList & 	getFormattedLines()
	
	Rem
	bbdoc: Returns the line number for the given character index.
	End Rem
	Method getLineNumberFromIndex:Int(index:Int)
		Return bmx_cegui_multilineeditbox_getlinenumberfromindex(objectPtr, index)
	End Method
	
	Rem
	bbdoc: Specifies whether the edit box is read-only.
	End Rem
	Method setReadOnly(setting:Int)
		bmx_cegui_multilineeditbox_setreadonly(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the current position of the carat.
	End Rem
	Method setCaratIndex(caratPos:Int)
		bmx_cegui_multilineeditbox_setcaratindex(objectPtr, caratPos)
	End Method
	
	Rem
	bbdoc: Defines the current selection for the edit box.
	End Rem
	Method setSelection(startPos:Int, endPos:Int)
		bmx_cegui_multilineeditbox_setselection(objectPtr, startPos, endPos)
	End Method
	
	Rem
	bbdoc: Sets the maximum text length for this edit box.
	End Rem
	Method setMaxTextLength(maxLen:Int)
		bmx_cegui_multilineeditbox_setmaxtextlength(objectPtr, maxLen)
	End Method
	
	Rem
	bbdoc: Scrolls the editbox so that the carat is visible.
	End Rem
	Method ensureCaratIsVisible()
		bmx_cegui_multilineeditbox_ensurecaratisvisible(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the text will be word wrapped or not.
	End Rem
	Method setWordWrapping(setting:Int)
		bmx_cegui_multilineeditbox_setwordwrapping(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown.
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_multilineeditbox_setshowvertscrollbar(objectPtr, setting)
	End Method

End Type

Rem
bbdoc: Base type for an Editbox widget.
End Rem
Type TCEEditbox Extends TCEWindow

	Function _create:TCEEditbox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEEditbox = New TCEEditbox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function


	Rem
	bbdoc: The read-only mode for the edit box has been changed. 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: The masked rendering mode (password mode) has been changed. 
	End Rem
	Const EventMaskedRenderingModeChanged:String = "MaskRenderChanged"
	Rem
	bbdoc: The code point (character) to use for masked text has been changed. 
	End Rem
	Const EventMaskCodePointChanged:String = "MaskCPChanged"
	Rem
	bbdoc: The validation string has been changed. 
	End Rem
	Const EventValidationStringChanged:String = "ValidatorChanged"
	Rem
	bbdoc: The maximum allowable string length has been changed. 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaxTextLenChanged"
	Rem
	bbdoc: Some operation has made the current text invalid with regards to the validation string. 
	End Rem
	Const EventTextInvalidated:String = "TextInvalidated"
	Rem
	bbdoc: The user attempted to modify the text in a way that would have made it invalid. 
	End Rem
	Const EventInvalidEntryAttempted:String = "InvalidInputAttempt"
	Rem
	bbdoc: The text carat (insert point) has changed. 
	End Rem
	Const EventCaratMoved:String = "TextCaratMoved"
	Rem
	bbdoc: The current text selection has changed. 
	End Rem
	Const EventTextSelectionChanged:String = "TextSelectChanged"
	Rem
	bbdoc: The number of characters in the edit box has reached the current maximum. 
	End Rem
	Const EventEditboxFull:String = "EditboxFull"
	Rem
	bbdoc: The user has accepted the current text by pressing Return, Enter, or Tab. 
	End Rem
	Const EventTextAccepted:String = "TextAccepted"


	Rem
	bbdoc: Returns true if the Editbox has input focus.
	End Rem
	Method hasInputFocus:Int()
		Return bmx_cegui_editbox_hasinputfocus(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the Editbox is read-only.
	End Rem
	Method isReadOnly:Int()
		Return bmx_cegui_editbox_isreadonly(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the text for the Editbox will be rendered masked.
	End Rem
	Method isTextMasked:Int()
		Return bmx_cegui_editbox_istextmasked(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the Editbox text is valid given the currently set validation string.
	about: It is possible to programmatically set 'invalid' text for the Editbox by calling setText. This has certain
	implications since if invalid text is set, whatever the user types into the box will be rejected when the input is validated.
	<p>
	Validation is performed by means of a regular expression. If the text matches the regex, the text is said to have passed
	validation. If the text does not match with the regex then the text fails validation.
	</p>
	End Rem
	Method isTextValid:Int()
		Return bmx_cegui_editbox_istextvalid(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the currently set validation string
	End Rem
	Method getValidationString:String()
		Return _convertUTF8ToMax(bmx_cegui_editbox_getvalidationstring(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current position of the carat.
	returns: Index of the insert carat relative to the start of the text.
	End Rem
	Method getCaratIndex:Int()
		Return bmx_cegui_editbox_getcaratindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current selection start point.
	returns: Index of the selection start point relative to the start of the text. If no selection is defined this function returns the position of the carat.
	End Rem
	Method getSelectionStartIndex:Int()
		Return bmx_cegui_editbox_getselectionstartindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current selection end point.
	returns: Index of the selection end point relative to the start of the text. If no selection is defined this function returns the position of the carat.
	End Rem
	Method getSelectionEndIndex:Int()
		Return bmx_cegui_editbox_getselectionendindex(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the length of the current selection (in code points / characters).
	End Rem
	Method getSelectionLength:Int()
		Return bmx_cegui_editbox_getselectionlength(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the maximum text length set for this Editbox.
	End Rem
	Method getMaxTextLength:Int()
		Return bmx_cegui_editbox_getmaxtextlength(objectPtr)
	End Method
	
	Rem
	bbdoc: Specifies whether the Editbox is read-only.
	End Rem
	Method setReadOnly(setting:Int)
		bmx_cegui_editbox_setreadonly(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Specifies whether the text for the Editbox will be rendered masked.
	End Rem
	Method setTextMasked(setting:Int)
		bmx_cegui_editbox_settextmasked(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the text validation string.
	about: Validation is performed by means of a regular expression. If the text matches the regex, the text is said to have
	passed validation. If the text does not match with the regex then the text fails validation.
	End Rem
	Method setValidationString(validationString:String)
		bmx_cegui_editbox_setvalidationstring(objectPtr, _convertMaxToUTF8(validationString))
	End Method
	
	Rem
	bbdoc: Sets the current position of the carat.
	End Rem
	Method setCaratIndex(caratPos:Int)
		bmx_cegui_editbox_setcaratindex(objectPtr, caratPos)
	End Method
	
	Rem
	bbdoc: Defines the current selection for the Editbox.
	End Rem
	Method setSelection(startPos:Int, endPos:Int)
		bmx_cegui_editbox_setselection(objectPtr, startPos, endPos)
	End Method
	
	Rem
	bbdoc: Sets the maximum text length for this Editbox.
	about: Depending on the validation string set, the actual length of text that can be entered may be less than the
	value set here (it will never be more).
	End Rem
	Method setMaxTextLength(maxLen:Int)
		bmx_cegui_editbox_setmaxtextlength(objectPtr, maxLen)
	End Method

End Type

Rem
bbdoc: ScrolledItemListBase window type. 
End Rem
Type TCEScrolledItemListBase Extends TCEItemListBase

	Function _create:TCEScrolledItemListBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrolledItemListBase = New TCEScrolledItemListBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the vertical scroll bar mode changes. 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: Event fired when the horizontal scroll bar mode change. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"

	Rem
	bbdoc: Returns whether the vertical scrollbar is being forced visible. Despite content size. 
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_scrolleditemlistbase_isvertscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the horizontal scrollbar is being forced visible. Despite content size. 
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_scrolleditemlistbase_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Get the vertical scrollbar component attached to this window. 
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_scrolleditemlistbase_getvertscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Get the horizontal scrollbar component attached to this window. 
	End Rem
	Method getHorzScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_scrolleditemlistbase_gethorzscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets whether the vertical scrollbar should be forced visible. Despite content size. 
	End Rem
	Method setShowVertScrollbar(mode:Int)
		bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(objectPtr, mode)
	End Method
	
	Rem
	bbdoc: Sets whether the horizontal scrollbar should be forced visible. Despite content size.
	End Rem
	Method setShowHorzScrollbar(mode:Int)
		bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(objectPtr, mode)
	End Method

End Type

Rem
bbdoc: ItemListbox window type. 
End Rem
Type TCEItemListBox Extends TCEScrolledItemListBase

	Function _create:TCEItemListBox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemListBox = New TCEItemListBox
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event fired when the selection changes. 
	End Rem
	Const EventSelectionChanged:String = "SelectionChanged"
	Rem
	bbdoc: Event fired when the multiselect mode changes. 
	End Rem
	Const EventMultiSelectModeChanged:String = "MultiSelectModeChanged"

	Rem
	bbdoc: 
	End Rem
	Method getSelectedCount:Int()
		Return bmx_cegui_itemlistbox_getselectedcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the last selected item.
	End Rem
	Method getLastSelectedItem:TCEItemEntry()
		Return TCEItemEntry(bmx_cegui_itemlistbox_getlastselecteditem(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the first selected item.
	about: If multiselect is disabled then this does the equivalent of calling getLastSelectedItem. If multiselect
	is enabled it will search the array starting at start_index.
	End Rem
	Method getFirstSelectedItem:TCEItemEntry(startIndex:Int = 0)
		Return TCEItemEntry(bmx_cegui_itemlistbox_getfirstselecteditem(objectPtr, startIndex))
	End Method
	
	Rem
	bbdoc: Returns the next seleced item relative to a previous call to getFirstSelectedItem or getNextSelectedItem.
	End Rem
	Method getNextSelectedItem:TCEItemEntry()
		Return TCEItemEntry(bmx_cegui_itemlistbox_getnextselecteditem(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the next selected item after the item @startItem given.
	End Rem
	Method getNextSelectedItemAfter:TCEItemEntry(startItem:TCEItemEntry)
		Return TCEItemEntry(bmx_cegui_itemlistbox_getnextselecteditemafter(objectPtr, startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns True if multiple selections are allowed, False if not.
	End Rem
	Method isMultiSelectEnabled:Int()
		Return bmx_cegui_itemlistbox_ismultiselectenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the item at the given index is selectable and currently selected. 
	End Rem
	Method isItemSelected:Int(index:Int)
		Return bmx_cegui_itemlistbox_isitemselected(objectPtr, index)
	End Method
	
	Rem
	bbdoc: Sets whether or not multiple selections should be allowed. 
	End Rem
	Method setMultiSelectEnabled(state:Int)
		bmx_cegui_itemlistbox_setmultiselectenabled(objectPtr, state)
	End Method
	
	Rem
	bbdoc: Clears all selections. 
	End Rem
	Method clearAllSelections()
		bmx_cegui_itemlistbox_clearallselections(objectPtr)
	End Method
	
	Rem
	bbdoc: Selects a range of items.
	End Rem
	Method selectRange(a:Int, z:Int)
		bmx_cegui_itemlistbox_selectrange(objectPtr, a, z)
	End Method
	
	Rem
	bbdoc: Selects all items.
	about: Does nothing if multiselect is disabled. 
	End Rem
	Method selectAllItems()
		bmx_cegui_itemlistbox_selectallitems(objectPtr)
	End Method

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
bbdoc: 
End Rem
Type TCEImagesetManager

	Function createImageset:TCEImageset(filename:String, resourceGroup:String = "")
	End Function
	
	Function createImagesetFromTexture:TCEImageset(name:String, texture:TCETexture)
	End Function
	
	Function createImagesetFromImageFile:TCEImageset(name:String, filename:String, resourceGroup:String = "")
		Return TCEImageset._create(bmx_cegui_imagesetmanager_createimagesetfromimagefile(_convertMaxToUTF8(name), _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Function
	
	Function destroyImageSet(imageset:Object)
		If TCEImageset(imageset) Then
		
		ElseIf String(imageset) Then
		
		End If
	End Function
	
	Function destroyAllImagesets()
	End Function
	
	Function getImageset:TCEImageset(name:String)
	End Function
	
	Function isImagesetPresent:Int(name:String)
	End Function
	
	Function notifyScreenResolution(width:Float, height:Float)
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCETexture

	Field objectPtr:Byte Ptr
	
	Method getWidth:Int()
	End Method
	
	Method getOriginalWidth:Int()
	End Method
	
	Method getXScale:Float()
	End Method
	
	Method getHeight:Int()
	End Method
	
	Method getOriginalHeight:Int()
	End Method
	
	Method getYScale:Float()
	End Method
	
	Method loadFromFile(filename:String, resourceGroup:String)
	End Method
	
	Method loadFromMemory(buffer:Byte Ptr, width:Int, height:Int, pixelFormat:Int)
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


Type TCERenderer

	Field objectPtr:Byte Ptr
	
	Method CreateTexture:TCETexture(filename:String, resourceGroup:String)
	End Method
	
	Method createTextureWithSize:TCETexture(size:Float)
	End Method
	
	Method destroyTexture(texture:TCETexture)
	End Method
	
	Method destroyAllTextures()
	End Method
	
	Method isQueueingEnabled:Int()
	End Method
	
	Method getWidth:Float()
	End Method
	
	Method getHeight:Float()
	End Method
	
	Method getSize(width:Float Var, height:Float Var)
	End Method
	
	Method getMaxTextureSize:Int()
	End Method
	
	Method getHorzScreenDPI:Int()
	End Method
	
	Method getVertScreenDPI:Int()
	End Method
	
	Method resetZValue()
	End Method
	
	Method advanceZValue()
	End Method
	
	Method getCurrentZ:Float()
	End Method
	
	Method getZLayer:Float(layer:Int)
	End Method
	
	Method getIdentifierString:String()
	End Method

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


Type TCEColourRect
End Type

Type TCEColour
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
	
	Function createFontType:TCEFont(fontType:String, name:String, fontName:String, resourceGroup:String = "")
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
	
	

End Type

