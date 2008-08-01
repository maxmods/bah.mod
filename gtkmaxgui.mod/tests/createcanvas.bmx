
' createcanvas.bmx

Strict 

Framework bah.gtkmaxgui
Import BRL.Timer
Import brl.eventqueue

Import BRL.Max2D
Import BRL.GLMax2d


Global GAME_WIDTH=320
Global GAME_HEIGHT=240

' create a centered window with client size GAME_WIDTH,GAME_HEIGHT

Local wx=(GadgetWidth(Desktop())-GAME_WIDTH)/2
Local wy=(GadgetHeight(Desktop())-GAME_HEIGHT)/2

Local window:TGadget=CreateWindow("My Canvas",wx,wy,GAME_WIDTH,GAME_HEIGHT,Null,WINDOW_TITLEBAR|WINDOW_CLIENTCOORDS)

' create a canvas for our game

Local canvas:TGadget=CreateCanvas(0,0,320,240,window)

' create an update timer

CreateTimer 60

ActivateGadget(canvas)

While WaitEvent()
	Print CurrentEvent.toString()
	Select EventID()
		Case EVENT_TIMERTICK
			RedrawGadget canvas

		Case EVENT_GADGETPAINT
			SetGraphics CanvasGraphics(canvas)
			SetOrigin 160,120
			SetLineWidth 5
			Cls
			Local t=MilliSecs()
			DrawLine 0,0,120*Cos(t),120*Sin(t)
			DrawLine 0,0,80*Cos(t/60),80*Sin(t/60)
			Flip

		Case EVENT_MOUSEMOVE
			Print "MOVE! : " + CurrentEvent.x + " , " + CurrentEvent.y

		Case EVENT_MOUSELEAVE
			Print "Out..."

		Case EVENT_MOUSEENTER
			Print "In..."

		Case EVENT_MOUSEDOWN
			Print "Mouse down..." + EventData()

		Case EVENT_MOUSEUP
			Print "Mouse up  ..." + EventData()

		Case EVENT_KEYDOWN
			Print "Down   = " + EventData()

		Case EVENT_KEYREPEAT
			Print "Repeat = " + EventData()

		Case EVENT_KEYCHAR
			Print "Char   = " + EventData()

		Case EVENT_KEYUP
			Print "Up     = " + EventData()

		Case EVENT_MOUSEWHEEL
			Print "Mouse wheel..." + EventData()

		Case EVENT_WINDOWCLOSE
			FreeGadget canvas
			End

		Case EVENT_APPTERMINATE
			End
	End Select
Wend