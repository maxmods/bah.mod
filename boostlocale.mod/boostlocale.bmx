' Copyright (c) 2013 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
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

Rem
bbdoc: Boost Locale
End Rem
Module BaH.BoostLocale

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2013 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."


ModuleInfo "CC_OPTS: -fexceptions"

?Not Win32
ModuleInfo "CC_OPTS: -DBOOST_LOCALE_NO_WINAPI_BACKEND -DBOOST_LOCALE_NO_STD_BACKEND -DBOOST_LOCALE_WITH_ICONV"
?Win32
ModuleInfo "CC_OPTS: -DBOOST_LOCALE_NO_POSIX_BACKEND -DBOOST_LOCALE_NO_STD_BACKEND"
?

Import BaH.Boost
?Not Win32
Import BaH.libiconv
?

Import "common.bmx"


Private
Global localeGenerator:TBLGenerator = New TBLGenerator.Create()

Public

Rem
bbdoc: 
End Rem
Type TBLGenerator

	Field genPtr:Byte Ptr

	Function CreateGenerator:TBLGenerator()
		Return New TBLGenerator.Create()
	End Function
	
	Method Create:TBLGenerator()
		genPtr = bmx_boostlocale_generator_create()
		Return Self
	End Method
	


	Method generate:TBLLocale(id:String)
		Return TBLLocale._create(bmx_boostlocale_generator_generate(genPtr, id))
	End Method

End Type

Rem
bbdoc: The system default locale
End Rem
Global defaultLocale:TBLLocale = localeGenerator.Generate("")

Rem
bbdoc: The current locale
End Rem
Global currentLocale:TBLLocale = defaultLocale

Rem
bbdoc: Sets the current locale.
End Rem
Function SetLocale(locale:Object)
	If locale Then
		If String(locale) Then
			currentLocale = TBLLocale.Create(String(locale))
		Else If TBLLocale(locale) Then
			currentLocale = TBLLocale(locale)
		End If
	End If
End Function

Rem
bbdoc: 
End Rem
Type TBLLocale

	Field localePtr:Byte Ptr
	
	Function Create:TBLLocale(locale:String)
		Return localeGenerator.generate(locale)
	End Function
	
	Function _create:TBLLocale(localePtr:Byte Ptr)
		If localePtr Then
			Local this:TBLLocale = New TBLLocale
			this.localePtr = localePtr
			Return this
		End If
		Return Null
	End Function

	Method name:String()
		Return bmx_boostlocale_locale_name(localePtr)
	End Method
	
	Method language:String()
		Return bmx_boostlocale_locale_language(localePtr)
	End Method
	
	Method country:String()
		Return bmx_boostlocale_locale_country(localePtr)
	End Method
	
	Method variant:String()
		Return bmx_boostlocale_locale_variant(localePtr)
	End Method
	
	Method encoding:String()
		Return bmx_boostlocale_locale_encoding(localePtr)
	End Method
	
	Method utf8:Int()
		Return bmx_boostlocale_locale_utf8(localePtr)
	End Method

	Method Delete()
		If localePtr Then
			bmx_boostlocale_locale_free(localePtr)
			localePtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Return an int as a locale formatted string, using the current locale.
End Rem
Function IntAsNumber:String(value:Int)
	Return bmx_boostlocale_intasnumber(currentLocale.localePtr, value)
End Function

Rem
bbdoc: Return a float as a locale formatted string, using the current locale.
End Rem
Function FloatAsNumber:String(value:Float)
	Return bmx_boostlocale_floatasnumber(currentLocale.localePtr, value)
End Function

Rem
bbdoc: Return an int as a locale formatted currency string, using the current locale.
End Rem
Function IntAsCurrency:String(value:Int)
	Return bmx_boostlocale_intascurrency(currentLocale.localePtr, value)
End Function

Rem
bbdoc: Return a float as a locale formatted currency string, using the current locale.
End Rem
Function FloatAsCurrency:String(value:Float)
	Return bmx_boostlocale_floatascurrency(currentLocale.localePtr, value)
End Function
