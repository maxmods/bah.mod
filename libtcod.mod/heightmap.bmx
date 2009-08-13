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

Import "noise.bmx"
Import "random.bmx"
Import "common.bmx"

Rem
bbdoc: A 2D heightmap grid of float values.
End Rem
Type TCODHeightMap

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a height map of the given dimensions.
	End Rem
	Function CreateHeightMap:TCODHeightMap(w:Int, h:Int)
		Return New TCODHeightMap.Create(w, h)
	End Function
	
	Rem
	bbdoc: Creates a height map of the given dimensions.
	End Rem
	Method Create:TCODHeightMap(w:Int, h:Int)
		objectPtr = bmx_tcodheightmap_create(w, h)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetValue(x:Int, y:Int, v:Float)
		bmx_tcodheightmap_setvalue(objectPtr, x, y, v)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Add(value:Float)
		bmx_tcodheightmap_add(objectPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Scale(value:Float)
		bmx_tcodheightmap_scale(objectPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Clear()
		bmx_tcodheightmap_clear(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Clamp(minimum:Float, maximum:Float)
		bmx_tcodheightmap_clamp(objectPtr, minimum, maximum)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Copy(source:TCODHeightMap)
		bmx_tcodheightmap_copy(objectPtr, source.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Normalize(minimum:Float = 0.0, maximum:Float = 1.0)
		bmx_tcodheightmap_normalize(objectPtr, minimum, maximum)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Lerp(a:TCODHeightMap, b:TCODHeightMap, coef:Float)
		bmx_tcodheightmap_lerp(objectPtr, a.objectPtr, b.objectPtr, coef)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddMaps(a:TCODHeightMap, b:TCODHeightMap)
		bmx_tcodheightmap_addmaps(objectPtr, a.objectPtr, b.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MultiplyMaps(a:TCODHeightMap, b:TCODHeightMap)
		bmx_tcodheightmap_multiplymaps(objectPtr, a.objectPtr, b.objectPtr)
	End Method
	
	Rem
	bbdoc: Adds a hill (a half spheroid) at given position.
	End Rem
	Method AddHill(x:Float, y:Float, radius:Float, height:Float)
		bmx_tcodheightmap_addhill(objectPtr, x, y, radius, height)
	End Method
	
	Rem
	bbdoc: Takes the highest value (if height > 0) or the lowest (if height < 0) between the map and the hill.
	about: It's main goal is to carve things in maps (like rivers) by digging hills along a curve.
	End Rem
	Method DigHill(hx:Float, hy:Float, hradius:Float, height:Float)
		bmx_tcodheightmap_dighill(objectPtr, hx, hy, hradius, height)
	End Method
	
	Rem
	bbdoc: Simulates the effect of rain drops on the terrain, resulting in erosion patterns.
	End Rem
	Method RainErosion(nbDrops:Int, erosionCoef:Float, sedimentationCoef:Float, generator:TCODRandom = Null)
		If generator Then
			bmx_tcodheightmap_rainerosion(objectPtr, nbDrops, erosionCoef, sedimentationCoef, generator.randomPtr)
		Else
			bmx_tcodheightmap_rainerosion(objectPtr, nbDrops, erosionCoef, sedimentationCoef, Null)
		End If
	End Method
	
	Rem
	bbdoc: Allows you to apply a generic transformation on the map, so that each resulting cell value is the weighted sum of several neighbour cells.
	about: This can be used to smooth/sharpen the map.
	End Rem
	Method KernelTransform(dx:Int[], dy:Int[], weight:Float[], minLevel:Float , maxLevel:Float)
		bmx_tcodheightmap_kerneltransform(objectPtr, dx, dy, weight, minLevel, maxLevel)
	End Method
	
	Rem
	bbdoc: Adds values from a Voronoi diagram to the map.
	End Rem
	Method AddVoronoi(nbPoints:Int, nbCoef:Int, coef:Float[], generator:TCODRandom = Null)
		If generator Then
			bmx_tcodheightmap_addvoronoi(objectPtr, nbPoints, nbCoef, coef, generator.randomPtr)
		Else
			bmx_tcodheightmap_addvoronoi(objectPtr, nbPoints, nbCoef, coef, Null)
		End If
	End Method
	
	Rem
	bbdoc: Adds values from a simplex fbm function to the map.
	End Rem
	Method AddFbm(noise:TCODNoise, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
		bmx_tcodheightmap_addfbm(objectPtr, noise.objectPtr, mulx, muly, addx, addy, octaves, delta, scale)
	End Method
	
	Rem
	bbdoc: Works like #AddFbm, but it multiplies the resulting value instead of adding it to the heightmap.
	End Rem
	Method ScaleFbm(noise:TCODNoise, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
		bmx_tcodheightmap_scalefbm(objectPtr, noise.objectPtr, mulx, muly, addx, addy, octaves, delta, scale)
	End Method
	
	Rem
	bbdoc: Carves a path along a cubic Bezier curve using the #DigHill function.
	about: Could be used for roads/rivers/...
	<p>
	Both radius and depth can vary linearly along the path.
	</p>
	End Rem
	Method DigBezier(px:Int[], py:Int[], startRadius:Float, startDepth:Float, endRadius:Float, endDepth:Float)
		bmx_tcodheightmap_digbezier(objectPtr, px, py, startRadius, startDepth, endRadius, endDepth)
	End Method
	
	Rem
	bbdoc: Returns the height value of a map cell.
	End Rem
	Method GetValue:Float(x:Int, y:Int)
		Return bmx_tcodheightmap_getvalue(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the interpolated height at non integer coordinates.
	End Rem
	Method GetInterpolatedValue:Float(x:Float, y:Float)
		Return bmx_tcodheightmap_getinterpolatedvalue(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the slope between 0 and PI/2 at given coordinates.
	End Rem
	Method GetSlope:Float(x:Int, y:Int)
		Return bmx_tcodheightmap_getslope(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the map normal at given coordinates.
	End Rem
	Method GetNormal:Int[](x:Float, y:Float, waterLevel:Float = 0.0)
		Return bmx_tcodheightmap_getnormal(objectPtr, x, y, waterLevel)
	End Method
	
	Rem
	bbdoc: Returns the number of map cells which value is between min and max.
	End Rem
	Method CountCells:Int(minimum:Float, maximum:Float)
		Return bmx_tcodheightmap_countcells(objectPtr, minimum, maximum)
	End Method
	
	Rem
	bbdoc: Checks if the cells on the map border are below a certain height.
	End Rem
	Method HasLandOnBorder:Int(waterLevel:Float)
		Return bmx_tcodheightmap_haslandonborder(objectPtr, waterLevel)
	End Method
	
	Rem
	bbdoc: Calculates the min and max of all values inside the map.
	End Rem
	Method GetMinMax(minimum:Float Var, maximum:Float Var)
		bmx_tcodheightmap_getminmax(objectPtr, Varptr minimum, Varptr maximum)
	End Method
	
	Rem
	bbdoc: Releases the resources used by a heightmap.
	End Rem
	Method Free()
		If objectPtr Then
			bmx_tcodheightmap_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


