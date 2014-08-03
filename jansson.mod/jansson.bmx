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
bbdoc: A JSON encoder/decoder.
End Rem
Module BaH.Jansson

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2014 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TJSON

	Field jsonPtr:Byte Ptr

	Function _create:TJSON(jsonPtr:Byte Ptr, jsonType:Int)
		Local this:TJSON
		Select jsonType
			Case JSON_TYPE_OBJECT
				this = New TJSONObject
			Case JSON_TYPE_ARRAY
				this = New TJSONArray
			Case JSON_TYPE_STRING
				this = New TJSONString
			Case JSON_TYPE_INTEGER
				this = New TJSONInteger
			Case JSON_TYPE_REAL
				this = New TJSONReal
			Case JSON_TYPE_TRUE
				this = New TJSONBool
				TJSONBool(this).isTrue = True
			Case JSON_TYPE_FALSE
				this = New TJSONBool
			Case JSON_TYPE_NULL
				this = New TJSONNull
			Default
				Return Null
		End Select
		
		this.jsonPtr = jsonPtr
		
		Return this
	End Function
	
	Rem
	bbdoc: Returns the JSON representation of the object as a String, or NULL on error. 
	End Rem
	Method SaveString:String(flags:Int = 0, indent:Int = 0, precision:Int = 17)
		Return bmx_json_dumps(jsonPtr, flags, indent, precision)
	End Method
	
	Rem
	bbdoc: Writes the JSON representation of the object to the stream output.
	about: The stream should already be open for writing.
	End Rem
	Method SaveStream:Int(stream:TStream, flags:Int = 0, indent:Int = 0, precision:Int = 17)
		' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function Load:TJSON(data:Object, flags:Int = 0, error:TJSONError Var)
	
		If String(data) Then
			' load as text
			Local err:TJSONError = TJSONError(bmx_json_loads(String(data), flags))

			If err._js Then
				Return err._js
			End If
			
			error = err
			
		Else If TStream(data) Then
			' load as stream
			' TODO
		End If
		
		Return Null
	End Function

	Method Delete()
		If jsonPtr Then
			bmx_json_decref(jsonPtr)
			jsonPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A JSON array is an ordered collection of other JSON values.
