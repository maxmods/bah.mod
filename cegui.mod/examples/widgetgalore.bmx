SuperStrict

Framework BaH.CEGUI
Import BRL.GLGraphics
Import BRL.GLMax2d

Graphics 800, 600, 0

HideMouse

' Initialize CEGUI !
Init_CEGUI()


' Load the TaharezLook scheme and set up the default mouse cursor and font
TCESchemeManager.loadScheme("TaharezLook.scheme")
TCESystem.setDefaultMouseCursor("TaharezLook", "MouseArrow")
If Not TCEFontManager.isFontPresent("Commonwealth-10") Then
	TCEFontManager.createFont("Commonwealth-10.font")
End If

' Set the GUI Sheet
Local sheet:TCEWindow = TCEWindowManager.createWindow("DefaultWindow", "root_wnd")
TCESystem.setGUISheet(sheet)

' Load a layout
Local guiLayout:TCEWindow = TCEWindowManager.loadWindowLayout("WidgetGalore.layout")
sheet.addChildWindow(guiLayout)



' Tooltip
TCESystem.setDefaultTooltip("TaharezLook/Tooltip") ' Set the name of the default tooltip
Local tooltip:TCETooltip = TCESystem.getDefaultTooltip()
tooltip.setHoverTime(0.5) ' Display the tooltip after the mouse has been hovering over the widget for half a second
tooltip.setDisplayTime(10.0) ' Display for 15 seconds then disappear
tooltip.setFadeTime(1.0) ' Duration of the transition between fully visible and fully invisible
' To set the tooltip text for a window simply call setTooltipText()   see staticText for an example

' StaticText
Local staticText:TCEWindow = TCEWindowManager.getWindow("StaticText")
staticText.setText("Red Static Text")
' Colors are specified as aarrggbb in Hexadecimal
' Where aa is alpha, rr is red, gg is green, and bb is blue 
' tl: top left,  tr: top right,  bl: bottom left,  br: bottom right
staticText.setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000")
staticText.setProperty("VertFormatting", "VertCentred") ' TopAligned, BottomAligned, VertCentred
staticText.setProperty("HorzFormatting", "HorzCentred") ' LeftAligned, RightAligned, HorzCentred
	' HorzJustified, WordWrapLeftAligned, WordWrapRightAligned, WordWrapCentred, WordWrapJustified
staticText.setTooltipText("This is a StaticText widget")

' StaticImage
TCEImagesetManager.createImagesetFromImageFile("ImageForStaticImage", "../datafiles/imagesets/GPN-2000-001437.tga")
Local staticImage:TCEWindow = TCEWindowManager.getWindow("StaticImage")
staticImage.setProperty("Image", "set:ImageForStaticImage image:full_image") ' "full_image" is a default name from CEGUIImageset::Imageset()

' ProgressBar
Local progressBar:TCEProgressBar = TCEProgressBar(TCEWindowManager.getWindow("ProgressBar"))
progressBar.setProgress(0.25) ' Initial progress of 25%
progressBar.setStepSize(0.10) ' Calling step() will increase the progress by 10%
progressBar.DoStep() ' Advance the progress by the size specified in setStepSize()
progressBar.adjustProgress(-0.05) ' Adjust the progress by a delta value rather than setting a new value through setProgress
Local valueProgressBar:Float = progressBar.getProgress() ' initial 0.25f + step 0.10f - adjustment 0.05f = 0.30f

' Button
Local btnClose:TCEPushButton = TCEPushButton(TCEWindowManager.getWindow("btnClose"))
btnClose.setText("Exit")

' ScrollablePane
Local scrollablePane:TCEScrollablePane = TCEScrollablePane(TCEWindowManager.getWindow("ScrollablePane"))
TCEImagesetManager.createImagesetFromImageFile("ImageForScrollablePane", "../datafiles/imagesets/GPN-2000-001437.tga")
Local staticImageInScrollablePane:TCEWindow = TCEWindowManager.createWindow("TaharezLook/StaticImage", "StaticImageInScrollablePane")
	staticImageInScrollablePane.setProperty("Image", "set:ImageForScrollablePane image:full_image") ' "full_image" is a default name from CEGUIImageset::Imageset()
	staticImageInScrollablePane.setPositionU(UDim(), UDim()) ' Start in the upper left corner
	staticImageInScrollablePane.setWidthU(UDim(2.0, 0.0)) ' Twice the width of the parent, the ScrollablePane
	staticImageInScrollablePane.setHeightU(UDim(2.0, 0.0)) ' Twice the height of the parent, the ScrollablePane
	scrollablePane.addChildWindow(staticImageInScrollablePane) ' Add the image to the ' Twice the width of the parent, the ScrollablePane
Local editboxInScrollablePane:TCEEditbox = TCEEditbox(TCEWindowManager.createWindow("TaharezLook/Editbox", "EditboxInScrollablePane"))
	editboxInScrollablePane.setPositionU(UDim(), UDim(2.1, 0.0)) ' Start below the image
	editboxInScrollablePane.setWidthU(UDim(2.0, 0.0)) 
	editboxInScrollablePane.setHeightU(UDim(0.3, 0.0))
	scrollablePane.addChildWindow(editboxInScrollablePane)

