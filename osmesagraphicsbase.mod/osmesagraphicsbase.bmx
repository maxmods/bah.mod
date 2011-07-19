
Strict

Rem
bbdoc: OSMesa Graphics Base
End Rem
Module BaH.OSMesaGraphicsBase

Rem
End Rem

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Mark Sibly and Bruce A Henderson"
ModuleInfo "License: Blitz Shared Source Code"
ModuleInfo "Copyright: Blitz Research Ltd"

ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: OSMesa port from original BRL.Graphics module"



Const GRAPHICS_BACKBUFFER=	$2
Const GRAPHICS_ALPHABUFFER=	$4
Const GRAPHICS_DEPTHBUFFER=	$8
Const GRAPHICS_STENCILBUFFER=	$10
Const GRAPHICS_ACCUMBUFFER=	$20

Type TGraphics

	Method _pad()
	End Method

	Method Driver:TGraphicsDriver() Abstract

	Method GetSettings( width Var,height Var,depth Var,hertz Var,flags Var ) Abstract

	Method Close() Abstract
	
End Type

Type TGraphicsMode

	Field width,height,depth,hertz
	
	Method ToString$()
		Return width+","+height+","+depth+" "+hertz+"Hz"
	End Method

End Type

Type TGraphicsDriver

	Method GraphicsModes:TGraphicsMode[]() Abstract
	
	Method AttachGraphics:TGraphics( widget,flags ) Abstract
	
	Method CreateGraphics:TGraphics( width,height,depth,hertz,flags ) Abstract
	
	Method SetGraphics( g:TGraphics ) Abstract
	
	Method Flip( sync ) Abstract
	
End Type

Private

Global _defaultFlags
Global _driver:TGraphicsDriver
Global _graphicsModes:TGraphicsMode[]
Global _graphics:TGraphics,_gWidth,_gHeight,_gDepth,_gHertz,_gFlags

Global _exGraphics:TGraphics

'Only valid if _exGraphics=_graphics
Global _softSync,_hardSync,_syncRate,_syncPeriod,_syncFrac,_syncAccum,_syncTime

Public

Global GraphicsSeq=1

Function BumpGraphicsSeq()
	GraphicsSeq:+1
	If Not GraphicsSeq GraphicsSeq=1
End Function

Function SetGraphicsDriver( driver:TGraphicsDriver,defaultFlags=GRAPHICS_BACKBUFFER )
	BumpGraphicsSeq
	If driver<>_driver
		If _driver And _graphics _driver.SetGraphics Null
		_graphicsModes=Null
		_driver=driver
	EndIf
	_defaultFlags=defaultFlags
	_graphics=Null
	_gWidth=0
	_gHeight=0
	_gDepth=0
	_gHertz=0
	_gFlags=0
End Function

Function GetGraphicsDriver:TGraphicsDriver()
	Return _driver
End Function

Function DefaultGraphicsFlags()
	Return _defaultFlags
End Function

Function GraphicsModes:TGraphicsMode[]()
	If Not _graphicsModes _graphicsModes=_driver.GraphicsModes()
	Return _graphicsModes[..]
End Function

Function CountGraphicsModes()
	Return GraphicsModes().length
End Function

Function GetGraphicsMode( index,width Var,height Var,depth Var,hertz Var )
	Local mode:TGraphicsMode=GraphicsModes()[index]
	width=mode.width
	height=mode.height
	depth=mode.depth
	hertz=mode.hertz
End Function

Function GraphicsModeExists( width,height,depth=0,hertz=0 )
	For Local mode:TGraphicsMode=EachIn GraphicsModes()
		If width And width<>mode.width Continue
		If height And height<>mode.height Continue
		If depth And depth<>mode.depth Continue
		If hertz And hertz<>mode.hertz Continue
		Return True
	Next
	Return False
End Function

Function CreateGraphics:TGraphics( width,height,depth,hertz,flags )
	flags:|_defaultFlags
	Local g:TGraphics
	Try
		g=_driver.CreateGraphics( width,height,depth,hertz,flags )
	Catch ex:Object
?Debug
		WriteStdout "CreateGraphics failed:"+ex.ToString()
?
	End Try
	Return g
End Function

Function AttachGraphics:TGraphics( widget,flags )
	flags:|_defaultFlags
	Local g:TGraphics
	Try
		g=_driver.AttachGraphics( widget,flags )
	Catch ex:Object
?Debug
		WriteStdout "AttachGraphics failed:"+ex.ToString()
?
	End Try
	Return g
End Function

Function CloseGraphics( g:TGraphics )
	If g=_exGraphics _exGraphics=Null
	If g=_graphics SetGraphics Null
	g.Close
End Function

Function SetGraphics( g:TGraphics )
	If Not g
		If _driver And _graphics _driver.SetGraphics Null
		_graphics=Null
		_gWidth=0
		_gHeight=0
		_gDepth=0
		_gHertz=0
		_gFlags=0
		Return
	EndIf
	Local d:TGraphicsDriver=g.Driver()
	If d<>_driver
		If _driver And _graphics _driver.SetGraphics Null
		_graphicsModes=Null
		_driver=d
	EndIf
	g.GetSettings _gWidth,_gHeight,_gDepth,_gHertz,_gFlags
	d.SetGraphics g
	_graphics=g
End Function

Function GraphicsWidth()
	Return _gWidth
End Function

Function GraphicsHeight()
	Return _gHeight
End Function

Function GraphicsDepth()
	Return _gDepth
End Function

Function GraphicsHertz()
	Return _gHertz
End Function

Function GraphicsFlags()
	Return _gFlags
End Function

Function Flip( sync=-1 )

	If sync<>-1
		_driver.Flip sync
		Return
	EndIf
	
	If _graphics<>_exGraphics
		_driver.Flip True
		Return
	EndIf

	If _softSync
		_syncTime:+_syncPeriod
		_syncAccum:+_syncFrac
		If _syncAccum>=_syncRate
			_syncAccum:-_syncRate
			_syncTime:+1
		EndIf
		Local dt=_syncTime-MilliSecs()
		If dt>0
			Delay dt
		Else
			_syncTime:-dt
		EndIf
		_driver.Flip False
	Else If _hardSync
		_driver.Flip True
	Else
		_driver.Flip False
	EndIf

End Function

Function Graphics:TGraphics( width,height,depth=0,hertz=60,flags=0 )
	EndGraphics
	flags:|_defaultFlags
	
	Local g:TGraphics=CreateGraphics( width,height,depth,hertz,flags )
	If Not g Return
	
	BumpGraphicsSeq
	
	SetGraphics g

	If depth
		_softSync=False
		_hardSync=(hertz<>0)
	Else
		_hardSync=False
		_softSync=(hertz<>0)
	EndIf
	
	If _softSync
		_syncRate=hertz
		If _syncRate _syncPeriod=1000/_syncRate Else _syncPeriod=0
		_syncFrac=1000-_syncPeriod*_syncRate
		_syncAccum=0
		_syncTime=MilliSecs()
	EndIf

	_exGraphics=g
	
	Global _onEnd
	If Not _onEnd
		_onEnd=True
		OnEnd EndGraphics
	EndIf

	Return g
End Function

Function EndGraphics()
	If Not _exGraphics Return

	GraphicsSeq:+1
	If Not GraphicsSeq GraphicsSeq=1

	CloseGraphics _exGraphics
End Function

