' Copyright (c) 2007,2008 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
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

Import BRL.Blitz

Import "glue.cpp"
Import "strptime.bmx"

Const DBTYPE_STRING:Int = 1
Const DBTYPE_INT:Int = 2
Const DBTYPE_DOUBLE:Int = 3
Const DBTYPE_FLOAT:Int = 4
Const DBTYPE_LONG:Int = 5
Const DBTYPE_DATE:Int = 6
Const DBTYPE_BLOB:Int = 7
Const DBTYPE_DATETIME:Int = 8
Const DBTYPE_TIME:Int = 9

Rem
bbdoc: The Base for database field types.
about: This type, and its sub-types are used to transparently convert data between BlitzMax
and the database.<br>
Currently implemented types are,
<ul>
<li>TDBString</li>
<li>TDBInt</li>
<li>TDBDouble</li>
<li>TDBFloat</li>
<li>TDBLong</li>
<li>TDBDate (see note below)</li>
<li>TDBBlob (see note below)</li>
</ul>
<p>
<b>Please Note:</b> Currently TDBDate and TDBBlob are not fully supported/implemented. I still haven't decided on the
best way to do these - especially the date stuff.<br>
I'm thinking of adding support for Date, DateTime and Time type fields, which are generally supported on
most databases. The only problem is, Blitz doesn't really handle them yet !!<br>
Perhaps I need to build a Date/Calendar module... :-p
</p>
End Rem
Type TDBType

	Field _isNull:Int = True

	Method clear() Abstract
	Method kind:Int() Abstract
	
	Method getString:String()
		Return Null
	End Method
	
	Method isNull:Int()
		Return _isNull
	End Method
		
	Method getInt:Int()
		Return 0
	End Method
	
	Method getDouble:Double()
		Return 0
	End Method
	
	Method getFloat:Float()
		Return 0
	End Method

	Method getLong:Long()
		Return 0
	End Method

	Method getDate:Long()
		Return 0
	End Method
	
	Method getBlob:Byte Ptr()
		Return Null
	End Method

	Method setLong(v:Long)
		Assert 0, "setLong not supported on this TDBType"
	End Method

	Method setDouble(v:Double)
		Assert 0, "setDouble not supported on this TDBType"
	End Method

	Method setString(v:String)
		Assert 0, "setString not supported on this TDBType"
	End Method

	Method setInt(v:Int)
		Assert 0, "setInt not supported on this TDBType"
	End Method

	Method setFloat(v:Float)
		Assert 0, "setFloat not supported on this TDBType"
	End Method

	Method setDate(v:Long)
		Assert 0, "setDate not supported on this TDBType"
	End Method
	
	Method setBlob(v:Byte Ptr, size:Int)
		Assert 0, "setBlob not supported on this TDBType"
	End Method
	
	Method size:Int()
		Assert 0, "size not supported on this TDBType"
	End Method
End Type

Rem
bbdoc: A String-type field.
End Rem
Type TDBString Extends TDBType
	Field value:String
	
	Rem
	bbdoc: Creates a new TDBString object with @value.
	End Rem
	Function Set:TDBString(value:String)
		Local this:TDBString = New TDBString
		this.setString(value)
		Return this
	End Function
	
	Rem
	bbdoc: Returns the string value.
	returns: The string or Null.
	End Rem
	Method getString:String()
		Return value
	End Method

	Rem
	bbdoc: Sets the string value.
	End Rem
	Method setString(v:String)
		If v Then
			value = v
			_isNull = False
		Else
			value = Null
			_isNull = True
		End If
	End Method

	Rem
	bbdoc: Returns the size of the string.
	returns: The size, or 0 if Null.
	End Rem
	Method size:Int()
		If value Then
			Return value.length
		End If
		Return 0
	End Method

	Method clear()
		value = Null
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_STRING
	End Method
End Type

Rem
bbdoc: An Integer-type field.
End Rem
Type TDBInt Extends TDBType
	Field value:Int

	Rem
	bbdoc: Creates a new TDBInt object with @value.
	End Rem
	Function Set:TDBInt(value:Int)
		Local this:TDBInt = New TDBInt
		this.setInt(value)
		Return this
	End Function
	
	Rem
	bbdoc: Returns the int value.
	End Rem
	Method getInt:Int()
		Return value
	End Method
	
	' Allows to return as a long.
	Rem
	bbdoc: Returns a long representation of the value.
	End Rem
	Method getLong:Long()
		Return Long(value)
	End Method

	Rem
	bbdoc: Sets the int value.
	End Rem
	Method setInt(v:Int)
		value = v
		_isNull = False
	End Method

	Method clear()
		value = 0
		_isNull = True
	End Method
	
	Method kind:Int()
		Return DBTYPE_INT
	End Method

End Type

