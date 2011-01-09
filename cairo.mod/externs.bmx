SuperStrict

Import Pub.FreeType
Import BRL.Stream
Import BRL.FileSystem
Import Pub.Glew
?Not linux
Import BRL.Map
?

' Cairo external functions

Extern
	Function cairo_append_path(context:Byte Ptr, path:Byte Ptr)
	Function cairo_arc(context:Byte Ptr, xcenter:Double, ycenter:Double, ..
							radius:Double, startangle:Double, endangle:Double)
	Function cairo_arc_negative(context:Byte Ptr, xcenter:Double, ycenter:Double, ..
							radius:Double, startangle:Double, endangle:Double)
	Function cairo_clip(context:Byte Ptr)
	Function cairo_clip_preserve(context:Byte Ptr)
	Function cairo_close_path(context:Byte Ptr)
	Function cairo_copy_page(context:Byte Ptr)
	Function cairo_copy_path:Byte Ptr(context:Byte Ptr)
	Function cairo_copy_path_flat:Byte Ptr(context:Byte Ptr)
	Function cairo_create:Byte Ptr(target:Byte Ptr)
	Function cairo_curve_to(context:Byte Ptr, x1:Double, y1:Double, ..
								x2:Double, y2:Double, ..
								x3:Double, y3:Double)
	Function cairo_debug_reset_static_data()
	Function cairo_destroy(context:Byte Ptr)
	Function cairo_device_to_user(context:Byte Ptr, x:Double Var, y:Double Var)
	Function cairo_device_to_user_distance(context:Byte Ptr, dx:Double Var, dy:Double Var)
	Function cairo_fill(context:Byte Ptr)
	Function cairo_fill_extents(context:Byte Ptr, x1:Double Var,y1:Double Var, ..
									x2:Double Var, y2:Double Var)
	Function cairo_fill_preserve(context:Byte Ptr)
	Function cairo_font_extents(context:Byte Ptr, font_extents:Byte Ptr)
	Function cairo_font_face_destroy(font_face:Byte Ptr)
	Function cairo_font_face_get_user_data:Byte Ptr(font_face:Byte Ptr, key:Byte Ptr)
	Function cairo_font_face_reference:Byte Ptr(font_face:Byte Ptr)
	Function cairo_font_face_set_user_data:Int(font_face:Byte Ptr, key:Byte Ptr, ..
														user_data:Byte Ptr, destory:Byte Ptr)
	Function cairo_font_face_status:Int(font_face:Byte Ptr)
	Function cairo_font_face_get_type:Int(font_face:Byte Ptr)
	
	Function cairo_font_options_copy:Byte Ptr(font_options:Byte Ptr)
	Function cairo_font_options_create:Byte Ptr()
	Function cairo_font_options_destroy(font_options:Byte Ptr)
	Function cairo_font_options_equal:Int(options1:Byte Ptr,option2:Byte Ptr)
	Function cairo_font_options_get_antialias:Int(font_options:Byte Ptr)
	Function cairo_font_options_get_hint_metrics:Int(font_options:Byte Ptr)
	Function cairo_font_options_get_hint_style:Int(font_options:Byte Ptr)
	Function cairo_font_options_get_subpixel_order:Int(font_options:Byte Ptr)
	Function cairo_font_options_hash:Long(font_options:Byte Ptr)
	Function cairo_font_options_merge(font_options:Byte Ptr, another_font_options:Byte Ptr)
	Function cairo_font_options_set_antialias(font_options:Byte Ptr, AntiAlias:Int)
	Function cairo_font_options_set_hint_metrics(font_options:Byte Ptr, hint_metrics:Int)
	Function cairo_font_options_set_hint_style(font_options:Byte Ptr, hint_style:Int)
	Function cairo_font_options_set_subpixel_order(font_options:Byte Ptr, subpixel_order:Int)
	Function cairo_font_options_status:Int(font_options:Byte Ptr)
	Function cairo_get_antialias:Int(context:Byte Ptr)
	Function cairo_get_current_point(context:Byte Ptr, x:Double Var, y:Double Var)
	Function cairo_get_fill_rule:Int(context:Byte Ptr)
	Function cairo_get_font_face:Byte Ptr(context:Byte Ptr)
	Function cairo_get_font_matrix(context:Byte Ptr, matrix:Byte Ptr)
	Function cairo_get_font_options(context:Byte Ptr, font_options:Byte Ptr)
	Function cairo_get_line_cap:Int(context:Byte Ptr)
	Function cairo_get_line_join:Int(context:Byte Ptr)
	Function cairo_get_line_width:Double(context:Byte Ptr)
	Function cairo_get_matrix(context:Byte Ptr, matrix:Byte Ptr)
	Function cairo_get_miter_limit:Double(context:Byte Ptr)
	Function cairo_get_operator:Int(context:Byte Ptr)
	Function cairo_get_source:Byte Ptr(context:Byte Ptr)
	Function cairo_get_target:Byte Ptr(context:Byte Ptr)
	Function cairo_get_tolerance:Double(context:Byte Ptr)
	Function cairo_glyph_extents(context:Byte Ptr, glyphs:Byte Ptr, ..
									num_glyphs:Int, extents:Byte Ptr)
	Function cairo_glyph_path(context:Byte Ptr, glyphs:Byte Ptr, num_glyphs:Int)
	Function cairo_identity_matrix(context:Byte Ptr)
	Function cairo_image_surface_create:Byte Ptr(format:Int, width:Int, height:Int)
	Function cairo_image_surface_create_for_data:Byte Ptr(data:Byte Ptr, format:Int, ..
																width:Int, height:Int, pitch:Int)
	Function cairo_image_surface_create_from_png:Byte Ptr(filename:Byte Ptr)
	Function cairo_image_surface_create_from_png_stream:Byte Ptr(read_func:Byte Ptr, ..
																		closure:Byte Ptr)
	Function cairo_image_surface_get_height:Int(surface:Byte Ptr)
	Function cairo_image_surface_get_width:Int(surface:Byte Ptr)
	Function cairo_image_surface_get_data:Byte Ptr(surface:Byte Ptr)
	Function cairo_image_surface_get_format:Int(surface:Byte Ptr)
	Function cairo_image_surface_get_stride:Int(surface:Byte Ptr)
	
	Function cairo_in_clip:Int(context:Byte Ptr, x:Double, y:Double)
	Function cairo_in_fill:Int(context:Byte Ptr, x:Double, y:Double)
	Function cairo_in_stroke:Int(context:Byte Ptr, x:Double, y:Double)
	Function cairo_line_to(context:Byte Ptr, x:Double, y:Double)
	Function cairo_mask(context:Byte Ptr, pattern:Byte Ptr)
	Function cairo_mask_surface(context:Byte Ptr, surface:Byte Ptr, ..
									surface_x:Double, surface_y:Double)
	Function cairo_matrix_init(matrix:Byte Ptr, xx:Double, yx:Double, xy:Double, ..
									yy:Double, x0:Double, y0:Double)
	Function cairo_matrix_init_identity(matrix:Byte Ptr)
	Function cairo_matrix_init_rotate(matrix:Byte Ptr, radians:Double)
	Function cairo_matrix_init_scale(matrix:Byte Ptr, sx:Double, sy:Double)
	Function cairo_matrix_init_translate(matrix:Byte Ptr, tx:Double, ty:Double)
	Function cairo_matrix_invert(matrix:Byte Ptr)
	Function cairo_matrix_multiply(result:Byte Ptr, a:Byte Ptr, b:Byte Ptr)
	Function cairo_matrix_rotate(matrix:Byte Ptr, radians:Double)
	Function cairo_matrix_scale(matrix:Byte Ptr, sx:Double, sy:Double)
	Function cairo_matrix_transform_distance(matrix:Byte Ptr, dx:Double Var, dy:Double Var)
	Function cairo_matrix_transform_point(matrix:Byte Ptr, x:Double Var, y:Double Var)
	Function cairo_matrix_translate(matrix:Byte Ptr, tx:Double, ty:Double)
	Function cairo_move_to(context:Byte Ptr, x:Double, y:Double)
	Function cairo_new_path(context:Byte Ptr)
	Function cairo_new_sub_path(context:Byte Ptr)
	Function cairo_paint(context:Byte Ptr)
	Function cairo_paint_with_alpha(context:Byte Ptr, alpha:Double)
	Function cairo_path_destroy(path:Byte Ptr)
	Function cairo_pattern_add_color_stop_rgb(pattern:Byte Ptr, offset:Double, ..
													r:Double, g:Double, b:Double)
	Function cairo_pattern_add_color_stop_rgba(pattern:Byte Ptr, offset:Double, ..
													r:Double, g:Double, b:Double, a:Double)
	Function cairo_pattern_create_for_surface:Byte Ptr(surface:Byte Ptr)
	Function cairo_pattern_create_linear:Byte Ptr(x0:Double, y0:Double, x1:Double, y1:Double)
	Function cairo_pattern_create_radial:Byte Ptr(cx0:Double, cy0:Double, radius0:Double, ..
														cx1:Double, cy1:Double, radius1:Double)
	Function cairo_pattern_create_rgb:Byte Ptr(red:Double, green:Double, blue:Double)
	Function cairo_pattern_create_rgba:Byte Ptr(r:Double, g:Double, b:Double, a:Double)
	Function cairo_pattern_destroy(pattern:Byte Ptr)
	Function cairo_pattern_get_extend:Int(pattern:Byte Ptr)
	Function cairo_pattern_get_filter:Int(pattern:Byte Ptr)
	Function cairo_pattern_get_matrix(pattern:Byte Ptr, matrix:Byte Ptr)
	Function cairo_pattern_reference:Byte Ptr(pattern:Byte Ptr)
	Function cairo_pattern_set_extend(pattern:Byte Ptr, extend:Int)
	Function cairo_pattern_set_filter(pattern:Byte Ptr, filter:Int)
	Function cairo_pattern_set_matrix(pattern:Byte Ptr, matrix:Byte Ptr)
	Function cairo_pattern_status:Int(pattern:Byte Ptr)
	Function cairo_pattern_get_type:Int(pattern:Byte Ptr)
	Function cairo_rectangle(context:Byte Ptr, x:Double, y:Double, width:Double, height:Double)
	Function cairo_reference:Byte Ptr(context:Byte Ptr)
	Function cairo_rel_curve_to(context:Byte Ptr, dx1:Double, dy1:Double, dx2:Double, ..
									dy2:Double, dx3:Double, dy3:Double)
	Function cairo_rel_line_to(context:Byte Ptr, dx:Double, dy:Double)
	Function cairo_rel_move_to(context:Byte Ptr, dx:Double, dy:Double)
	Function cairo_reset_clip(context:Byte Ptr)
	Function cairo_restore(context:Byte Ptr)
	Function cairo_rotate(context:Byte Ptr, angle:Double)
	Function cairo_save(context:Byte Ptr)
	Function cairo_scale(context:Byte Ptr, sx:Double, sy:Double)
	
	Function cairo_set_scaled_font(context:Byte Ptr, scaled_font:Byte Ptr)
	Function cairo_scaled_font_create:Byte Ptr(font_face:Byte Ptr, font_matrix:Byte Ptr, ..
													ctm:Byte Ptr, options:Byte Ptr)
	Function cairo_scaled_font_destroy(scaled_font:Byte Ptr)
	Function cairo_scaled_font_extents(scaled_font:Byte Ptr, extents:Byte Ptr)
	Function cairo_scaled_font_glyph_extents(scaled_font:Byte Ptr, glyphs:Byte Ptr, ..
													num_glyphs:Int, extents:Byte Ptr)
	Function cairo_scaled_font_reference:Byte Ptr(scaled_font:Byte Ptr)
	Function cairo_scaled_font_status:Int(scaled_font:Byte Ptr)
	Function cairo_scaled_font_text_extents(scaled_font:Byte Ptr, txt:Byte Ptr, extents:Byte Ptr)
	Function cairo_scaled_font_get_font_face:Byte Ptr(scaled_font:Byte Ptr)
	Function cairo_scaled_font_get_font_matrix(scaled_font:Byte Ptr, matrix:Byte Ptr)
	Function cairo_scaled_font_get_ctm(scaled_font:Byte Ptr, ctm:Byte Ptr)
	Function cairo_scaled_font_get_font_options(scaled_font:Byte Ptr, options:Byte Ptr)
	Function cairo_scaled_font_get_type:Int(scaled_font:Byte Ptr)
	
	Function cairo_select_font_face(context:Byte Ptr, family:Byte Ptr, slant:Int, weight:Int)
	Function cairo_set_antialias(context:Byte Ptr, AntiAlias:Int)
	Function cairo_set_dash(context:Byte Ptr, dashes:Byte Ptr, num_dashes:Int, offset:Double)
	Function cairo_set_fill_rule(context:Byte Ptr, fill_rule:Int)
	Function cairo_set_font_face(context:Byte Ptr, font_face:Byte Ptr)
	Function cairo_set_font_matrix(context:Byte Ptr, matrix:Byte Ptr)
	Function cairo_set_font_options(context:Byte Ptr, options:Byte Ptr)
	Function cairo_set_font_size(context:Byte Ptr, size:Double)
	Function cairo_set_line_cap(context:Byte Ptr, line_cap:Int)
	Function cairo_set_line_join(context:Byte Ptr, line_join:Int)
	Function cairo_set_line_width(context:Byte Ptr, width:Double)
	Function cairo_set_matrix(context:Byte Ptr, matrix:Byte Ptr)
	Function cairo_set_miter_limit(context:Byte Ptr, limit:Double)
	Function cairo_set_operator(context:Byte Ptr, op:Int)
	Function cairo_set_source(context:Byte Ptr, source:Byte Ptr)
	Function cairo_set_source_rgb(context:Byte Ptr, r:Double, g:Double, b:Double)
	Function cairo_set_source_rgba(context:Byte Ptr, r:Double, g:Double, b:Double, a:Double)
	Function cairo_set_source_surface(context:Byte Ptr, surface:Byte Ptr, x:Double, y:Double)
	Function cairo_set_tolerance(context:Byte Ptr, tolerance:Double)
	Function cairo_show_glyphs(context:Byte Ptr, glyphs:Byte Ptr, num_glyphs:Int)
	Function cairo_show_page(context:Byte Ptr)
	Function cairo_show_text(context:Byte Ptr, utf8string:Byte Ptr)
	Function cairo_status:Int(context:Byte Ptr)
	Function cairo_status_to_string:Byte Ptr(Status:Int)
	Function cairo_stroke(context:Byte Ptr)
	Function cairo_stroke_extents(context:Byte Ptr, x1:Double Var, y1:Double Var, ..
										x2:Double Var, y2:Double Var)
	Function cairo_stroke_preserve(context:Byte Ptr)
	Function cairo_surface_create_similar:Byte Ptr(other_surface:Byte Ptr, content:Int, ..
														width:Int, height:Int)
	Function cairo_surface_destroy(surface:Byte Ptr)
	Function cairo_surface_finish(surface:Byte Ptr)
	Function cairo_surface_flush(surface:Byte Ptr)
	Function cairo_surface_get_font_options(surface:Byte Ptr, options:Byte Ptr)
	Function cairo_surface_get_user_data:Byte Ptr(surface:Byte Ptr, key:Byte Ptr)
	Function cairo_surface_mark_dirty(surface:Byte Ptr)
	Function cairo_surface_mark_dirty_rectangle(surface:Byte Ptr, x:Int, y:Int, ..
														width:Int, height:Int)
	Function cairo_surface_reference:Byte Ptr(surface:Byte Ptr)
	Function cairo_surface_set_device_offset(surface:Byte Ptr, x_offset:Double,y_offset:Double)
	Function cairo_surface_set_user_data:Int(surface:Byte Ptr, key:Byte Ptr, ..
													user_data:Byte Ptr, Destroy:Int)
	Function cairo_surface_status:Int(surface:Byte Ptr)
	Function cairo_surface_write_to_png:Int(surface:Byte Ptr, filename:Byte Ptr)
	Function cairo_surface_write_to_png_stream:Int(surface:Byte Ptr, write_func:Byte Ptr, ..
															closure:Byte Ptr)
	Function cairo_surface_get_content:Int(surface:Byte Ptr)
	Function cairo_surface_get_device_offset(surface:Byte Ptr, x_offset:Double Var, y_offset:Double Var)
	Function cairo_surface_set_fallback_resolution(surface:Byte Ptr, x_pixels_per_inch:Double, ..
													y_pixels_per_inch:Double)
	Function cairo_surface_get_type:Int(surface:Byte Ptr)
															
	Function cairo_text_extents(context:Byte Ptr, utf8string:Byte Ptr, extents:Byte Ptr)
	Function cairo_text_path(context:Byte Ptr, utf8string:Byte Ptr)
	Function cairo_transform(context:Byte Ptr, matrix:Byte Ptr)
	Function cairo_translate(context:Byte Ptr, tx:Double, ty:Double)
	Function cairo_user_to_device(context:Byte Ptr, x:Double, y:Double)
	Function cairo_user_to_device_distance(context:Byte Ptr, dx:Double, dy:Double)
	Function cairo_version:Int()
	Function cairo_version_string:Byte Ptr()

	Function cairo_pdf_surface_create:Byte Ptr(filename:Byte Ptr, width:Double, height:Double)
	Function cairo_pdf_surface_set_size(surfacePtr:Byte Ptr, width:Double, height:Double)
	Function cairo_ps_surface_create:Byte Ptr(filename:Byte Ptr, width:Double, height:Double)
	Function cairo_ps_surface_set_size(surfacePtr:Byte Ptr, width:Double, height:Double)
	Function cairo_pdf_surface_create_for_stream:Byte Ptr(cb:Int(obj:Object, data:Byte Ptr, length:Int), obj:Object, width:Double, height:Double)
	Function cairo_ps_surface_create_for_stream:Byte Ptr(cb:Int(obj:Object, data:Byte Ptr, length:Int), obj:Object, width:Double, height:Double)
	Function cairo_pdf_version_to_string:Byte Ptr(version:Int)
	Function cairo_pdf_surface_restrict_to_version(surfacePtr:Byte Ptr, version:Int)
	Function cairo_pdf_get_versions(versions:Int Ptr Ptr, count:Int Var)
	
	Function cairo_push_group(context:Byte Ptr)
	Function cairo_push_group_with_content(context:Byte Ptr, content:Int)
	Function cairo_pop_group:Byte Ptr(context:Byte Ptr)
	Function cairo_pop_group_to_source(context:Byte Ptr)
	Function cairo_get_group_target:Byte Ptr(context:Byte Ptr)

	Function cairo_ft_font_face_create_for_ft_face:Byte Ptr(ft_face:Byte Ptr, loadFlags:Int)

	Function cairo_region_create:Byte Ptr()
	Function cairo_region_create_rectangle:Byte Ptr(rect:TCairoRectangleInt Ptr)
	Function cairo_region_copy:Byte Ptr(regionPtr:Byte Ptr)
	Function cairo_region_equal:Int(regionPtr:Byte Ptr, other:Byte Ptr)
	Function cairo_region_reference:Byte Ptr(regionPtr:Byte Ptr)
	Function cairo_region_status:Int(regionPtr:Byte Ptr)
	Function cairo_region_contains_point:Int(regionPtr:Byte Ptr, x:Int, y:Int)
	Function cairo_region_contains_rectangle:Int(regionPtr:Byte Ptr, rect:TCairoRectangleInt Ptr)
	Function cairo_region_is_empty:Int(regionPtr:Byte Ptr)
	Function cairo_region_translate(regionPtr:Byte Ptr, dx:Int, dy:Int)
	Function cairo_region_xor:Int(regionPtr:Byte Ptr, other:Byte Ptr)
	Function cairo_region_union:Int(regionPtr:Byte Ptr, other:Byte Ptr)
	Function cairo_region_get_extents(regionPtr:Byte Ptr, rect:TCairoRectangleInt Ptr)
	Function cairo_region_num_rectangles:Int(regionPtr:Byte Ptr)
	Function cairo_region_get_rectangle(regionPtr:Byte Ptr, index:Int, rect:TCairoRectangleInt Ptr)
	Function cairo_region_destroy(regionPtr:Byte Ptr)
	Function cairo_region_intersect:Int(regionPtr:Byte Ptr, other:Byte Ptr)
	Function cairo_region_subtract:Int(regionPtr:Byte Ptr, other:Byte Ptr)
	Function cairo_region_subtract_rectangle:Int(regionPtr:Byte Ptr, rect:TCairoRectangleInt Ptr)
	Function cairo_region_union_rectangle:Int(regionPtr:Byte Ptr, rect:TCairoRectangleInt Ptr)
	Function cairo_region_xor_rectangle:Int(regionPtr:Byte Ptr, rect:TCairoRectangleInt Ptr)
	
	'Function cairo_gl_surface_create:Byte Ptr(context:Byte Ptr, content:Int, width:Int, height:Int)
	'Function bmx_cairo_glcontext_create:Byte Ptr(context:Int)
	
	Type TCairoRectangleInt
		Field x:Int
		Field y:Int
		Field width:Int
		Field height:Int
	End Type
	
