SuperStrict

Import "common.bmx"

Type TCODWidget

	Field widgetPtr:Byte Ptr
	
	Field userData:Object
	
	Field cbk(widget:TCODWidget, userData:Object)

	Function _callback(widget:TCODWidget)
		widget.cbk(widget, widget.userData)
	End Function

End Type


Type TCODButton Extends TCODWidget


	Method Create:TCODWidget(label:String, tip:String, callback(widget:TCODWidget, userData:Object), x:Int = 0, y:Int = 0, w:Int = 0, h:Int = 0, userData:Object = Null)
		Self.userData = userData
		Self.cbk = callback
		widgetPtr = bmx_tcodbutton_create(Self, label, tip, x, y, w, h)
		Return Self
	End Method
	

End Type
