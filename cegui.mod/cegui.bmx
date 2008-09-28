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
ModuleInfo "CC_OPTS: -DCEGUI_STATIC -DCEGUI_FALAGARD_RENDERER -DCEGUI_WITH_TINYXML"

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


Global cegui_systemPtr:Byte Ptr
Global cegui_rendererPtr:Byte Ptr
Global cegui_startTime:Int

Global ce_event_handler:TCEEventHandler = New TCEEventHandler

Function cegui_cleanup()

	If cegui_systemPtr Then
		bmx_cegui_delete_system(cegui_systemPtr)
		cegui_systemPtr = Null
	End If
	
	If cegui_rendererPtr Then
		bmx_cegui_delete_renderer(cegui_rendererPtr)
		cegui_rendererPtr = Null
	End If
	
End Function

Rem
bbdoc:
End Rem
Function Init_CEGUI()
	If Not cegui_rendererPtr Then
		cegui_rendererPtr = bmx_cegui_new_oglrenderer()

		If cegui_rendererPtr Then
			cegui_systemPtr = bmx_cegui_new_system(cegui_rendererPtr)
		End If

		cegui_startTime = MilliSecs()
		
		AddHook EmitEventHook,TCEEvent.Keyhook,Null,0
	End If
End Function

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

Type TCEEventHandler

	'Field eventHandlerPtr:Byte Ptr
	
	Field callbacks:TMap = New TMap

	Method New()
		'eventHandlerPtr = bmx_cegui_eventhandler_new()
	End Method
	
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
		
	Method Delete()
		'If eventHandlerPtr Then
		'	bmx_cegui_eventhandler_delete(eventHandlerPtr)
		'	eventHandlerPtr = Null
		'End If
	End Method
	
