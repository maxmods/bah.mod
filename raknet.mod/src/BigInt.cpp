#if !defined(_XBOX) && !defined(X360)

#include "BigInt.h"
#include <ctype.h>
#include <string.h>

#include "RakAlloca.h"
#include "RakMemoryOverride.h"

namespace big
{
	static const char Bits256[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

	// returns the degree of the base 2 monic polynomial
	// (the number of bits used to represent the number)
	// eg, 0 0 0 0 1 0 1 1 ... => 28 out of 32 used
#ifdef _MSC_VER
#pragma warning(disable:4706)
#endif
	uint32_t Degree(uint32_t v)
	{
		uint32_t r, t = v >> 16;

		if (t)	r = (r = t >> 8) ? 24 + Bits256[r] : 16 + Bits256[t];
		else 	r = (r = v >> 8) ? 8 + Bits256[r] : Bits256[v];

		return r;
	}

	// returns the number of limbs that are actually used
	int LimbDegree(const uint32_t *n, int limbs)
	{
		while (limbs--)
			if (n[limbs])
				return limbs + 1;

		return 0;
	}

	// return bits used
	uint32_t Degree(const uint32_t *n, int limbs)
	{
		uint32_t limb_degree = LimbDegree(n, limbs);
		if (!limb_degree) return 0;
		--limb_degree;

		uint32_t msl_degree = n[limb_degree];

		return msl_degree + limb_degree*32;
	}

	void Set(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int min = lhs_limbs < rhs_limbs ? lhs_limbs : rhs_limbs;

		memcpy(lhs, rhs, min*4);
		memset(&lhs[min], 0, (lhs_limbs - min)*4);
	}
	void Set(uint32_t *lhs, int limbs, const uint32_t *rhs)
	{
		memcpy(lhs, rhs, limbs*4);
	}
	void Set32(uint32_t *lhs, int lhs_limbs, const uint32_t rhs)
	{
		*lhs = rhs;
		memset(&lhs[1], 0, (lhs_limbs - 1)*4);
	}

	bool Less(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		for (int ii = limbs-1; ii >= 0; --ii)
		{
			if (lhs[ii] != rhs[ii])
			{
				return lhs[ii] < rhs[ii];
			}
		}

		return false;
	}
	bool Greater(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		for (int ii = limbs-1; ii >= 0; --ii)
		{
			if (lhs[ii] != rhs[ii])
			{
				return lhs[ii] > rhs[ii];
			}
		}

		return false;
	}
	bool Equal(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		return 0 == memcmp(lhs, rhs, limbs*4);
	}

	bool Less(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return false; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return true; while (lhs_limbs < rhs_limbs);

		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return lhs[lhs_limbs] < rhs[lhs_limbs];
		return false; // equal
	}
	bool Greater(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return true; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return false; while (lhs_limbs < rhs_limbs);

		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return lhs[lhs_limbs] > rhs[lhs_limbs];
		return false; // equal
	}
	bool Equal(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return false; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return false; while (lhs_limbs < rhs_limbs);

		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return false;
		return true; // equal
	}

	bool Equal32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		if (*lhs != rhs) return false;
		while (--lhs_limbs)
			if (*++lhs) return false;
		return true; // equal
	}

	// lhs = rhs >>> shift
	// Precondition: 0 <= shift < 31
	void BitShiftRight(uint32_t *result, int result_limbs, const uint32_t *lhs, int lhs_limbs, int rhs_shift)
	{
		if (!rhs_shift)
		{
			Set(result, result_limbs, lhs, lhs_limbs);
			return;
		}

		uint32_t Carrie = 0;

		if (result_limbs > lhs_limbs)
			do result[--result_limbs] = 0; while (result_limbs > lhs_limbs);
		else if (result_limbs < lhs_limbs)
			Carrie = lhs[result_limbs] << (32 - rhs_shift);

		do {
			uint32_t n = lhs[--result_limbs];
			result[result_limbs] = Carrie | (n >> rhs_shift);
			Carrie = n << (32 - rhs_shift);
		} while (result_limbs);
	}

	// lhs = rhs <<< shift
	// Precondition: 0 <= shift < 31
	void BitShiftLeft(uint32_t *result, int result_limbs, const uint32_t *lhs, int lhs_limbs, int rhs_shift)
	{
		if (!rhs_shift)
		{
			Set(result, result_limbs, lhs, lhs_limbs);
			return;
		}

		uint32_t Carrie = 0;
		int ii = 0;

		if (result_limbs < lhs_limbs)
			lhs_limbs = result_limbs;

		for (; ii < lhs_limbs; ++ii)
		{
			uint32_t n = lhs[ii];
			result[ii] = (n << rhs_shift) | Carrie;
			Carrie = n >> (32 - rhs_shift);
		}

		if (result_limbs > lhs_limbs)
		{
			result[ii++] = Carrie;
			for (; ii < result_limbs; ++ii)
				result[ii] = 0;
		}
	}

	// n >>= shift*32
	void LimbShiftRight(uint32_t *n, int limbs, int rhs_shift)
	{
		limbs -= rhs_shift;

		while (limbs--)
		{
			n[0] = n[rhs_shift];
			++n;
		}

		while (rhs_shift--)
			*n++ = 0;
	}

	// n <<= shift*32
	void LimbShiftLeft(uint32_t *n, int limbs, int rhs_shift)
	{
		while (limbs-- > rhs_shift)
			n[limbs] = n[limbs-rhs_shift];

		while (limbs--)
			*n-- = 0;
	}

	// lhs += rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		uint64_t r = 0;

