' Copyright (c) 2006-2008 Bruce A Henderson
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

Import BRL.KeyCodes
Import brl.standardio ' Remove me (for debugging only!)

Extern
	Function gdk_keyval_name:Byte Ptr(key:Int)
End Extern

' GdkModifierType
Const GDK_SHIFT_MASK:Int    = 1
Const GDK_LOCK_MASK:Int	    = 2
Const GDK_CONTROL_MASK:Int  = 4
Const GDK_MOD1_MASK:Int	    = 8 ' alt
Const GDK_MOD2_MASK:Int	    = 16 ' num lock
Const GDK_MOD3_MASK:Int	    = 32
Const GDK_MOD4_MASK:Int	    = 64 ' windows key
Const GDK_MOD5_MASK:Int	    = 128
Const GDK_BUTTON1_MASK:Int  = 256
Const GDK_BUTTON2_MASK:Int  = 512
Const GDK_BUTTON3_MASK:Int  = 1024
Const GDK_BUTTON4_MASK:Int  = 2048
Const GDK_BUTTON5_MASK:Int  = 4096

' GDK keycodes
Const GDK_space:Int = 32
Const GDK_exclam:Int = 33
Const GDK_quotedbl:Int = 34
Const GDK_numbersign:Int = 35
Const GDK_dollar:Int = 36
Const GDK_percent:Int = 37
Const GDK_ampersand:Int = 38
Const GDK_apostrophe:Int = 39
Const GDK_quoteright:Int = 39
Const GDK_parenleft:Int = 40
Const GDK_parenright:Int = 41
Const GDK_asterisk:Int = 42
Const GDK_plus:Int = 43
Const GDK_comma:Int = 44
Const GDK_minus:Int = 45
Const GDK_period:Int = 46
Const GDK_slash :Int = 47

Const GDK_0:Int = 48
Const GDK_1:Int = 49
Const GDK_2:Int = 50
Const GDK_3:Int = 51
Const GDK_4:Int = 52
Const GDK_5:Int = 53
Const GDK_6:Int = 54
Const GDK_7:Int = 55
Const GDK_8:Int = 56
Const GDK_9:Int = 57
Const GDK_colon:Int = 58
Const GDK_semicolon:Int = 59
Const GDK_less:Int = 60
Const GDK_equal:Int = 61
Const GDK_greater:Int = 62
Const GDK_question:Int = 63
Const GDK_at:Int = 64

Const GDK_bracketleft:Int = 91 '0x05b
Const GDK_backslash:Int = 92
Const GDK_bracketright:Int = 93
Const GDK_asciicircum:Int = 94
Const GDK_underscore:Int = 95
Const GDK_grave:Int = 96
Const GDK_quoteleft:Int = 96
Const GDK_a:Int = 97
Const GDK_b:Int = 98
Const GDK_c:Int = 99
Const GDK_d:Int = 100
Const GDK_e:Int = 101
Const GDK_f:Int = 102
Const GDK_g:Int = 103
Const GDK_h:Int = 104
Const GDK_i:Int = 105
Const GDK_j:Int = 106
Const GDK_k:Int = 107
Const GDK_l:Int = 108
Const GDK_m:Int = 109
Const GDK_n:Int = 110
Const GDK_o:Int = 111
Const GDK_p:Int = 112
Const GDK_q:Int = 113
Const GDK_r:Int = 114
Const GDK_s:Int = 115
Const GDK_t:Int = 116
Const GDK_u:Int = 117
Const GDK_v:Int = 118
Const GDK_w:Int = 119
Const GDK_x:Int = 120
Const GDK_y:Int = 121
Const GDK_z:Int = 122

Const GDK_BackSpace:Int = 65288
Const GDK_Tab:Int = 65289
Const GDK_ISO_Left_Tab:Int = 65056
Const GDK_Linefeed:Int = 65290
Const GDK_Clear:Int = 65291
Const GDK_Return:Int = 65293
Const GDK_Pause:Int = 65299
Const GDK_Scroll_Lock:Int = 65300
Const GDK_Sys_Reqv:Int = 65301
Const GDK_Escape:Int = 65307
Const GDK__Delete:Int = 65535

Const GDK_Home:Int = 65360
Const GDK_Left:Int = 65361
Const GDK_Up:Int = 65362
Const GDK_Right:Int = 65363
Const GDK_Down:Int = 65364
Const GDK_Prior:Int = 65365
Const GDK_Page_Up:Int = 65365
Const GDK_Next:Int = 65366
Const GDK_Page_Down:Int = 65366
Const GDK_End:Int = 65367
Const GDK_Begin:Int = 65368

Const GDK_Select:Int = 65376
Const GDK_Print:Int = 65377
Const GDK_Execute:Int = 65378
Const GDK_Insert:Int = 65379
Const GDK_Undo:Int = 65381
Const GDK_Redo:Int = 65382

Const GDK_KP_Multiply:Int = 65450
Const GDK_KP_Add:Int = 65451
Const GDK_KP_Separator:Int = 65452
Const GDK_KP_Subtract:Int = 65453
Const GDK_KP_Decimal:Int = 65454
Const GDK_KP_Divide:Int = 65455
Const GDK_KP_0:Int = 65456
Const GDK_KP_1:Int = 65457
Const GDK_KP_2:Int = 65458
Const GDK_KP_3:Int = 65459
Const GDK_KP_4:Int = 65460
Const GDK_KP_5:Int = 65461
Const GDK_KP_6:Int = 65462
Const GDK_KP_7:Int = 65463
Const GDK_KP_8:Int = 65464
Const GDK_KP_9:Int = 65465

Const GDK_F1:Int = 65470
Const GDK_F2:Int = 65471
Const GDK_F3:Int = 65472
Const GDK_F4:Int = 65473
Const GDK_F5:Int = 65474
Const GDK_F6:Int = 65475
Const GDK_F7:Int = 65476
Const GDK_F8:Int = 65477
Const GDK_F9:Int = 65478
Const GDK_F10:Int = 65479
Const GDK_F11:Int = 65480
Const GDK_F12:Int = 65481

Const GDK_Shift_L:Int = 65505
Const GDK_Shift_R:Int = 65506
Const GDK_Control_L:Int = 65507
Const GDK_Control_R:Int = 65508
Const GDK_Caps_Lock:Int = 65509
Const GDK_Shift_Lock:Int = 65510
Const GDK_Meta_L:Int = 65511
Const GDK_Meta_R:Int = 65512
Const GDK_Alt_L:Int = 65513
Const GDK_Alt_R:Int = 65514
Const GDK_Super_L:Int = 65515
Const GDK_Super_R:Int = 65516
Const GDK_Hyper_L:Int = 65517
Const GDK_Hyper_R:Int = 65518

