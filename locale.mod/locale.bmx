' Copyright (c) 2007 Bruce A Henderson
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
bbdoc: Localization
End Rem
Module BaH.Locale

ModuleInfo "Version: 1.02"
ModuleInfo "License: MIT"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "Copyright: 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Added work-around for Win32 Event Hook bug"
ModuleInfo "History: Added Simon Read's CSV import/export."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed BLFBuilder translated text."
ModuleInfo "History: 1.00 Initial Release"


Import BaH.libxml
Import BRL.FileSystem
Import BRL.Map

Incbin "languages.xml"

Rem
bbdoc: Loads a BLF file into the library.
about: Supports incbin'd files.
<p>Parameters:
<ul>
<li><b>filename</b> : the name of the BLF file</li>
</ul>
</p>
End Rem
Function LoadLocaleFile(filename:String)
	TLocale.LoadFromBLF(filename)
End Function

Rem
bbdoc: Returns the text for the given tag.
returns: The localized text.
about: If @locale is not provided, the currentLocale is used.<br>
If text for the currentLocale is not present, the defaultLocale is used.<br>
If text for the defaultLocale is not present, the tag is returned preceded by an "@". (the reason being that
it highlights tags that require at least a default text)<br>
<p>Parameters:
<ul>
<li><b>tag</b> : the tag name</li>
<li><b>locale</b> : the language code (optional) <i>[in ISO 639-x format]</i></li>
</ul>
</p>
End Rem
Function GetLocaleText:String(tag:String, locale:String = Null)
	Return TLocale.getText(tag, locale)
End Function

Rem
bbdoc: Returns the optional value for the given tag.
returns: The locale-specific int value.
about: If @locale is not provided, the currentLocale is used.<br>
If no value is set, 0 (zero) is returned.<br>
This value might be used for example with a menu item, representing the value of the short-cut key.
<p>Parameters:
<ul>
<li><b>tag</b> : the tag name</li>
<li><b>locale</b> : the language code (optional) <i>[in ISO 639-x format]</i></li>
</ul>
</p>
End Rem
Function GetLocaleValue:Int(tag:String, locale:String = Null)
	Return TLocale.getValue(tag, locale)
End Function

Rem
bbdoc: Adds a new localized text entry to the library.
about: If @locale is not provided, the curreltLocale is assumed.
<p>Parameters:
<ul>
<li><b>tag</b> : the tag name</li>
<li><b>text</b> : the localized text</li>
<li><b>locale</b> : the language code (optional) <i>[in ISO 639-x format]</i></li>
<li><b>value</b> : the localized value (optional)</li>
</ul>
</p>
End Rem
Function AddLocaleText(tag:String, text:String, locale:String = Null, value:Int = 0)
	TLocale.addText(tag, text, locale, value)
End Function

Rem
bbdoc: Sets the current locale.
about: Subsequent calls to #GetLocaleText will be returned in this localization.
<p>Parameters:
<ul>
<li><b>locale</b> : the language code <i>[in ISO 639-x format]</i></li>
</ul>
</p>
End Rem
Function SetCurrentLocale(locale:String)
	TLocale.SetCurrentLocale(locale)
End Function

Rem
bbdoc: Returns the value of the current locale.
about: This is the value applied to #GetLocaleText when the locale parameter is not used.
End Rem
Function GetCurrentLocale:String()
	Return TLocale.GetCurrentLocale()
End Function

Rem
bbdoc: Sets the default locale.
about: The default locale is the base localization. There should always be localized text in the
default locale.
<p>Parameters:
<ul>
<li><b>locale</b> : the language code <i>[in ISO 639-x format]</i></li>
</ul>
</p>
End Rem
Function SetDefaultLocale(locale:String)
	TLocale.SetDefaultLocale(locale)
End Function

Rem
bbdoc: Returns the value of the default locale. <i>[in ISO 639-x format]</i>
End Rem
Function GetDefaultLocale:String()
	Return TLocale.GetDefaultLocale()
End Function


