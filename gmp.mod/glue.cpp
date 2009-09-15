/*
 Copyright (c) 2009 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
#include "gmp.h"


class MaxMpz;
class MaxMpq;
class MaxMpf;

extern "C" {

#include "blitz.h"

	MaxMpz * bmx_gmp_mpz_init();
	void bmx_gmp_mpz_free(MaxMpz * mpz);
	void bmx_gmp_mpz_set(MaxMpz * mpz, MaxMpz * op);
	void bmx_gmp_mpz_setint(MaxMpz * mpz, int op);
	void bmx_gmp_mpz_setdouble(MaxMpz * mpz, double op);
	void bmx_gmp_mpz_setrational(MaxMpz * mpz, MaxMpq * op);
	void bmx_gmp_mpz_setfloat(MaxMpz * mpz, MaxMpf  * op);
	int bmx_gmp_mpz_setstr(MaxMpz * mpz, BBString * str, int base);
	void bmx_gmp_mpz_swap(MaxMpz * mpz, MaxMpz * op);
	BBString * bmx_gmp_mpz_getstr(MaxMpz * mpz, int base);
	int bmx_gmp_mpz_getint(MaxMpz * mpz);
	double bmx_gmp_mpz_getdouble(MaxMpz * mpz);
	double bmx_gmp_mpz_getdouble2exp(MaxMpz * mpz, signed long int * _exp);
	MaxMpz * bmx_gmp_mpz_add(MaxMpz * op1, MaxMpz * op2);
	MaxMpz * bmx_gmp_mpz_addint(MaxMpz * op1, int op2);
	MaxMpz * bmx_gmp_mpz_sub(MaxMpz * op1, MaxMpz * op2);
	MaxMpz * bmx_gmp_mpz_subint(MaxMpz * op1, int op2);
	MaxMpz * bmx_gmp_mpz_intsub(MaxMpz * op2, int op1);
	MaxMpz * bmx_gmp_mpz_mul(MaxMpz * op1, MaxMpz * op2);
	MaxMpz * bmx_gmp_mpz_mulint(MaxMpz * op1, int op2);
	void bmx_gmp_mpz_addmul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_addmulint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_submul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_submulint(MaxMpz * mpz, MaxMpz * op1, int op2);
	MaxMpz * bmx_gmp_mpz_mul2exp(MaxMpz * op1, int op2);
	MaxMpz * bmx_gmp_mpz_neg(MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_abs(MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_cdiv_q(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_cdiv_r(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_cdiv_qr(MaxMpz * n, MaxMpz * r, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_cdiv_q_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_cdiv_r_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_cdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder);
	int bmx_gmp_mpz_cdiv_ui(MaxMpz * mpz, int d);
	MaxMpz * bmx_gmp_mpz_cdiv_q_2exp(MaxMpz * n, int b);
	MaxMpz * bmx_gmp_mpz_cdiv_r_2exp(MaxMpz * n, int b);
	MaxMpz * bmx_gmp_mpz_fdiv_q(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_fdiv_r(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_fdiv_qr(MaxMpz * r, MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_fdiv_q_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_fdiv_r_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_fdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder);
	int bmx_gmp_mpz_fdiv_ui(MaxMpz * mpz, int d);
	MaxMpz * bmx_gmp_mpz_fdiv_q_2exp(MaxMpz * n, int b);
	MaxMpz * bmx_gmp_mpz_fdiv_r_2exp(MaxMpz * n, int b);
	MaxMpz * bmx_gmp_mpz_tdiv_q(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_tdiv_r(MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_tdiv_qr(MaxMpz * r, MaxMpz * n, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_tdiv_q_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_tdiv_r_ui(MaxMpz * n, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_tdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder);
	int bmx_gmp_mpz_tdiv_ui(MaxMpz * mpz, int d);
	MaxMpz * bmx_gmp_mpz_tdiv_q_2exp(MaxMpz * n, int b);
	MaxMpz * bmx_gmp_mpz_tdiv_r_2exp(MaxMpz * n, int b);

	MaxMpz * bmx_gmp_mpz_mod(MaxMpz * op, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_mod_ui(MaxMpz * op, int d, int * remainder);
	MaxMpz * bmx_gmp_mpz_divexact(MaxMpz * op, MaxMpz * d);
	MaxMpz * bmx_gmp_mpz_divexact_ui(MaxMpz * op, int d);
	int bmx_gmp_mpz_divisible_p(MaxMpz * op, MaxMpz * d);
	int bmx_gmp_mpz_divisible_ui_p(MaxMpz * op, int d);
	int bmx_gmp_mpz_divisible_2exp_p(MaxMpz * op, int b);
	int bmx_gmp_mpz_congruent_p(MaxMpz * op, MaxMpz * c, MaxMpz * d);
	int bmx_gmp_mpz_congruent_ui_p(MaxMpz * op, int c, int d);
	int bmx_gmp_mpz_congruent_2exp_p(MaxMpz * op, MaxMpz * c, int b);
	MaxMpz * bmx_gmp_mpz_powm(MaxMpz * op, MaxMpz * _exp, MaxMpz * _mod);
	MaxMpz * bmx_gmp_mpz_powm_ui(MaxMpz * op, int _exp, MaxMpz * _mod);
	MaxMpz * bmx_gmp_mpz_pow_ui(MaxMpz * op, int _exp);
	MaxMpz * bmx_gmp_mpz_ui_pow_ui(int base, int _exp);
	MaxMpz * bmx_gmp_mpz_root(MaxMpz * op, int n, int * result);
	MaxMpz * bmx_gmp_mpz_rootrem(MaxMpz * op, MaxMpz * _rem, int n);
	MaxMpz * bmx_gmp_mpz_sqrt(MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_sqrtrem(MaxMpz * op, MaxMpz * rop);
	int bmx_gmp_mpz_perfect_power_p(MaxMpz * op);
	int bmx_gmp_mpz_perfect_square_p(MaxMpz * op);

	int bmx_gmp_mpz_probab_prime_p(MaxMpz * r, int reps);
	MaxMpz * bmx_gmp_mpz_nextprime(MaxMpz * r);
	MaxMpz * bmx_gmp_mpz_gcd(MaxMpz * r, MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_gcd_ui(MaxMpz * r, int op);
	MaxMpz * bmx_gmp_mpz_gcdext(MaxMpz * r, MaxMpz * s, MaxMpz * t, MaxMpz * b);
	MaxMpz * bmx_gmp_mpz_lcm(MaxMpz * r, MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_lcm_ui(MaxMpz * r, int op);
	MaxMpz * bmx_gmp_mpz_invert(MaxMpz * r, MaxMpz * op, int * result);
	int bmx_gmp_mpz_jacobi(MaxMpz * r, MaxMpz * b);
	int bmx_gmp_mpz_legendre(MaxMpz * r, MaxMpz * p);
	int bmx_gmp_mpz_kronecker(MaxMpz * r, MaxMpz * b);
	int bmx_gmp_mpz_kronecker_si(MaxMpz * r, int b);
	int bmx_gmp_mpz_si_kronecker(MaxMpz * r, int a);
	MaxMpz * bmx_gmp_mpz_remove(MaxMpz * r, MaxMpz * f, int * count);
	MaxMpz * bmx_gmp_mpz_fac_ui(int op);
	MaxMpz * bmx_gmp_mpz_bin_ui(MaxMpz * r, int k);
	MaxMpz * bmx_gmp_mpz_bin_uiui(int n, int k);
	MaxMpz * bmx_gmp_mpz_fib_ui(int n);
	MaxMpz * bmx_gmp_mpz_fib2_ui(MaxMpz * r, int n);
	MaxMpz * bmx_gmp_mpz_lucnum_ui(int n);
	MaxMpz * bmx_gmp_mpz_lucnum2_ui(MaxMpz * r, int n);

	int bmx_gmp_mpz_cmp(MaxMpz * r, MaxMpz * op);
	int bmx_gmp_mpz_cmp_d(MaxMpz * r, double op);
	int bmx_gmp_mpz_cmp_si(MaxMpz * r, int op);
	int bmx_gmp_mpz_cmpabs(MaxMpz * r, MaxMpz * op);
	int bmx_gmp_mpz_cmpabs_d(MaxMpz * r, double op);
	int bmx_gmp_mpz_cmpabs_ui(MaxMpz * r, int op);
	int bmx_gmp_mpz_sgn(MaxMpz * r);
	MaxMpz * bmx_gmp_mpz_and(MaxMpz * r, MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_ior(MaxMpz * r, MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_xor(MaxMpz * r, MaxMpz * op);
	MaxMpz * bmx_gmp_mpz_com(MaxMpz * r);
	int bmx_gmp_mpz_popcount(MaxMpz * r);
	int bmx_gmp_mpz_hamdist(MaxMpz * r, MaxMpz * op);
	int bmx_gmp_mpz_scan0(MaxMpz * r, int startingBit);
	int bmx_gmp_mpz_scan1(MaxMpz * r, int startingBit);
	void bmx_gmp_mpz_setbit(MaxMpz * r, int bitIndex);
	void bmx_gmp_mpz_clrbit(MaxMpz * r, int bitIndex);
	void bmx_gmp_mpz_combit(MaxMpz * r, int bitIndex);
	int bmx_gmp_mpz_tstbit(MaxMpz * r, int bitIndex);
	int bmx_gmp_mpz_odd_p(MaxMpz * r);
	int bmx_gmp_mpz_even_p(MaxMpz * r);

	MaxMpf * bmx_gmp_mpf_init(int prec);
	void bmx_gmp_mpf_set(MaxMpf * r, MaxMpf * op);
	void bmx_gmp_mpf_set_default_prec(int prec);
	int bmx_gmp_mpf_get_default_prec();
	int bmx_gmp_mpf_get_prec(MaxMpf * r);
	void bmx_gmp_mpf_set_prec(MaxMpf * r, int prec);
	void bmx_gmp_mpf_free(MaxMpf * r);
	void bmx_gmp_mpf_setint(MaxMpf * r, int op);
	void bmx_gmp_mpf_setdouble(MaxMpf * r, double op);
	void bmx_gmp_mpf_setinteger(MaxMpf * r, MaxMpz * op);
	void bmx_gmp_mpf_setrational(MaxMpf * r, MaxMpq * op);
	int bmx_gmp_mpf_setstr(MaxMpf * r, BBString * s, int base);
	void bmx_gmp_mpf_swap(MaxMpf * r, MaxMpf * op);
	double bmx_gmp_mpf_get_d(MaxMpf * r);
	double bmx_gmp_mpf_get_d_2exp(MaxMpf * r, int * _exp);
	int bmx_gmp_mpf_get_si(MaxMpf * r);
	BBString * bmx_gmp_mpf_get_str(MaxMpf * r, int * _exp, int base);

	MaxMpf * bmx_gmp_mpf_add(MaxMpf * r, MaxMpf * op);
	MaxMpf * bmx_gmp_mpf_addint(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_sub(MaxMpf * r, MaxMpf * op);
	MaxMpf * bmx_gmp_mpf_subint(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_intsub(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_mul(MaxMpf * r, MaxMpf * op);
	MaxMpf * bmx_gmp_mpf_mulint(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_div(MaxMpf * r, MaxMpf * op);
	MaxMpf * bmx_gmp_mpf_ui_div(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_div_ui(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_sqrt(MaxMpf * r);
	MaxMpf * bmx_gmp_mpf_sqrt_ui(int op);
	MaxMpf * bmx_gmp_mpf_pow_ui(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_mul2exp(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_div_2exp(MaxMpf * r, int op);
	MaxMpf * bmx_gmp_mpf_neg(MaxMpf * r);
	MaxMpf * bmx_gmp_mpf_abs(MaxMpf * r);
	MaxMpf * bmx_gmp_mpf_ceil(MaxMpf * r);
	MaxMpf * bmx_gmp_mpf_floor(MaxMpf * r);
	MaxMpf * bmx_gmp_mpf_trunc(MaxMpf * r);
	int bmx_gmp_mpf_integer_p(MaxMpf * r);


	int bmx_gmp_mpf_cmp(MaxMpf * r, MaxMpf * op);
	int bmx_gmp_mpf_cmp_d(MaxMpf * r, double op);
	int bmx_gmp_mpf_cmp_si(MaxMpf * r, int op);
	int bmx_gmp_mpf_eq(MaxMpf * r, MaxMpf * op1, int op2);
	MaxMpf * bmx_gmp_mpf_reldiff(MaxMpf * r, MaxMpf * op2);
	int bmx_gmp_mpf_sgn(MaxMpf * r);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MaxMpz
{
public:
	MaxMpz()
	{
		mpz_init(value);
	}
	
	~MaxMpz()
	{
		mpz_clear(value);
	}
	
	mpz_t & Value() {
		return value;
	}
	
private:
	mpz_t value;
};

class MaxMpq
{
public:
	MaxMpq()
	{
		mpq_init(value);
	}
	
	~MaxMpq()
	{
		mpq_clear(value);
	}
	
	mpq_t & Value() {
		return value;
	}
	
private:
	mpq_t value;
};

class MaxMpf
{
public:
	MaxMpf()
	{
		mpf_init(value);
	}

	MaxMpf(int prec)
	{
		mpf_init2(value, prec);
	}
	
	~MaxMpf()
	{
		mpf_clear(value);
	}
	
	mpf_t & Value() {
		return value;
	}
	
private:
	mpf_t value;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++


MaxMpz * bmx_gmp_mpz_init() {
	return new MaxMpz;
}

void bmx_gmp_mpz_free(MaxMpz * mpz) {
	delete mpz;
}

void bmx_gmp_mpz_set(MaxMpz * mpz, MaxMpz * op) {
	mpz_set(mpz->Value(), op->Value());
}

void bmx_gmp_mpz_setint(MaxMpz * mpz, int op) {
	mpz_set_si(mpz->Value(), op);
}

void bmx_gmp_mpz_setdouble(MaxMpz * mpz, double op) {
	mpz_set_d(mpz->Value(), op);
}

void bmx_gmp_mpz_setrational(MaxMpz * mpz, MaxMpq * op) {
	mpz_set_q(mpz->Value(), op->Value());
}

void bmx_gmp_mpz_setfloat(MaxMpz * mpz, MaxMpf  * op) {
	mpz_set_f(mpz->Value(), op->Value());
}

int bmx_gmp_mpz_setstr(MaxMpz * mpz, BBString * str, int base) {
	char * s = bbStringToCString(str);
	int res = mpz_set_str(mpz->Value(), s, base);
	bbMemFree(s);
	return res;
}

void bmx_gmp_mpz_swap(MaxMpz * mpz, MaxMpz * op) {
	mpz_swap(mpz->Value(), op->Value());
}

BBString * bmx_gmp_mpz_getstr(MaxMpz * mpz, int base) {
	char * s = mpz_get_str(NULL, base, mpz->Value());
	BBString * str = bbStringFromCString(s);
	free(s);
	return str;
}

int bmx_gmp_mpz_getint(MaxMpz * mpz) {
	return mpz_get_si(mpz->Value());
}

double bmx_gmp_mpz_getdouble(MaxMpz * mpz) {
	return mpz_get_d(mpz->Value());
}

double bmx_gmp_mpz_getdouble2exp(MaxMpz * mpz, signed long int * _exp) {
	return mpz_get_d_2exp(_exp, mpz->Value());
}

MaxMpz * bmx_gmp_mpz_add(MaxMpz * op1, MaxMpz * op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_add(mpz->Value(), op1->Value(), op2->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_addint(MaxMpz * op1, int op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_add_ui(mpz->Value(), op1->Value(), op2);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_sub(MaxMpz * op1, MaxMpz * op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_sub(mpz->Value(), op1->Value(), op2->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_subint(MaxMpz * op1, int op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_sub_ui(mpz->Value(), op1->Value(), op2);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_intsub(MaxMpz * op2, int op1) {
	MaxMpz * mpz = new MaxMpz;
	mpz_ui_sub(mpz->Value(), op1, op2->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_mul(MaxMpz * op1, MaxMpz * op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_mul(mpz->Value(), op1->Value(), op2->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_mulint(MaxMpz * op1, int op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_mul_si(mpz->Value(), op1->Value(), op2);
	return mpz;
}

void bmx_gmp_mpz_addmul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2) {
	mpz_addmul(mpz->Value(), op1->Value(), op2->Value());
}

void bmx_gmp_mpz_addmulint(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_addmul_ui(mpz->Value(), op1->Value(), op2);
}

void bmx_gmp_mpz_submul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2) {
	mpz_submul(mpz->Value(), op1->Value(), op2->Value());
}

void bmx_gmp_mpz_submulint(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_submul_ui(mpz->Value(), op1->Value(), op2);
}

MaxMpz * bmx_gmp_mpz_mul2exp(MaxMpz * op1, int op2) {
	MaxMpz * mpz = new MaxMpz;
	mpz_mul_2exp(mpz->Value(), op1->Value(), op2);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_neg(MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_neg(mpz->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_abs(MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_abs(mpz->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_q(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_cdiv_q(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_r(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_cdiv_r(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_qr(MaxMpz * n, MaxMpz * r, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_cdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_q_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_cdiv_q_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_r_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_cdiv_r_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_cdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d));
	return mpz;
}

int bmx_gmp_mpz_cdiv_ui(MaxMpz * mpz, int d) {
	return mpz_cdiv_ui(mpz->Value(), d);
}

MaxMpz * bmx_gmp_mpz_cdiv_q_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_cdiv_q_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_cdiv_r_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_cdiv_r_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_q(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fdiv_q(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_r(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fdiv_r(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_qr(MaxMpz * n, MaxMpz * r, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_q_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_fdiv_q_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_r_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_fdiv_r_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_fdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d));
	return mpz;
}

int bmx_gmp_mpz_fdiv_ui(MaxMpz * mpz, int d) {
	return mpz_fdiv_ui(mpz->Value(), d);
}

MaxMpz * bmx_gmp_mpz_fdiv_q_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fdiv_q_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fdiv_r_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fdiv_r_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_q(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_tdiv_q(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_r(MaxMpz * n, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_tdiv_r(mpz->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_qr(MaxMpz * n, MaxMpz * r, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_tdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_q_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_tdiv_q_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_r_ui(MaxMpz * n, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_tdiv_r_ui(mpz->Value(), n->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_qr_ui(MaxMpz * n, MaxMpz * r, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_tdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d));
	return mpz;
}

int bmx_gmp_mpz_tdiv_ui(MaxMpz * mpz, int d) {
	return mpz_tdiv_ui(mpz->Value(), d);
}

MaxMpz * bmx_gmp_mpz_tdiv_q_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_tdiv_q_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_tdiv_r_2exp(MaxMpz * n, int b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_tdiv_r_2exp(mpz->Value(), n->Value(), b);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_mod(MaxMpz * op, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_mod(mpz->Value(), op->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_mod_ui(MaxMpz * op, int d, int * remainder) {
	MaxMpz * mpz = new MaxMpz;
	*remainder = static_cast<int>(mpz_mod_ui(mpz->Value(), op->Value(), d));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_divexact(MaxMpz * op, MaxMpz * d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_divexact(mpz->Value(), op->Value(), d->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_divexact_ui(MaxMpz * op, int d) {
	MaxMpz * mpz = new MaxMpz;
	mpz_divexact_ui(mpz->Value(), op->Value(), d);
	return mpz;
}

int bmx_gmp_mpz_divisible_p(MaxMpz * op, MaxMpz * d) {
	return mpz_divisible_p(op->Value(), d->Value());
}

int bmx_gmp_mpz_divisible_ui_p(MaxMpz * op, int d) {
	return mpz_divisible_ui_p(op->Value(), d);
}

int bmx_gmp_mpz_divisible_2exp_p(MaxMpz * op, int b) {
	return mpz_divisible_2exp_p(op->Value(), b);
}

int bmx_gmp_mpz_congruent_p(MaxMpz * op, MaxMpz * c, MaxMpz * d) {
	return mpz_congruent_p(op->Value(), c->Value(), d->Value());
}

int bmx_gmp_mpz_congruent_ui_p(MaxMpz * op, int c, int d) {
	return mpz_congruent_ui_p(op->Value(), c, d);
}

int bmx_gmp_mpz_congruent_2exp_p(MaxMpz * op, MaxMpz * c, int b) {
	return mpz_congruent_2exp_p(op->Value(), c->Value(), b);
}

MaxMpz * bmx_gmp_mpz_powm(MaxMpz * op, MaxMpz * _exp, MaxMpz * _mod) {
	MaxMpz * mpz = new MaxMpz;
	mpz_powm(mpz->Value(), op->Value(), _exp->Value(), _mod->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_powm_ui(MaxMpz * op, int _exp, MaxMpz * _mod) {
	MaxMpz * mpz = new MaxMpz;
	mpz_powm_ui(mpz->Value(), op->Value(), _exp, _mod->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_pow_ui(MaxMpz * op, int _exp) {
	MaxMpz * mpz = new MaxMpz;
	mpz_pow_ui(mpz->Value(), op->Value(), _exp);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_ui_pow_ui(int base, int _exp) {
	MaxMpz * mpz = new MaxMpz;
	mpz_ui_pow_ui(mpz->Value(), base, _exp);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_root(MaxMpz * op, int n, int * result) {
	MaxMpz * mpz = new MaxMpz;
	*result = mpz_root(mpz->Value(), op->Value(), n);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_rootrem(MaxMpz * op, MaxMpz * _rem, int n) {
	MaxMpz * mpz = new MaxMpz;
	mpz_rootrem(mpz->Value(), _rem->Value(), op->Value(), n);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_sqrt(MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_sqrt(mpz->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_sqrtrem(MaxMpz * op, MaxMpz * rop) {
	MaxMpz * mpz = new MaxMpz;
	mpz_sqrtrem(mpz->Value(), rop->Value(), op->Value());
	return mpz;
}

int bmx_gmp_mpz_perfect_power_p(MaxMpz * op) {
	return mpz_perfect_power_p(op->Value());
}

int bmx_gmp_mpz_perfect_square_p(MaxMpz * op) {
	return mpz_perfect_square_p(op->Value());
}

int bmx_gmp_mpz_probab_prime_p(MaxMpz * r, int reps) {
	return mpz_probab_prime_p(r->Value(), reps);
}

MaxMpz * bmx_gmp_mpz_nextprime(MaxMpz * r) {
	MaxMpz * mpz = new MaxMpz;
	mpz_nextprime(mpz->Value(), r->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_gcd(MaxMpz * r, MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_gcd(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_gcd_ui(MaxMpz * r, int op) {
	MaxMpz * mpz = new MaxMpz;
	int res = mpz_gcd_ui(mpz->Value(), r->Value(), op);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_gcdext(MaxMpz * r, MaxMpz * s, MaxMpz * t, MaxMpz * b) {
	MaxMpz * mpz = new MaxMpz;
	mpz_gcdext(mpz->Value(), s->Value(), t->Value(), r->Value(), b->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_lcm(MaxMpz * r, MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_lcm(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_lcm_ui(MaxMpz * r, int op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_lcm_ui(mpz->Value(), r->Value(), op);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_invert(MaxMpz * r, MaxMpz * op, int * result) {
	MaxMpz * mpz = new MaxMpz;
	*result = mpz_invert(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

int bmx_gmp_mpz_jacobi(MaxMpz * r, MaxMpz * b) {
	return mpz_jacobi(r->Value(), b->Value());
}

int bmx_gmp_mpz_legendre(MaxMpz * r, MaxMpz * p) {
	return mpz_legendre(r->Value(), p->Value());
}

int bmx_gmp_mpz_kronecker(MaxMpz * r, MaxMpz * b) {
	return mpz_kronecker(r->Value(), b->Value());
}

int bmx_gmp_mpz_kronecker_si(MaxMpz * r, int b) {
	return mpz_kronecker_si(r->Value(), b);
}

int bmx_gmp_mpz_si_kronecker(MaxMpz * r, int a) {
	return mpz_si_kronecker(a, r->Value());
}

MaxMpz * bmx_gmp_mpz_remove(MaxMpz * r, MaxMpz * f, int * count) {
	MaxMpz * mpz = new MaxMpz;
	*count = static_cast<int>(mpz_remove(mpz->Value(), r->Value(), f->Value()));
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fac_ui(int op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fac_ui(mpz->Value(), op);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_bin_ui(MaxMpz * r, int k) {
	MaxMpz * mpz = new MaxMpz;
	mpz_bin_ui(mpz->Value(), r->Value(), k);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_bin_uiui(int n, int k) {
	MaxMpz * mpz = new MaxMpz;
	mpz_bin_uiui(mpz->Value(), n, k);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fib_ui(int n) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fib_ui(mpz->Value(), n);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_fib2_ui(MaxMpz * r, int n) {
	MaxMpz * mpz = new MaxMpz;
	mpz_fib2_ui(mpz->Value(), r->Value(), n);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_lucnum_ui(int n) {
	MaxMpz * mpz = new MaxMpz;
	mpz_lucnum_ui(mpz->Value(), n);
	return mpz;
}

MaxMpz * bmx_gmp_mpz_lucnum2_ui(MaxMpz * r, int n) {
	MaxMpz * mpz = new MaxMpz;
	mpz_lucnum2_ui(mpz->Value(), r->Value(), n);
	return mpz;
}

int bmx_gmp_mpz_cmp(MaxMpz * r, MaxMpz * op) {
	return mpz_cmp(r->Value(), op->Value());
}

int bmx_gmp_mpz_cmp_d(MaxMpz * r, double op) {
	return mpz_cmp_d(r->Value(), op);
}

int bmx_gmp_mpz_cmp_si(MaxMpz * r, int op) {
	return mpz_cmp_si(r->Value(), op);
}

int bmx_gmp_mpz_cmpabs(MaxMpz * r, MaxMpz * op) {
	return mpz_cmpabs(r->Value(), op->Value());
}

int bmx_gmp_mpz_cmpabs_d(MaxMpz * r, double op) {
	return mpz_cmpabs_d(r->Value(), op);
}

int bmx_gmp_mpz_cmpabs_ui(MaxMpz * r, int op) {
	return mpz_cmpabs_ui(r->Value(), op);
}

int bmx_gmp_mpz_sgn(MaxMpz * r) {
	return mpz_sgn(r->Value());
}

MaxMpz * bmx_gmp_mpz_and(MaxMpz * r, MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_and(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_ior(MaxMpz * r, MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_ior(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_xor(MaxMpz * r, MaxMpz * op) {
	MaxMpz * mpz = new MaxMpz;
	mpz_xor(mpz->Value(), r->Value(), op->Value());
	return mpz;
}

MaxMpz * bmx_gmp_mpz_com(MaxMpz * r) {
	MaxMpz * mpz = new MaxMpz;
	mpz_com(mpz->Value(), r->Value());
	return mpz;
}

int bmx_gmp_mpz_popcount(MaxMpz * r) {
	return static_cast<int>(mpz_popcount(r->Value()));
}

int bmx_gmp_mpz_hamdist(MaxMpz * r, MaxMpz * op) {
	return static_cast<int>(mpz_hamdist(r->Value(), op->Value()));
}

int bmx_gmp_mpz_scan0(MaxMpz * r, int startingBit) {
	return static_cast<int>(mpz_scan0(r->Value(), startingBit));
}

int bmx_gmp_mpz_scan1(MaxMpz * r, int startingBit) {
	return static_cast<int>(mpz_scan1(r->Value(), startingBit));
}

void bmx_gmp_mpz_setbit(MaxMpz * r, int bitIndex) {
	mpz_setbit(r->Value(), bitIndex);
}

void bmx_gmp_mpz_clrbit(MaxMpz * r, int bitIndex) {
	mpz_clrbit(r->Value(), bitIndex);
}

void bmx_gmp_mpz_combit(MaxMpz * r, int bitIndex) {
	mpz_combit(r->Value(), bitIndex);
}

int bmx_gmp_mpz_tstbit(MaxMpz * r, int bitIndex) {
	return mpz_tstbit(r->Value(), bitIndex);
}

int bmx_gmp_mpz_odd_p(MaxMpz * r) {
	return mpz_odd_p(r->Value());
}

int bmx_gmp_mpz_even_p(MaxMpz * r) {
	return mpz_even_p(r->Value());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxMpf * bmx_gmp_mpf_init(int prec) {
	if (prec) {
		return new MaxMpf(prec);
	} else {
		return new MaxMpf;
	}
}

void bmx_gmp_mpf_set(MaxMpf * r, MaxMpf * op) {
	mpf_set(r->Value(), op->Value());
}

void bmx_gmp_mpf_set_default_prec(int prec) {
	mpf_set_default_prec(prec);
}

int bmx_gmp_mpf_get_default_prec() {
	return static_cast<int>(mpf_get_default_prec());
}

int bmx_gmp_mpf_get_prec(MaxMpf * r) {
	return static_cast<int>(mpf_get_prec(r->Value()));
}

void bmx_gmp_mpf_set_prec(MaxMpf * r, int prec) {
	mpf_set_prec(r->Value(), prec);
}

void bmx_gmp_mpf_free(MaxMpf * r) {
	delete r;
}

void bmx_gmp_mpf_setint(MaxMpf * r, int op) {
	mpf_set_si(r->Value(), op);
}

void bmx_gmp_mpf_setdouble(MaxMpf * r, double op) {
	mpf_set_d(r->Value(), op);
}

void bmx_gmp_mpf_setinteger(MaxMpf * r, MaxMpz * op) {
	mpf_set_z(r->Value(), op->Value());
}

void bmx_gmp_mpf_setrational(MaxMpf * r, MaxMpq * op) {
	mpf_set_q(r->Value(), op->Value());
}

int bmx_gmp_mpf_setstr(MaxMpf * r, BBString * str, int base) {
	char * s = bbStringToCString(str);
	int res = mpf_set_str(r->Value(), s, base);
	bbMemFree(s);
	return res;
}

void bmx_gmp_mpf_swap(MaxMpf * r, MaxMpf * op) {
	mpf_swap(r->Value(), op->Value());
}

double bmx_gmp_mpf_get_d(MaxMpf * r) {
	return mpf_get_d(r->Value());
}

double bmx_gmp_mpf_get_d_2exp(MaxMpf * r, int * _exp) {
	signed long int exp;
	double res = mpf_get_d_2exp(&exp, r->Value());
	*_exp = static_cast<int>(exp);
	return res;
}

int bmx_gmp_mpf_get_si(MaxMpf * r) {
	return static_cast<int>(mpf_get_si(r->Value()));
}

BBString * bmx_gmp_mpf_get_str(MaxMpf * r, int * _exp, int base) {
	mp_exp_t expptr;
	char * s = mpf_get_str(NULL, &expptr, base, 0, r->Value());
	BBString * str = bbStringFromCString(s);
	free(s);
	*_exp = static_cast<int>(expptr);
	return str;
}

MaxMpf * bmx_gmp_mpf_add(MaxMpf * r, MaxMpf * op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_add(mpf->Value(), r->Value(), op->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_addint(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_add_ui(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_sub(MaxMpf * r, MaxMpf * op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_sub(mpf->Value(), r->Value(), op->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_subint(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_sub_ui(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_intsub(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_ui_sub(mpf->Value(), op, r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_mul(MaxMpf * r, MaxMpf * op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_mul(mpf->Value(), r->Value(), op->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_mulint(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_mul_ui(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_div(MaxMpf * r, MaxMpf * op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_div(mpf->Value(), r->Value(), op->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_ui_div(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_ui_div(mpf->Value(), op, r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_div_ui(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_div_ui(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_sqrt(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_sqrt(mpf->Value(), r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_sqrt_ui(int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_sqrt_ui(mpf->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_pow_ui(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_pow_ui(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_mul2exp(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_mul_2exp(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_div_2exp(MaxMpf * r, int op) {
	MaxMpf * mpf = new MaxMpf;
	mpf_div_2exp(mpf->Value(), r->Value(), op);
	return mpf;
}

MaxMpf * bmx_gmp_mpf_neg(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_neg(mpf->Value(), r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_abs(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_abs(mpf->Value(), r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_ceil(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_ceil(mpf->Value(), r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_floor(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_floor(mpf->Value(), r->Value());
	return mpf;
}

MaxMpf * bmx_gmp_mpf_trunc(MaxMpf * r) {
	MaxMpf * mpf = new MaxMpf;
	mpf_trunc(mpf->Value(), r->Value());
	return mpf;
}

int bmx_gmp_mpf_integer_p(MaxMpf * r) {
	return mpf_integer_p(r->Value());
}


int bmx_gmp_mpf_cmp(MaxMpf * r, MaxMpf * op) {
	return mpf_cmp(r->Value(), op->Value());
}

int bmx_gmp_mpf_cmp_d(MaxMpf * r, double op) {
	return mpf_cmp_d(r->Value(), op);
}

int bmx_gmp_mpf_cmp_si(MaxMpf * r, int op) {
	return mpf_cmp_si(r->Value(), op);
}

int bmx_gmp_mpf_eq(MaxMpf * r, MaxMpf * op1, int op2) {
	return mpf_eq(r->Value(), op1->Value(), op2);
}

MaxMpf * bmx_gmp_mpf_reldiff(MaxMpf * r, MaxMpf * op2) {
	MaxMpf * mpf = new MaxMpf;
	mpf_reldiff(mpf->Value(), r->Value(), op2->Value());
	return mpf;
}

int bmx_gmp_mpf_sgn(MaxMpf * r) {
	return mpf_sgn(r->Value());
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