Type TGTKKeyMap

	Function mapModifier:Int(maxKey:Int)
		Local m:Int = 0

		If maxKey & MODIFIER_SHIFT Then    'shift key
				m:| GDK_SHIFT_MASK
		End If

		If maxKey & MODIFIER_CONTROL Then  'ctrl key
				m:| GDK_CONTROL_MASK
		End If

		If maxKey & MODIFIER_OPTION Then   'alt or menu key
				m:| GDK_MOD1_MASK
		End If

		If maxKey & MODIFIER_SYSTEM Then   ' windows / apple
				m:| GDK_MOD2_MASK   ' ??? or 3, 4, or 5...
		End If

		Return m
	End Function

	Function mapModifierBack:Int(gtkMod:Int)
		Local m:Int = 0

		If gtkMod & GDK_SHIFT_MASK Then    'shift key
				m:| MODIFIER_SHIFT
		End If

		If gtkMod & GDK_CONTROL_MASK Then  'ctrl key
				m:| MODIFIER_CONTROL
		End If

		If gtkMod & GDK_MOD1_MASK Then   'alt or menu key
				m:| MODIFIER_OPTION
		End If

		If gtkMod & GDK_MOD4_MASK Then   ' windows / apple
				m:| MODIFIER_SYSTEM   ' ??? or 3, 4, or 5...
		End If

		Return m
	End Function

	Function mapKey:Int(maxKey:Int)
		Global map:Int[]

		If Not map Then
			map = New Int[255]

			map[KEY_BACKSPACE] = GDK_BackSpace
			map[KEY_TAB] = GDK_Tab
			map[KEY_CLEAR] = GDK_Clear
			map[KEY_RETURN] = GDK_Return
			map[KEY_ENTER] = GDK_Linefeed
			map[KEY_ESCAPE] = GDK_Escape
			map[KEY_SPACE] = GDK_space
			map[KEY_PAGEUP] = GDK_Page_Up
			map[KEY_PAGEDOWN] = GDK_Page_Down
			map[KEY_END] = GDK_End
			map[KEY_HOME] = GDK_Begin
			
			map[KEY_LEFT] = GDK_Left
			map[KEY_UP] = GDK_Up
			map[KEY_RIGHT] = GDK_Right
			map[KEY_DOWN] = GDK_Down
			
			map[KEY_SELECT] = GDK_Select
			map[KEY_PRINT] = GDK_Print
			map[KEY_EXECUTE] = GDK_Execute
			map[KEY_SCREEN] =  GDK_Sys_Reqv
			map[KEY_INSERT] = GDK_Insert
			map[KEY_DELETE] = GDK__Delete


			map[KEY_0] = GDK_0
			map[KEY_1] = GDK_1
			map[KEY_2] = GDK_2
			map[KEY_3] = GDK_3
			map[KEY_4] = GDK_4
			map[KEY_5] = GDK_5
			map[KEY_6] = GDK_6
			map[KEY_7] = GDK_7
			map[KEY_8] = GDK_8
			map[KEY_9] = GDK_9
			map[KEY_A] = GDK_a
			map[KEY_B] = GDK_b
			map[KEY_C] = GDK_c
			map[KEY_D] = GDK_d
			map[KEY_E] = GDK_e
			map[KEY_F] = GDK_f
			map[KEY_G] = GDK_g
			map[KEY_H] = GDK_h
			map[KEY_I] = GDK_i
			map[KEY_J] = GDK_j
			map[KEY_K] = GDK_k
			map[KEY_L] = GDK_l
			map[KEY_M] = GDK_m
			map[KEY_N] = GDK_n
			map[KEY_O] = GDK_o
			map[KEY_P] = GDK_p
			map[KEY_Q] = GDK_q
			map[KEY_R] = GDK_r
			map[KEY_S] = GDK_s
			map[KEY_T] = GDK_t
			map[KEY_U] = GDK_u
			map[KEY_V] = GDK_v
			map[KEY_W] = GDK_w
			map[KEY_X] = GDK_x
			map[KEY_Y] = GDK_y
			map[KEY_Z] = GDK_z

			map[KEY_NUM0] = GDK_KP_0
			map[KEY_NUM1] = GDK_KP_1
			map[KEY_NUM2] = GDK_KP_2
			map[KEY_NUM3] = GDK_KP_3
			map[KEY_NUM4] = GDK_KP_4
			map[KEY_NUM5] = GDK_KP_5
			map[KEY_NUM6] = GDK_KP_6
			map[KEY_NUM7] = GDK_KP_7
			map[KEY_NUM8] = GDK_KP_8
			map[KEY_NUM9] = GDK_KP_9
			
			map[KEY_NUMMULTIPLY] = GDK_KP_Multiply
			map[KEY_NUMADD] = GDK_KP_Add
			map[KEY_NUMSUBTRACT] = GDK_KP_Subtract
			map[KEY_NUMDECIMAL] = GDK_KP_Decimal
			map[KEY_NUMDIVIDE] = GDK_KP_Divide
			
			map[KEY_F1] = GDK_F1
			map[KEY_F2] = GDK_F2
			map[KEY_F3] = GDK_F3
			map[KEY_F4] = GDK_F4
			map[KEY_F5] = GDK_F5
			map[KEY_F6] = GDK_F6
			map[KEY_F7] = GDK_F7
			map[KEY_F8] = GDK_F8
			map[KEY_F9] = GDK_F9
			map[KEY_F10] = GDK_F10
			map[KEY_F11] = GDK_F11
			map[KEY_F12] = GDK_F12
			
			map[KEY_TILDE] = GDK_quotedbl
			map[KEY_MINUS] = GDK_minus
			map[KEY_EQUALS] = GDK_plus
			
			map[KEY_OPENBRACKET] = GDK_bracketleft
			map[KEY_CLOSEBRACKET] = GDK_bracketright
			map[KEY_BACKSLASH] = GDK_backslash
			
			map[KEY_SEMICOLON] = GDK_semicolon
			map[KEY_QUOTES] = GDK_at
			
			map[KEY_COMMA] = GDK_comma
			map[KEY_PERIOD] = GDK_period
			map[KEY_SLASH] = GDK_slash
			
			map[KEY_LSHIFT] = GDK_Shift_L
			map[KEY_RSHIFT] = GDK_Shift_R
			map[KEY_LCONTROL] = GDK_Control_L
			map[KEY_RCONTROL] = GDK_Control_R
			map[KEY_LALT] = GDK_Alt_L
			map[KEY_RALT] = GDK_Alt_R
			map[KEY_LSYS] = GDK_Meta_L
			map[KEY_RSYS] = GDK_Meta_R

		End If

		Return map[maxKey]

	End Function

	Function mapBack:Int(key:Int)
		Local gtkmap:Int
'Print "gdkkey = " + key + " : " + String.fromCString(gdk_keyval_name(key))
		Select key
			Case GDK_exclam
				gtkmap = KEY_1
			Case GDK_BackSpace
				gtkmap =  KEY_BACKSPACE
			Case GDK_Tab, GDK_ISO_Left_Tab
				gtkmap = KEY_TAB
			Case GDK_Clear
				gtkmap = KEY_CLEAR
			Case GDK_Return
				gtkmap = KEY_RETURN
			Case GDK_Linefeed
				gtkmap = KEY_ENTER
			Case GDK_Escape
				gtkmap = KEY_ESCAPE
			Case GDK_space
				gtkmap = KEY_SPACE
			Case GDK_Page_Up
				gtkmap = KEY_PAGEUP
			Case GDK_Page_Down
				gtkmap = KEY_PAGEDOWN
			Case GDK_End
				gtkmap = KEY_END
			Case GDK_Begin
				gtkmap = KEY_HOME
			
			Case GDK_Left
				gtkmap =  KEY_LEFT
			Case GDK_Up
				gtkmap =  KEY_UP
			Case GDK_Right
				gtkmap =  KEY_RIGHT
			Case GDK_Down
				gtkmap =  KEY_DOWN
			
			Case GDK_Select
				gtkmap =  KEY_SELECT
			Case GDK_Print
				gtkmap =  KEY_PRINT
			Case GDK_Execute
				gtkmap =  KEY_EXECUTE
			Case GDK_Sys_Reqv
				gtkmap =  KEY_SCREEN
			Case GDK_Insert
				gtkmap =  KEY_INSERT
			Case GDK__Delete
				gtkmap =  KEY_DELETE

			Case GDK_KP_Multiply
				gtkmap =  KEY_NUMMULTIPLY
			Case GDK_KP_Add
				gtkmap =  KEY_NUMADD
			Case GDK_KP_Subtract
				gtkmap =  KEY_NUMSUBTRACT
			Case GDK_KP_Decimal
				gtkmap =  KEY_NUMDECIMAL
			Case GDK_KP_Divide
				gtkmap =  KEY_NUMDIVIDE
		
			Case GDK_quotedbl
				gtkmap =  KEY_TILDE
			Case GDK_minus
				gtkmap =  KEY_MINUS
			Case GDK_plus
				gtkmap =  KEY_EQUALS
			
			Case GDK_bracketleft
				gtkmap =  KEY_OPENBRACKET
			Case GDK_bracketright
				gtkmap =  KEY_CLOSEBRACKET
			Case GDK_backslash
				gtkmap =  KEY_BACKSLASH
			
			Case GDK_semicolon, GDK_colon
				gtkmap =  KEY_SEMICOLON
			Case GDK_at, GDK_apostrophe
				gtkmap =  KEY_QUOTES
			
			Case GDK_comma, GDK_less
				gtkmap =  KEY_COMMA
			Case GDK_period, GDK_greater
				gtkmap =  KEY_PERIOD
			Case GDK_slash, GDK_question
				gtkmap =  KEY_SLASH
				
			Case GDK_Shift_L
				gtkmap =  KEY_LSHIFT
			Case GDK_Shift_R
				gtkmap =  KEY_RSHIFT
			Case GDK_Control_L
				gtkmap =  KEY_LCONTROL
			Case GDK_Control_R
				gtkmap =  KEY_RCONTROL
			Case GDK_Alt_L
				gtkmap =  KEY_LALT
			Case GDK_Alt_R
				gtkmap =  KEY_RALT
			Case GDK_Meta_L
				gtkmap =  KEY_LSYS
			Case GDK_Meta_R
				gtkmap =  KEY_RSYS

			Default
				If key >= GDK_a And key <= GDK_z Then
					Return key - 32
				End If

				If key >= KEY_A And key <= KEY_Z Then
					Return key
				End If

				If key >= GDK_0 And key <= GDK_9 Then
					Return key
				End If

				If key >= GDK_KP_0 And key <= GDK_KP_0 Then
					Return (key - GDK_KP_0) + KEY_NUM0
				End If

				If key >= GDK_F1 And key <= GDK_F12 Then
					Return (key - GDK_F1) + KEY_F1
				End If
		End Select

		Return gtkmap
	End Function

End Type