		for (ii = 0; ii < rhs_limbs; ++ii)
		{
			r >>= 32;
			r += (uint64_t)lhs[ii] + rhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (uint32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		return (uint32_t)(r >> 32);
	}

	// out = lhs + rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		uint64_t r = 0;

		for (ii = 0; ii < rhs_limbs; ++ii)
		{
			r >>= 32;
			r += (uint64_t)lhs[ii] + rhs[ii];
			out[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (uint32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			out[ii] = (uint32_t)r;
		}

		return (uint32_t)(r >> 32);
	}

	// lhs += rhs, return carry out
	// precondition: lhs_limbs > 0
	uint32_t Add32(uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		uint32_t n = lhs[0];
		uint32_t r = n + rhs;
		lhs[0] = r;

		if (r >= n)
			return 0;

		for (int ii = 1; ii < lhs_limbs; ++ii)
			if (++lhs[ii])
				return 0;

		return 1;
	}

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		int64_t r = 0;

		for (ii = 0; ii < rhs_limbs; ++ii)
		{
			r >>= 32;
			r += (int64_t)lhs[ii] - rhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (int32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		return (int32_t)(r >> 32);
	}

	// out = lhs - rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		int64_t r = 0;

		for (ii = 0; ii < rhs_limbs; ++ii)
		{
			r >>= 32;
			r += (int64_t)lhs[ii] - rhs[ii];
			out[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (int32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			out[ii] = (uint32_t)r;
		}

		return (int32_t)(r >> 32);
	}

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs > 0, result limbs = lhs_limbs
	int32_t Subtract32(uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		uint32_t n = lhs[0];
		uint32_t r = n - rhs;
		lhs[0] = r;

		if (r <= n)
			return 0;

		for (int ii = 1; ii < lhs_limbs; ++ii)
			if (lhs[ii]--)
				return 0;

		return -1;
	}

	// n = -n
	void Negate(uint32_t *n, int limbs)
	{
		int64_t r = 0;
		for (int ii = 0; ii < limbs; ++ii)
		{
			r >>= 32;
			r -= (int32_t)n[ii];
			n[ii] = (int32_t)r;
		}
	}

	// n = ~n, only invert bits up to the MSB, but none above that
	void BitNot(uint32_t *n, int limbs)
	{
		limbs = LimbDegree(n, limbs);
		if (limbs)
		{
			uint32_t high = n[--limbs];
			uint32_t high_degree = 32 - Degree(high);

			n[limbs] = ((uint32_t)(~high << high_degree) >> high_degree);
			while (limbs--) n[limbs] = ~n[limbs];
		}
	}

	// n = ~n, invert all bits, even ones above MSB
	void LimbNot(uint32_t *n, int limbs)
	{
		while (limbs--) *n++ = ~(*n);
	}

	// Return the carry out from A += B << S
    uint32_t AddLeftShift32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t S)			// 32-bit number
	{
		uint64_t sum = 0;
		uint32_t last = 0;

		while (limbs--)
		{
			uint32_t b = *B++;

			sum = (uint64_t)((b << S) | (last >> (32 - S))) + *A + (uint32_t)(sum >> 32);

			last = b;
			*A++ = (uint32_t)sum;
		}

		return (uint32_t)(sum >> 32) + (last >> (32 - S));
	}

	// Return the carry out from result = A * B
    uint32_t Multiply32(
    	int limbs,		// Number of limbs in parameter A, result
    	uint32_t *result,	// Large number
    	const uint32_t *A,	// Large number
    	uint32_t B)			// 32-bit number
	{
		uint64_t p = (uint64_t)A[0] * B;
		result[0] = (uint32_t)p;

		while (--limbs)
		{
			p = (uint64_t)*(++A) * B + (uint32_t)(p >> 32);
			*(++result) = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
	}

	// Return the carry out from X = X * M + A
    uint32_t MultiplyAdd32(
    	int limbs,	// Number of limbs in parameter A and B
    	uint32_t *X,		// Large number
    	uint32_t M,		// Large number
    	uint32_t A)		// 32-bit number
	{
		uint64_t p = (uint64_t)X[0] * M + A;
		X[0] = (uint32_t)p;

		while (--limbs)
		{
			p = (uint64_t)*(++X) * M + (uint32_t)(p >> 32);
			*X = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
	}

	// Return the carry out from A += B * M
    uint32_t AddMultiply32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t M)			// 32-bit number
	{
		// This function is roughly 85% of the cost of exponentiation
#if defined(ASSEMBLY_INTEL_SYNTAX) && !defined (_XBOX) && !defined(X360)
		ASSEMBLY_BLOCK // VS.NET, x86, 32-bit words
		{
			mov esi, [B]
			mov edi, [A]
			mov eax, [esi]
			mul [M]					; (edx,eax) = [M]*[esi]
			add eax, [edi]			; (edx,eax) += [edi]
			adc edx, 0
			; (edx,eax) = [B]*[M] + [A]

			mov [edi], eax
			; [A] = eax

			mov ecx, [limbs]
			sub ecx, 1
			jz loop_done
loop_head:
				lea esi, [esi + 4]	; ++B
				mov eax, [esi]		; eax = [B]
				mov ebx, edx		; ebx = last carry
				lea edi, [edi + 4]	; ++A
				mul [M]				; (edx,eax) = [M]*[esi]
				add eax, [edi]		; (edx,eax) += [edi]
				adc edx, 0
				add eax, ebx		; (edx,eax) += ebx
				adc edx, 0
				; (edx,eax) = [esi]*[M] + [edi] + (ebx=last carry)

				mov [edi], eax
				; [A] = eax

			sub ecx, 1
			jnz loop_head
loop_done:
			mov [M], edx	; Use [M] to copy the carry into C++ land
		}

		return M;
#else
		// Unrolled first loop
		uint64_t p = B[0] * (uint64_t)M + A[0];
		A[0] = (uint32_t)p;

		while (--limbs)
		{
			p = *(++B) * (uint64_t)M + *(++A) + (uint32_t)(p >> 32);
			A[0] = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
#endif
	}

	// product = x * y
	void SimpleMultiply(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x,	// Large number
		const uint32_t *y)	// Large number
	{
		// Roughly 25% of the cost of exponentiation
		product[limbs] = Multiply32(limbs, product, x, y[0]);

		uint32_t ctr = limbs;
		while (--ctr)
		{
			++product;
			product[limbs] = AddMultiply32(limbs, product, x, (++y)[0]);
		}
	}

	// product = x ^ 2
	void SimpleSquare(
		int limbs,		// Number of limbs in parameter x
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x)	// Large number
	{
		// Seems about 15% faster than SimpleMultiply() in practice
		uint32_t *cross_product = (uint32_t*)alloca(limbs*2*4);

		// Calculate square-less and repeat-less cross products
		cross_product[limbs] = Multiply32(limbs - 1, cross_product + 1, x + 1, x[0]);
		for (int ii = 1; ii < limbs - 1; ++ii)
		{
			cross_product[limbs + ii] = AddMultiply32(limbs - ii - 1, cross_product + ii*2 + 1, x + ii + 1, x[ii]);
		}

		// Calculate square products
		for (int ii = 0; ii < limbs; ++ii)
		{
			uint32_t xi = x[ii];
			uint64_t si = (uint64_t)xi * xi;
			product[ii*2] = (uint32_t)si;
			product[ii*2+1] = (uint32_t)(si >> 32);
		}

		// Multiply the cross product by 2 and add it to the square products
		product[limbs*2 - 1] += AddLeftShift32(limbs*2 - 2, product + 1, cross_product + 1, 1);
	}

	// product = low half of x * y product
	void SimpleMultiplyLowHalf(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs
		const uint32_t *x,	// Large number
		const uint32_t *y)	// Large number
	{
		memset(product, 0, limbs*4);

		for (int ii = 0; ii < limbs; ++ii)
		{
			AddMultiply32(limbs - ii, product + ii, x, y[ii]);
		}
	}

	/*
	 * Multiply two large numbers using the Schoolbook method
	 * Only produces the low y_limbs of the result
	 *
	 * The product buffer may not be pointed to by x or y
	 */
	void HalfSchoolbookMultiply(
		uint32_t *product,	// Buffer size = x_limbs+y_limbs
		const uint32_t *x,	// Number to multiply, buffer size = x_limbs
		int x_limbs,	// Size of x
		const uint32_t *y,	// Number to multiply, buffer size = y_limbs
		int y_limbs)	// Size of y
	{
		// Insure that y is the shorter number
		if (x_limbs < y_limbs)
		{
			HalfSchoolbookMultiply(product, y, y_limbs, x, x_limbs);
			return;
		}

		if (x_limbs == y_limbs)
		{
			SimpleMultiplyLowHalf(x_limbs, product, x, y);
			return;
		}

		uint64_t high = 0;

		/* Compute the low y_limbs of output
		 * x =         P  A  B  C  D
		 * y =               E  F  G
		 *         -----------------
		 *            ?? ?? BG CG DG
		 *         ?? ?? ?? CF DF
		 *    + ?? ?? ?? ?? DE
		 *   -----------------------
		 *   ?? ?? ?? ?? ?? ^^ ^^ ^^
		 */
		for (int ii = 0; ii < y_limbs; ++ii)
		{
			uint64_t low = high;
			high >>= 32;

			for (int jj = 0; jj <= ii; ++jj)
			{
				low = (uint64_t)x[jj] * y[ii-jj] + (uint32_t)low;
				high += low >> 32;
			}

			product[ii] = (uint32_t)low;
		}
	}

	/*
	 * Multiply two large numbers using the Schoolbook method
	 *
	 * The product buffer may not be pointed to by x or y
	 */
	void SchoolbookMultiply(
		uint32_t *product,	// Buffer size = x_limbs+y_limbs
		const uint32_t *x,	// Number to multiply, buffer size = x_limbs
		int x_limbs,	// Size of x
		const uint32_t *y,	// Number to multiply, buffer size = y_limbs
		int y_limbs)	// Size of y
	{
		// Insure that y is the shorter number
		if (x_limbs < y_limbs)
		{
			SchoolbookMultiply(product, y, y_limbs, x, x_limbs);
			return;
		}

		if (x_limbs == y_limbs)
		{
			Multiply(y_limbs, product, x, y);
			return;
		}

		int ii;
		uint64_t high = 0;

		/* Compute the low y_limbs of output
		 * x =         P  A  B  C  D
		 * y =               E  F  G
		 *         -----------------
		 *            ?? ?? BG CG DG
		 *         ?? ?? ?? CF DF
		 *    + ?? ?? ?? ?? DE
		 *   -----------------------
		 *   ?? ?? ?? ?? ?? ^^ ^^ ^^
		 */
		for (ii = 0; ii < y_limbs; ++ii)
		{
			uint64_t low = high;
			high >>= 32;

			for (int jj = 0; jj <= ii; ++jj)
			{
				low = (uint64_t)x[jj] * y[ii-jj] + (uint32_t)low;
				high += low >> 32;
			}

			product[ii] = (uint32_t)low;
		}

		/* Compute the middle x_limbs - y_limbs of output
		 * x =         P  A  B  C  D
		 * y =               E  F  G
		 *         -----------------
		 *            PG AG ?? ?? ??
		 *         ?? AF BF ?? ??
		 *    + ?? ?? BE CE ??
		 *   -----------------------
		 *   ?? ?? ?? ^^ ^^ ?? ?? ??
		 */
		for (; ii < x_limbs; ++ii)
		{
			uint64_t low = high;
			high >>= 32;

			for (int jj = 0; jj < y_limbs; ++jj)
			{
				low = (uint64_t)x[ii-jj] * y[jj] + (uint32_t)low;
				high += low >> 32;
			}

			product[ii] = (uint32_t)low;
		}

		/* Compute the high y_limbs-1 of output
		 * x =         P  A  B  C  D
		 * y =               E  F  G
		 *         -----------------
		 *            ?? ?? ?? ?? ??
		 *         PF ?? ?? ?? ??
		 *    + PE AE ?? ?? ??
		 *   -----------------------
		 *   ?? ^^ ^^ ?? ?? ?? ?? ??
		 */
		for (int kk = y_limbs-2; kk >= 0; --kk)
		{
			uint64_t low = high;
			high >>= 32;

			for (int jj = 0; jj <= kk; ++jj)
			{
				low = (uint64_t)x[x_limbs-1-jj] * y[y_limbs-1-kk+jj] + (uint32_t)low;
				high += low >> 32;
			}

			product[ii++] = (uint32_t)low;
		}

		/* Record carry into most significant limb
		 * x =         P  A  B  C  D
		 * y =               E  F  G
		 *         -----------------
		 *            ?? ?? ?? ?? ??
		 *         ?? ?? ?? ?? ??
		 *    + ?? ?? ?? ?? ??
		 *   -----------------------
		 *   ^^ ?? ?? ?? ?? ?? ?? ??
		 */
		product[ii] = (uint32_t)high;
	}

	// product = xy
	// memory space for product may not overlap with x,y
    void Multiply(
    	int limbs,		// Number of limbs in x,y
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x,	// Large number; buffer size = limbs
    	const uint32_t *y)	// Large number; buffer size = limbs
	{
		// Stop recursing under 640 bits or odd limb count
		if (limbs < 20 || (limbs & 1))
		{
			SimpleMultiply(limbs, product, x, y);
			return;
		}

		// Compute high and low products
		Multiply(limbs/2, product, x, y);
		Multiply(limbs/2, product + limbs, x + limbs/2, y + limbs/2);

		// Compute (x1 + x2), xc = carry out
		uint32_t *xsum = (uint32_t*)alloca((limbs/2)*4);
		uint32_t xcarry = Add(xsum, x, limbs/2, x + limbs/2, limbs/2);

		// Compute (y1 + y2), yc = carry out
		uint32_t *ysum = (uint32_t*)alloca((limbs/2)*4);
		uint32_t ycarry = Add(ysum, y, limbs/2, y + limbs/2, limbs/2);

		// Compute (x1 + x2) * (y1 + y2)
		uint32_t *cross_product = (uint32_t*)alloca(limbs*4);
		Multiply(limbs/2, cross_product, xsum, ysum);

		// Subtract out the high and low products
		int32_t cross_carry = Subtract(cross_product, limbs, product, limbs);
		cross_carry += Subtract(cross_product, limbs, product + limbs, limbs);

		// Fix the extra high carry bits of the result
		if (ycarry) cross_carry += Add(cross_product + limbs/2, limbs/2, xsum, limbs/2);
		if (xcarry) cross_carry += Add(cross_product + limbs/2, limbs/2, ysum, limbs/2);
		cross_carry += (xcarry & ycarry);

		// Add the cross product into the result
		cross_carry += Add(product + limbs/2, limbs*3/2, cross_product, limbs);

		// Add in the fixed high carry bits
		if (cross_carry) Add32(product + limbs*3/2, limbs/2, cross_carry);
	}

	// product = x^2
	// memory space for product may not overlap with x
    void Square(
    	int limbs,		// Number of limbs in x
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x)	// Large number; buffer size = limbs
	{
		// Stop recursing under 1280 bits or odd limb count
		if (limbs < 40 || (limbs & 1))
		{
			SimpleSquare(limbs, product, x);
			return;
		}

		// Compute high and low squares
		Square(limbs/2, product, x);
		Square(limbs/2, product + limbs, x + limbs/2);

		// Generate the cross product
		uint32_t *cross_product = (uint32_t*)alloca(limbs*4);
		Multiply(limbs/2, cross_product, x, x + limbs/2);

		// Multiply the cross product by 2 and add it to the result
		uint32_t cross_carry = AddLeftShift32(limbs, product + limbs/2, cross_product, 1);

		// Roll the carry out up to the highest limb
		if (cross_carry) Add32(product + limbs*3/2, limbs/2, cross_carry);
	}

	// Multiply two large, 2's complement signed numbers: result = a0 * b0
    void SignedMultiply(
    	int limbs,		// Number of limbs in parameters a0,b0
    	uint32_t *result,	// Output, buffer size = limbs*2
    	const uint32_t *a0,	// Large number, buffer size = limbs
    	const uint32_t *b0)	// Large number, buffer size = limbs
	{
		const uint32_t *a, *b;
		uint32_t *a1 = (uint32_t*)alloca(limbs*4);
		uint32_t *b1 = (uint32_t*)alloca(limbs*4);

		uint32_t a_high = a0[limbs-1] & 0x80000000;

		if (a_high) {
			Set(a1, limbs, a0, limbs);
			Negate(a1, limbs);
			a = a1;
		} else
			a = a0;

		uint32_t b_high = b0[limbs-1] & 0x80000000;

		if (b_high) {
			Set(b1, limbs, b0, limbs);
			Negate(b1, limbs);
			b = b1;
		} else
			b = b0;

		Multiply(limbs, result, a, b);

		if (a_high ^ b_high)
			Negate(result, limbs);
	}

	// Returns the remainder of N / divisor for a 32-bit divisor
    uint32_t Modulus32(
    	int limbs,		// Number of limbs in parameter N
    	const uint32_t *N,	// Large number, buffer size = limbs
    	uint32_t divisor)	// 32-bit number
	{
		uint32_t remainder = N[limbs-1] < divisor ? N[limbs-1] : 0;
		uint32_t counter = N[limbs-1] < divisor ? limbs-1 : limbs;

		while (counter--) remainder = (uint32_t)((((uint64_t)remainder << 32) | N[counter]) % divisor);

		return remainder;
	}


	/*
	 * 'A' is overwritten with the quotient of the operation
	 * Returns the remainder of 'A' / divisor for a 32-bit divisor
	 *
	 * Does not check for divide-by-zero
	 */
    uint32_t Divide32(
    	int limbs,		// Number of limbs in parameter A
    	uint32_t *A,			// Large number, buffer size = limbs
    	uint32_t divisor)	// 32-bit number
	{
		uint64_t r = 0;
		for (int ii = limbs-1; ii >= 0; --ii)
		{
			uint64_t n = (r << 32) | A[ii];
			A[ii] = (uint32_t)(n / divisor);
			r = n % divisor;
		}

		return (uint32_t)r;
	}

	// returns (n ^ -1) Mod 2^32
	uint32_t MulInverseGF32(uint32_t n)
	{
		// {u1, g1} = 2^32 / n
		uint32_t hb = (~(n - 1) >> 31);
		uint32_t u1 = -(int32_t)(0xFFFFFFFF / n + hb);
		uint32_t g1 = ((-(int32_t)hb) & (0xFFFFFFFF % n + 1)) - n;

		if (!g1) {
			if (n != 1) return 0;
			else return 1;
		}

		uint32_t q, u = 1, g = n;

		for (;;) {
			q = g / g1;
			g %= g1;

			if (!g) {
				if (g1 != 1) return 0;
				else return u1;
			}

			u -= q*u1;
			q = g1 / g;
			g1 %= g;

			if (!g1) {
				if (g != 1) return 0;
				else return u;
			}

			u1 -= q*u;
		}
	}

	/*
	 * Schoolbook division algorithm
	 *
	 * Returns true on success and false on failure (like divide by 0)
	 *
	 * Quotient and Remainder pointers can be the same as any other
	*/
	bool SchoolbookDivide(
		const uint32_t *dividend,	// Large number (numerator), buffer size = dividend_limbs
		int dividend_limbs,		// Dividend limbs
		const uint32_t *divisor,		// Large number (denominator), buffer size = divisor_limbs
		int divisor_limbs,		// Divisor limbs
		uint32_t *quotient,			// Quotient of division, buffer size = dividend_limbs
		uint32_t *remainder)			// Remainder of division, buffer size = divisor_limbs
	{
		// Find divisor MSB limb
		uint32_t divisor_used = LimbDegree(divisor, divisor_limbs);

		// If divisor = 0, return false for error
		if (!divisor_used)
			return false;

		// Find dividend MSB limb
		uint32_t dividend_used = LimbDegree(dividend, dividend_limbs);

		// If dividend < divisor, quotient = 0 and remainder = dividend
		if (Less(dividend, dividend_used, divisor, divisor_used))
		{
			Set(remainder, divisor_limbs, dividend, dividend_used);
			Set32(quotient, dividend_limbs, 0);
			return true;
		}

		// If divisor is just one word, use the faster Divide32() algorithm
		if (divisor_used == 1)
		{
			Set(quotient, dividend_limbs, dividend, dividend_used);
			Set32(remainder, divisor_limbs, Divide32(dividend_used, quotient, divisor[0]));
			return true;
		}

		// Find divisor shift to fill MSB
		uint32_t divisor_shift = 32 - Degree(divisor[divisor_used-1]);

		// Shift dividend by divisor_shift
		uint32_t sd_used = dividend_used + 1;
		uint32_t *shifted_dividend = (uint32_t*)alloca(sd_used*4);
		BitShiftLeft(shifted_dividend, sd_used, dividend, dividend_used, divisor_shift);

		// Find dividend shift to fill MSB
		uint32_t dividend_shift;
		uint32_t dividend_msl = shifted_dividend[sd_used-1];
		if (dividend_msl)
			dividend_shift = 32 - Degree(dividend_msl);
		else
		{
			--sd_used;
			dividend_shift = 32 - Degree(shifted_dividend[sd_used-1]);
		}

		// Shift dividend to fill MSB
		BitShiftLeft(shifted_dividend, sd_used, shifted_dividend, sd_used, dividend_shift);

		// Shift divisor to fill MSB
		uint32_t *shifted_divisor = (uint32_t*)alloca(divisor_used*4);
		BitShiftLeft(shifted_divisor, divisor_used, divisor, divisor_used, divisor_shift);

		// shifted_quotient = 0
		uint32_t *shifted_quotient = (uint32_t*)alloca(sd_used*4);
		Set32(shifted_quotient, sd_used, 0);

		// If they are the same bitlength or dividend length < divisor length, quotient = 1 or 0
		if (sd_used <= divisor_used)
		{
			// if dividend < divisor, shifted_quotient = 0 and shifted_dividend = shifted_dividend
			// if dividend >= divisor,
			if (GreaterOrEqual(shifted_dividend, sd_used, shifted_divisor, divisor_used))
			{
				// shifted_quotient = 1
				shifted_quotient[0] = 1;

				// shifted_dividend -= shifted_divisor
				Subtract(shifted_dividend, sd_used, shifted_divisor, divisor_used);
			}
		}
		else
		{
			// Produce quotient and remainder one limb at a time
			uint32_t digit_limbs = divisor_used + 1;
			uint32_t *trial = (uint32_t*)alloca(digit_limbs*4);

			for (uint32_t ii = sd_used; ii > divisor_used; --ii)
			{
				// Digit of dividend to trial divide
				uint32_t *digit = shifted_dividend + ii - digit_limbs;

				// If high part of digit is greater than divisor,
				if (GreaterOrEqual(digit+1, divisor_used, shifted_divisor, divisor_used))
				{
					// Subtract it out and increment quotient for next highest limb
					Subtract(digit+1, divisor_used, shifted_divisor, divisor_used);
					shifted_quotient[ii-divisor_used]++;
				}

				// Trial divide high two of digit by high one of divisor
				uint64_t q64 = *(uint64_t*)(digit + divisor_used - 1) / shifted_divisor[divisor_used-1];
				uint32_t q = (uint32_t)q64;

				// Multiply the trial out to check the approximation
				trial[divisor_used] = Multiply32(divisor_used, trial, shifted_divisor, q);

				// Can be off by at most two, so fix it up if needed
				if (Greater(trial, digit_limbs, digit, digit_limbs))
				{
					--q;
					Subtract(trial, digit_limbs, shifted_divisor, divisor_used);

					if (Greater(trial, digit_limbs, digit, digit_limbs))
					{
						--q;
						Subtract(trial, digit_limbs, shifted_divisor, divisor_used);
					}
				}

				// Subtract the digit by trial to record remainder in place of digit
				Subtract(digit, digit_limbs, trial, digit_limbs);

				// Record quotient for this limb
				shifted_quotient[ii-digit_limbs] = q;
			}
		}

		// Remainder we calculated is (2^dividend_shift * dividend) (Mod divisor) instead of (dividend) (Mod divisor)
		if (!dividend_shift)
		{
			// When dividend_shift = 0, the result is easily fixable
			BitShiftRight(remainder, divisor_limbs, shifted_dividend, divisor_used, divisor_shift);

			// Return quotient
			Set(quotient, dividend_limbs, shifted_quotient, sd_used);
		}
		else
		{
			// If there was a dividend shift we need to do a trial multiply to recover the remainder =(

			// Copy only the limbs we need to compute the remainder
			Set(remainder, divisor_limbs, dividend, dividend_used > divisor_used ? divisor_used : dividend_used);

			// Shift quotient back and return it
			BitShiftRight(quotient, dividend_limbs, shifted_quotient, sd_used, dividend_shift);

			// Calculate remainder = dividend - quotient * divisor, considering just the low limbs
			HalfSchoolbookMultiply(shifted_divisor, quotient, dividend_used, divisor, divisor_used);
			Subtract(remainder, divisor_used, shifted_divisor, divisor_used);
		}

		return true;
	}

	// Convert bigint to string
	/*
	std::string ToStr(const uint32_t *n, int limbs, int base)
	{
		limbs = LimbDegree(n, limbs);
		if (!limbs) return "0";

		std::string out;
		char ch;

		uint32_t *m = (uint32_t*)alloca(limbs*4);
		Set(m, limbs, n, limbs);

		while (limbs)
		{
			uint32_t mod = Divide32(limbs, m, base);
			if (mod <= 9) ch = '0' + mod;
			else ch = 'A' + mod - 10;
			out = ch + out;
			limbs = LimbDegree(m, limbs);
		}

		return out;
	}
	*/

	// Convert string to bigint
	// Return 0 if string contains non-digit characters, else number of limbs used
	int ToInt(uint32_t *lhs, int max_limbs, const char *rhs, uint32_t base)
	{
		if (max_limbs < 2) return 0;

		lhs[0] = 0;
		int used = 1;

		char ch;
		while ((ch = *rhs++))
		{
			uint32_t mod;
			if (ch >= '0' && ch <= '9') mod = ch - '0';
			else mod = toupper(ch) - 'A' + 10;
			if (mod >= base) return 0;

			// lhs *= base
			uint32_t carry = MultiplyAdd32(used, lhs, base, mod);

			// react to running out of room
			if (carry)
			{
				if (used >= max_limbs)
					return 0;

				lhs[used++] = carry;
			}
		}

		if (used < max_limbs)
			Set32(lhs+used, max_limbs-used, 0);

		return used;
	}

	/*
	 * Computes: result = GCD(a, b)  (greatest common divisor)
	 *
	 * Length of result is the length of the smallest argument
	 */
	void GCD(
		const uint32_t *a,	//	Large number, buffer size = a_limbs
		int a_limbs,	//	Size of a
		const uint32_t *b,	//	Large number, buffer size = b_limbs
		int b_limbs,	//	Size of b
		uint32_t *result)	//	Large number, buffer size = min(a, b)
	{
		int limbs = (a_limbs <= b_limbs) ? a_limbs : b_limbs;

		uint32_t *g = (uint32_t*)alloca(limbs*4);
		uint32_t *g1 = (uint32_t*)alloca(limbs*4);
		uint32_t *q = (uint32_t*)alloca((a_limbs+b_limbs)*4);

		if (a_limbs <= b_limbs)
		{
			// g = a, g1 = b (mod a)
			Set(g, limbs, a, a_limbs);
			SchoolbookDivide(b, b_limbs, a, a_limbs, q/*ignore*/, g1);
		}
		else
		{
			// g = b, g1 = a (mod b)
			Set(g, limbs, b, b_limbs);
			SchoolbookDivide(a, a_limbs, b, b_limbs, q/*ignore*/, g1);
		}

		for (;;) {
			// g = (g mod g1)
			SchoolbookDivide(g, limbs, g1, limbs, q/*ignore*/, g);

			if (!LimbDegree(g, limbs)) {
				Set(result, limbs, g1, limbs);
				return;
			}

			// g1 = (g1 mod g)
			SchoolbookDivide(g1, limbs, g, limbs, q/*ignore*/, g1);

			if (!LimbDegree(g1, limbs)) {
				Set(result, limbs, g, limbs);
				return;
			}
		}
	}

	/*
	 * Computes: result = (n ^ -1) (Mod modulus)
	 * Such that: result * n (Mod modulus) = 1
	 * Using Extended Euclid's Algorithm (GCDe)
	 *
	 * This is not always possible, so it will return false iff not possible.
	 */
	bool InvMod(
		const uint32_t *n,		//	Large number, buffer size = n_limbs
		int n_limbs,		//	Size of n
		const uint32_t *modulus,	//	Large number, buffer size = limbs
		int limbs,			//	Size of modulus
		uint32_t *result)		//	Large number, buffer size = limbs
	{
		uint32_t *u = (uint32_t*)alloca(limbs*4);
		uint32_t *u1 = (uint32_t*)alloca(limbs*4);
		uint32_t *g = (uint32_t*)alloca(limbs*4);
		uint32_t *g1 = (uint32_t*)alloca(limbs*4);
		uint32_t *q = (uint32_t*)alloca(limbs*4);
		uint32_t *p = (uint32_t*)alloca((n_limbs+limbs*2)*4);

		SchoolbookDivide(n, n_limbs, modulus, limbs, p/*ignore*/, g);
		Set(g1, limbs, modulus, limbs);

		Set32(u, limbs, 1);
		Set32(u1, limbs, 0);

		for (;;) {
			if (Equal32(g, limbs, 1)) {
				Set(result, limbs, u, limbs);
				return true;
			}
			if (!LimbDegree(g, limbs)) return false;

			// {q, g} = g / g1
			SchoolbookDivide(g, limbs, g1, limbs, q, g);

			// p = q * u1
			Multiply(limbs, p, q, u1);

			// q = p (Mod modulus)
			SchoolbookDivide(p, limbs*2, modulus, limbs, p/*ignore*/, q);

			if (Less(u, limbs, q, limbs))
				Add(u, limbs, modulus, limbs);
			Subtract(u, limbs, q, limbs);

			if (Equal32(g1, limbs, 1)) {
				Set(result, limbs, u1, limbs);
				return true;
			}
			if (!LimbDegree(g1, limbs)) return false;

			// {q, g1} = g1 / g
			SchoolbookDivide(g1, limbs, g, limbs, q, g1);

			// p = q * u
			Multiply(limbs, p, q, u);

			// q = p (Mod modulus)
			SchoolbookDivide(p, limbs*2, modulus, limbs, p/*ignore*/, q);

			if (Less(u1, limbs, q, limbs))
				Add(u1, limbs, modulus, limbs);
			Subtract(u1, limbs, q, limbs);
		}
	}

	// Calculates mod_inv from low limb of modulus
	uint32_t MonModInv(uint32_t modulus0)
	{
		// mod_inv = -M ^ -1 (Mod 2^32)
		return MulInverseGF32(-(int32_t)modulus0);
	}

	// Compute n_residue for Montgomery reduction
	void MonInputResidue(
		const uint32_t *n,		//	Large number, buffer size = n_limbs
		int n_limbs,		//	Number of limbs in n
		const uint32_t *modulus,	//	Large number, buffer size = m_limbs
		int m_limbs,		//	Number of limbs in modulus
		uint32_t *n_residue)		//	Result, buffer size = m_limbs
	{
		// p = n * 2^(k*m)
		uint32_t *p = (uint32_t*)alloca((n_limbs+m_limbs)*4);
		Set32(p, m_limbs, 0);
		Set(p+m_limbs, n_limbs, n, n_limbs);

		// n_residue = p (Mod modulus)
		SchoolbookDivide(p, n_limbs+m_limbs, modulus, m_limbs, p, n_residue);
	}

	// result = a * b * r^-1 (Mod modulus) in Montgomery domain
	void MonPro(
		int limbs,				// Number of limbs in each parameter
		const uint32_t *a_residue,	// Large number, buffer size = limbs
		const uint32_t *b_residue,	// Large number, buffer size = limbs
		const uint32_t *modulus,		// Large number, buffer size = limbs
		uint32_t mod_inv,			// MonModInv() return
		uint32_t *result)			// Large number, buffer size = limbs
	{
		uint32_t *t = (uint32_t*)alloca(limbs*2*4);

		Multiply(limbs, t, a_residue, b_residue);
		MonReduce(limbs, t, modulus, mod_inv, result);
	}

#if 0 // Unused -- keeping it around for novelty purposes
	// result = a * b * r^-1 (Mod modulus) in Montgomery domain
	void CIOSMonPro(
		int limbs,				// Number of limbs in each parameter
		const u32 *a_residue,	// Large number, buffer size = limbs
		const u32 *b_residue,	// Large number, buffer size = limbs
		const u32 *modulus,		// Large number, buffer size = limbs
		u32 mod_inv,			// MonModInv() return
		u32 *result)			// Large number, buffer size = limbs
	{
		// s = 0
		u32 *s = (u32*)alloca(limbs*4);
		memset(s, 0, limbs*4);
		u32 s_high = 0;

		for (int ii = 0; ii < limbs; ++ii)
		{
			// bi = b_residue[ii]
			u32 bi = b_residue[ii];

			// s += a * b[ii]
			u64 p = a_residue[0] * (u64)bi + s[0];

			// qi = s * mod_inv (Mod 2^32)
			u32 qi = (u32)p * mod_inv;

			// s += modulus * qi
			u64 q = modulus[0] * (u64)qi + (u32)p;
			s[0] = (u32)q;

			for (int jj = 1; jj < limbs; ++jj)
			{
				p = a_residue[jj] * (u64)bi + s[jj] + (u32)(p >> 32);

				q = modulus[jj] * (u64)qi + (u32)p + (u32)(q >> 32);
				s[jj - 1] = (u32)q;
			}

			p = (u64)s_high + (u32)(p >> 32) + (u32)(q >> 32);
			s[limbs - 1] = (u32)p;
			s_high = (u32)(p >> 32);
		}

		s64 r = 0;
		for (int ii = 0; ii < limbs; ++ii)
		{
			r >>= 32;
			r += (s64)s[ii] - modulus[ii];
			result[ii] = (u32)r;
		}

		r >>= 32;
		r += (s64)s_high;

		if ((s32)(r >> 32))
			memcpy(result, s, limbs*4);
	}
#endif

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	// The result may be greater than the modulus, but this is okay since
	// the result is still in the RNS.  MonFinish() corrects this at the end.
	void MonReduce(
		int limbs,			// Number of limbs in modulus
		uint32_t *s,				// Large number, buffer size = limbs*2, gets clobbered
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv,		// MonModInv() return
		uint32_t *result)		// Large number, buffer size = limbs
	{
		// This function is roughly 60% of the cost of exponentiation
		for (int ii = 0; ii < limbs; ++ii)
		{
			uint32_t q = s[0] * mod_inv;
			s[0] = AddMultiply32(limbs, s, modulus, q);
			++s;
		}

		// Add the saved carries
		if (Add(result, s, limbs, s - limbs, limbs))
		{
			// Reduce the result only when needed
			Subtract(result, limbs, modulus, limbs);
		}
	}

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	void MonFinish(
		int limbs,			// Number of limbs in each parameter
		uint32_t *n,				// Large number, buffer size = limbs
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv)		// MonModInv() return
	{
		uint32_t *t = (uint32_t*)alloca(limbs*2*4);
		memcpy(t, n, limbs*4);
		memset(t + limbs, 0, limbs*4);

		// Reduce the number
		MonReduce(limbs, t, modulus, mod_inv, n);

		// Fix MonReduce() results greater than the modulus
		if (!Less(limbs, n, modulus))
			Subtract(n, limbs, modulus, limbs);
	}

	// Simple internal version without windowing for small exponents
	static void SimpleMonExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = mod_limbs
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonModInv() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		bool set = false;

		// Run down exponent bits and use the squaring method
		for (int ii = exponent_limbs - 1; ii >= 0; --ii)
		{
			uint32_t e_i = exponent[ii];

			for (uint32_t mask = 0x80000000; mask; mask >>= 1)
			{
				if (set)
				{
					// result = result^2
					MonPro(mod_limbs, result, result, modulus, mod_inv, result);

					if (e_i & mask)
					{
						// result *= base
						MonPro(mod_limbs, result, base, modulus, mod_inv, result);
					}
				}
				else
				{
					if (e_i & mask)
					{
						// result = base
						Set(result, mod_limbs, base, mod_limbs);
						set = true;
					}
				}
			}
		}
	}

	// Precompute a window for ExpMod() and MonExpMod()
	// Requires 2^window_bits multiplies
	uint32_t *PrecomputeWindow(const uint32_t *base, const uint32_t *modulus, int limbs, uint32_t mod_inv, int window_bits)
	{
		uint32_t *temp = (uint32_t*)alloca(limbs*2*4);

		uint32_t *base_squared = (uint32_t*)alloca(limbs*4);
		Square(limbs, temp, base);
		MonReduce(limbs, temp, modulus, mod_inv, base_squared);

		// precomputed window starts with 000001, 000011, 000101, 000111, ...
		uint32_t k = (1 << (window_bits - 1));

		uint32_t *window = RakNet::OP_NEW_ARRAY<uint32_t>(limbs * k, __FILE__, __LINE__ );

		uint32_t *cw = window;
		Set(window, limbs, base);

		while (--k)
		{
			// cw+1 = cw * base^2
			Multiply(limbs, temp, cw, base_squared);
			MonReduce(limbs, temp, modulus, mod_inv, cw + limbs);
			cw += limbs;
		}

		return window;
	};

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	// Base parameter must be a Montgomery Residue created with MonInputResidue()
	void MonExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = mod_limbs
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonModInv() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		// Calculate the number of window bits to use (decent approximation..)
		int window_bits = Degree(exponent_limbs);

		// If the window bits are too small, might as well just use left-to-right S&M method
		if (window_bits < 4)
		{
			SimpleMonExpMod(base, exponent, exponent_limbs, modulus, mod_limbs, mod_inv, result);
			return;
		}

		// Precompute a window of the size determined above
		uint32_t *window = PrecomputeWindow(base, modulus, mod_limbs, mod_inv, window_bits);

		bool seen_bits = false;
		uint32_t e_bits=0, trailing_zeroes=0, used_bits = 0;

		uint32_t *temp = (uint32_t*)alloca((mod_limbs*2)*4);

		for (int ii = exponent_limbs - 1; ii >= 0; --ii)
		{
			uint32_t e_i = exponent[ii];

			int wordbits = 32;
			while (wordbits--)
			{
				// If we have been accumulating bits,
				if (used_bits)
				{
					// If this new bit is set,
					if (e_i >> 31)
					{
						e_bits <<= 1;
						e_bits |= 1;

						trailing_zeroes = 0;
					}
					else // the new bit is unset
					{
						e_bits <<= 1;

						++trailing_zeroes;
					}

					++used_bits;

					// If we have used up the window bits,
					if (used_bits == (uint32_t) window_bits)
					{
						// Select window index 1011 from "101110"
						uint32_t window_index = e_bits >> (trailing_zeroes + 1);

						if (seen_bits)
						{
							uint32_t ctr = used_bits - trailing_zeroes;
							while (ctr--)
							{
								// result = result^2
								Square(mod_limbs, temp, result);
								MonReduce(mod_limbs, temp, modulus, mod_inv, result);
							}

							// result = result * window[index]
							Multiply(mod_limbs, temp, result, &window[window_index * mod_limbs]);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}
						else
						{
							// result = window[index]
							Set(result, mod_limbs, &window[window_index * mod_limbs]);
							seen_bits = true;
						}

						while (trailing_zeroes--)
						{
							// result = result^2
							Square(mod_limbs, temp, result);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}

						used_bits = 0;
					}
				}
				else
				{
					// If this new bit is set,
					if (e_i >> 31)
					{
						used_bits = 1;
						e_bits = 1;
						trailing_zeroes = 0;
					}
					else // the new bit is unset
					{
						// If we have processed any bits yet,
						if (seen_bits)
						{
							// result = result^2
							Square(mod_limbs, temp, result);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}
					}
				}

				e_i <<= 1;
			}
		}

		if (used_bits)
		{
			// Select window index 1011 from "101110"
			uint32_t window_index = e_bits >> (trailing_zeroes + 1);

			if (seen_bits)
			{
				uint32_t ctr = used_bits - trailing_zeroes;
				while (ctr--)
				{
					// result = result^2
					Square(mod_limbs, temp, result);
					MonReduce(mod_limbs, temp, modulus, mod_inv, result);
				}

				// result = result * window[index]
				Multiply(mod_limbs, temp, result, &window[window_index * mod_limbs]);
				MonReduce(mod_limbs, temp, modulus, mod_inv, result);
			}
			else
			{
				// result = window[index]
				Set(result, mod_limbs, &window[window_index * mod_limbs]);
				//seen_bits = true;
			}

			while (trailing_zeroes--)
			{
				// result = result^2
				Square(mod_limbs, temp, result);
				MonReduce(mod_limbs, temp, modulus, mod_inv, result);
			}

			//e_bits = 0;
		}

		RakNet::OP_DELETE_ARRAY(window, __FILE__, __LINE__);
	}

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	void ExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = base_limbs
		int base_limbs,		//	Number of limbs in base
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonModInv() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		uint32_t *mon_base = (uint32_t*)alloca(mod_limbs*4);
		MonInputResidue(base, base_limbs, modulus, mod_limbs, mon_base);

		MonExpMod(mon_base, exponent, exponent_limbs, modulus, mod_limbs, mod_inv, result);

		MonFinish(mod_limbs, result, modulus, mod_inv);
	}

	// Computes: result = base ^ exponent (Mod modulus=mod_p*mod_q)
	// Using Montgomery multiplication with Chinese Remainder Theorem
	void ExpCRT(
		const uint32_t *base,	//	Base for exponentiation, buffer size = base_limbs
		int base_limbs,		//	Number of limbs in base
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *mod_p,	//	Large number, factorization of modulus, buffer size = p_limbs
		uint32_t p_inv,			//	MonModInv() return
		const uint32_t *mod_q,	//	Large number, factorization of modulus, buffer size = q_limbs
		uint32_t q_inv,			//	MonModInv() return
		const uint32_t *pinvq,	//	Large number, InvMod(p, q) precalculated, buffer size = phi_limbs
		int mod_limbs,		//	Number of limbs in p, q and phi
		uint32_t *result)		//	Result, buffer size = mod_limbs*2
	{
		// p = c ^ d (Mod mod_p)
		uint32_t *p = (uint32_t*)alloca(mod_limbs*4);
		ExpMod(base, base_limbs, exponent, exponent_limbs, mod_p, mod_limbs, p_inv, p);

		// q = c ^ d (Mod mod_q)
		uint32_t *q = (uint32_t*)alloca(mod_limbs*4);
		ExpMod(base, base_limbs, exponent, exponent_limbs, mod_q, mod_limbs, q_inv, q);

		uint32_t *product = (uint32_t*)alloca((mod_limbs*2)*4);
		uint32_t *r = (uint32_t*)alloca(mod_limbs*4);

		// result = ((q - p) * (mod_p^-1) mod mod_q) * mod_p + p
		if (Greater(p, mod_limbs, q, mod_limbs))
		{
			// r = p - q (result is positive)
			Set(r, mod_limbs, p, mod_limbs);
			Subtract(r, mod_limbs, q, mod_limbs);

			// product = r * pinvq
			Multiply(mod_limbs, product, r, pinvq);

			// q = product (Mod mod_q)
			SchoolbookDivide(product, mod_limbs*2, mod_q, mod_limbs, product/*ignore*/, q);

			// r = mod_q - q
			Set(r, mod_limbs, mod_q, mod_limbs);
			Subtract(r, mod_limbs, q, mod_limbs);
		}
		else
		{
			// q -= p (result is non-negative)
			Subtract(q, mod_limbs, p, mod_limbs);

			// product = q * pinvq
			Multiply(mod_limbs, product, q, pinvq);

			// mod_q = product (Mod mod_q)
			SchoolbookDivide(product, mod_limbs*2, mod_q, mod_limbs, product/*ignore*/, r);
		}

		// result = r * mod_p
		Multiply(mod_limbs, result, r, mod_p);

		// result += p
		Add(result, mod_limbs*2, p, mod_limbs);
	}
}

#endif