Rem
bbdoc: Returns a String array of locales currently loaded into the library via a BLF file.
End Rem
Function GetAvailableLocales:String[]()
	Return TLocale.getLocales()
End Function

Rem
bbdoc: Returns a String array of all locales known to the system.
about: These are the 190+ languages known to the Locale module.
End Rem
Function GetSystemLocales:String[]()
	Return TLanguages.getLocales()
End Function

Rem
bbdoc: Returns the language name of the provided locale.
about: Default is in English, or set @native to true to return the language-native string, if available.<br>
eg. "de" returns "German" in English, and "Deutsch" in native.
<p>Parameters:
<ul>
<li><b>locale</b> : the language code <i>[in ISO 639-x format]</i></li>
<li><b>native</b> : whether to return language text in English or native script.</li>
</ul>
</p>
End Rem
Function GetLanguage:String(locale:String, native:Int = False)
	Return TLanguages.GetLanguage(locale, native)
End Function


Type TLocale

	Global textMap:TMap = New TMap
	
	Global defaultLocale:String = "en"
	Global currentLocale:String = "en"
	
	Global langMap:TMap = TLocale.initLanguages()

	' Loads a blitz locale file into the TLocale library.
	Function LoadFromBLF(filename:String)
		If Not filename.startsWith("incbin") And FileType(filename) <> 1 Then
			Throw "blf file '" + filename + "' not found"
		End If
		
		Local doc:TxmlDoc = TxmlDoc.parseFile(filename)
		
		If doc Then
		
			Local root:TxmlNode = doc.getRootElement()
			
			' default language
			Local df:String = root.getAttribute("default")
			If df Then
				defaultLocale = df
				currentLocale = df
			End If
			
			For Local node:TxmlNode = EachIn root.getChildren()
			
				If node.getName() = "tag" Then
				
					Local tag:String = node.getAttribute("name")
				
					For Local trans:TxmlNode = EachIn node.getChildren()
						
						If trans.getName() = "trans" Then
						
							addText(tag, trans.getContent(), trans.getAttribute("lang"), trans.getAttribute("value"))
						
						End If
						
					Next
					
				End If
			
			Next
		
			doc.free()
		End If
		
	End Function
	
	Function addText(tag:String, text:String, locale:String = Null, value:String = Null)
	
		Local lt:TLocalizedText = TLocalizedText(textMap.ValueForKey(tag))
		
		Local v:Int = 0
		If value Then
			v = value.toInt()
		End If
		
		If lt = Null Then
			If locale Then
				textMap.insert(tag, TLocalizedText.Create(tag, text, locale, v))
			Else
				textMap.insert(tag, TLocalizedText.Create(tag, text, defaultLocale, v))
			End If
		Else
			If locale Then
				lt.addLocale(text, locale, v)
			Else
				lt.addLocale(text, defaultLocale, v)
			End If
		End If
	
		If locale Then
			addLanguage(locale)
		End If
	End Function

	Function getText:String(tag:String, locale:String = Null)
		Local lt:TLocalizedText = TLocalizedText(textMap.ValueForKey(tag))
		
		If lt Then
			If locale Then
				Return lt.getText(locale)
			Else
				Return lt.getText(currentLocale)
			End If
		Else
			Return "@" + tag
		End If
		
	End Function

	Function getValue:Int(tag:String, locale:String = Null)
		Local lt:TLocalizedText = TLocalizedText(textMap.ValueForKey(tag))
		
		If lt Then
			If locale Then
				Return lt.getValue(locale)
			Else
				Return lt.getValue(currentLocale)
			End If
		Else
			Return 0
		End If
		
	End Function
	
	Function SetDefaultLocale(locale:String)
		defaultLocale = locale
	End Function
	
	Function GetDefaultLocale:String()
		Return defaultLocale
	End Function

	Function SetCurrentLocale(locale:String)
		currentLocale = locale
	End Function
	
	Function GetCurrentLocale:String()
		Return currentLocale
	End Function

	Function initLanguages:TMap()
		Local map:TMap = New TMap
		map.insert(defaultLocale, defaultLocale)
		Return map
	End Function
	
	Function addLanguage(language:String)
		Local lang:String = String(langMap.ValueForKey(language))
		
		If Not lang Then
			langMap.insert(language, language)
		End If
	End Function
	
	Function getLocales:String[]()
		Local locales:String[] = New String[10]
		
		Local count:Int = 0
		For Local key:String = EachIn langMap.Keys()
			count:+ 1
			If count > locales.length Then
				locales = locales[..count + 5]
			End If
			
			locales[count-1] = key
		Next
		
		Return locales[..count]
	End Function
	
