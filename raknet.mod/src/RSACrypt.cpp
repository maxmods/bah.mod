#include "RSACrypt.h"
//#include "Random.hpp"
#include "Rand.h"
#include "BigInt.h"
#if (defined(__GNUC__)  || defined(__GCCXML__))
#include <alloca.h>
#else
#include <malloc.h> // alloca
#endif
#include "RakMemoryOverride.h"
using namespace big;


RSACrypt::RSACrypt()
	: p(0), q(0), pinvq(0), d(0), modulus(0)
{
	e = 0;
}

RSACrypt::~RSACrypt()
{
	cleanup();
}

void RSACrypt::cleanup()
{
	e = 0;
	if (p) RakNet::OP_DELETE_ARRAY(p);
	p = 0;
	if (q)RakNet::OP_DELETE_ARRAY(q);
	q = 0;
	if (pinvq) RakNet::OP_DELETE_ARRAY(pinvq);
	pinvq = 0;
	if (d) RakNet::OP_DELETE_ARRAY(d);
	d = 0;
	if (modulus) RakNet::OP_DELETE_ARRAY(modulus);
	modulus = 0;
}

bool RSACrypt::generateExponents(const uint32_t *p, const uint32_t *q, int limbs, uint32_t &e, uint32_t *d)
{
	// pm = p-1
	uint32_t *pm = (uint32_t *)alloca(limbs*4);
	Set(pm, limbs, p);
	Subtract32(pm, limbs, 1);

	// qm = q-1
	uint32_t *qm = (uint32_t *)alloca(limbs*4);
	Set(qm, limbs, q);
	Subtract32(qm, limbs, 1);

	// phi = (p-1)*(q-1)
	uint32_t *phi = (uint32_t *)alloca(limbs*2*4);
	Multiply(limbs, phi, pm, qm);

	e = 65537-2;

	// e = first number relatively prime to phi, starting at 65537
	uint32_t r;
	do {
		e += 2;
		GCD(&e, 1, phi, limbs*2, &r);
	} while (r != 1 && e >= 65537);

	if (r != 1) return false;

	// Compute private exponent
	return InvMod(&e, 1, phi, limbs*2, d);
}

bool RSACrypt::setPrivateKey(const uint32_t *pi, const uint32_t *qi, int factor_limbsi)
{
	cleanup();

	factor_limbs = factor_limbsi;
	mod_limbs = factor_limbs * 2;

	p = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs);
	q = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs);
	pinvq = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs);
	modulus = RakNet::OP_NEW_ARRAY<uint32_t>(mod_limbs);
	d = RakNet::OP_NEW_ARRAY<uint32_t>(mod_limbs);
	if (!p || !q || !pinvq || !modulus || !d) return false;

	Set(p, factor_limbs, pi);
	p_inv = MonModInv(p[0]);

	Set(q, factor_limbs, qi);
	q_inv = MonModInv(q[0]);

	if (!InvMod(p, factor_limbs, q, factor_limbs, pinvq))
		return false;

	if (!generateExponents(p, q, factor_limbs, e, d))
		return false;

	Multiply(factor_limbs, modulus, p, q);

	mod_inv = MonModInv(modulus[0]);

	return true;
}

bool RSACrypt::setPublicKey(const uint32_t *modulusi, int mod_limbsi, uint32_t ei)
{
	cleanup();

	e = ei;

	mod_limbs = mod_limbsi;

	modulus = RakNet::OP_NEW_ARRAY<uint32_t>(mod_limbs);
	if (!modulus) return false;

	Set(modulus, mod_limbs, modulusi);

	mod_inv = MonModInv(modulus[0]);

	return true;
}

// Rabin-Miller method for finding a strong pseudo-prime
// Preconditions: High bit and low bit of n = 1
bool primeTest(const uint32_t *n, int limbs, uint32_t k)
{
	// n1 = n - 1
	uint32_t *n1 = (uint32_t *)alloca(limbs*4);
	Set(n1, limbs, n);
	Subtract32(n1, limbs, 1);

	// d = n1
	uint32_t *d = (uint32_t *)alloca(limbs*4);
	Set(d, limbs, n1);

	// remove factors of two from d
	while (!(d[0] & 1))
		BitShiftRight(d, limbs, d, limbs, 1);

	uint32_t *a = (uint32_t *)alloca(limbs*4);
	uint32_t *t = (uint32_t *)alloca(limbs*4);
	uint32_t *p = (uint32_t *)alloca((limbs*2)*4);
	uint32_t n_inv = MonModInv(n[0]);

	// iterate k times
	while (k--)
	{
//		do Random::ref()->generate(a, limbs*4);
		do fillBufferMT(a,limbs*4);
		while (GreaterOrEqual(a, limbs, n, limbs));

		// a = a ^ d (Mod n)
		ExpMod(a, limbs, d, limbs, n, limbs, n_inv, a);

		Set(t, limbs, d);
		while (!Equal(limbs, t, n1) &&
			   !Equal32(a, limbs, 1) &&
			   !Equal(limbs, a, n1))
		{
			// y = y^2 (Mod n), non-critical path
			Square(limbs, p, a);
			SchoolbookDivide(p, limbs, n, limbs, p/*ignore*/, a);

			// t <<= 1
			BitShiftLeft(t, limbs, t, limbs, 1);
		}

		if (!Equal(limbs, a, n1) && !(t[0] & 1)) return false;
	}
	
	return true;
}

// generates a strong pseudo-prime

void generateStrongPseudoPrime(uint32_t *n, int limbs)
{
	do {
		//Random::ref()->generate(n, limbs*4);
		fillBufferMT(n,limbs*4);
		n[limbs-1] |= 0x80000000;
		n[0] |= 1;
	} while (!primeTest(n, limbs, 20));
}

bool RSACrypt::generatePrivateKey(uint32_t limbs)
{
	uint32_t *pf = (uint32_t *)alloca(limbs*4);
	generateStrongPseudoPrime(pf, limbs/2);
	
	uint32_t *qf = (uint32_t *)alloca(limbs*4);
	generateStrongPseudoPrime(qf, limbs/2);

	return setPrivateKey(pf, qf, limbs/2);
}

uint32_t RSACrypt::getFactorLimbs()
{
	return factor_limbs;
}

void RSACrypt::getPrivateP(uint32_t *po)
{
	Set(po, factor_limbs, p);
}

void RSACrypt::getPrivateQ(uint32_t *qo)
{
	Set(qo, factor_limbs, q);
}

uint32_t RSACrypt::getModLimbs()
{
	return mod_limbs;
}

void RSACrypt::getPublicModulus(uint32_t *moduluso)
{
	Set(moduluso, mod_limbs, modulus);
}

uint32_t RSACrypt::getPublicExponent()
{
	return e;
}

bool RSACrypt::encrypt(uint32_t *ct, const uint32_t *pt)
{
	if (!e) return false;
	ExpMod(pt, mod_limbs, &e, 1, modulus, mod_limbs, mod_inv, ct);
	return true;
}

bool RSACrypt::decrypt(uint32_t *pt, const uint32_t *ct)
{
	if (!e) return false;
	ExpCRT(ct, mod_limbs, d, mod_limbs, p, p_inv, q, q_inv, pinvq, factor_limbs, pt);
	return true;
}
