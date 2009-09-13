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

Import BRL.Blitz

?macos
Import "-lgmp"
?win32
Import "-lgmp"
?linux
?

Import "include/*.h"
Import "glue.cpp"


Extern

	Function bmx_gmp_mpz_init:Byte Ptr()
	Function bmx_gmp_mpz_free(handle:Byte Ptr)
	Function bmx_gmp_mpz_set(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_setint(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_setdouble(handle:Byte Ptr, op:Double)
	Function bmx_gmp_mpz_setrational(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_setfloat(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_setstr:Int(handle:Byte Ptr, s:String, base:Int)
	Function bmx_gmp_mpz_swap(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_getstr:String(handle:Byte Ptr, base:Int)
	Function bmx_gmp_mpz_getint:Int(handle:Byte Ptr)
	Function bmx_gmp_mpz_getdouble:Double(handle:Byte Ptr)
	Function bmx_gmp_mpz_getdouble2exp:Double(handle:Byte Ptr, _exp:Int Ptr)
	Function bmx_gmp_mpz_add:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_addint:Byte Ptr(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_sub:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_subint:Byte Ptr(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_intsub:Byte Ptr(handle:Byte Ptr, op1:Int)
	Function bmx_gmp_mpz_mul:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_mulint:Byte Ptr(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_addmul(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_addmulint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_submul(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_submulint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_mul2exp:Byte Ptr(handle:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_neg:Byte Ptr(handle:Byte Ptr)
	Function bmx_gmp_mpz_abs:Byte Ptr(handle:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_q:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_r:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_qr:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_q_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_cdiv_r_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_cdiv_qr_ui:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_cdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_cdiv_q_2exp:Byte Ptr(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_cdiv_r_2exp:Byte Ptr(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_fdiv_q:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_r:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_qr:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_q_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_fdiv_r_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_fdiv_qr_ui:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_fdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_fdiv_q_2exp:Byte Ptr(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_fdiv_r_2exp:Byte Ptr(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_tdiv_q:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_r:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_qr:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_q_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_tdiv_r_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_tdiv_qr_ui:Byte Ptr(handle:Byte Ptr, r:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_tdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_tdiv_q_2exp:Byte Ptr(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_tdiv_r_2exp:Byte Ptr(handle:Byte Ptr, b:Int)

	Function bmx_gmp_mpz_mod:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_mod_ui:Byte Ptr(handle:Byte Ptr, d:Int, remainder:Int Ptr)
	Function bmx_gmp_mpz_divexact:Byte Ptr(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_divexact_ui:Byte Ptr(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_divisible_p:Int(handle:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_divisible_ui_p:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_divisible_2exp_p:Int(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_congruent_p:Int(handle:Byte Ptr, c:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_congruent_ui_p:Int(handle:Byte Ptr, c:Int, d:Int)
	Function bmx_gmp_mpz_congruent_2exp_p:Int(handle:Byte Ptr, c:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_powm:Byte Ptr(handle:Byte Ptr, _exp:Byte Ptr, _mod:Byte Ptr)
	Function bmx_gmp_mpz_powm_ui:Byte Ptr(handle:Byte Ptr, _exp:Int, _mod:Byte Ptr)
	Function bmx_gmp_mpz_pow_ui:Byte Ptr(handle:Byte Ptr, _exp:Int)
	Function bmx_gmp_mpz_ui_pow_ui:Byte Ptr(base:Int, _exp:Int)
	Function bmx_gmp_mpz_root:Byte Ptr(handle:Byte Ptr, n:Int, result:Int Ptr)
	Function bmx_gmp_mpz_rootrem:Byte Ptr(handle:Byte Ptr, _rem:Byte Ptr, n:Int)
	Function bmx_gmp_mpz_sqrt:Byte Ptr(handle:Byte Ptr)
	Function bmx_gmp_mpz_sqrtrem:Byte Ptr(handle:Byte Ptr, rop:Byte Ptr)
	Function bmx_gmp_mpz_perfect_power_p:Int(handle:Byte Ptr)
	Function bmx_gmp_mpz_perfect_square_p:Int(handle:Byte Ptr)

	Function bmx_gmp_mpz_probab_prime_p:Int(handle:Byte Ptr, reps:Int)
	Function bmx_gmp_mpz_nextprime:Byte Ptr(handle:Byte Ptr)
	Function bmx_gmp_mpz_gcd:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_gcd_ui:Byte Ptr(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_gcdext:Byte Ptr(handle:Byte Ptr, s:Byte Ptr, t:Byte Ptr, b:Byte Ptr)
	Function bmx_gmp_mpz_lcm:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_lcm_ui:Byte Ptr(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_invert:Byte Ptr(handle:Byte Ptr, op:Byte Ptr, result:Int Ptr)
	Function bmx_gmp_mpz_jacobi:Int(handle:Byte Ptr, b:Byte Ptr)
	Function bmx_gmp_mpz_legendre:Int(handle:Byte Ptr, p:Byte Ptr)
	Function bmx_gmp_mpz_kronecker:Int(handle:Byte Ptr, b:Byte Ptr)
	Function bmx_gmp_mpz_kronecker_si:Int(handle:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_si_kronecker:Int(handle:Byte Ptr, a:Int)
	Function bmx_gmp_mpz_remove:Byte Ptr(handle:Byte Ptr, f:Byte Ptr, count:Int Ptr)
	Function bmx_gmp_mpz_fac_ui:Byte Ptr(op:Int)
	Function bmx_gmp_mpz_bin_ui:Byte Ptr(handle:Byte Ptr, k:Int)
	Function bmx_gmp_mpz_bin_uiui:Byte Ptr(n:Int, k:Int)
	Function bmx_gmp_mpz_fib_ui:Byte Ptr(n:Int)
	Function bmx_gmp_mpz_fib2_ui:Byte Ptr(handle:Byte Ptr, n:Int)
	Function bmx_gmp_mpz_lucnum_ui:Byte Ptr(n:Int)
	Function bmx_gmp_mpz_lucnum2_ui:Byte Ptr(handle:Byte Ptr, n:Int)

	Function bmx_gmp_mpz_cmp:Int(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_cmp_d:Int(handle:Byte Ptr, op:Double)
	Function bmx_gmp_mpz_cmp_si:Int(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_cmpabs:Int(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_cmpabs_d:Int(handle:Byte Ptr, op:Double)
	Function bmx_gmp_mpz_cmpabs_ui:Int(handle:Byte Ptr, op:Int)
	Function bmx_gmp_mpz_sgn:Int(handle:Byte Ptr)
	Function bmx_gmp_mpz_and:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_ior:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_xor:Byte Ptr(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_com:Byte Ptr(handle:Byte Ptr)
	Function bmx_gmp_mpz_popcount:Int(handle:Byte Ptr)
	Function bmx_gmp_mpz_hamdist:Int(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_scan0:Int(handle:Byte Ptr, startingBit:Int)
	Function bmx_gmp_mpz_scan1:Int(handle:Byte Ptr, startingBit:Int)
	Function bmx_gmp_mpz_setbit(handle:Byte Ptr, bitIndex:Int)
	Function bmx_gmp_mpz_clrbit(handle:Byte Ptr, bitIndex:Int)
	Function bmx_gmp_mpz_combit(handle:Byte Ptr, bitIndex:Int)
	Function bmx_gmp_mpz_tstbit:Int(handle:Byte Ptr, bitIndex:Int)
	Function bmx_gmp_mpz_odd_p:Int(handle:Byte Ptr)
	Function bmx_gmp_mpz_even_p:Int(handle:Byte Ptr)

End Extern

