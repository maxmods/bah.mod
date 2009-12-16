' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: GNU Multiple Precision arithmetic library.
End Rem
Module BaH.GMP

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: GMP - 1991-2009 Free Software Foundation"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

Import "common.bmx"


Rem
bbdoc: A GMP integer.
about: Used for performing integer arithmetic.
End Rem
Type GMPInt

	Field gmpPtr:Byte Ptr
	
	Function _create:GMPInt(gmpPtr:Byte Ptr)
		If gmpPtr Then
			Local this:GMPInt = New GMPInt
			this.gmpPtr = gmpPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates and initialises a new GMPInt.
	End Rem
	Function Init:GMPInt()
		Local this:GMPInt = New GMPInt
		this.gmpPtr = bmx_gmp_mpz_init()
		Return this
	End Function
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method Set(op:GMPInt)
		bmx_gmp_mpz_set(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetInt(op:Int)
		bmx_gmp_mpz_setint(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	about: @op is truncated to make it an integer.
	End Rem
	Method SetDouble(op:Double)
		bmx_gmp_mpz_setdouble(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	about: @op is truncated to make it an integer.
	End Rem
	Method SetRational(op:GMPRational)
		bmx_gmp_mpz_setrational(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	about: @op is truncated to make it an integer.
	End Rem
	Method SetFloat(op:GMPFloat)
		bmx_gmp_mpz_setfloat(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the value from a String, in base @base.
	about: The default is base 10.
	<p>
	White space is allowed in the string, and is simply ignored.
	</p>
	<p>
	The base may vary from 2 to 62, or if base is 0, then the leading characters are used: 0x and 0X for hexadecimal, 0b and 0B for binary, 0 for octal,
	or decimal otherwise.
	</p>
	<p>
	For bases up to 36, case is ignored; upper-case and lower-case letters have the same value. For bases 37 to 62, upper-case letter represent the usual
	10..35 while lower-case letter represent 36..61.
	</p>
	<p>
	This method returns 0 if the entire string is a valid number in base base. Otherwise it returns "1.
	</p>
	End Rem
	Method SetStr:Int(s:String, base:Int = 10)
		Return bmx_gmp_mpz_setstr(gmpPtr, s, base)
	End Method
	
	Rem
	bbdoc: Swaps this value with @op efficiently.
	End Rem
	Method Swap(op:GMPInt)
		bmx_gmp_mpz_swap(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: If the value fits into a an int returns the value.
	about: Otherwise returns the least significant part of the value, with the same sign.
	<p>
	If the value is too big to fit in an int, the returned result is probably not very useful. To find out if the value will fit, use the function FitsInt().
	</p>
	End Rem
	Method GetInt:Int()
		Return bmx_gmp_mpz_getint(gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns as a double, truncating if necessary (ie. rounding towards zero).
	about: If the exponent from the conversion is too big, the result is system dependent. An infinity is returned where available.
	A hardware overflow trap may or may not occur.
	End Rem
	Method GetDouble:Double()
		Return bmx_gmp_mpz_getdouble(gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns as a double, truncating if necessary (ie. rounding towards zero), and returning the exponent separately.
	about: The return value is in the range 0.5<=abs(d)<1 and the exponent is stored to *exp. d * 2^exp is the (truncated) op value.
	If op is zero, the return is 0.0 and 0 is stored to *exp.
	<p>
	This is similar to the standard C frexp
	</p>
	End Rem
	Method GetDouble2Exp:Double(_exp:Int Var)
		Return bmx_gmp_mpz_getdouble2exp(gmpPtr, Varptr _exp)
	End Method
	
	Rem
	bbdoc: Returns a string of digits in base @base.
	about: The @base argument may vary from 2 to 62 or from "2 to "36. The default is 10.
	<p>
	For base in the range 2..36, digits and lower-case letters are used; for "2.."36, digits and upper-case letters are used; for 37..62, digits,
	upper-case letters, and lower-case letters (in that significance order) are used.
	</p>
	End Rem
	Method GetStr:String(base:Int = 10)
		Return bmx_gmp_mpz_getstr(gmpPtr, base)
	End Method
	
	Rem
	bbdoc: Adds @op to this value, returning the result.
	End Rem
	Method Add:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_add(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Adds @op to this value, returning the result.
	End Rem
	Method AddInt:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_addint(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Subtracts @op from this value, returning the result.
	End Rem
	Method Sub:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_sub(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Subtracts @op from this value, returning the result.
	End Rem
	Method SubInt:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_subint(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Subtracts this value from @op, returning the result.
	End Rem
	Method IntSub:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_intsub(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Multiplies value by @op, returning the result.
	End Rem
	Method Mul:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_mul(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Multiplies value by @op, returning the result.
	End Rem
	Method MulInt:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_mulint(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Adds the result of @op1 multplied by @op2, to this value.
	End Rem
	Method AddMul(op1:GMPInt, op2:GMPInt)
		bmx_gmp_mpz_addmul(gmpPtr, op1.gmpPtr, op2.gmpPtr)
	End Method
	
	Rem
	bbdoc: Adds the result of @op1 multplied by @op2, to this value.
	End Rem
	Method AddMulInt(op1:GMPInt, op2:Int)
		bmx_gmp_mpz_addmulint(gmpPtr, op1.gmpPtr, op2)
	End Method
	
	Rem
	bbdoc: Subtracts the result of @op1 multplied by @op2, from this value.
	End Rem
	Method SubMul(op1:GMPInt, op2:GMPInt)
		bmx_gmp_mpz_submul(gmpPtr, op1.gmpPtr, op2.gmpPtr)
	End Method
	
	Rem
	bbdoc: Subtracts the result of @op1 multplied by @op2, from this value.
	End Rem
	Method SubMulInt(op1:GMPInt, op2:Int)
		bmx_gmp_mpz_submulint(gmpPtr, op1.gmpPtr, op2)
	End Method
	
	Rem
	bbdoc: Multiplies this value times 2 raised to @op, and returns the result.
	about: This operation can also be defined as a left shift by @op2 bits.
	End Rem
	Method Mul2Exp:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_mul2exp(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Returns the negative of this value.
	End Rem
	Method Neg:GMPInt()
		Return GMPInt._create(bmx_gmp_mpz_neg(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the absolute value.
	End Rem
	Method Absolute:GMPInt()
		Return GMPInt._create(bmx_gmp_mpz_abs(gmpPtr))
	End Method

	Rem
	bbdoc: Divides the value by @d, returning the quotient.
	about: Rounds up towards +infinity. The C stands for 'ceil'.
	End Rem
	Method CDivQ:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_q(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: Result has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivR:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_r(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r.
	about: Rounds the quotient up towards +infinity. Remainder has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivQR:GMPInt(r:GMPInt, d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_qr(gmpPtr, r.gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient up towards +infinity. Remainder has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivQInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_q_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient up towards +infinity. Remainder has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivRInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_r_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r and @remainder.
	about: Rounds the quotient up towards +infinity. Remainder has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivQRInt:GMPInt(r:GMPInt, d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_qr_ui(gmpPtr, r.gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: The result has the opposite sign to @d. The C stands for 'ceil'.
	End Rem
	Method CDivInt:Int(d:Int)
		Return bmx_gmp_mpz_cdiv_ui(gmpPtr, d)
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the quotient.
	about: Rounds up towards +infinity. The C stands for 'ceil'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method CDivQ2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_q_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the remainder.
	about: Result has the opposite sign to 2^b. The C stands for 'ceil'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method CDivR2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_cdiv_r_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient.
	about: 
	End Rem
	Method FDivQ:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_q(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: Result has the opposite sign to @d.
	<p>
	Rounds down towards "infinity. The F stands for 'floor'.
	</p>
	End Rem
	Method FDivR:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_r(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r.
	about: Rounds the quotient down towards -infinity. Remainder has the opposite sign to @d. The F stands for 'floor'.
	End Rem
	Method FDivQR:GMPInt(r:GMPInt, d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_qr(gmpPtr, r.gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient down towards -infinity. Remainder has the opposite sign to @d. The F stands for 'floor'.
	End Rem
	Method FDivQInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_q_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient down towards -infinity. Remainder has the opposite sign to @d. The F stands for 'floor'.
	End Rem
	Method FDivRInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_r_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r and @remainder.
	about: Rounds the quotient down towards -infinity. Remainder has the opposite sign to @d. The F stands for 'floor'.
	End Rem
	Method FDivQRInt:GMPInt(r:GMPInt, d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_qr_ui(gmpPtr, r.gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: The result has the opposite sign to @d. Rounds down towards -infinity. The F stands for 'floor'.
	End Rem
	Method FDivInt:Int(d:Int)
		Return bmx_gmp_mpz_fdiv_ui(gmpPtr, d)
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the quotient.
	about: Rounds down towards -infinity. The F stands for 'floor'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method FDivQ2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_q_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the remainder.
	about: Result has the opposite sign to 2^b. The F stands for 'floor'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method FDivR2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_fdiv_r_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient.
	about: Rounds towards zero. The T stands for 'truncate'.
	End Rem
	Method TDivQ:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_q(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: Result has the opposite sign to @d. Rounds towards zero. The T stands for 'truncate'.
	End Rem
	Method TDivR:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_r(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r.
	about: Rounds the quotient towards zero. Remainder has the opposite sign to @d. The T stands for 'truncate'.
	End Rem
	Method TDivQR:GMPInt(r:GMPInt, d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_qr(gmpPtr, r.gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient towards zero. Remainder has the opposite sign to @d. The T stands for 'truncate'.
	End Rem
	Method TDivQInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_q_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient, and the @remainder.
	about: Rounds the quotient towards zero. Remainder has the opposite sign to @d. The T stands for 'truncate'.
	End Rem
	Method TDivRInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_r_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the quotient and setting the remainder in @r and @remainder.
	about: Rounds the quotient towards zero. Remainder has the opposite sign to @d. The T stands for 'truncate'.
	End Rem
	Method TDivQRInt:GMPInt(r:GMPInt, d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_qr_ui(gmpPtr, r.gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the remainder.
	about: The result has the opposite sign to @d. Rounds towards zero. The T stands for 'truncate'.
	End Rem
	Method TDivInt:Int(d:Int)
		Return bmx_gmp_mpz_tdiv_ui(gmpPtr, d)
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the quotient.
	about: Rounds towards zero. The T stands for 'truncate'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method TDivQ2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_q_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Divides the value by 2^b, returning the remainder.
	about: Result has the opposite sign to 2^b. The T stands for 'truncate'.
	<p>
	This method is implemented as right shifts and bit masks, but of course it rounds the same as the other methods.
	</p>
	End Rem
	Method TDivR2exp:GMPInt(b:Int)
		Return GMPInt._create(bmx_gmp_mpz_tdiv_r_2exp(gmpPtr, b))
	End Method
	
	Rem
	bbdoc: Returns the value mod @d.
	about: The sign of the divisor is ignored; the result is always non-negative.
	End Rem
	Method Modulus:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_mod(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the value mod @d and the @remainder.
	about: The sign of the divisor is ignored; the result is always non-negative.
	End Rem
	Method ModulusInt:GMPInt(d:Int, remainder:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_mod_ui(gmpPtr, d, Varptr remainder))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the result.
	about: Produces correct results only when it is known in advance that @d divides the value.
	<p>
	This routine is much faster than the other division methods, and is the best choice when
	exact division is known to occur, for example reducing a rational to lowest terms. 
	</p>
	End Rem
	Method DivExact:GMPInt(d:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_divexact(gmpPtr, d.gmpPtr))
	End Method
	
	Rem
	bbdoc: Divides the value by @d, returning the result.
	about: Produces correct results only when it is known in advance that @d divides the value.
	<p>
	This routine is much faster than the other division methods, and is the best choice when
	exact division is known to occur, for example reducing a rational to lowest terms. 
	</p>
	End Rem
	Method DivExactInt:GMPInt(d:Int)
		Return GMPInt._create(bmx_gmp_mpz_divexact_ui(gmpPtr, d))
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is exactly divisible by @d.
	about: The value is divisible by @d if there exists an integer q satisfying n = q*d.
	Unlike the other division methods, d=0 is accepted and following the rule it can be seen that only 0
	is considered divisible by 0. 
	End Rem
	Method Divisible:Int(d:GMPInt)
		Return bmx_gmp_mpz_divisible_p(gmpPtr, d.gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is exactly divisible by @d.
	about: The value is divisible by @d if there exists an integer q satisfying n = q*d.
	Unlike the other division methods, d=0 is accepted and following the rule it can be seen that only 0
	is considered divisible by 0. 
	End Rem
	Method DivisibleInt:Int(d:Int)
		Return bmx_gmp_mpz_divisible_ui_p(gmpPtr, d)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is exactly divisible by 2^b.
	about: The value is divisible by @d if there exists an integer q satisfying n = q*d.
	Unlike the other division methods, d=0 is accepted and following the rule it can be seen that only 0
	is considered divisible by 0. 
	End Rem
	Method Divisible2Exp:Int(b:Int)
		Return bmx_gmp_mpz_divisible_2exp_p(gmpPtr, b)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is congruent to @c modulo @d.
	End Rem
	Method Congruent:Int(c:GMPInt, d:GMPInt)
		Return bmx_gmp_mpz_congruent_p(gmpPtr, c.gmpPtr, d.gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is congruent to @c modulo @d.
	End Rem
	Method CongruentInt:Int(c:Int, d:Int)
		Return bmx_gmp_mpz_congruent_ui_p(gmpPtr, c, d)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is congruent to @c modulo 2^b.
	End Rem
	Method Congruent2Exp:Int(c:GMPInt, b:Int)
		Return bmx_gmp_mpz_congruent_2exp_p(gmpPtr, c.gmpPtr, b)
	End Method
	
	Rem
	bbdoc: Returns the value raised to @_exp modulo @_mod.
	about: Negative @_exp is supported if an inverse value^-1 mod @_mod exists.
	If an inverse doesn't exist then a divide by zero is raised. 
	End Rem
	Method PowM:GMPInt(_exp:GMPInt, _mod:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_powm(gmpPtr, _exp.gmpPtr, _mod.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the value raised to @_exp modulo @_mod.
	about: Negative @_exp is supported if an inverse value^-1 mod @_mod exists.
	If an inverse doesn't exist then a divide by zero is raised. 
	End Rem
	Method PowMInt:GMPInt(_exp:Int, _mod:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_powm_ui(gmpPtr, _exp, _mod.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the value raised to @_exp.
	about: The case 0^0 yields 1. 
	End Rem
	Method Pow:GMPInt(_exp:Int)
		Return GMPInt._create(bmx_gmp_mpz_pow_ui(gmpPtr, _exp))
	End Method
	
	Rem
	bbdoc: Returns @base raised to @_exp.
	about: The case 0^0 yields 1.
	End Rem
	Function PowInt:GMPInt(base:Int, _exp:Int)
		Return GMPInt._create(bmx_gmp_mpz_ui_pow_ui(base, _exp))
	End Function
	
	Rem
	bbdoc: Returns the truncated integer part of the nth root of the value.
	about: Returns non-zero if the computation was exact, i.e., if the value is the result to the nth power.
	End Rem
	Method Root:GMPInt(n:Int, result:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_root(gmpPtr, n, Varptr result))
	End Method
	
	Rem
	bbdoc: Returns the truncated integer part of the nth root of the value.
	about: Sets @_rem to the remainder, value"root**n. 
	End Rem
	Method RootRem:GMPInt(_rem:GMPInt, n:Int)
		Return GMPInt._create(bmx_gmp_mpz_rootrem(gmpPtr, _rem.gmpPtr, n))
	End Method
	
	Rem
	bbdoc: Returns the truncated integer part of the square root of the value.
	End Rem
	Method Sqrt:GMPInt()
		Return GMPInt._create(bmx_gmp_mpz_sqrt(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the truncated integer part of the square root of the value, like Sqrt.
	about: Sets @rop to the remainder value"result*result, which will be zero if the value is a perfect square.
	End Rem
	Method SqrtRem:GMPInt(rop:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_sqrtrem(gmpPtr, rop.gmpPtr))
	End Method

	Rem
	bbdoc: Returns non-zero if the value is a perfect power, i.e., if there exist integers a and b, with b>1, such that the value equals a raised to the power b.
	about: Under this definition both 0 and 1 are considered to be perfect powers.
	Negative values of the value are accepted, but of course can only be odd perfect powers.
	End Rem
	Method PerfectPower:Int()
		Return bmx_gmp_mpz_perfect_power_p(gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is a perfect square, i.e., if the square root of the value is an integer.
	about: Under this definition both 0 and 1 are considered to be perfect squares. 
	End Rem
	Method PerfectSquare:Int()
		Return bmx_gmp_mpz_perfect_square_p(gmpPtr)
	End Method

	Rem
	bbdoc: Determines whether the value is prime.
	about: Returns 2 if the value is definitely prime, returns 1 if the value is probably prime (without being certain),
	or returns 0 if value is definitely composite.
	<p>
	This method does some trial divisions, then some Miller-Rabin probabilistic primality tests. @reps controls
	how many such tests are done, 5 to 10 is a reasonable number, more will reduce the
	chances of a composite being returned as "probably prime".
	</p>
	<p>
	Miller-Rabin and similar tests can be more properly called compositeness tests. Numbers which fail are
	known to be composite but those which pass might be prime or might be composite. Only a few
	composites pass, hence those which pass are considered probably prime.
	</p>
	End Rem
	Method ProbabPrime:Int(reps:Int)
		Return bmx_gmp_mpz_probab_prime_p(gmpPtr, reps)
	End Method
	
	Rem
	bbdoc: Returns the next prime greater than the value.
	about: This method uses a probabilistic algorithm to identify primes. For practical purposes it's adequate,
	the chance of a composite passing will be extremely small. 
	End Rem
	Method NextPrime:GMPInt()
		Return GMPInt._create(bmx_gmp_mpz_nextprime(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the greatest common divisor of this value and @op.
	about: The result is always positive even if one or both values are negative.
	End Rem
	Method Gcd:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_gcd(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the greatest common divisor of this value and @op.
	End Rem
	Method GcdInt:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_gcd_ui(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Returns the greatest common divisor of this value and @b, and in addition set s and t to coefficients satisfying this*s + b*t = g. 
	about: The returned value is always positive, even if one or both of this value and @b are negative.
	The values in @s and @t are chosen such that abs(s) <= abs(b) and abs(t) <= abs(this).
	End Rem
	Method GcdExt:GMPInt(s:GMPInt, t:GMPInt, b:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_gcdext(gmpPtr, s.gmpPtr, t.gmpPtr, b.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the least common multiple of this value and @op.
	about: The result is always positive, irrespective of the signs of this value and @op.
	The result will be zero if either this value or @op is zero.  
	End Rem
	Method Lcm:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_lcm(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the least common multiple of this value and @op.
	about: The result is always positive, irrespective of the signs of this value and @op.
	The result will be zero if either this value or @op is zero.  
	End Rem
	Method LcmInt:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_lcm_ui(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Computes the inverse of this value modulo @op.
	about: If the inverse exists, the @result is set to non-zero and the return value will
	satisfy 0 &lt;= value &lt; @op. If an inverse doesn't exist @result is zero and Null is returned. 
	End Rem
	Method Invert:GMPInt(op:GMPInt, result:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_invert(gmpPtr, op.gmpPtr, Varptr result))
	End Method
	
	Rem
	bbdoc: Calculates the Jacobi symbol (value/b).
	about: This is defined only for @b odd. 
	End Rem
	Method Jacobi:Int(b:GMPInt)
		Return bmx_gmp_mpz_jacobi(gmpPtr, b.gmpPtr)
	End Method
	
	Rem
	bbdoc: Calculates the Legendre symbol (value/p).
	about: This is defined only for @p an odd positive prime, and for such @p it's identical to the Jacobi symbol. 
	End Rem
	Method Legendre:Int(p:GMPInt)
		Return bmx_gmp_mpz_legendre(gmpPtr, p.gmpPtr)
	End Method
	
	Rem
	bbdoc: Calculates the Jacobi symbol (a/b) with the Kronecker extension (a/2)=(2/a) when a odd, or (a/2)=0 when a even. 
	about: When b is odd the Jacobi symbol and Kronecker symbol are identical.
	End Rem
	Method Kronecker:Int(b:GMPInt)
		Return bmx_gmp_mpz_kronecker(gmpPtr, b.gmpPtr)
	End Method
	
	Rem
	bbdoc: Calculates the Jacobi symbol (a/b) with the Kronecker extension (a/2)=(2/a) when a odd, or (a/2)=0 when a even. 
	about: When b is odd the Jacobi symbol and Kronecker symbol are identical.
	End Rem
	Method KroneckerInt:Int(b:Int)
		Return bmx_gmp_mpz_kronecker_si(gmpPtr, b)
	End Method
	
	Rem
	bbdoc: Calculates the Jacobi symbol (a/b) with the Kronecker extension (a/2)=(2/a) when a odd, or (a/2)=0 when a even. 
	about: When b is odd the Jacobi symbol and Kronecker symbol are identical.
	End Rem
	Method IntKronecker:Int(a:Int)
		Return bmx_gmp_mpz_si_kronecker(gmpPtr, a)
	End Method
	
	Rem
	bbdoc: Removes all occurrences of the factor @f from this value and return the result.
	about: @count is set with how many such occurrences were removed.
	End Rem
	Method Remove:GMPInt(f:GMPInt, count:Int Var)
		Return GMPInt._create(bmx_gmp_mpz_remove(gmpPtr, f.gmpPtr, Varptr count))
	End Method
	
	Rem
	bbdoc: Returns op!, the factorial of @op.
	End Rem
	Function Fac:GMPInt(op:Int)
		Return GMPInt._create(bmx_gmp_mpz_fac_ui(op))
	End Function
	
	Rem
	bbdoc: Computes the binomial coefficient of this value over @k and returns the result.
	about: Negative values of this value are supported, using the identity bin(-n,k) = (-1)^k * bin(n+k-1,k), see Knuth volume 1 section 1.2.6 part G. 
	End Rem
	Method Bin:GMPInt(k:Int)
		Return GMPInt._create(bmx_gmp_mpz_bin_ui(gmpPtr, k))
	End Method
	
	Rem
	bbdoc: Computes the binomial coefficient of @n over @k and returns the result.
	about: Negative values of @n are supported, using the identity bin(-n,k) = (-1)^k * bin(n+k-1,k), see Knuth volume 1 section 1.2.6 part G. 
	End Rem
	Function BinInt:GMPInt(n:Int, k:Int)
		Return GMPInt._create(bmx_gmp_mpz_bin_uiui(n, k))
	End Function
	
	Rem
	bbdoc: Returns F[n], the n'th Fibonacci number.
	about: These method is designed for calculating isolated Fibonacci numbers. When a sequence
	of values is wanted it's best to start with #Fib2 and iterate the defining F[n+1]=F[n]+F[n-1] or similar.
	End Rem
	Function Fib:GMPInt(n:Int)
		Return GMPInt._create(bmx_gmp_mpz_fib_ui(n))
	End Function
	
	Rem
	bbdoc: Returns F[n], the n'th Fibonacci number, and sets @sub to F[n-1].
	about: These method is designed for calculating isolated Fibonacci numbers. When a sequence
	of values is wanted it's best to start with #Fib2 and iterate the defining F[n+1]=F[n]+F[n-1] or similar.
	End Rem
	Function Fib2:GMPInt(sub:GMPInt, n:Int)
		Return GMPInt._create(bmx_gmp_mpz_fib2_ui(sub.gmpPtr, n))
	End Function
	
	Rem
	bbdoc: Returns L[n], the n'th Lucas number.
	about: This method is designed for calculating isolated Lucas numbers. When a sequence of values
	is wanted it's best to start with #LucNum2 and iterate the defining L[n+1]=L[n]+L[n-1] or similar. 
	End Rem
	Function LucNum:GMPInt(n:Int)
		Return GMPInt._create(bmx_gmp_mpz_lucnum_ui(n))
	End Function
	
	Rem
	bbdoc: Returns L[n], the n'th Lucas number, and sets @sub to L[n-1].
	about: This method is designed for calculating isolated Lucas numbers. When a sequence of values
	is wanted it's best to start with #LucNum2 and iterate the defining L[n+1]=L[n]+L[n-1] or similar. 
	End Rem
	Function LucNum2:GMPInt(sub:GMPInt, n:Int)
		Return GMPInt._create(bmx_gmp_mpz_lucnum2_ui(sub.gmpPtr, n))
	End Function

	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method Cmp:Int(op:GMPInt)
		Return bmx_gmp_mpz_cmp(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method CmpD:Int(op:Double)
		Return bmx_gmp_mpz_cmp_d(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method CpmInt:Int(op:Int)
		Return bmx_gmp_mpz_cmp_si(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Compare the absolute values of this value and @op.
	about: Returns a positive value if abs(Self) &gt; abs(@op), zero if abs(Self) = abs(@op), or a negative value if abs(Self) &lt; abs(@op).
	End Rem
	Method CmpAbs:Int(op:GMPInt)
		Return bmx_gmp_mpz_cmpabs(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Compare the absolute values of this value and @op.
	about: Returns a positive value if abs(Self) > abs(@op), zero if abs(Self) = abs(@op), or a negative value if abs(Self) < abs(@op).
	End Rem
	Method CmpAbsD:Int(op:Double)
		Return bmx_gmp_mpz_cmpabs_d(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Compare the absolute values of this value and @op.
	about: Returns a positive value if abs(Self) > abs(@op), zero if abs(Self) = abs(@op), or a negative value if abs(Self) < abs(@op).
	End Rem
	Method CmpAbsInt:Int(op:Int)
		Return bmx_gmp_mpz_cmpabs_ui(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Returns +1 if the value &tg; 0, 0 if the value = 0, and -1 if the value &lt; 0
	End Rem
	Method Sign:Int()
		Return bmx_gmp_mpz_sgn(gmpPtr)
	End Method

	Rem
	bbdoc: Returns this value bitwise-and @op.
	End Rem
	Method BitAnd:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_and(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns this value bitwise inclusive-or @op.
	End Rem
	Method BitIor:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_ior(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns this value bitwise exclusive-or @op.
	End Rem
	Method BitXor:GMPInt(op:GMPInt)
		Return GMPInt._create(bmx_gmp_mpz_xor(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the one's complement of this value.
	End Rem
	Method BitCom:GMPInt()
		Return GMPInt._create(bmx_gmp_mpz_com(gmpPtr))
	End Method
	
	Rem
	bbdoc: If the value &gt;=0, returns the population count of the value, which is the number of 1 bits in the binary representation.
	about: If the value &lt; 0, the number of 1s is infinite, and the return value is ULONG_MAX, the largest possible int.
	End Rem
	Method PopCount:Int()
		Return bmx_gmp_mpz_popcount(gmpPtr)
	End Method
	
	Rem
	bbdoc: If the value and @op are both &gt;=0 or both &lt;0, returns the hamming distance between the two operands, which is the number of bit positions where the value and @op have different bit values.
	about: If one operand is &gt;=0 and the other &lt;0 then the number of bits different is infinite, and the return value is ULONG_MAX,
	the largest possible unsigned long.
	End Rem
	Method HamDist:Int(op:GMPInt)
		Return bmx_gmp_mpz_hamdist(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Scans the value, starting from bit @startingBit, towards more significant bits, until the first 0 is found.
	returns: The index of the found bit.
	about: If the bit at @startingBit is already what's sought, then @startingBit is returned.
	<p>
	If there's no bit found, then ULONG_MAX is returned. This will happen past the end of a negative number.
	</p>
	End Rem
	Method Scan0:Int(startingBit:Int)
		Return bmx_gmp_mpz_scan0(gmpPtr, startingBit)
	End Method
	
	Rem
	bbdoc: Scans the value, starting from bit @startingBit, towards more significant bits, until the first 1 is found.
	returns: The index of the found bit.
	about: If the bit at @startingBit is already what's sought, then @startingBit is returned.
	<p>
	If there's no bit found, then ULONG_MAX is returned. This will happen past the end of a nonnegative number.
	</p>
	End Rem
	Method Scan1:Int(startingBit:Int)
		Return bmx_gmp_mpz_scan1(gmpPtr, startingBit)
	End Method
	
	Rem
	bbdoc: Sets bit @bitIndex.
	End Rem
	Method SetBit(bitIndex:Int)
		bmx_gmp_mpz_setbit(gmpPtr, bitIndex)
	End Method
	
	Rem
	bbdoc: Clears bit @bitIndex .
	End Rem
	Method ClrBit(bitIndex:Int)
		bmx_gmp_mpz_clrbit(gmpPtr, bitIndex)
	End Method
	
	Rem
	bbdoc: Complements bit @bitIndex. 
	End Rem
	Method ComBit(bitIndex:Int)
		bmx_gmp_mpz_combit(gmpPtr, bitIndex)
	End Method
	
	Rem
	bbdoc: Tests bit @bitIndex in the value and returns 0 or 1 accordingly.
	End Rem
	Method TstBit:Int(bitIndex:Int)
		Return bmx_gmp_mpz_tstbit(gmpPtr, bitIndex)
	End Method
	
	Rem
	bbdoc: Determines whether the value is odd.
	about: Returns non-zero if yes, zero if no.
	End Rem
	Method Odd:Int()
		Return bmx_gmp_mpz_odd_p(gmpPtr)
	End Method
	
	Rem
	bbdoc: Determines whether the value is even.
	about: Returns non-zero if yes, zero if no.
	End Rem
	Method Even:Int()
		Return bmx_gmp_mpz_even_p(gmpPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If gmpPtr Then
			bmx_gmp_mpz_free(gmpPtr)
			gmpPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: A GMP floating point number.
about: The mantissa of each float has a user-selectable precision, limited only by available memory. Each variable has its own
precision, and that can be increased or decreased at any time.
<p>
The exponent of each float is a fixed precision, one machine word on most systems. In the current implementation the exponent is a count of limbs, so
for example on a 32-bit system this means a range of roughly 2^-68719476768 to 2^68719476736. Note however GetStr can only return an exponent which
fits an Int.
</p>
<p>
Each variable keeps a size for the mantissa data actually in use. This means that if a float is exactly represented in only a few bits
then only those bits will be used in a calculation, even if the selected precision is high.
</p>
<p>
All calculations are performed to the precision of the destination variable. Each method is defined to calculate with  infinite precision  followed by
a truncation to the destination precision, but of course the work done is only what's needed to determine a result under that definition.
</p>
<p>
The precision selected for a variable is a minimum value, GMP may increase it a little to facilitate efficient calculation. Currently this means
rounding up to a whole limb, and then sometimes having a further partial limb, depending on the high limb of the mantissa. But applications
shouldn't be concerned by such details.
</p>
<p>
The mantissa in stored in binary, as might be imagined from the fact precisions are expressed in bits. One consequence of this is that
decimal fractions like 0.1 cannot be represented exactly. The same is true of plain IEEE double floats. This makes both highly unsuitable
for calculations involving money or other values that should be exact decimal fractions. (Suitably scaled integers, or perhaps rationals, are better choices.)
</p>
<p>
Note that the GMPFloat methods are not intended as a smooth extension to IEEE P754 arithmetic.
</p>
End Rem
Type GMPFloat

	Field gmpPtr:Byte Ptr

	Function _create:GMPFloat(gmpPtr:Byte Ptr)
		If gmpPtr Then
			Local this:GMPFloat = New GMPFloat
			this.gmpPtr = gmpPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates and initialises a new GMPFloat, optionally setting the precision to be at least @prec bits.
	about: If not set, the current default precision will be used.
	End Rem
	Function Init:GMPFloat(prec:Int = 0)
		Local this:GMPFloat = New GMPFloat
		this.gmpPtr = bmx_gmp_mpf_init(prec)
		Return this
	End Function
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method Set(op:GMPFloat)
		bmx_gmp_mpf_set(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the default precision to be at least @prec bits.
	about: All subsequent creations of GMPFloats will use this precision, but previously initialized variables are unaffected.
	End Rem
	Function SetDefaultPrec(prec:Int)
		bmx_gmp_mpf_set_default_prec(prec)
	End Function
	
	Rem
	bbdoc: Returns the default precision actually used.
	End Rem
	Function GetDefaultPrec:Int()
		Return bmx_gmp_mpf_get_default_prec()
	End Function
	
	Rem
	bbdoc: Return the current precision, in bits.
	End Rem
	Method GetPrec:Int()
		Return bmx_gmp_mpf_get_prec(gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the precision to be at least @prec bits.
	about: The value will be truncated to the new precision.
	<p>
	This method requires a call to realloc, and so should not be used in a tight loop.
	</p>
	End Rem
	Method SetPrec(prec:Int)
		bmx_gmp_mpf_set_prec(gmpPtr, prec)
	End Method

	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetInt(op:Int)
		bmx_gmp_mpf_setint(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetFloat(op:Float)
		setDouble(op)
	End Method

	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetDouble(op:Double)
		bmx_gmp_mpf_setdouble(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetRational(op:GMPRational)
		bmx_gmp_mpf_setrational(gmpPtr, op.gmpPtr)
	End Method

	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method SetInteger(op:GMPInt)
		bmx_gmp_mpf_setinteger(gmpPtr, op.gmpPtr)
	End Method

	Rem
	bbdoc: Sets the value from a String, in base @base.
	about: The string is of the form 'M@N' or, if the base is 10 or less, alternatively 'MeN'. 'M' is the mantissa and 'N' is the exponent.
	The mantissa is always in the specified base. The exponent is either in the specified base or, if base is negative, in
	decimal. The decimal point expected is taken from the current locale, on systems providing localeconv.
	<p>
	The default is base 10.
	</p>
	<p>
	White space is allowed in the string, and is simply ignored.
	</p>
	<p>
	The base may vary from 2 to 62, or if base is 0, then the leading characters are used: 0x and 0X for hexadecimal, 0b and 0B for binary, 0 for octal,
	or decimal otherwise.
	</p>
	<p>
	For bases up to 36, case is ignored; upper-case and lower-case letters have the same value. For bases 37 to 62, upper-case letter represent the usual
	10..35 while lower-case letter represent 36..61.
	</p>
	<p>
	Unlike the corresponding GMPInt method, the base will not be determined from the leading characters of the string if base is 0.
	This is so that numbers like '0.23' are not interpreted as octal.
	</p>
	<p>
	White space is allowed in the string, and is simply ignored. [This is not really true; white-space is ignored in the beginning
	of the string and within the mantissa, but not in other places, such as after a minus sign or in the exponent. We are considering
	changing the definition of this method, making it fail when there is any white-space in the input, since that makes a lot of
	sense.]
	</p>
	<p>
	This method returns 0 if the entire string is a valid number in base base. Otherwise it returns "1.
	</p>
	End Rem
	Method SetStr:Int(s:String, base:Int = 10)
		Return bmx_gmp_mpf_setstr(gmpPtr, s, base)
	End Method
	
	Rem
	bbdoc: Swaps this value with @op efficiently.
	about: Both the values and the precisions of the two variables are swapped.
	End Rem
	Method Swap(op:GMPFloat)
		bmx_gmp_mpf_swap(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns a double, truncating if necessary (ie. rounding towards zero).
	about: If the exponent is too big or too small to fit a double then the result is system dependent. For too big an infinity
	is returned when available. For too small 0.0 is normally returned. Hardware overflow, underflow and denorm traps may or may not occur.
	End Rem
	Method GetDouble:Double()
		Return bmx_gmp_mpf_get_d(gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns a double, truncating if necessary (ie. rounding towards zero), and with an exponent returned separately.
	about: The return value is in the range 0.5&ltg;=abs(d)&lt;1 and the exponent is stored to @_exp. d * 2^exp is the (truncated) value. 
	If the value is zero, the return is 0.0 and 0 is stored to @_exp.
	End Rem
	Method GetDouble2Exp:Double(_exp:Int Var)
		Return bmx_gmp_mpf_get_d_2exp(gmpPtr, Varptr _exp)
	End Method
	
	Rem
	bbdoc: Returns the Int value, truncating any fraction part.
	about: If the value is too big for the Int, the result is undefined.
	End Rem
	Method GetInt:Int()
		Return bmx_gmp_mpf_get_si(gmpPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetStr:String(_exp:Int Var, base:Int = 10)
		Return bmx_gmp_mpf_get_str(gmpPtr, Varptr _exp, base)
	End Method

	Rem
	bbdoc: Adds @op to this value, returning the result.
	End Rem
	Method Add:GMPFloat(op:GMPFloat)
		Return GMPFloat._create(bmx_gmp_mpf_add(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Adds @op to this value, returning the result.
	End Rem
	Method AddInt:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_addint(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Subtracts @op from this value, returning the result.
	End Rem
	Method Sub:GMPFloat(op:GMPFloat)
		Return GMPFloat._create(bmx_gmp_mpf_sub(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Subtracts @op from this value, returning the result.
	End Rem
	Method SubInt:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_subint(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Subtracts this value from @op, returning the result.
	End Rem
	Method IntSub:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_intsub(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Multiplies value by @op, returning the result.
	End Rem
	Method Mul:GMPFloat(op:GMPFloat)
		Return GMPFloat._create(bmx_gmp_mpf_mul(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Multiplies value by @op, returning the result.
	End Rem
	Method MulInt:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_mulint(gmpPtr, op))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Div:GMPFloat(op:GMPFloat)
		Return GMPFloat._create(bmx_gmp_mpf_div(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DivInt:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_ui_div(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method IntDiv:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_div_ui(gmpPtr, op))
	End Method

	Rem
	bbdoc: Returns the square root of this value.
	End Rem
	Method Sqrt:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_sqrt(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the square root of @op.
	End Rem
	Function SqrtInt:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_sqrt_ui(op))
	End Function

	Rem
	bbdoc: Returns the value raised to the power @op.
	End Rem
	Method Pow:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_pow_ui(gmpPtr, op))
	End Method

	Rem
	bbdoc: Multiplies this value times 2 raised to @op, and returns the result.
	about: This operation can also be defined as a left shift by @op2 bits.
	End Rem
	Method Mul2Exp:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_mul2exp(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Divides this value by 2 raised to @op.
	End Rem
	Method Div2Exp:GMPFloat(op:Int)
		Return GMPFloat._create(bmx_gmp_mpf_div_2exp(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Returns the negative of this value.
	End Rem
	Method Neg:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_neg(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the absolute value.
	End Rem
	Method Absolute:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_abs(gmpPtr))
	End Method
	
	Rem
	bbdoc: Rounds to the next higher integer.
	End Rem
	Method Ceil:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_ceil(gmpPtr))
	End Method
	
	Rem
	bbdoc: Rounds to the next lowest integer.
	End Rem
	Method Floor:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_floor(gmpPtr))
	End Method
	
	Rem
	bbdoc: Rounds towards zero.
	End Rem
	Method Trunc:GMPFloat()
		Return GMPFloat._create(bmx_gmp_mpf_trunc(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value is an integer.
	End Rem
	Method IsInteger:Int()
		Return bmx_gmp_mpf_integer_p(gmpPtr)
	End Method

	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method Cmp:Int(op:GMPFloat)
		Return bmx_gmp_mpf_cmp(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method CmpD:Int(op:Double)
		Return bmx_gmp_mpf_cmp_d(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method CpmInt:Int(op:Int)
		Return bmx_gmp_mpf_cmp_si(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Eq:Int(op1:GMPFloat, op2:Int)
		Return bmx_gmp_mpf_eq(gmpPtr, op1.gmpPtr, op2)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RelDiff:GMPFloat(op2:GMPFloat)
		Return GMPFloat._create(bmx_gmp_mpf_reldiff(gmpPtr, op2.gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns +1 if the value &gt; 0, 0 if the value = 0, and -1 if the value &lt; 0
	End Rem
	Method Sign:Int()
		Return bmx_gmp_mpf_sgn(gmpPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If gmpPtr Then
			bmx_gmp_mpf_free(gmpPtr)
			gmpPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: A GMP rational.
about: Used for performing rational number arithmetic.
End Rem
Type GMPRational

	Field gmpPtr:Byte Ptr

	Function _create:GMPRational(gmpPtr:Byte Ptr)
		If gmpPtr Then
			Local this:GMPRational = New GMPRational
			this.gmpPtr = gmpPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates and initialises a new GMPRational.
	End Rem
	Function Init:GMPRational()
		Local this:GMPRational = New GMPRational
		this.gmpPtr = bmx_gmp_mpq_init()
		Return this
	End Function
	
	Rem
	bbdoc: Sets the value from @op.
	End Rem
	Method Set(op:GMPRational)
		bmx_gmp_mpq_set(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Assigns the value from @op.
	End Rem
	Method SetInt(op:GMPInt)
		bmx_gmp_mpq_set_z(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Set the value to @op1 / @op2.
	about: Note that if @op1 and @op2 have common factors, the value has to be passed to #Canonicalize before any operations are performed on it.
	End Rem
	Method SetDM(op1:Int, op2:Int)
		bmx_gmp_mpq_set_si(gmpPtr, op1, op2)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetStr:Int(s:String, base:Int = 10)
		Return bmx_gmp_mpq_setstr(gmpPtr, s, base)
	End Method

	Rem
	bbdoc: Swaps the value with @op efficiently.
	End Rem
	Method Swap(op:GMPRational)
		bmx_gmp_mpq_swap(gmpPtr, op.gmpPtr)
	End Method

	Rem
	bbdoc: Converst the value to a double, truncating if necessary (ie. rounding towards zero).
	about: If the exponent from the conversion is too big or too small to fit a double then the result is system dependent.
	For too big an infinity is returned when available. For too small 0.0 is normally returned. Hardware overflow,
	underflow and denorm traps may or may not occur.
	End Rem
	Method GetDouble:Double()
		Return bmx_gmp_mpq_get_d(gmpPtr)
	End Method
	
	Rem
	bbdoc: Sets the value to @op.
	about: There is no rounding, this conversion is exact.
	End Rem
	Method SetDouble(op:Double)
		bmx_gmp_mpq_set_d(gmpPtr, op)
	End Method
	
	Rem
	bbdoc: Sets the value to @op.
	about: There is no rounding, this conversion is exact.
	End Rem
	Method SetFloat(op:GMPFloat)
		bmx_gmp_mpq_set_f(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns a string of digits in base @base.
	about: The base may vary from 2 to 36. The string will be of the form 'num/den', or if the denominator is 1 then just 'num'.
	End Rem
	Method GetStr:String(base:Int = 10)
		Return bmx_gmp_mpq_get_str(gmpPtr, base)
	End Method

	Rem
	bbdoc: Removes any factors that are common to the numerator and denominator, and makes the denominator positive.
	End Rem
	Method Canonicalize()
		bmx_gmp_mpq_canonicalize(gmpPtr)
	End Method

	Rem
	bbdoc: Adds @op to this value, returning the result.
	End Rem
	Method Add:GMPRational(op:GMPRational)
		Return GMPRational._create(bmx_gmp_mpq_add(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Subtracts @op from this value, returning the result.
	End Rem
	Method Sub:GMPRational(op:GMPRational)
		Return GMPRational._create(bmx_gmp_mpq_sub(gmpPtr, op.gmpPtr))
	End Method
	
	Rem
	bbdoc: Multiplies value by @op, returning the result.
	End Rem
	Method Mul:GMPRational(op:GMPRational)
		Return GMPRational._create(bmx_gmp_mpq_mul(gmpPtr, op.gmpPtr))
	End Method

	Rem
	bbdoc: Divides the value by @op, returning the result.
	End Rem
	Method Div:GMPRational(op:GMPRational)
		Return GMPRational._create(bmx_gmp_mpq_div(gmpPtr, op.gmpPtr))
	End Method

	Rem
	bbdoc: Multiplies this value times 2 raised to @op, and returns the result.
	about: This operation can also be defined as a left shift by @op2 bits.
	End Rem
	Method Mul2Exp:GMPRational(op:Int)
		Return GMPRational._create(bmx_gmp_mpq_mul2exp(gmpPtr, op))
	End Method

	Rem
	bbdoc: Returns the value divided by 2 raised to @op.
	End Rem
	Method Div2Exp:GMPRational(op:Int)
		Return GMPRational._create(bmx_gmp_mpq_div_2exp(gmpPtr, op))
	End Method
	
	Rem
	bbdoc: Returns the negative of this value.
	End Rem
	Method Neg:GMPRational()
		Return GMPRational._create(bmx_gmp_mpq_neg(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns the absolute value.
	End Rem
	Method Absolute:GMPRational()
		Return GMPRational._create(bmx_gmp_mpq_abs(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns 1 / the value.
	about: If the new denominator is zero, this routine will divide by zero.
	End Rem
	Method Inv:GMPRational()
		Return GMPRational._create(bmx_gmp_mpq_inv(gmpPtr))
	End Method

	Rem
	bbdoc: Compares this value and @op.
	about: Returns a positive value if Self &gt; @op, zero if Self = @op, or a negative value if Self &lt; @op.
	End Rem
	Method Cmp:Int(op:GMPRational)
		Return bmx_gmp_mpq_cmp(gmpPtr, op.gmpPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CmpInt:Int(op1:Int, op2:Int)
		Return bmx_gmp_mpq_cmp_si(gmpPtr, op1, op2)
	End Method
	
	Rem
	bbdoc: Returns +1 if the value &gt; 0, 0 if the value = 0, and -1 if the value &lt; 0
	End Rem
	Method Sign:Int()
		Return bmx_gmp_mpq_sgn(gmpPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if the value and @op are equal, zero if they are non-equal.
	about: Although Cmp can be used for the same purpose, this method is much faster.
	End Rem
	Method IsEqual:Int(op:GMPRational)
		Return bmx_gmp_mpq_equal(gmpPtr, op.gmpPtr)
	End Method

	Rem
	bbdoc: Returns a reference to the numerator. 
	End Rem
	Method NumRef:GMPInt()
		Return GMPInt._create(bmx_gmp_mpq_numref(gmpPtr))
	End Method
	
	Rem
	bbdoc: Returns a reference to the denominator. 
	End Rem
	Method DenRef:GMPInt()
		Return GMPInt._create(bmx_gmp_mpq_denref(gmpPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If gmpPtr Then
			bmx_gmp_mpq_free(gmpPtr)
			gmpPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method

End Type
