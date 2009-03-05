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

Import "random.bmx"
Import "common.bmx"

Rem
bbdoc: Provides several functions to generate Perlin noise and other derived noises.
about: It can handle noise functions from 1 to 4 dimensions.
End Rem
Type TCODNoise

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a noise generator from a number of dimensions (from 1 to 4), some fractal parameters and an optional random number generator.
	End Rem
	Function CreateNoise:TCODNoise(dimensions:Int, hurst:Float = TCOD_NOISE_DEFAULT_HURST, lacunarity:Float = TCOD_NOISE_DEFAULT_LACUNARITY, generator:TCODRandom = Null)
		Return New TCODNoise.Create(dimensions, hurst, lacunarity, generator)
	End Function

	Rem
	bbdoc: Creates a noise generator from a number of dimensions (from 1 to 4), some fractal parameters and an optional random number generator.
	End Rem
	Method Create:TCODNoise(dimensions:Int, hurst:Float = TCOD_NOISE_DEFAULT_HURST, lacunarity:Float = TCOD_NOISE_DEFAULT_LACUNARITY, generator:TCODRandom = Null)
		If generator Then
			objectPtr = bmx_tcodnoise_create(dimensions, hurst, lacunarity, generator.randomPtr)
		Else
			objectPtr = bmx_tcodnoise_create(dimensions, hurst, lacunarity, Null)
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the perlin noise function value between -1.0 and 1.0 at given coordinates.
	End Rem
	Method GetPerlin:Float(f:Float[])
		Return bmx_tcodnoise_getperlin(objectPtr, f)
	End Method
	
	Rem
	bbdoc: Returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	End Rem
	Method GetFbmPerlin:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getfbmperlin(objectPtr, f, octaves)
	End Method
	
	Rem
	bbdoc: Returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	End Rem
	Method GetTurbulencePerlin:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getturbulenceperlin(objectPtr, f, octaves)
	End Method

	Rem
	bbdoc: Returns the simplex noise function value between -1.0 and 1.0 at given coordinates.
	about: The simplex noise is much faster than Perlin, especially in 4 dimensions. It has a better contrast too.
	End Rem
	Method GetSimplex:Float(f:Float[])
		Return bmx_tcodnoise_getsimplex(objectPtr, f)
	End Method
	
	Rem
	bbdoc: Returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	about: The simplex noise is much faster than Perlin, especially in 4 dimensions. It has a better contrast too.
	End Rem
	Method GetFbmSimplex:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getfbmsimplex(objectPtr, f, octaves)
	End Method
	
	Rem
	bbdoc: Returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	about: The simplex noise is much faster than Perlin, especially in 4 dimensions. It has a better contrast too.
	End Rem
	Method GetTurbulenceSimplex:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getturbulencesimplex(objectPtr, f, octaves)
	End Method

	Rem
	bbdoc: Returns the wavelet noise function value between -1.0 and 1.0 at given coordinates.
	about: The wavelet noise is much slower than Perlin, and can only be computed in 1,2 or 3 dimensions. But it doesn't blur at high scales.
	End Rem
	Method GetWavelet:Float(f:Float[])
		Return bmx_tcodnoise_getwavelet(objectPtr, f)
	End Method
	
	Rem
	bbdoc: Returns the fbm function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	about: The wavelet noise is much slower than Perlin, and can only be computed in 1,2 or 3 dimensions. But it doesn't blur at high scales.
	End Rem
	Method GetFbmWavelet:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getfbmwavelet(objectPtr, f, octaves)
	End Method
	
	Rem
	bbdoc: Returns the turbulence function value between -1.0 and 1.0 at given coordinates, using fractal hurst and lacunarity defined when the generator was created.
	about: The wavelet noise is much slower than Perlin, and can only be computed in 1,2 or 3 dimensions. But it doesn't blur at high scales.
	End Rem
	Method GetTurbulenceWavelet:Float(f:Float[], octaves:Float)
		Return bmx_tcodnoise_getturbulencewavelet(objectPtr, f, octaves)
	End Method
	
	Rem
	bbdoc: Releases resources used by the noise generator.
	End Rem
	Method Free()
		If objectPtr Then
			bmx_tcodnoise_delete(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type