End Type

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

	Rem
	bbdoc: Render the GUI.
	End Rem
	Function renderGUI()
		bmx_cegui_system_renderGUI()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function setDefaultFont(font:String)
		bmx_cegui_system_setDefaultFont(_convertMaxToUTF8(font))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function setDefaultMouseCursor(a:String, b:String)
		bmx_cegui_system_setDefaultMouseCursor(_convertMaxToUTF8(a), _convertMaxToUTF8(b))
	End Function
	
	Rem
	bbdoc: Set the active GUI sheet (root) window.
	End Rem
	Function setGUISheet(window:TCEWindow)
		bmx_cegui_system_setGUISheet(window.objectPtr)
	End Function
	
	Rem
	bbdoc: Inject time pulses into the system.
	about: Accepts a value indicating the amount of time passed, in seconds, since the last time this method was called.
	End Rem 
	Function injectTimePulse(time:Float)
		bmx_cegui_system_injectTimePulse(time)
	End Function
	
	Function injectMousePosition(x:Int, y:Int)
		bmx_cegui_system_injectMousePosition(x, y)
	End Function
	
	Function injectMouseButtonUp(button:Int)
		bmx_cegui_system_injectMouseButtonUp(button)
	End Function

	Function injectMouseButtonDown(button:Int)
		bmx_cegui_system_injectMouseButtonDown(button)
	End Function
	
	Function injectKeyDown:Int(key:Int)
		Return bmx_cegui_system_injectkeydown(key)
	End Function

	Function injectKeyUp:Int(key:Int)
		Return bmx_cegui_system_injectkeyup(key)
	End Function

	Function injectChar:Int(key:Int)
		Return bmx_cegui_system_injectchar(key)
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

	Rem
	bbdoc: Creates a set of windows (a Gui layout) from the information in the specified XML file.
	End Rem
	Function loadWindowLayout:TCEWindow(filename:String, namePrefix:String = "", resourceGroup:String = "")
		Return TCEWindow(bmx_cegui_windowmanager_loadWindowLayout(_convertMaxToUTF8(filename), ..
			_convertMaxToUTF8(namePrefix), _convertMaxToUTF8(resourceGroup)))
	End Function

	Rem
	bbdoc: Returns a reference to the specified Window object.
	End Rem
	Function getWindow:TCEWindow(name:String)
		Return TCEWindow(bmx_cegui_windowmanager_getwindow(_convertMaxToUTF8(name)))
	End Function
	
	Function isWindowPresent:Int(name:String)
	End Function
	
	Function destroyAllWindows()
	End Function
	
	Function destroyWindow(window:Object)
		If TCEWindow(window) Then
			
		ElseIf String(window) Then
		
		End If
	End Function
	
	Function renameWindow(window:Object, newName:String)
		If TCEWindow(window) Then
			
		ElseIf String(window) Then
		
		End If
	End Function

	Rem
	bbdoc: Creates a new Window object of the specified type, and gives it the specified unique name.
	returns: The newly created Window object.
	End Rem
	Function CreateWindow:TCEWindow(windowType:String, name:String = "", prefix:String = "")
		Return TCEWindow(bmx_cegui_windowmanager_createwindow(_convertMaxToUTF8(windowType), _convertMaxToUTF8(name), _convertMaxToUTF8(prefix)))
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
	
	Method isSizingEnabled:Int()
	End Method
	
	Method isFrameEnabled:Int()
	End Method
	
	Method isTitleBarEnabled:Int()
	End Method
	
	Method isCloseButtonEnabled:Int()
	End Method
	
	Method isRollupEnabled:Int()
	End Method
	
	Method isRolledup:Int()
	End Method
	
	Method getSizingBorderThickness:Float()
	End Method
	
	Method setSizingEnabled(setting:Int)
	End Method
	
	Method setFrameEnabled(setting:Int)
	End Method
	
	Method setTitleBarEnabled(setting:Int)
	End Method
	
	Method setCloseButtonEnabled(setting:Int)
	End Method
	
	Method setRollupEnabled(setting:Int)
	End Method
	
	Method toggleRollup()
	End Method
	
	Method setSizingBorderThickness(pizels:Float)
	End Method
	
	Method offsetPixelPosition(x:Float, y:Float)
	End Method
	
	Method isDragMovingEnabled:Int()
	End Method
	
	Method setDragMovingEnabled(setting:Int)
	End Method
	
	Method isHit:Int(x:Float, y:Float)
	End Method
	
	Method getTitlebar:TCETitlebar()
	End Method
	
	Method getCloseButton:TCEPushButton()
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

	Method isDraggingEnabled:Int()
	End Method
	
	Method setDraggingEnabled(setting:Int)
	End Method

End Type

