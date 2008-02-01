' Copyright (c) 2008 Bruce A Henderson
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
bbdoc: Mike's Arbitrary Precision Math library
End Rem
Module BaH.MAPM

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: BlitzMax port - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TMAPM

	Field mapmPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateMAPM:TMAPM(value:String = Null)
		Return New TMAPM.Create(value)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TMAPM(value:String = Null)
		mapmPtr = m_apm_init()
		If value Then
			SetString(value)
		End If
		Return Self
	End Method
	
	Function _create:TMAPM(mapmPtr:Byte Ptr)
		If mapmPtr Then
			Local this:TMAPM = New TMAPM
			this.mapmPtr = mapmPtr
			Return this
		End If
	End Function


	Rem
	bbdoc: Sets the MAPM value to the value specified by the string variable.
	about: Integers and floating point are supported as is floating point with scientific notation.
	<ul>
	<li>Lead-in whitespace is ignored.</li>
	<li>A lead-in '+' sign is optional.</li>
	<li>A negative number must have '-' as the first non-whitespace char</li>
	<li>An exponent, 'E' or 'e', is optional.</li>
	<li>The decimal point is optional. The decimal point may be anywhere in the number, but before the exponent.</li>
	<li>The exponent may have an optional '+' sign.</li>
	<li>The exponent must be an integer value (no decimal point)</li>
	</ul>
	End Rem
	Method SetString(value:String)
		m_apm_set_string(mapmPtr, value)
	End Method
		
	Rem
	bbdoc: Sets the MAPM value to the value specified by the int variable.
	End Rem
	Method SetInt(value:Int)
		m_apm_set_long(mapmPtr, value)
	End Method
	
	Rem
	bbdoc: Sets the MAPM value to the value specified by the double variable.
	about: The double value will be rounded to the 15 most significant digits and then converted
	to the MAPM value. If you want an 'exact' conversion, use the SetString method since some
	C floating point library's may round your double in an unpredictable manner.
	End Rem
	Method SetDouble(value:Double)
		m_apm_set_double(mapmPtr, value)
	End Method
	
	Rem
	bbdoc: Converts an MAPM value into a string and is meant to be used with floating point MAPM values.
	about: The output string will always be in scientific (exponential) notation. There will
	be a leading '-' sign for negative numbers. There will be 'decimal_places' number of digits
	after the decimal point. If decimal_places is >= 0, the value will be rounded to that number
	of digits and then the string will be filled, with trailing zero's appended if necessary to
	fill out the decimal place specification. If decimal_places < 0, ALL the significant digits
	of the MAPM number will be output. In some applications, it is convienent to round the value
	yourself (see 'm_apm_round') and then display ALL the digits.
	<p>
	If value = 3.640083E-4
	<pre>
            1)  ToExpString(4)
	        string -> "3.6401E-4"

            2)  ToExpString(14)
	        string -> "3.64008300000000E-4"

            3)  ToExpString(-1)
	        string -> "3.640083E-4"
	</pre>
	</p>
	End Rem
	Method ToExpString:String(decimalPlaces:Int)
		Local buf:Byte[decimalPlaces + 64]
		m_apm_to_string(buf, decimalPlaces, mapmPtr)
		Return String.FromCString(buf)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ToFixtPtString:String(decimalPlaces:Int)
		Local buf:Byte[SignificantDigits() + 16]
		m_apm_to_fixpt_string(buf, decimalPlaces, mapmPtr)
		Return String.FromCString(buf)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ToIntString:String()
		Local buf:Byte[SignificantDigits() + 16]
		m_apm_to_integer_string(buf, mapmPtr)
		Return String.FromCString(buf)
	End Method
	
	Rem
	bbdoc: Returns the absolute MAPM value.
	End Rem
	Method AbsoluteValue:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_absolute_value(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the MAPM value, negated.
	End Rem
	Method Negate:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_negate(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Copy:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_copy(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Round:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_round(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
		
	Rem
	bbdoc: 
	End Rem
	Method Compare:Int(other:Object)
		If TMAPM(other) Then
			Return m_apm_compare(mapmPtr, TMAPM(other).mapmPtr)
		End If
		Return -1
	End Method
	
	Rem
	bbdoc: Returns the sign of the number.
	about: The method will return :
	<pre>
	    -1 : num < 0
	     0 : num = 0
	     1 : num > 0
	</pre>

	End Rem
	Method Sign:Int()
		Return m_apm_sign(mapmPtr)
	End Method
	
	Rem
	bbdoc: Returns the exponent of the number.
	about: <pre>
     If apm_num = 3.86742E+12,    12 will be returned.
                = 9.61082E-56,   -56 will be returned.
                = 0.0              0 will be returned.
	</pre>
	End Rem
	Method Exponent:Int()
		Return m_apm_exponent(mapmPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of significant digits of the number.
	End Rem
	Method SignificantDigits:Int()
		Return m_apm_significant_digits(mapmPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the number is an integer, False otherwise.
	End Rem
	Method IsInteger:Int()
		Return m_apm_is_integer(mapmPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the number is even, False otherwise.
	about: It the number is not an integer, it will result in a warning on stderr and the
	return value is undefined.
	End Rem
	Method IsEven:Int()
		Return m_apm_is_even(mapmPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the number is odd, False otherwise.
	about: It the number is not an integer, it will result in a warning on stderr and the
	return value is undefined.
	End Rem
	Method IsOdd:Int()
		Return m_apm_is_odd(mapmPtr)
	End Method
	
	Rem
	bbdoc: Sets the randon number generator to a known starting value.
	about: The argument should correspond to any *integer* value between 0 and (1.0E+15 - 1)
	End Rem
	Function SetRandomSeed(seed:String)
		m_apm_set_random_seed(seed)
	End Function
	
	Rem
	bbdoc: Returns a random floating point number between the values 0 and 1.
	about: The first time the function is called the generator is initialized with the system
	time. This generator will not repeat its pattern until 1.0E+15 numbers have been generated.
	End Rem
	Function Random:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_get_random(mapm.mapmPtr)
		Return mapm
	End Function
	
	Rem
	bbdoc: Adds @value to the number, returning the result.
	End Rem
	Method Add:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_add(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Subtracts @value from the number, returning the result.
	End Rem
	Method Subtract:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_subtract(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Multiplies @value with the number, returning the result.
	End Rem
	Method Multiply:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_multiply(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Divide:TMAPM(value:TMAPM, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_divide(mapm.mapmPtr, decimalPlaces, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the reciprocal of the number (compute 1.0 / number).
	about: The result will be accurate to the number of decimal places specified.
	<p>
     An input of zero creates a zero result and a warning on stderr.
	</p>
	End Rem
	Method Reciprocal:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_reciprocal(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Divides the number by @value, truncating the result to an integer.
	End Rem
	Method IntegerDivide:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_integer_divide(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Divides the number by @value, truncating the result to an integer and putting the result in @quotient and the remainder in @remainder.
	about: So, 173 / 26 will yield a quotient of 6 and a remainder of 17.
	<p>
	Note that the input numbers do not necessarily have to be
	    integers. This method can be used to split up the integer
	    portion and fractional portion of a floating point number
	    by calling the function with @value set to 'MM_One'. So,
	    32.17042 can be split up into '32' and '0.17042'.
	</p>
	End Rem
	Method IntegerDivRem(value:TMAPM, quotient:TMAPM, remainder:TMAPM)
		m_apm_integer_div_rem(quotient.mapmPtr, remainder.mapmPtr, mapmPtr, value.mapmPtr)
	End Method
	
	Rem
	bbdoc: Computes the factorial of the number and returns the result.
	about: A non-integer number will yield nonsense. Actually, the algorithm simply multiplies
	 : (though 0! and 1! return 1)
	<pre>
	    N * (N - 1) * (N - 2) ... until N < 2.
	</pre>
	End Rem
	Method Factorial:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_factorial(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the number rounded downwards to the nearest integer.
	End Rem
	Method Floor:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_floor(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the number rounded upwards to the nearest integer.
	End Rem
	Method Ceil:TMAPM()
		Local mapm:TMAPM = CreateMAPM()
		m_apm_ceil(mapm.mapmPtr, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Computes the GCD (greatest common divisor) of this number and @value.
	End Rem
	Method GCD:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_gcd(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Computes the LCM (least common multi) of this number and @value.
	End Rem
	Method LCM:TMAPM(value:TMAPM)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_lcm(mapm.mapmPtr, mapmPtr, value.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the square root of the number.
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Sqrt:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_sqrt(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the cube root of the number.
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Cbrt:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_cbrt(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the natural log (base 2.718 ...) of the number.
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Log:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_log(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the common log (base 10) of the number.
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Log10:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_log10(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Performs E ^ the number where 'E' is 2.718... (the exponential function).
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Exp:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_exp(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: Raises the number to @power.
	about: The result will be accurate to the number of decimal places specified.
	End Rem
	Method Pow:TMAPM(power:TMAPM, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_pow(mapm.mapmPtr, decimalPlaces, mapmPtr, power.mapmPtr)
		Return mapm
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method IntPow:TMAPM(power:Int, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_integer_pow(mapm.mapmPtr, decimalPlaces, mapmPtr, power)
		Return mapm
	End Method
	
	Rem
	bbdoc: Returns the number raised to @power.
	about: This method is similiar to IntPow except the result is NOT ROUNDED (Nr). This
	method would typically be used in an integer only application where the full precision
	of the result is desired.
	<p>
	Note that @power is an integer and not a MAPM number.
	</p>
	<p>
    @power must be >= zero. @power < 0 creates a zero result and a warning on stderr.
	</p>
	End Rem
	Method IntPowNr:TMAPM(power:Int, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_integer_pow_nr(mapm.mapmPtr, decimalPlaces, mapmPtr, power)
		Return mapm
	End Method

	Rem
	bbdoc: Returns the sine of the number.
	about: The input angle is assumed to be in degrees. The result will be accurate to
	the number of decimal places specified.
	End Rem
	Method Sin:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_sin(mapm.mapmPtr, decimalPlaces, DegToRad.Multiply(Self).mapmPtr)
		Return mapm
	End Method

	Rem
	bbdoc: Returns the cosine of the number.
	about: The input angle is assumed to be in degrees. The result will be accurate to
	the number of decimal places specified.
	End Rem
	Method Cos:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_cos(mapm.mapmPtr, decimalPlaces, DegToRad.Multiply(Self).mapmPtr)
		Return mapm
	End Method

	Rem
	bbdoc: Returns the sine and cosine of the number.
	about: The input angle is assumed to be in degrees. The result will be accurate to
	the number of decimal places specified, and is more efficient that calling the other
	two methods separately.
	End Rem
	Method SinCos(_sin:TMAPM, _cos:TMAPM, decimalPlaces:Int)
		m_apm_sin_cos(_sin.mapmPtr, _cos.mapmPtr, decimalPlaces, DegToRad.Multiply(Self).mapmPtr)
	End Method

	Rem
	bbdoc: Returns the tangent of the number.
	about: The input angle is assumed to be in degrees. The result will be accurate to
	the number of decimal places specified.
	End Rem
	Method Tan:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_tan(mapm.mapmPtr, decimalPlaces, DegToRad.Multiply(Self).mapmPtr)
		Return mapm
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ArcSin:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_arcsin(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return RadToDeg.Multiply(mapm)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ArcCos:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_arccos(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return RadToDeg.Multiply(mapm)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ArcTan:TMAPM(decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_arctan(mapm.mapmPtr, decimalPlaces, mapmPtr)
		Return RadToDeg.Multiply(mapm)
	End Method

	Rem
	bbdoc: Returns the 4 quadrant arc tangent of 'y' and 'x'.
	about: The function will determine the quadrant based on the signs of the 2 inputs.
	The result will be accurate to the number of decimal places specified.
	End Rem
	Function ArcTan2:TMAPM(y:TMAPM, x:TMAPM, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_arctan2(mapm.mapmPtr, decimalPlaces, y.mapmPtr, x.mapmPtr)
		Return RadToDeg.Multiply(mapm)
	End Function

	Rem
	bbdoc: Returns the 4 quadrant arc tangent of 'y' and 'x'.
	about: The function will determine the quadrant based on the signs of the 2 inputs.
	The result will be accurate to the number of decimal places specified.
	End Rem
	Function ATan2:TMAPM(y:TMAPM, x:TMAPM, decimalPlaces:Int)
		Local mapm:TMAPM = CreateMAPM()
		m_apm_arctan2(mapm.mapmPtr, decimalPlaces, y.mapmPtr, x.mapmPtr)
		Return RadToDeg.Multiply(mapm)
	End Function

	Method Delete()
		If mapmPtr Then
			m_apm_free(mapmPtr)
			mapmPtr = Null
		End If
	End Method

End Type

Global RadToDeg:TMAPM = TMAPM.CreateMAPM("57.2957795130823234")
Global DegToRad:TMAPM = TMAPM.CreateMAPM("0.0174532925199433")

Global MM_Zero:TMAPM = TMAPM.CreateMAPM()
Global MM_One:TMAPM = TMAPM.CreateMAPM("1")
Global MM_Two:TMAPM = TMAPM.CreateMAPM("2")
Global MM_Three:TMAPM = TMAPM.CreateMAPM("3")
Global MM_Four:TMAPM = TMAPM.CreateMAPM("4")
Global MM_Five:TMAPM = TMAPM.CreateMAPM("5")
Global MM_Ten:TMAPM = TMAPM.CreateMAPM("10")

Global MM_PI:TMAPM = TMAPM.CreateMAPM("3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955")

