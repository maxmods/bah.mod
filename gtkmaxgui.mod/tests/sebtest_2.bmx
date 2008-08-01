SuperStrict

Framework bah.gtkmaxgui

Rem

Instructions
=================================

First, try with just EVENT_WINDOWMOVE... The window just keeps moving down and
to the left of the screen infintely.

Next, try with just EVENT_WINDOWSIZE... The window suddenly jumps to becoming
the full width/height of the screen.


NOTE: Both of these tests perform normally when compiled on Windows/Mac.

=================================

EndRem

Global wndTest:TGadget = CreateWindow("Test Window",100,100,300,200,Null,15|WINDOW_CLIENTCOORDS)
		
Repeat

	Select WaitEvent()
	
		Case EVENT_WINDOWCLOSE;End
		Case EVENT_APPTERMINATE;End
		
		'Case EVENT_WINDOWMOVE;SetGadgetShape(wndTest,GadgetX(wndTest),GadgetY(wndTest),ClientWidth(wndTest),ClientHeight(wndTest))
		Case EVENT_WINDOWSIZE;SetGadgetShape(wndTest,GadgetX(wndTest),GadgetY(wndTest),ClientWidth(wndTest),ClientHeight(wndTest))
		
		Default 
		
	EndSelect
	
	DebugLog CurrentEvent.ToString$()
	
Forever
	