End Extern


Function _cairo_ISO8859toUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3]
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next
	Return String.fromBytes(s, count)
End Function

?win32
Const kCDEFAULT_FONTPATH:String = "C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
Const kCPATHSEPARATOR:String = ";"
?linux
Const kCDEFAULT_FONTPATH:String = "/usr/share/X11/fonts/TTF:/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:" + ..
	"/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/bitstream-vera:" + ..
	"/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1"
Const kCPATHSEPARATOR:String = ":"
?macos
Const kCDEFAULT_FONTPATH:String = "~~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
Const kCPATHSEPARATOR:String = ":"
?

?Not linux
Private
Type TFTMap
	Global _ft_map:TMap = New TMap

	Field face:Byte Ptr
	
	Function Create(src:String, face:Byte Ptr)
		Local this:TFTMap = New TFTMap
		this.face = face

		_ft_map.Insert(src, this)
	End Function

	Function GetFace:Byte Ptr(src:String)
		Local map:TFTMap = TFTMap(TFTMap._ft_map.ValueForKey(src))
		If map Then
			Return map.face
		End If
	End Function
			
End Type

Public
Function LoadFT:Byte Ptr( src$ )

	Local ft_face:Byte Ptr = TFTMap.GetFace(src)
	If ft_face Then
		Return ft_face
	End If

	Global ft_lib:Byte Ptr
	
	If Not ft_lib
		If FT_Init_FreeType( Varptr ft_lib ) Return Null
	EndIf

	Local buf:Byte Ptr,buf_size:Int
			
	'Local ft_face:Byte Ptr

	If src.Find( "::" )>0
		Local tmp:Byte[]=LoadByteArray( src )
		buf_size=tmp.length
		If Not buf_size Return Null
		buf=MemAlloc( buf_size )
		MemCopy buf,tmp,buf_size
		If FT_New_Memory_Face( ft_lib,buf,buf_size,0,Varptr ft_face )
			MemFree buf
			Return Null
		EndIf
	Else
		If FT_New_Face( ft_lib,src$,0,Varptr ft_face ) Return Null
	EndIf
	
	TFTMap.Create(src, ft_face)
	
	Return ft_face

