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
bbdoc: 
End Rem
Type TCODHeightMap

	Field objectPtr:Byte Ptr
	
	Function CreateHeightMap:TCODHeightMap(w:Int, h:Int)
	End Function
	
	Method Create:TCODHeightMap(w:Int, h:Int)
	End Method
	
	Method SetValue(x:Int, y:Int, v:Float)
	End Method
	
	Method Add(value:Float)
	End Method
	
	Method Scale(value:Float)
	End Method
	
	Method Clear()
	End Method
	
	Method Clamp(minimum:Float, maximum:Float)
	End Method
	
	Method Copy(source:TCODHeightMap)
	End Method
	
	Method Normalize(minimum:Float = 0.0, maxiumum:Float = 1.0)
	End Method
	
	Method Lerp(a:TCODHeightMap, b:TCODHeightMap, coef:Float)
	End Method
	
	Method AddMaps(a:TCODHeightMap, b:TCODHeightMap)
	End Method
	
	Method MultiplyMaps(a:TCODHeightMap, b:TCODHeightMap)
	End Method
	
	Method AddHill(x:Float, y:Float, radius:Float, height:Float)
	End Method
	
	Method DigHill(hx:Float, hy:Float, hradius:Float, height:Float)
	End Method
	
	Method RainErosion(nbDrops:Int, erosionCoef:Float, sedimentationCoef:Float, generator:TCODRandom = Null)
	End Method
	
	Method KernelTransform(kernelSize:Int, dx:Int[], dy:Int[], weight:Float[], minLevel:Float , maxLevel:Float)
	End Method
	
	Method AddVoronoi(nbPoints:Int, nbCoef:Int, coef:Float[], generator:TCODRandom = Null)
	End Method
	
	Method AddFbm(noise:TCODNoise, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
	End Method
	
	Method ScaleFbm(noise:TCODNoise, mulx:Float, muly:Float, addx:Float, addy:Float, octaves:Float, delta:Float, scale:Float)
	End Method
	
	Method DigBezier(px:Int[], py:Int[], startRadius:Float, startDepth:Float, endRadius:Float, endDepth:Float)
	End Method
	
	Method GetValue:Float(x:Int, y:Int)
	End Method
	
	Method GetInterpolatedValue:Float(x:Float, y:Float)
	End Method
	
	Method GetSlope:Float(x:Int, y:Int)
	End Method
	
	Method GetNormal:Int[](x:Float, y:Float, waterLevel:Float = 0.0)
	End Method
	
	Method CountCells:Int(minimum:Float, maximum:Float)
	End Method
	
	Method HasLandOnBorder:Int(waterLevel:Float)
	End Method
	
	Method GetMinMax(minimum:Float Var, maximum:Float Var)
	End Method
	
	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


