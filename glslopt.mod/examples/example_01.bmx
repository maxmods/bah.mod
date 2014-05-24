SuperStrict

Framework BaH.GLSLOpt
Import BRL.StandardIO

DebugStop
Local inp:String = LoadText("../glslopt/tests/fragment/zun-Toon_Basic-in.txt")


Local opt:TGLSLOptimizer = TGLSLOptimizer.Create(SHADERTARGET_OPENGL)

Local shader:TGLSLShader = opt.Optimize(SHADERTYPE_FRAGMENT, inp, 0)

If shader.GetStatus() Then

	' original
	Print "Original :~n"
	Print inp


	' optimized
	Print "~nOptimized :~n"
	Print shader.GetOutput()

Else

	Print shader.GetLog()

End If