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

End Extern