End Type


Type TLocalizedText

	Field tag:String
	Field locales:TMap = New TMap

	Function Create:TLocalizedText(tag:String, text:String, locale:String, value:Int = 0)
		Local this:TLocalizedText = New TLocalizedText
		
		Assert tag, "tag text is required"
		Assert text, "translation text is required"
		Assert locale, "locale is required"
		
		this.tag = tag
		this.locales.insert(locale, TLocalePart.Create(text, value))
		
		Return this
	End Function


	Method addLocale(text:String, locale:String, value:Int = 0)
		
		locales.insert(locale, TLocalePart.Create(text, value))
		
	End Method
	
	Method getText:String(locale:String)
	
		Local tp:TLocalePart = TLocalePart(locales.ValueForKey(locale))
	
		If tp = Null Then
		
			If locale <> TLocale.defaultLocale Then
				Return getText(TLocale.defaultLocale)
			Else
				Return "@" + tag
			End If
		End If
		
		Return tp.text
	
	End Method
	
	Method getValue:Int(locale:String)
	
		Local tp:TLocalePart = TLocalePart(locales.ValueForKey(locale))
	
		If tp = Null Then
		
			If locale <> TLocale.defaultLocale Then
				Return getValue(TLocale.defaultLocale)
			Else
				Return 0
			End If
		End If
		
		Return tp.value
	
	End Method

End Type

Type TLocalePart

	Field text:String
	Field value:Int

	Function Create:TLocalePart(text:String, value:Int = 0)
		Local this:TLocalePart = New TLocalePart
		
		this.text = text
		this.value = value
		
		Return this
	End Function
	
End Type

Type TLanguages

	Global languages:TMap = TLanguages.loadLanguages()

	Function loadLanguages:TMap()
		Local map:TMap = New TMap

		Local doc:TxmlDoc = TxmlDoc.parseFile("incbin::languages.xml")
		
		If doc Then
			Local root:TxmlNode = doc.getRootElement()
			
			For Local node:TxmlNode = EachIn root.getChildren()
			
				If node.getName() = "lang" Then
				
					Local iso:String = node.getAttribute("iso")
					map.insert(iso, TLanguage.Create(iso, node.getAttribute("language"), node.getAttribute("native")))
					
				End If
			Next
			
			doc.free()
		End If
		
		Return map
	End Function
	
	Function GetLanguage:String(locale:String, native:Int = False)
		If native Then
			Return TLanguage(languages.ValueForKey(locale)).getNative()
		Else
			Return TLanguage(languages.ValueForKey(locale)).language
		End If
	End Function
	
	Function getLocales:String[]()
		Local locales:String[] = New String[100]
		
		Local count:Int = 0
		For Local key:String = EachIn languages.Keys()
			count:+ 1
			If count > locales.length Then
				locales = locales[..count + 20]
			End If
			
			locales[count-1] = key
		Next
		
		Return locales[..count]
	End Function
	
End Type

Type TLanguage

	Field iso:String
	Field language:String
	Field native:String

	Function Create:TLanguage(iso:String, language:String, native:String)
		Local this:TLanguage = New TLanguage
		
		this.iso = iso
		this.language = language
		this.native = native
		
		Return this
	End Function
	
	Method getNative:String()
		If native And native.length > 0 Then
			Return native
		End If
		Return language
	End Method
	
End Type