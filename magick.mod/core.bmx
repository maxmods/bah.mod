' Copyright (c) 2008-2013 Bruce A Henderson
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

Import BRL.FileSystem

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TMColor

	Field colorPtr:Byte Ptr


End Type




?win32
Const kMDEFAULT_FONTPATH:String = "C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
Const kMPATHSEPARATOR:String = ";"
?linux
Const kMDEFAULT_FONTPATH:String = "/usr/share/X11/fonts/TTF:/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:" + ..
	"/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/bitstream-vera:" + ..
	"/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1"
Const kMPATHSEPARATOR:String = ":"
?macos
Const kMDEFAULT_FONTPATH:String = "~~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
Const kMPATHSEPARATOR:String = ":"
?

Global MAGICK_USER_FONTPATH:String = "."

Type TCustomMagickFont

	Function getFont:String(name:String)
		
		Local paths:String
		
		If MAGICK_USER_FONTPATH Then
			paths = MAGICK_USER_FONTPATH + kMPATHSEPARATOR + kMDEFAULT_FONTPATH
		Else
			paths = kMDEFAULT_FONTPATH
		End If
		
		Local fname:String = alternateFontList(name)
		
		If Not fname Then
			fname = name
		End If
		
		Return getFontPath(paths, fname)
			
	End Function

	Function alternateFontList:String(fname:String)
		Local fontList:String
		
		fname = fname.toLower()
		
		Local fontNoSep:String = ""
		
		If fname.find("-") > 0 Then
			fontNoSep = fname[0..fname.find("-")]
		Else If fname.find("_") > 0 Then
			fontNoSep = fname[0..fname.find("_")]
		Else
			fontNoSep = fname
		End If
		
		If fname = "times-bold" Or fontNoSep = "timesbd" Or fontNoSep = "timesb" Then
			fontList = "timesbd;Timesbd;TIMESBD;timesb;Timesb;TIMESB;VeraSeBd;Times"
		Else If fname = "times-italic" Or fontNoSep = "timesi" Then
			fontList = "timesi;Timesi;TIMESI;VeraIt;Times"
		Else If fname = "timesnewroman" Or fname = "timesnew" Or fname = "timesroman" Or fontNoSep = "times" Then
			fontList = "times;Times;TIMES;VeraSe"
		Else If fname = "arial-bold" Or fontNoSep = "arialb" Or fontNoSep = "arialbd" Then
			fontList = "arialb;Arialb;ARIALB;arialbd;Arialbd;ARIALBD;VeraBd;Helvetica"
		Else If fname = "arial-italic" Or fontNoSep = "ariali" Then
			fontList = "ariali;Ariali;ARIALI;VeraIt;Helvetica"
		Else If fname = "arial-bold-italic" Or fontNoSep = "arialbi" Then
			fontList = "arialbi;Arialbi;ARIALBI;VeraBI;Helvetica"
		Else If fontNoSep = "helvetica" Then
			fontList = "helvetica;Helvetica;HELVETICA;arial;Arial;ARIAL;Vera"
		Else If fontNoSep = "arial" Then
			fontList = "arial;Arial;ARIAL;Vera;helvetica;Helvetica;HELVETICA"
		Else If fontNoSep = "courier" Then
			fontList = "courier;Courier;COURIER;cour;VeraMono"
		End If
		
		Return fontList
	End Function
	
	Function getFontPath:String(paths:String, fontList:String)
	
		Local fontNames:String[] = fontList.Split(";")
		Local pathNames:String[] = paths.Split(kMPATHSEPARATOR)
		Local fullname:String
		Local found:Int = False
		
		For Local f:Int = 0 Until fontNames.length
		
		
			For Local p:Int = 0 Until pathNames.length
			
				Local baseName:String = pathNames[p] + "/" + fontNames[f]
				
				' already has a suffix ?
				If fontNames[f].find(".") > 0 Then
					
					If FileType(baseName) Then
						Return baseName
					End If
					
					' try next path...
					Continue
				End If
			
				fullName = baseName + ".ttf"

				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfa"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfb"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".dfont"
				If FileType(fullName) Then
					Return fullName
				End If
		
			Next
		
		Next
	End Function

End Type

