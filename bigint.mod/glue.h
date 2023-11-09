/*
  Copyright (c) 2013 Bruce A Henderson
 
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

#include "BigInteger.hh"
#include "BigIntegerUtils.hh"

class MaxBigUnsigned;
class MaxBigInteger;

extern "C" {

#include "blitz.h"

	MaxBigUnsigned * bmx_bigint_BigUnsigned_create(BBString * value);
	MaxBigUnsigned * bmx_bigint_BigUnsigned_CreateWithInt(int value);
	void bmx_bigint_BigUnsigned_free(MaxBigUnsigned * big);
	BBString * bmx_bigint_BigUnsigned_ToString(MaxBigUnsigned * big);

	void bmx_bigint_BigUnsigned_add(MaxBigUnsigned * target, MaxBigUnsigned * value);
	void bmx_bigint_BigUnsigned_divide(MaxBigUnsigned * target, MaxBigUnsigned * value);




	MaxBigInteger * bmx_bigint_BigInteger_create(BBString * value);
	MaxBigInteger * bmx_bigint_BigInteger_CreateWithInt(int value);
	void bmx_bigint_BigInteger_free(MaxBigInteger * big);
	BBString * bmx_bigint_BigInteger_ToString(MaxBigInteger * big);

	void bmx_bigint_BigInteger_add(MaxBigInteger * target, MaxBigInteger * value);
	void bmx_bigint_BigInteger_subtract(MaxBigInteger * target, MaxBigInteger * value);
	void bmx_bigint_BigInteger_divide(MaxBigInteger * target, MaxBigInteger * value);
	void bmx_bigint_BigInteger_multiply(MaxBigInteger * target, MaxBigInteger * value);
	void bmx_bigint_BigInteger_modulo(MaxBigInteger * target, MaxBigInteger * value);

	MaxBigInteger * bmx_bigint_new_BigInteger_add(MaxBigInteger * target, MaxBigInteger * value);
	MaxBigInteger * bmx_bigint_new_BigInteger_subtract(MaxBigInteger * target, MaxBigInteger * value);
	MaxBigInteger * bmx_bigint_new_BigInteger_divide(MaxBigInteger * target, MaxBigInteger * value);
	MaxBigInteger * bmx_bigint_new_BigInteger_multiply(MaxBigInteger * target, MaxBigInteger * value);
	MaxBigInteger * bmx_bigint_new_BigInteger_modulo(MaxBigInteger * target, MaxBigInteger * value);

	int bmx_bigint_BigInteger_equal(MaxBigInteger * a, MaxBigInteger * b);
	int bmx_bigint_BigInteger_notequal(MaxBigInteger * a, MaxBigInteger * b);
	int bmx_bigint_BigInteger_lessThan(MaxBigInteger * a, MaxBigInteger * b);
	int bmx_bigint_BigInteger_lessThanOrEqual(MaxBigInteger * a, MaxBigInteger * b);
	int bmx_bigint_BigInteger_greaterThan(MaxBigInteger * a, MaxBigInteger * b);
	int bmx_bigint_BigInteger_greaterThanOrEqual(MaxBigInteger * a, MaxBigInteger * b);
}



class MaxBigUnsigned
{
public:
	MaxBigUnsigned(BigUnsigned & b);
	MaxBigUnsigned(const std::string & b);
	MaxBigUnsigned(int value);
	~MaxBigUnsigned() {};

	BigUnsigned & Big();
private:
	BigUnsigned big;
};


class MaxBigInteger
{
public:
	MaxBigInteger(BigInteger & b);
	MaxBigInteger(const std::string & b);
	MaxBigInteger(int value);
	~MaxBigInteger() {};

	BigInteger & Big();
private:
	BigInteger big;
};