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
#include <boost/math/concepts/distributions.hpp>
#include <boost/math/distributions/binomial.hpp>

extern "C" {

#include "blitz.h"

	BBObject * _bah_mathtoolkit_TOverflowException__create();


	double bmx_boost_math_factorial(unsigned int i);
	double bmx_boost_math_uncheckedfactorial(unsigned int i);
	unsigned int bmx_boost_math_maxfactorial();
	double bmx_boost_math_doublefactorial(unsigned int i);
	double bmx_boost_math_risingfactorial(double x, unsigned int i);
	double bmx_boost_math_fallingfactorial(double x, unsigned int i);
	double bmx_boost_math_binomialcoefficient(unsigned int n, unsigned int k);

	double bmx_boost_math_cbrt(double x);
	double bmx_boost_math_hypot(double x, double y);

	boost::math::binomial_distribution<double> * bmx_boost_math_binomial_distribution_create(int n, double p);
	double bmx_boost_math_binomial_distribution_successfraction(boost::math::binomial_distribution<double> * dist);
	int bmx_boost_math_binomial_distribution_trials(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_mean(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_mode(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_standarddeviation(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_skewness(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_pdf(boost::math::binomial_distribution<double> * dist, double k);
	double bmx_boost_math_binomial_distribution_cdf(boost::math::binomial_distribution<double> * dist, double k);

}


void bmx_throw_overflow_exception() {
	bbExThrow(_bah_mathtoolkit_TOverflowException__create());
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

// ************************************************

boost::math::binomial_distribution<double> * bmx_boost_math_binomial_distribution_create(int n, double p) {
	return new boost::math::binomial_distribution<double>(n, p);
}

double bmx_boost_math_binomial_distribution_successfraction(boost::math::binomial_distribution<double> * dist) {
	return dist->success_fraction();
}

int bmx_boost_math_binomial_distribution_trials(boost::math::binomial_distribution<double> * dist) {
	return dist->trials();
}

double bmx_boost_math_binomial_distribution_mean(boost::math::binomial_distribution<double> * dist) {
	return mean(*dist);
}

double bmx_boost_math_binomial_distribution_mode(boost::math::binomial_distribution<double> * dist) {
	return mode(*dist);
}

double bmx_boost_math_binomial_distribution_standarddeviation(boost::math::binomial_distribution<double> * dist) {
	return standard_deviation(*dist);
}

double bmx_boost_math_binomial_distribution_skewness(boost::math::binomial_distribution<double> * dist) {
	return skewness(*dist);
}

double bmx_boost_math_binomial_distribution_pdf(boost::math::binomial_distribution<double> * dist, double k) {
	return pdf(*dist, k);
}

double bmx_boost_math_binomial_distribution_cdf(boost::math::binomial_distribution<double> * dist, double k) {
	return cdf(*dist, k);
}