Type TCEEvent

	Global mx:Int, my:Int
	Global mb1:Int, mb2:Int, mb3:Int
	

	Function mouseEvents(x:Int, y:Int)
	
		Local elapsed:Float = (MilliSecs() - cegui_startTime)/1000.0
		
		If x <> mx Or y <> my Then
		
			TCESystem.injectTimePulse(elapsed)
			TCESystem.injectMousePosition(x, y)
			mx = x
			my = y
		
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

	Method isHovering:Int()
	End Method
	
	Method isPushed:Int()
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
bbdoc: 
End Rem
Type TCERadioButton Extends TCEButtonBase

	Function _create:TCERadioButton(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCERadioButton = New TCERadioButton
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

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
bbdoc: 
End Rem
Type TCEComboDropList Extends TCEListbox

	Function _create:TCEComboDropList(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEComboDropList = New TCEComboDropList
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

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
bbdoc: 
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
bbdoc: 
End Rem
Type TCESpinner Extends TCEWindow

	Function _create:TCESpinner(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCESpinner = New TCESpinner
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

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
bbdoc: 
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
bbdoc: 
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
bbdoc: 
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
bbdoc: 
End Rem
Type TCEListHeader Extends TCEWindow

	Function _create:TCEListHeader(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListHeader = New TCEListHeader
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCEScrollbar Extends TCEWindow

	Function _create:TCEScrollbar(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrollbar = New TCEScrollbar
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCESlider Extends TCEWindow

	Function _create:TCESlider(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCESlider = New TCESlider
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCETabControl Extends TCEWindow

	Function _create:TCETabControl(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETabControl = New TCETabControl
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

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
	bbdoc: 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventWordWrapModeChanged:String = "WordWrapModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaximumTextLengthChanged"
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
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"

	Method hasInputFocus:Int()
	End Method

	Method isReadOnly:Int()
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
	
	Method isWordWrapped:Int()
	End Method
	
	Method getVertScrollbar:TCEScrollbar()
	End Method
	
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	
	Method getHorzScrollbar:TCEScrollbar()
	End Method
	
	Method getTextRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	'Const LineList & 	getFormattedLines()
	
	Method getLineNumberFromIndex:Int(index:Int)
	End Method
	
	Method initialiseComponents()
	End Method
	
	Method setReadOnly(setting:Int)
	End Method
	
	Method setCaratIndex(caratPos:Int)
	End Method
	
	Method setSelection(startPos:Int, endPos:Int)
	End Method
	
	Method setMaxTextLength(maxLen:Int)
	End Method
	
	Method ensureCaratIsVisible()
	End Method
	
	Method setWordWrapping(setting:Int)
	End Method
	
	Method setShowVertScrollbar(setting:Int)
	End Method

End Type

Rem
bbdoc: 
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
	bbdoc: 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMaskedRenderingModeChanged:String = "MaskRenderChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMaskCodePointChanged:String = "MaskCPChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventValidationStringChanged:String = "ValidatorChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaxTextLenChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventTextInvalidated:String = "TextInvalidated"
	Rem
	bbdoc: 
	End Rem
	Const EventInvalidEntryAttempted:String = "InvalidInputAttempt"
	Rem
	bbdoc: 
	End Rem
	Const EventCaratMoved:String = "TextCaratMoved"
	Rem
	bbdoc: 
	End Rem
	Const EventTextSelectionChanged:String = "TextSelectChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventEditboxFull:String = "EditboxFull"
	Rem
	bbdoc: 
	End Rem
	Const EventTextAccepted:String = "TextAccepted"


	Rem
	bbdoc: 
	End Rem
	Method hasInputFocus:Int()
		Return bmx_cegui_editbox_hasinputfocus(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isReadOnly:Int()
		Return bmx_cegui_editbox_isreadonly(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isTextMasked:Int()
		Return bmx_cegui_editbox_istextmasked(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isTextValid:Int()
		Return bmx_cegui_editbox_istextvalid(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getValidationString:String()
		Return _convertUTF8ToMax(bmx_cegui_editbox_getvalidationstring(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getCaratIndex:Int()
		Return bmx_cegui_editbox_getcaratindex(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getSelectionStartIndex:Int()
		Return bmx_cegui_editbox_getselectionstartindex(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getSelectionEndIndex:Int()
		Return bmx_cegui_editbox_getselectionendindex(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getSelectionLength:Int()
		Return bmx_cegui_editbox_getselectionlength(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getMaxTextLength:Int()
		Return bmx_cegui_editbox_getmaxtextlength(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setReadOnly(setting:Int)
		bmx_cegui_editbox_setreadonly(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTextMasked(setting:Int)
		bmx_cegui_editbox_settextmasked(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setValidationString(validationString:String)
		bmx_cegui_editbox_setvalidationstring(objectPtr, _convertMaxToUTF8(validationString))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setCaratIndex(caratPos:Int)
		bmx_cegui_editbox_setcaratindex(objectPtr, caratPos)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSelection(startPos:Int, endPos:Int)
		bmx_cegui_editbox_setselection(objectPtr, startPos, endPos)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMaxTextLength(maxLen:Int)
		bmx_cegui_editbox_setmaxtextlength(objectPtr, maxLen)
	End Method

End Type

Type TCEScrolledItemListBase Extends TCEItemListBase

	Function _create:TCEScrolledItemListBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEScrolledItemListBase = New TCEScrolledItemListBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Type TCEItemListBox Extends TCEScrolledItemListBase

	Function _create:TCEItemListBox(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemListBox = New TCEItemListBox
			this.objectPtr = objectPtr
			Return this
		End If
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

Type TCEImage
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

