
Strict

Rem
bbdoc: OSMesaMaz2D Base
End Rem
Module BaH.OSMesaMax2DBase

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Mark Sibly, Simon Armstrong and Bruce A Henderson"
ModuleInfo "License: Blitz Shared Source Code"
ModuleInfo "Copyright: Blitz Research Ltd"

ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: OSMesa port from original BRL.Max2D module"

Import BRL.LinkedList

Import "image.bmx"
Import "driver.bmx"
Import "imagefont.bmx"

Private

Global gc:TMax2DGraphics

Function UpdateTransform()
	Local s#=Sin(gc.tform_rot)
	Local c#=Cos(gc.tform_rot)
	gc.tform_ix= c*gc.tform_scale_x
	gc.tform_iy=-s*gc.tform_scale_y
	gc.tform_jx= s*gc.tform_scale_x
	gc.tform_jy= c*gc.tform_scale_y
	_max2dDriver.SetTransform gc.tform_ix,gc.tform_iy,gc.tform_jx,gc.tform_jy
	SetCollisions2DTransform gc.tform_ix,gc.tform_iy,gc.tform_jx,gc.tform_jy
End Function

Public

Type TMax2DGraphics Extends TGraphics

	Field color_red,color_green,color_blue
	Field color_alpha#
	Field clscolor_red,clscolor_green,clscolor_blue
	Field line_width#
	Field tform_rot#,tform_scale_x#,tform_scale_y#
	Field tform_ix#,tform_iy#,tform_jx#,tform_jy#
	Field viewport_x,viewport_y,viewport_w,viewport_h
	Field origin_x#,origin_y#
	Field handle_x#,handle_y#
	Field image_font:TImageFont
	Field blend_mode
	Field vres_width#,vres_height#
	Field vres_mousexscale#,vres_mouseyscale#

	Field g_width,g_height

	Global default_font:TImageFont
	Global mask_red,mask_green,mask_blue
	Global auto_midhandle
	Global auto_imageflags=MASKEDIMAGE|FILTEREDIMAGE

	Field _graphics:TGraphics,_driver:TMax2DDriver,_setup
	
	Method Driver:TMax2DDriver()
		Return _driver
	End Method
	
	Method GetSettings( width Var,height Var,depth Var,hertz Var,flags Var )
		Local w,h,d,r,f
		_graphics.GetSettings w,h,d,r,f
		width=w
		height=h
		depth=d
		hertz=r
		flags=f
	End Method
	
	Method Close()
		If Not _graphics Return
		_graphics.Close
		_graphics=Null
		_driver=Null
	End Method
	
	Method Validate()
		Local w,h,d,r,f
		_graphics.GetSettings w,h,d,r,f
		If w<>g_width Or h<>g_height
			g_width=w
			g_height=h
			vres_width=w
			vres_height=h						
			vres_mousexscale=1
			vres_mouseyscale=1
		EndIf
		SetVirtualResolution vres_width,vres_height
		SetBlend blend_mode
		SetColor color_red,color_green,color_blue
		SetAlpha color_alpha
		SetClsColor clscolor_red,clscolor_green,clscolor_blue
		SetLineWidth line_width
		SetRotation tform_rot
		SetScale tform_scale_x,tform_scale_y
		SetViewport viewport_x,viewport_y,viewport_w,viewport_h
		SetOrigin origin_x,origin_y
		SetHandle -handle_x,-handle_y
		SetImageFont image_font
	End Method
	
	Method MakeCurrent()
		gc=Self
		_max2dDriver=TMax2DDriver( Driver() )
		Assert _max2dDriver
		Validate
		If _setup Return
		Cls
		Flip 0
		Cls
		Flip 0
		_setup=True	
	End Method
	
	Function ClearCurrent()
		gc=Null
		_max2dDriver=Null
	End Function
	
	Function Current:TMax2DGraphics()
		Return gc
	End Function
	
	Function Create:TMax2DGraphics( g:TGraphics,d:TMax2DDriver )
		Local gw,gh,gd,gr,gf
		g.GetSettings gw,gh,gd,gr,gf
		
		If Not default_font default_font=TImageFont.CreateDefault()

		Local t:TMax2DGraphics=New TMax2DGraphics
		
		t.g_width=gw
		t.g_height=gh
		t.blend_mode=MASKBLEND
		t.color_red=255
		t.color_green=255
		t.color_blue=255
		t.color_alpha=1
		t.clscolor_red=0
		t.clscolor_green=0
		t.clscolor_blue=0
		t.line_width=1
		t.tform_rot=0
		t.tform_scale_x=1
		t.tform_scale_y=1
		t.tform_ix=1
		t.tform_iy=0
		t.tform_jx=1
		t.tform_jy=0
		t.viewport_x=0
		t.viewport_y=0
		t.viewport_w=gw
		t.viewport_h=gh
		t.origin_x=0
		t.origin_y=0
		t.handle_x=0
		t.handle_y=0
		t.image_font=default_font
		t.vres_width=gw
		t.vres_height=gh						
		t.vres_mousexscale=1
		t.vres_mouseyscale=1

		t._graphics=g
		t._driver=d
		t._setup=False

		Return t
	End Function
	
