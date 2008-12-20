' Copyright (c) 2007-2008, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
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

Rem
bbdoc: Curses
End Rem
Module BaH.Curses

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2007,2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (cdk 5.0.20060507, pdcurses 3.4)"

Import Pub.StdC

Import "common.bmx"

Import "core.bmx"
Import "types/buttonbox.bmx"
Import "types/dialog.bmx"
Import "types/entry.bmx"
Import "types/label.bmx"
Import "types/scrolllist.bmx"
Import "types/slider.bmx"

New TCDKDriver




Type TCursesPanel

	Field panelPtr:Byte Ptr
	Field windowPtr:Byte Ptr
	
	Function Create:TCursesPanel(x:Int, y:Int, w:Int, h:Int)
		Local this:TCursesPanel = New TCursesPanel
	
		this.windowPtr = newwin(h, w, y, x)
		this.panelPtr = new_panel(this.windowPtr)
	
		Return this
	End Function

	Method bottom()
		bottom_panel(panelPtr)
	End Method
	
	Method hide()
		hide_panel(panelPtr)
	End Method
	
	Method move(x:Int, y:Int)
		move_panel(panelPtr, x, y)
	End Method
	
	Method isHidden:Int()
		Return panel_hidden(panelPtr)
	End Method
	
	
End Type