Rem
const GDK_VoidSymbol 0xFFFFFF
const GDK_BackSpace 0xFF08
const GDK_Tab 0xFF09
const GDK_Linefeed 0xFF0A
const GDK_Clear 0xFF0B
const GDK_Return 0xFF0D
const GDK_Pause 0xFF13
const GDK_Scroll_Lock 0xFF14
const GDK_Sys_Req 0xFF15
const GDK_Escape 0xFF1B
const GDK_Delete 0xFFFF
const GDK_Multi_key 0xFF20
const GDK_Codeinput 0xFF37
const GDK_SingleCandidate 0xFF3C
const GDK_MultipleCandidate 0xFF3D
const GDK_PreviousCandidate 0xFF3E
const GDK_Kanji 0xFF21
const GDK_Muhenkan 0xFF22
const GDK_Henkan_Mode 0xFF23
const GDK_Henkan 0xFF23
const GDK_Romaji 0xFF24
const GDK_Hiragana 0xFF25
const GDK_Katakana 0xFF26
const GDK_Hiragana_Katakana 0xFF27
const GDK_Zenkaku 0xFF28
const GDK_Hankaku 0xFF29
const GDK_Zenkaku_Hankaku 0xFF2A
const GDK_Touroku 0xFF2B
const GDK_Massyo 0xFF2C
const GDK_Kana_Lock 0xFF2D
const GDK_Kana_Shift 0xFF2E
const GDK_Eisu_Shift 0xFF2F
const GDK_Eisu_toggle 0xFF30
const GDK_Kanji_Bangou 0xFF37
const GDK_Zen_Koho 0xFF3D
const GDK_Mae_Koho 0xFF3E
const GDK_Home 0xFF50
const GDK_Left 0xFF51
const GDK_Up 0xFF52
const GDK_Right 0xFF53
const GDK_Down 0xFF54
const GDK_Prior 0xFF55
const GDK_Page_Up 0xFF55
const GDK_Next 0xFF56
const GDK_Page_Down 0xFF56
const GDK_End 0xFF57
const GDK_Begin 0xFF58
const GDK_Select 0xFF60
const GDK_Print 0xFF61
const GDK_Execute 0xFF62
const GDK_Insert 0xFF63
const GDK_Undo 0xFF65
const GDK_Redo 0xFF66
const GDK_Menu 0xFF67
const GDK_Find 0xFF68
const GDK_Cancel 0xFF69
const GDK_Help 0xFF6A
const GDK_Break 0xFF6B
const GDK_Mode_switch 0xFF7E
const GDK_script_switch 0xFF7E
const GDK_Num_Lock 0xFF7F
const GDK_KP_Space 0xFF80
const GDK_KP_Tab 0xFF89
const GDK_KP_Enter 0xFF8D
const GDK_KP_F1 0xFF91
const GDK_KP_F2 0xFF92
const GDK_KP_F3 0xFF93
const GDK_KP_F4 0xFF94
const GDK_KP_Home 0xFF95
const GDK_KP_Left 0xFF96
const GDK_KP_Up 0xFF97
const GDK_KP_Right 0xFF98
const GDK_KP_Down 0xFF99
const GDK_KP_Prior 0xFF9A
const GDK_KP_Page_Up 0xFF9A
const GDK_KP_Next 0xFF9B
const GDK_KP_Page_Down 0xFF9B
const GDK_KP_End 0xFF9C
const GDK_KP_Begin 0xFF9D
const GDK_KP_Insert 0xFF9E
const GDK_KP_Delete 0xFF9F
const GDK_KP_Equal 0xFFBD
const GDK_KP_Multiply 0xFFAA
const GDK_KP_Add 0xFFAB
const GDK_KP_Separator 0xFFAC
const GDK_KP_Subtract 0xFFAD
const GDK_KP_Decimal 0xFFAE
const GDK_KP_Divide 0xFFAF
const GDK_KP_0 0xFFB0
const GDK_KP_1 0xFFB1
const GDK_KP_2 0xFFB2
const GDK_KP_3 0xFFB3
const GDK_KP_4 0xFFB4
const GDK_KP_5 0xFFB5
const GDK_KP_6 0xFFB6
const GDK_KP_7 0xFFB7
const GDK_KP_8 0xFFB8
const GDK_KP_9 0xFFB9
const GDK_F1 0xFFBE
const GDK_F2 0xFFBF
const GDK_F3 0xFFC0
const GDK_F4 0xFFC1
const GDK_F5 0xFFC2
const GDK_F6 0xFFC3
const GDK_F7 0xFFC4
const GDK_F8 0xFFC5
const GDK_F9 0xFFC6
const GDK_F10 0xFFC7
const GDK_F11 0xFFC8
const GDK_L1 0xFFC8
const GDK_F12 0xFFC9
const GDK_L2 0xFFC9
const GDK_F13 0xFFCA
const GDK_L3 0xFFCA
const GDK_F14 0xFFCB
const GDK_L4 0xFFCB
const GDK_F15 0xFFCC
const GDK_L5 0xFFCC
const GDK_F16 0xFFCD
const GDK_L6 0xFFCD
const GDK_F17 0xFFCE
const GDK_L7 0xFFCE
const GDK_F18 0xFFCF
const GDK_L8 0xFFCF
const GDK_F19 0xFFD0
const GDK_L9 0xFFD0
const GDK_F20 0xFFD1
const GDK_L10 0xFFD1
const GDK_F21 0xFFD2
const GDK_R1 0xFFD2
const GDK_F22 0xFFD3
const GDK_R2 0xFFD3
const GDK_F23 0xFFD4
const GDK_R3 0xFFD4
const GDK_F24 0xFFD5
const GDK_R4 0xFFD5
const GDK_F25 0xFFD6
const GDK_R5 0xFFD6
const GDK_F26 0xFFD7
const GDK_R6 0xFFD7
const GDK_F27 0xFFD8
const GDK_R7 0xFFD8
const GDK_F28 0xFFD9
const GDK_R8 0xFFD9
const GDK_F29 0xFFDA
const GDK_R9 0xFFDA
const GDK_F30 0xFFDB
const GDK_R10 0xFFDB
const GDK_F31 0xFFDC
const GDK_R11 0xFFDC
const GDK_F32 0xFFDD
const GDK_R12 0xFFDD
const GDK_F33 0xFFDE
const GDK_R13 0xFFDE
const GDK_F34 0xFFDF
const GDK_R14 0xFFDF
const GDK_F35 0xFFE0
const GDK_R15 0xFFE0
const GDK_Shift_L 0xFFE1
const GDK_Shift_R 0xFFE2
const GDK_Control_L 0xFFE3
const GDK_Control_R 0xFFE4
const GDK_Caps_Lock 0xFFE5
const GDK_Shift_Lock 0xFFE6
const GDK_Meta_L 0xFFE7
const GDK_Meta_R 0xFFE8
const GDK_Alt_L 0xFFE9
const GDK_Alt_R 0xFFEA
const GDK_Super_L 0xFFEB
const GDK_Super_R 0xFFEC
const GDK_Hyper_L 0xFFED
const GDK_Hyper_R 0xFFEE
const GDK_ISO_Lock 0xFE01
const GDK_ISO_Level2_Latch 0xFE02
const GDK_ISO_Level3_Shift 0xFE03
const GDK_ISO_Level3_Latch 0xFE04
const GDK_ISO_Level3_Lock 0xFE05
const GDK_ISO_Group_Shift 0xFF7E
const GDK_ISO_Group_Latch 0xFE06
const GDK_ISO_Group_Lock 0xFE07
const GDK_ISO_Next_Group 0xFE08
const GDK_ISO_Next_Group_Lock 0xFE09
const GDK_ISO_Prev_Group 0xFE0A
const GDK_ISO_Prev_Group_Lock 0xFE0B
const GDK_ISO_First_Group 0xFE0C
const GDK_ISO_First_Group_Lock 0xFE0D
const GDK_ISO_Last_Group 0xFE0E
const GDK_ISO_Last_Group_Lock 0xFE0F
const GDK_ISO_Left_Tab 0xFE20
const GDK_ISO_Move_Line_Up 0xFE21
const GDK_ISO_Move_Line_Down 0xFE22
const GDK_ISO_Partial_Line_Up 0xFE23
const GDK_ISO_Partial_Line_Down 0xFE24
const GDK_ISO_Partial_Space_Left 0xFE25
const GDK_ISO_Partial_Space_Right 0xFE26
const GDK_ISO_Set_Margin_Left 0xFE27
const GDK_ISO_Set_Margin_Right 0xFE28
const GDK_ISO_Release_Margin_Left 0xFE29
const GDK_ISO_Release_Margin_Right 0xFE2A
const GDK_ISO_Release_Both_Margins 0xFE2B
const GDK_ISO_Fast_Cursor_Left 0xFE2C
const GDK_ISO_Fast_Cursor_Right 0xFE2D
const GDK_ISO_Fast_Cursor_Up 0xFE2E
const GDK_ISO_Fast_Cursor_Down 0xFE2F
const GDK_ISO_Continuous_Underline 0xFE30
const GDK_ISO_Discontinuous_Underline 0xFE31
const GDK_ISO_Emphasize 0xFE32
const GDK_ISO_Center_Object 0xFE33
const GDK_ISO_Enter 0xFE34
const GDK_dead_grave 0xFE50
const GDK_dead_acute 0xFE51
const GDK_dead_circumflex 0xFE52
const GDK_dead_tilde 0xFE53
const GDK_dead_macron 0xFE54
const GDK_dead_breve 0xFE55
const GDK_dead_abovedot 0xFE56
const GDK_dead_diaeresis 0xFE57
const GDK_dead_abovering 0xFE58
const GDK_dead_doubleacute 0xFE59
const GDK_dead_caron 0xFE5A
const GDK_dead_cedilla 0xFE5B
const GDK_dead_ogonek 0xFE5C
const GDK_dead_iota 0xFE5D
const GDK_dead_voiced_sound 0xFE5E
const GDK_dead_semivoiced_sound 0xFE5F
const GDK_dead_belowdot 0xFE60
const GDK_dead_hook 0xFE61
const GDK_dead_horn 0xFE62
const GDK_First_Virtual_Screen 0xFED0
const GDK_Prev_Virtual_Screen 0xFED1
const GDK_Next_Virtual_Screen 0xFED2
const GDK_Last_Virtual_Screen 0xFED4
const GDK_Terminate_Server 0xFED5
const GDK_AccessX_Enable 0xFE70
const GDK_AccessX_Feedback_Enable 0xFE71
const GDK_RepeatKeys_Enable 0xFE72
const GDK_SlowKeys_Enable 0xFE73
const GDK_BounceKeys_Enable 0xFE74
const GDK_StickyKeys_Enable 0xFE75
const GDK_MouseKeys_Enable 0xFE76
const GDK_MouseKeys_Accel_Enable 0xFE77
const GDK_Overlay1_Enable 0xFE78
const GDK_Overlay2_Enable 0xFE79
const GDK_AudibleBell_Enable 0xFE7A
const GDK_Pointer_Left 0xFEE0
const GDK_Pointer_Right 0xFEE1
const GDK_Pointer_Up 0xFEE2
const GDK_Pointer_Down 0xFEE3
const GDK_Pointer_UpLeft 0xFEE4
const GDK_Pointer_UpRight 0xFEE5
const GDK_Pointer_DownLeft 0xFEE6
const GDK_Pointer_DownRight 0xFEE7
const GDK_Pointer_Button_Dflt 0xFEE8
const GDK_Pointer_Button1 0xFEE9
const GDK_Pointer_Button2 0xFEEA
const GDK_Pointer_Button3 0xFEEB
const GDK_Pointer_Button4 0xFEEC
const GDK_Pointer_Button5 0xFEED
const GDK_Pointer_DblClick_Dflt 0xFEEE
const GDK_Pointer_DblClick1 0xFEEF
const GDK_Pointer_DblClick2 0xFEF0
const GDK_Pointer_DblClick3 0xFEF1
const GDK_Pointer_DblClick4 0xFEF2
const GDK_Pointer_DblClick5 0xFEF3
const GDK_Pointer_Drag_Dflt 0xFEF4
const GDK_Pointer_Drag1 0xFEF5
const GDK_Pointer_Drag2 0xFEF6
const GDK_Pointer_Drag3 0xFEF7
const GDK_Pointer_Drag4 0xFEF8
const GDK_Pointer_Drag5 0xFEFD
const GDK_Pointer_EnableKeys 0xFEF9
const GDK_Pointer_Accelerate 0xFEFA
const GDK_Pointer_DfltBtnNext 0xFEFB
const GDK_Pointer_DfltBtnPrev 0xFEFC
const GDK_3270_Duplicate 0xFD01
const GDK_3270_FieldMark 0xFD02
const GDK_3270_Right2 0xFD03
const GDK_3270_Left2 0xFD04
const GDK_3270_BackTab 0xFD05
const GDK_3270_EraseEOF 0xFD06
const GDK_3270_EraseInput 0xFD07
const GDK_3270_Reset 0xFD08
const GDK_3270_Quit 0xFD09
const GDK_3270_PA1 0xFD0A
const GDK_3270_PA2 0xFD0B
const GDK_3270_PA3 0xFD0C
const GDK_3270_Test 0xFD0D
const GDK_3270_Attn 0xFD0E
const GDK_3270_CursorBlink 0xFD0F
const GDK_3270_AltCursor 0xFD10
const GDK_3270_KeyClick 0xFD11
const GDK_3270_Jump 0xFD12
const GDK_3270_Ident 0xFD13
const GDK_3270_Rule 0xFD14
const GDK_3270_Copy 0xFD15
const GDK_3270_Play 0xFD16
const GDK_3270_Setup 0xFD17
const GDK_3270_Record 0xFD18
const GDK_3270_ChangeScreen 0xFD19
const GDK_3270_DeleteWord 0xFD1A
const GDK_3270_ExSelect 0xFD1B
const GDK_3270_CursorSelect 0xFD1C
const GDK_3270_PrintScreen 0xFD1D
const GDK_3270_Enter 0xFD1E
const GDK_space 0x020
const GDK_exclam 0x021
const GDK_quotedbl 0x022
const GDK_numbersign 0x023
const GDK_dollar 0x024
const GDK_percent 0x025
const GDK_ampersand 0x026
const GDK_apostrophe 0x027
const GDK_quoteright 0x027
const GDK_parenleft 0x028
const GDK_parenright 0x029
const GDK_asterisk 0x02a
const GDK_plus 0x02b
const GDK_comma 0x02c
const GDK_minus 0x02d
const GDK_period 0x02e
const GDK_slash 0x02f
const GDK_0 = 48
const GDK_1 = 49
const GDK_2 = 50
const GDK_3 = 51
const GDK_4 = 52
const GDK_5 = 53
const GDK_6 = 54
const GDK_7 = 55
const GDK_8 = 56
const GDK_9 = 57
const GDK_colon = 58
const GDK_semicolon = 59
const GDK_less = 60
const GDK_equal = 61
const GDK_greater = 62
const GDK_question = 63
const GDK_at = 64
const GDK_A = 65
const GDK_B = 66
const GDK_C = 67
const GDK_D = 68
const GDK_E = 69
const GDK_F = 70
const GDK_G = 71
const GDK_H = 72
const GDK_I = 73
const GDK_J = 74
const GDK_K = 75
const GDK_L = 76
const GDK_M = 77
const GDK_N = 78
const GDK_O = 79
const GDK_P = 80
const GDK_Q = 81
const GDK_R = 82
const GDK_S = 83
const GDK_T = 84
const GDK_U = 85
const GDK_V = 86
const GDK_W = 87
const GDK_X = 88
const GDK_Y = 89
const GDK_Z = 90  '0x05a
const GDK_bracketleft = 91 '0x05b
const GDK_backslash = 92
const GDK_bracketright = 93
const GDK_asciicircum = 94
const GDK_underscore = 95
const GDK_grave = 96
const GDK_quoteleft = 96
const GDK_a = 97
const GDK_b = 98
const GDK_c = 99
const GDK_d = 100
const GDK_e = 101
const GDK_f = 102
const GDK_g = 103
const GDK_h = 104
const GDK_i = 105
const GDK_j = 106
const GDK_k = 107
const GDK_l = 108
const GDK_m = 109
const GDK_n = 110
const GDK_o = 111
const GDK_p = 112
const GDK_q = 113
const GDK_r = 114
const GDK_s = 115
const GDK_t = 116
const GDK_u = 117
const GDK_v = 118
const GDK_w = 119
const GDK_x = 120
const GDK_y = 121
const GDK_z = 122
const GDK_braceleft 0x07b
const GDK_bar 0x07c
const GDK_braceright 0x07d
const GDK_asciitilde 0x07e
const GDK_nobreakspace 0x0a0
const GDK_exclamdown 0x0a1
const GDK_cent 0x0a2
const GDK_sterling 0x0a3
const GDK_currency 0x0a4
const GDK_yen 0x0a5
const GDK_brokenbar 0x0a6
const GDK_section 0x0a7
const GDK_diaeresis 0x0a8
const GDK_copyright 0x0a9
const GDK_ordfeminine 0x0aa
const GDK_guillemotleft 0x0ab
const GDK_notsign 0x0ac
const GDK_hyphen 0x0ad
const GDK_registered 0x0ae
const GDK_macron 0x0af
const GDK_degree 0x0b0
const GDK_plusminus 0x0b1
const GDK_twosuperior 0x0b2
const GDK_threesuperior 0x0b3
const GDK_acute 0x0b4
const GDK_mu 0x0b5
const GDK_paragraph 0x0b6
const GDK_periodcentered 0x0b7
const GDK_cedilla 0x0b8
const GDK_onesuperior 0x0b9
const GDK_masculine 0x0ba
const GDK_guillemotright 0x0bb
const GDK_onequarter 0x0bc
const GDK_onehalf 0x0bd
const GDK_threequarters 0x0be
const GDK_questiondown 0x0bf
const GDK_Agrave 0x0c0
const GDK_Aacute 0x0c1
const GDK_Acircumflex 0x0c2
const GDK_Atilde 0x0c3
const GDK_Adiaeresis 0x0c4
const GDK_Aring 0x0c5
const GDK_AE 0x0c6
const GDK_Ccedilla 0x0c7
const GDK_Egrave 0x0c8
const GDK_Eacute 0x0c9
const GDK_Ecircumflex 0x0ca
const GDK_Ediaeresis 0x0cb
const GDK_Igrave 0x0cc
const GDK_Iacute 0x0cd
const GDK_Icircumflex 0x0ce
const GDK_Idiaeresis 0x0cf
const GDK_ETH 0x0d0
const GDK_Eth 0x0d0
const GDK_Ntilde 0x0d1
const GDK_Ograve 0x0d2
const GDK_Oacute 0x0d3
const GDK_Ocircumflex 0x0d4
const GDK_Otilde 0x0d5
const GDK_Odiaeresis 0x0d6
const GDK_multiply 0x0d7
const GDK_Ooblique 0x0d8
const GDK_Ugrave 0x0d9
const GDK_Uacute 0x0da
const GDK_Ucircumflex 0x0db
const GDK_Udiaeresis 0x0dc
const GDK_Yacute 0x0dd
const GDK_THORN 0x0de
const GDK_Thorn 0x0de
const GDK_ssharp 0x0df
const GDK_agrave 0x0e0
const GDK_aacute 0x0e1
const GDK_acircumflex 0x0e2
const GDK_atilde 0x0e3
const GDK_adiaeresis 0x0e4
const GDK_aring 0x0e5
const GDK_ae 0x0e6
const GDK_ccedilla 0x0e7
const GDK_egrave 0x0e8
const GDK_eacute 0x0e9
const GDK_ecircumflex 0x0ea
const GDK_ediaeresis 0x0eb
const GDK_igrave 0x0ec
const GDK_iacute 0x0ed
const GDK_icircumflex 0x0ee
const GDK_idiaeresis 0x0ef
const GDK_eth 0x0f0
const GDK_ntilde 0x0f1
const GDK_ograve 0x0f2
const GDK_oacute 0x0f3
const GDK_ocircumflex 0x0f4
const GDK_otilde 0x0f5
const GDK_odiaeresis 0x0f6
const GDK_division 0x0f7
const GDK_oslash 0x0f8
const GDK_ugrave 0x0f9
const GDK_uacute 0x0fa
const GDK_ucircumflex 0x0fb
const GDK_udiaeresis 0x0fc
const GDK_yacute 0x0fd
const GDK_thorn 0x0fe
const GDK_ydiaeresis 0x0ff
const GDK_Aogonek 0x1a1
const GDK_breve 0x1a2
const GDK_Lstroke 0x1a3
const GDK_Lcaron 0x1a5
const GDK_Sacute 0x1a6
const GDK_Scaron 0x1a9
const GDK_Scedilla 0x1aa
const GDK_Tcaron 0x1ab
const GDK_Zacute 0x1ac
const GDK_Zcaron 0x1ae
const GDK_Zabovedot 0x1af
const GDK_aogonek 0x1b1
const GDK_ogonek 0x1b2
const GDK_lstroke 0x1b3
const GDK_lcaron 0x1b5
const GDK_sacute 0x1b6
const GDK_caron 0x1b7
const GDK_scaron 0x1b9
const GDK_scedilla 0x1ba
const GDK_tcaron 0x1bb
const GDK_zacute 0x1bc
const GDK_doubleacute 0x1bd
const GDK_zcaron 0x1be
const GDK_zabovedot 0x1bf
const GDK_Racute 0x1c0
const GDK_Abreve 0x1c3
const GDK_Lacute 0x1c5
const GDK_Cacute 0x1c6
const GDK_Ccaron 0x1c8
const GDK_Eogonek 0x1ca
const GDK_Ecaron 0x1cc
const GDK_Dcaron 0x1cf
const GDK_Dstroke 0x1d0
const GDK_Nacute 0x1d1
const GDK_Ncaron 0x1d2
const GDK_Odoubleacute 0x1d5
const GDK_Rcaron 0x1d8
const GDK_Uring 0x1d9
const GDK_Udoubleacute 0x1db
const GDK_Tcedilla 0x1de
const GDK_racute 0x1e0
const GDK_abreve 0x1e3
const GDK_lacute 0x1e5
const GDK_cacute 0x1e6
const GDK_ccaron 0x1e8
const GDK_eogonek 0x1ea
const GDK_ecaron 0x1ec
const GDK_dcaron 0x1ef
const GDK_dstroke 0x1f0
const GDK_nacute 0x1f1
const GDK_ncaron 0x1f2
const GDK_odoubleacute 0x1f5
const GDK_udoubleacute 0x1fb
const GDK_rcaron 0x1f8
const GDK_uring 0x1f9
const GDK_tcedilla 0x1fe
const GDK_abovedot 0x1ff
const GDK_Hstroke 0x2a1
const GDK_Hcircumflex 0x2a6
const GDK_Iabovedot 0x2a9
const GDK_Gbreve 0x2ab
const GDK_Jcircumflex 0x2ac
const GDK_hstroke 0x2b1
const GDK_hcircumflex 0x2b6
const GDK_idotless 0x2b9
const GDK_gbreve 0x2bb
const GDK_jcircumflex 0x2bc
const GDK_Cabovedot 0x2c5
const GDK_Ccircumflex 0x2c6
const GDK_Gabovedot 0x2d5
const GDK_Gcircumflex 0x2d8
const GDK_Ubreve 0x2dd
const GDK_Scircumflex 0x2de
const GDK_cabovedot 0x2e5
const GDK_ccircumflex 0x2e6
const GDK_gabovedot 0x2f5
const GDK_gcircumflex 0x2f8
const GDK_ubreve 0x2fd
const GDK_scircumflex 0x2fe
const GDK_kra 0x3a2
const GDK_kappa 0x3a2
const GDK_Rcedilla 0x3a3
const GDK_Itilde 0x3a5
const GDK_Lcedilla 0x3a6
const GDK_Emacron 0x3aa
const GDK_Gcedilla 0x3ab
const GDK_Tslash 0x3ac
const GDK_rcedilla 0x3b3
const GDK_itilde 0x3b5
const GDK_lcedilla 0x3b6
const GDK_emacron 0x3ba
const GDK_gcedilla 0x3bb
const GDK_tslash 0x3bc
const GDK_ENG 0x3bd
const GDK_eng 0x3bf
const GDK_Amacron 0x3c0
const GDK_Iogonek 0x3c7
const GDK_Eabovedot 0x3cc
const GDK_Imacron 0x3cf
const GDK_Ncedilla 0x3d1
const GDK_Omacron 0x3d2
const GDK_Kcedilla 0x3d3
const GDK_Uogonek 0x3d9
const GDK_Utilde 0x3dd
const GDK_Umacron 0x3de
const GDK_amacron 0x3e0
const GDK_iogonek 0x3e7
const GDK_eabovedot 0x3ec
const GDK_imacron 0x3ef
const GDK_ncedilla 0x3f1
const GDK_omacron 0x3f2
const GDK_kcedilla 0x3f3
const GDK_uogonek 0x3f9
const GDK_utilde 0x3fd
const GDK_umacron 0x3fe
const GDK_OE 0x13bc
const GDK_oe 0x13bd
const GDK_Ydiaeresis 0x13be
const GDK_overline 0x47e
const GDK_kana_fullstop 0x4a1
const GDK_kana_openingbracket 0x4a2
const GDK_kana_closingbracket 0x4a3
const GDK_kana_comma 0x4a4
const GDK_kana_conjunctive 0x4a5
const GDK_kana_middledot 0x4a5
const GDK_kana_WO 0x4a6
const GDK_kana_a 0x4a7
const GDK_kana_i 0x4a8
const GDK_kana_u 0x4a9
const GDK_kana_e 0x4aa
const GDK_kana_o 0x4ab
const GDK_kana_ya 0x4ac
const GDK_kana_yu 0x4ad
const GDK_kana_yo 0x4ae
const GDK_kana_tsu 0x4af
const GDK_kana_tu 0x4af
const GDK_prolongedsound 0x4b0
const GDK_kana_A 0x4b1
const GDK_kana_I 0x4b2
const GDK_kana_U 0x4b3
const GDK_kana_E 0x4b4
const GDK_kana_O 0x4b5
const GDK_kana_KA 0x4b6
const GDK_kana_KI 0x4b7
const GDK_kana_KU 0x4b8
const GDK_kana_KE 0x4b9
const GDK_kana_KO 0x4ba
const GDK_kana_SA 0x4bb
const GDK_kana_SHI 0x4bc
const GDK_kana_SU 0x4bd
const GDK_kana_SE 0x4be
const GDK_kana_SO 0x4bf
const GDK_kana_TA 0x4c0
const GDK_kana_CHI 0x4c1
const GDK_kana_TI 0x4c1
const GDK_kana_TSU 0x4c2
const GDK_kana_TU 0x4c2
const GDK_kana_TE 0x4c3
const GDK_kana_TO 0x4c4
const GDK_kana_NA 0x4c5
const GDK_kana_NI 0x4c6
const GDK_kana_NU 0x4c7
const GDK_kana_NE 0x4c8
const GDK_kana_NO 0x4c9
const GDK_kana_HA 0x4ca
const GDK_kana_HI 0x4cb
const GDK_kana_FU 0x4cc
const GDK_kana_HU 0x4cc
const GDK_kana_HE 0x4cd
const GDK_kana_HO 0x4ce
const GDK_kana_MA 0x4cf
const GDK_kana_MI 0x4d0
const GDK_kana_MU 0x4d1
const GDK_kana_ME 0x4d2
const GDK_kana_MO 0x4d3
const GDK_kana_YA 0x4d4
const GDK_kana_YU 0x4d5
const GDK_kana_YO 0x4d6
const GDK_kana_RA 0x4d7
const GDK_kana_RI 0x4d8
const GDK_kana_RU 0x4d9
const GDK_kana_RE 0x4da
const GDK_kana_RO 0x4db
const GDK_kana_WA 0x4dc
const GDK_kana_N 0x4dd
const GDK_voicedsound 0x4de
const GDK_semivoicedsound 0x4df
const GDK_kana_switch 0xFF7E
const GDK_Arabic_comma 0x5ac
const GDK_Arabic_semicolon 0x5bb
const GDK_Arabic_question_mark 0x5bf
const GDK_Arabic_hamza 0x5c1
const GDK_Arabic_maddaonalef 0x5c2
const GDK_Arabic_hamzaonalef 0x5c3
const GDK_Arabic_hamzaonwaw 0x5c4
const GDK_Arabic_hamzaunderalef 0x5c5
const GDK_Arabic_hamzaonyeh 0x5c6
const GDK_Arabic_alef 0x5c7
const GDK_Arabic_beh 0x5c8
const GDK_Arabic_tehmarbuta 0x5c9
const GDK_Arabic_teh 0x5ca
const GDK_Arabic_theh 0x5cb
const GDK_Arabic_jeem 0x5cc
const GDK_Arabic_hah 0x5cd
const GDK_Arabic_khah 0x5ce
const GDK_Arabic_dal 0x5cf
const GDK_Arabic_thal 0x5d0
const GDK_Arabic_ra 0x5d1
const GDK_Arabic_zain 0x5d2
const GDK_Arabic_seen 0x5d3
const GDK_Arabic_sheen 0x5d4
const GDK_Arabic_sad 0x5d5
const GDK_Arabic_dad 0x5d6
const GDK_Arabic_tah 0x5d7
const GDK_Arabic_zah 0x5d8
const GDK_Arabic_ain 0x5d9
const GDK_Arabic_ghain 0x5da
const GDK_Arabic_tatweel 0x5e0
const GDK_Arabic_feh 0x5e1
const GDK_Arabic_qaf 0x5e2
const GDK_Arabic_kaf 0x5e3
const GDK_Arabic_lam 0x5e4
const GDK_Arabic_meem 0x5e5
const GDK_Arabic_noon 0x5e6
const GDK_Arabic_ha 0x5e7
const GDK_Arabic_heh 0x5e7
const GDK_Arabic_waw 0x5e8
const GDK_Arabic_alefmaksura 0x5e9
const GDK_Arabic_yeh 0x5ea
const GDK_Arabic_fathatan 0x5eb
const GDK_Arabic_dammatan 0x5ec
const GDK_Arabic_kasratan 0x5ed
const GDK_Arabic_fatha 0x5ee
const GDK_Arabic_damma 0x5ef
const GDK_Arabic_kasra 0x5f0
const GDK_Arabic_shadda 0x5f1
const GDK_Arabic_sukun 0x5f2
const GDK_Arabic_switch 0xFF7E
const GDK_Serbian_dje 0x6a1
const GDK_Macedonia_gje 0x6a2
const GDK_Cyrillic_io 0x6a3
const GDK_Ukrainian_ie 0x6a4
const GDK_Ukranian_je 0x6a4
const GDK_Macedonia_dse 0x6a5
const GDK_Ukrainian_i 0x6a6
const GDK_Ukranian_i 0x6a6
const GDK_Ukrainian_yi 0x6a7
const GDK_Ukranian_yi 0x6a7
const GDK_Cyrillic_je 0x6a8
const GDK_Serbian_je 0x6a8
const GDK_Cyrillic_lje 0x6a9
const GDK_Serbian_lje 0x6a9
const GDK_Cyrillic_nje 0x6aa
const GDK_Serbian_nje 0x6aa
const GDK_Serbian_tshe 0x6ab
const GDK_Macedonia_kje 0x6ac
const GDK_Ukrainian_ghe_with_upturn 0x6ad
const GDK_Byelorussian_shortu 0x6ae
const GDK_Cyrillic_dzhe 0x6af
const GDK_Serbian_dze 0x6af
const GDK_numerosign 0x6b0
const GDK_Serbian_DJE 0x6b1
const GDK_Macedonia_GJE 0x6b2
const GDK_Cyrillic_IO 0x6b3
const GDK_Ukrainian_IE 0x6b4
const GDK_Ukranian_JE 0x6b4
const GDK_Macedonia_DSE 0x6b5
const GDK_Ukrainian_I 0x6b6
const GDK_Ukranian_I 0x6b6
const GDK_Ukrainian_YI 0x6b7
const GDK_Ukranian_YI 0x6b7
const GDK_Cyrillic_JE 0x6b8
const GDK_Serbian_JE 0x6b8
const GDK_Cyrillic_LJE 0x6b9
const GDK_Serbian_LJE 0x6b9
const GDK_Cyrillic_NJE 0x6ba
const GDK_Serbian_NJE 0x6ba
const GDK_Serbian_TSHE 0x6bb
const GDK_Macedonia_KJE 0x6bc
const GDK_Ukrainian_GHE_WITH_UPTURN 0x6bd
const GDK_Byelorussian_SHORTU 0x6be
const GDK_Cyrillic_DZHE 0x6bf
const GDK_Serbian_DZE 0x6bf
const GDK_Cyrillic_yu 0x6c0
const GDK_Cyrillic_a 0x6c1
const GDK_Cyrillic_be 0x6c2
const GDK_Cyrillic_tse 0x6c3
const GDK_Cyrillic_de 0x6c4
const GDK_Cyrillic_ie 0x6c5
const GDK_Cyrillic_ef 0x6c6
const GDK_Cyrillic_ghe 0x6c7
const GDK_Cyrillic_ha 0x6c8
const GDK_Cyrillic_i 0x6c9
const GDK_Cyrillic_shorti 0x6ca
const GDK_Cyrillic_ka 0x6cb
const GDK_Cyrillic_el 0x6cc
const GDK_Cyrillic_em 0x6cd
const GDK_Cyrillic_en 0x6ce
const GDK_Cyrillic_o 0x6cf
const GDK_Cyrillic_pe 0x6d0
const GDK_Cyrillic_ya 0x6d1
const GDK_Cyrillic_er 0x6d2
const GDK_Cyrillic_es 0x6d3
const GDK_Cyrillic_te 0x6d4
const GDK_Cyrillic_u 0x6d5
const GDK_Cyrillic_zhe 0x6d6
const GDK_Cyrillic_ve 0x6d7
const GDK_Cyrillic_softsign 0x6d8
const GDK_Cyrillic_yeru 0x6d9
const GDK_Cyrillic_ze 0x6da
const GDK_Cyrillic_sha 0x6db
const GDK_Cyrillic_e 0x6dc
const GDK_Cyrillic_shcha 0x6dd
const GDK_Cyrillic_che 0x6de
const GDK_Cyrillic_hardsign 0x6df
const GDK_Cyrillic_YU 0x6e0
const GDK_Cyrillic_A 0x6e1
const GDK_Cyrillic_BE 0x6e2
const GDK_Cyrillic_TSE 0x6e3
const GDK_Cyrillic_DE 0x6e4
const GDK_Cyrillic_IE 0x6e5
const GDK_Cyrillic_EF 0x6e6
const GDK_Cyrillic_GHE 0x6e7
const GDK_Cyrillic_HA 0x6e8
const GDK_Cyrillic_I 0x6e9
const GDK_Cyrillic_SHORTI 0x6ea
const GDK_Cyrillic_KA 0x6eb
const GDK_Cyrillic_EL 0x6ec
const GDK_Cyrillic_EM 0x6ed
const GDK_Cyrillic_EN 0x6ee
const GDK_Cyrillic_O 0x6ef
const GDK_Cyrillic_PE 0x6f0
const GDK_Cyrillic_YA 0x6f1
const GDK_Cyrillic_ER 0x6f2
const GDK_Cyrillic_ES 0x6f3
const GDK_Cyrillic_TE 0x6f4
const GDK_Cyrillic_U 0x6f5
const GDK_Cyrillic_ZHE 0x6f6
const GDK_Cyrillic_VE 0x6f7
const GDK_Cyrillic_SOFTSIGN 0x6f8
const GDK_Cyrillic_YERU 0x6f9
const GDK_Cyrillic_ZE 0x6fa
const GDK_Cyrillic_SHA 0x6fb
const GDK_Cyrillic_E 0x6fc
const GDK_Cyrillic_SHCHA 0x6fd
const GDK_Cyrillic_CHE 0x6fe
const GDK_Cyrillic_HARDSIGN 0x6ff
const GDK_Greek_ALPHAaccent 0x7a1
const GDK_Greek_EPSILONaccent 0x7a2
const GDK_Greek_ETAaccent 0x7a3
const GDK_Greek_IOTAaccent 0x7a4
const GDK_Greek_IOTAdieresis 0x7a5
const GDK_Greek_IOTAdiaeresis GDK_Greek_IOTAdieresis
const GDK_Greek_OMICRONaccent 0x7a7
const GDK_Greek_UPSILONaccent 0x7a8
const GDK_Greek_UPSILONdieresis 0x7a9
const GDK_Greek_OMEGAaccent 0x7ab
const GDK_Greek_accentdieresis 0x7ae
const GDK_Greek_horizbar 0x7af
const GDK_Greek_alphaaccent 0x7b1
const GDK_Greek_epsilonaccent 0x7b2
const GDK_Greek_etaaccent 0x7b3
const GDK_Greek_iotaaccent 0x7b4
const GDK_Greek_iotadieresis 0x7b5
const GDK_Greek_iotaaccentdieresis 0x7b6
const GDK_Greek_omicronaccent 0x7b7
const GDK_Greek_upsilonaccent 0x7b8
const GDK_Greek_upsilondieresis 0x7b9
const GDK_Greek_upsilonaccentdieresis 0x7ba
const GDK_Greek_omegaaccent 0x7bb
const GDK_Greek_ALPHA 0x7c1
const GDK_Greek_BETA 0x7c2
const GDK_Greek_GAMMA 0x7c3
const GDK_Greek_DELTA 0x7c4
const GDK_Greek_EPSILON 0x7c5
const GDK_Greek_ZETA 0x7c6
const GDK_Greek_ETA 0x7c7
const GDK_Greek_THETA 0x7c8
const GDK_Greek_IOTA 0x7c9
const GDK_Greek_KAPPA 0x7ca
const GDK_Greek_LAMDA 0x7cb
const GDK_Greek_LAMBDA 0x7cb
const GDK_Greek_MU 0x7cc
const GDK_Greek_NU 0x7cd
const GDK_Greek_XI 0x7ce
const GDK_Greek_OMICRON 0x7cf
const GDK_Greek_PI 0x7d0
const GDK_Greek_RHO 0x7d1
const GDK_Greek_SIGMA 0x7d2
const GDK_Greek_TAU 0x7d4
const GDK_Greek_UPSILON 0x7d5
const GDK_Greek_PHI 0x7d6
const GDK_Greek_CHI 0x7d7
const GDK_Greek_PSI 0x7d8
const GDK_Greek_OMEGA 0x7d9
const GDK_Greek_alpha 0x7e1
const GDK_Greek_beta 0x7e2
const GDK_Greek_gamma 0x7e3
const GDK_Greek_delta 0x7e4
const GDK_Greek_epsilon 0x7e5
const GDK_Greek_zeta 0x7e6
const GDK_Greek_eta 0x7e7
const GDK_Greek_theta 0x7e8
const GDK_Greek_iota 0x7e9
const GDK_Greek_kappa 0x7ea
const GDK_Greek_lamda 0x7eb
const GDK_Greek_lambda 0x7eb
const GDK_Greek_mu 0x7ec
const GDK_Greek_nu 0x7ed
const GDK_Greek_xi 0x7ee
const GDK_Greek_omicron 0x7ef
const GDK_Greek_pi 0x7f0
const GDK_Greek_rho 0x7f1
const GDK_Greek_sigma 0x7f2
const GDK_Greek_finalsmallsigma 0x7f3
const GDK_Greek_tau 0x7f4
const GDK_Greek_upsilon 0x7f5
const GDK_Greek_phi 0x7f6
const GDK_Greek_chi 0x7f7
const GDK_Greek_psi 0x7f8
const GDK_Greek_omega 0x7f9
const GDK_Greek_switch 0xFF7E
const GDK_leftradical 0x8a1
const GDK_topleftradical 0x8a2
const GDK_horizconnector 0x8a3
const GDK_topintegral 0x8a4
const GDK_botintegral 0x8a5
const GDK_vertconnector 0x8a6
const GDK_topleftsqbracket 0x8a7
const GDK_botleftsqbracket 0x8a8
const GDK_toprightsqbracket 0x8a9
const GDK_botrightsqbracket 0x8aa
const GDK_topleftparens 0x8ab
const GDK_botleftparens 0x8ac
const GDK_toprightparens 0x8ad
const GDK_botrightparens 0x8ae
const GDK_leftmiddlecurlybrace 0x8af
const GDK_rightmiddlecurlybrace 0x8b0
const GDK_topleftsummation 0x8b1
const GDK_botleftsummation 0x8b2
const GDK_topvertsummationconnector 0x8b3
const GDK_botvertsummationconnector 0x8b4
const GDK_toprightsummation 0x8b5
const GDK_botrightsummation 0x8b6
const GDK_rightmiddlesummation 0x8b7
const GDK_lessthanequal 0x8bc
const GDK_notequal 0x8bd
const GDK_greaterthanequal 0x8be
const GDK_integral 0x8bf
const GDK_therefore 0x8c0
const GDK_variation 0x8c1
const GDK_infinity 0x8c2
const GDK_nabla 0x8c5
const GDK_approximate 0x8c8
const GDK_similarequal 0x8c9
const GDK_ifonlyif 0x8cd
const GDK_implies 0x8ce
const GDK_identical 0x8cf
const GDK_radical 0x8d6
const GDK_includedin 0x8da
const GDK_includes 0x8db
const GDK_intersection 0x8dc
const GDK_union 0x8dd
const GDK_logicaland 0x8de
const GDK_logicalor 0x8df
const GDK_partialderivative 0x8ef
const GDK_function 0x8f6
const GDK_leftarrow 0x8fb
const GDK_uparrow 0x8fc
const GDK_rightarrow 0x8fd
const GDK_downarrow 0x8fe
const GDK_blank 0x9df
const GDK_soliddiamond 0x9e0
const GDK_checkerboard 0x9e1
const GDK_ht 0x9e2
const GDK_ff 0x9e3
const GDK_cr 0x9e4
const GDK_lf 0x9e5
const GDK_nl 0x9e8
const GDK_vt 0x9e9
const GDK_lowrightcorner 0x9ea
const GDK_uprightcorner 0x9eb
const GDK_upleftcorner 0x9ec
const GDK_lowleftcorner 0x9ed
const GDK_crossinglines 0x9ee
const GDK_horizlinescan1 0x9ef
const GDK_horizlinescan3 0x9f0
const GDK_horizlinescan5 0x9f1
const GDK_horizlinescan7 0x9f2
const GDK_horizlinescan9 0x9f3
const GDK_leftt 0x9f4
const GDK_rightt 0x9f5
const GDK_bott 0x9f6
const GDK_topt 0x9f7
const GDK_vertbar 0x9f8
const GDK_emspace 0xaa1
const GDK_enspace 0xaa2
const GDK_em3space 0xaa3
const GDK_em4space 0xaa4
const GDK_digitspace 0xaa5
const GDK_punctspace 0xaa6
const GDK_thinspace 0xaa7
const GDK_hairspace 0xaa8
const GDK_emdash 0xaa9
const GDK_endash 0xaaa
const GDK_signifblank 0xaac
const GDK_ellipsis 0xaae
const GDK_doubbaselinedot 0xaaf
const GDK_onethird 0xab0
const GDK_twothirds 0xab1
const GDK_onefifth 0xab2
const GDK_twofifths 0xab3
const GDK_threefifths 0xab4
const GDK_fourfifths 0xab5
const GDK_onesixth 0xab6
const GDK_fivesixths 0xab7
const GDK_careof 0xab8
const GDK_figdash 0xabb
const GDK_leftanglebracket 0xabc
const GDK_decimalpoint 0xabd
const GDK_rightanglebracket 0xabe
const GDK_marker 0xabf
const GDK_oneeighth 0xac3
const GDK_threeeighths 0xac4
const GDK_fiveeighths 0xac5
const GDK_seveneighths 0xac6
const GDK_trademark 0xac9
const GDK_signaturemark 0xaca
const GDK_trademarkincircle 0xacb
const GDK_leftopentriangle 0xacc
const GDK_rightopentriangle 0xacd
const GDK_emopencircle 0xace
const GDK_emopenrectangle 0xacf
const GDK_leftsinglequotemark 0xad0
const GDK_rightsinglequotemark 0xad1
const GDK_leftdoublequotemark 0xad2
const GDK_rightdoublequotemark 0xad3
const GDK_prescription 0xad4
const GDK_minutes 0xad6
const GDK_seconds 0xad7
const GDK_latincross 0xad9
const GDK_hexagram 0xada
const GDK_filledrectbullet 0xadb
const GDK_filledlefttribullet 0xadc
const GDK_filledrighttribullet 0xadd
const GDK_emfilledcircle 0xade
const GDK_emfilledrect 0xadf
const GDK_enopencircbullet 0xae0
const GDK_enopensquarebullet 0xae1
const GDK_openrectbullet 0xae2
const GDK_opentribulletup 0xae3
const GDK_opentribulletdown 0xae4
const GDK_openstar 0xae5
const GDK_enfilledcircbullet 0xae6
const GDK_enfilledsqbullet 0xae7
const GDK_filledtribulletup 0xae8
const GDK_filledtribulletdown 0xae9
const GDK_leftpointer 0xaea
const GDK_rightpointer 0xaeb
const GDK_club 0xaec
const GDK_diamond 0xaed
const GDK_heart 0xaee
const GDK_maltesecross 0xaf0
const GDK_dagger 0xaf1
const GDK_doubledagger 0xaf2
const GDK_checkmark 0xaf3
const GDK_ballotcross 0xaf4
const GDK_musicalsharp 0xaf5
const GDK_musicalflat 0xaf6
const GDK_malesymbol 0xaf7
const GDK_femalesymbol 0xaf8
const GDK_telephone 0xaf9
const GDK_telephonerecorder 0xafa
const GDK_phonographcopyright 0xafb
const GDK_caret 0xafc
const GDK_singlelowquotemark 0xafd
const GDK_doublelowquotemark 0xafe
const GDK_cursor 0xaff
const GDK_leftcaret 0xba3
const GDK_rightcaret 0xba6
const GDK_downcaret 0xba8
const GDK_upcaret 0xba9
const GDK_overbar 0xbc0
const GDK_downtack 0xbc2
const GDK_upshoe 0xbc3
const GDK_downstile 0xbc4
const GDK_underbar 0xbc6
const GDK_jot 0xbca
const GDK_quad 0xbcc
const GDK_uptack 0xbce
const GDK_circle 0xbcf
const GDK_upstile 0xbd3
const GDK_downshoe 0xbd6
const GDK_rightshoe 0xbd8
const GDK_leftshoe 0xbda
const GDK_lefttack 0xbdc
const GDK_righttack 0xbfc
const GDK_hebrew_doublelowline 0xcdf
const GDK_hebrew_aleph 0xce0
const GDK_hebrew_bet 0xce1
const GDK_hebrew_beth 0xce1
const GDK_hebrew_gimel 0xce2
const GDK_hebrew_gimmel 0xce2
const GDK_hebrew_dalet 0xce3
const GDK_hebrew_daleth 0xce3
const GDK_hebrew_he 0xce4
const GDK_hebrew_waw 0xce5
const GDK_hebrew_zain 0xce6
const GDK_hebrew_zayin 0xce6
const GDK_hebrew_chet 0xce7
const GDK_hebrew_het 0xce7
const GDK_hebrew_tet 0xce8
const GDK_hebrew_teth 0xce8
const GDK_hebrew_yod 0xce9
const GDK_hebrew_finalkaph 0xcea
const GDK_hebrew_kaph 0xceb
const GDK_hebrew_lamed 0xcec
const GDK_hebrew_finalmem 0xced
const GDK_hebrew_mem 0xcee
const GDK_hebrew_finalnun 0xcef
const GDK_hebrew_nun 0xcf0
const GDK_hebrew_samech 0xcf1
const GDK_hebrew_samekh 0xcf1
const GDK_hebrew_ayin 0xcf2
const GDK_hebrew_finalpe 0xcf3
const GDK_hebrew_pe 0xcf4
const GDK_hebrew_finalzade 0xcf5
const GDK_hebrew_finalzadi 0xcf5
const GDK_hebrew_zade 0xcf6
const GDK_hebrew_zadi 0xcf6
const GDK_hebrew_qoph 0xcf7
const GDK_hebrew_kuf 0xcf7
const GDK_hebrew_resh 0xcf8
const GDK_hebrew_shin 0xcf9
const GDK_hebrew_taw 0xcfa
const GDK_hebrew_taf 0xcfa
const GDK_Hebrew_switch 0xFF7E
const GDK_Thai_kokai 0xda1
const GDK_Thai_khokhai 0xda2
const GDK_Thai_khokhuat 0xda3
const GDK_Thai_khokhwai 0xda4
const GDK_Thai_khokhon 0xda5
const GDK_Thai_khorakhang 0xda6
const GDK_Thai_ngongu 0xda7
const GDK_Thai_chochan 0xda8
const GDK_Thai_choching 0xda9
const GDK_Thai_chochang 0xdaa
const GDK_Thai_soso 0xdab
const GDK_Thai_chochoe 0xdac
const GDK_Thai_yoying 0xdad
const GDK_Thai_dochada 0xdae
const GDK_Thai_topatak 0xdaf
const GDK_Thai_thothan 0xdb0
const GDK_Thai_thonangmontho 0xdb1
const GDK_Thai_thophuthao 0xdb2
const GDK_Thai_nonen 0xdb3
const GDK_Thai_dodek 0xdb4
const GDK_Thai_totao 0xdb5
const GDK_Thai_thothung 0xdb6
const GDK_Thai_thothahan 0xdb7
const GDK_Thai_thothong 0xdb8
const GDK_Thai_nonu 0xdb9
const GDK_Thai_bobaimai 0xdba
const GDK_Thai_popla 0xdbb
const GDK_Thai_phophung 0xdbc
const GDK_Thai_fofa 0xdbd
const GDK_Thai_phophan 0xdbe
const GDK_Thai_fofan 0xdbf
const GDK_Thai_phosamphao 0xdc0
const GDK_Thai_moma 0xdc1
const GDK_Thai_yoyak 0xdc2
const GDK_Thai_rorua 0xdc3
const GDK_Thai_ru 0xdc4
const GDK_Thai_loling 0xdc5
const GDK_Thai_lu 0xdc6
const GDK_Thai_wowaen 0xdc7
const GDK_Thai_sosala 0xdc8
const GDK_Thai_sorusi 0xdc9
const GDK_Thai_sosua 0xdca
const GDK_Thai_hohip 0xdcb
const GDK_Thai_lochula 0xdcc
const GDK_Thai_oang 0xdcd
const GDK_Thai_honokhuk 0xdce
const GDK_Thai_paiyannoi 0xdcf
const GDK_Thai_saraa 0xdd0
const GDK_Thai_maihanakat 0xdd1
const GDK_Thai_saraaa 0xdd2
const GDK_Thai_saraam 0xdd3
const GDK_Thai_sarai 0xdd4
const GDK_Thai_saraii 0xdd5
const GDK_Thai_saraue 0xdd6
const GDK_Thai_sarauee 0xdd7
const GDK_Thai_sarau 0xdd8
const GDK_Thai_sarauu 0xdd9
const GDK_Thai_phinthu 0xdda
const GDK_Thai_maihanakat_maitho 0xdde
const GDK_Thai_baht 0xddf
const GDK_Thai_sarae 0xde0
const GDK_Thai_saraae 0xde1
const GDK_Thai_sarao 0xde2
const GDK_Thai_saraaimaimuan 0xde3
const GDK_Thai_saraaimaimalai 0xde4
const GDK_Thai_lakkhangyao 0xde5
const GDK_Thai_maiyamok 0xde6
const GDK_Thai_maitaikhu 0xde7
const GDK_Thai_maiek 0xde8
const GDK_Thai_maitho 0xde9
const GDK_Thai_maitri 0xdea
const GDK_Thai_maichattawa 0xdeb
const GDK_Thai_thanthakhat 0xdec
const GDK_Thai_nikhahit 0xded
const GDK_Thai_leksun 0xdf0
const GDK_Thai_leknung 0xdf1
const GDK_Thai_leksong 0xdf2
const GDK_Thai_leksam 0xdf3
const GDK_Thai_leksi 0xdf4
const GDK_Thai_lekha 0xdf5
const GDK_Thai_lekhok 0xdf6
const GDK_Thai_lekchet 0xdf7
const GDK_Thai_lekpaet 0xdf8
const GDK_Thai_lekkao 0xdf9
const GDK_Hangul 0xff31
const GDK_Hangul_Start 0xff32
const GDK_Hangul_End 0xff33
const GDK_Hangul_Hanja 0xff34
const GDK_Hangul_Jamo 0xff35
const GDK_Hangul_Romaja 0xff36
const GDK_Hangul_Codeinput 0xff37
const GDK_Hangul_Jeonja 0xff38
const GDK_Hangul_Banja 0xff39
const GDK_Hangul_PreHanja 0xff3a
const GDK_Hangul_PostHanja 0xff3b
const GDK_Hangul_SingleCandidate 0xff3c
const GDK_Hangul_MultipleCandidate 0xff3d
const GDK_Hangul_PreviousCandidate 0xff3e
const GDK_Hangul_Special 0xff3f
const GDK_Hangul_switch 0xFF7E
const GDK_Hangul_Kiyeog 0xea1
const GDK_Hangul_SsangKiyeog 0xea2
const GDK_Hangul_KiyeogSios 0xea3
const GDK_Hangul_Nieun 0xea4
const GDK_Hangul_NieunJieuj 0xea5
const GDK_Hangul_NieunHieuh 0xea6
const GDK_Hangul_Dikeud 0xea7
const GDK_Hangul_SsangDikeud 0xea8
const GDK_Hangul_Rieul 0xea9
const GDK_Hangul_RieulKiyeog 0xeaa
const GDK_Hangul_RieulMieum 0xeab
const GDK_Hangul_RieulPieub 0xeac
const GDK_Hangul_RieulSios 0xead
const GDK_Hangul_RieulTieut 0xeae
const GDK_Hangul_RieulPhieuf 0xeaf
const GDK_Hangul_RieulHieuh 0xeb0
const GDK_Hangul_Mieum 0xeb1
const GDK_Hangul_Pieub 0xeb2
const GDK_Hangul_SsangPieub 0xeb3
const GDK_Hangul_PieubSios 0xeb4
const GDK_Hangul_Sios 0xeb5
const GDK_Hangul_SsangSios 0xeb6
const GDK_Hangul_Ieung 0xeb7
const GDK_Hangul_Jieuj 0xeb8
const GDK_Hangul_SsangJieuj 0xeb9
const GDK_Hangul_Cieuc 0xeba
const GDK_Hangul_Khieuq 0xebb
const GDK_Hangul_Tieut 0xebc
const GDK_Hangul_Phieuf 0xebd
const GDK_Hangul_Hieuh 0xebe
const GDK_Hangul_A 0xebf
const GDK_Hangul_AE 0xec0
const GDK_Hangul_YA 0xec1
const GDK_Hangul_YAE 0xec2
const GDK_Hangul_EO 0xec3
const GDK_Hangul_E 0xec4
const GDK_Hangul_YEO 0xec5
const GDK_Hangul_YE 0xec6
const GDK_Hangul_O 0xec7
const GDK_Hangul_WA 0xec8
const GDK_Hangul_WAE 0xec9
const GDK_Hangul_OE 0xeca
const GDK_Hangul_YO 0xecb
const GDK_Hangul_U 0xecc
const GDK_Hangul_WEO 0xecd
const GDK_Hangul_WE 0xece
const GDK_Hangul_WI 0xecf
const GDK_Hangul_YU 0xed0
const GDK_Hangul_EU 0xed1
const GDK_Hangul_YI 0xed2
const GDK_Hangul_I 0xed3
const GDK_Hangul_J_Kiyeog 0xed4
const GDK_Hangul_J_SsangKiyeog 0xed5
const GDK_Hangul_J_KiyeogSios 0xed6
const GDK_Hangul_J_Nieun 0xed7
const GDK_Hangul_J_NieunJieuj 0xed8
const GDK_Hangul_J_NieunHieuh 0xed9
const GDK_Hangul_J_Dikeud 0xeda
const GDK_Hangul_J_Rieul 0xedb
const GDK_Hangul_J_RieulKiyeog 0xedc
const GDK_Hangul_J_RieulMieum 0xedd
const GDK_Hangul_J_RieulPieub 0xede
const GDK_Hangul_J_RieulSios 0xedf
const GDK_Hangul_J_RieulTieut 0xee0
const GDK_Hangul_J_RieulPhieuf 0xee1
const GDK_Hangul_J_RieulHieuh 0xee2
const GDK_Hangul_J_Mieum 0xee3
const GDK_Hangul_J_Pieub 0xee4
const GDK_Hangul_J_PieubSios 0xee5
const GDK_Hangul_J_Sios 0xee6
const GDK_Hangul_J_SsangSios 0xee7
const GDK_Hangul_J_Ieung 0xee8
const GDK_Hangul_J_Jieuj 0xee9
const GDK_Hangul_J_Cieuc 0xeea
const GDK_Hangul_J_Khieuq 0xeeb
const GDK_Hangul_J_Tieut 0xeec
const GDK_Hangul_J_Phieuf 0xeed
const GDK_Hangul_J_Hieuh 0xeee
const GDK_Hangul_RieulYeorinHieuh 0xeef
const GDK_Hangul_SunkyeongeumMieum 0xef0
const GDK_Hangul_SunkyeongeumPieub 0xef1
const GDK_Hangul_PanSios 0xef2
const GDK_Hangul_KkogjiDalrinIeung 0xef3
const GDK_Hangul_SunkyeongeumPhieuf 0xef4
const GDK_Hangul_YeorinHieuh 0xef5
const GDK_Hangul_AraeA 0xef6
const GDK_Hangul_AraeAE 0xef7
const GDK_Hangul_J_PanSios 0xef8
const GDK_Hangul_J_KkogjiDalrinIeung 0xef9
const GDK_Hangul_J_YeorinHieuh 0xefa
const GDK_Korean_Won 0xeff
const GDK_EcuSign 0x20a0
const GDK_ColonSign 0x20a1
const GDK_CruzeiroSign 0x20a2
const GDK_FFrancSign 0x20a3
const GDK_LiraSign 0x20a4
const GDK_MillSign 0x20a5
const GDK_NairaSign 0x20a6
const GDK_PesetaSign 0x20a7
const GDK_RupeeSign 0x20a8
const GDK_WonSign 0x20a9
const GDK_NewSheqelSign 0x20aa
const GDK_DongSign 0x20ab
const GDK_EuroSign 0x20ac
End Rem