End Rem
Type TJSONArray Extends TJSON

	Rem
	bbdoc: Creates a new TJSONArray.
	End Rem
	Method Create:TJSONArray()
		jsonPtr = json_array()
		Return Self
	End Method

	Rem
	bbdoc: Returns the number of elements in array, or 0 if array is NULL
	End Rem
	Method Size:Int()
		Return bmx_json_array_size(jsonPtr)
	End Method
	
	Rem
	bbdoc: Returns the element in array at position index.
	about: The valid range for index is from 0 to the return value of Size() minus 1. If index is out of range, NULL is returned.
	End Rem
	Method Get:TJSON(index:Int)
		Return TJSON(bmx_json_array_get(jsonPtr, index))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Set:Int(index:Int, value:TJSON)
		Return bmx_json_array_set(jsonPtr, index, value.jsonPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Append:Int(value:TJSON)
		Return bmx_json_array_append(jsonPtr, value.jsonPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Insert:Int(index:Int, value:TJSON)
		Return bmx_json_array_insert(jsonPtr, index, value.jsonPtr)
	End Method

	Rem
	bbdoc: Removes all elements from array.
	returns: 0 on sucess and -1 on error.
	End Rem
	Method Clear:Int()
		Return json_array_clear(jsonPtr)
	End Method
	
	Rem
	bbdoc: Removes the element in array at position index, shifting the elements after index one position towards the start of the array.
	returns: 0 on success and -1 on error.
	End Rem
	Method Remove:Int(index:Int)
		Return json_array_remove(jsonPtr, index)
	End Method
	
End Type

Rem
bbdoc: A JSON object is a dictionary of key-value pairs, where the key is a Unicode string and the value is any JSON value.
End Rem
Type TJSONObject Extends TJSON

	Rem
	bbdoc: Creates a new TJSONObject.
	End Rem
	Method Create:TJSONObject()
		jsonPtr = json_object()
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the number of elements in the object.
	End Rem
	Method Size:Int()
		Return bmx_json_object_size(jsonPtr)
	End Method
	
	Rem
	bbdoc: Gets a value corresponding to key from the object.
	returns: Null if key is not found or on error.
	End Rem
	Method Get:TJSON(key:String)
		Return TJSON(bmx_json_object_get(jsonPtr, key))
	End Method
	
	Rem
	bbdoc: Sets the value of key to value in the object.
	returns: 0 on success and -1 on error.
	about: If there already is a value for key, it is replaced by the new value. 
	End Rem
	Method Set:Int(key:String, value:TJSON)
		Return bmx_json_object_set_nocheck(jsonPtr, key, value.jsonPtr)
	End Method
	
	Rem
	bbdoc: Deletes key from the object if it exists.
	returns: 0 on success, or -1 if key was not found. 
	End Rem
	Method Del:Int(key:String)
		Return bmx_json_object_del(jsonPtr, key)
	End Method
	
	Rem
	bbdoc: Removes all elements from the object.
	returns: 0 on success, -1 otherwise.
	End Rem
	Method Clear:Int()
		Return json_object_clear(jsonPtr)
	End Method
	
	Rem
	bbdoc: Updates the object with the key-value pairs from @other, overwriting existing keys.
	returns: 0 on success or -1 on error.
	End Rem
	Method Update:Int(other:TJSONObject)
		Return json_object_update(jsonPtr, other.jsonPtr)
	End Method
	
	Rem
	bbdoc: Updates the object with the key-value pairs from @other, but only the values of existing keys are updated.
	returns: 0 on success or -1 on error.
	about: No new keys are created.
	End Rem
	Method UpdateExisting:Int(other:TJSONObject)
		Return json_object_update_existing(jsonPtr, other.jsonPtr)
	End Method

	Rem
	bbdoc: Updates the object with the key-value pairs from @other, but only new keys are created.
	returns: 0 on success or -1 on error.
	about: The value of any existing key is not changed.
	End Rem
	Method UpdateMissing:Int(other:TJSONObject)
		Return json_object_update_missing(jsonPtr, other.jsonPtr)
	End Method
	
End Type

Rem
bbdoc: A JSON String.
End Rem
Type TJSONString Extends TJSON

	Rem
	bbdoc: Creates a new TJSONString.
	End Rem
	Method Create:TJSONString(text:String)
		jsonPtr = bmx_json_string_nocheck(text)
		Return Self
	End Method
	
	Method Value:String()
		Return bmx_json_string_value(jsonPtr)
	End Method
	
End Type

Type JSONNumber Extends TJSON

End Type

Rem
bbdoc: a JSON integer.
End Rem
Type TJSONInteger Extends JSONNumber

	Method Create:TJSONInteger(v:Long)
		jsonPtr = bmx_json_integer(v)
		Return Self
	End Method

	Rem
	bbdoc: Returns the associated value of the integer.
	End Rem
	Method Value:Long()
		Local v:Long
		bmx_json_integer_value(jsonPtr, Varptr v)
	End Method
	
	Rem
	bbdoc: Sets the associated value of integer to @v.
	about: Returns 0 on success, -1 otherwise.
	End Rem
	Method Set:Int(v:Long)
		Return bmx_json_integer_set(jsonPtr, v)
	End Method
	
End Type

Rem
bbdoc: A JSON real number.
End Rem
Type TJSONReal Extends JSONNumber

	Method Create:TJSONReal(v:Double)
		jsonPtr = json_real(v)
		Return Self
	End Method

	Rem
	bbdoc: Returns the associated value of the real.
	End Rem
	Method Value:Double()
		Return json_real_value(jsonPtr)
	End Method

	Rem
	bbdoc: Sets the associated value of real to @v.
	about: Returns 0 on success, -1 otherwise.
	End Rem
	Method Set:Int(v:Double)
		Return json_real_set(jsonPtr, v)
	End Method

End Type

Rem
bbdoc: A JSON boolean.
End Rem
Type TJSONBool Extends TJSON

	Field isTrue:Int

End Type

Rem
bbdoc: A JSON Null.
End Rem
Type TJSONNull Extends TJSON

End Type

Rem
bbdoc: JSON error information.
End Rem
Type TJSONError
	Rem
	bbdoc: The error message, or an empty string if a message is not available.
	End Rem
	Field text:String
	Rem
	bbdoc: Source of the error.
	about: This can be (a part of) the file name or a special identifier in angle brackers (e.g. &lt;string&gt;).
	End Rem
	Field source:String
	Rem
	bbdoc: The line number on which the error occurred.
	End Rem
	Field line:Int
	Rem
	bbdoc: The column on which the error occurred.
	about:  Note that this is the character column, not the byte column, i.e. a multibyte UTF-8 character counts as one column.
	End Rem
	Field column:Int
	Rem
	bbdoc: The position in bytes from the start of the input.
	about: This is useful for debugging Unicode encoding problems.
	End Rem
	Field position:Int
	
	Field _js:TJSON
	
	Function _createError:TJSONError(text:String, source:String, line:Int, column:Int, position:Int)
		Local this:TJSONError = New TJSONError
		this.text = text
		this.source = source
		this.line = line
		this.column = column
		this.position = position
		Return this
	End Function

	Function _createNoError:TJSONError(_js:TJSON)
		Local this:TJSONError = New TJSONError
		this._js = _js
		Return this
	End Function

End Type
