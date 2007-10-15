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

Type TLoc

	Field modified:Int = False

	Field textMap:TMap = New TMap
	Field langMap:TMap = New TMap

	Field defaultLang:String
	
	Method addText(tag:String, text:String, locale:String, value:String)
'Print "addText('" + tag + "', '" + text + "', '" + locale + "', '" + value + "')"
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(tag))
		
		Local v:Int = 0
		If value Then
			v = value.toInt()
		End If
		
		If lt = Null Then
			If locale Then
				textMap.insert(tag, TLocEntry.Create(tag, text, locale, v))
			End If
		Else
			If locale Then
				lt.addLocale(text, locale, v)
			End If
		End If
	
		If locale Then
			addLanguage(locale)
		End If
	End Method
	
	Method tagExists:Int(tag:String)
		Return textMap.ValueForKey(tag) <> Null
	End Method

	Method localizedTagExists:Int(tag:String, lang:String)
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(tag))
		If lt Then
			Return lt.locales.ValueForKey(lang) <> Null
		End If
	End Method
	
	Method addLanguage(language:String)
		If Not defaultLang Then
			defaultLang = language
		End If
	
		Local lang:String = String(langMap.ValueForKey(language))
		
		If Not lang Then
			langMap.insert(language, language)
		End If
	End Method

	Method getText:String(tag:String, locale:String = Null)
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(tag))
		
		If lt Then
			If locale Then
				Return lt.getText(locale)
			End If
		End If
		
	End Method
	
	Method renameTag(oldName:String, newName:String)
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(oldName))
		
		If lt Then
			lt.tag = newName
			
			textMap.remove(oldName)
			textMap.insert(newName, lt)
		End If
		
		modified = True
	End Method

	Method getValue:String(tag:String, locale:String = Null)
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(tag))
		
		If lt Then
			If locale Then
				Return lt.getValue(locale)
			End If
		End If
		
	End Method

	Method getLocales:String[]()
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
	End Method

	Method getTags:String[]()
		Local tags:String[] = New String[100]
		
		Local count:Int = 0
		For Local key:String = EachIn textMap.Keys()
			count:+ 1
			If count > tags.length Then
				tags = tags[..count + 5]
			End If
			
			tags[count-1] = key
		Next
		
		Return tags[..count]
	End Method
	
	Method deleteTag(tag:String)
		Local lt:TLocEntry = TLocEntry(textMap.ValueForKey(tag))
		
		If lt Then
			lt.free()
			textMap.remove(tag)
			
			modified = True
		End If
		
	End Method
	
	Method free()
		langMap.clear()
		For Local entry:TLocEntry = EachIn textMap.Values()
			entry.free()
		Next
		textMap.clear()
	End Method
	
End Type

Type TLocEntry
	Field tag:String
	Field locales:TMap = New TMap

	Function Create:TLocEntry(tag:String, text:String, locale:String, value:Int = 0)
		Local this:TLocEntry = New TLocEntry
		
		this.tag = tag
		this.locales.insert(locale, TLocPart.Create(text, value))
		
		Return this
	End Function

	Method addLocale(text:String, locale:String, value:Int = 0)
		locales.insert(locale, TLocPart.Create(text, value))
	End Method
	
	Method getText:String(locale:String)
		Local lp:TLocPart = TLocPart(locales.ValueForKey(locale))
		If lp Then
			Return lp.text
		Else
			Return ""
		End If
	End Method
	
	Method getValue:String(locale:String)
		Local lp:TLocPart = TLocPart(locales.ValueForKey(locale))
		If lp Then
			Local v:Int = lp.value
			If v <> 0 Then
				Return String.fromInt(v)
			Else
				Return ""
			End If
		Else
			Return ""
		End If
	End Method
	
	Method getLocales:String[]()
		Local locs:String[] = New String[5]
			
		Local count:Int = 0
		For Local key:String = EachIn locales.Keys()
			count:+ 1
			If count > locs.length Then
				locs = locs[..count + 5]
			End If
				
			locs[count-1] = key
		Next
		
		Return locs[..count]
	End Method

	
	Method free()
		locales.clear()
	End Method
	
End Type

Type TLocPart
	Field text:String
	Field value:Int

	Function Create:TLocPart(text:String, value:Int = 0)
		Local this:TLocPart = New TLocPart
		
		this.text = text
		this.value = value
		
		Return this
	End Function
End Type
