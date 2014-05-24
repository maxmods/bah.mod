' Copyright (c) 2014 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import "source.bmx"


Extern

	Function bmx_glslopt_initialize:Byte Ptr(target:Int)
	Function bmx_glslopt_cleanup(handle:Byte Ptr)
	Function bmx_glslopt_set_max_unroll_iterations(handle:Byte Ptr, iterations:Int)
	Function bmx_glslopt_optimize:Byte Ptr(handle:Byte Ptr, shaderType:Int, source:String, options:Int)

	Function bmx_glslopt_get_status:Int(handle:Byte Ptr)
	Function bmx_glslopt_get_output:String(handle:Byte Ptr)
	Function bmx_glslopt_get_raw_output:String(handle:Byte Ptr)
	Function bmx_glslopt_get_log:String(handle:Byte Ptr)
	Function bmx_glslopt_shader_get_input_count:Int(handle:Byte Ptr)
	Function bmx_glslopt_shader_get_input_name:String(handle:Byte Ptr, index:Int)
	Function bmx_glslopt_shader_get_stats(handle:Byte Ptr, approxMath:Int Ptr, approxTex:Int Ptr, approxFlow:Int Ptr)
	Function bmx_glslopt_shader_delete(handle:Byte Ptr)

End Extern


Const SHADERTARGET_OPENGL:Int = 0
Const SHADERTARGET_OPENGLES20:Int = 1
Const SHADERTARGET_OPENGLES30:Int = 2
	
Const SHADERTYPE_VERTEX:Int = 0
Const SHADERTYPE_FRAGMENT:Int = 1

Const SHADEROPTION_SKIPPREPROCESSOR:Int = 1 Shl 0
Const SHADEROPTION_NOTFULLSHADER:Int = 1 Shl 1
