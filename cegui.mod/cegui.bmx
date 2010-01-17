' Copyright (c) 2008-2010 Bruce A Henderson
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
ModuleInfo "Copyright: CEGUI - 2004 - 2009 Paul D Turner & The CEGUI Development Team"
ModuleInfo "Copyright: Wrapper - 2008-2010 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (CEGUI 0.7.x - SVN rev 2394)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DPCRE_STATIC"
ModuleInfo "CC_OPTS: -DCEGUI_STATIC -DCEGUI_FALAGARD_RENDERER -DCEGUI_WITH_TINYXML -DCEGUI_CODEC_FREEIMAGE"

?win32
ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB -DPNG_iTXt_SUPPORTED -DHAVE_PROTOTYPES -DOPJ_STATIC"
ModuleInfo "CC_OPTS: -DGLEW_STATIC"
?

Import "base.bmx"

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

'
' CODING NOTES :
'
' Remember that glue should return BBString, and not utf*, because there are issues where the returned utf8* might
' be GC'd before the BBString is created on the BlitzMax side...
'


Rem
bbdoc: EventArgs based type that is used for objects passed to input event handlers concerning mouse cursor events.
End Rem
Type TCEMouseCursorEventArgs Extends TCEEventArgs

	Function _create:TCEMouseCursorEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEMouseCursorEventArgs = New TCEMouseCursorEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the Image object of relevance to the event. 
	End Rem
	Method getImage:TCEImage()
		Return TCEImage(bmx_cegui_mousecursoreventargs_getimage(eventArgsPtr))
	End Method
	
End Type