Rem
bbdoc: A Double-type field.
End Rem
Type TDBDouble Extends TDBType
	Field value:Double

	Rem
	bbdoc: Creates a new TDBDouble object with @value.
	End Rem
	Function Set:TDBDouble(value:Double)
		Local this:TDBDouble = New TDBDouble
		this.setDouble(value)
		Return this
	End Function

	Rem
	bbdoc: Returns the double value.
	End Rem
	Method getDouble:Double()
		Return value
	End Method

	' since doubles and floats are similar... allow doubles to return Floats too.
	Rem
	bbdoc: Returns the float representation of the double value.
	End Rem
	Method getFloat:Float()
		Return Float(value)
	End Method

	Rem
	bbdoc: Sets the double value.
	End Rem
	Method setDouble(v:Double)
		value = v
		_isNull = False
	End Method

	Method clear()
		value = 0
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_DOUBLE
	End Method

End Type

Rem
bbdoc: A Float-type field.
End Rem
Type TDBFloat Extends TDBType
	Field value:Float
	
	Rem
	bbdoc: Creates a new TDBFloat object with @value.
	End Rem
	Function Set:TDBFloat(value:Float)
		Local this:TDBFloat = New TDBFloat
		this.setFloat(value)
		Return this
	End Function

	Rem
	bbdoc: Returns the float value.
	End Rem
	Method getFloat:Float()
		Return value
	End Method
	
	Rem
	bbdoc: Returns the double representation of the float value.
	End Rem
	Method getDouble:Double()
		Return Double(value)
	End Method

	Rem
	bbdoc: Sets the float value.
	End Rem
	Method setFloat(v:Float)
		value = v
		_isNull = False
	End Method

	Method clear()
		value = 0
		_isNull = True
	End Method
	
	Method kind:Int()
		Return DBTYPE_FLOAT
	End Method

End Type

Rem
bbdoc: A Long-type field.
End Rem
Type TDBLong Extends TDBType
	Field value:Long

	Rem
	bbdoc: Creates a new TDBLong object with @value.
	End Rem
	Function Set:TDBLong(value:Long)
		Local this:TDBLong = New TDBLong
		this.setLong(value)
		Return this
	End Function

	Rem
	bbdoc: Returns the long value.
	End Rem
	Method getLong:Long()
		Return value
	End Method
	
	Rem
	bbdoc: Returns the int representation of the long value.
	End Rem
	Method getInt:Int()
		Return Int(value)
	End Method

	Rem
	bbdoc: Sets the long value.
	End Rem
	Method setLong(v:Long)
		value = v
		_isNull = False
	End Method
	
	Method clear()
		value = 0
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_LONG
	End Method

End Type

Type TDBDateBase Extends TDBType

	Field value:Long

	Method format:String(fmt:String) Abstract
	
End Type


Rem
bbdoc: A Date-type field.
about: <b>Note:</b> This type may change!
End Rem
Type TDBDate Extends TDBDateBase

	Field _year:Int = 1900
	Field _month:Int = 1
	Field _day:Int = 1

	Function Set:TDBDate(year:Int, Month:Int, day:Int)
		Local this:TDBDate = New TDBDate 
		this.setFromParts(year, Month, day)
		Return this
	End Function

	Function SetWithLong:TDBDate(value:Long)
		Local this:TDBDate = New TDBDate
		this.setDate(value)
		Return this
	End Function
	
	Rem
	bbdoc: Creates a TDBDate from a string.
	about: The date should be in the format: YYYY-MM-DD
	End Rem
	Function SetFromString:TDBDate(date:String)
		Local y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int
		If dbStrptime(date, "%Y-%m-%d", y, m, d, hh, mm, ss)
			Return Set(y, m, d)
		End If
	End Function

	Method getDate:Long()
		Return value
	End Method
	
	Method getYear:Int()
		Return _year
	End Method
	
	Method getMonth:Int()
		Return _month
	End Method
	
	Method getDay:Int()
		Return _day
	End Method
	
	Method setFromParts(y:Int, m:Int, d:Int)
		_year = y
		_month = m
		_day = d

		_calcDateValue(Varptr value, _year, _month, _day, 0, 0, 0)
		_isNull = False
	End Method

	Rem
	bbdoc: Formats the DateTime using the specified formatting
	End Rem
	Method format:String(fmt:String = "%Y-%m-%d")
		Return _formatDate:String(fmt, _year, _month, _day, 0, 0, 0)
	End Method

	Method getString:String()
		Return format()
	End Method
	
	Method setDate(v:Long)
		value = v
		_isNull = False
	End Method

	Method clear() 
		value = 0
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_DATE
	End Method

End Type

Rem
bbdoc: A Blob-type field.
about: A blob is binary data, like an image.<br>
Many databases allow storage of binary data.
End Rem
Type TDBBlob Extends TDBType
	Field value:Byte Ptr
	Field _size:Int

	Function Set:TDBBlob(value:Byte Ptr, size:Int)
		Local this:TDBBlob = New TDBBlob
		this.setBlob(value, size)
		Return this
	End Function

	Method getBlob:Byte Ptr()
		Return value
	End Method

	Method setBlob(v:Byte Ptr, _size:Int)
		If value Then
			' clear first, or we have memory lying about!
			clear()
		End If
		
		If _size > 0 Then
			value = MemAlloc(_size)
			MemCopy(value, v, _size)
		End If
		_isNull = False
	End Method

	Method size:Int()
		Return _size
	End Method

	Method clear()
		If value Then
			MemFree(value)
			value = Null
			_isNull = True
		End If
	End Method

	Method kind:Int()
		Return DBTYPE_BLOB
	End Method

	Method Delete()
		clear()
	End Method
	
