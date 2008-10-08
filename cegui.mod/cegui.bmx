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

	Method setSelected(selected:Int)
	End Method
	
	Method isSelected:Int()
	End Method
	
	Method setTargetWindow(wnd:TCEWindow)
	End Method
	
	Method getTargetWindow:TCEWindow()
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
bbdoc: Base type for list box items. 
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

	Method getText:String()
		Return _convertUTF8ToMax(bmx_cegui_listboxitem_gettext(objectPtr))
	End Method
	
	Method getTooltipText:String()
		Return _convertUTF8ToMax(bmx_cegui_listboxitem_gettooltiptext(objectPtr))
	End Method
	
	Method getID:Int()
		Return bmx_cegui_listboxitem_getid(objectPtr)
	End Method
	
	Method getUserData:Object()
	End Method
	
	Method isSelected:Int()
		Return bmx_cegui_listboxitem_isselected(objectPtr)
	End Method
	
	Method isDisabled:Int()
		Return bmx_cegui_listboxitem_isdisabled(objectPtr)
	End Method
	
	Method isAutoDeleted:Int()
		Return bmx_cegui_listboxitem_isautodeleted(objectPtr)
	End Method
	
	Method getOwnerWindow:TCEWindow()
		Return TCEWindow._create(bmx_cegui_listboxitem_getownerwindow(objectPtr))
	End Method
	
	Method getSelectionColours:TCEColourRect()
	End Method
	
	Method getSelectionBrushImage:TCEImage()
	End Method
	
	Method setText(text:String)
		bmx_cegui_listboxitem_settext(objectPtr, text)
	End Method
	
	Method setTooltipText(text:String)
		bmx_cegui_listboxitem_settooltiptext(objectPtr, text)
	End Method
	
	Method setID(itemId:Int)
		bmx_cegui_listboxitem_setid(objectPtr, itemId)
	End Method
	
	Method setUserData(itemData:Object)
	End Method
	
	Method setSelected(setting:Int)
		bmx_cegui_listboxitem_setselected(objectPtr, setting)
	End Method
	
	Method setDisabled(setting:Int)
		bmx_cegui_listboxitem_setdisabled(objectPtr, setting)
	End Method
	
	Method setAutoDeleted(setting:Int)
		bmx_cegui_listboxitem_setautodeleted(objectPtr, setting)
	End Method
	
	Method setOwnerWindow(owner:TCEWindow)
		bmx_cegui_listboxitem_setownerwindow(objectPtr, owner.objectPtr)
	End Method
	
	Method setSelectionColours(cols:TCEColourRect)
	End Method
	
	Method setSelectionColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	End Method
	
	Method setSelectionColoursForColour(col:TCEColour)
	End Method
	
	Method setSelectionBrushImage(image:TCEImage)
	End Method
	
	Method setSelectionBrushImageByName(imageset:String, image:String)
	End Method
	
	Method getPixelSize(width:Float Var, height:Float Var)
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

	Method getFont:TCEFont()
	End Method
	
	Method getTextColours:TCEColourRect()
	End Method
	
	Method setFont(font:TCEFont)
	End Method
	
	Method setFontByName(font_name:String)
	End Method
	
	Method setTextColours(cols:TCEColourRect)
	End Method
	
	Method setTextColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	End Method
	
	Method setTextColoursForColour(col:TCEColour)
	End Method
	
	Method getPixelSize(width:Float Var, height:Float Var) 
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

	Method getClipArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method getClipperWindow:TCEWindow()
	End Method
	
	Method setClipArea(rx:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method setClipperWindow(w:TCEWindow)
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
	bbdoc: 
	End Rem
	Const EventNamespace:String = "ScrolledContainer"
	Rem
	bbdoc: 
	End Rem
	Const EventContentChanged:String = "ContentChanged"
	Rem
	bbdoc: 
	End Rem
	Const EventAutoSizeSettingChanged:String = "AutoSizeSettingChanged"

	Method isContentPaneAutoSized:Int()
	End Method
	
	Method setContentPaneAutoSized(setting:Int)
	End Method
	
	Method getContentArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method setContentArea(x:Float, y:Float, w:Float, h:Float)
	End Method
	
	Method getChildExtentsArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var) 
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
	
	Method setTextColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	End Method
	
	Method setTextColoursForColour(col:TCEColour)
	End Method
	
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
	
	Method setSelectionColoursForCorners(topLeftColour:TCEColour, topRightColour:TCEColour, bottomLeftColour:TCEColour, bottomRightColour:TCEColour)
	End Method
	
	Method setSelectionColoursForColour(col:TCEColour)
	End Method
	
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

	Method isUserSortControlEnabled:Int()
	End Method
	
	Method isUserColumnSizingEnabled:Int()
	End Method
	
	Method isUserColumnDraggingEnabled:Int()
	End Method
	
	Method getColumnCount:Int()
	End Method
	
	Method getRowCount:Int()
	End Method
	
	Method getSortColumn:Int()
	End Method
	
	Method getColumnWithID:Int(col_id:Int)
	End Method
	
	Method getColumnWithHeaderText:Int(text:String)
	End Method
	
	Method getTotalColumnHeadersWidth:Float()
	End Method
	
	Method getColumnHeaderWidth:Float(col_idx:Int)
	End Method
	
	Method getSortDirection:Int()
	End Method
	
	Method getHeaderSegmentForColumn:TCEListHeaderSegment(col_idx:Int)
	End Method
	
	Method getItemRowIndex:Int(item:TCEListboxItem)
	End Method
	
	Method getItemColumnIndex:Int(item:TCEListboxItem)
	End Method
	
	Method getItemGridReference(item:TCEListboxItem, row:Int Var, col:Int Var)
	End Method
	
	Method getItemAtGridReference:TCEListboxItem(row:Int, col:Int)
	End Method
	
	Method isListboxItemInColumn:Int(item:TCEListboxItem, col_idx:Int)
	End Method
	
	Method isListboxItemInRow:Int(item:TCEListboxItem, row_idx:Int)
	End Method
	
	Method isListboxItemInList:Int(item:TCEListboxItem)
	End Method
	
	Method findColumnItemWithText:TCEListboxItem(text:String, col_idx:Int, start_item:TCEListboxItem)
	End Method
	
	Method findRowItemWithText:TCEListboxItem(text:String, row_idx:Int, start_item:TCEListboxItem)
	End Method
	
	Method findListItemWithText:TCEListboxItem(text:String, start_item:TCEListboxItem)
	End Method
	
	Method getFirstSelectedItem:TCEListboxItem()
	End Method
	
	Method getNextSelected:TCEListboxItem(start_item:TCEListboxItem)
	End Method
	
	Method getSelectedCount:Int()
	End Method
	
	Method isItemSelected:Int(row:Int, col:Int)
	End Method
	
	Method getNominatedSelectionColumnID:Int()
	End Method
	
	Method getNominatedSelectionColumn:Int()
	End Method
	
	Method getNominatedSelectionRow:Int()
	End Method
	
	Method getSelectionMode:Int()
	End Method
	
	Method isVertScrollbarAlwaysShown:Int()
	End Method
	
	Method isHorzScrollbarAlwaysShown:Int()
	End Method
	
	Method getColumnID:Int(col_idx:Int)
	End Method
	
	Method getRowID:Int(row_idx:Int)
	End Method
	
	Method getRowWithID:Int(row_id:Int)
	End Method
	
	Method getListRenderArea(x:Float Var, y:Float Var, w:Float Var, h:Float Var)
	End Method
	
	Method getVertScrollbar:TCEScrollbar()
	End Method
	
	Method getHorzScrollbar:TCEScrollbar()
	End Method
	
	Method getListHeader:TCEListHeader()
	End Method
	
	Method getTotalRowsHeight:Float()
	End Method
	
	Method getWidestColumnItemWidth:Float(col_idx:Int)
	End Method
	
	Method getHighestRowItemHeight:Float(row_idx:Int)
	End Method
	
	Method resetList()
	End Method
	
	Method addColumn(text:String, col_id:Int, width:Float)
	End Method
	
	Method insertColumn(text:String, col_id:Int, width:Float, position:Int)
	End Method
	
	Method removeColumn(col_idx:Int)
	End Method
	
	Method removeColumnWithID(col_id:Int)
	End Method
	
	Method moveColumn(col_idx:Int, position:Int)
	End Method
	
	Method moveColumnWithID(col_id:Int, position:Int)
	End Method
	
	Method addRow:Int(row_id:Int = 0)
	End Method
	
	Method addRowItem:Int(item:TCEListboxItem, col_id:Int, row_id:Int = 0)
	End Method
	
	Method insertRow:Int(row_idx:Int, row_id:Int = 0)
	End Method
	
	Method insertRowItem:Int(item:TCEListboxItem, col_id:Int, row_idx:Int, row_id:Int = 0)
	End Method
	
	Method removeRow(row_idx:Int)
	End Method
	
	Method setItemForGridRef(item:TCEListboxItem, row:Int, col:Int)
	End Method
	
	Method setItem(item:TCEListboxItem, col_id:Int, row_idx:Int)
	End Method
	
	Method setSelectionMode(sel_mode:Int)
	End Method
	
	Method setNominatedSelectionColumnID(col_id:Int)
	End Method
	
	Method setNominatedSelectionColumn(col_idx:Int)
	End Method
	
	Method setNominatedSelectionRow(row_idx:Int)
	End Method
	
	Method setSortDirection(direction:Int)
	End Method
	
	Method setSortColumn(col_idx:Int)
	End Method
	
	Method setSortColumnByID(col_id:Int)
	End Method
	
	Method setShowVertScrollbar(setting:Int)
	End Method
	
	Method setShowHorzScrollbar(setting:Int)
	End Method
	
	Method clearAllSelections()
	End Method
	
	Method setItemSelectState(item:TCEListboxItem, state:Int)
	End Method
	
	Method setItemSelectStateForGridRef(row:Int, col:Int, state:Int)
	End Method
	
	Method handleUpdatedItemData()
	End Method
	
	Method setColumnHeaderWidth(col_idx:Int, width:Float)
	End Method
	
	Method setUserSortControlEnabled(setting:Int)
	End Method
	
	Method setUserColumnSizingEnabled(setting:Int)
	End Method
	
	Method setUserColumnDraggingEnabled(setting:Int)
	End Method
	
	Method autoSizeColumnHeader(col_idx:Int)
	End Method
	
	Method setRowID(row_idx:Int, row_id:Int)
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