End Function

Rem
bbdoc: User derived font path.
about: Add your own here.
End Rem
Global CAIRO_USER_FONTPATH:String = "."

Type TCustomCairoFont

	Function getFont:String(name:String)
		
		Local paths:String
		
		If CAIRO_USER_FONTPATH Then
			paths = CAIRO_USER_FONTPATH + kCPATHSEPARATOR + kCDEFAULT_FONTPATH
		Else
			paths = kCDEFAULT_FONTPATH
		End If
		
		Local fname:String = alternateFontList(name)
		
		If Not fname Then
			fname = name
		End If
		
		Return getFontPath(paths, fname)
			
	End Function

	Function alternateFontList:String(fname:String)
		Local fontList:String
		
		fname = fname.toLower()
		
		Local fontNoSep:String = ""
		
		If fname.find("-") > 0 Then
			fontNoSep = fname[0..fname.find("-")]
		Else If fname.find("_") > 0 Then
			fontNoSep = fname[0..fname.find("_")]
		Else
			fontNoSep = fname
		End If
		
		If fname = "times-bold" Or fontNoSep = "timesbd" Or fontNoSep = "timesb" Then
			fontList = "timesbd;Timesbd;TIMESBD;timesb;Timesb;TIMESB;VeraSeBd;Times"
		Else If fname = "times-italic" Or fontNoSep = "timesi" Then
			fontList = "timesi;Timesi;TIMESI;VeraIt;Times"
		Else If fname = "timesnewroman" Or fname = "timesnew" Or fname = "timesroman" Or fontNoSep = "times" Then
			fontList = "times;Times;TIMES;VeraSe"
		Else If fname = "arial-bold" Or fontNoSep = "arialb" Or fontNoSep = "arialbd" Then
			fontList = "arialb;Arialb;ARIALB;arialbd;Arialbd;ARIALBD;VeraBd;Helvetica"
		Else If fname = "arial-italic" Or fontNoSep = "ariali" Then
			fontList = "ariali;Ariali;ARIALI;VeraIt;Helvetica"
		Else If fname = "arial-bold-italic" Or fontNoSep = "arialbi" Then
			fontList = "arialbi;Arialbi;ARIALBI;VeraBI;Helvetica"
		Else If fontNoSep = "helvetica" Then
			fontList = "helvetica;Helvetica;HELVETICA;arial;Arial;ARIAL;Vera"
		Else If fontNoSep = "arial" Then
			fontList = "arial;Arial;ARIAL;Vera;helvetica;Helvetica;HELVETICA"
		Else If fontNoSep = "courier" Then
			fontList = "courier;Courier;COURIER;cour;VeraMono"
		End If
		
		Return fontList
	End Function
	
	Function getFontPath:String(paths:String, fontList:String)
	
		Local fontNames:String[] = fontList.Split(";")
		Local pathNames:String[] = paths.Split(kCPATHSEPARATOR)
		Local fullname:String
		Local found:Int = False
		
		For Local f:Int = 0 Until fontNames.length
		
		
			For Local p:Int = 0 Until pathNames.length
			
				Local baseName:String = pathNames[p] + "/" + fontNames[f]
				
				' already has a suffix ?
				If fontNames[f].find(".") > 0 Then
					
					If FileType(baseName) Then
						Return baseName
					End If
					
					' try next path...
					Continue
				End If
			
				fullName = baseName + ".ttf"

				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfa"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfb"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".dfont"
				If FileType(fullName) Then
					Return fullName
				End If
		
			Next
		
		Next
	End Function

End Type
?