End Type

Rem
bbdoc: A DateTime-type field.
about: <b>Note:</b> This type may change!
End Rem
Type TDBDateTime Extends TDBDateBase 

	Field _year:Int = 1900
	Field _month:Int = 1
	Field _day:Int = 1
	Field _hour:Int
	Field _minute:Int
	Field _second:Int
	Field value:Long

	Function Set:TDBDateTime(year:Int, Month:Int, day:Int, hours:Int, mins:Int, secs:Int)
		Local this:TDBDateTime = New TDBDateTime
		this.setFromParts(year, Month, day, hours, mins, secs)
		Return this
	End Function

	Function SetWithLong:TDBDateTime(value:Long)
		Local this:TDBDateTime = New TDBDateTime
		this.setDate(value)
		Return this
	End Function

	Rem
	bbdoc: Creates a TDBDateTime from a string.
	about: The datetime should be in the format: YYYY-MM-DD HH:MM:SS
	End Rem
	Function SetFromString:TDBDateTime(date:String)
		Local y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int
		If dbStrptime(date, "%Y-%m-%d %H:%M:%S", y, m, d, hh, mm, ss)
			Return Set(y, m, d, hh, mm, ss)
		End If
	End Function

	Method setFromParts(y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int)
		_year = y
		_month = m
		_day = d
		_hour = hh
		_minute = mm
		_second = ss
		
		_calcDateValue(Varptr value, _year, _month, _day, _hour, _minute, _second)
		_isNull = False
	End Method

	Method getDate:Long()
		Return value
	End Method

	Method getYear:Int()
		Return _year
	End Method
	
	Method getMonth:Int()
		Return _month
	End Method
	
	Method getDay:Int()
		Return _day
	End Method
	
	Method getHour:Int()
		Return _hour
	End Method
	
	Method getMinute:Int()
		Return _minute
	End Method
	
	Method getSecond:Int()
		Return _second
	End Method
	
	Rem
	bbdoc: Formats the DateTime using the specified formatting
	End Rem
	Method format:String(fmt:String = "%Y-%m-%d %H:%M:%S")
		Return _formatDate:String(fmt, _year, _month, _day, _hour, _minute, _second)
	End Method

	Method getString:String()
		Return format()
	End Method

	Method setDate(v:Long)
		value = v

		_isNull = False
	End Method

	Method clear() 
		value = 0
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_DATETIME
	End Method

End Type

Rem
bbdoc: A Time-type field.
about: <b>Note:</b> This type may change!
End Rem
Type TDBTime Extends TDBDateBase 

	Field _hour:Int
	Field _minute:Int
	Field _second:Int
	Field value:Long

	Function Set:TDBTime(hours:Int, mins:Int, secs:Int)
		Local this:TDBTime = New TDBTime 
		this.setFromParts(hours, mins, secs)
		Return this
	End Function

	Function SetWithLong:TDBTime(value:Long)
		Local this:TDBTime = New TDBTime
		this.setDate(value)
		Return this
	End Function

	Rem
	bbdoc: Creates a TDBTime from a string.
	about: The datetime should be in the format: YYYY-MM-DD HH:MM:SS
	End Rem
	Function SetFromString:TDBTime(date:String)
		Local y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int
		If dbStrptime(date, "%H:%M:%S", y, m, d, hh, mm, ss)
			Return Set(hh, mm, ss)
		End If
	End Function

	Method getDate:Long()
		Return value
	End Method

	Method setDate(v:Long)
		value = v
		Local time:Long = v
		_second = time Mod 60
		time :/ 60
		_minute = time Mod 60
		time :/60
		_hour = time Mod 24
		_isNull = False
	End Method

	Method setFromParts(hh:Int, mm:Int, ss:Int)
		_hour = hh
		_minute = mm
		_second = ss
		
		_calcDateValue(Varptr value, 1900, 1, 0, _hour, _minute, _second)
		_isNull = False
	End Method

	Method getHour:Int()
		Return _hour
	End Method
	
	Method getMinute:Int()
		Return _minute
	End Method
	
	Method getSecond:Int()
		Return _second
	End Method

	Rem
	bbdoc: Formats the DateTime using the specified formatting
	End Rem
	Method format:String(fmt:String = "%H:%M:%S")
		Return _formatDate:String(fmt, 1900, 1, 0, _hour, _minute, _second)
	End Method

	Method getString:String()
		Return format()
	End Method

	Method clear() 
		value = 0
		_isNull = True
	End Method

	Method kind:Int()
		Return DBTYPE_TIME
	End Method

End Type

Extern
	Function _calcDateValue(value:Long Ptr, y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int)
	Function 	_formatDate:String(format:String, y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int)
End Extern


