' createmenu.bmx

Strict 

Framework BaH.GTKMaxGUI


Local window:TGadget
Local filemenu:TGadget
Local editmenu:TGadget
Local extramenu:TGadget
Local helpmenu:TGadget

Const MENU_NEW=101
Const MENU_OPEN=102
Const MENU_SAVE=103
Const MENU_CLOSE=104
Const MENU_EXIT=105

Const MENU_CUT=106
Const MENU_COPY=107
Const MENU_PASTE=108
Const MENU_EXTRA=209
Const MENU_EXTRA_SUB=210
Const MENU_EXTRA_SUB2=211
Const MENU_CHECKABLE=110

Const MENU_ABOUT=109

window=CreateWindow("My Window",40,40,320,240)

filemenu=CreateMenu("&File",0,WindowMenu(window))
CreateMenu"&New",MENU_NEW,filemenu,KEY_N,MODIFIER_COMMAND
CreateMenu"&Open",MENU_OPEN,filemenu,KEY_O,MODIFIER_COMMAND
CreateMenu"&Close",MENU_CLOSE,filemenu,KEY_W,MODIFIER_COMMAND
CreateMenu"",0,filemenu
CreateMenu"&Save",MENU_SAVE,filemenu,KEY_S,MODIFIER_COMMAND
CreateMenu"",0,filemenu
CreateMenu"&Quit",MENU_EXIT,filemenu,KEY_F4,MODIFIER_COMMAND


editmenu=CreateMenu("&Edit",0,WindowMenu(window))
CreateMenu "&Remove Paste",MENU_CUT,editmenu,KEY_X,MODIFIER_COMMAND
CreateMenu "&Copy",MENU_COPY,editmenu,KEY_C,MODIFIER_COMMAND
Local pastemenu:TGadget = CreateMenu("&Paste",MENU_PASTE,editmenu,KEY_V,MODIFIER_COMMAND)
extramenu = CreateMenu("&Extra",MENU_EXTRA,editmenu)
Local cmenu:TGadget = CreateMenu("Chec&kable", MENU_CHECKABLE, editmenu)
CheckMenu(cmenu)

Local submenu:TGadget = CreateMenu("Select me",MENU_EXTRA_SUB,extramenu,KEY_E,MODIFIER_COMMAND)
UncheckMenu(submenu)

Local disabledmenu:TGadget = CreateMenu("Sub menu 2!",MENU_EXTRA_SUB2,extramenu,KEY_R,MODIFIER_COMMAND)
DisableMenu(disabledmenu)


helpmenu=CreateMenu("&Help",0,WindowMenu(window))
CreateMenu "&About",MENU_ABOUT,helpmenu

UpdateWindowMenu window

While True
	WaitEvent
Print CurrentEvent.toString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_MENUACTION
			Select EventData()
				Case MENU_EXIT
					End
				Case MENU_COPY
					CheckMenu(submenu)

					Print SetMenuText(editmenu, MenuText(editmenu) + "X")
				Case MENU_CUT
					If pastemenu Then
						FreeMenu(pastemenu)
					End If
				Case MENU_EXTRA_SUB
					If MenuChecked(submenu) Then
						EnableMenu(disabledmenu)
					Else
						DisableMenu(disabledmenu)
					End If
				Case MENU_ABOUT
					Notify "Incrediabler~n(C)2005 Incredible Software"
			End Select
	End Select
Wend
