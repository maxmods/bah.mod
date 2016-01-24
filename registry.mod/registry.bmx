' Copyright (c) 2016 Bruce A Henderson
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
bbdoc: Windows Registry access.
End Rem
Module BaH.Registry

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2016 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release."

?win32

Import "common.bmx"


Rem
bbdoc: Creates the specified registry key.
about: If the key already exists, the function opens it. Note that key names are not case sensitive.
@key is a handle to an open registry key, or one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_CURRENT_CONFIG or HKEY_USERS.
@subKey is the name of a subkey that this function opens or creates.
End Rem
Function RegCreateKey:TRegistryKey(key:Int, subKey:String, samDesired:Int = KEY_QUERY_VALUE | KEY_CREATE_SUB_KEY | KEY_SET_VALUE | KEY_ENUMERATE_SUB_KEYS)
	
	Local res:Int
	Local disp:Int
	
	Local err:Int = bmx_reg_RegCreateKey(key, subKey, "", REG_OPTION_NON_VOLATILE, samDesired, Null, Varptr res, Varptr disp)

	If Not err Then
	
		Local regKey:TRegistryKey = New TRegistryKey
	
		regKey.root = key
		regKey.key = res
		regKey.path = subKey
		regKey.disposition = disp
		regKey.samDesired = samDesired
	
		Return regKey
	Else
		' error
	End If
	
End Function

Rem
bbdoc: Opens the specified registry key.
about: Note that key names are not case sensitive.
End Rem
Function RegOpenKey:TRegistryKey(key:Int, subKey:String, samDesired:Int = KEY_QUERY_VALUE | KEY_CREATE_SUB_KEY | KEY_SET_VALUE | KEY_ENUMERATE_SUB_KEYS)

	Local res:Int
	Local err:Int = bmx_reg_RegOpenKey(key, subKey, samDesired, Varptr res)

	If Not err Then

		Local regKey:TRegistryKey = New TRegistryKey
	
		regKey.root = key
		regKey.key = res
		regKey.path = subKey
		regKey.samDesired = samDesired
	
		Return regKey
	Else
		' error
	End If

End Function

