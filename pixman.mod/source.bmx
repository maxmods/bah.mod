' Copyright (c) 2006-2017 Bruce A Henderson
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


' pixman
Import "pixman/*.h"

Import "pixman/pixman-access.c"
Import "pixman/pixman-access-accessors.c"
Import "pixman/pixman-gradient-walker.c"
Import "pixman/pixman-region16.c"
Import "pixman/pixman-region32.c"
Import "pixman/pixman-image.c"
Import "pixman/pixman-implementation.c"
Import "pixman/pixman-combine32.c"
Import "pixman/pixman-combine-float.c"
Import "pixman/pixman-general.c"
Import "pixman/pixman.c"
Import "pixman/pixman-fast-path.c"
Import "pixman/pixman-solid-fill.c"
Import "pixman/pixman-conical-gradient.c"
Import "pixman/pixman-linear-gradient.c"
Import "pixman/pixman-radial-gradient.c"
Import "pixman/pixman-bits-image.c"
Import "pixman/pixman-utils.c"
Import "pixman/pixman-edge.c"
Import "pixman/pixman-edge-accessors.c"
Import "pixman/pixman-trap.c"
Import "pixman/pixman-timer.c"
Import "pixman/pixman-matrix.c"
?x86
Import "pixman/pixman-mmx.c"
?x64
Import "pixman/pixman-sse2.c"
?
Import "pixman/pixman-x86.c"
Import "pixman/pixman-arm.c"
Import "pixman/pixman-noop.c"
Import "pixman/pixman-mips.c"
Import "pixman/pixman-ppc.c"

