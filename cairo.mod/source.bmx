' Copyright (c) 2006-2011 Bruce A Henderson
'
'  The contents of this file are subject to the Mozilla Public License
'  Version 1.1 (the "License"); you may not use this file except in
'  compliance with the License. You may obtain a copy of the License at
'  http://www.mozilla.org/MPL/
'  
'  Software distributed under the License is distributed on an "AS IS"
'  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
'  License for the specific language governing rights and limitations
'  under the License.
'  
'  The Original Code is BaH.Cairo.
'  
'  The Initial Developer of the Original Code is Duncan Cross.
'
SuperStrict

' this first to ensure we get *our* config.h
Import "src/*.h"

Import "../../pub.mod/zlib.mod/*.h"
Import "../../pub.mod/libpng.mod/*.h"
Import "../../pub.mod/freetype.mod/include/*.h"
Import "../../pub.mod/glew.mod/*.h"

' pixman
Import "src/pixman/*.h"
Import "src/pixman/pixman-access.c"
Import "src/pixman/pixman-access-accessors.c"
Import "src/pixman/pixman-cpu.c"
Import "src/pixman/pixman-gradient-walker.c"
Import "src/pixman/pixman-region16.c"
Import "src/pixman/pixman-region32.c"
Import "src/pixman/pixman-image.c"
Import "src/pixman/pixman-implementation.c"
Import "src/pixman/pixman-combine32.c"
Import "src/pixman/pixman-combine64.c"
Import "src/pixman/pixman-general.c"
Import "src/pixman/pixman.c"
Import "src/pixman/pixman-fast-path.c"
Import "src/pixman/pixman-solid-fill.c"
Import "src/pixman/pixman-conical-gradient.c"
Import "src/pixman/pixman-linear-gradient.c"
Import "src/pixman/pixman-radial-gradient.c"
Import "src/pixman/pixman-bits-image.c"
Import "src/pixman/pixman-utils.c"
Import "src/pixman/pixman-edge.c"
Import "src/pixman/pixman-edge-accessors.c"
Import "src/pixman/pixman-trap.c"
Import "src/pixman/pixman-timer.c"
Import "src/pixman/pixman-matrix.c"
?macosx86
Import "src/pixman/pixman-mmx.c"
Import "src/pixman/pixman-sse2.c"
?


' cairo
Import "src/cairo-analysis-surface.c"
Import "src/cairo-arc.c"
Import "src/cairo-array.c"
Import "src/cairo-atomic.c"
Import "src/cairo-base64-stream.c"
Import "src/cairo-base85-stream.c"
Import "src/cairo-bentley-ottmann.c"
Import "src/cairo-bentley-ottmann-rectangular.c"
Import "src/cairo-bentley-ottmann-rectilinear.c"
Import "src/cairo.c"
Import "src/cairo-boxes.c"
Import "src/cairo-cache.c"
Import "src/cairo-clip.c"
Import "src/cairo-color.c"
Import "src/cairo-composite-rectangles.c"
Import "src/cairo-debug.c"
Import "src/cairo-device.c"
Import "src/cairo-fixed.c"
Import "src/cairo-font-face.c"
Import "src/cairo-font-face-twin.c"
Import "src/cairo-font-face-twin-data.c"
Import "src/cairo-font-options.c"
Import "src/cairo-freelist.c"
Import "src/cairo-gstate.c"
Import "src/cairo-hash.c"
Import "src/cairo-hull.c"
Import "src/cairo-image-info.c"
Import "src/cairo-image-surface.c"
Import "src/cairo-lzw.c"
Import "src/cairo-matrix.c"
'Import "src/cairo-meta-surface.c"
Import "src/cairo-misc.c"
Import "src/cairo-mutex.c"
Import "src/cairo-output-stream.c"
Import "src/cairo-observer.c"
Import "src/cairo-paginated-surface.c"
Import "src/cairo-path-bounds.c"
Import "src/cairo-path.c"
Import "src/cairo-path-fill.c"
Import "src/cairo-path-fixed.c"
Import "src/cairo-path-in-fill.c"
Import "src/cairo-path-stroke.c"
Import "src/cairo-pattern.c"
Import "src/cairo-pen.c"
Import "src/cairo-polygon.c"
Import "src/cairo-rectangle.c"
Import "src/cairo-rectangular-scan-converter.c"
Import "src/cairo-recording-surface.c"
Import "src/cairo-region.c"
Import "src/cairo-rtree.c"
Import "src/cairo-scaled-font.c"
Import "src/cairo-slope.c"
Import "src/cairo-spans.c"
Import "src/cairo-spline.c"
Import "src/cairo-stroke-style.c"
Import "src/cairo-surface.c"
Import "src/cairo-surface-fallback.c"
Import "src/cairo-surface-clipper.c"
Import "src/cairo-surface-snapshot.c"
Import "src/cairo-surface-wrapper.c"
Import "src/cairo-system.c"
'Import "src/cairo-tee-surface.c"
Import "src/cairo-tor-scan-converter.c"
Import "src/cairo-toy-font-face.c"
Import "src/cairo-traps.c"
Import "src/cairo-unicode.c"
Import "src/cairo-user-font.c"
Import "src/cairo-version.c"
Import "src/cairo-wideint.c"


Import "src/cairo-cff-subset.c"
Import "src/cairo-scaled-font-subsets.c"
Import "src/cairo-truetype-subset.c"
Import "src/cairo-type1-fallback.c"
Import "src/cairo-type1-subset.c"
Import "src/cairo-type3-glyph-surface.c"


Import "src/cairo-pdf-surface.c"
Import "src/cairo-pdf-operators.c"
Import "src/cairo-deflate-stream.c"

Import "src/cairo-png.c"

Import "src/cairo-ps-surface.c"

Import "src/cairo-ft-font.c"

'Import "src/cairo-gl-shaders.c"
'Import "src/cairo-gl-surface.c"
'Import "src/cairo-gl-glyphs.c"
'Import "src/cairo-gl-device.c"
'Import "src/cairo-gl-composite.c"
'Import "src/cairo-gl-gradient.c"

Import "src/cairo-script-surface.c"

Import "src/cairo-svg-surface.c"

Import "src/cairo-xml-surface.c"

?macos
'Import "glglue_macos.m"
?

