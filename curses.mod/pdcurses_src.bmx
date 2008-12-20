' Copyright (c) 2007-2008, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "src/pdcurses/*.h"
Import "src/pdcurses/win32/*.h"

Import "src/pdcurses/win32/pdcclip.c"
Import "src/pdcurses/win32/pdcdisp.c"
Import "src/pdcurses/win32/pdcgetsc.c"
Import "src/pdcurses/win32/pdckbd.c"
Import "src/pdcurses/win32/pdcscrn.c"
Import "src/pdcurses/win32/pdcsetsc.c"
Import "src/pdcurses/win32/pdcutil.c"


Import "src/pdcurses/pdcurses/attr.c"
Import "src/pdcurses/pdcurses/clear.c"
Import "src/pdcurses/pdcurses/deleteln.c"
Import "src/pdcurses/pdcurses/getyx.c"
Import "src/pdcurses/pdcurses/inopts.c"
Import "src/pdcurses/pdcurses/kernel.c"
Import "src/pdcurses/pdcurses/outopts.c"
Import "src/pdcurses/pdcurses/refresh.c"
Import "src/pdcurses/pdcurses/slk.c"
Import "src/pdcurses/pdcurses/util.c"
Import "src/pdcurses/pdcurses/addch.c"
Import "src/pdcurses/pdcurses/beep.c"
Import "src/pdcurses/pdcurses/color.c"
Import "src/pdcurses/pdcurses/deprec.c"
Import "src/pdcurses/pdcurses/inch.c"
Import "src/pdcurses/pdcurses/insch.c"
Import "src/pdcurses/pdcurses/keyname.c"
Import "src/pdcurses/pdcurses/overlay.c"
Import "src/pdcurses/pdcurses/scanw.c"
Import "src/pdcurses/pdcurses/termattr.c"
Import "src/pdcurses/pdcurses/window.c"
Import "src/pdcurses/pdcurses/addchstr.c"
Import "src/pdcurses/pdcurses/bkgd.c"
Import "src/pdcurses/pdcurses/debug.c"
Import "src/pdcurses/pdcurses/getch.c"
Import "src/pdcurses/pdcurses/inchstr.c"
Import "src/pdcurses/pdcurses/insstr.c"
Import "src/pdcurses/pdcurses/mouse.c"
Import "src/pdcurses/pdcurses/pad.c"
Import "src/pdcurses/pdcurses/scr_dump.c"
Import "src/pdcurses/pdcurses/terminfo.c"
Import "src/pdcurses/pdcurses/addstr.c"
Import "src/pdcurses/pdcurses/border.c"
Import "src/pdcurses/pdcurses/delch.c"
Import "src/pdcurses/pdcurses/getstr.c"
Import "src/pdcurses/pdcurses/initscr.c"
Import "src/pdcurses/pdcurses/instr.c"
Import "src/pdcurses/pdcurses/move.c"
Import "src/pdcurses/pdcurses/printw.c"
Import "src/pdcurses/pdcurses/bmx_scroll.c" ' renamed!!
Import "src/pdcurses/pdcurses/touch.c"

Import "src/pdcurses/pdcurses/panel.c"

'Import "cursesglue.cpp"
