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
#include <boost/math/special_functions/factorials.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <boost/math/special_functions/cbrt.hpp>
#include <boost/math/special_functions/hypot.hpp>

extern "C" {

#include "blitz.h"

	BBObject * _bah_math_TOverflowException__create();


	double bmx_boost_math_factorial(unsigned int i);
	double bmx_boost_math_uncheckedfactorial(unsigned int i);
	unsigned int bmx_boost_math_maxfactorial();
	double bmx_boost_math_doublefactorial(unsigned int i);
	double bmx_boost_math_risingfactorial(double x, unsigned int i);
	double bmx_boost_math_fallingfactorial(double x, unsigned int i);
	double bmx_boost_math_binomialcoefficient(unsigned int n, unsigned int k);

	double bmx_boost_math_cbrt(double x);
	double bmx_boost_math_hypot(double x, double y);

}


void bmx_throw_overflow_exception() {
	bbExThrow(_bah_math_TOverflowException__create());
}



double bmx_boost_math_factorial(unsigned int i) {
	try {
		return boost::math::factorial<double>(i);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

double bmx_boost_math_uncheckedfactorial(unsigned int i) {
	try {
		return boost::math::unchecked_factorial<double>(i);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

unsigned int bmx_boost_math_maxfactorial() {
	return boost::math::max_factorial<double>::value;
}

double bmx_boost_math_doublefactorial(unsigned int i) {
	try {
		return boost::math::double_factorial<double>(i);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

double bmx_boost_math_risingfactorial(double x, unsigned int i) {
	try {
		return boost::math::rising_factorial<double>(x, i);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

double bmx_boost_math_fallingfactorial(double x, unsigned int i) {
	try {
		return boost::math::falling_factorial<double>(x, i);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

double bmx_boost_math_binomialcoefficient(unsigned int n, unsigned int k) {
	try {
		return boost::math::binomial_coefficient<double>(n, k);
	} catch(...) {
		bmx_throw_overflow_exception();
	}
}

double bmx_boost_math_cbrt(double x) {
	return boost::math::cbrt<double>(x);
}

double bmx_boost_math_hypot(double x, double y) {
	return boost::math::hypot<double>(x, y);
}

