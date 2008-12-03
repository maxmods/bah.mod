SuperStrict


Rem
bbdoc: Cairo Vector Graphics Library
End Rem
Module BaH.cairo

ModuleInfo "Version: 1.21"
ModuleInfo "License: MPL / LGPL"
ModuleInfo "Author: University of Southern California and Carl D. Worth"
ModuleInfo "Credit: Adapted for BlitzMax by Duncan Cross and Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.21"
ModuleInfo "History: Cairo upgrade to 1.8.4"
ModuleInfo "History: Removal of fontconfig dependency on Win32 and Mac."
ModuleInfo "History: 1.20"
ModuleInfo "History: Fixed Text issues with UTF-8."
ModuleInfo "History: 1.19"
ModuleInfo "History: Cairo upgrade to 1.4.6."
ModuleInfo "History: Pixmap format changes."
ModuleInfo "History: 1.18"
ModuleInfo "History: Added CreateFromImage function to TCairoImageSurface."
ModuleInfo "History: Proper SetDash implementation. (API change!)"
ModuleInfo "History: Added group methods - PushGroup(), PushGroupWithContent(), PopGroup(), PopGroupToSource() and GetGroupTarget()."
ModuleInfo "History: Added CAIRO_EXTEND_PAD const."
ModuleInfo "History: Added pattern GetType() and pattern type consts."
ModuleInfo "History: Added NewSubPath(), SetScaledFont()."
ModuleInfo "History: Added font type consts."
ModuleInfo "History: Added scaled font methods - TextExtents(), GetFontFace(), GetFontOptions(), GetFontMatrix(), GetCTM() and GetType()."
ModuleInfo "History: Added font face GetType()."
ModuleInfo "History: Added surface methods - GetContent(), GetDeviceOffset(), SetFallbackResolution(), GetType(), and surface type consts."
ModuleInfo "History: Added image surface methods - GetData(), GetFormat() and GetStride()."
ModuleInfo "History: Added PDF and PS surface SetSize() methods."
ModuleInfo "History: Improved API documentation."
ModuleInfo "History: Linux now requires libcairo (1.2.0+) installed."
ModuleInfo "History: 1.17"
ModuleInfo "History: Linux now requires libcairo (1.1.6+) installed. Does not now use included source."
ModuleInfo "History: 1.16"
ModuleInfo "History: Cairo upgrade to 1.2.2."
ModuleInfo "History: 1.15"
ModuleInfo "History: Cairo upgrade to 1.1.10."
ModuleInfo "History: 1.14"
ModuleInfo "History: Fixed Mac (big-endian) image handling."
ModuleInfo "History: Had to change a slew of pixman code to support BGRA rather than ABGR."
ModuleInfo "History: 1.13"
ModuleInfo "History: Added TCairoMatrixStruct, GetFontMatrix() and rewrote TCairoMatrix code."
ModuleInfo "History: 1.12"
ModuleInfo "History: Added TCairoFontExtents type, more documentation and examples."
ModuleInfo "History: PDF doubles are now rounded to 2 decimal places - for slightly smaller file sizes."
ModuleInfo "History: 1.11"
ModuleInfo "History: Removed memory leak."
ModuleInfo "History: Added Stroke() to ShowText() - seems to be a bug on the PDF surface."
ModuleInfo "History: Added import for libpng"
ModuleInfo "History: 1.10"
ModuleInfo "History: Reorganization of Cairo Types to support multiple surface types. (now includes PDF and PS !!)"
ModuleInfo "History: Lots more documentation."
ModuleInfo "History: Addition of new types - TCairoTextExtents, TCairoPaperSize and TCairoPaperDimension."
ModuleInfo "History: 1.04"
ModuleInfo "History: Added Clear() convenience method."
ModuleInfo "History: 1.03"
ModuleInfo "History: Cairo upgrade to 1.1.6."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added initial Mac support - compiles but has major issues..."
ModuleInfo "History: 1.01"
ModuleInfo "History: Added PF_RGBA8888 Pixmap format for Linux"
ModuleInfo "History: Fixed Delete() method for Linux to prevent memory exception"
ModuleInfo "History: 1.00 Initial Release (Cairo 1.0.4)"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

' pixman-private.h
' includes config.h

Import BRL.Pixmap
Import BRL.Max2D
Import Pub.Zlib
Import Pub.libpng
'Import BaH.Fontconfig

Import "externs.bmx"

?win32
Import "source.bmx"
?macos
Import "source.bmx"
?linux
'Import "source.bmx"
Import "-lcairo"
?


' Constant declarations

Const CAIRO_LINE_CAP_BUTT:Int = 0
Const CAIRO_LINE_CAP_ROUND:Int = 1
Const CAIRO_LINE_CAP_SQUARE:Int = 2

Const CAIRO_LINE_JOIN_MITER:Int = 0
Const CAIRO_LINE_JOIN_ROUND:Int = 1
Const CAIRO_LINE_JOIN_BEVEL:Int = 2

Const CAIRO_FORMAT_ARGB32:Int = 0
Const CAIRO_FORMAT_RGB24:Int = 1
Const CAIRO_FORMAT_A8:Int = 2
Const CAIRO_FORMAT_A1:Int = 3

Const CAIRO_STATUS_SUCCESS:Int = 0
Const CAIRO_STATUS_NO_MEMORY:Int = 1
Const CAIRO_STATUS_INVALID_RESTORE:Int = 2
Const CAIRO_STATUS_INVALID_POP_GROUP:Int = 3
Const CAIRO_STATUS_NO_CURRENT_POINT:Int = 4
Const CAIRO_STATUS_INVALID_MATRIX:Int = 5
Const CAIRO_STATUS_INVALID_STATUS:Int = 6
Const CAIRO_STATUS_NULL_POINTER:Int = 7
Const CAIRO_STATUS_INVALID_STRING:Int = 8
Const CAIRO_STATUS_INVALID_PATH_DATA:Int = 9
Const CAIRO_STATUS_READ_ERROR:Int = 10
Const CAIRO_STATUS_WRITE_ERROR:Int = 11
Const CAIRO_STATUS_SURFACE_FINISHED:Int = 12
Const CAIRO_STATUS_SURFACE_TYPE_MISMATCH:Int = 13
Const CAIRO_STATUS_PATTERN_TYPE_MISMATCH:Int = 14
Const CAIRO_STATUS_INVALID_CONTENT:Int = 15
Const CAIRO_STATUS_INVALID_FORMAT:Int = 16
Const CAIRO_STATUS_INVALID_VISUAL:Int = 17
Const CAIRO_STATUS_FILE_NOT_FOUND:Int = 18
Const CAIRO_STATUS_INVALID_DASH:Int = 19

Const CAIRO_ANTIALIAS_DEFAULT:Int = 0
Const CAIRO_ANTIALIAS_NONE:Int = 1
Const CAIRO_ANTIALIAS_GRAY:Int = 2
Const CAIRO_ANTIALIAS_SUBPIXEL:Int = 3

Const CAIRO_FONT_SLANT_NORMAL:Int = 0
Const CAIRO_FONT_SLANT_ITALIC:Int = 1
Const CAIRO_FONT_SLANT_OBLIQUE:Int = 2

Const CAIRO_FONT_WEIGHT_NORMAL:Int = 0
Const CAIRO_FONT_WEIGHT_BOLD:Int = 1

Const CAIRO_EXTEND_NONE:Int = 0
Const CAIRO_EXTEND_REPEAT:Int = 1
Const CAIRO_EXTEND_REFLECT:Int = 2
Const CAIRO_EXTEND_PAD:Int = 3

Const CAIRO_OPERATOR_CLEAR:Int = 0
Const CAIRO_OPERATOR_SOURCE:Int = 1
Const CAIRO_OPERATOR_OVER:Int = 2
Const CAIRO_OPERATOR_IN:Int = 3
Const CAIRO_OPERATOR_OUT:Int = 4
Const CAIRO_OPERATOR_ATOP:Int = 5
Const CAIRO_OPERATOR_DEST:Int = 6
Const CAIRO_OPERATOR_DEST_OVER:Int = 7
Const CAIRO_OPERATOR_DEST_IN:Int = 8
Const CAIRO_OPERATOR_DEST_OUT:Int = 9
Const CAIRO_OPERATOR_DEST_ATOP:Int = 10
Const CAIRO_OPERATOR_XOR:Int = 11
Const CAIRO_OPERATOR_ADD:Int = 12
Const CAIRO_OPERATOR_SATURATE:Int = 13

Const CAIRO_CONTENT_COLOR:Int = 4096
Const CAIRO_CONTENT_ALPHA:Int = 8192
Const CAIRO_CONTENT_COLOR_ALPHA:Int = 12288

Const CAIRO_SUBPIXEL_ORDER_DEFAULT:Int = 0
Const CAIRO_SUBPIXEL_ORDER_RGB:Int = 1
Const CAIRO_SUBPIXEL_ORDER_BGR:Int = 2
Const CAIRO_SUBPIXEL_ORDER_VRGB:Int = 3
Const CAIRO_SUBPIXEL_ORDER_VBGR:Int = 4

Const CAIRO_HINT_STYLE_DEFAULT:Int = 0
Const CAIRO_HINT_STYLE_NONE:Int = 1
Const CAIRO_HINT_STYLE_SLIGHT:Int = 2
Const CAIRO_HINT_STYLE_MEDIUM:Int = 3
Const CAIRO_HINT_STYLE_FULL:Int = 4

Const CAIRO_HINT_METRICS_DEFAULT:Int = 0
Const CAIRO_HINT_METRICS_OFF:Int = 1
Const CAIRO_HINT_METRICS_ON:Int = 2

Const CAIRO_FILTER_FAST:Int = 0
Const CAIRO_FILTER_GOOD:Int = 1
Const CAIRO_FILTER_BEST:Int = 2
Const CAIRO_FILTER_NEAREST:Int = 3
Const CAIRO_FILTER_BILINEAR:Int = 4
Const CAIRO_FILTER_GAUSSIAN:Int = 5

Const CAIRO_PATTERN_TYPE_SOLID:Int = 0
Const CAIRO_PATTERN_TYPE_SURFACE:Int = 1
Const CAIRO_PATTERN_TYPE_LINEAR:Int = 2
Const CAIRO_PATTERN_TYPE_RADIAL:Int = 3

Const CAIRO_FONT_TYPE_TOY:Int = 0
Const CAIRO_FONT_TYPE_FT:Int = 1
Const CAIRO_FONT_TYPE_WIN32:Int = 2
Const CAIRO_FONT_TYPE_ATSUI:Int = 3

Const CAIRO_SURFACE_TYPE_IMAGE:Int = 0
Const CAIRO_SURFACE_TYPE_PDF:Int = 1
Const CAIRO_SURFACE_TYPE_PS:Int = 2
Const CAIRO_SURFACE_TYPE_XLIB:Int = 3
Const CAIRO_SURFACE_TYPE_XCB:Int = 4
Const CAIRO_SURFACE_TYPE_GLITZ:Int = 5
Const CAIRO_SURFACE_TYPE_QUARTZ:Int = 6
Const CAIRO_SURFACE_TYPE_WIN32:Int = 7
Const CAIRO_SURFACE_TYPE_BEOS:Int = 8
Const CAIRO_SURFACE_TYPE_DIRECTFB:Int = 9
Const CAIRO_SURFACE_TYPE_SVG:Int = 10

Rem
bbdoc: The Cairo context wrapper type
End Rem
Type TCairo

	' degrees to radians multiplier
	Const degToRad:Double = 0.017453292519943295
	
	' A pointer to the underlying data.
	Field contextPtr:Byte Ptr
	
	Field surface:TCairoSurface
	
	Rem
	bbdoc: Create a new TCairo object using the provided surface.
	returns: A new TCairo object.
	about: Parameters:
	<ul>
	<li><b>surf</b> : a #TCairoSurface object.</li>
	</ul>
	End Rem
	Function Create:TCairo(surf:TCairoSurface)
		Local cr:TCairo = New TCairo
	
		cr.surface = surf
		
		cr.createContext()
		
		Return cr
	End Function

	Rem
	bbdoc: Create a new Cairo context for objects to be drawn on.
	about: Typical usage might look something like:
	<pre>
	CreateContext()
	...  draw stuff
	Destroy()
	</pre>
	End Rem
	Method CreateContext()
		If contextPtr = Null Then
			contextPtr = cairo_create(surface.surfacePtr)
		End If
	End Method
	
	Method NoContextError()
		If contextPtr = Null Then
			Throw New TCairoNoContextException
		End If
	End Method

	Method Delete()
		surface.destroy()
	End Method

	Rem
	bbdoc: Append the path onto the current path.
	End Rem
	Method AppendPath(p:TCairoPath)
		NoContextError()
		cairo_append_path(contextPtr, p.pathPtr)
	End Method
	
	Rem
	bbdoc: Adds a circular arc of the given <b>radius</b> to the current path.
	about: The arc is centered at <b>(XCenter, YCenter)</b>, begins at <b>StartAngle</b> and proceeds in the direction
	of increasing angles to end at <b>EndAngle</b>. If <b>EndAngle</b> is less than <b>StartAngle</b> it will
	be progressively increased by 360 degrees until it is greater than <b>StartAngle</b>.
	<p>
	If there is a current point, an initial line segment will be added to the path to connect the current
	point to the beginning of the arc.
	</p>
	<p>
	Angles are measured in degrees. An angle of 0 is in the direction of the positive X axis (in user-space).
	An angle of 90 degrees is in the direction of the positive Y axis (in user-space).
	Angles increase in the direction from the positive X axis toward the positive Y axis.
	So with the default transformation matrix, angles increase in a clockwise direction.
	</p>
	<p>
	This function gives the arc in the direction of increasing angles; see #ArcNegative to get
	the arc in the direction of decreasing angles.
	</p>
	<p>
	The arc is circular in user-space. To achieve an elliptical arc, you can scale the current
	transformation matrix by different amounts in the X and Y directions.
	For example, to draw an ellipse in the box given by x, y, width, height:
	<pre>
