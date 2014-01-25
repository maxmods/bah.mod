' Copyright (c) 2006-2014 Bruce A Henderson
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

Import "gtkkeymap.bmx"

Import BRL.System
Import MaxGUI.MaxGUI
Import BRL.HTTPStream
Import BRL.RAMStream
Import BRL.Pixmap
Import BRL.BMPLoader
Import BRL.PNGLoader
Import BRL.Map

?linux
Import "-L/usr/lib"
Import "-L/opt/gnome/lib" ' some distros keep libs here
Import "-lglib-2.0"
Import "-lgtk-x11-2.0"
Import "-lgdk-x11-2.0"
Import "-latk-1.0"
'Import "-lpangox-1.0"
Import "-lpango-1.0"
Import "-lgobject-2.0"
Import "-lgmodule-2.0"
Import "-lgdk_pixbuf-2.0"
?

Incbin "gtk_triangle.png"

Extern

	Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function gtk_init(argc:Int, argv:Byte Ptr Ptr Ptr)
	Function gtk_main_iteration_do:Int(blocking:Int)
	Function gtk_main_quit()
	Function gtk_events_pending:Int()

	Function gtk_get_current_event_time:Int()

	Function g_signal_cb7:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr, drawable:Byte Ptr, val1:Int,val2:Int,val3:Int,val4:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb5:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,val1:Int,val2:Int,val3:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb6:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,context:Byte Ptr, val1:Int,val2:Int,val3:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb4a:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,val1:Int,val2:Double,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb4:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,url:Byte Ptr,stream:Byte Ptr,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb_int_3:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,arg:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb3:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,event:Byte Ptr,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb_mouse_3:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,event:TGdkEventButton,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb_splitter_3:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,scroll:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_cb2:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"
	Function g_signal_handler_disconnect(gtkwidget:Byte Ptr, handlerid:Long)
	Function g_signal_stop_emission_by_name(gtkwidget:Byte Ptr, name:Byte Ptr)
	Function g_signal_tabchange:Int(gtkwidget:Byte Ptr, name:Byte Ptr, callback(widget:Byte Ptr,a:Byte Ptr, index:Int,gadget:Object),gadget:Object,destroyhandler(data:Byte Ptr,user: Byte Ptr),flag:Int) = "g_signal_connect_data"

	Function gtk_window_new:Byte Ptr(windowtype:Int)
	Function gtk_window_set_title(windowPtr:Byte Ptr, title:Byte Ptr)
	Function gtk_window_move(windowPtr:Byte Ptr, x:Int, y:Int)
	Function gtk_window_set_default_size(windowPtr:Byte Ptr, w:Int, h:Int)
	Function gtk_window_set_resizable(windowPtr:Byte Ptr, bool:Int)
	Function gtk_window_set_decorated(windowPtr:Byte Ptr, bool:Int)
	Function gtk_window_add_accel_group(windowPtr:Byte Ptr, accel:Byte Ptr)
	Function gtk_window_maximize(windowPtr:Byte Ptr)
	Function gtk_window_unmaximize(windowPtr:Byte Ptr)
	Function gtk_window_iconify(windowPtr:Byte Ptr)
	Function gtk_window_deiconify(windowPtr:Byte Ptr)
	Function gtk_window_get_size(windowPtr:Byte Ptr, width:Int Ptr, height:Int Ptr)
	Function gtk_window_set_geometry_hints(window:Byte Ptr, geometry_widget:Byte Ptr, geometry:Byte Ptr, geom_mask:Int)
	Function gtk_window_get_focus:Byte Ptr(window:Byte Ptr)
	Function gtk_window_set_type_hint(window:Byte Ptr, hint:Int)
	Function gtk_window_set_icon(window:Byte Ptr, pixbuf:Int)
	Function gtk_window_present(window:Byte Ptr)
	Function gtk_window_set_default_icon(pixbuf:Int)
	Function gtk_window_resize(window:Byte Ptr, width:Int, height:Int)
	Function gtk_window_get_title:Byte Ptr(window:Byte Ptr)
	Function gtk_window_set_transient_for(window:Byte Ptr, parent:Byte Ptr)
	Function gtk_window_get_position(window:Byte Ptr, x:Int Ptr, y:Int Ptr)
	Function gtk_widget_set_uposition(window:Byte Ptr, x:Int, y:Int)

	Function gtk_widget_destroy(widgetPtr:Byte Ptr)
	Function gtk_widget_show(widgetPtr:Byte Ptr)
	Function gtk_widget_hide(widgetPtr:Byte Ptr)
	Function gtk_widget_set_size_request(widgetPtr:Byte Ptr, w:Int, h:Int)
	Function gtk_widget_set_sensitive(widgetPtr:Byte Ptr, bool:Int)
	Function gtk_widget_size_request(widgetPtr:Byte Ptr, req:Byte Ptr)
	Function gtk_widget_size_allocate(widgetPtr:Byte Ptr, alloc:Byte Ptr)
	Function gtk_widget_modify_bg(widgetPtr:Byte Ptr, state:Int, color:Byte Ptr)
	Function gtk_widget_modify_fg(widgetPtr:Byte Ptr, state:Int, color:Byte Ptr)
	Function gtk_widget_modify_base(widgetPtr:Byte Ptr, state:Int, color:Byte Ptr)
	Function gtk_widget_add_events(widgetPtr:Byte Ptr, _mask:Int)
	Function gtk_widget_activate:Int(widgetPtr:Byte Ptr)
	Function gtk_widget_modify_font(widgetPtr:Byte Ptr, pango:Byte Ptr)
	Function gtk_widget_grab_focus(widgetPtr:Byte Ptr)
	Function gtk_widget_grab_default(widgetPtr:Byte Ptr)
	Function gtk_widget_modify_text(widgetPtr:Byte Ptr, style:Int, color:Byte Ptr)
	Function gtk_widget_add_accelerator(widgetPtr:Byte Ptr, signal:Byte Ptr, group:Byte Ptr, key:Int, modtype:Int, flags:Int)
	Function gtk_widget_queue_draw_area(widgetPtr:Byte Ptr, x:Int, y:Int, width:Int, height:Int)
	Function gtk_widget_set_double_buffered(widgetPtr:Byte Ptr, bool:Int)
	Function gtk_widget_queue_draw(widgetPtr:Byte Ptr)
	Function gtk_widget_queue_resize(widgetPtr:Byte Ptr)
	Function gtk_widget_get_pointer(widget:Byte Ptr, x:Int Var, y:Int Var)
	Function gtk_widget_get_parent_window:Byte Ptr(widget:Byte Ptr)
	Function gdk_window_get_position(window:Byte Ptr, x:Int Var, y:Int Var)
	Function gtk_widget_set_accel_path(window:Byte Ptr, path:Byte Ptr, accelGroup:Byte Ptr)
	Function gtk_widget_unparent(window:Byte Ptr)
	Function gtk_widget_reparent(widget:Byte Ptr, newParent:Byte Ptr)
	Function gtk_widget_get_state:Int(widget:Byte Ptr)
	Function gtk_widget_get_visible:Int(widget:Byte Ptr)
	Function gtk_widget_is_sensitive:Int(widget:Byte Ptr)
	Function gtk_widget_has_focus:Int(widget:Byte Ptr)
	Function gtk_widget_get_window:Byte Ptr(widget:Byte Ptr)
	Function gtk_widget_get_allocation(widget:Byte Ptr, allocation:Byte Ptr)

	Function gtk_button_new:Byte Ptr()
	Function gtk_button_new_with_label:Byte Ptr(label:Byte Ptr)
	Function gtk_radio_button_new_with_label:Byte Ptr(group:Byte Ptr, label:Byte Ptr)
	Function gtk_check_button_new_with_label:Byte Ptr(label:Byte Ptr)
	Function gtk_toggle_button_set_active(widgetPtr:Byte Ptr, bool:Int)
	Function gtk_toggle_button_get_active:Int(widgetPtr:Byte Ptr)
	Function gtk_button_set_label(widgetPtr:Byte Ptr, label:Byte Ptr)
	Function gtk_button_new_from_stock:Byte Ptr(label:Byte Ptr)
	Function gtk_radio_button_get_group:Byte Ptr(button:Byte Ptr)
	Function gtk_button_get_label:Byte Ptr(button:Byte Ptr)
	Function gtk_button_set_use_stock(button:Byte Ptr, bool:Int)
	Function gtk_button_set_use_underline(button:Byte Ptr, bool:Int)
	Function gtk_button_set_image(button:Byte Ptr, image:Byte Ptr)
	Function gtk_button_set_image_position(button:Byte Ptr, pos:Int)
	Function gtk_button_set_alignment(button:Byte Ptr, xalign:Float, yalign:Float)
	Function gtk_button_get_alignment(button:Byte Ptr, xalign:Float Ptr, yalign:Float Ptr)

	Function gtk_fixed_new:Byte Ptr()
	Function gtk_fixed_put(container:Byte Ptr, widget:Byte Ptr, x:Int, y:Int)
	Function gtk_fixed_move(container:Byte Ptr, widget:Byte Ptr, x:Int, y:Int)
	Function gtk_container_add(container:Byte Ptr, widget:Byte Ptr)
	Function gtk_container_set_resize_mode(container:Byte Ptr, Mode:Int)
	Function gtk_container_remove(container:Byte Ptr, widget:Byte Ptr)
	Function gtk_container_get_border_width:Int(container:Byte Ptr)
	Function gtk_container_get_resize_mode:Int(container:Byte Ptr)

	Function gtk_vseparator_new:Byte Ptr()
	Function gtk_hseparator_new:Byte Ptr()
	Function gtk_frame_new:Byte Ptr(label:Byte Ptr)
	Function gtk_frame_set_shadow_type(frame:Byte Ptr, shadow:Int)
	Function gtk_frame_set_label(frame:Byte Ptr, label:Byte Ptr)
	Function gtk_frame_set_label_align(frame:Byte Ptr, xalign:Float, yalign:Float)

	Function gtk_label_new:Byte Ptr(label:Byte Ptr)
	Function gtk_label_set_text(labelPtr:Byte Ptr, label:Byte Ptr)
	Function gtk_label_set_justify(labelPtr:Byte Ptr, justify:Int)
	Function gtk_label_get_text:Byte Ptr(labelPtr:Byte Ptr)
	Function gtk_label_set_text_with_mnemonic(labelPtr:Byte Ptr, label:Byte Ptr)

	Function gtk_misc_set_alignment(widgetPtr:Byte Ptr, x:Float, y:Float)
	Function gtk_misc_get_padding(widgetPrt:Byte Ptr, xpad:Int Ptr, ypad:Int Ptr)
	Function gtk_misc_set_padding(widgetPrt:Byte Ptr, xpad:Int, ypad:Int)
	
	Function gtk_bin_get_child:Byte Ptr(widgetPtr:Byte Ptr)
	Function gtk_vbox_new:Byte Ptr(homogeneous:Int, spacing:Int)
	Function gtk_hbox_new:Byte Ptr(homogeneous:Int, spacing:Int)
	Function gtk_box_pack_start(box:Byte Ptr, child:Byte Ptr, expand:Int, _fill:Int, padding:Int)
	Function gtk_box_reorder_child(box:Byte Ptr, child:Byte Ptr, pos:Int)
	Function gtk_box_pack_end(box:Byte Ptr, child:Byte Ptr, expand:Int, _fill:Int, padding:Int)

	Function gtk_statusbar_new:Byte Ptr()
	Function gtk_statusbar_set_has_resize_grip(statusbarPtr:Byte Ptr, bool:Int)
	Function gtk_statusbar_pop(statusbarPtr:Byte Ptr, id:Int)
	Function gtk_statusbar_push(statusbarPtr:Byte Ptr, id:Int, txt:Byte Ptr)

	Function gtk_menu_bar_new:Byte Ptr()
	Function gtk_menu_new:Byte Ptr()
	Function gtk_menu_item_new_with_label:Byte Ptr(label:Byte Ptr)
	Function gtk_check_menu_item_new_with_label:Byte Ptr(label:Byte Ptr)
	Function gtk_menu_shell_append(menu:Byte Ptr, menuitem:Byte Ptr)
	Function gtk_menu_item_set_submenu(menuitem:Byte Ptr, submenu:Byte Ptr)
	Function gtk_separator_menu_item_new:Byte Ptr()
	Function gtk_menu_item_new_with_mnemonic:Byte Ptr(label:Byte Ptr)
	Function gtk_check_menu_item_new_with_mnemonic:Byte Ptr(label:Byte Ptr)
	Function gtk_check_menu_item_set_active(handle:Byte Ptr, bool:Int)
	Function gtk_check_menu_item_get_active:Int(handle:Byte Ptr)
	Function gtk_menu_shell_insert:Byte Ptr(menu:Byte Ptr, item:Byte Ptr, pos:Int)
	Function gtk_image_menu_item_new_from_stock:Byte Ptr(txt:Byte Ptr, accel_group:Byte Ptr)
	Function gtk_menu_popup(menu:Byte Ptr, parent_menu_shell:Byte Ptr, parent_menu_item:Byte Ptr, ..
				func:Byte Ptr, data:Byte Ptr, button:Int, activate_time:Int)
	Function gtk_menu_item_set_accel_path(menu:Byte Ptr, path:Byte Ptr)
	Function gtk_image_menu_item_set_image(menu:Byte Ptr, image:Byte Ptr)
	Function gtk_image_menu_item_new_with_label:Byte Ptr(label:Byte Ptr)
	Function gtk_image_menu_item_new_with_mnemonic:Byte Ptr(label:Byte Ptr)

	Function g_object_set_data(handle:Byte Ptr, name:Byte Ptr, pointer:Object)
	Function g_object_unref(handle:Byte Ptr)
	Function g_object_set_int(handle:Byte Ptr, property:Byte Ptr, value:Int, _null:Byte Ptr=Null) = "g_object_set"
	Function g_object_set_double(handle:Byte Ptr, property:Byte Ptr, value:Double, _null:Byte Ptr=Null) = "g_object_set"
	Function g_object_ref:Byte Ptr(handle:Byte Ptr)

	Function gtk_entry_new:Byte Ptr()
	Function gtk_entry_set_visibility(handle:Byte Ptr, bool:Int)
	Function gtk_editable_set_editable(handle:Byte Ptr, bool:Int)
	Function gtk_entry_get_text:Byte Ptr(handle:Byte Ptr)
	Function gtk_entry_set_text(handle:Byte Ptr, txt:Byte Ptr)
	Function gtk_editable_cut_clipboard(handle:Byte Ptr)
	Function gtk_editable_copy_clipboard(handle:Byte Ptr)
	Function gtk_editable_paste_clipboard(handle:Byte Ptr)

	Function gtk_scrolled_window_new:Byte Ptr(hadjustment:Byte Ptr, vadjustment:Byte Ptr)
	Function gtk_scrolled_window_set_policy(handle:Byte Ptr, hpolicy:Int, vpolicy:Int)

	Function gtk_notebook_new:Byte Ptr()
	Function gtk_notebook_insert_page(handle:Byte Ptr, child:Byte Ptr, label:Byte Ptr, index:Int)
	Function gtk_notebook_get_current_page:Int(handle:Byte Ptr)
	Function gtk_notebook_get_nth_page:Byte Ptr(handle:Byte Ptr, index:Int)
	Function gtk_notebook_get_tab_label:Byte Ptr(handle:Byte Ptr, child:Byte Ptr)
	Function gtk_notebook_remove_page:Int(handle:Byte Ptr, index:Int)
	Function gtk_notebook_set_current_page(handle:Byte Ptr, index:Int)
	Function gtk_notebook_set_scrollable(handle:Byte Ptr, bool:Int)
	Function gtk_notebook_get_n_pages:Int(handle:Byte Ptr)
	
	Function gtk_drawing_area_new:Byte Ptr()

	Function gtk_combo_box_new_text:Byte Ptr()
	Function gtk_combo_box_entry_new_text:Byte Ptr()
	Function gtk_combo_box_entry_new:Byte Ptr()
	Function gtk_combo_box_insert_text(handle:Byte Ptr, position:Int, text:Byte Ptr)
	Function gtk_combo_box_remove_text(handle:Byte Ptr, position:Int)
	Function gtk_combo_box_get_active:Int(handle:Byte Ptr)
	Function gtk_combo_box_set_active:Int(handle:Byte Ptr, position:Int)
	Function gtk_combo_box_get_active_text:Byte Ptr(handle:Byte Ptr)
	Function gtk_combo_box_set_model(handle:Byte Ptr, model:Byte Ptr)
	Function gtk_combo_box_entry_set_text_column(handle:Byte Ptr, index:Int)
	
	Function gtk_cell_layout_clear(handle:Byte Ptr)
	Function gtk_cell_layout_pack_start(handle:Byte Ptr, renderer:Byte Ptr, bool:Int)
	Function gtk_cell_layout_pack_end(handle:Byte Ptr, renderer:Byte Ptr, bool:Int)
	Function gtk_cell_layout_add_attribute(handle:Byte Ptr, renderer:Byte Ptr, desc:Byte Ptr, ind:Int)
	
	Function gtk_progress_bar_new:Byte Ptr()
	Function gtk_progress_bar_set_fraction(handle:Byte Ptr, fraction:Double)
	Function gtk_progress_bar_get_fraction:Double(handle:Byte Ptr)

	Function gtk_hscrollbar_new:Byte Ptr(adj:Byte Ptr)
	Function gtk_vscrollbar_new:Byte Ptr(adj:Byte Ptr)
	Function gtk_vscale_new_with_range:Byte Ptr(_min:Double, _max:Double, _step:Double)
	Function gtk_hscale_new_with_range:Byte Ptr(_min:Double, _max:Double, _step:Double)
	Function gtk_range_set_value(handle:Byte Ptr, value:Double)
	Function gtk_range_get_value:Double(handle:Byte Ptr)
	Function gtk_range_set_range(handle:Byte Ptr, _min:Double, _max:Double)
	Function gtk_scale_set_draw_value(handle:Byte Ptr, bool:Int)
	Function gtk_range_set_increments(handle:Byte Ptr, _step:Double, _page:Double)
	Function gtk_range_set_round_digits(handle:Byte Ptr, digits:Int)
	Function gtk_range_get_adjustment:Byte Ptr(handle:Byte Ptr)
	Function gtk_adjustment_set_page_size(handle:Byte Ptr, size:Int)

 	Function gtk_text_buffer_new:Byte Ptr(tagtable:Byte Ptr)
	Function gtk_text_view_new_with_buffer:Byte Ptr(buffer:Byte Ptr)
	Function gtk_text_buffer_set_text(buffer:Byte Ptr, text:Byte Ptr, textlen:Int)
	Function gtk_text_view_set_wrap_mode(handle:Byte Ptr, Mode:Int)
	Function gtk_text_view_set_editable(handle:Byte Ptr, bool:Int)
	Function gtk_text_buffer_get_selection_bounds:Int(buffer:Byte Ptr, iterstart:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_iter_get_line:Int(iter:Byte Ptr)
	Function gtk_text_iter_get_offset:Int(iter:Byte Ptr)
	Function gtk_text_buffer_get_iter_at_offset(buffer:Byte Ptr, iter:Byte Ptr, offset:Int)
	Function gtk_text_buffer_get_iter_at_line(buffer:Byte Ptr, iter:Byte Ptr, line:Int)
	Function gtk_text_buffer_select_range(buffer:Byte Ptr, iterstar:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_buffer_get_insert:Byte Ptr(buffer:Byte Ptr)
	Function gtk_text_buffer_get_iter_at_mark(buffer:Byte Ptr, iter:Byte Ptr, mark:Byte Ptr)
	Function gtk_text_buffer_insert(buffer:Byte Ptr, iter:Byte Ptr, text:Byte Ptr, length:Int)
	Function gtk_text_buffer_get_end_iter(buffer:Byte Ptr, iter:Byte Ptr)
	Function gtk_text_buffer_get_line_count:Int(buffer:Byte Ptr)
	Function gtk_text_buffer_get_char_count:Int(buffer:Byte Ptr)
	Function gtk_text_buffer_get_text:Byte Ptr(buffer:Byte Ptr, iterstart:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_buffer_get_tag_table:Byte Ptr(buffer:Byte Ptr)
	Function gtk_text_tag_table_lookup:Byte Ptr(tagtable:Byte Ptr, name:Byte Ptr)
	Function gtk_text_tag_new:Byte Ptr(name:Byte Ptr)
	Function gtk_set_text_tag:Byte Ptr(buffer:Byte Ptr, name:Byte Ptr, _txtfg:Byte Ptr, _fg:Byte Ptr, ..
					_txtstyle:Byte Ptr, _style:Int, _txtweight:Byte Ptr, _weight:Int, ..
					_txtunder:Byte Ptr, _under:Int, _txtstrike:Byte Ptr, _strike:Int, _null:Byte Ptr=Null) = "gtk_text_buffer_create_tag"
	Function gtk_set_text_bg_tag:Byte Ptr(buffer:Byte Ptr, name:Byte Ptr, _txtfg:Byte Ptr, _bg:Byte Ptr, ..
					 _null:Byte Ptr=Null) = "gtk_text_buffer_create_tag"
	Function gtk_text_tag_table_add(_tagtable:Byte Ptr, _tag:Byte Ptr)
	Function gtk_text_buffer_apply_tag(buffer:Byte Ptr, _tag:Byte Ptr, iterstart:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_buffer_remove_all_tags(buffer:Byte Ptr, iterstart:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_buffer_delete(buffer:Byte Ptr, iterstart:Byte Ptr, iterend:Byte Ptr)
	Function gtk_text_view_get_tabs:Byte Ptr(handle:Byte Ptr)
	Function gtk_text_view_set_tabs(handle:Byte Ptr, tabarray:Byte Ptr)
	Function pango_tab_array_new_with_positions:Byte Ptr(size:Int, pixels:Int, align:Int, pos:Int)
	Function pango_tab_array_set_tab(tabarray:Byte Ptr, index:Int, align:Int, pos:Int)
	Function pango_tab_array_get_tab(tabarray:Byte Ptr, index:Int, align:Byte Ptr, pos:Byte Ptr)
	Function pango_tab_array_free(tabarray:Byte Ptr)
	Function gtk_text_view_scroll_to_iter(handle:Byte Ptr, iter:Byte Ptr, margin:Double, align:Int, xalign:Double, yalign:Double)
	Function gtk_text_buffer_paste_clipboard(buffer:Byte Ptr, clipboard:Byte Ptr, override:Byte Ptr, editable:Int)
	Function gtk_text_buffer_copy_clipboard(buffer:Byte Ptr, clipboard:Byte Ptr)
	Function gtk_text_buffer_cut_clipboard(buffer:Byte Ptr, clipboard:Byte Ptr, editable:Int)
	Function gtk_text_buffer_move_mark(buffer:Byte Ptr, mark:Byte Ptr, iter:Byte Ptr)
	Function gtk_text_view_scroll_to_mark(handle:Byte Ptr, mark:Byte Ptr, margin:Double, align:Int, xalign:Double, yalign:Double)
	Function gtk_text_buffer_place_cursor(buffer:Byte Ptr, iter:Byte Ptr)
	Function gtk_text_view_scroll_mark_onscreen(handle:Byte Ptr, mark:Byte Ptr)
	
	Function pango_font_description_new:Byte Ptr()
	Function pango_font_description_set_family(pango:Byte Ptr, family:Byte Ptr)
	Function pango_font_description_set_weight(pango:Byte Ptr, weight:Int)
	Function pango_font_description_set_style(pango:Byte Ptr, style:Int)
	Function pango_font_description_set_size(pango:Byte Ptr, size:Int)
	Function pango_font_description_set_absolute_size(pango:Byte Ptr, size:Double)
	Function pango_font_description_free(pango:Byte Ptr)
	Function pango_font_description_from_string:Byte Ptr(name:Byte Ptr)
	Function pango_font_description_get_style:Int(pango:Byte Ptr)
	Function pango_font_description_get_weight:Int(pango:Byte Ptr)
	Function pango_font_description_get_size:Int(pango:Byte Ptr)
	Function pango_font_description_get_family:Byte Ptr(pango:Byte Ptr)
	Function pango_font_description_to_string:Byte Ptr(pango:Byte Ptr)

	Function pango_font_describe:Byte Ptr(font:Byte Ptr)
	Function pango_context_load_fontset:Byte Ptr(context:Byte Ptr, desc:Byte Ptr, lang:Byte Ptr)
	Function pango_fontset_foreach(fontset:Byte Ptr, callback(_set:Byte Ptr, _font:Byte Ptr ,data:Object), data:Object)
	Function gtk_widget_get_pango_context:Byte Ptr(widgetPtr:Byte Ptr)

	Function gtk_font_selection_dialog_new:Byte Ptr(title:Byte Ptr)
	Function gtk_font_selection_dialog_set_font_name(handle:Byte Ptr, name:Byte Ptr)
	Function gtk_font_selection_dialog_get_font_name:Byte Ptr(handle:Byte Ptr)
	Function gtk_dialog_run:Int(handle:Byte Ptr)
	Function gtk_dialog_add_button:Byte Ptr(handle:Byte Ptr, text:Byte Ptr, id:Int)

	Function gtk_color_selection_dialog_new:Byte Ptr(title:Byte Ptr)
	Function gtk_color_selection_set_current_color(handle:Byte Ptr, color:Byte Ptr)
	Function gtk_color_selection_get_current_color(handle:Byte Ptr, color:Byte Ptr)
	Function gtk_color_selection_dialog_get_color_selection:Byte Ptr(handle:Byte Ptr)

	Function gtk_file_chooser_dialog_new:Byte Ptr(title:Byte Ptr, parent:Byte Ptr, ..
				action:Int, but1:Byte Ptr, opt1:Int, but2:Byte Ptr, opt2:Int, opt3:Byte Ptr)
	Function gtk_file_chooser_get_filename:Byte Ptr(handle:Byte Ptr)
	Function gtk_file_chooser_set_filename(handle:Byte Ptr, name:Byte Ptr)
	Function gtk_file_chooser_set_current_folder(handle:Byte Ptr, folder:Byte Ptr)
	Function gtk_file_chooser_add_filter(handle:Byte Ptr, filter:Byte Ptr)

	Function gtk_file_filter_new:Byte Ptr()
	Function gtk_file_filter_add_pattern(filter:Byte Ptr, pat:Byte Ptr)
	Function gtk_file_filter_set_name(filter:Byte Ptr, name:Byte Ptr)

	Function gtk_message_dialog_new:Byte Ptr(window:Byte Ptr, _flags:Int, _type:Int, _buttons:Int, text:Byte Ptr)

	Function gdk_screen_get_default:Byte Ptr()
	Function gdk_screen_get_width:Int(handle:Byte Ptr)
	Function gdk_screen_get_height:Int(handle:Byte Ptr)
	Function gdk_screen_get_root_window:Byte Ptr(screen:Byte Ptr)
	Function gdk_screen_get_display:Byte Ptr(screen:Byte Ptr)

	Function gdk_pixbuf_new_from_data:Int(data:Byte Ptr, colorspace:Int, ..
					has_alpha:Int, bits_per_sample:Int, width:Int, height:Int, ..
                    rowstride:Int, destroy_fn:Byte Ptr, data_fn:Byte Ptr)
	Function gdk_pixbuf_new:Byte Ptr(colorspace:Int, alpha:Int, bits:Int, width:Int, height:Int)
	Function gdk_pixbuf_new_subpixbuf:Byte Ptr(src:Byte Ptr, srcx:Int, srcy:Int, width:Int, height:Int)
	Function gdk_pixbuf_copy_area(src:Byte Ptr, srcx:Int, srcy:Int, width:Int, height:Int, dest:Byte Ptr, destx:Int, desty:Int)
	Function gtk_image_new_from_pixbuf:Byte Ptr(image:Int)
	Function gtk_image_new:Byte Ptr()
	Function gtk_image_set_from_pixbuf(widgetPtr:Byte Ptr, image:Int)
	Function gdk_pixbuf_rotate_simple:Byte Ptr(pixbuf:Byte Ptr, rotate:Int)
	Function gtk_image_clear(imagePtr:Byte Ptr)

	Function gtk_toolbar_new:Byte Ptr()
	Function gtk_tool_button_new:Byte Ptr(icon:Byte Ptr, label:Byte Ptr)
	Function gtk_toggle_tool_button_new:Byte Ptr()
	Function gtk_tool_button_set_label(toolitem:Byte Ptr, label:Byte Ptr)
	Function gtk_tool_button_set_icon_widget(toolitem:Byte Ptr, icon:Byte Ptr)
	Function gtk_toolbar_insert(toolbar:Byte Ptr, item:Byte Ptr, index:Int)
	Function gtk_toolbar_set_style(toolbar:Byte Ptr, style:Int)
	Function gtk_separator_tool_item_new:Byte Ptr()
	Function gtk_toggle_tool_button_get_active:Int(toolitem:Byte Ptr)
	Function gtk_toggle_tool_button_set_active(toolitem:Byte Ptr, bool:Int)
	Function gtk_toolbar_get_item_index:Int(toolbar:Byte Ptr, toolitem:Byte Ptr)

	Function gtk_tooltips_new:Byte Ptr()
	Function gtk_tooltips_enable(tooltips:Byte Ptr)
	Function gtk_tool_item_set_tooltip(toolitem:Byte Ptr, tooltips:Byte Ptr, tip:Byte Ptr, priv:Byte Ptr)
	Function gtk_tooltips_set_tip(tooltips:Byte Ptr, widgetPtr:Byte Ptr, tip:Byte Ptr, priv:Byte Ptr)

	Function gtk_tree_view_new:Byte Ptr()
	Function gtk_list_store_new:Byte Ptr(COLS:Int, type1:Int, type2:Int)
	Function gtk_tree_view_set_model(view:Byte Ptr, store:Byte Ptr)
	Function gtk_tree_store_new:Byte Ptr(COLS:Int, type1:Int, type2:Int)
	Function gtk_list_store_insert(store:Byte Ptr, iter:Byte Ptr, index:Int)
	Function gtk_list_store_set_value(store:Byte Ptr, iter:Byte Ptr, col:Int, value:Byte Ptr)
	Function gtk_tree_store_set_value(store:Byte Ptr, iter:Byte Ptr, col:Int, value:Byte Ptr)
	Function gtk_tree_view_column_new:Byte Ptr()
	Function gtk_tree_view_append_column:Int(store:Byte Ptr, col:Byte Ptr)
	Function gtk_cell_renderer_pixbuf_new:Byte Ptr()
	Function gtk_cell_renderer_text_new:Byte Ptr()
	Function gtk_tree_view_column_pack_start(col:Byte Ptr, renderer:Byte Ptr, Fill:Int)
	Function gtk_tree_view_column_pack_end(col:Byte Ptr, renderer:Byte Ptr, Fill:Int)
	Function gtk_tree_view_column_add_attribute(col:Byte Ptr, renderer:Byte Ptr, desc:Byte Ptr, ind:Int)
	Function gtk_tree_view_set_headers_visible(view:Byte Ptr, bool:Int)
	Function gtk_list_store_append(store:Byte Ptr, iter:Byte Ptr)
	Function gtk_list_store_iter_is_valid:Int(store:Byte Ptr, iter:Byte Ptr)
	Function gtk_list_store_remove(store:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_model_get_iter_from_string:Int(store:Byte Ptr, iter:Byte Ptr, st:Byte Ptr)
	Function gtk_tree_view_get_selection:Byte Ptr(view:Byte Ptr)
	Function gtk_tree_selection_select_iter(sel:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_selection_unselect_iter(sel:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_selection_set_mode(sel:Byte Ptr, Mode:Int)
	Function gtk_tree_selection_iter_is_selected:Int(sel:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_selection_get_selected:Int(sel:Byte Ptr, model:Byte Ptr Ptr, iter:Byte Ptr)
	Function gtk_tree_model_get_string_from_iter:Byte Ptr(sel:Byte Ptr, iter:Byte Ptr)
	Function gtk_list_store_clear(store:Byte Ptr)
	Function gtk_tree_store_append(store:Byte Ptr, iter:Byte Ptr, parent:Byte Ptr)
	Function gtk_tree_store_insert(store:Byte Ptr, iter:Byte Ptr, parent:Byte Ptr, index:Int)
	Function gtk_tree_model_iter_n_children:Int(_store:Byte Ptr, parent:Byte Ptr)
	Function gtk_tree_store_remove:Int(_store:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_path_to_string:Byte Ptr(tree:Byte Ptr)
	Function gtk_tree_path_new_from_string:Byte Ptr(path:Byte Ptr)
	Function gtk_tree_view_expand_row(view:Byte Ptr, tree:Byte Ptr, bool:Int)
	Function gtk_tree_view_collapse_row(view:Byte Ptr, tree:Byte Ptr)
	Function gtk_tree_path_free(tree:Byte Ptr)
	Function gtk_tree_model_get_path:Byte Ptr(model:Byte Ptr, iter:Byte Ptr)
	Function gtk_tree_selection_select_path(sel:Byte Ptr, path:Byte Ptr)
	Function gtk_tree_view_get_path_at_pos:Int(tree:Byte Ptr, x:Int, y:Int, path:Byte Ptr, column:Byte Ptr, cell_x:Int Ptr, cell_y:Int Ptr)

	Function g_value_init(gtype:Byte Ptr, _type:Int)
	Function g_value_set_string(_type:Byte Ptr, s:Byte Ptr)
	Function g_value_set_object(_type:Byte Ptr, s:Int)
	Function g_value_unset(_value:Byte Ptr)
	Function g_free(obj:Byte Ptr)

	Function gtk_accel_group_new:Byte Ptr()

	Function gdk_x11_drawable_get_xid:Int(handle:Byte Ptr)

	Function gdk_atom_intern:Byte Ptr(name:Byte Ptr, bool:Int)
	Function gtk_clipboard_get:Byte Ptr(atom:Byte Ptr)
	Function gtk_clipboard_set_text(handle:Byte Ptr, text:Byte Ptr, length:Int)
	Function gtk_clipboard_wait_for_text:Byte Ptr(handle:Byte Ptr)

	Function gtk_settings_set_string_property(settings:Byte Ptr, name:Byte Ptr, v_string:Byte Ptr, origin:Byte Ptr)
	Function gtk_settings_get_default:Byte Ptr()

	Function gdk_keyval_to_unicode:Int(keyval:Int)
	
	Function gtk_event_box_new:Byte Ptr()
	Function gtk_event_box_set_visible_window(handle:Byte Ptr, bool:Int)
	
	Function gdk_cursor_new_for_display:Byte Ptr(screen:Byte Ptr, _type:Int)
	Function gdk_window_set_cursor(window:Byte Ptr, cursor:Byte Ptr)
	Function gdk_window_get_pointer:Byte Ptr(window:Byte Ptr, x:Int Var, y:Int Var, mask:Int Var)

	Function gtk_vpaned_new:Byte Ptr()
	Function gtk_hpaned_new:Byte Ptr()
	Function gtk_paned_pack1(handle:Byte Ptr, child:Byte Ptr, resize:Int, shrink:Int)
	Function gtk_paned_pack2(handle:Byte Ptr, child:Byte Ptr, resize:Int, shrink:Int)
	Function gtk_paned_set_position(handle:Byte Ptr, pos:Int)
	Function gtk_paned_get_position:Int(handle:Byte Ptr)
	
	Function gtk_arrow_new:Byte Ptr(_type:Int, shadow:Int)
	
	Function gtk_object_destroy(obj:Byte Ptr)
	
	Function gtk_style_new:Byte Ptr()
	Function gtk_style_attach:Byte Ptr(style:Byte Ptr, window:Byte Ptr)
	
	Function g_utf8_validate:Int(txt:Byte Ptr, length:Int, _end:Byte Ptr Ptr)
	Function g_convert:Byte Ptr(s:Byte Ptr, length:Int, to_codeset:Byte Ptr, ..
                                             from_codeset:Byte Ptr, bytes_read:Int Ptr, ..
                                             bytes_written:Int Ptr, error:Byte Ptr Ptr)

	Function gdk_pixbuf_copy:Byte Ptr(pix:Byte Ptr)
	Function gdk_pixbuf_scale_simple:Byte Ptr(pix:Byte Ptr, width:Int, height:Int, interp:Int)
	Function gdk_draw_pixbuf(drawable:Byte Ptr, gc:Byte Ptr, pixbuf:Byte Ptr, src_x:Int, src_y:Int, dest_x:Int, dest_y:Int, width:Int, height:Int, dither:Int, x_dither:Int, y_dither:Int)
	
	Function gtk_layout_new:Byte Ptr(had:Byte Ptr, vad:Byte Ptr)
	Function gtk_layout_put(layout:Byte Ptr, child:Byte Ptr, x:Int, y:Int)
	Function gtk_layout_move(layout:Byte Ptr, child:Byte Ptr, x:Int, y:Int)
	
	Function gtk_alignment_new:Byte Ptr(xalign:Float, yalign:Float, xscale:Float, yscale:Float)
	
	Function gtk_drag_dest_set(widget:Byte Ptr, flags:Int, targets:Byte Ptr, ntargets:Int, actions:Int)
	Function gtk_drag_dest_add_uri_targets(widget:Byte Ptr)
	Function gtk_drag_dest_add_text_targets(widget:Byte Ptr)
	
	Function gtk_invisible_new:Byte Ptr()
	
	Function gdk_display_get_default:Byte Ptr()
	Function gdk_display_get_pointer(display:Byte Ptr, screen:Byte Ptr Ptr, x:Int Var, y:Int Var, mask:Byte Ptr)

	Function gtk_accel_map_lookup_entry:Int(path:Byte Ptr, key:Byte Ptr = Null)
	Function gtk_accel_map_add_entry(path:Byte Ptr, key:Int, modifier:Int)
	Function gtk_accel_map_change_entry:Int(path:Byte Ptr, key:Int, modifier:Int, rep:Int)
	Function gtk_accelerator_name:Byte Ptr(keycode:Int, modifier:Int)
	Function gtk_accelerator_parse(accelString:Byte Ptr, accelKey:Int Ptr, modKey:Int Ptr)
	'Function gtk_widget_add_accelerator(handle:Byte Ptr, action:Byte Ptr, accelGroup:Byte Ptr, accelKey:Int, modKey:Int, flags:Int)
	Function gtk_widget_remove_accelerator(handle:Byte Ptr, accelGroup:Byte Ptr, accelKey:Int, modKey:Int)

	' types
	Function gnome_canvas_group_get_type:Int()
	Function gnome_canvas_widget_get_type:Int()
	Function gnome_canvas_polygon_get_type:Int()
	Function gnome_canvas_pixbuf_get_type:Int()
	Function gdk_pixbuf_get_type:Int()
	
	' event types
	Function bmx_gtkmaxgui_gdkeventbutton(event:Byte Ptr, x:Double Ptr, y:Double Ptr, button:Int Ptr)
	Function bmx_gtkmaxgui_gdkeventmotion(event:Byte Ptr, x:Double Ptr, y:Double Ptr, state:Int Ptr)
	Function bmx_gtkmaxgui_gdkeventscroll(event:Byte Ptr, x:Double Ptr, y:Double Ptr, direction:Int Ptr)
	Function bmx_gtkmaxgui_gdkeventkey(event:Byte Ptr, keyval:Int Ptr, state:Int Ptr)
	Function bmx_gtkmaxgui_gdkeventconfigure(event:Byte Ptr, x:Int Ptr, y:Int Ptr, w:Int Ptr, h:Int Ptr)
	Function bmx_gtkmaxgui_gdkeventwindowstate(event:Byte Ptr, state:Int Ptr)
	
End Extern

Type TGTKRequisition
	Field width:Int
	Field height:Int
End Type

Type TGTKAllocation
  Field x:Int
  Field y:Int
  Field width:Int
  Field height:Int
End Type

Type TGDKColor
  Field pixel:Int
  Field red:Short
  Field green:Short
  Field blue:Short
End Type

Type TGTKTextIter
  ' GtkTextIter is an opaque datatype; ignore all these fields.
  ' Initialize the iter with gtk_text_buffer_get_iter_*
  ' functions
  '< Private >
  Field dummy1:Int
  Field dummy2:Int
  Field dummy3:Int
  Field dummy4:Int
  Field dummy5:Int
  Field dummy6:Int
  Field dummy7:Int
  Field dummy8:Int
  Field dummy9:Int
  Field dummy10:Int
  Field dummy11:Int
  Field dummy12:Int
  ' padding
  Field dummy13:Int
  Field dummy14:Int
End Type

Type TGValue
	Field _type:Int
	Field d1:Long
	Field d2:Long
End Type

' creates an Object out of an "int"
Type TGTKInteger
	Field value:Int
	Function Set:TGTKInteger(value:Int)
		Local this:TGTKInteger = New TGTKInteger
		this.value = value
		Return this
	End Function
	Method Compare:Int(o:Object)
		Return value-TGTKInteger(o).value
	End Method

End Type

Type TGtkTreeIter
	Field stamp:Int
	Field user_data:Byte Ptr
	Field user_data2:Byte Ptr
	Field user_data3:Byte Ptr
End Type

Type TGdkEventButton
	Field _type:Byte
	Field _window:Byte Ptr
	Field _send_event:Byte
	Field _time:Int
	Field _x:Double
	Field _y:Double
	Field _axes:Double Ptr
	Field _state:Int
	Field _button:Int
	Field _device:Byte Ptr
	Field _x_root:Double
	Field _y_root:Double
End Type

Type TGdkGeometry
  Field min_width:Int
  Field min_height:Int
  Field max_width:Int
  Field max_height:Int
  Field base_width:Int
  Field base_height:Int
  Field width_inc:Int
  Field height_inc:Int
  Field min_aspect:Double
  Field max_aspect:Double
  Field win_gravity:Int
End Type

Global gtkTrianglePixmap:TPixmap = LoadPixmap("incbin::gtk_triangle.png")

Rem
internal: Returns a Pango font description based on a TGuiFont specification. (INTERNAL)
about: Note! You need to call pango_font_description_free(fontdesc) when you are done with it... TODO : Don't worry about freeing it now it's inside the TGtkGuiFont
End Rem
Function getPangoDescriptionFromGuiFont(font:TGtkGuiFont)
	If font = Null Then
		Return
	End If
	
	If Not font.fontDesc Then

		Local fontdesc:Byte Ptr = pango_font_description_new()
		Local s:Byte Ptr = font.name.toUTF8String()
			
		pango_font_description_set_family(fontdesc, s)
	
		If font.style & FONT_BOLD Then
			pango_font_description_set_weight(fontdesc, PANGO_WEIGHT_BOLD)
		Else
			pango_font_description_set_weight(fontdesc, PANGO_WEIGHT_NORMAL)
		End If
	
		If font.style & FONT_ITALIC Then
			pango_font_description_set_style(fontdesc, PANGO_STYLE_ITALIC)
		End If
	
		pango_font_description_set_absolute_size(fontdesc, font.size * 1024)
	
		MemFree(s)
		
		font.fontDesc = fontDesc
		
	End If

'	Return fontdesc
End Function

Rem
internal: Returns a TGuiFont from a pango description object. (INTERNAL)
about: Note! Remember to destroy the pango object with pango_font_description_free(fontdesc) when you are
done with it...
End Rem
Function getGuiFontFromPangoDescription:TGuiFont(fontdesc:Byte Ptr)
	Local font:TGtkGuiFont = New TGTKGuiFont

	font.name = String.FromCString(pango_font_description_get_family(fontdesc))
	'font.path = ...
	Local ital:Int = pango_font_description_get_style(fontdesc)
	Local bold:Int = pango_font_description_get_weight(fontdesc)

	If ital = PANGO_STYLE_OBLIQUE Or ital = PANGO_STYLE_ITALIC Then
		font.style:| FONT_ITALIC
	End If

	If bold > PANGO_WEIGHT_NORMAL Then
		font.style:| FONT_BOLD
	End If

	font.size = pango_font_description_get_size(fontdesc) / 1024

	font.fontDesc = fontdesc
	
	Return font
End Function

Rem
internal:  Splits a string into a string array, as specified by the separator.
End Rem
Function _stringSplit:String[](text:String, separator:String)
	Local splitArray:String[]
	Local fieldCount:Int = 1
	
	' how many elements ?
	Local loc:Int = text.find(separator)
	While loc >= 0
		loc = text.find(separator, loc + 1)
		fieldCount:+1
	Wend
	
	' set the array with the calculated size
	splitArray = New String[fieldCount]
	
	fieldcount = 0
	While True
		loc = text.find(separator)
		If loc >= 0 Then
			splitArray[fieldCount] = text[..loc]
			text = text[loc+1..]
		Else
			splitArray[fieldCount] = text
			Exit
		End If
		fieldCount:+1
	Wend
	
	Return splitArray
End Function

Global GTK_USE_STOCK_ITEMS:Int = True

Function getGTKStockIDFromName:String(name:String)
	Global stockIDList:String[] 

	If GTK_USE_STOCK_ITEMS Then
		If Not stockIDList Then
			stockIDList = [GTK_STOCK_DIALOG_AUTHENTICATION, GTK_STOCK_DIALOG_INFO, ..
			GTK_STOCK_DIALOG_WARNING, GTK_STOCK_DIALOG_ERROR, GTK_STOCK_DIALOG_QUESTION, ..
			GTK_STOCK_DND, GTK_STOCK_DND_MULTIPLE, GTK_STOCK_ABOUT, GTK_STOCK_ADD, ..
			GTK_STOCK_APPLY, GTK_STOCK_BOLD, GTK_STOCK_CANCEL, GTK_STOCK_CDROM, GTK_STOCK_CLEAR, ..
			GTK_STOCK_CLOSE, GTK_STOCK_COLOR_PICKER, GTK_STOCK_CONVERT, GTK_STOCK_CONNECT, ..
			GTK_STOCK_COPY, GTK_STOCK_CUT, GTK_STOCK_DELETE, GTK_STOCK_DIRECTORY, GTK_STOCK_DISCONNECT, ..
			GTK_STOCK_EDIT, GTK_STOCK_EXECUTE, GTK_STOCK_FILE, GTK_STOCK_FIND, GTK_STOCK_FIND_AND_REPLACE, ..
			GTK_STOCK_FLOPPY, GTK_STOCK_FULLSCREEN, GTK_STOCK_GOTO_BOTTOM, GTK_STOCK_GOTO_FIRST, ..
			GTK_STOCK_GOTO_LAST, GTK_STOCK_GOTO_TOP, GTK_STOCK_GO_BACK, GTK_STOCK_GO_DOWN, ..
			GTK_STOCK_GO_FORWARD, GTK_STOCK_GO_UP, GTK_STOCK_HARDDISK, GTK_STOCK_HELP, GTK_STOCK_HOME, ..
			GTK_STOCK_INDEX, GTK_STOCK_INDENT, GTK_STOCK_INFO, GTK_STOCK_UNINDENT, GTK_STOCK_ITALIC, ..
			GTK_STOCK_JUMP_TO, GTK_STOCK_JUSTIFY_CENTER, GTK_STOCK_JUSTIFY_FILL, GTK_STOCK_JUSTIFY_LEFT, ..
			GTK_STOCK_JUSTIFY_RIGHT, GTK_STOCK_LEAVE_FULLSCREEN, GTK_STOCK_MISSING_IMAGE, ..
			GTK_STOCK_MEDIA_FORWARD, GTK_STOCK_MEDIA_NEXT, GTK_STOCK_MEDIA_PAUSE, GTK_STOCK_MEDIA_PLAY, ..
			GTK_STOCK_MEDIA_PREVIOUS, GTK_STOCK_MEDIA_RECORD, GTK_STOCK_MEDIA_REWIND, GTK_STOCK_MEDIA_STOP, ..
			GTK_STOCK_NETWORK, GTK_STOCK_NEW, GTK_STOCK_NO, GTK_STOCK_OK, GTK_STOCK_OPEN, GTK_STOCK_PASTE, ..
			GTK_STOCK_PREFERENCES, GTK_STOCK_PRINT, GTK_STOCK_PRINT_PREVIEW, GTK_STOCK_PROPERTIES, ..
			GTK_STOCK_QUIT, GTK_STOCK_REDO, GTK_STOCK_REFRESH, GTK_STOCK_REMOVE, GTK_STOCK_REVERT_TO_SAVED, ..
			GTK_STOCK_SAVE, GTK_STOCK_SAVE_AS, GTK_STOCK_SELECT_COLOR, GTK_STOCK_SELECT_FONT, ..
			GTK_STOCK_SORT_ASCENDING, GTK_STOCK_SORT_DESCENDING, GTK_STOCK_SPELL_CHECK, GTK_STOCK_STOP, ..
			GTK_STOCK_STRIKETHROUGH, GTK_STOCK_UNDELETE, GTK_STOCK_UNDERLINE, GTK_STOCK_UNDO, ..
			GTK_STOCK_YES, GTK_STOCK_ZOOM_100, GTK_STOCK_ZOOM_FIT, GTK_STOCK_ZOOM_IN, GTK_STOCK_ZOOM_OUT ]
		End If
	
		Local txt:String = "gtk-" + name.Trim().toLower().Replace(" ", "-")
	
		For Local i:Int = 0 Until stockIDList.length
			If txt = stockIDList[i] Then
				Return txt
			End If
		Next
	End If

	Return name
End Function


Function gtkUTF8toISO8859:String(s:Byte Ptr)
	Local l:Int = _strlen(s)
	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l
		bc:+1
		c = s[i]
		If c<128 
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

' List of application windows
' We use it for SetPointer etc.
Global gtkWindows:TList = New TList

' I know... cup of cocoa anyone?
Global GadgetMap:TMap=New TMap


Global gtkKeysDown:Int[] = New Int[255]

' returns True if key is already believed to be DOWN/Pressed
Function gtkSetKeyDown:Int(key:Int)
	Assert key < 255, "gtkSetKeyDown key is out of range - " + key
	
	If gtkKeysDown[key] Then
		Return True
	End If
	
	gtkKeysDown[key] = 1
	Return False
End Function

Function gtkSetKeyUp(key:Int)
	Assert key < 255, "gtkSetKeyUp key is out of range - " + key
	
	gtkKeysDown[key] = 0
End Function

' gadget identifiers
Const GTK_WINDOW:Int = 0
Const GTK_BUTTON:Int = 1
Const GTK_RADIOBUTTON:Int = 2
Const GTK_CHECKBUTTON:Int = 3
Const GTK_TOGGLEBUTTON:Int = 4
Const GTK_LABEL:Int = 5
Const GTK_MENUITEM:Int = 6
Const GTK_TEXTFIELD:Int = 7
Const GTK_TEXTAREA:Int = 8
Const GTK_PANEL:Int = 9
Const GTK_COMBOBOX:Int = 10
Const GTK_HTMLVIEW:Int = 11
Const GTK_TABBER:Int = 12
Const GTK_PROGRESSBAR:Int = 13
Const GTK_SCROLLBAR:Int = 14
Const GTK_TRACKBAR:Int = 15
Const GTK_STEPPER:Int = 16
Const GTK_DESKTOP:Int = 17
Const GTK_TOOLBAR:Int = 18
Const GTK_LISTBOX:Int = 19
Const GTK_TREEVIEW:Int = 20
Const GTK_CANVAS:Int = 21

' Events
Const GDK_NOTHING:Int		= -1
Const GDK_DELETE:Int		= 0
Const GDK_DESTROY:Int		= 1
Const GDK_EXPOSE:Int		= 2
Const GDK_MOTION_NOTIFY:Int	= 3
Const GDK_BUTTON_PRESS:Int	= 4
Const GDK_2BUTTON_PRESS:Int	= 5
Const GDK_DOUBLE_BUTTON_PRESS:Int = GDK_2BUTTON_PRESS
Const GDK_3BUTTON_PRESS:Int	= 6
Const GDK_TRIPLE_BUTTON_PRESS:Int = GDK_3BUTTON_PRESS
Const GDK_BUTTON_RELEASE:Int	= 7
Const GDK_KEY_PRESS:Int		= 8
Const GDK_KEY_RELEASE:Int	= 9
Const GDK_ENTER_NOTIFY:Int	= 10
Const GDK_LEAVE_NOTIFY:Int	= 11
Const GDK_FOCUS_CHANGE:Int	= 12
Const GDK_CONFIGURE:Int		= 13
Const GDK_MAP:Int		= 14
Const GDK_UNMAP:Int		= 15
Const GDK_PROPERTY_NOTIFY:Int	= 16
Const GDK_SELECTION_CLEAR:Int	= 17
Const GDK_SELECTION_REQUEST:Int = 18
Const GDK_SELECTION_NOTIFY:Int	= 19
Const GDK_PROXIMITY_IN:Int	= 20
Const GDK_PROXIMITY_OUT:Int	= 21
Const GDK_DRAG_ENTER:Int        = 22
Const GDK_DRAG_LEAVE:Int        = 23
Const GDK_DRAG_MOTION:Int       = 24
Const GDK_DRAG_STATUS:Int       = 25
Const GDK_DROP_START:Int        = 26
Const GDK_DROP_FINISHED:Int     = 27
Const GDK_CLIENT_EVENT:Int	= 28
Const GDK_VISIBILITY_NOTIFY:Int = 29
Const GDK_NO_EXPOSE:Int		= 30
Const GDK_SCROLL:Int            = 31
Const GDK_WINDOW_STATE:Int      = 32
Const GDK_SETTING:Int           = 33
Const GDK_OWNER_CHANGE:Int      = 34
Const GDK_GRAB_BROKEN:Int       = 35
Const GDK_DAMAGE:Int            = 36
Const GDK_TOUCH_BEGIN:Int       = 37
Const GDK_TOUCH_UPDATE:Int      = 38
Const GDK_TOUCH_END:Int         = 39
Const GDK_TOUCH_CANCEL:Int      = 40

' gtkwindowtype
Const GTK_WINDOW_TOPLEVEL:Int = 0
Const GTK_WINDOW_POPUP:Int = 1

' gtkwindowposition
Const GTK_WIN_POS_NONE:Int = 0
Const GTK_WIN_POS_CENTER:Int = 1
Const GTK_WIN_POS_MOUSE:Int = 2
Const GTK_WIN_POS_CENTER_ALWAYS:Int = 3
Const GTK_WIN_POS_CENTER_ON_PARENT:Int = 4

' gtktoolbarstyle
Const GTK_TOOLBAR_ICONS:Int = 0
Const GTK_TOOLBAR_TEXT:Int = 1
Const GTK_TOOLBAR_BOTH:Int = 2
Const GTK_TOOLBAR_BOTH_HORIZ:Int = 3

' GtkStateType
Const GTK_STATE_NORMAL:Int = 0
Const GTK_STATE_ACTIVE:Int = 1
Const GTK_STATE_PRELIGHT:Int = 2
Const GTK_STATE_SELECTED:Int = 3
Const GTK_STATE_INSENSITIVE:Int = 4
Const GTK_STATE_INCONSISTENT:Int = 5
Const GTK_STATE_FOCUSED:Int = 6

' GtkShadowType
Const GTK_SHADOW_NONE:Int = 0
Const GTK_SHADOW_IN:Int = 1
Const GTK_SHADOW_OUT:Int = 2
Const GTK_SHADOW_ETCHED_IN:Int = 3
Const GTK_SHADOW_ETCHED_OUT:Int = 4

' GtkJustification
Const GTK_JUSTIFY_LEFT:Int = 0
Const GTK_JUSTIFY_RIGHT:Int = 1
Const GTK_JUSTIFY_CENTER:Int = 2
Const GTK_JUSTIFY_FILL:Int = 3

' GtkPolicyType
Const GTK_POLICY_ALWAYS:Int = 0
Const GTK_POLICY_AUTOMATIC:Int = 1
Const GTK_POLICY_NEVER:Int = 2

' GtkResizeMode
Const GTK_RESIZE_PARENT:Int = 0
Const GTK_RESIZE_QUEUE:Int = 1
Const GTK_RESIZE_IMMEDIATE:Int = 2

' GtkFileChooserAction
Const GTK_FILE_CHOOSER_ACTION_OPEN:Int = 0
Const GTK_FILE_CHOOSER_ACTION_SAVE:Int = 1
Const GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER:Int = 2
Const GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER:Int = 3

Const GDK_EXPOSURE_MASK:Int = 1 Shl 1
Const GDK_POINTER_MOTION_MASK:Int = 1 Shl 2
Const GDK_POINTER_MOTION_HINT_MASK:Int = 1 Shl 3
Const GDK_BUTTON_MOTION_MASK:Int = 1 Shl 4
Const GDK_BUTTON1_MOTION_MASK:Int = 1 Shl 5
Const GDK_BUTTON2_MOTION_MASK:Int = 1 Shl 6
Const GDK_BUTTON3_MOTION_MASK:Int = 1 Shl 7
Const GDK_BUTTON_PRESS_MASK:Int = 1 Shl 8
Const GDK_BUTTON_RELEASE_MASK:Int = 1 Shl 9
Const GDK_KEY_PRESS_MASK:Int = 1 Shl 10
Const GDK_KEY_RELEASE_MASK:Int = 1 Shl 11
Const GDK_ENTER_NOTIFY_MASK:Int = 1 Shl 12
Const GDK_LEAVE_NOTIFY_MASK:Int = 1 Shl 13
Const GDK_FOCUS_CHANGE_MASK:Int = 1 Shl 14
Const GDK_STRUCTURE_MASK:Int = 1 Shl 15
Const GDK_PROPERTY_CHANGE_MASK:Int = 1 Shl 16
Const GDK_VISIBILITY_NOTIFY_MASK:Int = 1 Shl 17
Const GDK_PROXIMITY_IN_MASK:Int = 1 Shl 18
Const GDK_PROXIMITY_OUT_MASK:Int = 1 Shl 19
Const GDK_SUBSTRUCTURE_MASK:Int = 1 Shl 20
Const GDK_SCROLL_MASK:Int = 1 Shl 21
Const GDK_TOUCH_MASK:Int = 1 Shl 22
Const GDK_SMOOTH_SCROLL_MASK:Int = 1 Shl 23
'Const GDK_ALL_EVENTS_MASK:Int = 0x3FFFFE

' GtkWrapMode
Const GTK_WRAP_NONE:Int = 0
Const GTK_WRAP_CHAR:Int = 1
Const GTK_WRAP_WORD:Int = 2
Const GTK_WRAP_WORD_CHAR:Int = 3

' PangoStyle
Const PANGO_STYLE_NORMAL:Int = 0
Const PANGO_STYLE_OBLIQUE:Int = 1
Const PANGO_STYLE_ITALIC:Int = 2

' PangoWeight
Const PANGO_WEIGHT_ULTRALIGHT:Int = 200
Const PANGO_WEIGHT_LIGHT:Int = 300
Const PANGO_WEIGHT_NORMAL:Int = 400
Const PANGO_WEIGHT_SEMIBOLD:Int = 600
Const PANGO_WEIGHT_BOLD:Int = 700
Const PANGO_WEIGHT_ULTRABOLD:Int = 800
Const PANGO_WEIGHT_HEAVY:Int = 900

' PangoUnderline
Const PANGO_UNDERLINE_NONE:Int = 0
Const PANGO_UNDERLINE_SINGLE:Int = 1
Const PANGO_UNDERLINE_DOUBLE:Int = 2
Const PANGO_UNDERLINE_LOW:Int = 3
Const PANGO_UNDERLINE_ERROR:Int = 4

' GtkResponseType
Const GTK_RESPONSE_NONE:Int = -1
Const GTK_RESPONSE_REJECT:Int = -2
Const GTK_RESPONSE_ACCEPT:Int = -3
Const GTK_RESPONSE_DELETE_EVENT:Int = -4
Const GTK_RESPONSE_OK:Int     = -5
Const GTK_RESPONSE_CANCEL:Int = -6
Const GTK_RESPONSE_CLOSE:Int  = -7
Const GTK_RESPONSE_YES:Int    = -8
Const GTK_RESPONSE_NO:Int     = -9
Const GTK_RESPONSE_APPLY:Int  = -10
Const GTK_RESPONSE_HELP:Int   = -11


' GtkStockItem
Const GTK_STOCK_ABOUT:String =            "gtk-about"
Const GTK_STOCK_ADD:String =              "gtk-add"
Const GTK_STOCK_APPLY:String =            "gtk-apply"
Const GTK_STOCK_BOLD:String =             "gtk-bold"
Const GTK_STOCK_CANCEL:String =           "gtk-cancel"
Const GTK_STOCK_CAPS_LOCK_WARNING:String = "gtk-caps-lock-warning"
Const GTK_STOCK_CDROM:String =            "gtk-cdrom"
Const GTK_STOCK_CLEAR:String =            "gtk-clear"
Const GTK_STOCK_CLOSE:String =            "gtk-close"
Const GTK_STOCK_COLOR_PICKER:String =     "gtk-color-picker"
Const GTK_STOCK_CONVERT:String =          "gtk-convert"
Const GTK_STOCK_CONNECT:String =          "gtk-connect"
Const GTK_STOCK_COPY:String =             "gtk-copy"
Const GTK_STOCK_CUT:String =              "gtk-cut"
Const GTK_STOCK_DELETE:String =           "gtk-delete"
Const GTK_STOCK_DIALOG_AUTHENTICATION:String = "gtk-dialog-authentication"
Const GTK_STOCK_DIALOG_ERROR:String =     "gtk-dialog-error"
Const GTK_STOCK_DIALOG_INFO:String =      "gtk-dialog-info"
Const GTK_STOCK_DIALOG_QUESTION:String =  "gtk-dialog-question"
Const GTK_STOCK_DIALOG_WARNING:String =   "gtk-dialog-warning"
Const GTK_STOCK_DIRECTORY:String =        "gtk-directory"
Const GTK_STOCK_DISCARD:String =          "gtk-discard"
Const GTK_STOCK_DISCONNECT:String =       "gtk-disconnect"
Const GTK_STOCK_DND:String =              "gtk-dnd"
Const GTK_STOCK_DND_MULTIPLE:String =     "gtk-dnd-multiple"
Const GTK_STOCK_EDIT:String =             "gtk-edit"
Const GTK_STOCK_EXECUTE:String =          "gtk-execute"
Const GTK_STOCK_FILE:String =             "gtk-file"
Const GTK_STOCK_FIND:String =             "gtk-find"
Const GTK_STOCK_FIND_AND_REPLACE:String = "gtk-find-and-replace"
Const GTK_STOCK_FLOPPY:String =           "gtk-floppy"
Const GTK_STOCK_FULLSCREEN:String =       "gtk-fullscreen"
Const GTK_STOCK_GOTO_BOTTOM:String =      "gtk-goto-bottom"
Const GTK_STOCK_GOTO_FIRST:String =       "gtk-goto-first"
Const GTK_STOCK_GOTO_LAST:String =        "gtk-goto-last"
Const GTK_STOCK_GOTO_TOP:String =         "gtk-goto-top"
Const GTK_STOCK_GO_BACK:String =          "gtk-go-back"
Const GTK_STOCK_GO_DOWN:String =          "gtk-go-down"
Const GTK_STOCK_GO_FORWARD:String =       "gtk-go-forward"
Const GTK_STOCK_GO_UP:String =            "gtk-go-up"
Const GTK_STOCK_HARDDISK:String =         "gtk-harddisk"
Const GTK_STOCK_HELP:String =             "gtk-help"
Const GTK_STOCK_HOME:String =             "gtk-home"
Const GTK_STOCK_INDENT:String =           "gtk-indent"
Const GTK_STOCK_INDEX:String =            "gtk-index"
Const GTK_STOCK_INFO:String =             "gtk-info"
Const GTK_STOCK_ITALIC:String =           "gtk-italic"
Const GTK_STOCK_JUMP_TO:String =          "gtk-jump-to"
Const GTK_STOCK_JUSTIFY_CENTER:String =   "gtk-justify-center"
Const GTK_STOCK_JUSTIFY_FILL:String =     "gtk-justify-fill"
Const GTK_STOCK_JUSTIFY_LEFT:String =     "gtk-justify-left"
Const GTK_STOCK_JUSTIFY_RIGHT:String =    "gtk-justify-right"
Const GTK_STOCK_LEAVE_FULLSCREEN:String = "gtk-leave-fullscreen"
Const GTK_STOCK_MEDIA_FORWARD:String =    "gtk-media-forward"
Const GTK_STOCK_MEDIA_NEXT:String =       "gtk-media-next"
Const GTK_STOCK_MEDIA_PAUSE:String =      "gtk-media-pause"
Const GTK_STOCK_MEDIA_PLAY:String =       "gtk-media-play"
Const GTK_STOCK_MEDIA_PREVIOUS:String =   "gtk-media-previous"
Const GTK_STOCK_MEDIA_RECORD:String =     "gtk-media-record"
Const GTK_STOCK_MEDIA_REWIND:String =     "gtk-media-rewind"
Const GTK_STOCK_MEDIA_STOP:String =       "gtk-media-stop"
Const GTK_STOCK_MISSING_IMAGE:String =    "gtk-missing-image"
Const GTK_STOCK_NETWORK:String =          "gtk-network"
Const GTK_STOCK_NEW:String =              "gtk-new"
Const GTK_STOCK_NO:String =               "gtk-no"
Const GTK_STOCK_OK:String =               "gtk-ok"
Const GTK_STOCK_OPEN:String =             "gtk-open"
Const GTK_STOCK_PAGE_SETUP:String =       "gtk-page-setup"
Const GTK_STOCK_PASTE:String =            "gtk-paste"
Const GTK_STOCK_PREFERENCES:String =      "gtk-preferences"
Const GTK_STOCK_PRINT:String =            "gtk-print"
Const GTK_STOCK_PRINT_ERROR:String =      "gtk-print-error"
Const GTK_STOCK_PRINT_PAUSED:String =     "gtk-print-paused"
Const GTK_STOCK_PRINT_PREVIEW:String =    "gtk-print-preview"
Const GTK_STOCK_PRINT_REPORT:String =     "gtk-print-report"
Const GTK_STOCK_PRINT_WARNING:String =    "gtk-print-warning"
Const GTK_STOCK_PROPERTIES:String =       "gtk-properties"
Const GTK_STOCK_QUIT:String =             "gtk-quit"
Const GTK_STOCK_REDO:String =             "gtk-redo"
Const GTK_STOCK_REFRESH:String =          "gtk-refresh"
Const GTK_STOCK_REMOVE:String =           "gtk-remove"
Const GTK_STOCK_REVERT_TO_SAVED:String =  "gtk-revert-to-saved"
Const GTK_STOCK_SAVE:String =             "gtk-save"
Const GTK_STOCK_SAVE_AS:String =          "gtk-save-as"
Const GTK_STOCK_SELECT_ALL:String =       "gtk-select-all"
Const GTK_STOCK_SELECT_COLOR:String =     "gtk-select-color"
Const GTK_STOCK_SELECT_FONT:String =      "gtk-select-font"
Const GTK_STOCK_SORT_ASCENDING:String =   "gtk-sort-ascending"
Const GTK_STOCK_SORT_DESCENDING:String =  "gtk-sort-descending"
Const GTK_STOCK_SPELL_CHECK:String =      "gtk-spell-check"
Const GTK_STOCK_STOP:String =             "gtk-stop"
Const GTK_STOCK_STRIKETHROUGH:String =    "gtk-strikethrough"
Const GTK_STOCK_UNDELETE:String =         "gtk-undelete"
Const GTK_STOCK_UNDERLINE:String =        "gtk-underline"
Const GTK_STOCK_UNDO:String =             "gtk-undo"
Const GTK_STOCK_UNINDENT:String =         "gtk-unindent"
Const GTK_STOCK_YES:String =              "gtk-yes"
Const GTK_STOCK_ZOOM_100:String =         "gtk-zoom-100"
Const GTK_STOCK_ZOOM_FIT:String =         "gtk-zoom-fit"
Const GTK_STOCK_ZOOM_IN:String =          "gtk-zoom-in"
Const GTK_STOCK_ZOOM_OUT:String =         "gtk-zoom-out"

' GtkFileFilterFlags
Const GTK_FILE_FILTER_FILENAME:Int = 1 Shl 0
Const GTK_FILE_FILTER_URI:Int = 1 Shl 1
Const GTK_FILE_FILTER_DISPLAY_NAME:Int = 1 Shl 2
Const GTK_FILE_FILTER_MIME_TYPE:Int = 1 Shl 3

' GtkMessageType
Const GTK_MESSAGE_INFO:Int = 0
Const GTK_MESSAGE_WARNING:Int = 1
Const GTK_MESSAGE_QUESTION:Int = 2
Const GTK_MESSAGE_ERROR:Int = 3
Const GTK_MESSAGE_OTHER:Int = 4

' GtkButtonsType
Const GTK_BUTTONS_NONE:Int = 0
Const GTK_BUTTONS_OK:Int = 1
Const GTK_BUTTONS_CLOSE:Int = 2
Const GTK_BUTTONS_CANCEL:Int = 3
Const GTK_BUTTONS_YES_NO:Int = 4
Const GTK_BUTTONS_OK_CANCEL:Int = 5

' GtkDialogFlags
Const GTK_DIALOG_MODAL:Int = 0
Const GTK_DIALOG_DESTROY_WITH_PARENT:Int = 1

' GdkColorspace
Const GDK_COLORSPACE_RGB:Int = 0

' PangoTabAlign
Const PANGO_TAB_LEFT:Int = 0
Const PANGO_TAB_RIGHT:Int = 1
Const PANGO_TAB_CENTER:Int = 2
Const PANGO_TAB_NUMERIC:Int = 3

' GType
Const G_TYPE_STRING:Int = 16 Shl 2
Const G_TYPE_POINTER:Int = 17 Shl 2
'Const GDK_TYPE_PIXBUF:Int = 150013448

' GtkSelectionMode
Const GTK_SELECTION_NONE:Int = 0                       ' Nothing can be selected
Const GTK_SELECTION_SINGLE:Int = 1
Const GTK_SELECTION_BROWSE:Int = 2
Const GTK_SELECTION_MULTIPLE:Int = 3

' GtkAccelFlags
Const GTK_ACCEL_VISIBLE:Int = 1 ' display in GtkAccelLabel?
Const GTK_ACCEL_LOCKED:Int  = 2 ' is it removable?
Const GTK_ACCEL_MASK:Int    = 7

' GdkCursorType
Const GDK_X_CURSOR:Int		  = 0
Const GDK_ARROW:Int			  = 2
Const GDK_BASED_ARROW_DOWN:Int	  = 4
Const GDK_BASED_ARROW_UP:Int	  = 6
Const GDK_BOAT:Int			  = 8
Const GDK_BOGOSITY:Int		  = 10
Const GDK_BOTTOM_LEFT_CORNER:Int = 12
Const GDK_BOTTOM_RIGHT_CORNER:Int= 14
Const GDK_BOTTOM_SIDE:Int		  = 16
Const GDK_BOTTOM_TEE:Int		  = 18
Const GDK_BOX_SPIRAL:Int		  = 20
Const GDK_CENTER_PTR:Int		  = 22
Const GDK_CIRCLE:Int			  = 24
Const GDK_CLOCK:Int			  = 26
Const GDK_COFFEE_MUG:Int		  = 28
Const GDK_CROSS:Int			  = 30
Const GDK_CROSS_REVERSE:Int	  = 32
Const GDK_CROSSHAIR:Int		  = 34
Const GDK_DIAMOND_CROSS:Int	  = 36
Const GDK_DOT:Int			  = 38
Const GDK_DOTBOX:Int 		  = 40
Const GDK_DOUBLE_ARROW:Int 	  = 42
Const GDK_DRAFT_LARGE:Int 	  = 44
Const GDK_DRAFT_SMALL:Int 	  = 46
Const GDK_DRAPED_BOX:Int 	  = 48
Const GDK_EXCHANGE:Int 		  = 50
Const GDK_FLEUR:Int 		  = 52
Const GDK_GOBBLER:Int 		  = 54
Const GDK_GUMBY:Int 			= 56
Const GDK_HAND1:Int 			= 58
Const GDK_HAND2:Int 			= 60
Const GDK_HEART:Int 			= 62
Const GDK_ICON:Int 			= 64
Const GDK_IRON_CROSS:Int 		= 66
Const GDK_LEFT_PTR:Int 		= 68
Const GDK_LEFT_SIDE:Int 		= 70
Const GDK_LEFT_TEE:Int 		= 72
Const GDK_LEFTBUTTON:Int 		= 74
Const GDK_LL_ANGLE:Int 		= 76
Const GDK_LR_ANGLE:Int 		= 78
Const GDK_MAN:Int			= 80
Const GDK_MIDDLEBUTTON:Int 	= 82
Const GDK_MOUSE:Int 			= 84
Const GDK_PENCIL:Int 		= 86
Const GDK_PIRATE:Int 		= 88
Const GDK_PLUS_CURSOR:Int		= 90
Const GDK_QUESTION_ARROW:Int 	= 92
Const GDK_RIGHT_PTR:Int 		= 94
Const GDK_RIGHT_SIDE:Int 	 	= 96
Const GDK_RIGHT_TEE:Int 	 	= 98
Const GDK_RIGHTBUTTON:Int 	  = 100
Const GDK_RTL_LOGO:Int 		  = 102
Const GDK_SAILBOAT:Int 		  = 104
Const GDK_SB_DOWN_ARROW:Int 	  = 106
Const GDK_SB_H_DOUBLE_ARROW:Int   = 108
Const GDK_SB_LEFT_ARROW:Int 	  = 110
Const GDK_SB_RIGHT_ARROW:Int 	  = 112
Const GDK_SB_UP_ARROW:Int 	  = 114
Const GDK_SB_V_DOUBLE_ARROW:Int   = 116
Const GDK_SHUTTLE:Int 		  = 118
Const GDK_SIZING:Int 		  = 120
Const GDK_SPIDER:Int		  = 122
Const GDK_SPRAYCAN:Int 		  = 124
Const GDK_STAR:Int 		  = 126
Const GDK_TARGET:Int 		  = 128
Const GDK_TCROSS:Int 		  = 130
Const GDK_TOP_LEFT_ARROW:Int 	  = 132
Const GDK_TOP_LEFT_CORNER:Int 	  = 134
Const GDK_TOP_RIGHT_CORNER:Int 	  = 136
Const GDK_TOP_SIDE:Int 		  = 138
Const GDK_TOP_TEE:Int 		  = 140
Const GDK_TREK:Int 		  = 142
Const GDK_UL_ANGLE:Int 		  = 144
Const GDK_UMBRELLA:Int 		  = 146
Const GDK_UR_ANGLE:Int 		  = 148
Const GDK_WATCH:Int 		  = 150
Const GDK_XTERM:Int 		  = 152

' GdkWindowHints
Const GDK_HINT_POS:Int	     = 1
Const GDK_HINT_MIN_SIZE:Int    = 2
Const GDK_HINT_MAX_SIZE:Int    = 4
Const GDK_HINT_BASE_SIZE:Int   = 8
Const GDK_HINT_ASPECT:Int      = 16
Const GDK_HINT_RESIZE_INC:Int  = 32
Const GDK_HINT_WIN_GRAVITY:Int = 64
Const GDK_HINT_USER_POS:Int    = 128
Const GDK_HINT_USER_SIZE:Int   = 256

' GdkWindowState
Const GDK_WINDOW_STATE_WITHDRAWN:Int  = 1 Shl 0
Const GDK_WINDOW_STATE_ICONIFIED:Int  = 1 Shl 1
Const GDK_WINDOW_STATE_MAXIMIZED:Int  = 1 Shl 2
Const GDK_WINDOW_STATE_STICKY:Int     = 1 Shl 3
Const GDK_WINDOW_STATE_FULLSCREEN:Int = 1 Shl 4
Const GDK_WINDOW_STATE_ABOVE:Int      = 1 Shl 5
Const GDK_WINDOW_STATE_BELOW:Int      = 1 Shl 6
Const GDK_WINDOW_STATE_FOCUSED:Int    = 1 Shl 7
Const GDK_WINDOW_STATE_TILED:Int      = 1 Shl 8

' GdkWindowTypeHint
Const GDK_WINDOW_TYPE_HINT_NORMAL:Int       = 0
Const GDK_WINDOW_TYPE_HINT_DIALOG:Int       = 1
Const GDK_WINDOW_TYPE_HINT_MENU:Int         = 2
Const GDK_WINDOW_TYPE_HINT_TOOLBAR:Int      = 3
Const GDK_WINDOW_TYPE_HINT_SPLASHSCREEN:Int = 4
Const GDK_WINDOW_TYPE_HINT_UTILITY:Int      = 5
Const GDK_WINDOW_TYPE_HINT_DOCK:Int         = 6
Const GDK_WINDOW_TYPE_HINT_DESKTOP:Int      = 7
Const GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU:Int= 8  ' A drop down menu (from a menubar)
Const GDK_WINDOW_TYPE_HINT_POPUP_MENU:Int   = 9  ' A popup menu (from Right-click)
Const GDK_WINDOW_TYPE_HINT_TOOLTIP:Int      = 10
Const GDK_WINDOW_TYPE_HINT_NOTIFICATION:Int = 11
Const GDK_WINDOW_TYPE_HINT_COMBO:Int        = 12
Const GDK_WINDOW_TYPE_HINT_DND:Int          = 13

' GtkArrowType
Const GTK_ARROW_UP:Int = 0
Const GTK_ARROW_DOWN:Int = 1
Const GTK_ARROW_LEFT:Int = 2
Const GTK_ARROW_RIGHT:Int = 3
Const GTK_ARROW_NONE:Int = 4

' GdkVisibilityState
Const GDK_VISIBILITY_UNOBSCURED:Int = 0
Const GDK_VISIBILITY_PARTIAL:Int = 1
Const GDK_VISIBILITY_FULLY_OBSCURED:Int = 2

' GdkInterpType
Const GDK_INTERP_NEAREST:Int = 0
Const GDK_INTERP_TILES:Int = 1
Const GDK_INTERP_BILINEAR:Int = 2
Const GDK_INTERP_HYPER:Int = 3

' GdkPixbufRotation
Const GDK_PIXBUF_ROTATE_NONE:Int             =   0
Const GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE:Int =  90
Const GDK_PIXBUF_ROTATE_UPSIDEDOWN:Int       = 180
Const GDK_PIXBUF_ROTATE_CLOCKWISE:Int        = 270

' GtkDestDefaults
Const GTK_DEST_DEFAULT_MOTION:Int     = 1 Shl 0 ' respond To "drag_motion"
Const GTK_DEST_DEFAULT_HIGHLIGHT:Int  = 1 Shl 1 ' auto-highlight
Const GTK_DEST_DEFAULT_DROP:Int       = 1 Shl 2 ' respond To "drag_drop"
Const GTK_DEST_DEFAULT_ALL:Int        = 7

' GdkDragAction
Const GDK_ACTION_DEFAULT:Int = 1 Shl 0
Const GDK_ACTION_COPY:Int    = 1 Shl 1
Const GDK_ACTION_MOVE:Int    = 1 Shl 2
Const GDK_ACTION_LINK:Int    = 1 Shl 3
Const GDK_ACTION_PRIVATE:Int = 1 Shl 4
Const GDK_ACTION_ASK:Int     = 1 Shl 5

' GdkScrollDirection
Const GDK_SCROLL_UP:Int = 0
Const GDK_SCROLL_DOWN:Int = 1
Const GDK_SCROLL_LEFT:Int = 2
Const GDK_SCROLL_RIGHT:Int = 3
Const GDK_SCROLL_SMOOTH:Int = 4

Const GTK_POS_LEFT:Int = 0
Const GTK_POS_RIGHT:Int = 1
Const GTK_POS_TOP:Int = 2
Const GTK_POS_BOTTOM:Int = 3


Type TGTKGuiFont Extends TGuiFont

	Field fontDesc:Byte Ptr
	
	Method Delete()
		If fontDesc Then
			pango_font_description_free(fontDesc)
			fontDesc = Null
		EndIf
	EndMethod
	
	Method CharWidth:Int(char:Int)
		If handle
			'Return NSCharWidth(handle,char)
		EndIf
		Return 0
	EndMethod 
		
EndType
