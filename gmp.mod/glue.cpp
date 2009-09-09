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
	void bmx_gmp_mpz_add(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_addint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_sub(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_subint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_intsub(MaxMpz * mpz, int op1, MaxMpz * op2);
	void bmx_gmp_mpz_mul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_mulint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_addmul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_addmulint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_submul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2);
	void bmx_gmp_mpz_submulint(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_mul2exp(MaxMpz * mpz, MaxMpz * op1, int op2);
	void bmx_gmp_mpz_neg(MaxMpz * mpz, MaxMpz * op);
	void bmx_gmp_mpz_abs(MaxMpz * mpz, MaxMpz * op);
	void bmx_gmp_mpz_cdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_cdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_cdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d);
	int bmx_gmp_mpz_cdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_cdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_cdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d);
	int bmx_gmp_mpz_cdiv_ui(MaxMpz * mpz, int d);
	void bmx_gmp_mpz_cdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b);
	void bmx_gmp_mpz_cdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b);
	void bmx_gmp_mpz_fdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_fdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_fdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d);
	int bmx_gmp_mpz_fdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_fdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_fdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d);
	int bmx_gmp_mpz_fdiv_ui(MaxMpz * mpz, int d);
	void bmx_gmp_mpz_fdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b);
	void bmx_gmp_mpz_fdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b);
	void bmx_gmp_mpz_tdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_tdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d);
	void bmx_gmp_mpz_tdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d);
	int bmx_gmp_mpz_tdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_tdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d);
	int bmx_gmp_mpz_tdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d);
	int bmx_gmp_mpz_tdiv_ui(MaxMpz * mpz, int d);
	void bmx_gmp_mpz_tdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b);
	void bmx_gmp_mpz_tdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b);

	
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

void bmx_gmp_mpz_add(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2) {
	mpz_add(mpz->Value(), op1->Value(), op2->Value());
}

void bmx_gmp_mpz_addint(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_add_ui(mpz->Value(), op1->Value(), op2);
}

void bmx_gmp_mpz_sub(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2) {
	mpz_sub(mpz->Value(), op1->Value(), op2->Value());
}

void bmx_gmp_mpz_subint(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_sub_ui(mpz->Value(), op1->Value(), op2);
}

void bmx_gmp_mpz_intsub(MaxMpz * mpz, int op1, MaxMpz * op2) {
	mpz_ui_sub(mpz->Value(), op1, op2->Value());
}

void bmx_gmp_mpz_mul(MaxMpz * mpz, MaxMpz * op1, MaxMpz * op2) {
	mpz_mul(mpz->Value(), op1->Value(), op2->Value());
}

void bmx_gmp_mpz_mulint(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_mul_si(mpz->Value(), op1->Value(), op2);
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

void bmx_gmp_mpz_mul2exp(MaxMpz * mpz, MaxMpz * op1, int op2) {
	mpz_mul_2exp(mpz->Value(), op1->Value(), op2);
}

void bmx_gmp_mpz_neg(MaxMpz * mpz, MaxMpz * op) {
	mpz_neg(mpz->Value(), op->Value());
}

void bmx_gmp_mpz_abs(MaxMpz * mpz, MaxMpz * op) {
	mpz_abs(mpz->Value(), op->Value());
}

void bmx_gmp_mpz_cdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_cdiv_q(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_cdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_cdiv_r(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_cdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d) {
	mpz_cdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
}

int bmx_gmp_mpz_cdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_cdiv_q_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_cdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_cdiv_r_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_cdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d) {
	return mpz_cdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d);
}

int bmx_gmp_mpz_cdiv_ui(MaxMpz * mpz, int d) {
	return mpz_cdiv_ui(mpz->Value(), d);
}

void bmx_gmp_mpz_cdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_cdiv_q_2exp(mpz->Value(), n->Value(), b);
}

void bmx_gmp_mpz_cdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_cdiv_r_2exp(mpz->Value(), n->Value(), b);
}

void bmx_gmp_mpz_fdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_fdiv_q(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_fdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_fdiv_r(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_fdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d) {
	mpz_fdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
}

int bmx_gmp_mpz_fdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_fdiv_q_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_fdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_fdiv_r_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_fdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d) {
	return mpz_fdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d);
}

int bmx_gmp_mpz_fdiv_ui(MaxMpz * mpz, int d) {
	return mpz_fdiv_ui(mpz->Value(), d);
}

void bmx_gmp_mpz_fdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_fdiv_q_2exp(mpz->Value(), n->Value(), b);
}

void bmx_gmp_mpz_fdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_fdiv_r_2exp(mpz->Value(), n->Value(), b);
}

void bmx_gmp_mpz_tdiv_q(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_tdiv_q(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_tdiv_r(MaxMpz * mpz, MaxMpz * n, MaxMpz * d) {
	mpz_tdiv_r(mpz->Value(), n->Value(), d->Value());
}

void bmx_gmp_mpz_tdiv_qr(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, MaxMpz * d) {
	mpz_tdiv_qr(mpz->Value(), r->Value(), n->Value(), d->Value());
}

int bmx_gmp_mpz_tdiv_q_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_tdiv_q_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_tdiv_r_ui(MaxMpz * mpz, MaxMpz * n, int d) {
	return mpz_tdiv_r_ui(mpz->Value(), n->Value(), d);
}

int bmx_gmp_mpz_tdiv_qr_ui(MaxMpz * mpz, MaxMpz * r, MaxMpz * n, int d) {
	return mpz_tdiv_qr_ui(mpz->Value(), r->Value(), n->Value(), d);
}

int bmx_gmp_mpz_tdiv_ui(MaxMpz * mpz, int d) {
	return mpz_tdiv_ui(mpz->Value(), d);
}

void bmx_gmp_mpz_tdiv_q_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_tdiv_q_2exp(mpz->Value(), n->Value(), b);
}

void bmx_gmp_mpz_tdiv_r_2exp(MaxMpz * mpz, MaxMpz * n, int b) {
	mpz_tdiv_r_2exp(mpz->Value(), n->Value(), b);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