Rem
bbdoc: 
End Rem
Function RegDeleteKey:Int(key:Int, subKey:String, samDesired:Int = 0, recurse:Int = True)

	' try to delete ourself
	Local res:Int = bmx_reg_RegDeleteKey(key, subKey, samDesired)
	
	' deleted?
	If Not res Then
		Return True
	End If
	
	' not deleted, we'll need to recurse into the sub keys
	If recurse Then
	
		Local reg:TRegistryKey = RegOpenKey(key, subKey, KEY_READ)
		
		If reg Then
		
			If Not subKey.EndsWith("\") Then
				subKey :+ "\"
			End If
			
			' enumerate subkeys for this key
			Local enum:TRegisterKeyEnumerator = New TRegisterKeyEnumerator
			enum._key = reg.key
			enum._enumForDelete = True
			enum.NextObject()
	
			Local objEnum:TRegisterEnumerator = New TRegisterEnumerator
			objEnum._enumerator = enum

			For Local node:String = EachIn objEnum
			
				' delete the subkey
				If Not RegDeleteKey(reg.key, node, samDesired) Then
					Exit
				End If
			
			Next
			
			reg.Close()
		
		End If
	
		' now try to delete the key again.
		res = bmx_reg_RegDeleteKey(key, subKey, samDesired)
		
		If Not res Then
			Return True
		End If
		
	End If

End Function



Rem
bbdoc: A handle to a registry key.
End Rem
Type TRegistryKey

	Field root:Int
	Field key:Int
	Field path:String
	Field disposition:Int
	Field samDesired:Int

	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method GetInt:Int(name:String, value:Int Var)
		Return bmx_reg_GetInt(key, name, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method GetLong:Int(name:String, value:Long Var)
		Return bmx_reg_GetLong(key, name, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method GetString:Int(name:String, value:String Var)
		Local res:Int
		value = bmx_reg_GetString(key, name, Varptr res)
		Return res
	End Method

	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method SetInt:Int(name:String, value:Int)
		Return bmx_reg_SetInt(key, name, value)
	End Method
	
	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method SetLong:Int(name:String, value:Long)
		Return bmx_reg_SetLong(key, name, value)
	End Method

	Rem
	bbdoc: 
	returns: 0 for success, otherwise an error code.
	End Rem
	Method SetString:Int(name:String, value:String)
		Return bmx_reg_SetString(key, name, value)
	End Method

	Rem
	bbdoc: Deletes a registry subkey.
	End Rem
	Method DeleteKey:Int(subKey:String = Null, samDesired:Int = 0, recurse:Int = True)
	
		If subKey Then
			Return RegDeleteKey(key, subKey)
		Else
			Return RegDeleteKey(root, path)
		End If
		
	End Method
	
	Rem
	bbdoc: Removes a named value from the registry key.
	about: Note that value names are not case sensitive.
	End Rem
	Method DeleteValue:Int(name:String)
		Return bmx_reg_RegDeleteValue(key, name)
	End Method
	
	Rem
	bbdoc: Enumerates sub keys.
	End Rem
	Method Keys:TRegisterEnumerator()
		Local enum:TRegisterKeyEnumerator = New TRegisterKeyEnumerator
		enum._key = key
		enum.NextObject()
		enum._index = 0
		
		Local objEnum:TRegisterEnumerator = New TRegisterEnumerator
		objEnum._enumerator = enum
		
		Return objEnum
	End Method
	
	Rem
	bbdoc: Enumerates key values.
	End Rem
	Method Values:TRegisterEnumerator()
		Local enum:TRegisterValueEnumerator = New TRegisterValueEnumerator
		enum._key = key
		enum.NextObject()
		enum._index = 0
		
		Local objEnum:TRegisterEnumerator = New TRegisterEnumerator
		objEnum._enumerator = enum
		
		Return objEnum
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Close()
		If key Then
			If key <> HKEY_CLASSES_ROOT And key <> HKEY_CURRENT_USER And key <> HKEY_LOCAL_MACHINE And key <> HKEY_USERS And key <> HKEY_CURRENT_CONFIG Then
				bmx_reg_RegCloseKey(key)
			End If
			key = 0
		End If
	End Method
	
	Method Delete()
		Close()
	End Method
	
End Type

Type TRegisterObjectEnumerator
	Method HasNext:Int() Abstract
	Method NextObject:Object() Abstract
End Type

Type TRegisterKeyEnumerator Extends TRegisterObjectEnumerator

	Field _key:Int
	Field _index:Int
	Field _subkey:String
	Field _hasNext:Int = True
	
	Field _enumForDelete:Int = False

	Method HasNext:Int()
		Return _hasNext
	End Method
	
	Method NextObject:Object()
		If _hasNext Then
			_subkey = bmx_reg_RegEnumKey(_key, _index, Varptr _hasNext)
			
			' for normal enumeration, we increment the index.
			' if we are using the enumerator for deletion, keep the index at zero.
			If Not _enumForDelete Then
				_index :+ 1
			End If
		End If
		
		Return _subKey
	End Method

End Type

Type TRegisterEnumerator
	Method ObjectEnumerator:TRegisterObjectEnumerator()
		Return _enumerator
	End Method
	Field _enumerator:TRegisterObjectEnumerator
End Type

Type TRegisterValueEnumerator Extends TRegisterObjectEnumerator

	Field _key:Int
	Field _index:Int
	Field _value:TRegistryValue
	
	Method HasNext:Int()
		If Not _value Then
			_value = New TRegistryValue
		End If
			
		_value.Reset()
			
		Return bmx_reg_RegEnumValue(_key, _index, _value)
	End Method
	
	Method NextObject:Object()

		_index :+ 1
		
		Return _value
	End Method

End Type

Rem
bbdoc: Read-only registry value.
End Rem
Type TRegistryValue

	Rem
	bbdoc: The name of the value.
	End Rem
	Field name:String
	Rem
	bbdoc: The type of data for the value.
	End Rem
	Field valueType:Int
	
	Rem
	bbdoc: 
	End Rem
	Field vInteger:Int
	Rem
	bbdoc: 
	End Rem
	Field vLong:Long
	Rem
	bbdoc: 
	End Rem
	Field vString:String

	Method Reset()
		name = ""
		valueType = 0
		vInteger = 0
		vLong = 0
		vString = ""
	End Method

	Rem
	bbdoc: Returns the registry value as a String.
	End Rem
	Method ToString:String()
		Select valueType
			Case REG_SZ, REG_EXPAND_SZ
				Return vString
			Case REG_DWORD
				Return vInteger
			Case REG_QWORD
				Return vLong
		End Select
	End Method

	Function _setName(value:TRegistryValue, name:String)
		value.name = name
	End Function

	Function _setValueType(value:TRegistryValue, valueType:Int)
		value.valueType = valueType
	End Function

	Function _setInteger(value:TRegistryValue, vInteger:Int)
		value.vInteger = vInteger
	End Function

	Function _setLong(value:TRegistryValue, vLong:Long)
		value.vLong = vLong
	End Function

	Function _setString(value:TRegistryValue, vString:String)
		value.vString = vString
	End Function
	
End Type

?
