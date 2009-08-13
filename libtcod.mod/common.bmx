' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
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


Import "source.bmx"


Global TCOD_REFRESH_IMAGE:Int = True


Extern

	Function bmx_tcodcolor_create:Byte Ptr(r:Int, g:Int, b:Int)
	Function bmx_tcodcolor_delete(handle:Byte Ptr)
	Function bmx_tcodcolor_equals:Int(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_tcodcolor_updatecolors(handle:Byte Ptr, red:Int Ptr, green:Int Ptr, blue:Int Ptr)
	Function bmx_tcodcolor_setr(handle:Byte Ptr, value:Int)
	Function bmx_tcodcolor_setg(handle:Byte Ptr, value:Int)
	Function bmx_tcodcolor_setb(handle:Byte Ptr, value:Int)
	Function bmx_tcodcolor_lerp:Byte Ptr(a:Byte Ptr, b:Byte Ptr, coef:Float)

	Function bmx_tcodcolor_black:Byte Ptr()
	Function bmx_tcodcolor_darkgrey:Byte Ptr()
	Function bmx_tcodcolor_grey:Byte Ptr()
	Function bmx_tcodcolor_lightgrey:Byte Ptr()
	Function bmx_tcodcolor_white:Byte Ptr()
	Function bmx_tcodcolor_red:Byte Ptr()
	Function bmx_tcodcolor_orange:Byte Ptr()
	Function bmx_tcodcolor_chartreuse:Byte Ptr()
	Function bmx_tcodcolor_yellow:Byte Ptr()
	Function bmx_tcodcolor_green:Byte Ptr()
	Function bmx_tcodcolor_sea:Byte Ptr()
	Function bmx_tcodcolor_cyan:Byte Ptr()
	Function bmx_tcodcolor_sky:Byte Ptr()
	Function bmx_tcodcolor_blue:Byte Ptr()
	Function bmx_tcodcolor_violet:Byte Ptr()
	Function bmx_tcodcolor_magenta:Byte Ptr()
	Function bmx_tcodcolor_pink:Byte Ptr()
	Function bmx_tcodcolor_darkred:Byte Ptr()
	Function bmx_tcodcolor_darkorange:Byte Ptr()
	Function bmx_tcodcolor_darkyellow:Byte Ptr()
	Function bmx_tcodcolor_darkchartreuse:Byte Ptr()
	Function bmx_tcodcolor_darkgreen:Byte Ptr()
	Function bmx_tcodcolor_darksea:Byte Ptr()
	Function bmx_tcodcolor_darkcyan:Byte Ptr()
	Function bmx_tcodcolor_darksky:Byte Ptr()
	Function bmx_tcodcolor_darkblue:Byte Ptr()
	Function bmx_tcodcolor_darkviolet:Byte Ptr()
	Function bmx_tcodcolor_darkmagenta:Byte Ptr()
	Function bmx_tcodcolor_darkpink:Byte Ptr()
	Function bmx_tcodcolor_darkerred:Byte Ptr()
	Function bmx_tcodcolor_darkerorange:Byte Ptr()
	Function bmx_tcodcolor_darkeryellow:Byte Ptr()
	Function bmx_tcodcolor_darkerchartreuse:Byte Ptr()
	Function bmx_tcodcolor_darkergreen:Byte Ptr()
	Function bmx_tcodcolor_darkersea:Byte Ptr()
	Function bmx_tcodcolor_darkercyan:Byte Ptr()
	Function bmx_tcodcolor_darkersky:Byte Ptr()
	Function bmx_tcodcolor_darkerblue:Byte Ptr()
	Function bmx_tcodcolor_darkerviolet:Byte Ptr()
	Function bmx_tcodcolor_darkermagenta:Byte Ptr()
	Function bmx_tcodcolor_darkerpink:Byte Ptr()
	Function bmx_tcodcolor_lightred:Byte Ptr()
	Function bmx_tcodcolor_lightorange:Byte Ptr()
	Function bmx_tcodcolor_lightyellow:Byte Ptr()
	Function bmx_tcodcolor_lightchartreuse:Byte Ptr()
	Function bmx_tcodcolor_lightgreen:Byte Ptr()
	Function bmx_tcodcolor_lightsea:Byte Ptr()
	Function bmx_tcodcolor_lightcyan:Byte Ptr()
	Function bmx_tcodcolor_lightsky:Byte Ptr()
	Function bmx_tcodcolor_lightblue:Byte Ptr()
	Function bmx_tcodcolor_lightviolet:Byte Ptr()
	Function bmx_tcodcolor_lightmagenta:Byte Ptr()
	Function bmx_tcodcolor_lightpink:Byte Ptr()
	Function bmx_tcodcolor_desaturatedred:Byte Ptr()
	Function bmx_tcodcolor_desaturatedorange:Byte Ptr()
	Function bmx_tcodcolor_desaturatedyellow:Byte Ptr()
	Function bmx_tcodcolor_desaturatedchartreuse:Byte Ptr()
	Function bmx_tcodcolor_desaturatedgreen:Byte Ptr()
	Function bmx_tcodcolor_desaturatedsea:Byte Ptr()
	Function bmx_tcodcolor_desaturatedcyan:Byte Ptr()
	Function bmx_tcodcolor_desaturatedsky:Byte Ptr()
	Function bmx_tcodcolor_desaturatedblue:Byte Ptr()
	Function bmx_tcodcolor_desaturatedviolet:Byte Ptr()
	Function bmx_tcodcolor_desaturatedmagenta:Byte Ptr()
	Function bmx_tcodcolor_desaturatedpink:Byte Ptr()
	Function bmx_tcodcolor_silver:Byte Ptr()
	Function bmx_tcodcolor_gold:Byte Ptr()

	Function bmx_tcodconsole_initroot:Byte Ptr(handle:Object, w:Int, h:Int, title:String, fullscreen:Int)
	Function bmx_tcodconsole_free(handle:Byte Ptr)
	Function bmx_tcodconsole_setcustomfont(fontFile:String, flags:Int, nbCharHoriz:Int, nbCharVertic:Int)
	Function bmx_tcodconsole_iswindowclosed:Int()
	Function bmx_tcodconsole_flush()
	Function bmx_tcodconsole_checkforkeypress:Byte Ptr()
	Function bmx_tcodconsole_setforegroundcolor(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_tcodconsole_printright(handle:Byte Ptr, x:Int, y:Int, text:String)
	Function bmx_tcodconsole_printleft(handle:Byte Ptr, x:Int, y:Int, text:String)
	Function bmx_tcodconsole_create:Byte Ptr(handle:Object, width:Int, height:Int)
	Function bmx_tcodconsole_isfullscreen:Int()
	Function bmx_tcodconsole_clear(handle:Byte Ptr)
	Function bmx_tcodconsole_getforegroundcolor:Byte Ptr(handle:Byte Ptr)
	Function bmx_tcodconsole_getbackgroundcolor:Byte Ptr(handle:Byte Ptr)
	Function bmx_tcodconsole_setbackgroundcolor(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_tcodconsole_setbackgroundflag(handle:Byte Ptr, flag:Int)
	Function bmx_tcodconsole_setforegroundflag(handle:Byte Ptr, flag:Int)
	Function bmx_tcodconsole_setfore(handle:Byte Ptr, x:Int, y:Int, col:Byte Ptr)
	Function bmx_tcodconsole_getfore:Byte Ptr(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodconsole_setback(handle:Byte Ptr, x:Int, y:Int, col:Byte Ptr)
	Function bmx_tcodconsole_getback:Byte Ptr(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodconsole_printcenter(handle:Byte Ptr, x:Int, y:Int, text:String)
	Function bmx_tcodconsole_printrightrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_tcodconsole_printleftrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_tcodconsole_printcenterrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_tcodconsole_putchar(handle:Byte Ptr, x:Int, y:Int, c:Int, fore:Byte Ptr, back:Byte Ptr)
	Function bmx_tcodconsole_brushchar(handle:Byte Ptr, x:Int, y:Int, c:Int)
	Function bmx_tcodconsole_blit(source:Byte Ptr, x:Int, y:Int, width:Int, height:Int, dest:Byte Ptr, posX:Int, posY:Int, fade:Int)
	Function bmx_tcodconsole_setchar(handle:Byte Ptr, x:Int, y:Int, c:Int)
	Function bmx_tcodconsole_rect(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, clear:Int)
	Function bmx_tcodconsole_credits()
	Function bmx_tcodconsole_rendercredits:Int(x:Int, y:Int, alpha:Int)
	Function bmx_tcodconsole_setfullscreen(fullscreen:Int)
	Function bmx_tcodconsole_getchar:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodconsole_getheightleftrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_tcodconsole_getheightcenterrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_tcodconsole_getheightrightrect:Int(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)

	Function bmx_tcodsystem_getelapsedmilli:Int()
	Function bmx_tcodsystem_getelapsedseconds:Float()
	Function bmx_tcodsystem_sleepmilli(value:Int)
	Function bmx_tcodsystem_setfps(value:Int)
	Function bmx_tcodsystem_getfps:Int()
	Function bmx_tcodsystem_getlastframelength:Float()
	Function bmx_tcodsystem_getcurrentresolution(w:Int Ptr, h:Int Ptr)
	Function bmx_tcodsystem_getcolsandascii(con:Byte Ptr, x:Int, y:Int, index:Int Ptr, ..
						br:Int Ptr, bg:Int Ptr, bb:Int Ptr, fr:Int Ptr, fg:Int Ptr, fb:Int Ptr, flags:Int)

	Function TCOD_sys_console_to_bitmap(screen:Byte Ptr, data:Byte Ptr)
	Function TCOD_console_get_width(handle:Byte Ptr)
	Function TCOD_console_get_height(handle:Byte Ptr)

	Function bmx_tcodrandom_getinstance:Byte Ptr()
	Function bmx_tcodrandom_getint:Int(handle:Byte Ptr, _min:Int, _max:Int)
	Function bmx_tcodrandom_getfloat:Float(handle:Byte Ptr, _min:Float, _max:Float)
	Function bmx_tcodrandom_create:Byte Ptr(seed:Int, useSeed:Int)
	Function bmx_tcodrandom_delete(handle:Byte Ptr)

	Function bmx_tcodline_init(xFrom:Int, yFrom:Int, xTo:Int, yTo:Int)
	Function bmx_tcodline_dostep:Int(xCur:Int Ptr, yCur:Int Ptr)
	Function TCOD_line:Int(xFrom:Int, yFrom:Int, xTo:Int, yTo:Int, listener:Int(x:Int, y:Int))

	Function bmx_tcodmap_create:Byte Ptr(width:Int, height:Int)
	Function bmx_tcodmap_setproperties(handle:Byte Ptr, x:Int, y:Int, isTransparent:Int, isWalkable:Int)
	Function bmx_tcodmap_clear(handle:Byte Ptr)
	Function bmx_tcodmap_copy(handle:Byte Ptr, source:Byte Ptr)
	Function bmx_tcodmap_computefov(handle:Byte Ptr, playerX:Int, playerY:Int, maxRadius:Int, lightWalls:Int, algo:Int)
	Function bmx_tcodmap_isinfov:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodmap_istransparent:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodmap_iswalkable:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodmap_delete(handle:Byte Ptr)

	Function bmx_tcodpath_create:Byte Ptr(map:Byte Ptr, diagonalCost:Float)
	Function bmx_tcodpath_compute:Int(handle:Byte Ptr, ox:Int, oy:Int, dx:Int, dy:Int)
	Function bmx_tcodpath_getorigin(handle:Byte Ptr, x:Int Ptr, y:Int Ptr)
	Function bmx_tcodpath_getdestination(handle:Byte Ptr, x:Int Ptr, y:Int Ptr)
	Function bmx_tcodpath_size:Int(handle:Byte Ptr)
	Function bmx_tcodpath_get(handle:Byte Ptr, index:Int, x:Int Ptr, y:Int Ptr)
	Function bmx_tcodpath_isempty:Int(handle:Byte Ptr)
	Function bmx_tcodpath_walk:Int(handle:Byte Ptr, x:Int Ptr, y:Int Ptr, recalculateWhenNeeded:Int)
	Function bmx_tcodpath_delete(handle:Byte Ptr)

	Function bmx_tcodnoise_create:Byte Ptr(dimensions:Int, hurst:Float, lacunarity:Float, generator:Byte Ptr)
	Function bmx_tcodnoise_delete(handle:Byte Ptr)
	Function bmx_tcodnoise_getperlin:Float(handle:Byte Ptr, f:Float[])
	Function bmx_tcodnoise_getfbmperlin:Float(handle:Byte Ptr, f:Float[], octaves:Float)
	Function bmx_tcodnoise_getturbulenceperlin:Float(handle:Byte Ptr, f:Float[], octaves:Float)
	Function bmx_tcodnoise_getsimplex:Float(handle:Byte Ptr, f:Float[])
	Function bmx_tcodnoise_getfbmsimplex:Float(handle:Byte Ptr, f:Float[], octaves:Float)
	Function bmx_tcodnoise_getturbulencesimplex:Float(handle:Byte Ptr, f:Float[], octaves:Float)
	Function bmx_tcodnoise_getwavelet:Float(handle:Byte Ptr, f:Float[])
	Function bmx_tcodnoise_getfbmwavelet:Float(handle:Byte Ptr, f:Float[], octaves:Float)
	Function bmx_tcodnoise_getturbulencewavelet:Float(handle:Byte Ptr, f:Float[], octaves:Float)

	Function bmx_tcodimage_createfromfile:Byte Ptr(filename:String)
	Function bmx_tcodimage_setkeycolor(handle:Byte Ptr, keyColor:Byte Ptr)
	Function bmx_tcodimage_blit(handle:Byte Ptr, image:Byte Ptr, x:Float, y:Float, scalex:Float, scaley:Float, angle:Float, op:Int)
	Function bmx_tcodimage_blitrect(handle:Byte Ptr, image:Byte Ptr, x:Int, y:Int, w:Int, h:Int, op:Int)
	Function bmx_tcodimage_blit2x(handle:Byte Ptr, console:Byte Ptr, dx:Int, dy:Int, sx:Int = 0, sy:Int = 0, w:Int = -1, h:Int = -1)
	Function bmx_tcodimage_clear(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_tcodimage_getsize(handle:Byte Ptr, w:Int Ptr, h:Int Ptr)
	Function bmx_tcodimage_getpixel:Byte Ptr(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodimage_getalpha:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodimage_ispixeltransparent:Int(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodimage_getmipmappixel:Byte Ptr(handle:Byte Ptr, x0:Float, y0:Float, x1:Float, y1:Float)
	Function bmx_tcodimage_putpixel(handle:Byte Ptr, x:Int, y:Int, col:Byte Ptr)
	Function bmx_tcodimage_scale(handle:Byte Ptr, newW:Int, newH:Int)
	Function bmx_tcodimage_hflip(handle:Byte Ptr)
	Function bmx_tcodimage_vflip(handle:Byte Ptr)
	Function bmx_tcodimage_invert(handle:Byte Ptr)
	Function bmx_tcodimage_free(handle:Byte Ptr)
	Function bmx_tcodimage_create:Byte Ptr(width:Int, height:Int)

	Function bmx_tcodmouse_getstatus:Object()
	Function bmx_tcodmouse_move(x:Int, y:Int)
	Function bmx_tcodmouse_showcursor(visible:Int)

	Function bmx_tcodwidget_setbackgroundcolor(col:Byte Ptr, colFocus:Byte Ptr)
	Function bmx_tcodwidget_setforegroundcolor(col:Byte Ptr, colFocus:Byte Ptr)
	Function bmx_tcodwidget_setconsole(con:Byte Ptr)
	Function bmx_tcodwidget_updatewidgets(vk:Int, c:Int, pressed:Int, lalt:Int, lctrl:Int, ralt:Int, rctrl:Int, shift:Int)
	Function bmx_tcodwidget_renderwidgets()
	Function bmx_tcodwidget_move(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodwidget_settip(handle:Byte Ptr, tip:String)
	Function bmx_tcodwidget_setvisible(handle:Byte Ptr, visible:Int)
	Function bmx_tcodwidget_isvisible:Int(handle:Byte Ptr)
	Function bmx_tcodwidget_focus:Object()

	Function bmx_tcodbutton_create:Byte Ptr(handle:Object, label:String, tip:String, x:Int, y:Int, w:Int, h:Int)
	Function bmx_tcodstatusbar_create:Byte Ptr(handle:Object, x:Int, y:Int, w:Int, h:Int)
	Function bmx_tcodvbox_create:Byte Ptr(handle:Object, x:Int, y:Int, padding:Int)

	Function bmx_tcodlabel_create:Byte Ptr(handle:Object, x:Int, y:Int, label:String, tip:String)
	Function bmx_tcodlabel_setvalue(handle:Byte Ptr, label:String)

	Function bmx_tcodcontainer_addwidget(handle:Byte Ptr, wid:Byte Ptr)
	Function bmx_tcodcontainer_removewidget(handle:Byte Ptr, wid:Byte Ptr)

	Function bmx_tcodtoolbar_create:Byte Ptr(handle:Object, x:Int, y:Int, name:String, tip:String)
	Function bmx_tcodtoolbar_setname(handle:Byte Ptr, name:String)
	Function bmx_tcodtoolbar_addseparator(handle:Byte Ptr, txt:String, tip:String)

	Function bmx_tcodbsp_create:Byte Ptr(x:Int, y:Int, w:Int, h:Int)
	Function bmx_tcodbsp_splitonce(handle:Byte Ptr, horizontal:Int, position:Int)
	Function bmx_tcodbsp_splitrecursive(handle:Byte Ptr, randomizer:Byte Ptr, nb:Int, minHSize:Int, maxHRatio:Int, minVSize:Int, maxVRatio:Int)
	Function bmx_tcodbsp_resize(handle:Byte Ptr, x:Int, y:Int, w:Int, h:Int)
	Function bmx_tcodbsp_removechildren(handle:Byte Ptr)
	Function bmx_tcodbsp_free(handle:Byte Ptr)
	Function bmx_tcodbsp_update(handle:Byte Ptr, x:Int Ptr, y:Int Ptr, w:Int Ptr, h:Int Ptr, position:Int Ptr, horizontal:Int Ptr, level:Int Ptr)
	Function bmx_tcodbsp_getleft:Object(handle:Byte Ptr)
	Function bmx_tcodbsp_getright:Object(handle:Byte Ptr)
	Function bmx_tcodbsp_getparent:Object(handle:Byte Ptr)
	Function bmx_tcodbsp_isleaf:Int(handle:Byte Ptr)
	Function bmx_tcodbsp_contains:Int(handle:Byte Ptr, cx:Int, cy:Int)
	Function bmx_tcodbsp_findnode:Object(handle:Byte Ptr, cx:Int, cy:Int)
	Function bmx_tcodbsp_traversepreorder:Int(handle:Byte Ptr)
	Function bmx_tcodbsp_traverseinorder:Int(handle:Byte Ptr)
	Function bmx_tcodbsp_traversepostorder:Int(handle:Byte Ptr)
	Function bmx_tcodbsp_traverselevelorder:Int(handle:Byte Ptr)
	Function bmx_tcodbsp_traverseinvertedlevelorder:Int(handle:Byte Ptr)

	Function bmx_tcodheightmap_create:Byte Ptr(w:Int, h:Int)
	Function bmx_tcodheightmap_setvalue(handle:Byte Ptr, x:Int, y:Int, v:Float)
	Function bmx_tcodheightmap_add(handle:Byte Ptr, value:Float)
	Function bmx_tcodheightmap_scale(handle:Byte Ptr, value:Float)
	Function bmx_tcodheightmap_clear(handle:Byte Ptr)
	Function bmx_tcodheightmap_clamp(handle:Byte Ptr, minimum:Float, maximum:Float)
	Function bmx_tcodheightmap_copy(handle:Byte Ptr, source:Byte Ptr)
	Function bmx_tcodheightmap_normalize(handle:Byte Ptr, minimum:Float, maximum:Float)
	Function bmx_tcodheightmap_lerp(handle:Byte Ptr, a:Byte Ptr, b:Byte Ptr, coef:Float)
	Function bmx_tcodheightmap_addmaps(handle:Byte Ptr, a:Byte Ptr, b:Byte Ptr)
	Function bmx_tcodheightmap_multiplymaps(handle:Byte Ptr, a:Byte Ptr, b:Byte Ptr)
	Function bmx_tcodheightmap_addhill(handle:Byte Ptr, x:Float, y:Float, radius:Float, height:Float)
	Function bmx_tcodheightmap_dighill(handle:Byte Ptr, hx:Float, hy:Float, hradius:Float, height:Float)
	Function bmx_tcodheightmap_rainerosion(handle:Byte Ptr, nbDrops:Int, erosionCoef:Float, sedimentationCoef:Float, generator:Byte Ptr)
	Function bmx_tcodheightmap_kerneltransform(handle:Byte Ptr, dx:Int[], dy:Int[], weight:Float[], minLevel:Float , maxLevel:Float)
	Function bmx_tcodheightmap_addvoronoi(handle:Byte Ptr, nbPoints:Int, nbCoef:Int, coef:Float[], generator:Byte Ptr)
	Function bmx_tcodheightmap_addfbm(handle:Byte Ptr, noise:Byte Ptr, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
	Function bmx_tcodheightmap_scalefbm(handle:Byte Ptr, noise:Byte Ptr, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
	Function bmx_tcodheightmap_digbezier(handle:Byte Ptr, px:Int[], py:Int[], startRadius:Float, startDepth:Float, endRadius:Float, endDepth:Float)
	Function bmx_tcodheightmap_getvalue:Float(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodheightmap_getinterpolatedvalue:Float(handle:Byte Ptr, x:Float, y:Float)
	Function bmx_tcodheightmap_getslope:Float(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_tcodheightmap_getnormal:Int[](handle:Byte Ptr, x:Float, y:Float, waterLevel:Float)
	Function bmx_tcodheightmap_countcells:Int(handle:Byte Ptr, minimum:Float, maximum:Float)
	Function bmx_tcodheightmap_haslandonborder:Int(handle:Byte Ptr, waterLevel:Float)
	Function bmx_tcodheightmap_getminmax(handle:Byte Ptr, minimum:Float Ptr, maximum:Float Ptr)
	Function bmx_tcodheightmap_free(handle:Byte Ptr)

End Extern

Const TCOD_CHAR_HLINE:Int = 196
Const TCOD_CHAR_VLINE:Int = 179
Const TCOD_CHAR_NE:Int = 191
Const TCOD_CHAR_NW:Int = 218
Const TCOD_CHAR_SE:Int = 217
Const TCOD_CHAR_SW:Int = 192
Const TCOD_CHAR_DHLINE:Int = 205
Const TCOD_CHAR_DVLINE:Int = 186
Const TCOD_CHAR_DNE:Int = 187
Const TCOD_CHAR_DNW:Int = 201
Const TCOD_CHAR_DSE:Int = 188
Const TCOD_CHAR_DSW:Int = 200
Const TCOD_CHAR_TEEW:Int = 180
Const TCOD_CHAR_TEEE:Int = 195
Const TCOD_CHAR_TEEN:Int = 193
Const TCOD_CHAR_TEES:Int = 194
Const TCOD_CHAR_DTEEW:Int = 181
Const TCOD_CHAR_DTEEE:Int = 198
Const TCOD_CHAR_DTEEN:Int = 208
Const TCOD_CHAR_DTEES:Int = 210
Const TCOD_CHAR_CHECKER:Int = 178
Const TCOD_CHAR_BLOCK:Int = 219
Const TCOD_CHAR_BLOCK1:Int = 178
Const TCOD_CHAR_BLOCK2:Int = 177
Const TCOD_CHAR_BLOCK3:Int = 176
Const TCOD_CHAR_BLOCK_B:Int = 220
Const TCOD_CHAR_BLOCK_T:Int = 223
Const TCOD_CHAR_DS_CROSSH:Int = 216
Const TCOD_CHAR_DS_CROSSV:Int = 215
Const TCOD_CHAR_CROSS:Int = 197
Const TCOD_CHAR_LIGHT:Int = 15
Const TCOD_CHAR_TREE:Int = 5
Const TCOD_CHAR_ARROW_N:Int = 24
Const TCOD_CHAR_ARROW_S:Int = 25
Const TCOD_CHAR_ARROW_E:Int = 26
Const TCOD_CHAR_ARROW_W:Int = 27

Const TCOD_COLCTRL_1:Int = 1
Const TCOD_COLCTRL_2:Int = 2
Const TCOD_COLCTRL_3:Int = 3
Const TCOD_COLCTRL_4:Int = 4
Const TCOD_COLCTRL_5:Int = 5
Const TCOD_COLCTRL_NUMBER:Int = 5
Const TCOD_COLCTRL_FORE_RGB:Int = 6
Const TCOD_COLCTRL_BACK_RGB:Int = 7
Const TCOD_COLCTRL_STOP:Int = 8

Const TCOD_BKGND_NONE:Int = 0
Const TCOD_BKGND_SET:Int = 1
Const TCOD_BKGND_MULTIPLY:Int = 2
Const TCOD_BKGND_LIGHTEN:Int = 3
Const TCOD_BKGND_DARKEN:Int = 4
Const TCOD_BKGND_SCREEN:Int = 5
Const TCOD_BKGND_COLOR_DODGE:Int = 6
Const TCOD_BKGND_COLOR_BURN:Int = 7
Const TCOD_BKGND_ADD:Int = 8
Const TCOD_BKGND_ADDA:Int = 9
Const TCOD_BKGND_BURN:Int = 10
Const TCOD_BKGND_OVERLAY:Int = 11
Const TCOD_BKGND_ALPH:Int = 12

Const TCOD_FONT_LAYOUT_ASCII_INCOL:Int = 1
Const TCOD_FONT_LAYOUT_ASCII_INROW:Int = 2
Const TCOD_FONT_TYPE_GREYSCALE:Int = 4
Const TCOD_FONT_TYPE_GRAYSCALE:Int = 4
Const TCOD_FONT_LAYOUT_TCOD:Int = 8

Const TCODK_NONE:Int = 0
Const TCODK_ESCAPE:Int = 1
Const TCODK_BACKSPACE:Int = 2
Const TCODK_TAB:Int = 3
Const TCODK_ENTER:Int = 4
Const TCODK_SHIFT:Int = 5
Const TCODK_CONTROL:Int = 6
Const TCODK_ALT:Int = 7
Const TCODK_PAUSE:Int = 8
Const TCODK_CAPSLOCK:Int = 9
Const TCODK_PAGEUP:Int = 10
Const TCODK_PAGEDOWN:Int = 11
Const TCODK_END:Int = 12
Const TCODK_HOME:Int = 13
Const TCODK_UP:Int = 14
Const TCODK_LEFT:Int = 15
Const TCODK_RIGHT:Int = 16
Const TCODK_DOWN:Int = 17
Const TCODK_PRINTSCREEN:Int = 18
Const TCODK_INSERT:Int = 19
Const TCODK_DELETE:Int = 20
Const TCODK_LWIN:Int = 21
Const TCODK_RWIN:Int = 22
Const TCODK_APPS:Int = 23
Const TCODK_0:Int = 24
Const TCODK_1:Int = 25
Const TCODK_2:Int = 26
Const TCODK_3:Int = 27
Const TCODK_4:Int = 28
Const TCODK_5:Int = 29
Const TCODK_6:Int = 30
Const TCODK_7:Int = 31
Const TCODK_8:Int = 32
Const TCODK_9:Int = 33
Const TCODK_KP0:Int = 34
Const TCODK_KP1:Int = 35
Const TCODK_KP2:Int = 36
Const TCODK_KP3:Int = 37
Const TCODK_KP4:Int = 38
Const TCODK_KP5:Int = 39
Const TCODK_KP6:Int = 40
Const TCODK_KP7:Int = 41
Const TCODK_KP8:Int = 42
Const TCODK_KP9:Int = 43
Const TCODK_KPADD:Int = 44
Const TCODK_KPSUB:Int = 45
Const TCODK_KPDIV:Int = 46
Const TCODK_KPMUL:Int = 47
Const TCODK_KPDEC:Int = 48
Const TCODK_KPENTER:Int = 49
Const TCODK_F1:Int = 50
Const TCODK_F2:Int = 51
Const TCODK_F3:Int = 52
Const TCODK_F4:Int = 53
Const TCODK_F5:Int = 54
Const TCODK_F6:Int = 55
Const TCODK_F7:Int = 56
Const TCODK_F8:Int = 57
Const TCODK_F9:Int = 58
Const TCODK_F10:Int = 59
Const TCODK_F11:Int = 60
Const TCODK_F12:Int = 61
Const TCODK_NUMLOCK:Int = 62
Const TCODK_SCROLLLOCK:Int = 63
Const TCODK_SPACE:Int = 64
Const TCODK_CHAR:Int = 65


Const FOV_BASIC:Int = 0
Const FOV_DIAMOND:Int = 1
Const FOV_SHADOW:Int = 2
Const FOV_PERMISSIVE_0:Int = 3
Const FOV_PERMISSIVE_1:Int = 4
Const FOV_PERMISSIVE_2:Int = 5
Const FOV_PERMISSIVE_3:Int = 6
Const FOV_PERMISSIVE_4:Int = 7
Const FOV_PERMISSIVE_5:Int = 8
Const FOV_PERMISSIVE_6:Int = 9
Const FOV_PERMISSIVE_7:Int = 10
Const FOV_PERMISSIVE_8:Int = 11
Const NB_FOV_ALGORITHMS:Int = FOV_PERMISSIVE_8

Rem
bbdoc: 
End Rem
Const TCOD_NOISE_MAX_OCTAVES:Int = 128	
Rem
bbdoc: 
End Rem
Const TCOD_NOISE_MAX_DIMENSIONS:Int = 4
Rem
bbdoc: 
End Rem
Const TCOD_NOISE_DEFAULT_HURST:Float = 0.5
Rem
bbdoc: 
End Rem
Const TCOD_NOISE_DEFAULT_LACUNARITY:Float = 2.0