End Type

Function Cls()
	_max2dDriver.Cls
End Function

Function SetClsColor( red,green,blue )
	gc.clscolor_red=red
	gc.clscolor_green=green
	gc.clscolor_blue=blue
	_max2dDriver.SetClsColor red,green,blue
End Function

Function GetClsColor( red Var,green Var,blue Var )
	red=gc.clscolor_red
	green=gc.clscolor_green
	blue=gc.clscolor_blue
End Function

Function Plot( x#,y# )
	_max2dDriver.Plot x+gc.origin_x,y+gc.origin_y
End Function

Function DrawRect( x#,y#,width#,height# )
	_max2dDriver.DrawRect..
	gc.handle_x,gc.handle_y,..
	gc.handle_x+width,gc.handle_y+height,..
	x+gc.origin_x,y+gc.origin_y
End Function

Function DrawLine( x#,y#,x2#,y2#,draw_last_pixel=True )
	_max2dDriver.DrawLine..
	gc.handle_x,gc.handle_y,..
	gc.handle_x+x2-x,gc.handle_y+y2-y,..
	x+gc.origin_x,y+gc.origin_y
	If Not draw_last_pixel Return
	Local px#=gc.handle_x+x2-x,py#=gc.handle_y+y2-y
	_max2dDriver.Plot..
	px*gc.tform_ix+py*gc.tform_iy+x+gc.origin_x,px*gc.tform_jx+py*gc.tform_jy+y+gc.origin_y
End Function

Function DrawOval( x#,y#,width#,height# )
	_max2dDriver.DrawOval..
	gc.handle_x,gc.handle_y,..
	gc.handle_x+width,gc.handle_y+height,..
	x+gc.origin_x,y+gc.origin_y
End Function

Function DrawPoly( xy#[] )
	_max2dDriver.DrawPoly xy,..
	gc.handle_x,gc.handle_y,..
	gc.origin_x,gc.origin_y
End Function

Function DrawText( t$,x#,y# )
	gc.image_font.Draw t,..
	x+gc.origin_x+gc.handle_x*gc.tform_ix+gc.handle_y*gc.tform_iy,..
	y+gc.origin_y+gc.handle_x*gc.tform_jx+gc.handle_y*gc.tform_jy,..
	gc.tform_ix,gc.tform_iy,gc.tform_jx,gc.tform_jy
End Function

Function DrawImage( image:TImage,x#,y#,frame=0 )
	Local x0#=-image.handle_x,x1#=x0+image.width
	Local y0#=-image.handle_y,y1#=y0+image.height
	Local iframe:TImageFrame=image.Frame(frame)
	If iframe iframe.Draw x0,y0,x1,y1,x+gc.origin_x,y+gc.origin_y,0,0,image.width,image.height
End Function

Function DrawImageRect( image:TImage,x#,y#,w#,h#,frame=0 )
	Local x0#=-image.handle_x,x1#=x0+w
	Local y0#=-image.handle_y,y1#=y0+h
	Local iframe:TImageFrame=image.Frame(frame)
	If iframe iframe.Draw x0,y0,x1,y1,x+gc.origin_x,y+gc.origin_y,0,0,image.width,image.height
End Function

Function DrawSubImageRect( image:TImage,x#,y#,w#,h#,sx#,sy#,sw#,sh#,hx#=0,hy#=0,frame=0 )
	Local x0#=-hx*w/sw,x1#=x0+w
	Local y0#=-hy*h/sh,y1#=y0+h
	Local iframe:TImageFrame=image.Frame(frame)
	If iframe iframe.Draw x0,y0,x1,y1,x+gc.origin_x,y+gc.origin_y,sx,sy,sw,sh
End Function

Function TileImage( image:TImage,x#=0#,y#=0#,frame=0 )
	Local iframe:TImageFrame=image.Frame(frame)
	If Not iframe Return
	
	_max2dDriver.SetTransform 1,0,0,1

	Local w=image.width
	Local h=image.height
	Local ox=gc.viewport_x-w+1
	Local oy=gc.viewport_y-h+1
	Local px#=x+gc.origin_x-image.handle_x
	Local py#=y+gc.origin_y-image.handle_y
	Local fx#=px-Floor(px)
	Local fy#=py-Floor(py)
	Local tx=Floor(px)-ox
	Local ty=Floor(py)-oy

	If tx>=0 tx=tx Mod w + ox Else tx=w - -tx Mod w + ox
	If ty>=0 ty=ty Mod h + oy Else ty=h - -ty Mod h + oy

	Local vr=gc.viewport_x+gc.viewport_w,vb=gc.viewport_y+gc.viewport_h

	Local iy=ty
	While iy<vb
		Local ix=tx
		While ix<vr
			iframe.Draw 0,0,w,h,ix+fx,iy+fy,0,0,w,h
			ix=ix+w
		Wend
		iy=iy+h
	Wend

	UpdateTransform

End Function

Function SetColor( red,green,blue )
	gc.color_red=red
	gc.color_green=green
	gc.color_blue=blue
	_max2dDriver.SetColor red,green,blue
End Function

Function GetColor( red Var,green Var,blue Var )
	red=gc.color_red
	green=gc.color_green
	blue=gc.color_blue
End Function

Function SetBlend( blend )
	gc.blend_mode=blend
	_max2dDriver.SetBlend blend
End Function

Function GetBlend()
	Return gc.blend_mode
End Function

Function SetAlpha( alpha# )
	gc.color_alpha=alpha
	_max2dDriver.SetAlpha alpha
End Function

Function GetAlpha#()
	Return gc.color_alpha
End Function

Function SetLineWidth( width# )
	gc.line_width=width
	_max2dDriver.SetLineWidth width
End Function

Function GetLineWidth#()
	Return gc.line_width
End Function

Function SetMaskColor( red,green,blue )
	gc.mask_red=red
	gc.mask_green=green
	gc.mask_blue=blue
End Function

Function GetMaskColor( red Var,green Var,blue Var )
	red=gc.mask_red
	green=gc.mask_green
	blue=gc.mask_blue
End Function

Function SetVirtualResolution( width#,height# )
	gc.vres_width=width
	gc.vres_height=height
	gc.vres_mousexscale=width/GraphicsWidth()
	gc.vres_mouseyscale=height/GraphicsHeight()
	_max2dDriver.SetResolution width,height
End Function

Function VirtualResolutionWidth#()
	Return gc.vres_width
End Function

Function VirtualResolutionHeight#()
	Return gc.vres_height
End Function

Function VirtualMouseX#()
	'Return MouseX() * gc.vres_mousexscale
End Function

Function VirtualMouseY#()
	'Return MouseY() * gc.vres_mouseyscale
End Function

Function VirtualMouseXSpeed#()
	'Return MouseXSpeed() * gc.vres_mousexscale
End Function

Function VirtualMouseYSpeed#()
	'Return MouseYSpeed() * gc.vres_mouseyscale
End Function

Function MoveVirtualMouse( x#,y# )
	'MoveMouse x/gc.vres_mousexscale,y/gc.vres_mouseyscale
End Function

Function SetViewport( x,y,width,height )
	gc.viewport_x=x
	gc.viewport_y=y
	gc.viewport_w=width
	gc.viewport_h=height
	Local x0=Floor( x / gc.vres_mousexscale )
	Local y0=Floor( y / gc.vres_mouseyscale )
	Local x1=Floor( (x+width) / gc.vres_mousexscale )
	Local y1=Floor( (y+height) / gc.vres_mouseyscale )
	_max2dDriver.SetViewport x0,y0,(x1-x0),(y1-y0)
End Function

Function GetViewport( x Var,y Var,width Var,height Var )
	x=gc.viewport_x
	y=gc.viewport_y
	width=gc.viewport_w
	height=gc.viewport_h
End Function

Function SetOrigin( x#,y# )
	gc.origin_x=x
	gc.origin_y=y
End Function

Function GetOrigin( x# Var,y# Var )
	x=gc.origin_x
	y=gc.origin_y
End Function

Function SetHandle( x#,y# )
	gc.handle_x=-x
	gc.handle_y=-y
End Function

Function GetHandle( x# Var,y# Var )
	x=-gc.handle_x
	y=-gc.handle_y
End Function

Function SetRotation( rotation# )
	gc.tform_rot=rotation
	UpdateTransform
End Function

Function GetRotation#()
	Return gc.tform_rot
End Function

Function SetScale( scale_x#,scale_y# )
	gc.tform_scale_x=scale_x
	gc.tform_scale_y=scale_y
	UpdateTransform
End Function

Function GetScale( scale_x# Var,scale_y# Var )
	scale_x=gc.tform_scale_x
	scale_y=gc.tform_scale_y
End Function

Function SetTransform( rotation#=0,scale_x#=1,scale_y#=1 )
	gc.tform_rot=rotation
	gc.tform_scale_x=scale_x
	gc.tform_scale_y=scale_y
	UpdateTransform
End Function

Function LoadImageFont:TImageFont( url:Object,size,style=SMOOTHFONT )
	Return TImageFont.Load( url,size,style )
End Function

Function SetImageFont( font:TImageFont )
	If Not font font=gc.default_font
	gc.image_font=font
End Function

Function GetImageFont:TImageFont()
	Return gc.image_font
End Function

Function TextWidth( text$ )
	Local width=0
	For Local n=0 Until text.length
		Local i=gc.image_font.CharToGlyph( text[n] )
		If i<0 Continue
		width:+gc.image_font.LoadGlyph(i).Advance()
	Next
	Return width
End Function

Function TextHeight( text$ )
	Return gc.image_font.Height()
	Rem
	Local height=0
	For Local n=0 Until text.length
		Local c=text[n]-image_font.BaseChar()
		If c<0 Or c>=image_font.CountGlyphs() Continue
		Local x,y,w,h
		image_font.Glyph(c).GetRect( x,y,w,h )
		height=Max(height,h)
	Next
	Return height
	End Rem
End Function

Function LoadImage:TImage( url:Object,flags=-1 )
	If flags=-1 flags=gc.auto_imageflags
	Local image:TImage=TImage.Load( url,flags,gc.mask_red,gc.mask_green,gc.mask_blue )
	If Not image Return
	If gc.auto_midhandle MidHandleImage image
	Return image
End Function

Function LoadAnimImage:TImage( url:Object,cell_width,cell_height,first_cell,cell_count,flags=-1 )
	If flags=-1 flags=gc.auto_imageflags
	Local image:TImage=TImage.LoadAnim( url,cell_width,cell_height,first_cell,cell_count,flags,gc.mask_red,gc.mask_green,gc.mask_blue )
	If Not image Return
	If gc.auto_midhandle MidHandleImage image
	Return image
End Function 

Function SetImageHandle( image:TImage,x#,y# )
	image.handle_x=x
	image.handle_y=y
End Function

Function AutoMidHandle( enable )
	gc.auto_midhandle=enable
End Function

Function AutoImageFlags( flags )
	If flags=-1 Return
	gc.auto_imageflags=flags
End Function

Function MidHandleImage( image:TImage )
	image.handle_x=image.width*.5
	image.handle_y=image.height*.5
End Function

Function ImageWidth( image:TImage )
	Return image.width
End Function

Function ImageHeight( image:TImage )
	Return image.height
End Function

Function CreateImage:TImage( width,height,frames=1,flags=-1 )
	If flags=-1 flags=gc.auto_imageflags
	Local image:TImage=TImage.Create( width,height,frames,flags|DYNAMICIMAGE,gc.mask_red,gc.mask_green,gc.mask_blue )
	If gc.auto_midhandle MidHandleImage image
	Return image
End Function

Function LockImage:TPixmap( image:TImage,frame=0,read_lock=True,write_lock=True )
	Return image.Lock( frame,read_lock,write_lock )
End Function

Function UnlockImage( image:TImage,frame=0 )
End Function

Function GrabImage( image:TImage,x,y,frame=0 )
	Local pixmap:TPixmap=_max2dDriver.GrabPixmap( x,y,image.width,image.height )
	If image.flags&MASKEDIMAGE 
		pixmap=MaskPixmap( pixmap,gc.mask_red,gc.mask_green,gc.mask_blue )
	EndIf
	image.SetPixmap frame,pixmap
End Function

Function DrawPixmap( pixmap:TPixmap,x,y )
	_max2dDriver.DrawPixmap pixmap,x,y
End Function

Function GrabPixmap:TPixmap( x,y,width,height )
	Return _max2dDriver.GrabPixmap( x,y,width,height )
End Function

Const COLLISION_LAYER_ALL=0
Const COLLISION_LAYER_1=$0001
Const COLLISION_LAYER_2=$0002
Const COLLISION_LAYER_3=$0004
Const COLLISION_LAYER_4=$0008
Const COLLISION_LAYER_5=$0010
Const COLLISION_LAYER_6=$0020
Const COLLISION_LAYER_7=$0040
Const COLLISION_LAYER_8=$0080
Const COLLISION_LAYER_9=$0100
Const COLLISION_LAYER_10=$0200
Const COLLISION_LAYER_11=$0400
Const COLLISION_LAYER_12=$0800
Const COLLISION_LAYER_13=$1000
Const COLLISION_LAYER_14=$2000
Const COLLISION_LAYER_15=$4000
Const COLLISION_LAYER_16=$8000
Const COLLISION_LAYER_17=$00010000
Const COLLISION_LAYER_18=$00020000
Const COLLISION_LAYER_19=$00040000
Const COLLISION_LAYER_20=$00080000
Const COLLISION_LAYER_21=$00100000
Const COLLISION_LAYER_22=$00200000
Const COLLISION_LAYER_23=$00400000
Const COLLISION_LAYER_24=$00800000
Const COLLISION_LAYER_25=$01000000
Const COLLISION_LAYER_26=$02000000
Const COLLISION_LAYER_27=$04000000
Const COLLISION_LAYER_28=$08000000
Const COLLISION_LAYER_29=$10000000
Const COLLISION_LAYER_30=$20000000
Const COLLISION_LAYER_31=$40000000
Const COLLISION_LAYER_32=$80000000

Function ImagesCollide(image1:TImage,x1,y1,frame1,image2:TImage,x2,y2,frame2)
	ResetCollisions COLLISION_LAYER_32
	CollideImage image1,x1,y1,frame1,0,COLLISION_LAYER_32
	If CollideImage(image2,x2,y2,frame2,COLLISION_LAYER_32,0) Return True
End Function

Function ImagesCollide2(image1:TImage,x1,y1,frame1,rot1#,scalex1#,scaley1#,image2:TImage,x2,y2,frame2,rot2#,scalex2#,scaley2#)
	Local	_scalex#,_scaley#,_rot#,res
	_rot=GetRotation()
	GetScale _scalex,_scaley
	ResetCollisions COLLISION_LAYER_32
	SetRotation rot1
	SetScale scalex1,scaley1
	CollideImage image1,x1,y1,frame1,0,COLLISION_LAYER_32
	SetRotation rot2
	SetScale scalex2,scaley2
	If CollideImage(image2,x2,y2,frame2,COLLISION_LAYER_32,0) res=True
	SetRotation _rot
	SetScale _scalex,_scaley
	Return res
End Function

Function ResetCollisions(mask%=0)
	Local	i,q:TQuad
	For i=0 To 31
		If mask=0 Or mask&(1 Shl i)
			q=quadlayer[i]
			If q
				q.mask=Null
				q.id=Null
				While q.link
					q=q.link
					q.mask=Null
					q.id=Null
				Wend
				q.link=freequads				
				q=quadlayer[i]
				freequads=q
				quadlayer[i]=Null
			EndIf
		EndIf
	Next
End Function

Function CollideImage:Object[](image:TImage,x,y,frame,collidemask%,writemask%,id:Object=Null) 
	Local	q:TQuad
	q=CreateQuad(image,frame,x,y,image.width,image.height,id)
	Return CollideQuad(q,collidemask,writemask)
End Function

Function CollideRect:Object[](x,y,w,h,collidemask%,writemask%,id:Object=Null) 
	Local	q:TQuad
	q=CreateQuad(Null,0,x,y,w,h,id)
	Return CollideQuad(q,collidemask,writemask)
End Function

Private

Global	cix#,ciy#,cjx#,cjy#

Function SetCollisions2DTransform(ix#,iy#,jx#,jy#)	'callback from module Blitz2D
	cix=ix
	ciy=iy
	cjx=jx
	cjy=jy
End Function

Global TextureMaps:TPixmap[]
Global LineBuffer[]
Global quadlayer:TQuad[32]
Global freequads:TQuad

Const POLYX=0
Const POLYY=1
Const POLYU=2
Const POLYV=3

Function DotProduct(x0#,y0#,x1#,y1#,x2#,y2#)
	Return (((x2-x1)*(y1-y0))-((x1-x0)*(y2-y1)))
End Function

Function ClockwisePoly(data#[],channels)	'flips order if anticlockwise
	Local	count,clk,i,j
	Local	r0,r1,r2
	Local	t#
	
	count=Len(data)/channels
' clock wise test
	r0=0
	r1=channels
	clk=2
	For i=2 To count-1
		r2=r1+channels
		If DotProduct(data[r0+POLYX],data[r0+POLYY],data[r1+POLYX],data[r1+POLYY],data[r2+POLYX],data[r2+POLYY])>=0 clk:+1	
		r1=r2
	Next
	If clk<count Return
' flip order for anticockwise
	r0=0
	r1=(count-1)*channels
	While r0<r1
		For j=0 To channels-1
			t=data[r0+j]
			data[r0+j]=data[r1+j]
			data[r1+j]=t
		Next
		r0:+channels
		r1:-channels
	Wend
End Function

Type rpoly
	Field	texture:TPixmap
	Field	data#[]
	Field	channels,count,size
	Field	ldat#[],ladd#[]
	Field	rdat#[],radd#[]
	Field	Left,Right,top
	Field	state
End Type

Function RenderPolys(vdata#[][],channels[],textures:TPixmap[],renderspans(polys:TList,count,ypos))
	Local	polys:rpoly[],p:rpoly,pcount
	Local	active:TList
	Local	top,bot
	Local	n,y,h,i,j,res
	Local	data#[]

	bot=$80000000
	top=$7fffffff
	n=Len(vdata)
' create polys an array of poly renderers	
	polys=New rpoly[n]		
	For i=0 Until n
		p=New rpoly
		polys[i]=p
		p.texture=textures[i]
		p.data=vdata[i]
		p.channels=channels[i]
		p.count=Len(p.data)/p.channels
		p.size=p.count*p.channels
		ClockwisePoly(p.data,p.channels)	'flips order if anticlockwise
' find top verticies
		p.Left=0
		j=0
		p.top=$7fffffff
		While j<p.size
			y=p.data[j+POLYY]		'float to int conversion
			If y<p.top p.top=y;p.Left=j
			If y<top top=y
			If y>bot bot=y
			j:+p.channels
		Wend
		p.Right=p.Left
	Next
	active=New TList
	pcount=0
' draw top to bottom
	For y=top To bot-1
' get left gradient
		For p=EachIn polys			
			If p.state=2 Continue 
			If p.state=0 And y<p.top Continue
			data=p.data
			If y>=Int(data[p.Left+POLYY])
				j=p.Left
				i=(p.Left-p.channels)
				If i<0 i:+p.size
				While i<>p.Left
					If Int(data[i+POLYY])>y Exit
					j=i
					i=(i-p.channels)
					If i<0 i:+p.size
				Wend
				h=Int(data[i+POLYY])-Int(data[j+POLYY])
				If i=p.Left Or h<=0
					active.remove p
'					p.remove
					pcount:-1
					p.state=2
					Continue
				EndIf
				p.ldat=data[j..j+p.channels]
				p.ladd=data[i..i+p.channels]				
				For j=0 To p.channels-1
					p.ladd[j]=(p.ladd[j]-p.ldat[j])/h
					p.ldat[j]:+p.ladd[j]*0.5
				Next
				p.Left=i			
				If p.state=0
					p.state=1
					active.AddLast p
					pcount:+1
				EndIf			
			EndIf
' get right gradient
			If y>=Int(data[p.Right+POLYY])
				i=(p.Right+p.channels) Mod p.size
				j=p.Right
				While i<>p.Right
					If Int(data[i+POLYY])>y Exit
					j=i
					i=(i+p.channels)Mod p.size
				Wend
				h=Int(data[i+POLYY])-Int(data[j+POLYY])
				If i=p.Right Or h<=0
					active.remove p
					pcount:-1
					p.state=2
					Continue
				EndIf
				p.rdat=data[j..j+p.channels]
				p.radd=data[i..i+p.channels]
				For j=0 To p.channels-1
					p.radd[j]=(p.radd[j]-p.rdat[j])/h
					p.rdat[j]:+p.radd[j]*0.5
				Next
				p.Right=i
				If p.state=0
					p.state=1
					active.AddLast p
					pcount:+1
				EndIf			
			EndIf
		Next	
' call renderer
		If pcount
			res=renderspans(active,pcount,y)
			If res<0 Return res
		EndIf
' increment spans
		For p=EachIn active
			For j=0 To p.channels-1
				p.ldat[j]:+p.ladd[j]
				p.rdat[j]:+p.radd[j]
			Next
		Next
	Next
	Return res
End Function

Function CollideSpans(polys:TList,count,y)
	Local	p:rpoly
	Local	startx,endx
	Local	x0,x1,w,x
	Local	u#,v#,ui#,vi#
	Local	pix Ptr
	Local	src:TPixmap
	Local	tw,th,tp,argb
	Local	width,skip#
	

	startx=$7fffffff
	endx=$80000000
	If count<2 Return 0
	p=rpoly(polys.ValueAtIndex(0))
	startx=p.ldat[POLYX]
	endx=p.rdat[POLYX]
	p=rpoly(polys.ValueAtIndex(1))
	x0=p.ldat[POLYX]
	x1=p.rdat[POLYX]
	If x0>=endx Return 0
	If x1<=startx Return 0
	If x0>startx startx=x0
	If x1<endx endx=x1
	width=endx-startx
	If width<=0 Return 0
	If width>Len(LineBuffer) LineBuffer=New Int[width]
	MemClear LineBuffer,width*4
	For p=EachIn polys
		src=p.texture
		If src
			x0=p.ldat[POLYX]
			x1=p.rdat[POLYX]
			w=x1-x0
			If w<=0 Continue		
			u=p.ldat[POLYU]
			v=p.ldat[POLYV]
			ui=(p.rdat[POLYU]-u)/w
			vi=(p.rdat[POLYV]-v)/w
			skip=(startx-x0)+0.5
			u=u+ui*skip
			v=v+vi*skip			
			pix=Int Ptr(src.pixels)
			tw=src.width
			th=src.height
			tp=src.pitch/4
			For x=0 Until width
				If u<0.0 u=0.0
				If v<0.0 v=0.0
				If u>1.0 u=1.0
				If v>1.0 v=1.0
?BigEndian
				argb=$00000080 & pix[(Int(v*th))*tp+(Int(u*tw))]
?LittleEndian
				argb=$80000000 & pix[(Int(v*th))*tp+(Int(u*tw))]
?
				If (argb)
					If LineBuffer[x] Return -1
					LineBuffer[x]=argb
				EndIf
				u:+ui
				v:+vi
			Next
		Else
			For x=0 Until width
				If LineBuffer[x] Return -1
				LineBuffer[x]=-1
			Next
		EndIf
	Next
	Return 0
End Function

Type TQuad
	Field	link:TQuad
	Field	id:Object
	Field	mask:TPixmap
	Field	frame	
	Field	minx#,miny#,maxx#,maxy#
	Field	xyuv#[16]
		
	Method SetCoords(tx0#,ty0#,tx1#,ty1#,tx2#,ty2#,tx3#,ty3#)
		xyuv[0]=tx0
		xyuv[1]=ty0
		xyuv[2]=0.0
		xyuv[3]=0.0		
		xyuv[4]=tx1
		xyuv[5]=ty1
		xyuv[6]=1.0
		xyuv[7]=0.0				
		xyuv[8]=tx2
		xyuv[9]=ty2
		xyuv[10]=1.0
		xyuv[11]=1.0		
		xyuv[12]=tx3
		xyuv[13]=ty3
		xyuv[14]=0.0
		xyuv[15]=1.0
		minx=Min(Min(Min(tx0,tx1),tx2),tx3)
		miny=Min(Min(Min(ty0,ty1),ty2),ty3)
		maxx=Max(Max(Max(tx0,tx1),tx2),tx3)
		maxy=Max(Max(Max(ty0,ty1),ty2),ty3)
	End Method
End Type

Function QuadsCollide(p:TQuad,q:TQuad)
	If p.maxx<q.minx Or p.maxy<q.miny Or p.minx>q.maxx Or p.miny>q.maxy Return False
	Local	vertlist#[][2]
	Local	textures:TPixmap[2]
	Local	channels[2]	
	vertlist[0]=p.xyuv	
	vertlist[1]=q.xyuv	
	textures[0]=p.mask
	textures[1]=q.mask
	channels[0]=4
	channels[1]=4
	Return RenderPolys(vertlist,channels,textures,CollideSpans)
End Function

Function CreateQuad:TQuad(image:TImage,frame,x#,y#,w#,h#,id:Object)
	Local	x0#,y0#,x1#,y1#,tx#,ty#
	Local	tx0#,ty0#,tx1#,ty1#,tx2#,ty2#,tx3#,ty3#
	Local	minx#,miny#,maxx#,maxy#
	Local	q:TQuad
	Local	pix:TPixmap
	
	If image
		x0=-image.handle_x
		y0=-image.handle_y
	EndIf
	x1=x0+w
	y1=y0+h
	tx=x+gc.origin_x
	ty=y+gc.origin_y
	tx0=x0*cix+y0*ciy+tx
	ty0=x0*cjx+y0*cjy+ty
	tx1=x1*cix+y0*ciy+tx
	ty1=x1*cjx+y0*cjy+ty
	tx2=x1*cix+y1*ciy+tx
	ty2=x1*cjx+y1*cjy+ty
	tx3=x0*cix+y1*ciy+tx
	ty3=x0*cjx+y1*cjy+ty
	If freequads
		q=freequads
		freequads=q.link
		q.link=Null
	Else
		q=New TQuad
	EndIf
	q.id=id
	If image
		pix=image.Lock( frame,True,False )
		If AlphaBitsPerPixel[pix.format] q.mask=pix
	EndIf
	q.setcoords(tx0,ty0,tx1,ty1,tx2,ty2,tx3,ty3)	
	Return q
End Function

Function CollideQuad:Object[](pquad:TQuad,collidemask%,writemask%) 
	Local	result:Object[]
	Local	p:TQuad,q:TQuad
	Local	i,j,count

	p=pquad				'CreateImageQuad(image,frame,x,y)
' check for collisions
	For i=0 To 31
		If collidemask & (1 Shl i)
			q=quadlayer[i]
			While q
				If QuadsCollide(p,q)
					If count=Len(result) result=result[..((count+4)*1.2)]
					result[count]=q.id
					count:+1
				EndIf				
				q=q.link
			Wend		
		EndIf
	Next
' write to layers	
	For i=0 To 31
		If writemask & (1 Shl i)
			If freequads
				q=freequads
				freequads=q.link
			Else
				q=New TQuad
			EndIf
			q.id=p.id;	'TODO:optimize with memcpy?
			q.mask=p.mask;
			q.frame=p.frame
			MemCopy q.xyuv,p.xyuv,64
			q.minx=p.minx;q.miny=p.miny;q.maxx=p.maxx;q.maxy=p.maxy;
			q.link=quadlayer[i]
			quadlayer[i]=q
		EndIf
	Next
' return result
	If count Return result[..count]
End Function
