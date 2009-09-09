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
	Function bmx_gmp_mpz_add(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_addint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_sub(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_subint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_intsub(handle:Byte Ptr, op1:Int, op2:Byte Ptr)
	Function bmx_gmp_mpz_mul(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_mulint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_addmul(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_addmulint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_submul(handle:Byte Ptr, op1:Byte Ptr, op2:Byte Ptr)
	Function bmx_gmp_mpz_submulint(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_mul2exp(handle:Byte Ptr, op1:Byte Ptr, op2:Int)
	Function bmx_gmp_mpz_neg(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_abs(handle:Byte Ptr, op:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_q(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_r(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_qr(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_cdiv_q_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_cdiv_r_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_cdiv_qr_ui:Int(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_cdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_cdiv_q_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_cdiv_r_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_fdiv_q(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_r(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_qr(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_fdiv_q_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_fdiv_r_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_fdiv_qr_ui:Int(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_fdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_fdiv_q_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_fdiv_r_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_tdiv_q(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_r(handle:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_qr(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Byte Ptr)
	Function bmx_gmp_mpz_tdiv_q_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_tdiv_r_ui:Int(handle:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_tdiv_qr_ui:Int(handle:Byte Ptr, r:Byte Ptr, n:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_tdiv_ui:Int(handle:Byte Ptr, d:Int)
	Function bmx_gmp_mpz_tdiv_q_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	Function bmx_gmp_mpz_tdiv_r_2exp(handle:Byte Ptr, n:Byte Ptr, b:Int)
	
End Extern

