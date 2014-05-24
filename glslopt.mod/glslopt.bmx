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

Rem
bbdoc: A GLSL Optimizer
End Rem
Module BaH.GLSLOpt

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "Copyright: Wrapper - 2014 Bruce A Henderson"
ModuleInfo "Copyright: GLSLOptimizer - 2010-2013 Unity Technologies"

ModuleInfo "History: 1.00 Initial Release"

?macos
ModuleInfo "CC_OPTS: -O0"
?

Import "common.bmx"

Rem
bbdoc: An instance of the GLSL optimizer.
End Rem
Type TGLSLOptimizer

	Field ctxPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new GLSL optimizer instance for the given target.
	about: A target can be one of, SHADERTARGET_OPENGL, SHADERTARGET_OPENGLES20, or SHADERTARGET_OPENGLES30.
	End Rem
	Function Create:TGLSLOptimizer(target:Int)
		Local this:TGLSLOptimizer = New TGLSLOptimizer
		this.ctxPtr = bmx_glslopt_initialize(target)
		If this.ctxPtr Then
			Return this
		End If
		Return Null
	End Function
	
	Rem
	bbdoc: Sets the maximum number of unroll iterations to perform during optimization.
	End Rem
	Method SetMaxUnrollIterations(iterations:Int)
		bmx_glslopt_set_max_unroll_iterations(ctxPtr, iterations)
	End Method
	
	Rem
	bbdoc: Optmized the provided shader source, for the specific shader type, and given options.
	returns: a TGLSLShader object.
	about: A shaderType is one of SHADERTYPE_VERTEX or SHADERTYPE_FRAGMENT.
	Options can be an bitwise combination of SHADEROPTION_SKIPPREPROCESSOR, SHADEROPTION_NOTFULLSHADER, and 0.
	End Rem
	Method Optimize:TGLSLShader(shaderType:Int, source:String, options:Int)
		Return TGLSLShader._create(bmx_glslopt_optimize(ctxPtr, shaderType, source, options))
	End Method

	Rem
	bbdoc: Frees the optimizer instance.
	End Rem
	Method Free()
		If ctxPtr Then
			bmx_glslopt_cleanup(ctxPtr)
			ctxPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: An optimized shader.
End Rem
Type TGLSLShader

	Field shaderPtr:Byte Ptr
	
	Function _create:TGLSLShader(shaderPtr:Byte Ptr)
		If shaderPtr Then
			Local this:TGLSLShader = New TGLSLShader
			this.shaderPtr = shaderPtr
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Gets the shader optimization status.
	returns: 1 on success, 0 on error.
	about: Use #GetLog to check details of errors.
	End Rem
	Method GetStatus:Int()
		Return bmx_glslopt_get_status(shaderPtr)
	End Method
	
	Rem
	bbdoc: Returns the optimized output.
	End Rem
	Method GetOutput:String()
		Return bmx_glslopt_get_output(shaderPtr)
	End Method

	Rem
	bbdoc: Returns the raw output.
	End Rem
	Method GetRawOutput:String()
		Return bmx_glslopt_get_raw_output(shaderPtr)
	End Method
	
	Rem
	bbdoc: Returns the optimization log for this shader.
	End Rem
	Method GetLog:String()
		Return bmx_glslopt_get_log(shaderPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of inputs.
	End Rem
	Method GetInputCount:Int()
		Return bmx_glslopt_shader_get_input_count(shaderPtr)
	End Method
	
	Rem
	bbdoc: Returns the input name for the given index.
	End Rem
	Method GetInputName:String(index:Int)
		Return bmx_glslopt_shader_get_input_name(shaderPtr, index)
	End Method
	
	Rem
	bbdoc: Gets some statistics about the optimization.
	End Rem
	Method GetShaderStats(approxMath:Int Var, approxTex:Int Var, approxFlow:Int Var)
		bmx_glslopt_shader_get_stats(shaderPtr, Varptr approxMath, Varptr approxTex, Varptr approxFlow)
	End Method
	
	Rem
	bbdoc: Frees this optimized shader.
	End Rem
	Method Free()
		If shaderPtr Then
			bmx_glslopt_shader_delete(shaderPtr)
			shaderPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type
