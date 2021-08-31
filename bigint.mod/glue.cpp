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

#include "glue.h"

MaxBigUnsigned::MaxBigUnsigned(BigUnsigned & b)
	: big(b)
{
}

MaxBigUnsigned::MaxBigUnsigned(const std::string & b)
	: big(stringToBigUnsigned(b))
{
}

MaxBigUnsigned::MaxBigUnsigned(int value)
	: big(value)
{
}

BigUnsigned & MaxBigUnsigned::Big() {
	return big;
}





MaxBigInteger::MaxBigInteger(BigInteger & b)
	: big(b)
{
}

MaxBigInteger::MaxBigInteger(const std::string & b)
	: big(stringToBigInteger(b))
{
}

MaxBigInteger::MaxBigInteger(int value)
	: big(value)
{
}

BigInteger & MaxBigInteger::Big() {
	return big;
}


// --------------------------------------------------------

MaxBigUnsigned * bmx_bigint_BigUnsigned_create(BBString * value) {
	char * p = (char*)bbStringToCString(value);
	try {
		MaxBigUnsigned * big = new MaxBigUnsigned(p);
		bbMemFree(p);
		return big;
	} catch(char const* err) {
		bbMemFree(p);
		bbExThrow((BBObject*)bbStringFromCString(err));
	}
}

MaxBigUnsigned * bmx_bigint_BigUnsigned_CreateWithInt(int value) {
	try {
		return new MaxBigUnsigned(value);
	} catch(char const* err) {
		bbExThrow((BBObject*)bbStringFromCString(err));
	}
}

void bmx_bigint_BigUnsigned_free(MaxBigUnsigned * big) {
	delete big;
}

BBString * bmx_bigint_BigUnsigned_ToString(MaxBigUnsigned * big) {
	return bbStringFromCString(bigUnsignedToString(big->Big()).c_str());
}


void bmx_bigint_BigUnsigned_add(MaxBigUnsigned * target, MaxBigUnsigned * value) {
	target->Big().add(target->Big(), value->Big());
}
void bmx_bigint_BigUnsigned_subtract(MaxBigUnsigned * target, MaxBigUnsigned * value) {
	target->Big().subtract(target->Big(), value->Big());
}

void bmx_bigint_BigUnsigned_divide(MaxBigUnsigned * target, MaxBigUnsigned * value) {
	target->Big().divideWithRemainder(target->Big(), value->Big());
}




// --------------------------------------------------------

MaxBigInteger * bmx_bigint_BigInteger_create(BBString * value) {
	char * p = (char*)bbStringToCString(value);
	try {
		MaxBigInteger * big = new MaxBigInteger(p);
		bbMemFree(p);
		return big;
	} catch(char const* err) {
		bbMemFree(p);
		bbExThrow((BBObject*)bbStringFromCString(err));
	}
}

MaxBigInteger * bmx_bigint_BigInteger_CreateWithInt(int value) {
	try {
		return new MaxBigInteger(value);
	} catch(char const* err) {
		bbExThrow((BBObject*)bbStringFromCString(err));
	}
}

void bmx_bigint_BigInteger_free(MaxBigInteger * big) {
	delete big;
}

BBString * bmx_bigint_BigInteger_ToString(MaxBigInteger * big) {
	return bbStringFromCString(bigIntegerToString(big->Big()).c_str());
}


int bmx_bigint_BigInteger_equal(MaxBigInteger * a, MaxBigInteger * b) {
	if(a->Big() == b->Big())
		return 1;
	return 0;
}

int bmx_bigint_BigInteger_notequal(MaxBigInteger * a, MaxBigInteger * b) {
	if(a->Big() != b->Big())
		return 1;
	return 0;
}

int bmx_bigint_BigInteger_lessThan(MaxBigInteger * a, MaxBigInteger * b) {
	if (a->Big() < b->Big())
		return 1;
	return 0;
}

int bmx_bigint_BigInteger_lessThanOrEqual(MaxBigInteger * a, MaxBigInteger * b) {
	if (a->Big() <= b->Big())
		return 1;
	return 0;
}

int bmx_bigint_BigInteger_greaterThan(MaxBigInteger * a, MaxBigInteger * b) {
	if(a->Big() > b->Big())
		return 1;
	return 0;
}

int bmx_bigint_BigInteger_greaterThanOrEqual(MaxBigInteger * a, MaxBigInteger * b) {
	if(a->Big() >= b->Big())
		return 1;
	return 0;
}


void bmx_bigint_BigInteger_add(MaxBigInteger * target, MaxBigInteger * value) {
//	target->Big().add(target->Big(), value->Big());
	target->Big() += value->Big();
}

void bmx_bigint_BigInteger_subtract(MaxBigInteger * target, MaxBigInteger * value) {
//	target->Big().subtract(target->Big(), value->Big());
	target->Big() -= value->Big();
}

void bmx_bigint_BigInteger_modulo(MaxBigInteger * target, MaxBigInteger * value) {
//	MaxBigInteger * quotient = new MaxBigInteger(0);
//	target->Big().divideWithRemainder(value->Big(), quotient->Big());
//	delete quotient;
	target->Big() %= value->Big();
}

void bmx_bigint_BigInteger_divide(MaxBigInteger * target, MaxBigInteger * value) {
	target->Big() /= value->Big();
}

void bmx_bigint_BigInteger_multiply(MaxBigInteger * target, MaxBigInteger * value) {
//	target->Big().multiply(target->Big(), value->Big());
	target->Big() *= value->Big();
}



MaxBigInteger * bmx_bigint_new_BigInteger_add(MaxBigInteger * target, MaxBigInteger * value) {
	MaxBigInteger * result = new MaxBigInteger(target->Big());
	result->Big() += value->Big();
	return result;
}

MaxBigInteger * bmx_bigint_new_BigInteger_subtract(MaxBigInteger * target, MaxBigInteger * value) {
	MaxBigInteger * result = new MaxBigInteger(target->Big());
	result->Big() -= value->Big();
	return result;
}

MaxBigInteger * bmx_bigint_new_BigInteger_modulo(MaxBigInteger * target, MaxBigInteger * value) {
	MaxBigInteger * result = new MaxBigInteger(target->Big());
	result->Big() %= value->Big();
	return result;
}

MaxBigInteger * bmx_bigint_new_BigInteger_divide(MaxBigInteger * target, MaxBigInteger * value) {
	MaxBigInteger * result = new MaxBigInteger(target->Big());
	result->Big() /= value->Big();
	return result;
}

MaxBigInteger * bmx_bigint_new_BigInteger_multiply(MaxBigInteger * target, MaxBigInteger * value) {
	MaxBigInteger * result = new MaxBigInteger(target->Big());
	result->Big() *= value->Big();
	return result;
}
