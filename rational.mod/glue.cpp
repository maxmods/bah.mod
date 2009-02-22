/*
 Copyright (c) 2009 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <boost/rational.hpp>


extern "C" {

	boost::rational<int> * bmx_rational_create(int n, int d);
	void bmx_rational_delete(boost::rational<int> * rat);

	double bmx_rational_todouble(boost::rational<int> * rat);
	int bmx_rational_numerator(boost::rational<int> * rat);
	int bmx_rational_denominator(boost::rational<int> * rat);
	void bmx_rational_assign(boost::rational<int> * rat, int n, int d);
	void bmx_rational_add(boost::rational<int> * rat, int value);
	void bmx_rational_sub(boost::rational<int> * rat, int value);
	void bmx_rational_mul(boost::rational<int> * rat, int value);
	void bmx_rational_div(boost::rational<int> * rat, int value);
	void bmx_rational_addr(boost::rational<int> * rat, boost::rational<int> * value);
	void bmx_rational_subr(boost::rational<int> * rat, boost::rational<int> * value);
	void bmx_rational_mulr(boost::rational<int> * rat, boost::rational<int> * value);
	void bmx_rational_divr(boost::rational<int> * rat, boost::rational<int> * value);
	bool bmx_rational_equals(boost::rational<int> * rat, int value);
	bool bmx_rational_equalsr(boost::rational<int> * rat, boost::rational<int> * value);
	boost::rational<int> * bmx_rational_plus(boost::rational<int> * rat, int value);
	boost::rational<int> * bmx_rational_subtract(boost::rational<int> * rat, int value);
	boost::rational<int> * bmx_rational_multiply(boost::rational<int> * rat, int value);
	boost::rational<int> * bmx_rational_divide(boost::rational<int> * rat, int value);
	boost::rational<int> * bmx_rational_plusr(boost::rational<int> * rat, boost::rational<int> * value);
	boost::rational<int> * bmx_rational_subtractr(boost::rational<int> * rat, boost::rational<int> * value);
	boost::rational<int> * bmx_rational_multiplyr(boost::rational<int> * rat, boost::rational<int> * value);
	boost::rational<int> * bmx_rational_divider(boost::rational<int> * rat, boost::rational<int> * value);
	boost::rational<int> * bmx_rational_negative(boost::rational<int> * rat);
	boost::rational<int> * bmx_rational_abs(boost::rational<int> * rat);
	bool bmx_rational_lessthan(boost::rational<int> * rat, int value);
	bool bmx_rational_greaterthan(boost::rational<int> * rat, int value);
	bool bmx_rational_lessthanr(boost::rational<int> * rat, boost::rational<int> * value);
	bool bmx_rational_greaterthanr(boost::rational<int> * rat, boost::rational<int> * value);

}


boost::rational<int> * bmx_rational_create(int n, int d) {
	return new boost::rational<int>(n, d);
}

void bmx_rational_delete(boost::rational<int> * rat) {
	delete rat;
}


double bmx_rational_todouble(boost::rational<int> * rat) {
	return boost::rational_cast<double>(*rat);
}

int bmx_rational_numerator(boost::rational<int> * rat) {
	return rat->numerator();
}

int bmx_rational_denominator(boost::rational<int> * rat) {
	return rat->denominator();
}

void bmx_rational_assign(boost::rational<int> * rat, int n, int d) {
	rat->assign(n, d);
}

void bmx_rational_add(boost::rational<int> * rat, int value) {
	*rat += value;
}

void bmx_rational_sub(boost::rational<int> * rat, int value) {
	*rat -= value;
}

void bmx_rational_mul(boost::rational<int> * rat, int value) {
	*rat *= value;
}

void bmx_rational_div(boost::rational<int> * rat, int value) {
	*rat /= value;
}

void bmx_rational_addr(boost::rational<int> * rat, boost::rational<int> * value) {
	*rat += *value;
}

void bmx_rational_subr(boost::rational<int> * rat, boost::rational<int> * value) {
	*rat -= *value;
}

void bmx_rational_mulr(boost::rational<int> * rat, boost::rational<int> * value) {
	*rat *= *value;
}

void bmx_rational_divr(boost::rational<int> * rat, boost::rational<int> * value) {
	*rat /= *value;
}

bool bmx_rational_equals(boost::rational<int> * rat, int value) {
	return *rat == value;
}

bool bmx_rational_equalsr(boost::rational<int> * rat, boost::rational<int> * value) {
	return *rat == *value;
}

boost::rational<int> * bmx_rational_plus(boost::rational<int> * rat, int value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat + value;
	return r;
}

boost::rational<int> * bmx_rational_subtract(boost::rational<int> * rat, int value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat - value;
	return r;
}

boost::rational<int> * bmx_rational_multiply(boost::rational<int> * rat, int value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat * value;
	return r;
}

boost::rational<int> * bmx_rational_divide(boost::rational<int> * rat, int value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat / value;
	return r;
}

boost::rational<int> * bmx_rational_plusr(boost::rational<int> * rat, boost::rational<int> * value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat + *value;
	return r;
}

boost::rational<int> * bmx_rational_subtractr(boost::rational<int> * rat, boost::rational<int> * value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat - *value;
	return r;
}

boost::rational<int> * bmx_rational_multiplyr(boost::rational<int> * rat, boost::rational<int> * value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat * *value;
	return r;
}

boost::rational<int> * bmx_rational_divider(boost::rational<int> * rat, boost::rational<int> * value) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = *rat / *value;
	return r;
}

boost::rational<int> * bmx_rational_negative(boost::rational<int> * rat) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = -*rat;
	return r;
}

boost::rational<int> * bmx_rational_abs(boost::rational<int> * rat) {
	boost::rational<int> * r = new boost::rational<int>();
	*r = abs(*rat);
	return r;
}

bool bmx_rational_lessthan(boost::rational<int> * rat, int value) {
	return *rat < value;
}

bool bmx_rational_greaterthan(boost::rational<int> * rat, int value) {
	return *rat > value;
}

bool bmx_rational_lessthanr(boost::rational<int> * rat, boost::rational<int> * value) {
	return *rat < *value;
}

bool bmx_rational_greaterthanr(boost::rational<int> * rat, boost::rational<int> * value) {
	return *rat > *value;
}







