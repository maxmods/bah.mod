SuperStrict

Framework bah.gtkmaxgui

Local window:TGadget

window=CreateWindow("My Window",40,40,320,240, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE)

Local hbox:Byte Ptr = gtk_hbox_new(False, 0)
gtk_widget_show(hbox)

Local statusbar:Byte Ptr[] = New Byte Ptr[3]
statusbar[0] = gtk_statusbar_new()
gtk_statusbar_set_has_resize_grip(statusbar[0],False)
gtk_widget_show(statusbar[0])

gtk_box_pack_start(hbox, statusbar[0], True, True, 0)

gtk_statusbar_push(statusbar[0], 0, "Hello there1")


statusbar[1] = gtk_statusbar_new()
gtk_statusbar_set_has_resize_grip(statusbar[1],False)
gtk_widget_show(statusbar[1])

gtk_box_pack_start(hbox, statusbar[1], True, True, 0)

gtk_statusbar_push(statusbar[1], 0, "Hello there2")


statusbar[2] = gtk_statusbar_new()
gtk_statusbar_set_has_resize_grip(statusbar[2],True)
gtk_widget_show(statusbar[2])


gtk_box_pack_end(hbox, statusbar[2], True, True, 0)

gtk_statusbar_push(statusbar[2], 0, "Hello there3")
'gtk_frame_set_label_align(Byte Ptr(Int Ptr(statusbar[2] + 76)[0]), 1, 0.5)


gtk_box_pack_start(TGTKWindow(window).vbox, hbox, False, False, 0)


Rem
Local vbox:Byte Ptr = gtk_vbox_new(False, 0)
gtk_widget_show(vbox)

Local sep:Byte Ptr = gtk_hseparator_new()
gtk_widget_show(sep)
Local label:Byte Ptr = gtk_label_new("Hello there")
gtk_widget_show(label)

gtk_box_pack_start(vbox, sep, False, True, 0)
gtk_box_pack_start(vbox, label, True, True, 0)


gtk_box_pack_start(statusbar, vbox, True, True, 0)

vbox = gtk_vbox_new(False, 0)
gtk_widget_show(vbox)

sep = gtk_hseparator_new()
gtk_widget_show(sep)

label = gtk_label_new("Hello there")
gtk_misc_set_alignment(label, 1, 0.5)
gtk_widget_show(label)

gtk_box_pack_start(vbox, sep, False, True, 0)
gtk_box_pack_start(vbox, label, True, True, 0)

gtk_box_pack_start(statusbar, vbox, True, True, 0)
End Rem

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