' Check box
Local checkbox:TCECheckbox = TCECheckbox(TCEWindowManager.getWindow("Checkbox"))
checkbox.setSelected( True )
Local valueCheckbox:Int = checkbox.isSelected() ' Retrieve whether it is checked

' Spinner
Local spinner:TCESpinner = TCESpinner(TCEWindowManager.getWindow("Spinner"))
spinner.setTextInputMode(TCESpinner.INPUT_FLOAT) ' INPUT_FLOAT, INPUT_INTEGER, INPUT_HEX, INPUT_OCTAL
spinner.setMinimumValue(-10.0)
spinner.setMaximumValue(10.0)
spinner.setStepSize(0.2)
spinner.setCurrentValue(5.2)
Local valueSpinner:Float = spinner.getCurrentValue() ' Retrieve the value

' Editbox
Local editbox:TCEEditbox = TCEEditbox(TCEWindowManager.getWindow("Editbox"))
editbox.setText("Editbox values")
editbox.setMaxTextLength(13) ' The trailing 's' will not be displayed
editbox.setReadOnly(False)
editbox.setTextMasked(False)
editbox.setMaskCodePoint($002A) ' *
Local valueEditbox:String = editbox.getText() ' Retrieve the text

' Slider
Local slider:TCESlider = TCESlider(TCEWindowManager.getWindow("Slider"))
slider.setMaxValue(100.0)
slider.setClickStep(10.0)
slider.setCurrentValue(20.0)
Local valueSlider:Float = slider.getCurrentValue() ' Retrieve the value

' Scrollbar (Horizontal)
Local scrollbarHorizontal:TCEScrollbar = TCEScrollbar(TCEWindowManager.getWindow("HorizontalScrollbar"))
scrollbarHorizontal.setDocumentSize(100.0)
scrollbarHorizontal.setPageSize(10.0)
scrollbarHorizontal.setStepSize(1.0)
scrollbarHorizontal.setScrollPosition(75.0)
Local valueScrollbarHorizontal:Float = scrollbarHorizontal.getScrollPosition() ' Retrieve the scroll position

' Scrollbar (Vertical)
Local scrollbarVertical:TCEScrollbar = TCEScrollbar(TCEWindowManager.getWindow("VerticalScrollbar"))
scrollbarVertical.setDocumentSize(100.0)
scrollbarVertical.setPageSize(10.0)
scrollbarVertical.setStepSize(1.0)
scrollbarVertical.setScrollPosition(25.0)
Local valueScrollbarVertical:Float = scrollbarVertical.getScrollPosition() ' Retrieve the scroll position

' MultiLineEditbox
Local multiLineEditbox:TCEMultiLineEditbox = TCEMultiLineEditbox(TCEWindowManager.getWindow("MultiLineEditbox"))
multiLineEditbox.setText("MultiLineEditbox value")
multiLineEditbox.setReadOnly(False)
multiLineEditbox.setWordWrapping(True)
Local valueMultiLineEditbox:String = multiLineEditbox.getText() ' Retrieve the text

' RadioButton
Local radioButton:TCERadioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_A"))
	radioButton.setGroupID(1)
	radioButton.setID(101)
	radioButton.setSelected(True)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_B"))
	radioButton.setGroupID(1)
	radioButton.setID(102)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_C"))
	radioButton.setGroupID(1)
	radioButton.setID(103)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_1"))
	radioButton.setGroupID(2)
	radioButton.setID(201)
	radioButton.setSelected(True)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_2"))
	radioButton.setGroupID(2)
	radioButton.setID(202)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_3"))
	radioButton.setGroupID(2)
	radioButton.setID(203)
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_A")) ' Get handle of one radio button from the group
Local valueRadioButtonLetters:Int = radioButton.getSelectedButtonInGroup().getID() ' Get selected ID
radioButton = TCERadioButton(TCEWindowManager.getWindow("RadioButton_3")) ' Can obtain the handle of any radio button in the group
Local valueRadioButtonNumbers:Int = radioButton.getSelectedButtonInGroup().getID()
radioButton.setSelected(True) ' Specify which button should appear selected by default

' Listbox
Local listbox:TCEListbox = TCEListbox(TCEWindowManager.getWindow("Listbox"))
listbox.setMultiselectEnabled(False)
Local itemListbox:TCEListboxTextItem = New TCEListboxTextItem.Create("Value A", 1)
	itemListbox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	listbox.addItem(itemListbox)
itemListbox = New TCEListboxTextItem.Create("Value B", 2)
	itemListbox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	listbox.addItem(itemListbox)
itemListbox = New TCEListboxTextItem.Create("Value C", 3)
	itemListbox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	listbox.addItem(itemListbox)
itemListbox = New TCEListboxTextItem.Create("Value D", 4)
	itemListbox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	listbox.addItem(itemListbox)
listbox.setItemSelectState(itemListbox, True)
listbox.ensureItemIsVisible(itemListbox)
Local valueListbox:Int = listbox.getFirstSelectedItem().getID() ' Retrieve the ID of the selected listbox item

