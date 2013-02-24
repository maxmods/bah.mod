SuperStrict

Rem
bbdoc: 
End Rem
Module BaH.CairoWin32

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

?win32

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TCairoWin32Surface Extends TCairoSurface

	Rem
	bbdoc: Creates a cairo surface that targets the given DC.
	about: The DC will be queried for its initial clip extents, and this will be used as the size
	of the cairo surface. The resulting surface will always be of format CAIRO_FORMAT_RGB24.
	Should you need another surface format, you will need to create one through CreateWithDIB().
	End Rem
	Function Create:TCairoWin32Surface(hdc:Int)
		Local win32Surf:TCairoWin32Surface = New TCairoWin32Surface 
		win32Surf.surfacePtr = cairo_win32_surface_create(hdc)
		Return win32Surf
	End Function



End Type


?
