' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "main.bmx"


Rem
bbdoc: 
End Rem
Type TCODWidget

	Field widgetPtr:Byte Ptr
	
	Field userData:Object
	
	Field cbk(widget:TCODWidget, userData:Object)

	Function _callback(widget:TCODWidget)
		widget.cbk(widget, widget.userData)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function SetBackgroundColor(col:TCODColor, colFocus:TCODColor)
		bmx_tcodwidget_setbackgroundcolor(col.colorPtr, colFocus.colorPtr)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function SetForegroundColor(col:TCODColor, colFocus:TCODColor)
		bmx_tcodwidget_setforegroundcolor(col.colorPtr, colFocus.colorPtr)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function SetConsole(con:TCODConsole)
		bmx_tcodwidget_setconsole(con.consolePtr)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function UpdateWidgets(key:TCODKey)
		bmx_tcodwidget_updatewidgets(key.vk, key.c, key.pressed, key.lalt, key.lctrl, key.ralt, key.rctrl, key.shift)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function RenderWidgets()
		bmx_tcodwidget_renderwidgets()
	End Function

	Rem
	bbdoc: 
	End Rem
	Method move(x:Int, y:Int)
		bmx_tcodwidget_move(widgetPtr, x, y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTip(tip:String)
		bmx_tcodwidget_settip(widgetPtr, tip)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setVisible(visible:Int)
		bmx_tcodwidget_setvisible(widgetPtr, visible)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isVisible:Int()
		Return bmx_tcodwidget_isvisible(widgetPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Function Focus:TCODWidget()
		Return TCODWidget(bmx_tcodwidget_focus())
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCODButton Extends TCODWidget


	Rem
	bbdoc: 
	End Rem
	Method Create:TCODButton(x:Int = 0, y:Int = 0, w:Int = 0, h:Int = 0, label:String = Null, tip:String = Null)
		widgetPtr = bmx_tcodbutton_create(Self, label, tip, x, y, w, h)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetCallback(callback(widget:TCODWidget, userData:Object), userData:Object = Null)
		Self.userData = userData
		Self.cbk = callback
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCODContainer Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method AddWidget(wid:TCODWidget)
		bmx_tcodcontainer_addwidget(widgetPtr, wid.widgetPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RemoveWidget(wid:TCODWidget)
		bmx_tcodcontainer_removewidget(widgetPtr, wid.widgetPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCODHBox Extends TCODContainer
End Type

Rem
bbdoc: 
End Rem
Type TCODVBox Extends TCODContainer

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODVBox(x:Int = 0, y:Int = 0, padding:Int = 0)
		widgetPtr = bmx_tcodvbox_create(Self, x, y, padding)
		Return Self
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCODToolBar Extends TCODContainer

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODToolBar(x:Int = 0, y:Int = 0, name:String = Null, tip:String = Null)
		widgetPtr = bmx_tcodtoolbar_create(Self, x, y, name, tip)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetName(name:String)
		bmx_tcodtoolbar_setname(widgetPtr, name)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddSeparator(txt:String, tip:String = Null)
		bmx_tcodtoolbar_addseparator(widgetPtr, txt, tip)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TCODToggleButton Extends TCODButton
End Type

Rem
bbdoc: 
End Rem
Type TCODTextBox Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODTextBox(x:Int = 0, y:Int = 0, w:Int = 0, maxw:Int = 0, label:String = Null, value:String = Null, tip:String = Null)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetText(txt:String)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetValue:String()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function SetBlinkingDelay(_delay:Float)
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCODRadioButton Extends TCODButton
End Type

Rem
bbdoc: 
End Rem
Type TCODLabel Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODLabel(x:Int = 0, y:Int = 0, label:String = Null, tip:String = Null)
		widgetPtr = bmx_tcodlabel_create(Self, x, y, label, tip)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetValue(label:String)
		bmx_tcodlabel_setvalue(widgetPtr, label)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TCODFlatList Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODFlatList(x:Int = 0, y:Int = 0, w:Int = 0, list:String[], label:String = Null, tip:String = Null)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetText(txt:String)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetValue:String()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function SetBlinkingDelay(_delay:Float)
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCODSlider Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODSlider(x:Int = 0, y:Int = 0, w:Int = 0, _min:Float = 0, _max:Float = 10, label:String = Null, tip:String = Null)
	End Method

	Method SetMinMax(_min:Float, _max:Float)
	End Method
	
	Method SetFormat(fmt:String)
	End Method
	
	Method SetValue(value:Float)
	End Method
	
	Method SetSensitivity(sensitivity:Float)
	End Method

	Method SetText(txt:String)
	End Method

	Method GetValue:String()
	End Method
	
	Function SetBlinkingDelay(_delay:Float)
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TCODStatusBar Extends TCODWidget

	Rem
	bbdoc: 
	End Rem
	Method Create:TCODStatusBar(x:Int = 0, y:Int = 0, w:Int = 0, h:Int = 0)
		widgetPtr = bmx_tcodstatusbar_create(Self, x, y, w, h)
		Return Self
	End Method

End Type