' Combobox
Local combobox:TCECombobox = TCECombobox(TCEWindowManager.getWindow("Combobox"))
combobox.setReadOnly(True)
Local itemCombobox:TCEListboxTextItem = New TCEListboxTextItem.Create("Value 1", 1)
	itemCombobox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	combobox.addItem(itemCombobox)
itemCombobox = New TCEListboxTextItem.Create("Value 2", 2)
	itemCombobox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	combobox.addItem(itemCombobox)
	itemCombobox.setSelected(True) ' Select this item
	combobox.setText(itemCombobox.getText()) ' Copy the item's text into the Editbox
itemCombobox = New TCEListboxTextItem.Create("Value 3", 3)
	itemCombobox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	combobox.addItem(itemCombobox)
itemCombobox = New TCEListboxTextItem.Create("Value 4", 4)
	itemCombobox.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	combobox.addItem(itemCombobox)
Local valueCombobox:String = combobox.getText() ' Retrieve the displayed text
Local idCombobox:Int = combobox.getSelectedItem().getID() ' Retrieve the ID of the selected combobox item

' MultiColumnList
Local multiColumnList:TCEMultiColumnList = TCEMultiColumnList(TCEWindowManager.getWindow("MultiColumnList"))
multiColumnList.addColumnU("Col A", 0, UDim(0.32, 0))
multiColumnList.addColumnU("Col B", 1, UDim(0.32, 0))
multiColumnList.addColumnU("Col C", 2, UDim(0.32, 0))
multiColumnList.setSelectionMode(MultiColumnList.SELECT_ROWSINGLE) ' MultiColumnList::RowMultiple
Local itemMultiColumnList:TCEListboxTextItem
multiColumnList.addRow()
itemMultiColumnList = New TCEListboxTextItem.Create("A1", 101)
	itemMultiColumnList.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	multiColumnList.setItem(itemMultiColumnList, 0, 0) ' ColumnID, RowID
itemMultiColumnList = New TCEListboxTextItem.Create("B1", 102)
	'itemMultiColumnList.setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush")
	' By commenting the line above a cell does not specify a selection indicator
	'  selecting that line will show a "gap" in the selection.
	multiColumnList.setItem(itemMultiColumnList, 1, 0) ' ColumnID, RowID
itemMultiColumnList = New TCEListboxTextItem.Create("C1", 103)
	itemMultiColumnList.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	multiColumnList.setItem(itemMultiColumnList, 2, 0) ' ColumnID, RowID
multiColumnList.addRow()
itemMultiColumnList = New TCEListboxTextItem.Create("A2", 201)
	itemMultiColumnList.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	multiColumnList.setItem(itemMultiColumnList, 0, 1) ' ColumnID, RowID
itemMultiColumnList = New TCEListboxTextItem.Create("B2", 202)
	itemMultiColumnList.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	multiColumnList.setItem(itemMultiColumnList, 1, 1) ' ColumnID, RowID
itemMultiColumnList = New TCEListboxTextItem.Create("C2", 203)
	itemMultiColumnList.setSelectionBrushImageByName("TaharezLook", "MultiListSelectionBrush")
	multiColumnList.setItem(itemMultiColumnList, 2, 1) ' ColumnID, RowID
' Select according to a grid reference second row
multiColumnList.setItemSelectStateForGridRef(1, 0, True)
Local listboxItem:TCEListboxItem = multiColumnList.getFirstSelectedItem()
Local valueColumnA:Int = listboxItem.getID() ' Retrieve the value of the selected item from column A
listboxItem = multiColumnList.getNextSelected(listboxItem)
Local valueColumnB:Int = listboxItem.getID() ' Retrieve the value of the selected item from column B
listboxItem = multiColumnList.getNextSelected(listboxItem)
Local valueColumnC:Int = listboxItem.getID() ' Retrieve the value of the selected item from column C

' TabControl
Local winTabControl:TCETabControl = TCETabControl(TCEWindowManager.getWindow("TabControl"))
winTabControl.setTabHeightU(UDim(0.15, 0.0)) ' Make the tab buttons a little bigger
Local tabPage:TCEWindow = TCEWindowManager.getWindow("TabPane1")
	tabPage.setText("Page 1")
	tabPage.setSizeU(UDim(1.0, 0.0), UDim(1.0, 0.0)) ' Size to 100% of its parent, the TabControl
	tabPage.setPositionU(UDim(0.0, 0.0), UDim(0.0, 0.0)) ' Move to the upper left corner of its parent
	winTabControl.addTab(tabPage)
tabPage = TCEWindowManager.getWindow("TabPane2")
	tabPage.setText("Page 2")
	tabPage.setSizeU(UDim(1.0, 0.0), UDim(1.0, 0.0)) 
	tabPage.setPositionU(UDim(0.0, 0.0), UDim(0.0, 0.0))
	winTabControl.addTab(tabPage)


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	TCEEvent.mouseEvents(MouseX(), MouseY(), MouseZ())
	TCESystem.renderGUI()

	Flip

Wend

cegui_cleanup()

End

