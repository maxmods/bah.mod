SuperStrict

Framework bah.gtkmaxgui

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)

Local hbox:Byte Ptr = gtk_hbox_new(False, 0)
gtk_widget_show(hbox)


Local vbox:Byte Ptr = gtk_vbox_new(False, 0)
gtk_widget_show(vbox)

Local sep:Byte Ptr = gtk_hseparator_new()
gtk_widget_show(sep)
Local label:Byte Ptr = gtk_label_new("Hello there")
gtk_misc_set_alignment(label, 0, 0.5)
gtk_widget_show(label)

gtk_box_pack_start(vbox, sep, False, True, 0)
gtk_box_pack_start(vbox, label, True, True, 0)

gtk_box_pack_start(hbox, vbox, True, True, 0)


vbox = gtk_vbox_new(False, 0)
gtk_widget_show(vbox)

sep = gtk_hseparator_new()
gtk_widget_show(sep)

label = gtk_label_new("Hello there")
'gtk_misc_set_alignment(label, 1, 0.5)
gtk_widget_show(label)

gtk_box_pack_start(vbox, sep, False, True, 0)
gtk_box_pack_start(vbox, label, True, True, 0)

gtk_box_pack_start(hbox, vbox, True, True, 0)

vbox = gtk_vbox_new(False, 0)
gtk_widget_show(vbox)



sep = gtk_hseparator_new()
gtk_widget_show(sep)

label = gtk_label_new("Hello there")
gtk_misc_set_alignment(label, 1, 0.5)
gtk_widget_show(label)

gtk_box_pack_start(vbox, sep, False, True, 0)
gtk_box_pack_start(vbox, label, True, True, 0)

gtk_box_pack_start(hbox, vbox, True, True, 0)


'Local hb:Byte Ptr = gtk_vbox_new(False, 0)
'gtk_widget_show(hb)
Local statusbar:Byte Ptr = gtk_statusbar_new()
gtk_statusbar_set_has_resize_grip(statusbar,True)
gtk_widget_show(statusbar)

gtk_statusbar_push(statusbar, 0, "     ")

gtk_box_pack_start(hbox, statusbar, False, True, 0)
'gtk_box_pack_start(hbox, hb, False, True, 0)


gtk_box_pack_start(TGTKWindow(window).vbox, hbox, False, False, 0)


While True
	WaitEvent 
	Print CurrentEvent.ToString()
	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
	End Select
	
'	Print "max = " + WindowMaximized(window)
'	Print "min = " + WindowMinimized(window)
Wend