Save()
Translate(x + width / 2.0, y + height / 2.0)
Scale (1.0 / (height / 2.0), 1.0 / (width / 2.0))
Arc(0, 0, 1, 0, 360)
Restore()
	</pre>
	</p>
	<p>Parameters:
	<ul>
	<li><b>XCenter</b> : X position of the center of the arc</li>
	<li><b>YCenter</b> : Y position of the center of the arc</li>
	<li><b>Radius</b> : the radius of the arc</li>
	<li><b>StartAngle</b> : the start angle, in degrees</li>
	<li><b>EndAngle</b> : the end angle, in degrees</li>
	</ul>
	</p>
	End Rem
	Method Arc(XCenter:Double, YCenter:Double, Radius:Double, ..
				StartAngle:Double, EndAngle:Double)
		NoContextError()
		cairo_arc(contextPtr, XCenter, YCenter, Radius, StartAngle * degToRad, EndAngle * degToRad)
	End Method
	
	Rem
	bbdoc: Adds a circular arc of the given <b>radius</b> to the current path.
	about: The arc is centered at <b>(XCenter, YCenter)</b>, begins at <b>StartAndle</b> and proceeds in the direction of
	decreasing angles to end at <b>EndAngle</b>. If <b>EndAngle</b> is greater than <b>StartAndle</b> it will be progressively
	decreased by 360 until it is greater than <b>StartAndle</b>.
	<p>
	See #Arc for more details. This function differs only in the direction of the arc between the two angles.
	</p>
	<p>Parameters:
	<ul>
	<li><b>XCenter</b> : X position of the center of the arc</li>
	<li><b>YCenter</b> : Y position of the center of the arc</li>
	<li><b>Radius</b> : the radius of the arc</li>
	<li><b>StartAngle</b> : the start angle, in degrees</li>
	<li><b>EndAngle</b> : the end angle, in degrees</li>
	</ul>
	</p>
	End Rem
	Method ArcNegative(XCenter:Double, YCenter:Double, Radius:Double, ..
				StartAngle:Double, EndAngle:Double)
		NoContextError()
		cairo_arc_negative(contextPtr, XCenter, YCenter, Radius, StartAngle * degToRad, EndAngle * degToRad)
	End Method
	
	Rem
	bbdoc: Clear an area of the context with the specified color at the current translation point.
	about: A convenience method for clearing / coloring an area of the context. You can use this for clearing the whole
	visible area of a context.
	<p>Parameters:
	<ul>
	<li><b>red</b> : red component of color</li>
	<li><b>green</b> : blue component of color</li>
	<li><b>blue</b> : green component of color</li>
	<li><b>width</b> : width of the area to be cleared</li>
	<li><b>height</b> : height of the area to be cleared</li>
	</ul>
	</p>	
	End Rem
	Method Clear(red:Double, green:Double, blue:Double, width:Double, height:Double)
		Rectangle(0, 0, width, height)
		SetSourceRGB(red, green, blue)
		Fill()
	End Method
	
	Rem
	bbdoc: Establishes a new clip region by intersecting the current clip region with the current path as it would be filled by #Fill and according to the current fill rule (see #SetFillRule).
	about: After #Clip, the current path will be cleared from the cairo context.
	<p>
	The current clip region affects all drawing operations by effectively masking out any changes to the surface
	that are outside the current clip region.<br>
	</p>
	<p>Calling #Clip can only make the clip region smaller, never larger. But the current clip is part of
	the graphics state, so a temporary restriction of the clip region can be achieved by calling
	#Clip within a #Save / #Restore pair. The only other means of increasing the size of the clip region
	is #ResetClip.
	</p>
	End Rem
	Method Clip()
		NoContextError()
		cairo_clip(contextPtr)
	End Method
	
	Rem
	bbdoc: Establishes a new clip region by intersecting the current clip region with the current path as it would be filled by #Fill and according to the current fill rule (see #SetFillRule).
	about: Unlike #Clip, #ClipPreserve preserves the path within the cairo context.
	<p>
	The current clip region affects all drawing operations by effectively masking out any changes to
	the surface that are outside the current clip region.
	</p>
	<p>
	Calling #ClipPreserve can only make the clip region smaller, never larger. But the current clip is
	part of the graphics state, so a temporary restriction of the clip region can be achieved by calling
	#ClipPreserve within a #Save / #Restore pair. The only other means of increasing the size
	of the clip region is #ResetClip.
	End Rem
	Method ClipPreserve()
		NoContextError()
		cairo_clip_preserve(contextPtr)
	End Method
	
	Rem
	bbdoc: Adds a line segment to the path from the current point to the beginning of the current subpath, (the most recent point passed to #MoveTo), and closes this subpath.
	about: The behavior of #ClosePath is distinct from simply calling #LineTo with the equivalent coordinate
	in the case of stroking. When a closed subpath is stroked, there are no caps on the ends of the subpath.
	Instead, their is a line join connecting the final and initial segments of the subpath.
	End Rem
	Method ClosePath()
		NoContextError()
		cairo_close_path(contextPtr)
	End Method
	
	Rem
	bbdoc: Takes a copy of the current surface to create a new Page.
	about: This is not supported by all surfaces and therefore may not appear to do anything.
	End Rem
	Method CopyPage()
		NoContextError()
		cairo_copy_page(contextPtr)
	End Method
	
	Rem
	bbdoc: Creates a copy of the current path and returns it to the user as a #TCairoPath.
	returns: The copy of the current path. The caller owns the returned object and should call #Destroy when finished with it.
	End Rem
	Method CopyPath:TCairoPath()
		TCairoPath._create(cairo_copy_path(contextPtr))
	End Method

	Rem
	bbdoc: Gets a flattened copy of the current path and returns it to the user as a #TCairoPath.
	returns: The copy of the current path. The caller owns the returned object and should call #Destroy when finished with it.
	about: This function is like #CopyPath except that any curves in the path will be approximated with
	piecewise-linear approximations, (accurate to within the current tolerance value). That is, the result
	is guaranteed to not have any elements of type CAIRO_PATH_CURVE_TO which will instead be replaced by
	a series of CAIRO_PATH_LINE_TO elements.
	End Rem
	Method CopyPathFlat:TCairoPath()
		TCairoPath._create(cairo_copy_path(contextPtr))
	End Method

	Rem
	bbdoc: Adds a cubic Bezier spline to the path from the current point to position <b>(x3, y3)</b> in user-space coordinates, using <b>(x1, y1)</b> and <b>(x2, y2)</b> as the control points.
	about: After this call the current point will be <b>(x3, y3)</b>.
	<p>Parameters:
	<ul>
	<li><b>x1</b> : the X coordinate of the first control point</li>
	<li><b>y1</b> : the Y coordinate of the first control point</li>
	<li><b>x2</b> : the X coordinate of the second control point</li>
	<li><b>y2</b> : the Y coordinate of the second control point</li>
	<li><b>x3</b> : the X coordinate of the end of the curve</li>
	<li><b>y3</b> : the Y coordinate of the end of the curve</li>
	</ul>
	</p>
	End Rem
	Method CurveTo(x1:Double,y1:Double,  x2:Double,y2:Double,  x3:Double,y3:Double)
		NoContextError()
		cairo_curve_to(contextPtr, x1,y1, x2,y2, x3,y3)
	End Method
	
	Rem
	bbdoc: Decreases the reference count by one.
	about: If the result is zero, then all associated resources are freed.
	End Rem
	Method Destroy()
		NoContextError()
		cairo_destroy(contextPtr)
		contextPtr = Null
	End Method
	
	Rem
	bbdoc: Transform a coordinate from device space to user space by multiplying the given point by the inverse of the current transformation matrix
	about:Parameters:
	<ul>
	<li><b>x</b> : X value of coordinate <i>(in/out parameter)</i></li>
	<li><b>y</b> : Y value of coordinate <i>(in/out parameter)</i></li>
	</ul>
	End Rem
	Method DeviceToUser(x:Double Var, y:Double Var)
		NoContextError()
		cairo_device_to_user(contextPtr, x, y)
	End Method
	
	Rem
	bbdoc: Transform a distance vector from device space to user space.
	about: This function is similar to #DeviceToUser except that the translation components of the inverse
	current transformation matrix will be ignored when transforming (dx,dy).
	<p>Parameters:
	<ul>
	<li><b>dx</b> : X component of a distance vector <i>(in/out parameter)</i></li>
	<li><b>dy</b> : Y component of a distance vector <i>(in/out parameter)</i></li>
	</ul>
	</p>
	End Rem
	Method DeviceToUserDistance(dx:Double Var, dy:Double Var)
		NoContextError()
		cairo_device_to_user_distance(contextPtr, dx, dy)
	End Method
	
	Rem
	bbdoc: A drawing operator that fills the current path according to the current fill rule, (each sub-path is implicitly closed before being filled).
	about: After #Fill, the current path will be cleared from the cairo context. See #SetFillRule and #FillPreserve.
	End Rem
	Method Fill()
		NoContextError()
		cairo_fill(contextPtr)
	End Method
	
	Rem
	bbdoc: Returns the extents for the current fill.
	End Rem
	Method FillExtents(x1:Double Var,y1:Double Var, x2:Double Var,y2:Double Var)
		NoContextError()
		cairo_fill_extents(contextPtr, x1,y1, x2,y2)
	End Method
	
	Rem
	bbdoc: A drawing operator that fills the current path according to the current fill rule, (each sub-path is implicitly closed before being filled).
	about: Unlike #Fill, #FillPreserve preserves the path within the cairo context.
	End Rem
	Method FillPreserve()
		NoContextError()
		cairo_fill_preserve(contextPtr)
	End Method
	
	Rem
	bbdoc: Gets the current shape antialiasing mode, as set by #SetAntialias.
	returns : the current shape antialiasing mode.
	End Rem
	Method GetAntialias:Int()
		NoContextError()
		cairo_get_antialias(contextPtr)
	End Method
	
	Rem
	bbdoc: Returns the current point in the context.
	End Rem
	Method GetCurrentPoint(x:Double Var,y:Double Var)
		NoContextError()
		cairo_get_current_point(contextPtr,x,y)
	End Method
	
	Rem
	bbdoc: Gets the current fill rule, as set by #SetFillRule.
	returns : the current fill rule.
	End Rem
	Method GetFillRule:Int()
		NoContextError()
		Return cairo_get_fill_rule(contextPtr)
	End Method
	
	Rem
	bbdoc: Gets the current font face for the context.
	returns: The current font object. Can return Null on out-of-memory or if the context is already in an error state. <b>This object is owned by cairo</b>. To keep a reference to it, you must call fontface.Reference()
	End Rem
	Method GetFontFace:TCairoFontFace()
		Return TCairoFontFace._create(cairo_get_font_face(contextPtr))
	End Method

	Rem
	bbdoc: Gets the current font matrix
	returns: return value for the matrix.
	about: See #TCairoMatrix for more information.
	End Rem
	Method GetFontMatrix:TCairoMatrix()
		Local r:TCairoMatrixStruct = New TCairoMatrixStruct
		cairo_get_font_matrix(contextPtr, r)
		Return TCairoMatrix._create(r)
	End Method

	Rem
	bbdoc: Gets the target surface for the current group as started by the most recent call to #PushGroup or #PushGroupWithContent.
	returns: The target group surface, or Null if none. This object is owned by cairo. To keep a reference to it, you must call surface.Reference().
	about: This method will return Null if called "outside" of any group rendering blocks, (that is, after
	the last balancing call to #PopGroup or #PopGroupToSource ).
	End Rem
	Method GetGroupTarget:TCairoSurface()
		NoContextError()
		Return TCairoSurface._create(cairo_get_group_target(contextPtr))
	End Method
	
	Rem
	bbdoc: Gets the current line cap style, as set by #SetLineCap.
	returns : the current line cap style.
	End Rem
	Method GetLineCap:Int()
		NoContextError()
		Return cairo_get_line_cap(contextPtr)
	End Method

	Rem
	bbdoc: Gets the current line join style, as set by #SetLineJoin.
	returns : the current line join style.
	End Rem
	Method GetLineJoin:Int()
		NoContextError()
		Return cairo_get_line_join(contextPtr)
	End Method

	Rem
	bbdoc: Gets the current line width, as set by #SetLineWidth.
	returns : the current line width, in user-space units.
	End Rem
	Method GetLineWidth:Double()
		NoContextError()
		Return cairo_get_line_width(contextPtr)
	End Method

	Rem
	bbdoc: Gets the miter limit, as set by #SetMiterLimit.
	returns : the current miter limit.
	End Rem
	Method GetMiterLimit:Double()
		NoContextError()
		Return cairo_get_miter_limit(contextPtr)
	End Method

	Rem
	bbdoc: Gets the current compositing operator for the cairo context.
	about: See #SetOperator for details of returnable values.
	returns : the current compositing operator.
	End Rem
	Method GetOperator:Int()
		NoContextError()
		Return cairo_get_operator(contextPtr)
	End Method
	
	Rem
	bbdoc: Gets the current source pattern for the context.
	returns: The current source pattern. <b>This object is owned by cairo</b>. To keep a reference to it, you must call pattern.Reference()
	End Rem
	Method GetSource:TCairoPattern()
		Return TCairoPattern._create(cairo_get_source(contextPtr))
	End Method

	Rem
	bbdoc: Gets the target surface for the cairo context as passed to #Create.
	returns: The target surface. <b>This object is owned by cairo</b>. To keep a reference to it, you must call cairosurface.Reference()
	about: This function will always return a valid object, but the result can be a "nil" surface if the context is already in an error state, (ie. cairo.Status() <> CAIRO_STATUS_SUCCESS). A nil surface is indicated by cairosurface.Status() <> CAIRO_STATUS_SUCCESS.
	End Rem
	Method GetTarget:TCairoSurface()
		Return surface
	End Method

	Rem
	bbdoc: Gets the current tolerance value, as set by #SetTolerance.
	returns : the current tolerance value.
	End Rem
	Method GetTolerance:Double()
		NoContextError()
		Return cairo_get_tolerance(contextPtr)
	End Method
	
'	Method GlyphExtents

'	Method GlyphPath

	Rem
	bbdoc: Resets the current transformation matrix by setting it equal to the identity matrix.
	about: That is, the user-space and device-space axes will be aligned and one user-space unit will
	transform to one device-space unit.
	End Rem
	Method IdentityMatrix()
		NoContextError()
		cairo_identity_matrix(contextPtr)
	End Method

	Rem
	bbdoc: Tests whether the given point is inside the area that would be filled by doing a #Fill operation given the current path and filling parameters.
	returns: A non-zero value if the point is inside, or zero if outside.
	about: See #Fill, #SetFillRule and #FillPreserve.
	<p>Parameters:
	<ul>
	<li><b>x</b> : X coordinate of the point to test</li>
	<li><b>y</b> : Y coordinate of the point to test</li>
	</ul>
	</p>
	End Rem
	Method InFill:Int(X:Double, Y:Double)
		NoContextError()
		Return cairo_in_fill(contextPtr, x, y)
	End Method
	
	Rem
	bbdoc: Tests whether the given point is inside the area that would be stroked by doing a #Stroke operation given the current path and stroking parameters.
	returns: A non-zero value if the point is inside, or zero if outside.
	about: See #Stroke, #SetLineWidth, #SetLineJoin, #SetLineCap, #SetDash, and #StrokePreserve.
	<p>Parameters:
	<ul>
	<li><b>x</b> : X coordinate of the point to test</li>
	<li><b>y</b> : Y coordinate of the point to test</li>
	</ul>
	</p>
	End Rem
	Method InStroke:Int(X:Double, Y:Double)
		NoContextError()
		Return cairo_in_stroke(contextPtr, x, y)
	End Method
	
	Rem
	bbdoc: Adds a line to the path from the current point to position <b>(x, y)</b> in user-space coordinates.
	about: After this call the current point will be <b>(x, y)</b>.
	<p>Parameters:
	<ul>
	<li><b>x</b> : the X coordinate of the end of the new line</li>
	<li><b>y</b> : the Y coordinate of the end of the new line</li>
	</ul>
	</p>
	End Rem
	Method LineTo(x:Double, y:Double)
		NoContextError()
		cairo_line_to(contextPtr, x, y)
	End Method
	
	Rem
	bbdoc: A drawing operator that paints the current source using the alpha channel of pattern as a mask.
	about: Opaque areas of <b>mask</b> are painted with the source, transparent areas are not painted.
	<p>Parameters:
	<ul>
	<li><b>pattern</b> : a TCairoPattern</li>
	</ul>
	</p>
	End Rem
	Method Mask(pattern:TCairoPattern)
		NoContextError()
		cairo_mask(contextPtr, pattern.patternPtr)
	End Method
	
	Rem
	bbdoc: A drawing operator that paints the current source using the alpha channel of surface as a mask.
	about: Opaque areas of surface are painted with the source, transparent areas are not painted.
	<p>Parameters:
	<ul>
	<li><b>surface</b> : a TCairoSurface</li>
	<li><b>surface_x</b> : X coordinate at which to place the origin of <b>surface</b></li>
	<li><b>surface_y</b> : Y coordinate at which to place the origin of <b>surface</b></li>
	</ul>
	</p>
	End Rem
	Method MaskSurface(surface:TCairoSurface, SurfaceX:Double, SurfaceY:Double)
		NoContextError()
		cairo_mask_surface(contextPtr, surface.surfacePtr, SurfaceX, SurfaceY)
	End Method

	Rem
	bbdoc: If the current subpath is not empty, begin a new subpath.
	about: After this call the current point will be <b>(x, y)</b>.
	<p>Parameters:
	<ul>
	<li><b>x</b> : the X coordinate of the new position</li>
	<li><b>y</b> : the Y coordinate of the new position</li>
	</ul>
	</p>
	End Rem
	Method MoveTo(x:Double, y:Double)
		NoContextError()
		cairo_move_to(contextPtr, x,y)
	End Method
	
	Rem
	bbdoc: Clears the current path.
	about: After this call there will be no current point.
	End Rem
	Method NewPath()
		NoContextError()
		cairo_new_path(contextPtr)
	End Method
	
	Rem
	bbdoc: Begin a new sub-path.
	about: Note that the existing path is not affected. After this call there will be no current point.
	<p>In many cases, this call is not needed since new sub-paths are frequently started with #MoveTo.</p>
	<p>A call to #NewSubPath is particularly useful when beginning a new sub-path with one of the
	#Arc calls. This makes things easier as it is no longer necessary to manually compute the arc's
	initial coordinates for a call to #MoveTo.</p>
	End Rem
	Method NewSubPath()
		NoContextError()
		cairo_new_sub_path(contextPtr)
	End Method
	
	Rem
	bbdoc: A drawing operator that paints the current source everywhere within the current clip region.
	End Rem
	Method Paint()
		NoContextError()
		cairo_paint(contextPtr)
	End Method
	
	Rem
	bbdoc: A drawing operator that paints the current source everywhere within the current clip region using a mask of constant alpha value <b>alpha</b>.
	about: The effect is similar to #Paint, but the drawing is faded out using the alpha value.
	<p>Parameters:
	<ul>
	<li><b>alpha</b> : alpha value, between 0 (transparent) and 1 (opaque)</li>
	</ul>
	</p>
	End Rem
	Method PaintWithAlpha(alpha:Double)
		NoContextError()
		cairo_paint_with_alpha(contextPtr, alpha)
	End Method
	
	Rem
	bbdoc: Terminates the redirection begun by a call to #PushGroup or #PushGroupWithContent and returns a new pattern containing the results of all drawing operations performed to the group.
	returns: A newly created (surface) pattern containing the results of all drawing operations performed to the group. The TCairoPattern should be destroyed when finished with.
	about: The #PopGroup method calls #Restore, (balancing a call to #Save by the PushGroup method), so that
	any changes to the graphics state will not be visible outside the group.
	End Rem
	Method PopGroup:TCairoPattern()
		NoContextError()
		Return TCairoPattern._create(cairo_pop_group(contextPtr))
	End Method
	
	Rem
	bbdoc: Terminates the redirection begun by a call to #PushGroup or #PushGroupWithContent and installs the resulting pattern as the source pattern in the cairo context.
	about: The behavior of this function is equivalent to the sequence of operations:
	<pre>
local group:TCairoPattern = cairo.PopGroup()
cairo.SetSource(group)
group.Destroy()
	</pre>
	but is more convenient as there is no need for a variable to store the short-lived pointer to the pattern.
	<p>The #PopGroup method calls #Restore, (balancing a call to #Save by the PushGroup method), so that any
	changes to the graphics state will not be visible outside the group.</p>
	End Rem
	Method PopGroupToSource()
		NoContextError()
		cairo_pop_group_to_source(contextPtr)
	End Method

	Rem
	bbdoc: Temporarily redirects drawing to an intermediate surface known as a group.
	about: The redirection lasts until the group is completed by a call to #PopGroup or #PopGroupToSource.
	These calls provide the result of any drawing to the group as a pattern, (either as an explicit
	object, or set as the source pattern).
	<p>This group functionality can be convenient for performing intermediate compositing. One common use
	of a group is to render objects as opaque within the group, (so that they occlude each other), and then
	blend the result with translucence onto the destination.</p>
	<p>Groups can be nested arbitrarily deep by making balanced calls to #PushGroup / #PopGroup.
	Each call pushes/pops the new target group onto/from a stack.</p>
	<p>The #PushGroup function calls #Save so that any changes to the graphics state will not be visible
	outside the group, (the pop_group functions call #Restore ).</p>
	<p>By default the intermediate group will have a content type of CAIRO_CONTENT_COLOR_ALPHA. Other content
	types can be chosen for the group by using #PushGroupWithContent instead.</p>
	<p>As an example, here is how one might fill and stroke a path with translucence, but without any portion
	of the fill being visible under the stroke:</p>
	<pre>
cairo.PushGroup()
cairo.SetSource(fill_pattern)
cairo.FillPreserve()
cairo.SetSource(stroke_pattern)
cairo.Stroke()
cairo.PopGroupToSource()
cairo.PaintWithAlpha(alpha)
	</pre>
	End Rem
	Method PushGroup()
		NoContextError()
		cairo_push_group(contextPtr)
	End Method

	Rem
	bbdoc: Temporarily redirects drawing to an intermediate surface known as a group.
	about: The redirection lasts until the group is completed by a call to #PopGroup or #PopGroupToSource. 
	These calls provide the result of any drawing to the group as a pattern, (either as an explicit object,
	or set as the source pattern).
	<p>The group will have a content type of @content (see below). The ability to control this content type is the only
	distinction between this function and #PushGroup which you should see for a more detailed description
	of group rendering.</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_CONTENT_COLOR</td><td>The surface will hold color content only</td></tr>
	<tr><td>CAIRO_CONTENT_ALPHA</td><td>The surface will hold alpha content only</td></tr>
	<tr><td>CAIRO_CONTENT_COLOR_ALPHA</td><td>The surface will hold color and alpha content</td></tr>
	</table>
	End Rem
	Method PushGroupWithContent(content:Int)
		NoContextError()
		cairo_push_group_with_content(contextPtr, content)
	End Method
	
	Rem
	bbdoc: Adds a closed-subpath rectangle of the given size to the current path at position <b>(x, y)</b> in user-space coordinates.
	about: This function is logically equivalent to:
	<pre>
MoveTo(x, y)
RelLineTo(width, 0)
RelLineTo(0, height)
RelLineTo(-width, 0)
Close_path()
	</pre>
	<p>Parameters:
	<ul>
	<li><b>x</b> : the X coordinate of the top left corner of the rectangle</li>
	<li><b>y</b> : the Y coordinate of the top left corner of the rectangle</li>
	<li><b>width</b> : the width of the rectangle</li>
	<li><b>height</b> : the height of the rectangle</li>
	</ul>
	</p>
	End Rem
	Method Rectangle(x:Double, y:Double, width:Double, height:Double)
		NoContextError()
		cairo_rectangle(contextPtr, x,y,width,height)
	End Method

	Rem
	bbdoc: Increases the reference count on the context by one.
	about: This prevents the context from being destroyed until a matching call to #Destroy is made.
	End Rem
	Method Reference()
		contextPtr = cairo_reference(contextPtr)
	End Method

	Rem
	bbdoc: Relative-coordinate version of #CurveTo.
	about: All offsets are relative to the current point. Adds a cubic Bezier spline to the path from
	the current point to a point offset from the current point by <b>(dx3, dy3)</b>, using points offset
	by <b>(dx1, dy1)</b> and <b>(dx2, dy2)</b> as the control points. After this call the current point
	will be offset by <b>(dx3, dy3)</b>.
	<p>
	Given a current point of (x, y), RelCurveTo(dx1, dy1, dx2, dy2, dx3, dy3) is logically equivalent to
	CurveTo(x + dx1, y + dy1, x + dx2, y + dy2, x + dx3, y + dy3).
	</p>
	<p>Parameters:
	<ul>
	<li><b>dx1</b> : the X offset to the first control point</li>
	<li><b>dy1</b> : the Y offset to the first control point</li>
	<li><b>dx2</b> : the X offset to the second control point</li>
	<li><b>dy2</b> : the Y offset to the second control point</li>
	<li><b>dx3</b> : the X offset to the end of the curve</li>
	<li><b>dy3</b> : the Y offset to the end of the curve</li>
	</ul>
	</p>
	End Rem
	Method RelCurveTo(dx1:Double,dy1:Double, dx2:Double,dy2:Double, dx3:Double,dy3:Double)
		NoContextError()
		cairo_rel_curve_to(contextPtr, dx1,dy1, dx2,dy2, dx3,dy3)
	End Method
	
	Rem
	bbdoc: Relative-coordinate version of #LineTo.
	about: Adds a line to the path from the current point to a point that is offset from the current point
	by <b>(dx, dy)</b> in user space. After this call the current point will be offset by <b>(dx, dy)</b>.
	<p>
	Given a current point of (x, y), RelLineTo(dx, dy) is logically equivalent to LineTo (x + dx, y + dy)
	</p>
	<p>Parameters:
	<ul>
	<li><b>dx</b> : the X offset to the end of the new line</li>
	<li><b>dy</b> : the Y offset to the end of the new line</li>
	</ul>
	</p>
	End Rem
	Method RelLineTo(dx:Double, dy:Double)
		NoContextError()
		cairo_rel_line_to(contextPtr, dx,dy)
	End Method

	Rem
	bbdoc: If the current subpath is not empty, begin a new subpath.
	about: After this call the current point will offset by <b>(dx, dy)</b>.
	<p>
	Given a current point of (x, y), RelMoveTo(dx, dy) is logically equivalent to MoveTo(x + dx, y + dy).
	</p>
	<p>Parameters:
	<ul>
	<li><b>dx</b> : the X offset</li>
	<li><b>dy</b> : the Y offset</li>
	</ul>
	</p>
	End Rem
	Method RelMoveTo(dx:Double, dy:Double)
		NoContextError()
		cairo_rel_move_to(contextPtr, dx,dy)
	End Method

	Rem
	bbdoc: Reset the current clip region to its original, unrestricted state.
	about: That is, set the clip region to an infinitely large shape containing the target surface.
	Equivalently, if infinity is too hard to grasp, one can imagine the clip region being reset to the
	exact bounds of the target surface.
	<p>
	Note that code meant to be reusable should not call #ResetClip as it will cause results
	unexpected by higher-level code which calls #Clip. Consider using #Save and #Rrestore
	around #Clip as a more robust means of temporarily restricting the clip region.
	</p>
	End Rem
	Method ResetClip()
		NoContextError()
		cairo_reset_clip(contextPtr)
	End Method

	Rem
	bbdoc: Restores the state saved by a preceding call to #Save and removes that state from the stack of saved states.
	End Rem
	Method Restore()
		NoContextError()
		cairo_restore(contextPtr)
	End Method
	
	Rem
	bbdoc: Modifies the current transformation matrix by rotating the user-space axes by <b>angle</b> degrees.
	about: The rotation of the axes takes places after any existing transformation of user space.
	The rotation direction for positive angles is from the positive X axis toward the positive Y axis.
	<p>Parameters:
	<ul>
	<li><b>angle</b> : angle (in degrees) by which the user-space axes will be rotated</li>
	</ul>
	</p>
	End Rem
	Method Rotate(angle:Double)
		NoContextError()
		cairo_rotate(contextPtr, angle * degToRad)
	End Method

	Rem
	bbdoc: Makes a copy of the current state and saves it on an internal stack of saved states.
	about: When #Restore is called, it will be restored to the saved state.
	Multiple calls to #Save and #Restore can be nested; each call to #Restore restores the state
	from the matching paired #Save.<br>
	It isn't necessary to clear all saved states before an object is freed. If the reference count of a
	object drops to zero in response to a call to #Destroy, any saved states will be freed along with the object.
	End Rem
	Method Save()
		NoContextError()
		cairo_save(contextPtr)
	End Method
	
	Rem
	bbdoc: Modifies the current transformation matrix by scaling the X and Y user-space axes.
	about: The scaling of the axes takes place after any existing transformation of user space.
	<p>Parameters:
	<ul>
	<li><b>sx</b> : scale factor for the X dimension</li>
	<li><b>sy</b> : scale factor for the Y dimension</li>
	</ul>
	</p>
	End Rem
	Method Scale(sx:Double, sy:Double)
		NoContextError()
		cairo_scale(contextPtr, sx,sy)
	End Method

	Rem
	bbdoc: Selects a family and style of font from a simplified description as a family name, slant and weight.
	about: This function is meant to be used only for applications with simple font needs: Cairo doesn't provide
	for operations such as listing all available fonts on the system, and it is expected that most
	applications will need to use a more comprehensive font handling and text layout library in addition to cairo.
	<p>Parameters:
	<ul>
	<li><b>family</b> : a font family name</li>
	<li><b>slant</b> : the slant for the font</li>
	<li><b>weight</b> : the weight for the font</li>
	</ul>
	</p>
	End Rem
	Method SelectFontFace( family:String, slant:Int, weight:Int)
		NoContextError()

?Not linux
		Local ft_face:Byte Ptr = loadft(TCustomCairoFont.getFont(family))
		If Not ft_face
		End If

		Local cface:TCairoFontFace = TCairoFontFace._create(cairo_ft_font_face_create_for_ft_face(ft_face, 0))
		SetFontFace(cface)
?linux
		cairo_select_font_face( contextPtr, _cairo_ISO8859toUTF8(family) , slant, weight)
?
	End Method

	Rem
	bbdoc: Set the antialiasing mode of the rasterizer used for drawing shapes.
	about: This value is a hint, and a particular backend may or may not support a particular value.
	At the current time, no backend supports CAIRO_ANTIALIAS_SUBPIXEL when drawing shapes.
	<p>Note that this option does not affect text rendering, instead see TCairoFontOptions.SetAntiAlias().</p>
	Possible Antialias values :<br>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_ANTIALIAS_DEFAULT</td><td>Use the default antialiasing for the subsystem and target device</td></tr>
	<tr><td>CAIRO_ANTIALIAS_NONE</td><td>Use a bilevel alpha mask</td></tr>
	<tr><td>CAIRO_ANTIALIAS_GRAY</td><td>Perform single-color antialiasing (using shades of gray for black text
	on a white background, for example).</td></tr>
	<tr><td>CAIRO_ANTIALIAS_SUBPIXEL</td><td>Perform antialiasing by taking advantage of the order of
	subpixel elements on devices such as LCD panels</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>antialias</b> : the new antialiasing mode</li>
	</ul>
	</p>
	End Rem
	Method SetAntialias(aa:Int)
		NoContextError()
		cairo_set_antialias(contextPtr, aa)
	End Method

	Rem
	bbdoc: Sets the dash pattern to be used by #Stroke.
	about: A dash pattern is specified by <b>dashes</b>, an array of positive values. Each value provides the
	user-space length of alternate "on" and "off" portions of the stroke. The <b>offset</b> specifies an offset
	into the pattern at which the stroke begins.
	<p>Each "on" segment will have caps applied as if the segment were a separate sub-path. In particular,
	it is valid to use an "on" length of 0.0 with CAIRO_LINE_CAP_ROUND or CAIRO_LINE_CAP_SQUARE in order
	to distributed dots or squares along a path.</p>
	<p>Note: The length values are in user-space units as evaluated at the time of stroking. This is not
	necessarily the same as the user space at the time of #SetDash</p>
	<p>If <b>num_dashes</b> is 0 dashing is disabled.</p>
	<p>If <b>num_dashes</b> is 1 a symmetric pattern is assumed with alternating on and off portions of the
	size specified by the single value in <b>dashes</b>.</p>
	<p>If any value in <b>dashes</b> is negative, or if all values are 0, then object will be put
	into an error state with a status of CAIRO_STATUS_INVALID_DASH.</p>
	<p>Parameters:
	<ul>
	<li><b>dashes</b> : an array specifying alternate lengths of on and off</li>
	<li><b>offset</b> : an offset into the dash pattern at which the stroke should start (optional)</li>
	</ul>
	</p>
	End Rem
	Method SetDash(dashes:Double[], offset:Double = 0)
		NoContextError()
		If dashes = Null Or dashes.length = 0 Then
			cairo_set_dash(contextPtr, Null, 0, offset)
		Else
			Local num_dashes:Int = dashes.length
			' create memory for doubles array
			Local dashBank:TBank = CreateBank(num_dashes * 8)
			' populate the double mem
			For Local i:Int = 0 Until num_dashes
				dashBank.PokeDouble(i * 8, dashes[i])
			Next
			cairo_set_dash(contextPtr, dashBank.lock(), num_dashes, offset)
			
			dashBank.unLock()
		End If
	End Method
	
	Rem
	bbdoc: Set the current fill rule within the cairo context.
	about: The fill rule is used to determine which regions are inside or outside a complex (potentially
	self-intersecting) path. The current fill rule affects both #Fill and #Clip.
	<p>
	The following details the semantics of each available fill rule :
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_FILL_RULE_WINDING</td><td>If the path crosses the ray from left-to-right, counts +1.
	If the path crosses the ray from right to left, counts -1. (Left and right are determined from the perspective
	of looking along the ray from the starting point.) If the total count is non-zero, the point will be filled.</td></tr>
	<tr><td>CAIRO_FILL_RULE_EVEN_ODD</td><td>Counts the total number of intersections, without regard to the
	orientation of the contour. If the total number of intersections is odd, the point will be filled.</td></tr>
	</table>
	<p>
	For both fill rules, whether or not a point is included in the fill is determined by taking a ray from
	that point to infinity and looking at intersections with the path. The ray can be in any direction,
	as long as it doesn't pass through the end point of a segment or have a tricky intersection such as
	intersecting tangent to the path. (Note that filling is not actually implemented in this way.
	This is just a description of the rule that is applied.)
	</p>
	<p>Parameters:
	<ul>
	<li><b>fill_rule</b> : a fill rule, specified as above</li>
	</ul>
	</p>
	End Rem
	Method SetFillRule(rule:Int)
		NoContextError()
		cairo_set_fill_rule(contextPtr, rule)
	End Method

	Rem
	bbdoc: Set the current font face within the cairo context.
	about: Parameters:
	<ul>
	<li><b>face</b> : a #TCairoFontFace object.</li>
	</ul>
	End Rem
	Method SetFontFace(face:TCairoFontFace)
		NoContextError()
		cairo_set_font_face(contextPtr, face.facePtr)
	End Method
	
	Rem
	bbdoc: Set the current font matrix within the cairo context.
	about: Parameters:
	<ul>
	<li><b>matrix</b> : a #TCairoMatrix object.</li>
	</ul>
	End Rem
	Method SetFontMatrix(matrix:TCairoMatrix)
		NoContextError()
		cairo_set_font_matrix(contextPtr, matrix.matrix)
	End Method
	
	Rem
	bbdoc: Set the current font options within the cairo context.
	about: Parameters:
	<ul>
	<li><b>options</b> : a #TCairoFontOptions object.</li>
	</ul>
	End Rem
	Method SetFontOptions(options:TCairoFontOptions)
		NoContextError()
		cairo_set_font_options(contextPtr, options.optionsPtr)
	End Method

	Rem
	bbdoc: Sets the current font size.
	about: Parameters:
	<ul>
	<li><b>size</b> : the new font size to set.</li>
	</ul>
	End Rem
	Method SetFontSize(size:Double)
		NoContextError()
		cairo_set_font_size(contextPtr, size)
	End Method

	Rem
	bbdoc: Sets the current line cap style within the cairo context.
	about: As with the other stroke parameters, the current line cap style is examined by
	#Stroke, #StrokeExtents, and #StrokeToPath, but does not have any effect during path construction.
	<p>
	The following details how the available line cap styles are drawn :
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_LINE_CAP_BUTT</td><td>start(stop) the line exactly at the start(end) point</td></tr>
	<tr><td>CAIRO_LINE_CAP_ROUND</td><td>use a round ending, the center of the circle is the end point</td></tr>
	<tr><td>CAIRO_LINE_CAP_SQUARE</td><td>use squared ending, the center of the square is the end point</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>cap</b> : a line cap style, specified as above</li>
	</ul>
	</p>
	End Rem
	Method SetLineCap(cap:Int)
		NoContextError()
		cairo_set_line_cap(contextPtr, cap)
	End Method
	
	Rem
	bbdoc: Sets the current line join style within the cairo context.
	about: As with the other stroke parameters, the current line join style is examined by
	#Stroke, #StrokeExtents, and #StrokeToPath, but does not have any effect during path construction.
	<p>
	The following details how the available line join styles are drawn :
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_LINE_JOIN_MITER</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_LINE_JOIN_ROUND</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_LINE_JOIN_BEVEL</td><td>&nbsp;</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>join</b> : a line join style, specified as above</li>
	</ul>
	</p>
	End Rem
	Method SetLineJoin(join:Int)
		NoContextError()
		cairo_set_line_join(contextPtr, join)
	End Method
	
	Rem
	bbdoc: Sets the current line width within the cairo context.
	about: The line width specifies the diameter of a pen that is circular in user-space.<br>
	As with the other stroke parameters, the current line cap style is examined by #Stroke,
	#StrokeExtents, and #StrokeToPath(), but does not have any effect during path construction.
	<p>Parameters:
	<ul>
	<li><b>width</b> : a line width, as a user-space value</li>
	</ul>
	</p>
	End Rem
	Method SetLineWidth(width:Double)
		NoContextError()
		cairo_set_line_width(contextPtr, width)
	End Method
	
	Rem
	bbdoc: Modifies the current transformation matrix by setting it equal to matrix.
	about:
	<p>Parameters:
	<ul>
	<li><b>matrix</b> : a transformation matrix from user space to device space</li>
	</ul>
	</p>	
	End Rem
	Method SetMatrix(matrix:TCairoMatrix)
		NoContextError()
		cairo_set_matrix(contextPtr, matrix.matrix)
	End Method

	Method GetMatrix:TCairoMatrix()
		Local r:TCairoMatrixStruct = New TCairoMatrixStruct
		cairo_get_matrix(contextPtr, r)
		Return TCairoMatrix._create(r)
	End Method


	Rem
	bbdoc: Sets the current miter limit within the cairo context.
	about: Parameters:
	<ul>
	<li><b>limit</b> : the limit to set</li>
	</ul>
	End Rem
	Method SetMiterLimit(limit:Double)
		NoContextError()
		cairo_set_miter_limit(contextPtr, limit)
	End Method
	
	Rem
	bbdoc: Sets the compositing operator to be used for all drawing operations.
	about: 
	<p>
	The following details the semantics of each available compositing operator :
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_OPERATOR_CLEAR</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_SOURCE</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_OVER</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_IN</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_OUT</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_ATOP</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_DEST</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_DEST_OVER</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_DEST_IN</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_DEST_OUT</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_DEST_ATOP</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_XOR</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_ADD</td><td>&nbsp;</td></tr>
	<tr><td>CAIRO_OPERATOR_SATURATE</td><td>&nbsp;</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>operator</b> : a compositing operator, specified as above</li>
	</ul>
	</p>
	End Rem
	Method SetOperator(operator:Int)
		NoContextError()
		cairo_set_operator(contextPtr, operator)
	End Method
	
	Rem
	bbdoc: Replaces the current font face, font matrix, and font options in the context with those of the TCairoScaledFont object.
	about: Except for some translation, the current CTM of the context should be the same as that of the
	TCairoScaledFont, which can be accessed using the scaled font GetCTM() method.
	End Rem
	Method SetScaledFont(scaledFont:TCairoScaledFont)
		NoContextError()
		cairo_set_scaled_font(contextPtr, scaledFont.scaledfontPtr)
	End Method

	Rem
	bbdoc: Sets the source pattern to <b>source</b>.
	about: This pattern will then be used for any subsequent drawing operation until a new source pattern is set.
	Note: The pattern's transformation matrix will be locked to the user space in effect at the time of
	#SetSource. This means that further modifications of the current transformation matrix will not affect
	the source pattern.
	<p>Parameters:
	<ul>
	<li><b>source</b> : a TCairoPattern to be used as the source for subsequent drawing operations.</li>
	</ul>
	</p>	
	End Rem
	Method SetSource(pattern:TCairoPattern)
		NoContextError()
		cairo_set_source(contextPtr, pattern.patternPtr)
	End Method
	
	Rem
	bbdoc: Sets the source pattern to an opaque color.
	about: This opaque color will then be used for any subsequent drawing operation until a
	new source pattern is set.<br>
	The color components are floating point numbers in the range 0 to 1. If the values passed in are outside
	that range, they will be clamped.
	<p>Parameters:
	<ul>
	<li><b>r</b> : red component of color</li>
	<li><b>g</b> : blue component of color</li>
	<li><b>b</b> : green component of color</li>
	</ul>
	</p>	
	End Rem
	Method SetSourceRGB(r:Double, g:Double, b:Double)
		NoContextError()
		cairo_set_source_rgb(contextPtr, r,g,b)
	End Method
	
	Rem
	bbdoc: Sets the source pattern to a translucent color.
	about: This color will then be used for any subsequent drawing operation until
	a new source pattern is set.<br>
	The color and alpha components are floating point numbers in the range 0 to 1. If the values passed
	in are outside that range, they will be clamped.
	<p>Parameters:
	<ul>
	<li><b>r</b> : red component of color</li>
	<li><b>g</b> : blue component of color</li>
	<li><b>b</b> : green component of color</li>
	<li><b>a</b> : alpha component of color</li>
	</ul>
	</p>	
	End Rem
	Method SetSourceRGBA(r:Double, g:Double, b:Double, a:Double)
		NoContextError()
		cairo_set_source_rgba(contextPtr, r,g,b,a)
	End Method

	Rem
	bbdoc: This is a convenience function for creating a pattern from surface and setting it as the source with #SetSource
	about: The <b>x</b> and <b>y</b> parameters give the user-space coordinate at which the surface origin should appear.
	(The surface origin is its upper-left corner before any transformation has been applied.)
	The <b>x</b> and <b>y</b> patterns are negated and then set as translation values in the pattern matrix.
	<p>Other than the initial translation pattern matrix, as described above, all other pattern attributes,
	(such as its extend mode), are set to the default values as in TCairoPattern.CreateForSurface().
	The resulting pattern can be queried with #GetSource so that these attributes can be modified
	if desired, (eg. to create a repeating pattern with TCairoPattern.SetExtend()).</p>
	<p>Parameters:
	<ul>
	<li><b>surface</b> : a surface to be used to set the source pattern</li>
	<li><b>x</b> : User-space X coordinate for surface origin</li>
	<li><b>y</b> : User-space Y coordinate for surface origin</li>
	</ul>
	</p>	
	End Rem
	Method SetSourceSurface(surface:TCairoSurface, x:Double,y:Double)
		NoContextError()
		cairo_set_source_surface(contextPtr, surface.surfacePtr, x,y)
	End Method
	
	Rem
	bbdoc: Sets the tolerance used when converting paths into trapezoids.
	about: Curved segments of the path will be subdivided until the maximum deviation between the
	original path and the polygonal approximation is less than <b>tolerance</b>. The default value is 0.1.
	A larger value will give better performance, a smaller value, better appearance. (Reducing the value
	from the default value of 0.1 is unlikely to improve appearance significantly.)
	<p>Parameters:
	<ul>
	<li><b>tolerance</b> : the tolerance, in device units (typically pixels)</li>
	</ul>
	</p>	
	End Rem
	Method SetTolerance(tolerance:Double)
		NoContextError()
		cairo_set_tolerance(contextPtr, tolerance)
	End Method
	
'	Method ShowGlyphs

	Rem
	bbdoc: Completes any required drawing operations for a Page.
	about: Not all surfaces support this, therefore nothing may actually happen.
	End Rem
	Method ShowPage()
		NoContextError()
		cairo_show_page(contextPtr)
	End Method
	
	Rem
	bbdoc: A drawing operator that generates the shape from a string of characters, rendered according to the current font face, font size (font matrix), and font options.
	about: This method first computes a set of glyphs for the string of text. The first glyph is placed so that
	its origin is at the current point. The origin of each subsequent glyph is offset from that of the previous glyph
	by the advance values of the previous glyph.<br>
	After this call the current point is moved to the origin of where the next glyph would be placed in this same
	progression. That is, the current point will be at the origin of the final glyph offset by its advance values.
	This allows for easy display of a single logical string with multiple calls to #ShowText.
	<p>Parameters:
	<ul>
	<li><b>text</b> : a string of text</li>
	</ul>
	</p>
	End Rem
	Method ShowText(text:String)
		NoContextError()
		cairo_show_text(contextPtr, _cairo_ISO8859toUTF8(text))
		Stroke()   ' added for 1.11 - a bug with PDF surface?
	End Method	
	
	Rem
	bbdoc: Checks whether an error has previously occurred for this context.
	returns: the current status of this context.
	about:Possible values returned are :<br>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_STATUS_SUCCESS</td><td>no error has occurred</td></tr>
	<tr><td>CAIRO_STATUS_NO_MEMORY</td><td>out of memory</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_RESTORE</td><td>#Restore without matching #Save</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_POP_GROUP</td><td>no saved group to pop</td></tr>
	<tr><td>CAIRO_STATUS_NO_CURRENT_POINT</td><td>no current point defined</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_MATRIX</td><td>invalid matrix (not invertible)</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_STATUS</td><td>invalid value for an input status</td></tr>
	<tr><td>CAIRO_STATUS_NULL_POINTER</td><td>NULL pointer</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_STRING</td><td>input string not valid UTF-8</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_PATH_DATA</td><td>input path data not valid</td></tr>
	<tr><td>CAIRO_STATUS_READ_ERROR</td><td>error while reading from input stream</td></tr>
	<tr><td>CAIRO_STATUS_WRITE_ERROR</td><td>error while writing to output stream</td></tr>
	<tr><td>CAIRO_STATUS_SURFACE_FINISHED</td><td>target surface has been finished</td></tr>
	<tr><td>CAIRO_STATUS_SURFACE_TYPE_MISMATCH</td><td>the surface type is not appropriate for the operation</td></tr>
	<tr><td>CAIRO_STATUS_PATTERN_TYPE_MISMATCH</td><td>the pattern type is not appropriate for the operation</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_CONTENT</td><td>invalid value for an input content</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_FORMAT</td><td>invalid value for an input format</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_VISUAL</td><td>invalid value for an input Visual</td></tr>
	<tr><td>CAIRO_STATUS_FILE_NOT_FOUND</td><td>file not found</td></tr>
	<tr><td>CAIRO_STATUS_INVALID_DASH</td><td>invalid value for a dash setting</td></tr>
	</table>
	End Rem
	Method Status:Int()
		NoContextError()
		Return cairo_status(contextPtr)
	End Method

	Rem
	bbdoc: Provides a human-readable description of the status value.
	returns: A string representation of the status.
	about: See #Status for a list of valid status values to pass into this method.
	<p>
	Parameters:
	<ul>
	<li><b>s</b> : a cairo status.</li>
	</ul>
	</p>
	End Rem
	Function StatusToString:String(s:Int)
		Return String.FromCString( cairo_status_to_string(s) )
	End Function
	
	Rem
	bbdoc: A drawing operator that strokes the current path according to the current line width, line join, line cap, and dash settings.
	about: After #Stroke, the current path will be cleared from the cairo context.
	See #SetLineWidth, #SetLineJoin, #SetLineCap, #SetDash, and #StrokePreserve.<br>
	Note: Degenerate segments and sub-paths are treated specially and provide a useful result.
	These can result in two different situations:<br>
	<ol>
	<li>Zero-length &quot;on&quot; segments set in #SetDash. If the cap style is CAIRO_LINE_CAP_ROUND or
	CAIRO_LINE_CAP_SQUARE then these segments will be drawn as circular dots or squares respectively.
	In the case of CAIRO_LINE_CAP_SQUARE, the orientation of the squares is determined by the direction
	of the underlying path.</li>
	<li>A sub-path created by #MoveTo followed by either a #ClosePath or one or more
	calls to #LineTo to the same coordinate as the #MoveTo. If the cap style is
	CAIRO_LINE_CAP_ROUND then these sub-paths will be drawn as circular dots. Note that in the
	case of CAIRO_LINE_CAP_SQUARE a degenerate sub-path will not be drawn at all, (since the correct
	orientation is indeterminate).</li>
	</ol>
	In no case will a cap style of CAIRO_LINE_CAP_BUTT cause anything to be drawn in the case of
	either degenerate segments or sub-paths.
	End Rem
	Method Stroke()
		NoContextError()
		cairo_stroke(contextPtr)
	End Method
	
	Rem
	bbdoc: Get the extents for the current stroke.
	End Rem
	Method StrokeExtents(x1:Double Var,y1:Double Var,  x2:Double Var,y2:Double Var)
		NoContextError()
		cairo_stroke_extents(contextPtr, x1,y1, x2,y2)
	End Method

	Rem
	bbdoc: A drawing operator that strokes the current path according to the current line width, line join, line cap, and dash settings.
	about: Unlike #Stroke, #StrokePreserve preserves the path within the cairo context.
	End Rem
	Method StrokePreserve()
		NoContextError()
		cairo_stroke_preserve(contextPtr)
	End Method

	Rem
	bbdoc: Gets the extents for a string of text.
	returns: A TCairoTextExtents object into which the results will be stored.
	about: The extents describe a user-space rectangle that encloses the "inked" portion of the text,
	(as it would be drawn by #ShowText). Additionally, the x_advance and y_advance values indicate
	the amount by which the current point would be advanced by #ShowText.<br>
	Note that whitespace characters do not directly contribute to the size of the rectangle
	(extents.width and extents.height). They do contribute indirectly by changing the position of non-whitespace
	characters. In particular, trailing whitespace characters are likely to not affect the size of the rectangle,
	though they will affect the x_advance and y_advance values.
	<p>Parameters:
	<ul>
	<li><b>text</b> : a string of text.</li>
	</ul>
	</p>
	End Rem
	Method TextExtents:TCairoTextExtents(text:String)
		Local extents:TCairoTextExtents = New TCairoTextExtents
		cairo_text_extents(contextPtr, _cairo_ISO8859toUTF8(text), extents)
		Return extents
	End Method

	Rem
	bbdoc: A drawing operator that generates the <i>outline</i> from a string of characters, rendered according to the current font face, font size (font matrix), and font options.
	about: See #ShowText for more information.
	<p>Parameters:
	<ul>
	<li><b>text</b> : a string of text.</li>
	</ul>
	</p>
	End Rem
	Method TextPath(text:String)
		NoContextError()
		cairo_text_path(contextPtr, _cairo_ISO8859toUTF8(text))
	End Method
	
	Rem
	bbdoc: Modifies the current transformation matrix by applying <b>matrix</b> as an additional transformation.
	about: The new transformation of user space takes place after any existing transformation.
	<p>Parameters:
	<ul>
	<li><b>matrix</b> : a transformation to be applied to the user-space axes</li>
	</ul>
	</p>
	End Rem
	Method Transform(matrix:TCairoMatrix)
		NoContextError()
		cairo_transform(contextPtr, matrix.matrix)
	End Method

	Rem
	bbdoc: Modifies the current transformation matrix.
	about: By translating the user-space origin by <b>(tx, ty)</b>. This offset is interpreted as a user-space
	coordinate according to the current transformation matrix in place before the new call to Translate.
	In other words, the translation of the user-space origin takes place <i>after</i> any existing transformation.
	<p>Parameters:
	<ul>
	<li><b>tx</b> : amount to translate in the X direction</li>
	<li><b>ty</b> : amount to translate in the Y direction</li>
	</ul>
	</p>
	End Rem
	Method Translate(tx:Double, ty:Double)
		NoContextError()
		cairo_translate(contextPtr, tx,ty)
	End Method
	
	Rem
	bbdoc: Transform a coordinate from user space to device space by multiplying the given point by the current transformation matrix.
	about:Parameters:
	<ul>
	<li><b>x</b> : X value of coordinate <i>(in/out parameter)</i></li>
	<li><b>y</b> : Y value of coordinate <i>(in/out parameter)</i></li>
	</ul>
	End Rem
	Method UserToDevice(x:Double Var, y:Double Var)
		NoContextError()
		cairo_user_to_device(contextPtr, x,y)
	End Method
	
	Rem
	bbdoc: Transform a distance vector from user space to device space.
	about: This function is similar to #UserToDevice except that the translation components of the
	current transformation matrix will be ignored when transforming (dx,dy).
	<p>Parameters:
	<ul>
	<li><b>dx</b> : X component of a distance vector <i>(in/out parameter)</i></li>
	<li><b>dy</b> : Y component of a distance vector <i>(in/out parameter)</i></li>
	</ul>
	</p>	
	End Rem
	Method UserToDeviceDistance(dx:Double Var,dy:Double Var)
		NoContextError()
		cairo_user_to_device_distance(contextPtr, dx,dy)
	End Method

	Rem
	bbdoc: Returns the version of the cairo library encoded in a single integer.
	returns: The encoded version.
	End Rem
	Function Version:Int()
		Return cairo_version()
	End Function
	
	Rem
	bbdoc: Returns the version of the cairo library as a human-readable string of the form "X.Y.Z".
	returns: A string containing the version.
	End Rem
	Function VersionString:String()
		Return String.fromCString( cairo_version_string() )
	End Function

End Type

Rem
bbdoc: A TCairoSurface represents an image either as the destination of a drawing operation or as source when drawing onto another surface.
about: You should use one of the following sub-types which define the different drawing backends :
<ul>
<li> #TCairoImageSurface - draws to an in-memory image.</li>
<li> #TCairoPDFSurface - creates a PDF representation of the image(s).</li>
<li> #TCairoPSSurface - creates a Postscript representation of the image(s).</li>
</ul>
End Rem
Type TCairoSurface

	Field surfacePtr:Byte Ptr

	Function _create:TCairoSurface(surfacePtr:Byte Ptr)
		If surfacePtr <> Null Then
			Local surface:TCairoSurface = New TCairoSurface
			surface.surfacePtr = surfacePtr
			Return surface
		End If
		
		Return Null
	End Function

	Rem
	bbdoc: Create a new surface that is as compatible as possible with an existing surface.
	returns: A new TCairoSurface
	about: The new surface will use the same backend as other unless that is not possible for some reason.
	<p>
	Surface content is defined as one of the following options :
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_CONTENT_COLOR</td><td>The surface will hold color content only</td></tr>
	<tr><td>CAIRO_CONTENT_ALPHA</td><td>The surface will hold alpha content only</td></tr>
	<tr><td>CAIRO_CONTENT_COLOR_ALPHA</td><td>The surface will hold color and alpha content</td></tr>
	</table>	
	<p>Parameters:
	<ul>
	<li><b>otherSurface</b> : an existing surface used to select the backend of the new surface</li>
	<li><b>content</b> : the content for the new surface. See above for values.</li>
	<li><b>width</b> : width of the new surface, (in device-space units)</li>
	<li><b>height</b> : height of the new surface (in device-space units)</li>
	</ul>
	</p>
	End Rem
	Function CreateSimilar:TCairoSurface(otherSurface:TCairoSurface, content:Int, width:Int, height:Int)
		Local s:Byte Ptr = cairo_surface_create_similar(othersurface.surfacePtr, content, width, height)
		If s <> Null Then
			Local surface:TCairoSurface = New TCairoSurface
			surface.surfacePtr = s
			Return surface
		End If
		Return Null
	End Function

	Rem
	bbdoc: Decreases the reference count on surface by one.
	about: If the result is zero, then surface and all associated resources are freed. See #Reference.
	End Rem
	Method Destroy()
		cairo_surface_destroy(surfacePtr)
	End Method
	
	Rem
	bbdoc: This function finishes the surface and drops all references to external resources.
	about: After calling #Finish the only valid operations on a surface are getting and setting user data and
	referencing and destroying it. Further drawing to the surface will not affect the surface but will instead trigger
	a CAIRO_STATUS_SURFACE_FINISHED error.<br>
	When the last call to #Destroy decreases the reference count to zero, cairo will call #Finish if it hasn't been
	called already, before freeing the resources associated with the surface.
	End Rem
	Method Finish()
		cairo_surface_finish(surfacePtr)
	End Method
	
	Rem
	bbdoc: Do any pending drawing for the surface and also restore any temporary modification's cairo has made to the surface's state.
	about: This function must be called before switching from drawing on the surface with cairo to drawing on it
	directly with native APIs. If the surface doesn't support direct access, then this function does nothing.
	End Rem
	Method Flush()
		cairo_surface_flush(surfacePtr)
	End Method

'	Method GetFontOptions:TCairoFontOptions

'	Method GetUserData

	Rem
	bbdoc: Returns the content type of surface which indicates whether the surface contains color and/or alpha information.
	returns: The content type.
	about: Content types are :
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_CONTENT_COLOR</td><td>The surface will hold color content only</td></tr>
	<tr><td>CAIRO_CONTENT_ALPHA</td><td>The surface will hold alpha content only</td></tr>
	<tr><td>CAIRO_CONTENT_COLOR_ALPHA</td><td>The surface will hold color and alpha content</td></tr>
	</table>
	End Rem
	Method GetContent:Int()
		Return cairo_surface_get_content(surfacePtr)
	End Method
	
	Rem
	bbdoc: This method returns the previous device offset set by #SetDeviceOffset.
	about: Parameters:
	<ul>
	<li><b>xOffset</b> : the offset in the X direction, in device units. <i>An in/out parameter</i>.</li>
	<li><b>yOffset</b> : the offset in the Y direction, in device units. <i>An in/out parameter</i>.</li>
	</ul>
	End Rem
	Method GetDeviceOffset(xOffset:Double Var, yOffset:Double Var)
		cairo_surface_get_device_offset(surfacePtr, xOffset, yOffset)
	End Method
	
	Rem
	bbdoc: This function returns the type of the backend used to create the surface.
	returns: The type of surface.
	about: The list of possible surfaces are :
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_SURFACE_TYPE_IMAGE</td><td>The surface is of type image</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_PDF</td><td>The surface is of type pdf</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_PS</td><td>The surface is of type ps</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_XLIB</td><td>The surface is of type xlib</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_XCB</td><td>The surface is of type xcb</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_GLITZ</td><td>The surface is of type glitz</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_QUARTZ</td><td>The surface is of type quartz</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_WIN32</td><td>The surface is of type win32</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_BEOS</td><td>The surface is of type beos</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_DIRECTFB</td><td>The surface is of type directfb</td></tr>
	<tr><td>CAIRO_SURFACE_TYPE_SVG</td><td>The surface is of type svg</td></tr>
	</table>
	End Rem
	Method GetType:Int()
		Return cairo_surface_get_type(surfacePtr)
	End Method
	
	Rem
	bbdoc: Tells cairo that drawing has been done to surface using means other than cairo, and that cairo should reread any cached areas.
	about: Note that you must call #Flush before doing such drawing.
	End Rem
	Method MarkDirty()
		cairo_surface_mark_dirty(surfacePtr)
	End Method
	
	Rem
	bbdoc: Like #MarkDirty, but drawing has been done only to the specified rectangle, so that cairo can retain cached contents for other parts of the surface.
	about: Parameters:
	<ul>
	<li><b>x</b> : X coordinate of dirty rectangle</li>
	<li><b>y</b> : Y coordinate of dirty rectangle</li>
	<li><b>width</b> : width of dirty rectangle</li>
	<li><b>height</b> : height of dirty rectangle</li>
	</ul>
	End Rem
	Method MarkDirtyRectangle(X:Int, Y:Int, Width:Int, Height:Int)
		cairo_surface_mark_dirty_rectangle(surfacePtr, X, Y, Width, Height)
	End Method		

	Rem
	bbdoc: Increases the reference count on surface by one.
	about: This prevents surface from being destroyed until a matching call to #Destroy is made.
	End Rem
	Method Reference()
		surfacePtr = cairo_surface_reference(surfacePtr)
	End Method

	Rem
	bbdoc: Sets an offset that is added to the device coordinates determined by the CTM when drawing to surface.
	about: One use case for this function is when we want to create a TCairoSurface that redirects drawing for a
	portion of an onscreen surface to an offscreen surface in a way that is completely invisible to the user
	of the cairo API. Setting a transformation via #Translate isn't sufficient to do this, since functions like
	#DeviceToUser will expose the hidden offset.<br>
	Note that the offset only affects drawing to the surface, not using the surface in a surface pattern.
	<p>Parameters:
	<ul>
	<li><b>xOffset</b> : the offset in the X direction, in device units</li>
	<li><b>yOffset</b> : the offset in the Y direction, in device units</li>
	</ul>
	</p>
	End Rem
	Method SetDeviceOffset(xOffset:Double, yOffset:Double)
		cairo_surface_set_device_offset(surfacePtr, xOffset, yOffset)
	End Method

'	Method SetUserData

	Rem
	bbdoc: Set the horizontal and vertical resolution for image fallbacks.
	about: When certain operations aren't supported natively by a backend, cairo will fallback by rendering
	operations to an image and then overlaying that image onto the output. For backends that are natively
	vector-oriented, this method can be used to set the resolution used for these image fallbacks,
	(larger values will result in more detailed images, but also larger file sizes).
	<p>Some examples of natively vector-oriented backends are the ps, pdf, and svg backends.</p>
	<p>For backends that are natively raster-oriented, image fallbacks are still possible, but they are
	always performed at the native device resolution. So this function has no effect on those backends.</p>
	<p>NOTE: The fallback resolution only takes effect at the time of completing a page
	(with #ShowPage or #CopyPage ) so there is currently no way to have more than one
	fallback resolution in effect on a single page.</p>
	<p>Parameters:
	<ul>
	<li><b>xPixelsPerInch</b> : horizontal setting for pixels per inch</li>
	<li><b>yPixelsPerInch</b> : vertical setting for pixels per inch</li>
	</ul>
	</p>
	End Rem
	Method SetFallbackResolution(xPixelsPerInch:Double, yPixelsPerInch:Double)
		cairo_surface_set_fallback_resolution(surfacePtr, xPixelsPerInch, yPixelsPerInch)
	End Method

	Rem
	bbdoc: Checks whether an error has previously occurred for this surface.
	returns: CAIRO_STATUS_SUCCESS, CAIRO_STATUS_NULL_POINTER, CAIRO_STATUS_NO_MEMORY, CAIRO_STATUS_READ_ERROR, CAIRO_STATUS_INVALID_CONTENT, CAIRO_STATUS_INVALUE_FORMAT, or CAIRO_STATUS_INVALID_VISUAL.
	End Rem
	Method Status:Int()
		Return cairo_surface_status(surfacePtr)
	End Method
	
	Rem
	bbdoc: Writes the contents of the surface to a new file @filename as a PNG image.
	returns: CAIRO_STATUS_SUCCESS if the PNG file was written successfully. Otherwise, CAIRO_STATUS_NO_MEMORY if memory could not be allocated for the operation or 'CAIRO_STATUS_SURFACE_TYPE_MISMATCH if the surface does not have pixel contents, or CAIRO_STATUS_WRITE_ERROR if an I/O error occurs while attempting to write the file.
	about: Parameters:
	<ul>
	<li><b>filename</b> : the name of a file to write to</li>
	</ul>
	End Rem
	Method WriteToPNG:Int(filename:String)
		Local cStr:Byte Ptr = filename.toCString()
		Local ret:Int = cairo_surface_write_to_png(surfacePtr, cStr)
		MemFree cStr
		Return ret
	End Method

'	Method WriteToPNGStream

End Type

Rem
bbdoc: A TCairoSurface for rendering PDF documents.
End Rem
Type TCairoPDFSurface Extends TCairoSurface

	Rem
	bbdoc: Creates an PDF surface for the specified filename (which will be created) and dimensions.
	returns: The newly created surface.
	about: Parameters:
	<ul>
	<li><b>filename</b> : the name of the PDF file to create.</li>
	<li><b>width</b> : width of the surface, in points. (1 point == 1/72.0 inch)</li>
	<li><b>height</b> : height of the surface, in points. (1 point == 1/72.0 inch)</li>
	</ul>
	End Rem
	Function CreateForPDF:TCairoPDFSurface(filename:String, width:Double, height:Double)
		Local surf:TCairoPDFSurface = New TCairoPDFSurface
		Local cStr:Byte Ptr = filename.toCString()
		surf.surfacePtr = cairo_pdf_surface_create(cStr, width, height)
		MemFree cStr
		Return surf
	End Function
	
	Rem
	bbdoc: Changes the size of a PDF surface for the current (and subsequent) pages.
	about: This method should only be called before any drawing operations have been performed on the
	current page. The simplest way to do this is to call this function immediately after creating the
	surface or immediately after completing a page with either #ShowPage or #CopyPage.
	<p>Parameters:
	<ul>
	<li><b>width</b> : new surface width, in points (1 point == 1/72.0 inch)</li>
	<li><b>height</b> : new surface height, in points (1 point == 1/72.0 inch)</li>
	</ul>
	</p>
	End Rem
	Method SetSize(width:Double, height:Double)
		cairo_pdf_surface_set_size(surfacePtr, width, height)
	End Method
End Type

Rem
bbdoc: A TCairoSurface for rendering Postscript documents.
End Rem
Type TCairoPSSurface Extends TCairoSurface

	Rem
	bbdoc: Creates an Postscript surface for the specified filename (which will be created) and dimensions.
	returns: The newly created surface.
	about: Parameters:
	<ul>
	<li><b>filename</b> : the name of the postscript file to create.</li>
	<li><b>width</b> : width of the surface, in pixels.</li>
	<li><b>height</b> : height of the surface, in pixels.</li>
	</ul>
	End Rem
	Function CreateForPS:TCairoPSSurface(filename:String, width:Double, height:Double)
		Local surf:TCairoPSSurface = New TCairoPSSurface
		Local cStr:Byte Ptr = filename.toCString()
		surf.surfacePtr = cairo_ps_surface_create(cStr, width, height)
		MemFree cStr
		Return surf
	End Function
	
	Rem
	bbdoc: Changes the size of a PostScript surface for the current (and subsequent) pages.
	about: This function should only be called before any drawing operations have been performed on
	the current page. The simplest way to do this is to call this function immediately after creating
	the surface or immediately after completing a page with either #ShowPage or #CopyPage.
	<p>Parameters:
	<ul>
	<li><b>width</b> : new surface width, in points (1 point == 1/72.0 inch)</li>
	<li><b>height</b> : new surface height, in points (1 point == 1/72.0 inch)</li>
	</ul>
	</p>
	End Rem
	Method SetSize(width:Double, height:Double)
		cairo_ps_surface_set_size(surfacePtr, width, height)
	End Method
End Type

Rem
bbdoc: Image surfaces provide the ability to render to memory buffers either allocated by cairo or by the calling code
End Rem
Type TCairoImageSurface Extends TCairoSurface

	Field pix:TPixmap

	Rem
	bbdoc: Creates an image surface of the specified format and dimensions.
	returns: The newly created surface. The caller owns the surface and should call #Destroy when done with it. This function always returns a valid surface, but it will return a "nil" surface if an error such as out of memory occurs. You can use #Status to check for this.
	about: The initial contents of the surface is undefined; you must explicitely clear the buffer, using,
	for example, #Rectangle and #Fill if you want it cleared.
	<p>The list of available formats are as follows:
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_FORMAT_ARGB32</td><td>each pixel is a 32-bit quantity, with alpha in the upper 8 bits, then red,
		then green, then blue. The 32-bit quantities are stored native-endian. Pre-multiplied alpha is used.
		(That is, 50% transparent red is 0x80800000, not 0x80ff0000.)</td></tr>
	<tr><td>CAIRO_FORMAT_RGB24</td><td>each pixel is a 32-bit quantity, with the upper 8 bits unused. Red, Green,
		and Blue are stored in the remaining 24 bits in that order.</td></tr>
	<tr><td>CAIRO_FORMAT_A8</td><td>each pixel is a 8-bit quantity holding an alpha value.</td></tr>
	<tr><td>CAIRO_FORMAT_A1</td><td>each pixel is a 1-bit quantity holding an alpha value. Pixels are packed together
		into 32-bit quantities. The ordering of the bits matches the endianess of the platform. On a big-endian machine,
		the first pixel is in the uppermost bit, on a little-endian machine the first pixel is in the least-significant
		bit.</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>width</b> : width of the surface, in pixels.</li>
	<li><b>height</b> : height of the surface, in pixels.</li>
	<li><b>format</b> : format of pixels in the surface to create. See above for possible formats.</li>
	</ul>
	</p>
	End Rem
	Function Create:TCairoImageSurface(width:Int, height:Int, format:Int)
		Local ImgSurf:TCairoImageSurface = New TCairoImageSurface
		ImgSurf.surfacePtr = cairo_image_surface_create(format, width, height)
		Return ImgSurf
	End Function

	Rem
	bbdoc: Creates an image surface from the specified png file.
	returns: The newly created surface.
	about: Parameters:
	<ul>
	<li><b>pngfilename</b> : the name of the PNG file.</li>
	</ul>
	End Rem
	Function CreateFromPNG:TCairoImageSurface(pngfilename:String)
		Local ImgSurf:TCairoImageSurface = New TCairoImageSurface
		Local cStr:Byte Ptr = pngfilename.toCString()
		ImgSurf.surfacePtr = cairo_image_surface_create_from_png(cStr)
		MemFree cStr
		Return ImgSurf
	End Function

	Rem
	bbdoc: Creates an image surface from a new pixmap of the specified format and dimensions.
	returns: The newly created surface.
	about: <b>Note</b> - The parameters @format and @align are deprecated. They are no longer used.<br>
	The Pixmap format is now hard-coded to PF_BGRA8888 on LittleEndian (x86) platforms, and PF_ARGB8888 on
	BigEndian (PPC) platforms.
	End Rem
	Function CreateForPixmap:TCairoImageSurface(width:Int, height:Int, format:Int=PF_BGRA8888, align:Int=4)
		?littleendian
		format = PF_BGRA8888
		?bigendian
		format = PF_ARGB8888
		?
		Local p:TPixmap = TPixmap.Create(width, height, format, 4)
		Return CreateFromPixmap(p)
	End Function
	
	Rem
	bbdoc: Creates an image surface from an existing TImage object.
	returns: The newly created surface.
	about: The image should be defined as a DYNAMICIMAGE.<br>
	Since BlitzMax internally caches the image data, the image should be locked (see #LockImage )
	before performing any cairo manipulation on the surface.<br>
	#CreateFromImage performs an image lock during creation of the cairo surface, which will allow you to
	manipulate the image in cairo before the first DrawImage. After that, #LockImage is required.<br>
	See arc_image and clock_image examples.
	<p>
	<b>Note</b> - The image pixmap is set to the correct format before use. If the image doesn't
	have an internal pixmap, one is created for it.<br>
	On LittleEndian (x86), the format is PF_BGRA8888.<br>
	On BigEndian (PPC), the format is PF_ARGB8888.
	</p>
	End Rem
	Function CreateFromImage:TCairoImageSurface(image:TImage)
		If image.pixmaps[0] = Null Then
			?littleendian
			image.pixmaps[0] = CreatePixmap( image.width, image.height,PF_BGRA8888 )
			?bigendian
			image.pixmaps[0] = CreatePixmap( image.width, image.height, PF_ARGB8888 )
			?			
		Else
			?littleendian
			If PixmapFormat(image.pixmaps[0]) <> PF_BGRA8888 Then
				image.pixmaps[0] = ConvertPixmap(image.pixmaps[0], PF_BGRA8888)
			End If
			?bigendian
			If PixmapFormat(image.pixmaps[0]) <> PF_ARGB8888 Then
				image.pixmaps[0] = ConvertPixmap(image.pixmaps[0], PF_ARGB8888)
			End If
			?
		End If
		
		Local p:TPixmap = image.lock(0, True, True)
		Return CreateFromPixmap(p)
	End Function

	Rem
	bbdoc: Creates an image surface from the provided pixmap.
	returns: The newly created surface.
	about: Parameters:
	<ul>
	<li><b>pixmap</b> : an existing pixmap object.</li>
	</ul>
	<p><b>Note</b> - If the pixmap is not of the format PF_BGRA8888 (x86) or PF_ARGB8888 (PPC),
	it is converted automatically.</p>
	End Rem
	Function CreateFromPixmap:TCairoImageSurface(_pixmap:TPixmap)
		Local cairoFormat:Int
		Local ImgSurf:TCairoImageSurface = New TCairoImageSurface
		
		' force to correct pixmap type for endian-ness
		?littleendian
		If PixmapFormat(_pixmap) <> PF_BGRA8888 Then
			_pixmap = ConvertPixmap(_pixmap, PF_BGRA8888)
		End If
		?bigendian
		If PixmapFormat(_pixmap) <> PF_ARGB8888 Then
			_pixmap = ConvertPixmap(_pixmap, PF_ARGB8888)
		End If
		?
		CairoFormat = CAIRO_FORMAT_ARGB32
		
		ImgSurf.surfacePtr = cairo_image_surface_create_for_data( PixmapPixelPtr(_pixmap), cairoFormat, ..
														PixmapWidth(_pixmap), PixmapHeight(_pixmap), ..
														PixmapPitch(_pixmap) )
		ImgSurf.pix = _pixmap
		Return ImgSurf
	End Function
	
	Rem
	bbdoc: Returns the pixmap instance, if one exists.
	End Rem
	Method Pixmap:TPixmap()
		Return pix
	End Method

'	Function cairo_image_surface_create_from_png_stream:Byte Ptr(read_func:Byte Ptr, ..

	Rem
	bbdoc: Get a pointer to the data of the image surface, for direct inspection or modification.
	returns: A pointer to the image data of this surface or Null if surface is not an image surface.
	End Rem
	Method GetData:Byte Ptr()
		Return cairo_image_surface_get_data(surfacePtr)
	End Method

	Rem
	bbdoc: Get the format of the surface.
	returns: The format of the surface.
	about: See #TCairoImageSurface for list of formats.
	End Rem
	Method getFormat:Int()
		Return cairo_image_surface_get_format(surfacePtr)
	End Method
	
	Rem
	bbdoc: Get the height of the image surface in pixels.
	returns: The height of the surface in pixels.
	End Rem
	Method GetHeight:Int()
		Return cairo_image_surface_get_height(surfacePtr)
	End Method

	Rem
	bbdoc: Get the stride of the image surface in bytes.
	returns: The stride of the image surface in bytes (or 0 if surface is not an image surface).
	about: The stride is the distance in bytes from the beginning of one row of the image data to
	the beginning of the next row.
	End Rem
	Method GetStride:Int()
		Return cairo_image_surface_get_stride(surfacePtr)
	End Method
	
	Rem
	bbdoc: Get the width of the image surface in pixels.
	returns: The width of the surface in pixels.
	End Rem
	Method GetWidth:Int()
		Return cairo_image_surface_get_width(surfacePtr)
	End Method
	
End Type

Rem
bbdoc: Gradients and filtered sources.
End Rem
Type TCairoPattern

	Const degToRad:Double = 0.017453292519943295

	Field patternPtr:Byte Ptr
	
	Function _create:TCairoPattern(patternPtr:Byte Ptr)
		If patternPtr <> Null Then
			Local pattern:TCairoPattern = New TCairoPattern
			pattern.patternPtr = patternPtr
			Return pattern
		End If
		
		Return Null
	End Function

	Rem
	bbdoc: Create a new TCairoPattern for the given surface.
	returns: A new TCairoPattern object
	End Rem
	Function CreateForSurface:TCairoPattern(surface:TCairoSurface)
		Local pattern:TCairoPattern = New TCairoPattern
		pattern.patternPtr = cairo_pattern_create_for_surface(surface.surfacePtr)
		Return pattern
	End Function
	
	Rem
	bbdoc: Create a new linear gradient cairo_pattern_t along the line defined by (x0, y0) and (x1, y1).
	returns: The newly created TCairoPattern if succesful, or an error pattern in case of no memory. The caller owns the returned object and should call #Destroy when finished with it. This function will always return a valid object, but if an error occurred the pattern status will be set to an error. To inspect the status of a pattern use #Status.
	about: Before using the gradient pattern, a number of color stops should be defined using
	#AddColorStopRGB or #AddColorStopRGBA.<br>
	Note: The coordinates here are in pattern space. For a new pattern, pattern space is identical to user space,
	but the relationship between the spaces can be changed with #SetMatrix.
	<p>Parameters:
	<ul>
	<li><b>x0</b> : x coordinate of the start point.</li>
	<li><b>y0</b> : y coordinate of the start point.</li>
	<li><b>x1</b> : x coordinate of the end point.</li>
	<li><b>y1</b> : y coordinate of the end point.</li>
	</ul>
	</p>
	End Rem
	Function CreateLinear:TCairoPattern(x0:Double, y0:Double, x1:Double, y1:Double)
		Local pattern:TCairoPattern = New TCairoPattern
		pattern.patternPtr = cairo_pattern_create_linear(x0,y0,x1,y1)
		Return pattern
	End Function
	
	Rem
	bbdoc: Creates a new radial gradient cairo_pattern_t between the two circles defined by (x0, y0, c0) and (x1, y1, c0).
	returns: The newly created TCairoPattern if succesful, or an error pattern in case of no memory. The caller owns the returned object and should call #Destroy when finished with it. This function will always return a valid object, but if an error occurred the pattern status will be set to an error. To inspect the status of a pattern use #Status.
	about: Before using the gradient pattern, a number of color stops should be defined using
	#AddColorStopRGB or #AddColorStopRGBA.<br>
	Note: The coordinates here are in pattern space. For a new pattern, pattern space is identical to user space,
	but the relationship between the spaces can be changed with #SetMatrix.
	<p>Parameters:
	<ul>
	<li><b>cx0</b> : x coordinate for the center of the start circle.</li>
	<li><b>cy0</b> : y coordinate for the center of the start circle.</li>
	<li><b>radius0</b> : radius of the start cirle in degrees.</li>
	<li><b>cx1</b> : x coordinate for the center of the end circle.</li>
	<li><b>cy1</b> : y coordinate for the center of the end circle.</li>
	<li><b>radius1</b> : radius of the end cirle in degrees.</li>
	</ul>
	</p>
	End Rem
	Function CreateRadial:TCairoPattern(cx0:Double,cy0:Double,radius0:Double,  cx1:Double,cy1:Double,radius1:Double)
		Local pattern:TCairoPattern = New TCairoPattern
		pattern.patternPtr = cairo_pattern_create_radial(cx0,cy0,radius0 * degToRad, cx1,cy1,radius1 * degToRad)
		Return pattern
	End Function

	Rem
	bbdoc: Creates a new TCairoPattern corresponding to an opaque color
	returns: The newly created TCairoPattern if succesful, or an error pattern in case of no memory. The caller owns the returned object and should call #Destroy when finished with it. This function will always return a valid object, but if an error occurred the pattern status will be set to an error. To inspect the status of a pattern use #Status.
	about: The color components are floating point numbers in the range 0 to 1. If the values passed in are outside
	that range, they will be clamped.
	<p>Parameters:
	<ul>
	<li><b>red</b> : red component of color.</li>
	<li><b>green</b> : green component of color.</li>
	<li><b>blue</b> : blue component of color.</li>
	</ul>
	</p>
	End Rem
	Function CreateRGB:TCairoPattern(red:Double, green:Double, blue:Double)
		Local pattern:TCairoPattern = New TCairoPattern
		pattern.patternPtr = cairo_pattern_create_rgb(red,green,blue)
		Return pattern
	End Function

	Rem
	bbdoc: Creates a new cairo_pattern_t corresponding to a translucent color.
	returns: The newly created TCairoPattern if succesful, or an error pattern in case of no memory. The caller owns the returned object and should call #Destroy when finished with it. This function will always return a valid object, but if an error occurred the pattern status will be set to an error. To inspect the status of a pattern use #Status.
	about: The color components are floating point numbers in the range 0 to 1. If the values passed in are outside
	that range, they will be clamped.
	<p>Parameters:
	<ul>
	<li><b>red</b> : red component of color.</li>
	<li><b>green</b> : green component of color.</li>
	<li><b>blue</b> : blue component of color.</li>
	<li><b>alpha</b> : alpha component of the color.</li>
	</ul>
	</p>
	End Rem
	Function CreateRGBA:TCairoPattern(red:Double, green:Double, blue:Double, alpha:Double)
		Local pattern:TCairoPattern = New TCairoPattern
		pattern.patternPtr = cairo_pattern_create_rgba(red,green,blue,alpha)
		Return pattern
	End Function

	Rem
	bbdoc: Adds an opaque color stop to a gradient pattern.
	about: The offset specifies the location along the gradient's control vector. For example, a linear gradient's
	control vector is from (x0,y0) to (x1,y1) while a radial gradient's control vector is from any point on the
	start circle to the corresponding point on the end circle.<br>
	The color is specified in the same way as in #SetSourceRGB.<br>
	Note: If the pattern is not a gradient pattern, (eg. a linear or radial pattern), then the pattern will be put
	into an error status with a status of CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
	<p>Parameters:
	<ul>
	<li><b>offset</b> : an offset in the range [0.0 .. 1.0].</li>
	<li><b>red</b> : red component of color.</li>
	<li><b>green</b> : green component of color.</li>
	<li><b>blue</b> : blue component of color.</li>
	</ul>
	</p>
	End Rem
	Method AddColorStopRGB(offset:Double, red:Double, green:Double, blue:Double)
		cairo_pattern_add_color_stop_rgb(patternPtr, offset, red, green, blue)
	End Method
	
	Rem
	bbdoc: Adds a translucent color stop to a gradient pattern.
	about: The offset specifies the location along the gradient's control vector. For example, a linear gradient's
	control vector is from (x0,y0) to (x1,y1) while a radial gradient's control vector is from any point on the
	start circle to the corresponding point on the end circle.<br>
	The color is specified in the same way as in #SetSourceRGBA.<br>
	Note: If the pattern is not a gradient pattern, (eg. a linear or radial pattern), then the pattern will be put
	into an error status with a status of CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
	<p>Parameters:
	<ul>
	<li><b>offset</b> : an offset in the range [0.0 .. 1.0].</li>
	<li><b>red</b> : red component of color.</li>
	<li><b>green</b> : green component of color.</li>
	<li><b>blue</b> : blue component of color.</li>
	<li><b>alpha</b> : alpha component of color.</li>
	</ul>
	</p>
	End Rem
	Method AddColorStopRGBA(offset:Double, red:Double, green:Double, blue:Double, alpha:Double)
		cairo_pattern_add_color_stop_rgba(patternPtr, offset, red,green,blue,alpha)
	End Method

	Rem
	bbdoc: Decreases the reference count on pattern by one.
	about: If the result is zero, then pattern and all associated resources are freed. See #Reference.
	End Rem
	Method Destroy()
		cairo_pattern_destroy(patternPtr)
	End Method
	
	Rem
	bbdoc: Get the current extend mode for the pattern.
	returns: The extend mode. See #SetExtend for the list of possible values.
	End Rem
	Method GetExtend:Int()
		Return cairo_pattern_get_extend(patternPtr)
	End Method

	Rem
	bbdoc: Get the current pattern filter.
	returns: The pattern filter. See #SetFilter for the list of possible values.
	End Rem
	Method GetFilter:Int()
		Return cairo_pattern_get_filter(patternPtr)
	End Method
	
	Rem
	bbdoc: This function returns the type of the pattern.
	returns: The type of pattern.
	about: The type of a pattern is determined by the function used to create it. The #CreateRGB and
	#CreateRGBA functions create SOLID patterns. The remaining pattern create functions map to pattern
	types in obvious ways.
	<p>Most pattern methods can be called with a pattern of any type, (though trying to change the
	extend or filter for a solid pattern will have no effect). A notable exception is
	#AddColorStopRGB and #AddColorStopRGBA which must only be called with gradient patterns
	(either LINEAR or RADIAL). Otherwise the pattern will be shutdown and put into an error state.</p>
	<p>New entries may be added in future versions.</p>
	Current patterns are:
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_PATTERN_TYPE_SOLID</td><td>The pattern is a solid (uniform) color. It may be opaque or translucent</td></tr>
	<tr><td>CAIRO_PATTERN_TYPE_SURFACE</td><td>The pattern is a based on a surface (an image)</td></tr>
	<tr><td>CAIRO_PATTERN_TYPE_LINEAR</td><td>The pattern is a linear gradient</td></tr>
	<tr><td>CAIRO_PATTERN_TYPE_RADIAL</td><td>The pattern is a radial gradient</td></tr>
	</table>
	End Rem
	Method GetType:Int()
		Return cairo_pattern_get_type(patternPtr)
	End Method
	
	Rem
	bbdoc: Increases the reference count on pattern by one.
	about: This prevents pattern from being destroyed until a matching call to #Destroy is made.
	End Rem
	Method Reference()
		patternPtr = cairo_pattern_reference(patternPtr)
	End Method

	Rem
	bbdoc: Sets the pattern extend mode.
	about: The mode determines how the pattern repeats.
	<p>The following is a list of the available extend modes:
	</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_EXTEND_NONE</td><td>Pixels outside of the source pattern are fully transparent</td></tr>
	<tr><td>CAIRO_EXTEND_REPEAT</td><td>The pattern is tiled by repeating</td></tr>
	<tr><td>CAIRO_EXTEND_REFLECT</td><td>The pattern is tiled by reflecting at the edges (not implemented for surface patterns currently)</td></tr>
	<tr><td>CAIRO_EXTEND_PAD</td><td>Pixels outside of the pattern copy the closest pixel from the source (not implemented for surface patterns currently)</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>extend</b> : the extend mode to set. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetExtend(extend:Int)
		cairo_pattern_set_extend(patternPtr, extend)
	End Method
	
	Rem
	bbdoc: Set the pattern filter.
	about: The following is a list of possible filters to apply:
	<table>
	<tr><th>Constant</th></tr>
	<tr><td>CAIRO_FILTER_FAST</td></tr>
	<tr><td>CAIRO_FILTER_GOOD</td></tr>
	<tr><td>CAIRO_FILTER_BEST</td></tr>
	<tr><td>CAIRO_FILTER_NEAREST</td></tr>
	<tr><td>CAIRO_FILTER_BILINEAR</td></tr>
	<tr><td>CAIRO_FILTER_GAUSSIAN</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>filter</b> : the filter to apply. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetFilter(filter:Int)
		cairo_pattern_set_filter(patternPtr, filter)
	End Method
	
	Rem
	bbdoc: Sets the pattern's transformation matrix to @matrix.
	about: This matrix is a transformation from user space to pattern space.<br>
	When a pattern is first created it always has the identity matrix for its transformation matrix,
	which means that pattern space is initially identical to user space.<br>
	Important: Please note that the direction of this transformation matrix is from user space to pattern space.
	This means that if you imagine the flow from a pattern to user space (and on to device space), then coordinates
	in that flow will be transformed by the inverse of the pattern matrix.<br>
	<p>
	For example, if you want to make a pattern appear twice as large as it does by default the correct code to use is:
	</p>
	<pre>
	matrix.InitScale(0.5, 0.5)
	pattern.SetMatrix(matrix)
	</pre>
	<p>
	Meanwhile, using values of 2.0 rather than 0.5 in the code above would cause the pattern to appear at half of
	its default size.<br>
	Also, please note the discussion of the user-space locking semantics of #SetSource.
	</p>
	<p>Parameters:
	<ul>
	<li><b>matrix</b> : a TCairoMatrix.</li>
	</ul>
	</p>
	End Rem
	Method SetMatrix(matrix:TCairoMatrix)
		cairo_pattern_set_matrix(patternPtr, matrix.matrix)
	End Method
	
	Rem
	bbdoc: Checks whether an error has previously occurred for this pattern.
	returns: CAIRO_STATUS_SUCCESS, CAIRO_STATUS_NO_MEMORY, or CAIRO_STATUS_PATTERN_TYPE_MISMATCH.
	End Rem
	Method Status:Int()
		Return cairo_pattern_status(patternPtr)
	End Method
	
End Type

Rem
bbdoc: A TCairoMatrixStruct holds an affine transformation, such as a scale, rotation, shear, or a combination of those.
about: The transformation of a point (x, y) is given by:
<pre>
x_new = xx * x + xy * y + x0
y_new = yx * x + yy * y + y0
</pre>
End Rem
Type TCairoMatrixStruct
	Rem
	bbdoc: xx component of the affine transformation
	End Rem
	Field xx:Double
	Rem
	bbdoc: yx component of the affine transformation
	End Rem
	Field yx:Double
	Rem
	bbdoc: xy component of the affine transformation
	End Rem
	Field xy:Double
	Rem
	bbdoc: yy component of the affine transformation
	End Rem
	Field yy:Double
	Rem
	bbdoc: X translation component of the affine transformation
	End Rem
	Field x0:Double
	Rem
	bbdoc: Y translation component of the affine transformation
	End Rem
	Field y0:Double
End Type

Rem
bbdoc: TCairoMatrix is used throughout cairo to convert between different coordinate spaces.
about: A TCairoMatrix holds an affine transformation, such as a scale, rotation, shear, or a combination of these.
The transformation of a point (x,y) is given by:
<pre>
    x_new = xx * x + xy * y + x0
    y_new = yx * x + yy * y + y0
</pre>
The current transformation matrix of a TCairo, represented as a TCairoMatrix, defines the transformation from user-space
coordinates to device-space coordinates. See #GetMatrix and #SetMatrix.
<p>
You can access the specific values of the transformation through the @matrix field (a #TCairoMatrixStruct) of the TCairoMatrix.
</p>
End Rem
Type TCairoMatrix

	Const degToRad:Double = 0.017453292519943295

	Field matrix:TCairoMatrixStruct

	Function _create:TCairoMatrix(matrix:TCairoMatrixStruct)
		Local this:TCairoMatrix = New TCairoMatrix
		this.matrix = matrix
		Return this
	End Function

	Rem
	bbdoc: Creates a new TCairoMatrix to be the given affine transformation.
	returns: A new matrix object.
	about: See #Init for more details.
	End Rem
	Function Create:TCairoMatrix(xx:Double,yx:Double, xy:Double,yy:Double, x0:Double,y0:Double)
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_init(this.matrix, xx,yx, xy,yy, x0,y0)
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new TCairoMatrix as an identity transformation.
	returns: A new matrix object.
	about: See #InitIdentity for more details.
	End Rem
	Function CreateIdentity:TCairoMatrix()
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_init_identity(this.matrix)
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new TCairoMatrix to be a transformation that rotates by @degreesn.
	returns: A new matrix object.
	about: See #InitRotate for more details.
	End Rem
	Function CreateRotate:TCairoMatrix(degrees:Double)
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_init_rotate(this.matrix, degrees * degToRad)
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new TCairoMatrix to be a transformation that scales by @sx and @sy in the X and Y dimensions, respectively.
	returns: A new matrix object.
	about: See #InitScale for more details.
	End Rem
	Function CreateScale:TCairoMatrix(sx:Double, sy:Double)
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_init_scale(this.matrix, sx,sy)
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new TCairoMatrix to be a transformation that translates by @tx and @ty in the X and Y dimensions, respectively.
	returns: A new matrix object.
	about: See #InitTranslate for more details.
	End Rem
	Function CreateTranslate:TCairoMatrix(tx:Double, ty:Double)
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_init_translate(this.matrix, tx,ty)
		
		Return this
	End Function

	Rem
	bbdoc: Multiplies the affine transformations in @mat1 and @mat2 together and stores the result in result.
	returns: The result of the multiplication of the two matrices.
	about: The effect of the resulting transformation is to first apply the transformation in a to the coordinates
	and then apply the transformation in b to the coordinates.<br>
	It is allowable for result to be identical to either @mat1 or @mat2.
	<p>Parameters:
	<ul>
	<li><b>mat1</b> : a TCairoMatrix.</li>
	<li><b>mat2</b> : a TCairoMatrix.</li>
	</ul>
	</p>
	End Rem
	Function Multiply:TCairoMatrix(mat1:TCairoMatrix, mat2:TCairoMatrix)
		Local this:TCairoMatrix = New TCairoMatrix
		
		this.matrix = New TCairoMatrixStruct
		cairo_matrix_multiply(this.matrix, mat1.matrix, mat2.matrix)
		
		Return this
	End Function

	Rem
	bbdoc: Sets matrix to be the affine transformation given by @xx, @yx, @xy, @yy, @x0, @y0.
	about: The transformation is given by:
	<pre>
	x_new = xx * x + xy * y + x0
	y_new = yx * x + yy * y + y0
	</pre>
	<p>Parameters:
	<ul>
	<li><b>xx</b> : xx component of the affine transformation.</li>
	<li><b>yx</b> : yx component of the affine transformation.</li>
	<li><b>xy</b> : xy component of the affine transformation.</li>
	<li><b>yy</b> : yy component of the affine transformation.</li>
	<li><b>x0</b> : X translation component of the affine transformation.</li>
	<li><b>y0</b> : Y translation component of the affine transformation.</li>
	</ul>
	</p>
	End Rem
	Method Init(xx:Double, yx:Double, xy:Double, yy:Double, x0:Double, y0:Double)
		cairo_matrix_init(matrix, xx,yx, xy,yy, x0,y0)
	End Method
	
	Rem
	bbdoc: Modifies matrix to be an identity transformation.
	End Rem
	Method InitIdentity()
		cairo_matrix_init_identity(matrix)
	End Method
	
	Rem
	bbdoc: Initialized matrix to a transformation that rotates by @degrees.
	about: Parameters:
	<ul>
	<li><b>degrees</b> : angle of rotation, in degrees. The direction of rotation is defined such that positive angles
		rotate in the direction from the positive X axis toward the positive Y axis. With the default axis
		orientation of cairo, positive angles rotate in a clockwise direction.</li>
	</ul>
	End Rem
	Method InitRotate(degrees:Double)
		cairo_matrix_init_rotate(matrix, degrees * degToRad)
	End Method
	
	Rem
	bbdoc: Initializes matrix to a transformation that scales by @sx and @sy in the X and Y dimensions, respectively.
	about: Parameters:
	<ul>
	<li><b>sx</b> : scale factor in the X direction.</li>
	<li><b>sy</b> : scale factor in the Y direction.</li>
	</ul>
	End Rem
	Method InitScale(sx:Double, sy:Double)
		cairo_matrix_init_scale(matrix, sx,sy)
	End Method
	
	Rem
	bbdoc: Initializes matrix to a transformation that translates by @tx and @ty in the X and Y dimensions, respectively.
	about: Parameters:
	<ul>
	<li><b>tx</b> : amount to translate in the X direction.</li>
	<li><b>ty</b> : amount to translate in the Y direction.</li>
	</ul>
	End Rem
	Method InitTranslate(tx:Double, ty:Double)
		cairo_matrix_init_translate(matrix, tx,ty)
	End Method
	
	Rem
	bbdoc: Changes matrix to be the inverse of it's original value.
	returns: If matrix has an inverse, modifies matrix to be the inverse matrix and returns CAIRO_STATUS_SUCCESS. Otherwise, CAIRO_STATUS_INVALID_MATRIX.
	about: Not all transformation matrices have inverses; if the matrix collapses points together (it is degenerate),
	then it has no inverse and this function will fail.
	End Rem
	Method Invert:Int()
		Return cairo_matrix_invert(matrix)
	End Method
	
	Rem
	bbdoc: Applies rotation by @degrees to the transformation in matrix.
	The effect of the new transformation is to first rotate the coordinates by @degrees, then apply the original
	transformation to the coordinates.
	<p>Parameters:
	<ul>
	<li><b>degrees</b> : angle of rotation, in degrees. The direction of rotation is defined such that positive angles
		rotate in the direction from the positive X axis toward the positive Y axis. With the default axis orientation
		of cairo, positive angles rotate in a clockwise direction.</li>
	</ul>
	</p>
	End Rem
	Method Rotate(degrees:Double)
		cairo_matrix_rotate(matrix, degrees * degToRad)
	End Method
	
	Rem
	bbdoc: Applies scaling by @sx, @sy to the transformation in matrix.
	about: The effect of the new transformation is to first scale the coordinates by @sx and @sy, then apply the
	original transformation to the coordinates.
	<p>Parameters:
	<ul>
	<li><b>sx</b> : scale factor in the X direction.</li>
	<li><b>sy</b> : scale factor in the Y direction.</li>
	</ul>
	</p>
	End Rem
	Method Scale(sx:Double, sy:Double)
		cairo_matrix_scale(matrix, sx,sy)
	End Method
	
	Rem
	bbdoc: Transforms the distance vector (dx,dy) by the matrix.
	about: This is similar to #Transform except that the translation components of the transformation are ignored.
	The calculation of the returned vector is as follows:
	<pre>
	dx2 = dx1 * a + dy1 * c
	dy2 = dx1 * b + dy1 * d
	</pre>
	Affine transformations are position invariant, so the same vector always transforms to the same vector.
	If (x1,y1) transforms to (x2,y2) then (x1 + dx1, y1 + dy1) will transform to (x1 + dx2, y1 + dy2) for all
	values of x1 and x2.
	<p>Parameters:
	<ul>
	<li><b>dx</b> : X component of a distance vector. <i>An in/out parameter</i>.</li>
	<li><b>dy</b> : Y component of a distance vector. <i>An in/out parameter</i>.</li>
	</ul>
	</p>
	End Rem
	Method TransformDistance(dx:Double Var,dy:Double Var)
		cairo_matrix_transform_distance(matrix, dx,dy)
	End Method
	
	Rem
	bbdoc: Transforms the point ( @x, @y ) by the matrix.
	about: Parameters:
	<ul>
	<li><b>x</b> : X position. <i>An in/out parameter</i>.</li>
	<li><b>y</b> : X position. <i>An in/out parameter</i>.</li>
	</ul>
	End Rem
	Method TransformPoint(x:Double Var,y:Double Var)
		cairo_matrix_transform_point(matrix, x,y)
	End Method

	Rem
	bbdoc: Applies a translation by @tx, @ty to the transformation in matrix.
	about: The effect of the new transformation is to first translate the coordinates by @tx and @ty, then apply
	the original transformation to the coordinates.
	<p>Parameters:
	<ul>
	<li><b>tx</b> : amount to translate in the X direction.</li>
	<li><b>ty</b> : amount to translate in the Y direction.</li>
	</ul>
	</p>
	End Rem
	Method Translate(tx:Double, ty:Double)
		cairo_matrix_translate(matrix, tx,ty)
	End Method

End Type

Rem
bbdoc: A TCairoFontFace specifies all aspects of a font other than the size or font matrix (a font matrix is used to distort a font by sheering it or scaling it unequally in the two directions).
about: A font face can be set on a TCairo by using #SetFontFace. The size and font matrix are set with #SetFontSize
and #SetFontMatrix.
End Rem
Type TCairoFontFace

	Field facePtr:Byte Ptr
	
	Function _create:TCairoFontFace(facePtr:Byte Ptr)
		If facePtr <> Null Then
			Local ff:TCairoFontFace = New TCairoFontFace
			ff.facePtr = facePtr
			Return ff
		End If
		Return Null
	End Function
	
	Rem
	bbdoc: Decreases the reference count on font_face by one.
	about: If the result is zero, then font_face and all associated resources are freed. See #Reference.
	End Rem
	Method Destroy()
		cairo_font_face_destroy(facePtr)
	End Method

'	Method GetUserData()

	Rem
	bbdoc: This function returns the type of the backend used to create a font face.
	about: The following lists the possible font types:
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_FONT_TYPE_TOY</td><td>The font was created using cairo's toy font api</td></tr>
	<tr><td>CAIRO_FONT_TYPE_FT</td><td>The font is of type FreeType</td></tr>
	<tr><td>CAIRO_FONT_TYPE_WIN32</td><td>The font is of type Win32</td></tr>
	<tr><td>CAIRO_FONT_TYPE_ATSUI</td><td>The font is of type ATSUI</td></tr>
	</table>
	End Rem
	Method GetType:Int()
		Return cairo_font_face_get_type(facePtr)
	End Method
	
	Rem
	bbdoc: Increases the reference count on font_face by one.
	about: This prevents the font face from being destroyed until a matching call to #Destroy is made.
	End Rem
	Method Reference()
		facePtr = cairo_font_face_reference(facePtr)
	End Method
	
'	Method SetUserData()

	Rem
	bbdoc: Checks whether an error has previously occurred for this font face.
	returns: CAIRO_STATUS_SUCCESS or another error such as CAIRO_STATUS_NO_MEMORY.
	End Rem
	Method Status:Int()
		Return cairo_font_face_status(facePtr)
	End Method

End Type

Type TCairoFontOptionsStruct
	Field AntiAlias:Int
	Field subpixel_order:Int
	Field hint_style:Int
	Field hint_metrics:Int
End Type

Rem
bbdoc: How a font should be rendered.
End Rem
Type TCairoFontOptions

	Field optionsPtr:Byte Ptr
	
	Rem
	bbdoc: Allocates a new font options object with all options initialized to default values.
	returns: A newly allocated TCairoFontOptions object. Free with #Destroy. This function always returns a valid object; if memory cannot be allocated, then a special error object is returned where all operations on the object do nothing. You can check for this with #Status.
	End Rem
	Function Create:TCairoFontOptions()
		Local options:TCairoFontOptions = New TCairoFontOptions
		options.optionsPtr = cairo_font_options_create()
		Return options
	End Function
	
	Function _create:TCairoFontOptions(fontOptions:TCairoFontOptionsStruct)
		Local options:TCairoFontOptions = New TCairoFontOptions
		options.optionsPtr = Varptr fontOptions
		Return options
	End Function
	
	Rem
	bbdoc: Allocates a new font options object copying the option values from this one.
	returns: A newly allocated TCairoFontOptions object. Free with #Destroy. This function always returns a valid object; if memory cannot be allocated, then a special error object is returned where all operations on the object do nothing. You can check for this with #Status.
	End Rem
	Method Copy:TCairoFontOptions()
		Local options:TCairoFontOptions = New TCairoFontOptions
		options.optionsPtr = cairo_font_options_copy(optionsPtr)
	End Method
	
	Rem
	bbdoc: Destroys a TCairoFontOptions object created with with #Create or #Copy.
	End Rem
	Method Destroy()
		cairo_font_options_destroy(optionsPtr)
	End Method
	
	Rem
	bbdoc: Compares two font options objects for equality.
	returns: True if all fields of the two font options objects match.
	about: Parameters:
	<ul>
	<li><b>other</b> : another TCairoFontOptions.</li>
	</ul>
	End Rem
	Method EqualTo:Int(other:TCairoFontOptions)
		Return cairo_font_options_equal(Self.optionsPtr, other.optionsPtr)
	End Method
	
	Rem
	bbdoc: Gets the antialising mode for the font options object.
	returns: The antialiasing mode.
	End Rem
	Method GetAntialias:Int()
		Return cairo_font_options_get_antialias(optionsPtr)
	End Method
	
	Rem
	bbdoc: Gets the metrics hinting mode for the font options object.
	returns: The metrics hinting mode for the font options object.
	about: See #SetHintMetrics for more details.
	End Rem
	Method GetHintMetrics:Int()
		Return cairo_font_options_get_hint_metrics(optionsPtr)
	End Method

	Rem
	bbdoc: Gets the hint style for font outlines for the font options object.
	returns: The hint style for the font options object.
	about: See #SetHintStyle for more information.
	End Rem
	Method GetHintStyle:Int()
		Return cairo_font_options_get_hint_style(optionsPtr)
	End Method

	Rem
	bbdoc: Gets the subpixel order for the font options object.
	returns: The subpixel order for the font options object.
	about: See #SetSubpixelOrder for more details.
	End Rem
	Method GetSubpixelOrder:Int()
		Return cairo_font_options_get_subpixel_order(optionsPtr)
	End Method
	
	Rem
	bbdoc: Compute a hash for the font options object.
	returns: The hash value for the font options object. The return value can be cast to a 32-bit type if a 32-bit hash value is needed.
	about: This value will be useful when storing an object containing a TCairoFontOptions in a hash table.
	End Rem
	Method Hash:Long()
		Return cairo_font_options_hash(optionsPtr)
	End Method
	
	Rem
	bbdoc: Merges non-default options from @other into this one, replacing existing values.
	about: This operation can be thought of as somewhat similar to compositing other onto options with the
	operation of CAIRO_OPERATION_OVER.
	<p>Parameters:
	<ul>
	<li><b>other</b> : another TCairoFontOptions.</li>
	</ul>
	</p>
	End Rem
	Method Merge(other:TCairoFontOptions)
		cairo_font_options_merge(Self.optionsPtr, other.optionsPtr)
	End Method

	Rem
	bbdoc: Sets the antiliasing mode for the font options object.
	about: This specifies the type of antialiasing to do when rendering text.
	<p>Possible Antialias values :</p>
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_ANTIALIAS_DEFAULT</td><td>Use the default antialiasing for the subsystem and target device</td></tr>
	<tr><td>CAIRO_ANTIALIAS_NONE</td><td>Use a bilevel alpha mask</td></tr>
	<tr><td>CAIRO_ANTIALIAS_GRAY</td><td>Perform single-color antialiasing (using shades of gray for black text
	on a white background, for example).</td></tr>
	<tr><td>CAIRO_ANTIALIAS_SUBPIXEL</td><td>Perform antialiasing by taking advantage of the order of
	subpixel elements on devices such as LCD panels</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>aa</b> : the new antialiasing mode. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetAntialias(aa:Int)
		cairo_font_options_set_antialias(optionsPtr, aa)
	End Method
	
	Rem
	bbdoc: Sets the metrics hinting mode for the font options object.
	about: This controls whether metrics are quantized to integer values in device units. The following describes the possible values :
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_HINT_METRICS_DEFAULT</td><td>Hint metrics in the default manner for the font backend and target device</td></tr>
	<tr><td>CAIRO_HINT_METRICS_OFF</td><td>Do not hint font metrics</td></tr>
	<tr><td>CAIRO_HINT_METRICS_ON</td><td>Hint font metrics</td></tr>
	</table>	
	<p>Parameters:
	<ul>
	<li><b>hm</b> : the new metrics hinting mode. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetHintMetrics(hm:Int)
		cairo_font_options_set_hint_metrics(optionsPtr, hm)
	End Method

	Rem
	bbdoc: Sets the hint style for font outlines for the font options object.
	about: This controls whether to fit font outlines to the pixel grid, and if so, whether to optimize for
	fidelity or contrast. The following describes the possible values :
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_HINT_STYLE_DEFAULT</td><td>Use the default hint style for for font backend and target device</td></tr>
	<tr><td>CAIRO_HINT_STYLE_NONE</td><td>Do not hint outlines</td></tr>
	<tr><td>CAIRO_HINT_STYLE_SLIGHT</td><td>Hint outlines slightly to improve contrast while retaining good fidelity to the original shapes.</td></tr>
	<tr><td>CAIRO_HINT_STYLE_MEDIUM</td><td>Hint outlines with medium strength giving a compromise between fidelity to the original shapes and contrast</td></tr>
	<tr><td>CAIRO_HINT_STYLE_FULL</td><td>Hint outlines to maximize contrast</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>hs</b> : the new hint style. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetHintStyle(hs:Int)
		cairo_font_options_set_hint_style(optionsPtr, hs)
	End Method

	Rem
	bbdoc: Sets the subpixel order for the font options object.
	about: The subpixel order specifies the order of color elements within each pixel on the display device
	when rendering with an antialiasing mode of CAIRO_ANTIALIAS_SUBPIXEL. The possible values are :
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_SUBPIXEL_ORDER_DEFAULT</td><td>Use the default subpixel order for for the target device</td></tr>
	<tr><td>CAIRO_SUBPIXEL_ORDER_RGB</td><td>Subpixel elements are arranged horizontally with red at the left</td></tr>
	<tr><td>CAIRO_SUBPIXEL_ORDER_BGR</td><td>Subpixel elements are arranged horizontally with blue at the left</td></tr>
	<tr><td>CAIRO_SUBPIXEL_ORDER_VRGB</td><td>Subpixel elements are arranged vertically with red at the top</td></tr>
	<tr><td>CAIRO_SUBPIXEL_ORDER_VBGR</td><td>Subpixel elements are arranged vertically with blue at the top</td></tr>
	</table>
	<p>Parameters:
	<ul>
	<li><b>spo</b> : the new subpixel order. See above for details.</li>
	</ul>
	</p>
	End Rem
	Method SetSubpixelOrder(spo:Int)
		cairo_font_options_set_subpixel_order(optionsPtr, spo)
	End Method
	
	Rem
	bbdoc: Checks whether an error has previously occurred for this font options object.
	returns: CAIRO_STATUS_SUCCESS or CAIRO_STATUS_NO_MEMORY
	End Rem
	Method Status:Int()
		Return cairo_font_options_status(optionsPtr)
	End Method

End Type

Rem
bbdoc: A data structure for holding a path.
about: This data structure serves as the return value for #CopyPath and #CopyPathFlat as well the input value for
#AppendPath.
End Rem
Type TCairoPath

	Field pathPtr:Byte Ptr

	Function _create:TCairoPath(pathPtr:Byte Ptr)
		If pathPtr <> Null Then
			Local p:TCairoPath = New TCairoPath
			p.pathPtr = pathPtr
			Return p
		End If
		Return Null
	End Function

	Rem
	bbdoc: Immediately releases all memory associated with path.
	about: After a call to #Destroy the path pointer is no longer valid and should not be used further.
	End Rem
	Method Destroy()
		cairo_path_destroy(pathPtr)
	End Method

End Type

Rem
bbdoc: The TCairoTextExtents structure stores the extents of a single glyph or a string of glyphs in user-space coordinates.
about: Because text extents are in user-space coordinates, they are mostly, but not entirely, independent of the
current transformation matrix. If you call #Scale(2.0, 2.0), text will be drawn twice as big, but the reported text
extents will not be doubled. They will change slightly due to hinting (so you can't assume that metrics are
independent of the transformation matrix), but otherwise will remain unchanged.
End Rem
Type TCairoTextExtents
	Rem
	bbdoc: The horizontal distance from the origin to the leftmost part of the glyphs as drawn.
	about: Positive if the glyphs lie entirely to the right of the origin.
	End Rem
	Field x_bearing:Double
	Rem
	bbdoc: The vertical distance from the origin to the topmost part of the glyphs as drawn.
	about: Positive only if the glyphs lie completely below the origin; will usually be negative.
	End Rem
	Field y_bearing:Double
	Rem
	bbdoc: Width of the glyphs as drawn.
	End Rem
	Field width:Double
	Rem
	bbdoc: Height of the glyphs as drawn.
	End Rem
	Field height:Double
	Rem
	bbdoc: Distance to advance in the X direction after drawing these glyphs.
	End Rem
	Field x_advance:Double
	Rem
	bbdoc: Distance to advance in the Y direction after drawing these glyphs.
	about; Will typically be zero except for vertical text layout as found in East-Asian languages.
	End Rem
	Field y_advance:Double
End Type

Rem
bbdoc: A TCairoScaledFont is a font scaled to a particular size and device resolution.
about: A TCairoScaledFont is most useful for low-level font usage where a library or application wants to cache a
reference to a scaled font to speed up the computation of metrics.
End Rem
Type TCairoScaledFont

	Field scaledfontPtr:Byte Ptr

	Rem
	bbdoc: Creates a TCairoScaledFont object from a font face and matrices that describe the size of the font and the environment in which it will be used.
	returns: A newly created TCairoScaledFont. Destroy with #Destroy.
	End Rem
	Function Create:TCairoScaledFont(font_face:TCairoFontFace, font_matrix:TCairoMatrix, ..
											ctm:TCairoMatrix, options:TCairoFontOptions)
		Local scaledfont:TCairoScaledFont = New TCairoScaledFont
		scaledfont.scaledfontPtr = cairo_scaled_font_create(font_face.facePtr, font_matrix.matrix, ..
																	ctm.matrix, options.optionsPtr)
		Return scaledfont
	End Function
	
	Rem
	bbdoc: Decreases the reference count on font by one.
	about: If the result is zero, then font and all associated resources are freed. See #Reference.
	End Rem
	Method Destroy()
		cairo_scaled_font_destroy(scaledfontPtr)
	End Method

	Rem
	bbdoc: Gets the metrics for a TCairoScaledFont.
	returns: A #TCairoFontExtents object.
	End Rem
	Method Extents:TCairoFontExtents()
		Local extents:TCairoFontExtents = New TCairoFontExtents
		cairo_font_extents(scaledfontPtr, extents)
		Return extents
	End Method

'	Method GlyphExtents

	Rem
	bbdoc: Returns the CTM with which the scaled font was created.
	returns: The CTM.
	End Rem
	Method GetCTM:TCairoMatrix()
		Local r:TCairoMatrixStruct = New TCairoMatrixStruct
		cairo_scaled_font_get_ctm(scaledfontPtr, r)
		Return TCairoMatrix._create(r)
	End Method
	
	Rem
	bbdoc: Gets the font face that this scaled font was created for.
	returns: The TCairoFontFace with which the scaled font was created.
	End Rem
	Method GetFontFace:TCairoFontFace()
		Return TCairoFontFace._create(cairo_scaled_font_get_font_face(scaledfontPtr))
	End Method
	
	Rem
	bbdoc: Returns the font matrix with which the scaled font was created.
	returns: The font matrix.
	End Rem
	Method GetFontMatrix:TCairoMatrix()
		Local r:TCairoMatrixStruct = New TCairoMatrixStruct
		cairo_scaled_font_get_font_matrix(scaledfontPtr, r)
		Return TCairoMatrix._create(r)
	End Method
	
	Rem
	bbdoc: Returns the font options with which the scaled font was created.
	returns: The font options.
	End Rem
	Method GetFontOptions:TCairoFontOptions()
		Local fontOptions:TCairoFontOptionsStruct = New TCairoFontOptionsStruct
		cairo_scaled_font_get_font_options(scaledfontPtr, fontOptions)
		Return TCairoFontOptions._create(fontOptions)
	End Method
	
	Rem
	bbdoc: This method returns the type of the backend used to create a scaled font.
	returns: The type of scaled font.
	about: The following lists the possible font types:
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>CAIRO_FONT_TYPE_TOY</td><td>The font was created using cairo's toy font api</td></tr>
	<tr><td>CAIRO_FONT_TYPE_FT</td><td>The font is of type FreeType</td></tr>
	<tr><td>CAIRO_FONT_TYPE_WIN32</td><td>The font is of type Win32</td></tr>
	<tr><td>CAIRO_FONT_TYPE_ATSUI</td><td>The font is of type ATSUI</td></tr>
	</table>
	End Rem
	Method GetType:Int()
		Return cairo_scaled_font_get_type(scaledfontPtr)
	End Method

	Rem
	bbdoc: Increases the reference count on scaled font by one.
	about: This prevents scaled font from being destroyed until a matching call to #Destroy is made.
	End Rem
	Method Reference()
		scaledfontPtr = cairo_scaled_font_reference(scaledfontPtr)
	End Method

	Rem
	bbdoc: Checks whether an error has previously occurred for this scaled font.
	returns: CAIRO_STATUS_SUCCESS or another error such as CAIRO_STATUS_NO_MEMORY.
	End Rem
	Method Status:Int()
		Return cairo_scaled_font_status(scaledfontPtr)
	End Method
	
	Rem
	bbdoc: Gets the extents for a string of text.
	returns: a TCairoTextExtents object containing the retrieved extents.
	about: The extents describe a user-space rectangle that encloses the "inked" portion of the text drawn
	at the origin (0,0) (as it would be drawn by #ShowText if the cairo graphics state were set to the
	same font face, font matrix, ctm, and font options as the scaled font). Additionally, the x_advance
	and y_advance values indicate the amount by which the current point would be advanced by #ShowText.
	<p>Note that whitespace characters do not directly contribute to the size of the rectangle
	(extents.width and extents.height). They do contribute indirectly by changing the position
	of non-whitespace characters. In particular, trailing whitespace characters are likely to not
	affect the size of the rectangle, though they will affect the x_advance and y_advance values.</p>
	<p>Parameters:
	<ul>
	<li><b>text</b> : a string of text.</li>
	</ul>
	</p>
	End Rem
	Method TextExtents:TCairoTextExtents(text:String)
		Local extents:TCairoTextExtents = New TCairoTextExtents
		cairo_scaled_font_text_extents(scaledfontPtr, _cairo_ISO8859toUTF8(text), extents)
		Return extents
	End Method
	
End Type

Rem
bbdoc: The TCairoFontExtents type stores metric information for a font.
about: Values are given in the current user-space coordinate system.<br>
Because font metrics are in user-space coordinates, they are mostly, but not entirely, independent of the
current transformation matrix. If you call #Scale(2.0, 2.0), text will be drawn twice as big,
but the reported text extents will not be doubled. They will change slightly due to hinting (so you can't assume
that metrics are independent of the transformation matrix), but otherwise will remain unchanged.
End Rem
Type TCairoFontExtents
	Rem
	bbdoc: The distance that the font extends above the baseline.
	about: Note that this is not always exactly equal to the maximum of the extents of all the glyphs in the font,
	but rather is picked to express the font designer's intent as to how the font should align with elements above it.
	End Rem
	Field ascent:Double
	Rem
	bbdoc: The distance that the font extends below the baseline.
	about: This value is positive for typical fonts that include portions below the baseline. Note that this is
	not always exactly equal to the maximum of the extents of all the glyphs in the font, but rather is picked
	to express the font designer's intent as to how the the font should align with elements below it.
	End Rem
	Field descent:Double
	Rem
	bbdoc: The recommended vertical distance between baselines when setting consecutive lines of text with the font.
	about: This is greater than <i>ascent + descent</i> by a quantity known as the <i>line spacing</i> or <i>external leading</i>.
	When space is at a premium, most fonts can be set with only a distance of <i>ascent + descent</i> between lines.
	End Rem
	Field height:Double
	Rem
	bbdoc: The maximum distance in the X direction that the the origin is advanced for any glyph in the font
	End Rem
	Field max_x_advance:Double
	Rem
	bbdoc: The maximum distance in the Y direction that the the origin is advanced for any glyph in the font.
	about: This will be zero for normal fonts used for horizontal writing. (The scripts of East Asia are
	sometimes written vertically.)
	End Rem
	Field max_y_advance:Double
End Type

Rem
bbdoc: Dimensions of a sheet of paper.
End Rem
Type TCairoPaperDimension

	Rem
	bbdoc: The paper width.
	End Rem
	Field width:Double
	
	Rem
	bbdoc: The paper height.
	End Rem
	Field height:Double
	
	Function Set:TCairoPaperDimension(width:Double, height:Double)
		Local pd:TCairoPaperDimension = New TCairoPaperDimension
		
		pd.width = Ceil(width / 25.4 * 72)
		pd.height = Ceil(height / 25.4 * 72)
		
		Return pd
	End Function

End Type

Rem
bbdoc: Contains a list of page-sizes, useful for setting up PDF and Postscript pages.
about: Usage: TCairoPaperSize.PAPER_A4.width
<p>
ISO sizes :<br>
PAPER_A0, PAPER_A1, PAPER_A2, PAPER_A3, PAPER_A4, PAPER_A5,<br>
PAPER_A6, PAPER_A7, PAPER_A8, PAPER_A9, PAPER_A10<br>
PAPER_B0, PAPER_B1, PAPER_B2, PAPER_B3, PAPER_B4, PAPER_B5,<br>
PAPER_B6, PAPER_B7, PAPER_B8, PAPER_B9, PAPER_B10<br>
PAPER_C0, PAPER_C1, PAPER_C2, PAPER_C3, PAPER_C4, PAPER_C5,<br>
PAPER_C6, PAPER_C7, PAPER_C8, PAPER_C9, PAPER_C10
</p>
<p>
US sizes:<br>
PAPER_4_5x7, PAPER_4_25x7_25<br>
PAPER_5_25x8, PAPER_5_25x8_5, PAPER_5_375x8_375, PAPER_5_5x8_5, PAPER_5_875x8_25<br>
PAPER_6x9, PAPER_6_125x9_25, PAPER_6_25x9_25, PAPER_6_5x9_25<br>
PAPER_7x10, PAPER_7_375x9_25, PAPER_7_5x9_375, PAPER_7_5x9_125, PAPER_7_5x10<br>
PAPER_8x10, PAPER_8_25x10_5, PAPER_8_5x11<br>
PAPER_9x12, PAPER_12x18
</p>
End Rem
Type TCairoPaperSize

	' define size in mm.. will convert to points -> DPI = 72.

	' ISO paper sizes
	Global PAPER_A0:TCairoPaperDimension = TCairoPaperDimension.Set(841 , 1189)
	Global PAPER_A1:TCairoPaperDimension = TCairoPaperDimension.Set(594 , 841)
	Global PAPER_A2:TCairoPaperDimension = TCairoPaperDimension.Set(420 , 594)
	Global PAPER_A3:TCairoPaperDimension = TCairoPaperDimension.Set(297 , 420)
	Global PAPER_A4:TCairoPaperDimension = TCairoPaperDimension.Set(210 , 297)
	Global PAPER_A5:TCairoPaperDimension = TCairoPaperDimension.Set(148 , 210)
	Global PAPER_A6:TCairoPaperDimension = TCairoPaperDimension.Set(105 , 148)
	Global PAPER_A7:TCairoPaperDimension = TCairoPaperDimension.Set(74 , 105)
	Global PAPER_A8:TCairoPaperDimension = TCairoPaperDimension.Set(52 , 74)
	Global PAPER_A9:TCairoPaperDimension = TCairoPaperDimension.Set(37 , 52)
	Global PAPER_A10:TCairoPaperDimension = TCairoPaperDimension.Set(26 , 37)
	
	Global PAPER_B0:TCairoPaperDimension = TCairoPaperDimension.Set(1000 , 1414)
	Global PAPER_B1:TCairoPaperDimension = TCairoPaperDimension.Set(707 , 1000)
	Global PAPER_B2:TCairoPaperDimension = TCairoPaperDimension.Set(500 , 707)
	Global PAPER_B3:TCairoPaperDimension = TCairoPaperDimension.Set(353 , 500)
	Global PAPER_B4:TCairoPaperDimension = TCairoPaperDimension.Set(250 , 353)
	Global PAPER_B5:TCairoPaperDimension = TCairoPaperDimension.Set(176 , 250)
	Global PAPER_B6:TCairoPaperDimension = TCairoPaperDimension.Set(125 , 176)
	Global PAPER_B7:TCairoPaperDimension = TCairoPaperDimension.Set(88 , 125)
	Global PAPER_B8:TCairoPaperDimension = TCairoPaperDimension.Set(62 , 88)
	Global PAPER_B9:TCairoPaperDimension = TCairoPaperDimension.Set(44 , 62)
	Global PAPER_B10:TCairoPaperDimension = TCairoPaperDimension.Set(31 , 44)

	Global PAPER_C0:TCairoPaperDimension = TCairoPaperDimension.Set(917 , 1297)
	Global PAPER_C1:TCairoPaperDimension = TCairoPaperDimension.Set(648 , 917)
	Global PAPER_C2:TCairoPaperDimension = TCairoPaperDimension.Set(458 , 648)
	Global PAPER_C3:TCairoPaperDimension = TCairoPaperDimension.Set(324 , 458)
	Global PAPER_C4:TCairoPaperDimension = TCairoPaperDimension.Set(229 , 324)
	Global PAPER_C5:TCairoPaperDimension = TCairoPaperDimension.Set(162 , 229)
	Global PAPER_C6:TCairoPaperDimension = TCairoPaperDimension.Set(114 , 162)
	Global PAPER_C7:TCairoPaperDimension = TCairoPaperDimension.Set(81 , 114)
	Global PAPER_C8:TCairoPaperDimension = TCairoPaperDimension.Set(57 , 81)
	Global PAPER_C9:TCairoPaperDimension = TCairoPaperDimension.Set(40 , 57)
	Global PAPER_C10:TCairoPaperDimension = TCairoPaperDimension.Set(28 , 40)

	' US paper sizes... name provides "inch" measurement.. eg.
	'   4_5x7 = 4 1/2 x 7  inches
	Global PAPER_4_5x7:TCairoPaperDimension = TCairoPaperDimension.Set(108 , 178)
	Global PAPER_4_25x7_25:TCairoPaperDimension = TCairoPaperDimension.Set(108 , 184)

	Global PAPER_5_25x8:TCairoPaperDimension = TCairoPaperDimension.Set(133 , 203)
	Global PAPER_5_25x8_5:TCairoPaperDimension = TCairoPaperDimension.Set(133 , 216)
	Global PAPER_5_375x8_375:TCairoPaperDimension = TCairoPaperDimension.Set(137 , 213)
	Global PAPER_5_5x8_5:TCairoPaperDimension = TCairoPaperDimension.Set(140 , 216)
	Global PAPER_5_875x8_25:TCairoPaperDimension = TCairoPaperDimension.Set(149 , 210)

	Global PAPER_6x9:TCairoPaperDimension = TCairoPaperDimension.Set(152 , 229)
	Global PAPER_6_125x9_25:TCairoPaperDimension = TCairoPaperDimension.Set(156 , 235)
	Global PAPER_6_25x9_25:TCairoPaperDimension = TCairoPaperDimension.Set(159 , 235)
	Global PAPER_6_5x9_25:TCairoPaperDimension = TCairoPaperDimension.Set(165 , 235)
	
	Global PAPER_7x10:TCairoPaperDimension = TCairoPaperDimension.Set(178 , 254)
	Global PAPER_7_375x9_25:TCairoPaperDimension = TCairoPaperDimension.Set(187 , 235)
	Global PAPER_7_5x9_375:TCairoPaperDimension = TCairoPaperDimension.Set(190 , 213)
	Global PAPER_7_5x9_125:TCairoPaperDimension = TCairoPaperDimension.Set(190 , 232)
	Global PAPER_7_5x10:TCairoPaperDimension = TCairoPaperDimension.Set(190 , 254)
	
	Global PAPER_8x10:TCairoPaperDimension = TCairoPaperDimension.Set(203 , 254)
	Global PAPER_8_25x10_5:TCairoPaperDimension = TCairoPaperDimension.Set(210 , 267)
	Global PAPER_8_5x11:TCairoPaperDimension = TCairoPaperDimension.Set(216 , 279)

	Global PAPER_9x12:TCairoPaperDimension = TCairoPaperDimension.Set(229 , 305)
	Global PAPER_12x18:TCairoPaperDimension = TCairoPaperDimension.Set(305 , 457)
	
End Type

Type TCairoNoContextException
	Method toString:String()
		Return "Call CreateContext() before attempting to use the context methods"
	End Method
End Type

