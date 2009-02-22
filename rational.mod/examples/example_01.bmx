SuperStrict

Framework BaH.Rational
Import BRL.StandardIO


Local half:TRational = Rational(1, 2)
Local one:TRational = Rational(1)
Local two:TRational = Rational(2)

' Some basic checks
AssertTrue half.Numerator() = 1
AssertTrue half.denominator() = 2
AssertTrue half.ToDouble() = 0.5

' Arithmetic
AssertTrue half.PlusR(half).EqualsR(one)
AssertTrue one.SubtractR(half).EqualsR(half)
AssertTrue two.MultiplyR(half).EqualsR(one)
AssertTrue one.DivideR(half).EqualsR(two)

' With conversions to integer
AssertTrue half.PlusR(half).Equals(1)
AssertTrue half.Multiply(2).EqualsR(one)
AssertTrue half.Multiply(2).Equals(1)
'AssertTrue TRational(one.DivideR(half)).Equals(2)

' Sign handling
Local minus_half:TRational = Rational(-1, 2)
AssertTrue half.Negative().EqualsR(minus_half)
AssertTrue minus_half.AbsR().EqualsR(half)

' Print some of the above results

Print half.ToString() + " + " + half.ToString() + " = " + one.ToString()
Print one.ToString() + " - " + half.ToString() + " = " + half.ToString()
Print two.ToString() + " * " + half.ToString() + " = " + one.ToString()
Print one.ToString() + " / " + half.ToString() + " = " + two.ToString()

Local Third:TRational = Rational(1, 3)
Local sixth:TRational = Rational(1, 6)
Print Third.ToString() + " * " + sixth.ToString() + " = " + Third.MultiplyR(sixth).ToString()

Function AssertTrue(value:Int)
?debug
	Assert value
?Not debug
	If Not value Then
		Print "Assertion failed"
		End
	End If
?
End Function