Rem
bbdoc: EventArgs based type that is used for objects passed to handlers triggered for events concerning some Window object.
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
bbdoc: EventArgs based type that is used for Activated and Deactivated window events.
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
bbdoc: EventArgs based type used for certain drag/drop notifications.
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
	bbdoc: Returns the drag container.
	End Rem
	Method getDragDropItem:TCEDragContainer()
		Return TCEDragContainer(bmx_cegui_dragdropeventargs_getdragdropitem(eventArgsPtr))
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

	Rem
	bbdoc: The original column index of the segment that has moved. 
	End Rem
	Method getOldIndex:Int()
		Return bmx_cegui_headersequenceeventargs_getoldindex(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: The new column index of the segment that has moved. 
	End Rem
	Method getNewIndex:Int()
		Return bmx_cegui_headersequenceeventargs_getnewindex(eventArgsPtr)
	End Method
	
End Type

Rem
bbdoc: EventArgs based type that is used for objects passed to input event handlers concerning keyboard input.
End Rem
Type TCEKeyEventArgs Extends TCEWindowEventArgs

	Function _create:TCEKeyEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEKeyEventArgs = New TCEKeyEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: utf32 codepoint for the key (only used for Character inputs). 
	End Rem
	Method getCodepoint:Int()
		Return bmx_cegui_keyeventargs_getcodepoint(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: Scan code of key that caused event (only used for key up & down inputs.)
	End Rem
	Method getScancode:Int()
		Return bmx_cegui_keyeventargs_getscancode(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: Current state of the system keys and mouse buttons. 
	End Rem
	Method getSysKeys:Int()
		Return bmx_cegui_keyeventargs_getsyskeys(eventArgsPtr)
	End Method
	
End Type

Rem
bbdoc: EventArgs based type that is used for objects passed to input event handlers concerning mouse input.
End Rem
Type TCEMouseEventArgs Extends TCEWindowEventArgs

	Function _create:TCEMouseEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCEMouseEventArgs = New TCEMouseEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: holds current mouse position. 
	End Rem
	Method getPosition(x:Float Var, y:Float Var)
		bmx_cegui_mouseeventargs_getposition(eventArgsPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: holds variation of mouse position from last mouse input 
	End Rem
	Method getMoveDelta(x:Float Var, y:Float Var)
		bmx_cegui_mouseeventargs_getmovedelta(eventArgsPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: one of the MouseButton enumerated values describing the mouse button causing the event (for button inputs only) 
	End Rem
	Method getButton:Int()
		Return bmx_cegui_mouseeventargs_getbutton(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: current state of the system keys and mouse buttons. 
	End Rem
	Method getSysKeys:Int()
		Return bmx_cegui_mouseeventargs_getsyskeys(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: Holds the amount the scroll wheel has changed. 
	End Rem
	Method getWheelChange:Float()
		Return bmx_cegui_mouseeventargs_getwheelchange(eventArgsPtr)
	End Method
	
	Rem
	bbdoc: Holds number of mouse button down events currently counted in a multi-click sequence (for button inputs only). 
	End Rem
	Method getClickCount:Int()
		Return bmx_cegui_mouseeventargs_getclickcount(eventArgsPtr)
	End Method
	
End Type

Rem
bbdoc: EventArgs based type that is used for objects passed to input event handlers concerning Tree events.
End Rem
Type TCETreeEventArgs Extends TCEWindowEventArgs

	Function _create:TCETreeEventArgs(eventArgsPtr:Byte Ptr)
		If eventArgsPtr Then
			Local this:TCETreeEventArgs = New TCETreeEventArgs
			this.eventArgsPtr = eventArgsPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the tree item involved in the event.
	End Rem
	Method getTreeItem:TCETreeItem()
		Return TCETreeItem(bmx_cegui_treeeventargs_gettreeitem(eventArgsPtr))
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
bbdoc: Base type for a movable, sizable, window with a title-bar and a frame. 
End Rem
Type TCEFrameWindow Extends TCEWindow

	Function _create:TCEFrameWindow(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEFrameWindow = New TCEFrameWindow
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Fired when the rollup (shade) state of the window changes. 
	End Rem
	Const EventRollupToggled:String = "RollupToggled"
	Rem
	bbdoc: Fired when the close button for the window is clicked. 
	End Rem
	Const EventCloseClicked:String = "CloseClicked"
	
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
	Method isHit:Int(x:Float, y:Float, allowDisabled:Int)
		Return bmx_cegui_framewindow_ishit(objectPtr, x, y, allowDisabled)
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
bbdoc: Base type for TabButtons.
about: A TabButton based type is used internally as the button that appears at the top of a TabControl widget
to select the active tab pane. 
End Rem
Type TCETabButton Extends TCEButtonBase

	Function _create:TCETabButton(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCETabButton = New TCETabButton
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The button was clicked. 
	End Rem
	Const EventClicked:String = "Clicked"
	Rem
	bbdoc: Attempt to drag the button with middle button. 
	End Rem
	Const EventDragged:String = "Dragged"
	Rem
	bbdoc: Scroll wheel activated on top of the button. 
	End Rem
	Const EventScrolled:String = "Scrolled"

	Rem
	bbdoc: Sets whether this tab button is selected or not. 
	End Rem
	Method setSelected(selected:Int)
		bmx_cegui_tabbutton_setselected(objectPtr, selected)
	End Method
	
	Rem
	bbdoc: Returns whether this tab button is selected or not. 
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_tabbutton_isselected(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the target window which is the content pane which this button is covering. 
	End Rem
	Method setTargetWindow(wnd:TCEWindow)
		bmx_cegui_tabbutton_settargetwindow(objectPtr, wnd.objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the target window which is the content pane which this button is covering. 
	End Rem
	Method getTargetWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_tabbutton_gettargetwindow(objectPtr))
	End Method

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

	Rem
	bbdoc: Returns number of items attached to the list box. 
	End Rem
	Method getItemCount:Int()
		Return bmx_cegui_listbox_getitemcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of selected items in the list box. 
	End Rem
	Method getSelectedCount:Int()
		Return bmx_cegui_listbox_getselectedcount(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the first selected item. 
	End Rem
	Method getFirstSelectedItem:TCEListboxItem()
		Return TCEListboxItem(bmx_cegui_listbox_getfirstselecteditem(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the next selected item after item startItem. 
	End Rem
	Method getNextSelected:TCEListboxItem(startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_listbox_getnextselected(objectPtr, startItem.objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the item at index position index. 
	End Rem
	Method getListboxItemFromIndex:TCEListboxItem(index:Int)
		Return TCEListboxItem(bmx_cegui_listbox_getlistboxitemfromindex(objectPtr, index))
	End Method
	 
	Rem
	bbdoc: Returns the index of ListboxItem @item. 
	End Rem
	Method getItemIndex:Int(item:TCEListboxItem)
		Return bmx_cegui_listbox_getitemindex(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether list sorting is enabled 
	End Rem
	Method isSortEnabled:Int()
		Return bmx_cegui_listbox_issortenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether multi-select is enabled 
	End Rem
	Method isMultiselectEnabled:Int()
		Return bmx_cegui_listbox_ismultiselectenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether item tooltips are enabled.
	End Rem
	Method isItemTooltipsEnabled:Int()
		Return bmx_cegui_listbox_isitemtooltipsenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the string at index position index is selected 
	End Rem
	Method isItemSelected:Int(index:Int)
		Return bmx_cegui_listbox_isitemselected(objectPtr, index)
	End Method
	 
	Rem
	bbdoc: Searches the list for an item with the specified text. 
	End Rem
	Method findItemWithText:TCEListboxItem(text:String, startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_listbox_finditemwithtext(objectPtr, _convertMaxToUTF8(text), startItem))
	End Method
	 
	Rem
	bbdoc: Returns whether the specified ListboxItem is in the List. 
	End Rem
	Method isListboxItemInList:Int(item:TCEListboxItem)
		Return bmx_cegui_listbox_islistboxiteminlist(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown. 
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_listbox_isvertscrollbaralwaysshown(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the horizontal scroll bar is always shown. 
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_listbox_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	 
	Rem
	bbdoc: Removes all items from the list.
	about: Note that this will cause 'AutoDelete' items to be deleted. 
	End Rem
	Method resetList()
		bmx_cegui_listbox_resetlist(objectPtr)
	End Method
	 
	Rem
	bbdoc: Adds the given ListboxItem to the list. 
	End Rem
	Method addItem(item:TCEListboxItem)
		item.owner = False
		bmx_cegui_listbox_additem(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Inserts an item into the list box before a specified item already in the list.
	about: Note that if the list is sorted, the item may not end up in the requested position.
	End Rem
	Method insertItem(item:TCEListboxItem, position:TCEListboxItem)
		item.owner = False
		bmx_cegui_listbox_insertitem(objectPtr, item.objectPtr, position.objectPtr)
	End Method
	 
	Rem
	bbdoc: Removes the given item from the list box.
	about: If the item is has the auto delete state set, the item will be deleted. 
	End Rem
	Method removeItem(item:TCEListboxItem)
		bmx_cegui_listbox_removeitem(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Clears the selected state for all items. 
	End Rem
	Method clearAllSelections()
		bmx_cegui_listbox_clearallselections(objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets whether the list should be sorted. 
	End Rem
	Method setSortingEnabled(setting:Int)
		bmx_cegui_listbox_setsortingenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether the list should allow multiple selections or just a single selection. 
	End Rem
	Method setMultiselectEnabled(setting:Int)
		bmx_cegui_listbox_setmultiselectenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown. 
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_listbox_setshowvertscrollbar(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether the horizontal scroll bar should always be shown. 
	End Rem
	Method setShowHorzScrollbar(setting:Int)
		bmx_cegui_listbox_setshowhorzscrollbar(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether item tooltips are enabled.
	End Rem
	Method setItemTooltipsEnabled(setting:Int)
		bmx_cegui_listbox_setitemtooltipsenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the select state of an attached ListboxItem.
	about: This is the recommended way of selecting and deselecting items attached to a list box as
	it respects the multi-select mode setting. It is possible to modify the setting on ListboxItems
	directly, but that approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectState(item:TCEListboxItem, state:Int)
		bmx_cegui_listbox_setitemselectstate(objectPtr, item.objectPtr, state)
	End Method
	 
	Rem
	bbdoc: Sets the select state of an attached ListboxItem.
	about: This is the recommended way of selecting and deselecting items attached to a list box
	as it respects the multi-select mode setting. It is possible to modify the setting on ListboxItems
	directly, but that approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectStateForIndex(itemIndex:Int, state:Int)
		bmx_cegui_listbox_setitemselectstateforindex(objectPtr, itemIndex, state)
	End Method
	 
	Rem
	bbdoc: Causes the list box to update it's internal state after changes have been made to one or more attached ListboxItem objects.
	about: Client code must call this whenever it has made any changes to ListboxItem objects already
	attached to the list box. If you are just adding items, or removed items to update them prior
	to re-adding them, there is no need to call this method.
	End Rem
	Method handleUpdatedItemData()
		bmx_cegui_listbox_handleupdateditemdata(objectPtr)
	End Method
	 
	Rem
	bbdoc: Ensures the item at the specified index is visible within the list box. 
	End Rem
	Method ensureItemIsVisibleForIndex(itemIndex:Int)
		bmx_cegui_listbox_ensureitemisvisibleforindex(objectPtr, itemIndex)
	End Method
	 
	Rem
	bbdoc: Ensures the item at the specified index is visible within the list box. 
	End Rem
	Method ensureItemIsVisible(item:TCEListboxItem)
		bmx_cegui_listbox_ensureitemisvisible(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns a rect describing, in un-clipped pixels, the window relative area that is to be used for rendering list items. 
	End Rem
	Method getListRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_listbox_getlistrenderarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	 
	Rem
	bbdoc: Returns the vertical scrollbar component widget for this Listbox. 
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_listbox_getvertscrollbar(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the horizontal scrollbar component widget for this Listbox. 
	End Rem
	Method getHorzScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_listbox_gethorzscrollbar(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the sum of all item heights. 
	End Rem
	Method getTotalItemsHeight:Float()
		Return bmx_cegui_listbox_gettotalitemsheight(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the width of the widest item. 
	End Rem
	Method getWidestItemWidth:Float()
		Return bmx_cegui_listbox_getwidestitemwidth(objectPtr)
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
bbdoc: Base type for list box items. 
End Rem
Type TCEListboxItem

	Field objectPtr:Byte Ptr
	Field owner:Int

	Function _create:TCEListboxItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListboxItem = New TCEListboxItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Method onInit()
	End Method

	Rem
	bbdoc: Returns the text string set for this list box item.
	about: Note that even if the item does not render text, the text string can still be useful,
	since it is used for sorting list box items.
	End Rem
	Method getText:String()
		Return bmx_cegui_listboxitem_gettext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the tooltip text.
	End Rem
	Method getTooltipText:String()
		Return bmx_cegui_listboxitem_gettooltiptext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current ID assigned to this list box item.
	about: Note that the system does not make use of this value, client code can assign any meaning it
	wishes to the ID.
	End Rem
	Method getID:Int()
		Return bmx_cegui_listboxitem_getid(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the client assigned user data attached to this lis box item, if set. 
	End Rem
	Method getUserData:Object()
		Return bmx_cegui_listboxitem_getuserdata(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item is selected. 
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_listboxitem_isselected(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item is disabled. 
	End Rem
	Method isDisabled:Int()
		Return bmx_cegui_listboxitem_isdisabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item will be automatically deleted when the list box it is attached to is destroyed, or when the item is removed from the list box.
	End Rem
	Method isAutoDeleted:Int()
		Return bmx_cegui_listboxitem_isautodeleted(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the owner window for this ListboxItem.
	about: The owner of a ListboxItem is typically set by the list box widgets when an item is added or inserted.
	End Rem
	Method getOwnerWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_listboxitem_getownerwindow(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current colours used for selection highlighting. 
	End Rem
	Method getSelectionColours:TCEColourRect()
		Return TCEColourRect._create(bmx_cegui_listboxitem_getselectioncolours(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current selection highlighting brush. 
	End Rem
	Method getSelectionBrushImage:TCEImage()
		Return TCEImage(bmx_cegui_listboxitem_getselectionbrushimage(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the text string for this list box item.
	about: Note that even if the item does not render text, the text string can still be useful, since
	it is used for sorting list box items.
	End Rem
	Method setText(text:String)
		bmx_cegui_listboxitem_settext(objectPtr, text)
	End Method
	
	Rem
	bbdoc: Sets the tooltip text.
	End Rem
	Method setTooltipText(text:String)
		bmx_cegui_listboxitem_settooltiptext(objectPtr, text)
	End Method
	
	Rem
	bbdoc: Sets the ID assigned to this list box item.
	about: Note that the system does not make use of this value, client code can assign any meaning it
	wishes to the ID.
	End Rem
	Method setID(itemId:Int)
		bmx_cegui_listboxitem_setid(objectPtr, itemId)
	End Method
	
	Rem
	bbdoc: Sets the client assigned user data attached to this lis box item.
	about: Note that the system does not make use of this data, client code can assign any meaning it
	wishes to the attached data.
	<p>
	Also note that you will need to ensure you keep a reference to the object yourself, as otherwise the GC
	may collect it. (The system does not reference count this object for you!)
	</p>
	End Rem
	Method setUserData(itemData:Object)
		bmx_cegui_listboxitem_setuserdata(objectPtr, itemData)
	End Method
	
	Rem
	bbdoc: Sets whether this item is selected. 
	End Rem
	Method setSelected(setting:Int)
		bmx_cegui_listboxitem_setselected(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this item is disabled. 
	End Rem
	Method setDisabled(setting:Int)
		bmx_cegui_listboxitem_setdisabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this item will be automatically deleted when the list box it is attached to is destroyed, or when the item is removed from the list box.
	End Rem
	Method setAutoDeleted(setting:Int)
		bmx_cegui_listboxitem_setautodeleted(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the owner window for this ListboxItem.
	about: This is called by all the list box widgets when an item is added or inserted. 
	End Rem
	Method setOwnerWindow(owner:TCEWindow)
		bmx_cegui_listboxitem_setownerwindow(objectPtr, owner.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting. 
	End Rem
	Method setSelectionColours(cols:TCEColourRect)
		bmx_cegui_listboxitem_setselectioncolours(objectPtr, cols.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting. 
	End Rem
	Method setSelectionColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
		bmx_cegui_listboxitem_setselectioncoloursforcorners(objectPtr, topLeftColour.objectPtr, topRightColour.objectPtr, bottomLeftColour.objectPtr, bottomRightColour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting. 
	End Rem
	Method setSelectionColoursForColour(col:TCEColour)
		bmx_cegui_listboxitem_setselectioncoloursforcolour(objectPtr, col.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting. 
	End Rem
	Method setSelectionBrushImage(image:TCEImage)
		bmx_cegui_listboxitem_setselectionbrushimage(objectPtr, image.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the selection highlighting brush image. 
	End Rem
	Method setSelectionBrushImageByName(imageset:String, image:String)
		bmx_cegui_listboxitem_setselectionbrushimagebyname(objectPtr, _convertMaxToUTF8(imageset), _convertMaxToUTF8(image))
	End Method
	
	Rem
	bbdoc: Sets the selection highlighting brush image. 
	End Rem
	Method getPixelSize(width:Float Var, height:Float Var)
		bmx_cegui_listboxitem_getpixelsize(objectPtr, Varptr width, Varptr height)
	End Method

End Type

Rem
bbdoc: Represents a textual item in a list box. 
End Rem
Type TCEListboxTextItem Extends TCEListboxItem

	Function _create:TCEListboxTextItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEListboxTextItem = New TCEListboxTextItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new TCEListboxTextItem object.
	End Rem	
	Method Create:TCEListboxTextItem(text:String, itemId:Int = 0, disabled:Int = False, autoDelete:Int = True)
		objectPtr = bmx_cegui_listboxtextitem_new(_convertMaxToUTF8(text), itemId, disabled, autoDelete)
		owner = True
		onInit()
		Return Self
	End Method

	Rem
	bbdoc: Returns a reference to the font being used by this ListboxTextItem.
	about: This method will try a number of places to find a font to be used. If no font can be found, NULL is
	returned.
	End Rem
	Method getFont:TCEFont()
		Return TCEFont._create(bmx_cegui_listboxtextitem_getfont(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current colours used for text rendering. 
	End Rem
	Method getTextColours:TCEColourRect()
		Return TCEColourRect._create(bmx_cegui_listboxtextitem_gettextcolours(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the font to be used by this ListboxTextItem. 
	End Rem
	Method setFont(font:TCEFont)
		bmx_cegui_listboxtextitem_setfont(objectPtr, font.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the font to be used by this ListboxTextItem. 
	End Rem
	Method setFontByName(FontName:String)
		bmx_cegui_listboxtextitem_setfontbyname(objectPtr, _convertMaxToUTF8(FontName))
	End Method
	
	Rem
	bbdoc: Sets the colours used for text rendering. 
	End Rem
	Method setTextColours(cols:TCEColourRect)
		bmx_cegui_listboxtextitem_settextcolours(objectPtr, cols.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTextColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
		bmx_cegui_listboxtextitem_settextcoloursforcorner(objectPtr, topLeftColour.objectPtr, topRightColour.objectPtr, bottomLeftColour.objectPtr, bottomRightColour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colour used for text rendering. 
	End Rem
	Method setTextColoursForColour(col:TCEColour)
		bmx_cegui_listboxtextitem_settextcoloursforcolour(objectPtr, col.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the rendered pixel size of this list box item. 
	End Rem
	Method getPixelSize(width:Float Var, height:Float Var)
		bmx_cegui_listboxtextitem_getpixelsize(objectPtr, Varptr width, Varptr height)
	End Method

	Method Delete()
		If objectPtr And owner Then
			bmx_cegui_listboxtextitem_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
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
bbdoc: Base type for the Combobox widget.
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
	bbdoc: The read-only mode for the edit box has been changed. 
	End Rem
	Const EventReadOnlyModeChanged:String = "ReadOnlyChanged"
	Rem
	bbdoc: The validation string has been changed. 
	End Rem
	Const EventValidationStringChanged:String = "ValidationStringChanged"
	Rem
	bbdoc: The maximum allowable string length has been changed. 
	End Rem
	Const EventMaximumTextLengthChanged:String = "MaximumTextLengthChanged"
	Rem
	bbdoc: Some operation has made the current text invalid with regards to the validation string. 
	End Rem
	Const EventTextInvalidated:String = "TextInvalidatedEvent"
	Rem
	bbdoc: The user attempted to modify the text in a way that would have made it invalid. 
	End Rem
	Const EventInvalidEntryAttempted:String = "InvalidEntryAttempted"
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
	bbdoc: The user has accepted the current text by pressing Return, Enter, or Tab. 
	End Rem
	Const EventTextAccepted:String = "TextAcceptedEvent"
	
	' event names from list box
	Rem
	bbdoc: Event triggered when the contents of the list is changed. 
	End Rem
	Const EventListContentsChanged:String = "ListContentsChanged"
	Rem
	bbdoc: Event triggered when there is a change to the currently selected item(s). 
	End Rem
	Const EventListSelectionChanged:String = "ListSelectionChanged"
	Rem
	bbdoc: Event triggered when the sort mode setting changes. 
	End Rem
	Const EventSortModeChanged:String = "SortModeChanged"
	Rem
	bbdoc: Event triggered when the vertical scroll bar 'force' setting changes. 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: Event triggered when the horizontal scroll bar 'force' setting changes. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"
	
	' events we produce / generate ourselves
	Rem
	bbdoc: Event triggered when the drop-down list is displayed. 
	End Rem
	Const EventDropListDisplayed:String = "DropListDisplayed"
	Rem
	bbdoc: Event triggered when the drop-down list is removed / hidden. 
	End Rem
	Const EventDropListRemoved:String = "DropListRemoved"
	Rem
	bbdoc: 	Event triggered when the user accepts a selection from the drop-down list. 
	End Rem
	Const EventListSelectionAccepted:String = "ListSelectionAccepted"
	
	Rem
	bbdoc: Checks if the given position would hit this window.
	End Rem
	Method isHit:Int(x:Float, y:Float, allowDisabled:Int = False)
		Return bmx_cegui_combobox_ishit(objectPtr, x, y, allowDisabled)
	End Method
	
	Rem
	bbdoc: Returns the mode of operation for the combo box.
	End Rem
	Method getSingleClickEnabled:Int()
		Return bmx_cegui_combobox_getsingleclickenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if the drop down list is visible.
	End Rem
	Method isDropDownListVisible:Int()
		Return bmx_cegui_combobox_isdropdownlistvisible(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the Editbox component widget for this Combobox.
	End Rem
	Method getEditbox:TCEEditbox()
		Return TCEEditbox(bmx_cegui_combobox_geteditbox(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the PushButton component widget for this Combobox.
	End Rem
	Method getPushButton:TCEPushButton()
		Return TCEPushButton(bmx_cegui_combobox_getpushbutton(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the ComboDropList component widget for this Combobox.
	End Rem
	Method getDropList:TCEComboDropList()
		Return TCEComboDropList(bmx_cegui_combobox_getdroplist(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns true if the Editbox has input focus.
	End Rem
	Method hasInputFocus:Int()
		Return bmx_cegui_combobox_hasinputfocus(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if the Editbox is read-only.
	End Rem
	Method isReadOnly:Int()
		Return bmx_cegui_combobox_isreadonly(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns true if the Editbox text is valid given the currently set validation string.
	about: It is possible to programmatically set 'invalid' text for the Editbox by calling setText. This has
	certain implications since if invalid text is set, whatever the user types into the box will be rejected
	when the input is validated.
	<p>
	Validation is performed by means of a regular expression. If the text matches the regex, the text is said
	to have passed validation. If the text does not match with the regex then the text fails validation.
	</p>
	End Rem
	Method isTextValid:Int()
		Return bmx_cegui_combobox_istextvalid(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the currently set validation string
	about: Validation is performed by means of a regular expression. If the text matches the regex, the text
	is said to have passed validation. If the text does not match with the regex then the text fails validation.
	End Rem
	Method getValidationString:String()
		Return bmx_cegui_combobox_getvalidationstring(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current position of the carat.
	End Rem
	Method getCaratIndex:Int()
		Return bmx_cegui_combobox_getcaratindex(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current selection start point.
	End Rem
	Method getSelectionStartIndex:Int()
		Return bmx_cegui_combobox_getselectionstartindex(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current selection end point.
	End Rem
	Method getSelectionEndIndex:Int()
		Return bmx_cegui_combobox_getselectionendindex(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the length of the current selection (in code points / characters).
	End Rem
	Method getSelectionLength:Int()
		Return bmx_cegui_combobox_getselectionlength(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the maximum text length set for this Editbox.
	about: Depending on the validation string set, the actual length of text that can be entered may be less
	than the value returned here (it will never be more).
	End Rem
	Method getMaxTextLength:Int()
		Return bmx_cegui_combobox_getmaxtextlength(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns number of items attached to the list box.
	End Rem
	Method getItemCount:Int()
		Return bmx_cegui_combobox_getitemcount(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the currently selected item.
	End Rem
	Method getSelectedItem:TCEListboxItem()
		Return TCEListboxItem(bmx_cegui_combobox_getselecteditem(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the item at index position @index.
	End Rem
	Method getListboxItemFromIndex:TCEListboxItem(index:Int)
		Return TCEListboxItem(bmx_cegui_combobox_getlistboxitemfromindex(objectPtr, index))
	End Method
	 
	Rem
	bbdoc: Returns the index of ListboxItem @item.
	End Rem
	Method getItemIndex:Int(item:TCEListboxItem)
		Return bmx_cegui_combobox_getitemindex(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether list sorting is enabled.
	End Rem
	Method isSortEnabled:Int()
		Return bmx_cegui_combobox_issortenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the string at index position @index is selected.
	End Rem
	Method isItemSelected:Int(index:Int)
		Return bmx_cegui_combobox_isitemselected(objectPtr, index)
	End Method
	 
	Rem
	bbdoc: Searches the list for an item with the specified text.
	End Rem
	Method findItemWithText:TCEListboxItem(text:String, startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_combobox_finditemwithtext(objectPtr, _convertMaxToUTF8(text), startItem.objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns whether the specified ListboxItem is in the List.
	End Rem
	Method isListboxItemInList:Int(item:TCEListboxItem)
		Return bmx_cegui_combobox_islistboxiteminlist(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown.
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_combobox_isvertscrollbaralwaysshown(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the horizontal scroll bar is always shown.
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_combobox_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	 
	Rem
	bbdoc: Shows the drop-down list.
	End Rem
	Method showDropList()
		bmx_cegui_combobox_showdroplist(objectPtr)
	End Method
	 
	Rem
	bbdoc: Hides the drop-down list.
	End Rem
	Method hideDropList()
		bmx_cegui_combobox_hidedroplist(objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the mode of operation for the combo box.
	End Rem
	Method setSingleClickEnabled(setting:Int)
		bmx_cegui_combobox_setsingleclickenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Specifes whether the Editbox is read-only.
	End Rem
	Method setReadOnly(setting:Int)
		bmx_cegui_combobox_setreadonly(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets the text validation string.
	about: Validation is performed by means of a regular expression. If the text matches the regex, the
	text is said to have passed validation. If the text does not match with the regex then the text fails
	validation.
	End Rem
	Method setValidationString(validationString:String)
		bmx_cegui_combobox_setvalidationstring(objectPtr, _convertMaxToUTF8(validationString))
	End Method
	 
	Rem
	bbdoc: Sets the current position of the carat.
	End Rem
	Method setCaratIndex(caratPos:Int)
		bmx_cegui_combobox_setcaratindex(objectPtr, caratPos)
	End Method
	 
	Rem
	bbdoc: Defines the current selection for the Editbox.
	End Rem
	Method setSelection(startPos:Int, endPos:Int)
		bmx_cegui_combobox_setselection(objectPtr, startPos, endPos)
	End Method
	 
	Rem
	bbdoc: Sets the maximum text length for this Editbox.
	about: Depending on the validation string set, the actual length of text that can be entered may
	be less than the value set here (it will never be more).
	End Rem
	Method setMaxTextLength(maxLen:Int)
		bmx_cegui_combobox_setmaxtextlength(objectPtr, maxLen)
	End Method
	 
	Rem
	bbdoc: Activates the edit box component of the Combobox.
	End Rem
	Method activateEditbox()
		bmx_cegui_combobox_activateeditbox(objectPtr)
	End Method
	 
	Rem
	bbdoc: Removes all items from the list.
	about: Note that this will cause 'AutoDelete' items to be deleted.
	End Rem
	Method resetList()
		bmx_cegui_combobox_resetlist(objectPtr)
	End Method
	 
	Rem
	bbdoc: Adds the given ListboxItem to the list.
	End Rem
	Method addItem(item:TCEListboxItem)
		bmx_cegui_combobox_additem(objectPtr, item.objectPtr)
		item.owner = False
	End Method
	 
	Rem
	bbdoc: Inserts an item into the list box after a specified item already in the list.
	about: Note that if the list is sorted, the item may not end up in the requested position.
	End Rem
	Method insertItem(item:TCEListboxItem, position:TCEListboxItem)
		bmx_cegui_combobox_insertitem(objectPtr, item.objectPtr, position.objectPtr)
		item.owner = False
	End Method
	 
	Rem
	bbdoc: Removes the given item from the list box.
	End Rem
	Method removeItem(item:TCEListboxItem)
		bmx_cegui_combobox_removeitem(objectPtr, item.objectPtr)
	End Method
	 
	Rem
	bbdoc: Clears the selected state for all items.
	End Rem
	Method clearAllSelections()
		bmx_cegui_combobox_clearallselections(objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets whether the list should be sorted.
	End Rem
	Method setSortingEnabled(setting:Int)
		bmx_cegui_combobox_setsortingenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown.
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_combobox_setshowvertscrollbar(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether the horizontal scroll bar should always be shown.
	End Rem
	Method setShowHorzScrollbar(setting:Int)
		bmx_cegui_combobox_setshowhorzscrollbar(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets the select state of an attached ListboxItem.
	about: This is the recommended way of selecting and deselecting items attached to a list box as it
	respects the multi-select mode setting. It is possible to modify the setting on ListboxItems directly,
	but that approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectState(item:TCEListboxItem, state:Int)
		bmx_cegui_combobox_setitemselectstate(objectPtr, item.objectPtr, state)
	End Method
	 
	Rem
	bbdoc: Sets the select state of an attached ListboxItem.
	about: This is the recommended way of selecting and deselecting items attached to a list box as it respects
	the multi-select mode setting. It is possible to modify the setting on ListboxItems directly, but that
	approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectStateIndex(itemIndex:Int, state:Int)
		bmx_cegui_combobox_setitemselectstateindex(objectPtr, itemIndex, state)
	End Method
	 
	Rem
	bbdoc: Causes the list box to update it's internal state after changes have been made to one or more attached ListboxItem objects.
	about: Client code must call this whenever it has made any changes to ListboxItem objects already attached
	to the list box. If you are just adding items, or removed items to update them prior to re-adding them,
	there is no need to call this method.
	End Rem
	Method handleUpdatedListItemData()
		bmx_cegui_combobox_handleupdatedlistitemdata(objectPtr)
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

	Rem
	bbdoc: Returns the content pane held by this GroupBox.
	End Rem
	Method getContentPane:TCEWindow()
		Return TCEWindow(bmx_cegui_groupbox_getcontentpane(objectPtr))
	End Method
	
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
	Method setTextInputMode(Mode:Int)
		bmx_cegui_spinner_settextinputmode(objectPtr, Mode)
	End Method

	
End Type


Rem
bbdoc: Base type for item list widgets. 
End Rem
Type TCEItemListBase Extends TCEWindow

	Function _create:TCEItemListBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemListBase = New TCEItemListBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Event triggered when the contents of the list is changed. 
	End Rem
	Const EventListContentsChanged:String = "ListItemsChanged"
	Rem
	bbdoc: Event fired when the sort enabled state changes. 
	End Rem
	Const EventSortEnabledChanged:String = "SortEnabledChanged"
	Rem
	bbdoc: Event fired when the sort mode changes. 
	End Rem
	Const EventSortModeChanged:String = "SortModeChanged"
	
	Rem
	bbdoc: Returns number of items attached to the list. 
	End Rem
	Method getItemCount:Int()
		Return bmx_cegui_itemlistbase_getitemcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the item at index position index. 
	End Rem
	Method getItemFromIndex:TCEItemEntry(index:Int)
		Return TCEItemEntry(bmx_cegui_itemlistbase_getitemfromindex(objectPtr, index))
	End Method
	
	Rem
	bbdoc: Returns the index of ItemEntry item. 
	End Rem
	Method getItemIndex:Int(item:TCEItemEntry)
		Return bmx_cegui_itemlistbase_getitemindex(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Searches the list for an item with the specified text. 
	End Rem
	Method findItemWithText:TCEItemEntry(text:String, startItem:TCEItemEntry)
		Return TCEItemEntry(bmx_cegui_itemlistbase_finditemwithtext(objectPtr, text, startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns whether the specified ItemEntry is in the List. 
	End Rem
	Method isItemInList:Int(item:TCEItemEntry)
		Return bmx_cegui_itemlistbase_isiteminlist(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this window is automatically resized to fit its content. 
	End Rem
	Method isAutoResizeEnabled:Int()
		Return bmx_cegui_itemlistbase_isautoresizeenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns 'true' if the list is sorted. 
	End Rem
	Method isSortEnabled:Int()
		Return bmx_cegui_itemlistbase_issortenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Get sort mode. 
	End Rem
	Method getSortMode:Int()
		Return bmx_cegui_itemlistbase_getsortmode(objectPtr)
	End Method
	
	'Method getSortCallback()
	'End Method
	
	Rem
	bbdoc: Removes all items from the list.
	about: Note that this will cause items, which does not have the 'DestroyedByParent' property set
	to 'false', to be deleted. 
	End Rem
	Method resetList()
		bmx_cegui_itemlistbase_resetlist(objectPtr)
	End Method
	
	Rem
	bbdoc: Adds the given ItemEntry to the list. 
	End Rem
	Method addItem(item:TCEItemEntry)
		bmx_cegui_itemlistbase_additem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Inserts an item into the list before a specified item already in the list.
	about: Note that if the list is sorted, the item may not end up in the requested position.
	End Rem
	Method insertItem(item:TCEItemEntry, position:TCEItemEntry)
		bmx_cegui_itemlistbase_insertitem(objectPtr, item.objectPtr, position.objectPtr)
	End Method
	
	Rem
	bbdoc: Removes the given item from the list.
	about: If the item is has the 'DestroyedByParent' property set to 'true', the item will be deleted.
	End Rem
	Method removeItem(item:TCEItemEntry)
		bmx_cegui_itemlistbase_removeitem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Causes the list to update it's internal state after changes have been made to one or more attached ItemEntry objects.
	about: It should not be necessary to call this from client code, as the ItemEntries themselves
	call it if their parent is an ItemListBase.
	End Rem
	Method handleUpdatedItemData(resort:Int = False)
		bmx_cegui_itemlistbase_handleupdateditemdata(objectPtr, resort)
	End Method
	
	Rem
	bbdoc: Sets whether or not this ItemListBase widget should automatically resize to fit its content. 
	End Rem
	Method setAutoResizeEnabled(setting:Int)
		bmx_cegui_itemlistbase_setautoresizeenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Resizes the ItemListBase to exactly fit the content that is attached to it.
	End Rem
	Method sizeToContent()
		bmx_cegui_itemlistbase_sizetocontent(objectPtr)
	End Method
	
	Rem
	bbdoc: Triggers a ListContentsChanged event.
	about: These are not fired during initialisation for optimization purposes. 
	End Rem
	Method endInitialisation()
		bmx_cegui_itemlistbase_endinitialisation(objectPtr)
	End Method
	
	Rem
	bbdoc: Called to perform extended laying out of attached child windows. 
	End Rem
	Method performChildWindowLayout()
		bmx_cegui_itemlistbase_performchildwindowlayout(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns a rect describing, in un-clipped pixels, the window relative area that is to be used for rendering list items. 
	End Rem
	Method getItemRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_itemlistbase_getitemrenderarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Returns the window that all items are directed too. 
	End Rem
	Method getContentPane:TCEWindow()
		Return TCEWindow(bmx_cegui_itemlistbase_getcontentpane(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets whether the list should be sorted (by text). 
	End Rem
	Method setSortEnabled(setting:Int)
		bmx_cegui_itemlistbase_setsortenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the mode to be used when sorting the list. 
	End Rem
	Method setSortMode(Mode:Int)
		bmx_cegui_itemlistbase_setsortmode(objectPtr, Mode)
	End Method
	
	'Method setSortCallback(SortCallback cb)
	'End Method
	
	Rem
	bbdoc: Sorts the list. 
	End Rem
	Method SortList(relayout:Int = True)
		bmx_cegui_itemlistbase_sortlist(objectPtr, relayout)
	End Method

End Type

Rem
bbdoc: Base type for menus. 
End Rem
Type TCEMenuBase Extends TCEItemListBase

	Function _create:TCEMenuBase(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMenuBase = New TCEMenuBase
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: A MenuItem attached to this menu opened a PopupMenu. 
	End Rem
	Const EventPopupOpened:String = "PopupOpened"
	Rem
	bbdoc: A MenuItem attached to this menu opened a PopupMenu. 
	End Rem
	Const EventPopupClosed:String = "PopupClosed"

	Rem
	bbdoc: Gets the item spacing for this menu. 
	End Rem
	Method getItemSpacing:Float()
		Return bmx_cegui_menubase_getitemspacing(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this menu allows multiple popup menus to open at the same time. 
	End Rem
	Method isMultiplePopupsAllowed:Int()
		Return bmx_cegui_menubase_ismultiplepopupsallowed(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets currently opened MenuItem in this menu. Returns NULL if no menu item is open. 
	End Rem
	Method getPopupMenuItem:TCEMenuItem()
		Return TCEMenuItem(bmx_cegui_menubase_getpopupmenuitem(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the item spacing for this menu. 
	End Rem
	Method setItemSpacing(spacing:Float)
		bmx_cegui_menubase_setitemspacing(objectPtr, spacing)
	End Method
	
	Rem
	bbdoc: Changes the currently open MenuItem in this menu. 
	End Rem
	Method changePopupMenuItem(item:TCEMenuItem)
		bmx_cegui_menubase_changepopupmenuitem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this menu allows multiple popup menus to be opened simultaneously. 
	End Rem
	Method setAllowMultiplePopups(setting:Int)
		bmx_cegui_menubase_setallowmultiplepopups(objectPtr, setting)
	End Method

End Type

Rem
bbdoc: Base type for menu bars. 
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
bbdoc: Base type for popup menus. 
End Rem
Type TCEPopupMenu Extends TCEMenuBase

	Function _create:TCEPopupMenu(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEPopupMenu = New TCEPopupMenu
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Gets the fade in time for this popup menu. 
	End Rem
	Method getFadeInTime:Float()
		Return bmx_cegui_popupmenu_getfadeintime(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the fade out time for this popup menu. 
	End Rem
	Method getFadeOutTime:Float()
		Return bmx_cegui_popupmenu_getfadeouttime(objectPtr)
	End Method
	
	Rem
	bbdoc: Find out if this popup menu is open or closed. 
	End Rem
	Method isPopupMenuOpen:Int()
		Return bmx_cegui_popupmenu_ispopupmenuopen(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the fade in time for this popup menu. 
	End Rem
	Method setFadeInTime(fadetime:Float)
		bmx_cegui_popupmenu_setfadeintime(objectPtr, fadetime)
	End Method
	
	Rem
	bbdoc: Sets the fade out time for this popup menu. 
	End Rem
	Method setFadeOutTime(fadetime:Float)
		bmx_cegui_popupmenu_setfadeouttime(objectPtr, fadetime)
	End Method
	
	Rem
	bbdoc: Tells the popup menu to open. 
	End Rem
	Method openPopupMenu(_notify:Int = True)
		bmx_cegui_popupmenu_openpopupmenu(objectPtr, _notify)
	End Method
	
	Rem
	bbdoc: Tells the popup menu to close. 
	End Rem
	Method closePopupMenu(_notify:Int = True)
		bmx_cegui_popupmenu_closepopupmenu(objectPtr, _notify)
	End Method

End Type

Rem
bbdoc: Base type for item type widgets. 
End Rem
Type TCEItemEntry Extends TCEWindow

	Function _create:TCEItemEntry(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEItemEntry = New TCEItemEntry
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the "optimal" size for the item.
	End Rem
	Method getItemPixelSize(w:Float Var, h:Float Var)
		bmx_cegui_itementry_getitempixelsize(objectPtr, Varptr w, Varptr h)
	End Method

	Rem
	bbdoc: Returns a reference to the owner ItemListBase, or Null if there is none. 
	End Rem
	Method getOwnerList:TCEItemListBase()
		Return TCEItemListBase(bmx_cegui_itementry_getownerlist(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns whether this item is selected or not. 
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_itementry_isselected(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item is selectable or not. 
	End Rem
	Method isSelectable:Int()
		Return bmx_cegui_itementry_isselectable(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the selection state of this item (on/off).
	about: If this item is not selectable this function does nothing.
	End Rem
	Method setSelected(setting:Int)
		bmx_cegui_itementry_setselected(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Selects the item. 
	End Rem
	Method selectEntry()
		bmx_cegui_itementry_selectentry(objectPtr)
	End Method
	
	Rem
	bbdoc: Deselects the item. 
	End Rem
	Method deselect()
		bmx_cegui_itementry_deselect(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether this item will be selectable.
	about: If the item is currently selectable and selected, calling this function with setting as 'false'
	will first deselect the item and then disable selectability.
	End Rem
	Method setSelectable(setting:Int)
		bmx_cegui_itementry_setselectable(objectPtr, setting)
	End Method

End Type

Rem
bbdoc: Base type for menu items.
End Rem
Type TCEMenuItem Extends TCEItemEntry

	Function _create:TCEMenuItem(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEMenuItem = New TCEMenuItem
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The menu item was clicked. 
	End Rem
	Const EventClicked:String = "Clicked"

	Rem
	bbdoc: Returns true if user is hovering over this widget (or it's pushed and user is not over it for highlight) 
	End Rem
	Method isHovering:Int()
		Return bmx_cegui_menuitem_ishovering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the button widget is in the pushed state. 
	End Rem
	Method isPushed:Int()
		Return bmx_cegui_menuitem_ispushed(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the popup menu attached to the menu item is open. 
	End Rem
	Method isOpened:Int()
		Return bmx_cegui_menuitem_isopened(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the PopupMenu that is currently attached to this MenuItem. 
	End Rem
	Method getPopupMenu:TCEPopupMenu()
		Return TCEPopupMenu(bmx_cegui_menuitem_getpopupmenu(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the popup menu for this item. 
	End Rem
	Method setPopupMenu(popup:TCEPopupMenu)
		bmx_cegui_menuitem_setpopupmenu(objectPtr, popup.objectPtr)
	End Method
	
	Rem
	bbdoc: Opens the PopupMenu. 
	End Rem
	Method openPopupMenu(_notify:Int = True)
		bmx_cegui_menuitem_openpopupmenu(objectPtr, _notify)
	End Method
	
	Rem
	bbdoc: Closes the PopupMenu. 
	End Rem
	Method closePopupMenu(_notify:Int = True)
		bmx_cegui_menuitem_closepopupmenu(objectPtr, _notify)
	End Method
	
	Rem
	bbdoc: Toggles the PopupMenu. 
	End Rem
	Method togglePopupMenu:Int()
		Return bmx_cegui_menuitem_togglepopupmenu(objectPtr)
	End Method

End Type

Rem
bbdoc: Base type for the ScrollablePane widget.
about: The ScrollablePane widget allows child windows to be attached which cover an area larger than the
ScrollablePane itself and these child windows can be scrolled into view using the scrollbars of the
scrollable pane.
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
	bbdoc: Event fired when an area on the content pane has been updated. 
	End Rem
	Const EventContentPaneChanged:String = "ContentPaneChanged"
	Rem
	bbdoc: Event triggered when the vertical scroll bar 'force' setting changes. 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertScrollbarModeChanged"
	Rem
	bbdoc: Event triggered when the horizontal scroll bar 'force' setting changes. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzScrollbarModeChanged"
	Rem
	bbdoc: Event fired when the auto size setting changes. 
	End Rem
	Const EventAutoSizeSettingChanged:String = "AutoSizeSettingChanged"
	Rem
	bbdoc: Event fired when the pane gets scrolled. 
	End Rem
	Const EventContentPaneScrolled:String = "ContentPaneScrolled"

	Rem
	bbdoc: Returns a reference to the window holding the pane contents.
	about: The purpose of this is so that attached windows may be inspected, client code may not modify
	the returned window in any way.
	End Rem
	Method getContentPane:TCEScrolledContainer()
		Return TCEScrolledContainer(bmx_cegui_scrollablepane_getcontentpane(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown. 
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_scrollablepane_isvertscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown. 
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_scrollablepane_setshowvertscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns whether the horizontal scroll bar is always shown. 
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_scrollablepane_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the horizontal scroll bar should always be shown. 
	End Rem
	Method setShowHorzScrollbar(setting:Int)
		bmx_cegui_scrollablepane_setshowhorzscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns whether the content pane is auto sized. 
	End Rem
	Method isContentPaneAutoSized:Int()
		Return bmx_cegui_scrollablepane_iscontentpaneautosized(objectPtr)
	End Method
	
	Rem
	bbdoc: Set whether the content pane should be auto-sized. 
	End Rem
	Method setContentPaneAutoSized(setting:Int)
		bmx_cegui_scrollablepane_setcontentpaneautosized(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns the current content pane area for the ScrollablePane. 
	End Rem
	Method getContentPaneArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_scrollablepane_getcontentpanearea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Sets the current content pane area for the ScrollablePane. 
	about: If the ScrollablePane is configured to auto-size the content pane this call will have no effect.
	End Rem
	Method setContentPaneArea(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_scrollablepane_setcontentpanearea(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: Returns the horizontal scrollbar step size as a fraction of one complete view page. 
	End Rem
	Method getHorizontalStepSize:Float()
		Return bmx_cegui_scrollablepane_gethorizontalstepsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the horizontal scrollbar step size as a fraction of one complete view page. 
	End Rem
	Method setHorizontalStepSize(stepSize:Float)
		bmx_cegui_scrollablepane_sethorizontalstepsize(objectPtr, stepSize)
	End Method
	
	Rem
	bbdoc: Returns the horizontal scrollbar overlap size as a fraction of one complete view page. 
	End Rem
	Method getHorizontalOverlapSize:Float()
		Return bmx_cegui_scrollablepane_gethorizontaloverlapsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the horizontal scrollbar overlap size as a fraction of one complete view page. 
	End Rem
	Method setHorizontalOverlapSize(overlap:Float)
		bmx_cegui_scrollablepane_sethorizontaloverlapsize(objectPtr, overlap)
	End Method
	
	Rem
	bbdoc: Returns the horizontal scroll position as a fraction of the complete scrollable width. 
	End Rem
	Method getHorizontalScrollPosition:Float()
		Return bmx_cegui_scrollablepane_gethorizontalscrollposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the horizontal scroll position as a fraction of the complete scrollable width. 
	End Rem
	Method setHorizontalScrollPosition(position:Float)
		bmx_cegui_scrollablepane_sethorizontalscrollposition(objectPtr, position)
	End Method
	
	Rem
	bbdoc: Returns the vertical scrollbar step size as a fraction of one complete view page. 
	End Rem
	Method getVerticalStepSize:Float()
		Return bmx_cegui_scrollablepane_getverticalstepsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the vertical scrollbar step size as a fraction of one complete view page. 
	End Rem
	Method setVerticalStepSize(stepSize:Float)
		bmx_cegui_scrollablepane_setverticalstepsize(objectPtr, stepSize)
	End Method
	
	Rem
	bbdoc: Returns the vertical scrollbar overlap size as a fraction of one complete view page. 
	End Rem
	Method getVerticalOverlapSize:Float()
		Return bmx_cegui_scrollablepane_getverticaloverlapsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the vertical scrollbar overlap size as a fraction of one complete view page. 
	End Rem
	Method setVerticalOverlapSize(overlap:Float)
		bmx_cegui_scrollablepane_setverticaloverlapsize(objectPtr, overlap)
	End Method
	
	Rem
	bbdoc: Returns the vertical scroll position as a fraction of the complete scrollable height. 
	End Rem
	Method getVerticalScrollPosition:Float()
		Return bmx_cegui_scrollablepane_getverticalscrollposition(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the vertical scroll position as a fraction of the complete scrollable height. 
	End Rem
	Method setVerticalScrollPosition(position:Float)
		bmx_cegui_scrollablepane_setverticalscrollposition(objectPtr, position)
	End Method
	
	Rem
	bbdoc: Returns a Rect that described the pane's viewable area, relative to this Window, in pixels. 
	End Rem
	Method getViewableArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_scrollablepane_getviewablearea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Returns a reference to the vertical scrollbar component widget for this ScrollablePane. 
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_scrollablepane_getvertscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Return a reference to the horizontal scrollbar component widget for this ScrollablePane. 
	End Rem
	Method getHorzScrollbar:TCEScrollbar() 
		Return TCEScrollbar(bmx_cegui_scrollablepane_gethorzscrollbar(objectPtr))
	End Method
	
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

	Rem
	bbdoc: Returns the current clipping rectangle. 
	End Rem
	Method getClipArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_clippedcontainer_getcliparea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Returns the reference window used for converting the clipper rect to screen space. 
	End Rem
	Method getClipperWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_clippedcontainer_getclipperwindow(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the clipper reference window. 
	End Rem
	Method setClipArea(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_clippedcontainer_setcliparea(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: Sets the clipper reference window. 
	End Rem
	Method setClipperWindow(w:TCEWindow)
		bmx_cegui_clippedcontainer_setclipperwindow(objectPtr, w.objectPtr)
	End Method

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

	Rem
	bbdoc: Event fired whenever some child changes. 
	End Rem
	Const EventContentChanged:String = "ContentChanged"
	Rem
	bbdoc: Event fired when the autosize setting changes. 
	End Rem
	Const EventAutoSizeSettingChanged:String = "AutoSizeSettingChanged"

	Rem
	bbdoc: Returns whether the content pane is auto sized. 
	End Rem
	Method isContentPaneAutoSized:Int()
		Return bmx_cegui_scrolledcontainer_iscontentpaneautosized(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the content pane should be auto-sized. 
	End Rem
	Method setContentPaneAutoSized(setting:Int)
		bmx_cegui_scrolledcontainer_setcontentpaneautosized(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Returns the current content pane area for the ScrolledContainer. 
	End Rem
	Method getContentArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_scrolledcontainer_getcontentarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Return the current content pane area for the ScrolledContainer. 
	about: If the ScrolledContainer is configured to auto-size the content pane this call will have no effect.
	End Rem
	Method setContentArea(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_scrolledcontainer_setcontentarea(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: Return the current extents of the attached content. 
	End Rem
	Method getChildExtentsArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var) 
		bmx_cegui_scrolledcontainer_getchildextentsarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
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

	Rem
	bbdoc: Event fired when the segment is clicked. 
	End Rem
	Const EventSegmentClicked:String = "SegmentClicked"
	Rem
	bbdoc: Event fired when the sizer/splitter is double-clicked.
	End Rem
	Const EventSplitterDoubleClicked:String = "SplitterDoubleClicked"
	Rem
	bbdoc: Event fired when the sizing setting changes. 
	End Rem
	Const EventSizingSettingChanged:String = "SizingSettingChanged"
	Rem
	bbdoc: Event fired when the sort direction value changes.
	End Rem
	Const EventSortDirectionChanged:String = "SortDirectionChanged"
	Rem
	bbdoc: Event fired when the movable setting changes. 
	End Rem
	Const EventMovableSettingChanged:String = "MovableSettingChanged"
	Rem
	bbdoc: Event fired when the segment has started to be dragged. 
	End Rem
	Const EventSegmentDragStart:String = "SegmentDragStart"
	Rem
	bbdoc: Event fired when segment dragging has stopped (via mouse release). 
	End Rem
	Const EventSegmentDragStop:String = "SegmentDragStop"
	Rem
	bbdoc: Event fired when the segment drag position has changed. 
	End Rem
	Const EventSegmentDragPositionChanged:String = "SegmentDragPositionChanged"
	Rem
	bbdoc: Event fired when the segment is sized. 
	End Rem
	Const EventSegmentSized:String = "SegmentSized"
	Rem
	bbdoc: Event fired when the clickable state of the segment changes. 
	End Rem
	Const EventClickableSettingChanged:String = "ClickableSettingChanged"

	Rem
	bbdoc: Items under this segment should not be sorted.
	End Rem
	Const SORT_NONE:Int = 0
	Rem
	bbdoc: Items under this segment should be sorted in ascending order.
	End Rem
	Const SORT_ASCENDING:Int = 1
	Rem
	bbdoc: Items under this segment should be sorted in descending order.
	End Rem
	Const SORT_DESCENDING:Int = 2

	Rem
	bbdoc: Returns whether this segment can be sized. 
	End Rem
	Method isSizingEnabled:Int()
		Return bmx_cegui_listheadersegment_issizingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current sort direction set for this segment. 
	about: Note that this has no impact on the way the segment functions (aside from the possibility of varied rendering).
	This is intended as a 'helper setting' to classes that make use of the ListHeaderSegment objects.
	End Rem
	Method getSortDirection:Int()
		Return bmx_cegui_listheadersegment_getsortdirection(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether drag moving is enabled for this segment. 
	End Rem
	Method isDragMovingEnabled:Int()
		Return bmx_cegui_listheadersegment_isdragmovingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current drag move position offset (in pixels relative to the top-left corner of the segment). 
	End Rem
	Method getDragMoveOffset(x:Float Var, y:Float Var)
		bmx_cegui_listheadersegment_getdragmoveoffset(objectPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Returns whether the segment is clickable.
	End Rem
	Method isClickable:Int()
		Return bmx_cegui_listheadersegment_isclickable(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the segment is currently in its hovering state. 
	End Rem
	Method isSegmentHovering:Int()
		Return bmx_cegui_listheadersegment_issegmenthovering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the segment is currently in its pushed state. 
	End Rem
	Method isSegmentPushed:Int()
		Return bmx_cegui_listheadersegment_issegmentpushed(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the splitter is currently in its hovering state. 
	End Rem
	Method isSplitterHovering:Int()
		Return bmx_cegui_listheadersegment_issplitterhovering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the segment is currently being drag-moved. 
	End Rem
	Method isBeingDragMoved:Int()
		Return bmx_cegui_listheadersegment_isbeingdragmoved(objectPtr)
	End Method
	
	Rem
	bbdoc: Return whether the segment is currently being drag-moved. 
	End Rem
	Method isBeingDragSized:Int()
		Return bmx_cegui_listheadersegment_isbeingdragsized(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the image used when sizing.
	End Rem
	Method getSizingCursorImage:TCEImage()
		Return TCEImage(bmx_cegui_listheadersegment_getsizingcursorimage(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the image used when moving.
	End Rem
	Method getMovingCursorImage:TCEImage()
		Return TCEImage(bmx_cegui_listheadersegment_getmovingcursorimage(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets whether this segment can be sized. 
	End Rem
	Method setSizingEnabled(setting:Int)
		bmx_cegui_listheadersegment_setsizingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets the current sort direction set for this segment. 
	about: Note that this has no impact on the way the segment functions (aside from the possibility of varied rendering).
	This is intended as a 'helper setting' to classes that make use of the ListHeaderSegment objects.
	End Rem
	Method setSortDirection(sortDir:Int)
		bmx_cegui_listheadersegment_setsortdirection(objectPtr, sortDir)
	End Method
	
	Rem
	bbdoc: Sets whether drag moving is allowed for this segment.
	End Rem
	Method setDragMovingEnabled(setting:Int)
		bmx_cegui_listheadersegment_setdragmovingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the segment is clickable. 
	End Rem
	Method setClickable(setting:Int)
		bmx_cegui_listheadersegment_setclickable(objectPtr, setting)
	End Method

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
	bbdoc: Event fired when the current sort column changes. 
	End Rem
	Const EventSortColumnChanged:String = "SortColumnChanged"
	Rem
	bbdoc: Event fired when the sort direction changes. 
	End Rem
	Const EventSortDirectionChanged:String = "SortDirectionChanged"
	Rem
	bbdoc: Event fired when a segment has been sized by the user (e.window is the segment).
	End Rem
	Const EventSegmentSized:String = "SegmentSized"
	Rem
	bbdoc: Event fired when a segment has been clicked by the user (e.window is the segment). 
	End Rem
	Const EventSegmentClicked:String = "SegmentClicked"
	Rem
	bbdoc: Event fired when a segment splitter has been double-clicked. (e.window is the segment). 
	End Rem
	Const EventSplitterDoubleClicked:String = "SplitterDoubleClicked"
	Rem
	bbdoc: Event fired when the order of the segments has changed. ('e' is a HeaderSequenceEventArgs). 
	End Rem
	Const EventSegmentSequenceChanged:String = "SegmentSequenceChanged"
	Rem
	bbdoc: Event fired when a segment is added to the header. 
	End Rem
	Const EventSegmentAdded:String = "SegmentAdded"
	Rem
	bbdoc: Event fired when a segment is removed from the header.
	End Rem
	Const EventSegmentRemoved:String = "SegmentRemoved"
	Rem
	bbdoc: Event fired when setting that controls user modification to sort configuration changes. 
	End Rem
	Const EventSortSettingChanged:String = "SortSettingChanged"
	Rem
	bbdoc: Event fired when setting that controls user drag & drop of segments changes. 
	End Rem
	Const EventDragMoveSettingChanged:String = "DragMoveSettingChanged"
	Rem
	bbdoc: Event fired when setting that controls user sizing of segments changes. 
	End Rem
	Const EventDragSizeSettingChanged:String = "DragSizeSettingChanged"
	Rem
	bbdoc: Event fired when the rendering offset for the segments changes. 
	End Rem
	Const EventSegmentRenderOffsetChanged:String = "SegmentOffsetChanged"

	Rem
	bbdoc: Returns the number of columns or segments attached to the header.
	End Rem
	Method getColumnCount:Int()
		Return bmx_cegui_listheader_getcolumncount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the ListHeaderSegment object for the specified column.
	End Rem
	Method getSegmentFromColumn:TCEListHeaderSegment(column:Int)
		Return TCEListHeaderSegment(bmx_cegui_listheader_getsegmentfromcolumn(objectPtr, column))
	End Method
	 
	Rem
	bbdoc: Returns the ListHeaderSegment object with the specified ID.
	End Rem
	Method getSegmentFromID:TCEListHeaderSegment(id:Int)
		Return TCEListHeaderSegment(bmx_cegui_listheader_getsegmentfromid(objectPtr, id))
	End Method
	 
	Rem
	bbdoc: Returns the ListHeaderSegment that is marked as being the 'sort key' segment.
	about: There must be at least one segment to successfully call this method.
	End Rem
	Method getSortSegment:TCEListHeaderSegment()
		Return TCEListHeaderSegment(bmx_cegui_listheader_getsortsegment(objectPtr))
	End Method
	 
	Rem
	bbdoc: Returns the zero based column index of the specified segment.
	End Rem
	Method getColumnFromSegment:Int(segment:TCEListHeaderSegment)
		Return bmx_cegui_listheader_getcolumnfromsegment(objectPtr, segment.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the zero based column index of the segment with the specified ID.
	End Rem
	Method getColumnFromID:Int(id:Int)
		Return bmx_cegui_listheader_getcolumnfromid(objectPtr, id)
	End Method
	 
	Rem
	bbdoc: Returns the zero based index of the current sort column.
	about: There must be at least one segment/column to successfully call this method.
	End Rem
	Method getSortColumn:Int()
		Return bmx_cegui_listheader_getsortcolumn(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the zero based column index of the segment with the specified text.
	End Rem
	Method getColumnWithText:Int(text:String)
		Return bmx_cegui_listheader_getcolumnwithtext(objectPtr, _convertMaxToUTF8(text))
	End Method
	 
	Rem
	bbdoc: Returns the pixel offset to the given ListHeaderSegment.
	End Rem
	Method getPixelOffsetToSegment:Float(segment:TCEListHeaderSegment)
		Return bmx_cegui_listheader_getpixeloffsettosegment(objectPtr, segment.objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the pixel offset to the ListHeaderSegment at the given zero based column index.
	End Rem
	Method getPixelOffsetToColumn:Float(column:Int)
		Return bmx_cegui_listheader_getpixeloffsettocolumn(objectPtr, column)
	End Method
	 
	Rem
	bbdoc: Returns the total pixel width of all attached segments.
	End Rem
	Method getTotalSegmentsPixelExtent:Float()
		Return bmx_cegui_listheader_gettotalsegmentspixelextent(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the width of the specified column.
	End Rem
	Method getColumnWidth:Float(column:Int)
		Return bmx_cegui_listheader_getcolumnwidth(objectPtr, column)
	End Method
	 
	Rem
	bbdoc: Returns the currently set sort direction.
	End Rem
	Method getSortDirection:Int()
		Return bmx_cegui_listheader_getsortdirection(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether user manipulation of the sort column & direction are enabled.
	End Rem
	Method isSortingEnabled:Int()
		Return bmx_cegui_listheader_issortingenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the user may size column segments.
	End Rem
	Method isColumnSizingEnabled:Int()
		Return bmx_cegui_listheader_iscolumnsizingenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns whether the user may modify the order of the segments.
	End Rem
	Method isColumnDraggingEnabled:Int()
		Return bmx_cegui_listheader_iscolumndraggingenabled(objectPtr)
	End Method
	 
	Rem
	bbdoc: Returns the current segment offset value.
	about: This value is used to implement scrolling of the header segments within the ListHeader area.
	End Rem
	Method getSegmentOffset:Float()
		Return bmx_cegui_listheader_getsegmentoffset(objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets whether user manipulation of the sort column and direction is enabled.
	End Rem
	Method setSortingEnabled(setting:Int)
		bmx_cegui_listheader_setsortingenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets the current sort direction.
	End Rem
	Method setSortDirection(direction:Int)
		bmx_cegui_listheader_setsortdirection(objectPtr, direction)
	End Method
	 
	Rem
	bbdoc: Sets the column segment to be used as the sort column.
	End Rem
	Method setSortSegment(segment:TCEListHeaderSegment)
		bmx_cegui_listheader_setsortsegment(objectPtr, segment.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the column to be used as the sort column.
	End Rem
	Method setSortColumn(column:Int)
		bmx_cegui_listheader_setsortcolumn(objectPtr, column)
	End Method
	 
	Rem
	bbdoc: Sets the column to to be used for sorting via its ID code.
	End Rem
	Method setSortColumnFromID(id:Int)
		bmx_cegui_listheader_setsortcolumnfromid(objectPtr, id)
	End Method
	 
	Rem
	bbdoc: Sets whether columns may be sized by the user.
	End Rem
	Method setColumnSizingEnabled(setting:Int)
		bmx_cegui_listheader_setcolumnsizingenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Sets whether columns may be reordered by the user via drag and drop.
	End Rem
	Method setColumnDraggingEnabled(setting:Int)
		bmx_cegui_listheader_setcolumndraggingenabled(objectPtr, setting)
	End Method
	 
	Rem
	bbdoc: Adds a new column segment to the end of the header.
	End Rem
	Method addColumn(text:String, id:Int, width:Float)
		bmx_cegui_listheader_addcolumn(objectPtr, _convertMaxToUTF8(text), id, width)
	End Method
	 
	Rem
	bbdoc: Inserts a new column segment at the specified position.
	End Rem
	Method insertColumn(text:String, id:Int, width:Float, position:Int)
		bmx_cegui_listheader_insertcolumn(objectPtr, _convertMaxToUTF8(text), id, width, position)
	End Method
	 
	Rem
	bbdoc: Inserts a new column segment at the specified position.
	End Rem
	Method insertColumnAtSegment(text:String, id:Int, width:Float, position:TCEListHeaderSegment)
		bmx_cegui_listheader_insertcolumnatsegment(objectPtr, _convertMaxToUTF8(text), id, width, position.objectPtr)
	End Method
	 
	Rem
	bbdoc: Removes a column segment from the ListHeader.
	End Rem
	Method removeColumn(column:Int)
		bmx_cegui_listheader_removecolumn(objectPtr, column)
	End Method
	 
	Rem
	bbdoc: Removes the specified segment from the ListHeader.
	End Rem
	Method removeSegment(segment:TCEListHeaderSegment)
		bmx_cegui_listheader_removesegment(objectPtr, segment.objectPtr)
	End Method
	 
	Rem
	bbdoc: Moves a column segment into a new position.
	End Rem
	Method moveColumn(column:Int, position:Int)
		bmx_cegui_listheader_movecolumn(objectPtr, column, position)
	End Method
	 
	Rem
	bbdoc: Moves a column segment to a new position.
	End Rem
	Method moveColumnAtSegment(column:Int, position:TCEListHeaderSegment)
		bmx_cegui_listheader_movecolumnatsegment(objectPtr, column, position.objectPtr)
	End Method
	 
	Rem
	bbdoc: Moves a segment into a new position.
	End Rem
	Method moveSegment(segment:TCEListHeaderSegment, position:Int)
		bmx_cegui_listheader_movesegment(objectPtr, segment.objectPtr, position)
	End Method
	 
	Rem
	bbdoc: Moves a segment to a new position.
	End Rem
	Method moveSegmentAtSegment(segment:TCEListHeaderSegment, position:TCEListHeaderSegment)
		bmx_cegui_listheader_movesegmentatsegment(objectPtr, segment.objectPtr, position.objectPtr)
	End Method
	 
	Rem
	bbdoc: Sets the current base segment offset.
	about: This implements scrolling of the header segments within the header area.
	End Rem
	Method setSegmentOffset(offset:Float)
		bmx_cegui_listheader_setsegmentoffset(objectPtr, offset)
	End Method
	 
	Rem
	bbdoc: Sets the width of the specified column.
	End Rem
	Method setColumnWidth(column:Int, width:Float)
		bmx_cegui_listheader_setcolumnwidth(objectPtr, column, width)
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
	bbdoc: Sets the height of the tabs. 
	End Rem
	Method setTabHeightU(height:TCEUDim)
		bmx_cegui_tabcontrol_settabheightu(objectPtr, height.objectPtr)
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
bbdoc: Base type for standard Tree widget. 
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
	Rem
	bbdoc: Event triggered when a branch of the tree is opened by the user. 
	End Rem
	Const EventBranchOpened:String = "BranchOpened"
	Rem
	bbdoc: Event triggered when a branch of the tree is closed by the user. 
	End Rem
	Const EventBranchClosed:String = "BranchClosed"

	Rem
	bbdoc: Returns number of items attached to the list box. 
	End Rem
	Method getItemCount:Int()
		Return bmx_cegui_tree_getitemcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of selected items in the list box. 
	End Rem
	Method getSelectedCount:Int()
		Return bmx_cegui_tree_getselectedcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the first selected item. 
	End Rem
	Method getFirstSelectedItem:TCETreeItem()
		Return TCETreeItem(bmx_cegui_tree_getfirstselecteditem(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the last selected item. 
	End Rem
	Method getLastSelectedItem:TCETreeItem()
		Return TCETreeItem(bmx_cegui_tree_getlastselecteditem(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the next selected item after item @startItem. 
	End Rem
	Method getNextSelected:TCETreeItem(startItem:TCETreeItem)
		Return TCETreeItem(bmx_cegui_tree_getnextselected(objectPtr, startItem.objectPtr))
	End Method
	
	'Method getNextSelectedItemFromList:TCETreeItem(Const LBItemList &itemList, start_item:TCETreeItem, bool foundStartItem:Int)
	'End Method
	
	Rem
	bbdoc: Returns whether list sorting is enabled 
	End Rem
	Method isSortEnabled:Int()
		Return bmx_cegui_tree_issortenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setItemRenderArea(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_tree_setitemrenderarea(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_tree_getvertscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getHorzScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_tree_gethorzscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns whether multi-select is enabled 
	End Rem
	Method isMultiselectEnabled:Int()
		Return bmx_cegui_tree_ismultiselectenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isItemTooltipsEnabled:Int()
		Return bmx_cegui_tree_isitemtooltipsenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Searches the list for an item with the specified text. 
	End Rem
	Method findFirstItemWithText:TCETreeItem(text:String)
		Return TCETreeItem(bmx_cegui_tree_findfirstitemwithtext(objectPtr, _convertMaxToUTF8(text)))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method findNextItemWithText:TCETreeItem(text:String, startItem:TCETreeItem)
		Return TCETreeItem(bmx_cegui_tree_findnextitemwithtext(objectPtr, _convertMaxToUTF8(text), startItem.objectPtr))
	End Method
	
	'Method findItemWithTextFromList:TCETreeItem(Const LBItemList &itemList, String &text:String, start_item:TCETreeItem, foundStartItem:Int)
	'End Method
	
	Rem
	bbdoc: Searches the list for an item with the specified ID. 
	End Rem
	Method findFirstItemWithID:TCETreeItem(searchID:Int)
		Return TCETreeItem(bmx_cegui_tree_findfirstitemwithid(objectPtr, searchID))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method findNextItemWithID:TCETreeItem(searchID:Int, startItem:TCETreeItem)
		Return TCETreeItem(bmx_cegui_tree_findnextitemwithid(objectPtr, searchID, startItem.objectPtr))
	End Method
	
	'Method findItemWithIDFromList:TCETreeItem(Const LBItemList &itemList, uint searchID:Int, start_item:TCETreeItem, foundStartItem:Int)
	'End Method
	
	Rem
	bbdoc: Returns whether the specified TreeItem is in the List. 
	End Rem
	Method isTreeItemInList:Int(item:TCETreeItem)
		Return bmx_cegui_tree_istreeiteminlist(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown. 
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_tree_isvertscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the horizontal scroll bar is always shown. 
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_tree_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Removes all items from the list.
	about: Note that this will cause 'AutoDelete' items to be deleted. 
	End Rem
	Method resetList()
		bmx_cegui_tree_resetlist(objectPtr)
	End Method
	
	Rem
	bbdoc: Adds the given TreeItem to the list. 
	End Rem
	Method addItem(item:TCETreeItem)
		bmx_cegui_tree_additem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Inserts an item into the list box after a specified item already in the list.
	about: Note that if the list is sorted, the item may not end up in the requested position.
	End Rem
	Method insertItem(item:TCETreeItem, position:TCETreeItem)
		bmx_cegui_tree_insertitem(objectPtr, item.objectPtr, position.objectPtr)
	End Method
	
	Rem
	bbdoc: Removes the given item from the list box.
	about: If the item is has the auto delete state set, the item will be deleted. 
	End Rem
	Method removeItem(item:TCETreeItem)
		bmx_cegui_tree_removeitem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Clear the selected state for all items. 
	End Rem
	Method clearAllSelections()
		bmx_cegui_tree_clearallselections(objectPtr)
	End Method
	
	'Method clearAllSelectionsFromList:Int(Const LBItemList &itemList)
	'End Method
	
	Rem
	bbdoc: Set whether the list should be sorted. 
	End Rem
	Method setSortingEnabled(setting:Int)
		bmx_cegui_tree_setsortingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the list should allow multiple selections or just a single selection. 
	End Rem
	Method setMultiselectEnabled(setting:Int)
		bmx_cegui_tree_setmultiselectenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown. 
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_tree_setshowvertscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the horizontal scroll bar should always be shown. 
	End Rem
	Method setShowHorzScrollbar(setting:Int)
		bmx_cegui_tree_setshowhorzscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setItemTooltipsEnabled(setting:Int)
		bmx_cegui_tree_setitemtooltipsenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Set the select state of an attached TreeItem.
	about:  This is the recommended way of selecting and deselecting items attached to a list box as it
	respects the multi-select mode setting. It is possible to modify the setting on TreeItems directly,
	but that approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectState(item:TCETreeItem, state:Int)
		bmx_cegui_tree_setitemselectstate(objectPtr, item.objectPtr, state)
	End Method
	
	Rem
	bbdoc: Set the select state of an attached TreeItem.
	about: This is the recommended way of selecting and deselecting items attached to a list box as it
	respects the multi-select mode setting. It is possible to modify the setting on TreeItems directly,
	but that approach does not respect the settings of the list box.
	End Rem
	Method setItemSelectStateIndex(itemIndex:Int, state:Int)
		bmx_cegui_tree_setitemselectstateindex(objectPtr, itemIndex, state)
	End Method
	
	Rem
	bbdoc: Set the LookNFeel that shoule be used for this window. 
	End Rem
	Method setLookNFeel(look:String)
		bmx_cegui_tree_setlooknfeel(objectPtr, _convertMaxToUTF8(look))
	End Method
	
	Rem
	bbdoc: Causes the list box to update it's internal state after changes have been made to one or more attached TreeItem objects.
	about: Client code must call this whenever it has made any changes to TreeItem objects already
	attached to the list box. If you are just adding items, or removed items to update them prior
	to re-adding them, there is no need to call this method.
	End Rem
	Method handleUpdatedItemData()
		bmx_cegui_tree_handleupdateditemdata(objectPtr)
	End Method
	
	Rem
	bbdoc: Ensure the item at the specified index is visible within the list box. 
	End Rem
	Method ensureItemIsVisible(item:TCETreeItem)
		bmx_cegui_tree_ensureitemisvisible(objectPtr, item.objectPtr)
	End Method
	

End Type

Rem
bbdoc: Base type for list box items.
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

	Rem
	bbdoc: TODO
	End Rem
	Method getFont:TCEFont()
	End Method
	
	Rem
	bbdoc: TODO
	End Rem
	Method getTextColours:TCEColourRect()
	End Method
	
	Rem
	bbdoc: TODO
	End Rem
	Method setFont(font:Object)
		If TCEFont(font) Then
		
		ElseIf String(font) Then
		
		End If
	End Method
	
	Rem
	bbdoc: Sets the colours used for text rendering.
	End Rem
	Method setTextColours(cols:TCEColourRect)
		bmx_cegui_treeitem_settextcolours(objectPtr, cols.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for text rendering.
	End Rem
	Method setTextColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
		bmx_cegui_treeitem_settextcoloursforcorners(objectPtr, topLeftColour.objectPtr, topRightColour.objectPtr, bottomLeftColour.objectPtr, bottomRightColour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for text rendering.
	End Rem
	Method setTextColoursForColour(col:TCEColour)
		bmx_cegui_treeitem_settextcoloursforcolour(objectPtr, col.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the text string set for this list box item.
	about: Note that even if the item does not render text, the text string can still be useful, since it is used for
	sorting list box items.
	End Rem
	Method getText:String()
		Return bmx_cegui_treeitem_gettext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the tooltip text.
	End Rem
	Method getTooltipText:String()
		Return bmx_cegui_treeitem_gettooltiptext(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current ID assigned to this list box item.
	about: Note that the system does not make use of this value, client code can assign any meaning it wishes to the ID.
	End Rem
	Method getID:Int()
		Return bmx_cegui_treeitem_getid(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the reference to any client assigned user data attached to this lis box item.
	End Rem
	Method getUserData:Object()
		Return bmx_cegui_treeitem_getuserdata(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item is selected.
	End Rem
	Method isSelected:Int()
		Return bmx_cegui_treeitem_isselected(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item is disabled.
	End Rem
	Method isDisabled:Int()
		Return bmx_cegui_treeitem_isdisabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether this item will be automatically deleted when the list box it is attached to is destroyed, or when the item is removed from the list box.
	End Rem
	Method isAutoDeleted:Int()
		Return bmx_cegui_treeitem_isautodeleted(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the owner window for this TreeItem.
	about: The owner of a TreeItem is typically set by the list box widgets when an item is added or inserted.
	End Rem
	Method getOwnerWindow:TCEWindow()
		Return TCEWindow(bmx_cegui_treeitem_getownerwindow(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current colours used for selection highlighting.
	End Rem
	Method getSelectionColours:TCEColourRect()
		Return TCEColourRect._create(bmx_cegui_treeitem_getselectioncolours(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the current selection highlighting brush.
	End Rem
	Method getSelectionBrushImage:TCEImage()
		Return TCEImage._create(bmx_cegui_treeitem_getselectionbrushimage(objectPtr))
	End Method
	
	Rem
	bbdoc: Sets the text string for this list box item.
	about: Note that even if the item does not render text, the text string can still be useful, since it is used for 
	sorting list box items.
	End Rem
	Method setText(text:String)
		bmx_cegui_treeitem_settext(objectPtr, _convertMaxToUTF8(text))
	End Method
	
	Rem
	bbdoc: Sets the tooltip text.
	End Rem
	Method setTooltipText(text:String)
		bmx_cegui_treeitem_settooltiptext(objectPtr, _convertMaxToUTF8(text))
	End Method
	
	Rem
	bbdoc: Sets the ID assigned to this list box item.
	about: Note that the system does not make use of this value, client code can assign any meaning it wishes to the ID.
	End Rem
	Method setID(itemId:Int)
		bmx_cegui_treeitem_setid(objectPtr, itemId)
	End Method
	
	Rem
	bbdoc: Sets the client assigned user data attached to this lis box item.
	End Rem
	Method setUserData(itemData:Object)
		bmx_cegui_treeitem_setuserdata(objectPtr, itemData)
	End Method
	
	Rem
	bbdoc: Sets whether this item is selected.
	End Rem
	Method setSelected(setting:Int)
		bmx_cegui_treeitem_setselected(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this item is disabled.
	End Rem
	Method setDisabled(setting:Int)
		bmx_cegui_treeitem_setdisabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether this item will be automatically deleted when the list box it is attached to is destroyed, or when the item is removed from the list box.
	End Rem
	Method setAutoDeleted(setting:Int)
		bmx_cegui_treeitem_setautodeleted(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Set the owner window for this TreeItem.
	about: This is called by all the list box widgets when an item is added or inserted.
	End Rem
	Method setOwnerWindow(owner:TCEWindow)
		bmx_cegui_treeitem_setownerwindow(objectPtr, owner.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting.
	End Rem
	Method setSelectionColours(cols:TCEColourRect)
		bmx_cegui_treeitem_setselectioncolours(objectPtr, cols.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting.
	End Rem
	Method setSelectionColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
		bmx_cegui_treeitem_setselectioncoloursforcorners(objectPtr, topLeftColour.objectPtr, topRightColour.objectPtr, bottomLeftColour.objectPtr, bottomRightColour.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the colours used for selection highlighting.
	End Rem
	Method setSelectionColoursForColour(col:TCEColour)
		bmx_cegui_treeitem_setselectioncoloursforcolour(objectPtr, col.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the selection highlighting brush image.
	End Rem
	Method setSelectionBrushImage(image:TCEImage)
		bmx_cegui_treeitem_setselectionbrushimage(objectPtr, image.objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the selection highlighting brush image.
	End Rem
	Method setSelectionBrushImageByName(imageset:String, image:String)
		bmx_cegui_treeitem_setselectionbrushimagebyname(objectPtr, _convertMaxToUTF8(imageset), _convertMaxToUTF8(image))
	End Method
	
	Rem
	bbdoc: Tells the treeItem where its button is located.
	End Rem
	Method setButtonLocation(x:Float, y:Float, w:Float, h:Float)
		bmx_cegui_treeitem_setbuttonlocation(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getButtonLocation(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_treeitem_getbuttonlocation(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getIsOpen:Int()
		Return bmx_cegui_treeitem_getisopen(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method toggleIsOpen()
		bmx_cegui_treeitem_toggleisopen(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getTreeItemFromIndex:TCETreeItem(itemIndex:Int)
		Return TCETreeItem(bmx_cegui_treeitem_gettreeitemfromindex(objectPtr, itemIndex))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getItemCount:Int()
		Return bmx_cegui_treeitem_getitemcount(objectPtr)
	End Method
	
	'LBItemList & 	getItemList()
	
	Rem
	bbdoc: 
	End Rem
	Method addItem(item:TCETreeItem)
		bmx_cegui_treeitem_additem(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setIcon(theIcon:TCEImage)
		bmx_cegui_treeitem_seticon(objectPtr, theIcon.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the rendered pixel size of this list box item.
	End Rem
	Method getPixelSize(width:Float Var, height:Float Var) 
		bmx_cegui_treeitem_getpixelsize(objectPtr, Varptr width, Varptr height)
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
	
	Const SELECT_ROWSINGLE:Int = 0
	Const SELECT_ROWMULTIPLE:Int = 1
	Const SELECT_CELLSINGLE:Int = 2
	Const SELECT_CELLMULTIPLE:Int = 3
	Const SELECT_NOMINATEDCOLUMNSINGLE:Int = 4
	Const SELECT_NOMINATEDCOLUMNMULTIPLE:Int = 5
	Const SELECT_COLUMNSINGLE:Int = 6
	Const SELECT_COLUMNMULTIPLE:Int = 7
	Const SELECT_NOMINATEDROWSINGLE:Int = 8
	Const SELECT_NOMINATEDROWMULTIPLE:Int = 9

	Rem
	bbdoc: Event fired when the selection mode for the list box changes. 
	End Rem
	Const EventSelectionModeChanged:String = "SelectModeChanged"
	Rem
	bbdoc: Event fired when the nominated select column changes. 
	End Rem
	Const EventNominatedSelectColumnChanged:String = "NomSelColChanged"
	Rem
	bbdoc: Event fired when the nominated select row changes. 
	End Rem
	Const EventNominatedSelectRowChanged:String = "NomSelRowChanged"
	Rem
	bbdoc: Event fired when the vertical scroll bar 'force' setting changes. 
	End Rem
	Const EventVertScrollbarModeChanged:String = "VertBarModeChanged"
	Rem
	bbdoc: Event fired when the horizontal scroll bar 'force' setting changes. 
	End Rem
	Const EventHorzScrollbarModeChanged:String = "HorzBarModeChanged"
	Rem
	bbdoc: Event fired when the current selection(s) within the list box changes. 
	End Rem
	Const EventSelectionChanged:String = "SelectionChanged"
	Rem
	bbdoc: Event fired when the contents of the list box changes. 
	End Rem
	Const EventListContentsChanged:String = "ContentsChanged"
	Rem
	bbdoc: Event fired when the sort column changes. 
	End Rem
	Const EventSortColumnChanged:String = "SortColChanged"
	Rem
	bbdoc: Event fired when the sort direction changes. 
	End Rem
	Const EventSortDirectionChanged:String = "SortDirChanged"
	Rem
	bbdoc: Event fired when the width of a column in the list changes. 
	End Rem
	Const EventListColumnSized:String = "ColSized"
	Rem
	bbdoc: Event fired when the column order changes. 
	End Rem
	Const EventListColumnMoved:String = "ColMoved"

	Rem
	bbdoc: Returns whether user manipulation of the sort column and direction are enabled.
	End Rem
	Method isUserSortControlEnabled:Int()
		Return bmx_cegui_multicolumnlist_isusersortcontrolenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the user may size column segments.
	End Rem
	Method isUserColumnSizingEnabled:Int()
		Return bmx_cegui_multicolumnlist_isusercolumnsizingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the user may modify the order of the columns.
	End Rem
	Method isUserColumnDraggingEnabled:Int()
		Return bmx_cegui_multicolumnlist_isusercolumndraggingenabled(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of columns in the multi-column list.
	End Rem
	Method getColumnCount:Int()
		Return bmx_cegui_multicolumnlist_getcolumncount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of rows in the multi-column list.
	End Rem
	Method getRowCount:Int()
		Return bmx_cegui_multicolumnlist_getrowcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the zero based index of the current sort column.
	about: There must be at least one column to successfully call this method.
	End Rem
	Method getSortColumn:Int()
		Return bmx_cegui_multicolumnlist_getsortcolumn(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the zero based column index of the column with the specified ID.
	End Rem
	Method getColumnWithID:Int(colId:Int)
		Return bmx_cegui_multicolumnlist_getcolumnwithid(objectPtr, colId)
	End Method
	
	Rem
	bbdoc: Returns the zero based index of the column whos header text matches the specified text.
	End Rem
	Method getColumnWithHeaderText:Int(text:String)
		Return bmx_cegui_multicolumnlist_getcolumnwithheadertext(objectPtr, _convertMaxToUTF8(text))
	End Method
	
	Rem
	bbdoc: Returns the total width of all column headers.
	End Rem
	Method getTotalColumnHeadersWidth:Float()
		Return bmx_cegui_multicolumnlist_gettotalcolumnheaderswidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the width of the specified column header (and therefore the column itself).
	End Rem
	Method getColumnHeaderWidth:Float(colIdx:Int)
		Return bmx_cegui_multicolumnlist_getcolumnheaderwidth(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Returns the currently set sort direction.
	about: One of TCEListHeaderSegment.SORT_NONE, TCEListHeaderSegment.SORT_ASCENDING or TCEListHeaderSegment.SORT_DESCENDING.
	End Rem
	Method getSortDirection:Int()
		Return bmx_cegui_multicolumnlist_getsortdirection(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the TCEListHeaderSegment object for the specified column.
	End Rem
	Method getHeaderSegmentForColumn:TCEListHeaderSegment(colIdx:Int)
		Return TCEListHeaderSegment(bmx_cegui_multicolumnlist_getheadersegmentforcolumn(objectPtr, colIdx))
	End Method
	
	Rem
	bbdoc: Returns the zero based index of the Row that contains @item.
	End Rem
	Method getItemRowIndex:Int(item:TCEListboxItem)
		Return bmx_cegui_multicolumnlist_getitemrowindex(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the current zero based index of the column that contains @item.
	End Rem
	Method getItemColumnIndex:Int(item:TCEListboxItem)
		Return bmx_cegui_multicolumnlist_getitemcolumnindex(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the grid reference for @item.
	End Rem
	Method getItemGridReference(item:TCEListboxItem, row:Int Var, col:Int Var)
		bmx_cegui_multicolumnlist_getitemgridreference(objectPtr, item.objectPtr, Varptr row, Varptr col)
	End Method
	
	Rem
	bbdoc: Returns the ListboxItem at the specified grid reference.
	End Rem
	Method getItemAtGridReference:TCEListboxItem(row:Int, col:Int)
		Return TCEListboxItem(bmx_cegui_multicolumnlist_getitematgridreference(objectPtr, row, col))
	End Method
	
	Rem
	bbdoc: Returns whether ListboxItem @item is attached to the column at index @colIdx.
	End Rem
	Method isListboxItemInColumn:Int(item:TCEListboxItem, colIdx:Int)
		Return bmx_cegui_multicolumnlist_islistboxitemincolumn(objectPtr, item.objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Returns whether ListboxItem @item is attached to the row at index @rowIdx.
	End Rem
	Method isListboxItemInRow:Int(item:TCEListboxItem, rowIdx:Int)
		Return bmx_cegui_multicolumnlist_islistboxiteminrow(objectPtr, item.objectPtr, rowIdx)
	End Method
	
	Rem
	bbdoc: Returns whether ListboxItem @item is attached to the list box.
	End Rem
	Method isListboxItemInList:Int(item:TCEListboxItem)
		Return bmx_cegui_multicolumnlist_islistboxiteminlist(objectPtr, item.objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the ListboxItem in column colIdx that has the text string @text.
	End Rem
	Method findColumnItemWithText:TCEListboxItem(text:String, colIdx:Int, startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_multicolumnlist_findcolumnitemwithtext(objectPtr, _convertMaxToUTF8(text), colIdx, startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the ListboxItem in row @rowIdx that has the text string @text.
	End Rem
	Method findRowItemWithText:TCEListboxItem(text:String, rowIdx:Int, startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_multicolumnlist_findrowitemwithtext(objectPtr, _convertMaxToUTF8(text), rowIdx, startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the ListboxItem that has the text string @text.
	End Rem
	Method findListItemWithText:TCEListboxItem(text:String, startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_multicolumnlist_findlistitemwithtext(objectPtr, _convertMaxToUTF8(text), startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the first selected ListboxItem attached to this list box.
	End Rem
	Method getFirstSelectedItem:TCEListboxItem()
		Return TCEListboxItem(bmx_cegui_multicolumnlist_getfirstselecteditem(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the next selected ListboxItem after @startItem.
	End Rem
	Method getNextSelected:TCEListboxItem(startItem:TCEListboxItem)
		Return TCEListboxItem(bmx_cegui_multicolumnlist_getnextselected(objectPtr, startItem.objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the number of selected ListboxItems attached to this list box.
	End Rem
	Method getSelectedCount:Int()
		Return bmx_cegui_multicolumnlist_getselectedcount(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the ListboxItem at the grid reference is selected.
	End Rem
	Method isItemSelected:Int(row:Int, col:Int)
		Return bmx_cegui_multicolumnlist_isitemselected(objectPtr, row, col)
	End Method
	
	Rem
	bbdoc: Returns the ID of the currently set nominated selection column to be used when in one of the NominatedColumn* selection modes.
	End Rem
	Method getNominatedSelectionColumnID:Int()
		Return bmx_cegui_multicolumnlist_getnominatedselectioncolumnid(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the index of the currently set nominated selection column to be used when in one of the NominatedColumn* selection modes.
	End Rem
	Method getNominatedSelectionColumn:Int()
		Return bmx_cegui_multicolumnlist_getnominatedselectioncolum(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the index of the currently set nominated selection row to be used when in one of the NominatedRow* selection modes.
	End Rem
	Method getNominatedSelectionRow:Int()
		Return bmx_cegui_multicolumnlist_getnominatedselectionrow(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the currently set selection mode.
	End Rem
	Method getSelectionMode:Int()
		Return bmx_cegui_multicolumnlist_getselectionmode(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the vertical scroll bar is always shown.
	End Rem
	Method isVertScrollbarAlwaysShown:Int()
		Return bmx_cegui_multicolumnlist_isvertscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns whether the horizontal scroll bar is always shown.
	End Rem
	Method isHorzScrollbarAlwaysShown:Int()
		Return bmx_cegui_multicolumnlist_ishorzscrollbaralwaysshown(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the ID code assigned to the requested column.
	End Rem
	Method getColumnID:Int(colIdx:Int)
		Return bmx_cegui_multicolumnlist_getcolumnid(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Returns the ID code assigned to the requested row.
	End Rem
	Method getRowID:Int(rowIdx:Int)
		Return bmx_cegui_multicolumnlist_getrowid(objectPtr, rowIdx)
	End Method
	
	Rem
	bbdoc: Return the zero based row index of the row with the specified ID.
	End Rem
	Method getRowWithID:Int(rowId:Int)
		Return bmx_cegui_multicolumnlist_getrowwithid(objectPtr, rowId)
	End Method

	Rem
	bbdoc: Return the rectangular area describing, in un-clipped pixels, the window relative area that is to be used for rendering list items.
	End Rem
	Method getListRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
		bmx_cegui_multicolumnlist_getlistrenderarea(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Returns the vertical scrollbar component widget for this MultiColumnList.
	End Rem
	Method getVertScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_multicolumnlist_getvertscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the horizontal scrollbar component widget for this MultiColumnList.
	End Rem
	Method getHorzScrollbar:TCEScrollbar()
		Return TCEScrollbar(bmx_cegui_multicolumnlist_gethorzscrollbar(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the list header component widget for this MultiColumnList.
	End Rem
	Method getListHeader:TCEListHeader()
		Return TCEListHeader(bmx_cegui_multicolumnlist_getlistheader(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns the sum of all row heights in pixels. 
	End Rem
	Method getTotalRowsHeight:Float()
		Return bmx_cegui_multicolumnlist_gettotalrowsheight(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the pixel width of the widest item in the given column. 
	End Rem
	Method getWidestColumnItemWidth:Float(colIdx:Int)
		Return bmx_cegui_multicolumnlist_getwidestcolumnitemwidth(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Returns, in pixels, the height of the highest item in the given row. 
	End Rem
	Method getHighestRowItemHeight:Float(rowIdx:Int)
		Return bmx_cegui_multicolumnlist_gethighestrowitemheight(objectPtr, rowIdx)
	End Method
	
	Rem
	bbdoc: Remove all items from the list.
	about: Note that this will cause 'AutoDelete' items to be deleted.
	End Rem
	Method resetList()
		bmx_cegui_multicolumnlist_resetlist(objectPtr)
	End Method
	
	Rem
	bbdoc: Add a column to the list box.
	End Rem
	Method addColumn(text:String, colId:Int, width:Float)
		bmx_cegui_multicolumnlist_addcolumn(objectPtr, _convertMaxToUTF8(text), colId, width)
	End Method

	Rem
	bbdoc: Add a column to the list box.
	End Rem
	Method addColumnU(text:String, colId:Int, width:TCEUDim)
		bmx_cegui_multicolumnlist_addcolumnu(objectPtr, _convertMaxToUTF8(text), colId, width.objectPtr)
	End Method
	
	Rem
	bbdoc: Insert a new column in the list.
	End Rem
	Method insertColumn(text:String, colId:Int, width:Float, position:Int)
		bmx_cegui_multicolumnlist_insertcolumn(objectPtr, _convertMaxToUTF8(text), colId, width, position)
	End Method
	
	Rem
	bbdoc: Insert a new column in the list.
	End Rem
	Method insertColumnU(text:String, colId:Int, width:TCEUDim, position:Int)
		bmx_cegui_multicolumnlist_insertcolumnu(objectPtr, _convertMaxToUTF8(text), colId, width.objectPtr, position)
	End Method

	Rem
	bbdoc: Removes a column from the list box.
	about: This will cause any ListboxItem using the autoDelete option in the column to be deleted.
	End Rem
	Method removeColumn(colIdx:Int)
		bmx_cegui_multicolumnlist_removecolumn(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Removes a column from the list box.
	about: This will cause any ListboxItem using the autoDelete option in the column to be deleted.
	End Rem
	Method removeColumnWithID(colId:Int)
		bmx_cegui_multicolumnlist_removecolumnwithid(objectPtr, colId)
	End Method
	
	Rem
	bbdoc: Moves the column at index @colIdx so it is at index @position.
	End Rem
	Method moveColumn(colIdx:Int, position:Int)
		bmx_cegui_multicolumnlist_movecolumn(objectPtr, colIdx, position)
	End Method
	
	Rem
	bbdoc: Moves the column with ID @colId so it is at index @position.
	End Rem
	Method moveColumnWithID(colId:Int, position:Int)
		bmx_cegui_multicolumnlist_movecolumnwithid(objectPtr, colId, position)
	End Method
	
	Rem
	bbdoc: Adds an empty row to the list box.
	about: If the list is being sorted, the new row will appear at an appropriate position according to
	the sorting being applied. If no sorting is being done, the new row will appear at the bottom
	of the list.
	End Rem
	Method addRow:Int(rowId:Int = 0)
		Return bmx_cegui_multicolumnlist_addrow(objectPtr, rowId)
	End Method
	
	Rem
	bbdoc: Adds a row to the list box, and set the item in the column with ID @colId to @item.
	about: If the list is being sorted, the new row will appear at an appropriate position
	according to the sorting being applied. If no sorting is being done, the new row will appear
	at the bottom of the list.
	End Rem
	Method addRowItem:Int(item:TCEListboxItem, colId:Int, rowId:Int = 0)
		item.owner = False
		Return bmx_cegui_multicolumnlist_addrowitem(objectPtr, item.objectPtr, colId, rowId)
	End Method
	
	Rem
	bbdoc: Inserts an empty row into the list box.
	about: If the list is being sorted, the new row will appear at an appropriate position according
	to the sorting being applied. If no sorting is being done, the new row will appear at the specified
	index.
	End Rem
	Method insertRow:Int(rowIdx:Int, rowId:Int = 0)
		Return bmx_cegui_multicolumnlist_insertrow(objectPtr, rowIdx, rowId)
	End Method
	
	Rem
	bbdoc: Inserts a row into the list box, and set the item in the column with ID @colId to @item.
	End Rem
	Method insertRowItem:Int(item:TCEListboxItem, colId:Int, rowIdx:Int, rowId:Int = 0)
		item.owner = False
		Return bmx_cegui_multicolumnlist_insertrowitem(objectPtr, item.objectPtr, colId, rowIdx, rowId)
	End Method
	
	Rem
	bbdoc: Removes the list box row with index @rowIdx.
	bout: Any ListboxItem in row rowIdx using autoDelete mode will be deleted.
	End Rem
	Method removeRow(rowIdx:Int)
		bmx_cegui_multicolumnlist_removerow(objectPtr, rowIdx)
	End Method
	
	Rem
	bbdoc: Sets the ListboxItem for grid reference position.
	End Rem
	Method setItemForGridRef(item:TCEListboxItem, row:Int, col:Int)
		item.owner = False
		bmx_cegui_multicolumnlist_setitemforgridref(objectPtr, item.objectPtr, row, col)
	End Method
	
	Rem
	bbdoc: Sets the ListboxItem for the column with ID @colId in row @rowIdx.
	End Rem
	Method setItem(item:TCEListboxItem, colId:Int, rowIdx:Int)
		item.owner = False
		bmx_cegui_multicolumnlist_setitem(objectPtr, item.objectPtr, colId, rowIdx)
	End Method
	
	Rem
	bbdoc: Sets the selection mode for the list box.
	End Rem
	Method setSelectionMode(selMode:Int)
		bmx_cegui_multicolumnlist_setselectionmode(objectPtr, selMode)
	End Method
	
	Rem
	bbdoc: Sets the column to be used for the NominatedColumn* selection modes.
	End Rem
	Method setNominatedSelectionColumnID(colId:Int)
		bmx_cegui_multicolumnlist_setnominatedselectioncolumnid(objectPtr, colId)
	End Method
	
	Rem
	bbdoc: Sets the column to be used for the NominatedColumn* selection modes.
	End Rem
	Method setNominatedSelectionColumn(colIdx:Int)
		bmx_cegui_multicolumnlist_setnominatedselectioncolumn(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Sets the row to be used for the NominatedRow* selection modes.
	End Rem
	Method setNominatedSelectionRow(rowIdx:Int)
		bmx_cegui_multicolumnlist_setnominatedselectionrow(objectPtr, rowIdx)
	End Method
	
	Rem
	bbdoc: Sets the sort direction to be used.
	End Rem
	Method setSortDirection(direction:Int)
		bmx_cegui_multicolumnlist_setsortdirection(objectPtr, direction)
	End Method
	
	Rem
	bbdoc: Sets the column to be used as the sort key.
	End Rem
	Method setSortColumn(colIdx:Int)
		bmx_cegui_multicolumnlist_setsortcolumn(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Sets the column to be used as the sort key
	End Rem
	Method setSortColumnByID(colId:Int)
		bmx_cegui_multicolumnlist_setsortcolumnbyid(objectPtr, colId)
	End Method
	
	Rem
	bbdoc: Sets whether the vertical scroll bar should always be shown, or just when needed.
	End Rem
	Method setShowVertScrollbar(setting:Int)
		bmx_cegui_multicolumnlist_setshowvertscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the horizontal scroll bar should always be shown, or just when needed.
	End Rem
	Method setShowHorzScrollbar(setting:Int)
		bmx_cegui_multicolumnlist_setshowhorzscrollbar(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Removes the selected state from any currently selected ListboxItem attached to the list.
	End Rem
	Method clearAllSelections()
		bmx_cegui_multicolumnlist_clearallselections(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets or clears the selected state of the given ListboxItem which must be attached to the list.
	End Rem
	Method setItemSelectState(item:TCEListboxItem, state:Int)
		bmx_cegui_multicolumnlist_setitemselectstate(objectPtr, item.objectPtr, state)
	End Method
	
	Rem
	bbdoc: Sets or clears the selected state of the ListboxItem at the given grid reference.
	End Rem
	Method setItemSelectStateForGridRef(row:Int, col:Int, state:Int)
		bmx_cegui_multicolumnlist_setitemselectstateforgridref(objectPtr, row, col, state)
	End Method
	
	Rem
	bbdoc: Informs the list box that one or more attached ListboxItems have been externally modified, and the list should re-sync its internal state and refresh the display as needed.
	End Rem
	Method handleUpdatedItemData()
		bmx_cegui_multicolumnlist_handleupdateditemdata(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the width of the specified column header (and therefore the column itself).
	End Rem
	Method setColumnHeaderWidth(colIdx:Int, width:Float)
		bmx_cegui_multicolumnlist_setcolumnheaderwidth(objectPtr, colIdx, width)
	End Method
	
	Rem
	bbdoc: Sets whether user manipulation of the sort column and direction are enabled.
	End Rem
	Method setUserSortControlEnabled(setting:Int)
		bmx_cegui_multicolumnlist_setusersortcontrolenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the user may size column segments.
	End Rem
	Method setUserColumnSizingEnabled(setting:Int)
		bmx_cegui_multicolumnlist_setusercolumnsizingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Sets whether the user may modify the order of the columns.
	End Rem
	Method setUserColumnDraggingEnabled(setting:Int)
		bmx_cegui_multicolumnlist_setusercolumndraggingenabled(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: Automatically determines the "best fit" size for the specified column and sets the column width to the same.
	End Rem
	Method autoSizeColumnHeader(colIdx:Int)
		bmx_cegui_multicolumnlist_autosizecolumnheader(objectPtr, colIdx)
	End Method
	
	Rem
	bbdoc: Set the ID code assigned to a given row.
	End Rem
	Method setRowID(rowIdx:Int, rowId:Int)
		bmx_cegui_multicolumnlist_setrowid(objectPtr, rowIdx, rowId)
	End Method

End Type

Rem
bbdoc: Base type for the multi-line edit box widget. 
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
		Return bmx_cegui_editbox_getvalidationstring(objectPtr)
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
	
	Rem
	bbdoc: Sets the utf32 code point used when rendering masked text.
	End Rem
	Method setMaskCodePoint(codePoint:Int)
		bmx_cegui_editbox_setmaskcodepoint(objectPtr, codePoint)
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
	Method setShowVertScrollbar(Mode:Int)
		bmx_cegui_scrolleditemlistbase_setshowvertscrollbar(objectPtr, Mode)
	End Method
	
	Rem
	bbdoc: Sets whether the horizontal scrollbar should be forced visible. Despite content size.
	End Rem
	Method setShowHorzScrollbar(Mode:Int)
		bmx_cegui_scrolleditemlistbase_setshowhorzscrollbar(objectPtr, Mode)
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
