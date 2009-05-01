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
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/zeta.hpp>
#include <boost/math/concepts/distributions.hpp>
#include <boost/math/distributions/binomial.hpp>
#include <boost/math/distributions/bernoulli.hpp>
#include <boost/math/distributions/beta.hpp>
#include <boost/math/distributions/cauchy.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/distributions/exponential.hpp>
#include <boost/math/distributions/extreme_value.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/pareto.hpp>
#include <boost/math/distributions/weibull.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/lognormal.hpp>
#include <boost/math/distributions/negative_binomial.hpp>
#include <boost/math/distributions/non_central_beta.hpp>
#include <boost/math/distributions/non_central_chi_squared.hpp>
#include <boost/math/distributions/non_central_f.hpp>
#include <boost/math/distributions/non_central_t.hpp>
#include <boost/math/distributions/poisson.hpp>
#include <boost/math/distributions/rayleigh.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/triangular.hpp>
#include <boost/math/distributions/uniform.hpp>
#include <boost/math/distributions/gamma.hpp>

extern "C" {

#include "blitz.h"

	BBObject * _bah_mathtoolkit_TOverflowException__create();
	BBObject * _bah_mathtoolkit_TDomainException__create();


	double bmx_boost_math_factorial(unsigned int i);
	double bmx_boost_math_uncheckedfactorial(unsigned int i);
	unsigned int bmx_boost_math_maxfactorial();
	double bmx_boost_math_doublefactorial(unsigned int i);
	double bmx_boost_math_risingfactorial(double x, unsigned int i);
	double bmx_boost_math_fallingfactorial(double x, unsigned int i);
	double bmx_boost_math_binomialcoefficient(unsigned int n, unsigned int k);

	double bmx_boost_math_cbrt(double x);
	double bmx_boost_math_hypot(double x, double y);
	double bmx_boost_math_log1p(double x);
	double bmx_boost_math_expm1(double x);
	double bmx_boost_math_sqrt1pm1(double x);
	double bmx_boost_math_powm1(double x, double y);
	double bmx_boost_math_zeta(double z);

	double bmx_boost_math_sphbessel(unsigned int v, double x);
	double bmx_boost_math_sphneumann(unsigned int v, double x);

	boost::math::binomial_distribution<double> * bmx_boost_math_binomial_distribution_create(int n, double p);
	double bmx_boost_math_binomial_distribution_successfraction(boost::math::binomial_distribution<double> * dist);
	int bmx_boost_math_binomial_distribution_trials(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_mean(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_mode(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_standarddeviation(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_skewness(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_pdf(boost::math::binomial_distribution<double> * dist, double k);
	double bmx_boost_math_binomial_distribution_cdf(boost::math::binomial_distribution<double> * dist, double k);
	void bmx_boost_math_binomial_distribution_free(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_quantile(boost::math::binomial_distribution<double> * dist, double k);
	double bmx_boost_math_binomial_distribution_cdfcomplement(boost::math::binomial_distribution<double> * dist, double k);
	double bmx_boost_math_binomial_distribution_quantilecomplement(boost::math::binomial_distribution<double> * dist, double k);
	double bmx_boost_math_binomial_distribution_hazard(boost::math::binomial_distribution<double> * dist, double x);
	double bmx_boost_math_binomial_distribution_chf(boost::math::binomial_distribution<double> * dist, double x);
	double bmx_boost_math_binomial_distribution_median(boost::math::binomial_distribution<double> * dist);
	void bmx_boost_math_binomial_distribution_range(boost::math::binomial_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_binomial_distribution_variance(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_kurtosis(boost::math::binomial_distribution<double> * dist);
	double bmx_boost_math_binomial_distribution_kurtosisexcess(boost::math::binomial_distribution<double> * dist);

	boost::math::bernoulli_distribution<double> * bmx_boost_math_bernoulli_distribution_create(double p);
	double bmx_boost_math_bernoulli_distribution_successfraction(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_mean(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_mode(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_standarddeviation(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_skewness(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_pdf(boost::math::bernoulli_distribution<double> * dist, double k);
	double bmx_boost_math_bernoulli_distribution_cdf(boost::math::bernoulli_distribution<double> * dist, double k);
	void bmx_boost_math_bernoulli_distribution_free(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_quantile(boost::math::bernoulli_distribution<double> * dist, double k);
	double bmx_boost_math_bernoulli_distribution_cdfcomplement(boost::math::bernoulli_distribution<double> * dist, double k);
	double bmx_boost_math_bernoulli_distribution_quantilecomplement(boost::math::bernoulli_distribution<double> * dist, double k);
	double bmx_boost_math_bernoulli_distribution_hazard(boost::math::bernoulli_distribution<double> * dist, double x);
	double bmx_boost_math_bernoulli_distribution_chf(boost::math::bernoulli_distribution<double> * dist, double x);
	double bmx_boost_math_bernoulli_distribution_median(boost::math::bernoulli_distribution<double> * dist);
	void bmx_boost_math_bernoulli_distribution_range(boost::math::bernoulli_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_bernoulli_distribution_variance(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_kurtosis(boost::math::bernoulli_distribution<double> * dist);
	double bmx_boost_math_bernoulli_distribution_kurtosisexcess(boost::math::bernoulli_distribution<double> * dist);

	
	boost::math::beta_distribution<double> * bmx_boost_math_beta_distribution_create(double a, double p);
	double bmx_boost_math_beta_distribution_alpha(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_beta(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_mean(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_mode(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_standarddeviation(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_skewness(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_pdf(boost::math::beta_distribution<double> * dist, double k);
	double bmx_boost_math_beta_distribution_cdf(boost::math::beta_distribution<double> * dist, double k);
	void bmx_boost_math_beta_distribution_free(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_quantile(boost::math::beta_distribution<double> * dist, double k);
	double bmx_boost_math_beta_distribution_cdfcomplement(boost::math::beta_distribution<double> * dist, double k);
	double bmx_boost_math_beta_distribution_quantilecomplement(boost::math::beta_distribution<double> * dist, double k);
	double bmx_boost_math_beta_distribution_hazard(boost::math::beta_distribution<double> * dist, double x);
	double bmx_boost_math_beta_distribution_chf(boost::math::beta_distribution<double> * dist, double x);
	double bmx_boost_math_beta_distribution_median(boost::math::beta_distribution<double> * dist);
	void bmx_boost_math_beta_distribution_range(boost::math::beta_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_beta_distribution_variance(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_kurtosis(boost::math::beta_distribution<double> * dist);
	double bmx_boost_math_beta_distribution_kurtosisexcess(boost::math::beta_distribution<double> * dist);

	
	boost::math::cauchy_distribution<double> * bmx_boost_math_cauchy_distribution_create(double location, double scale);
	double bmx_boost_math_cauchy_distribution_location(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_scale(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_mean(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_mode(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_standarddeviation(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_skewness(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_pdf(boost::math::cauchy_distribution<double> * dist, double k);
	double bmx_boost_math_cauchy_distribution_cdf(boost::math::cauchy_distribution<double> * dist, double k);
	void bmx_boost_math_cauchy_distribution_free(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_quantile(boost::math::cauchy_distribution<double> * dist, double k);
	double bmx_boost_math_cauchy_distribution_cdfcomplement(boost::math::cauchy_distribution<double> * dist, double k);
	double bmx_boost_math_cauchy_distribution_quantilecomplement(boost::math::cauchy_distribution<double> * dist, double k);
	double bmx_boost_math_cauchy_distribution_hazard(boost::math::cauchy_distribution<double> * dist, double x);
	double bmx_boost_math_cauchy_distribution_chf(boost::math::cauchy_distribution<double> * dist, double x);
	double bmx_boost_math_cauchy_distribution_median(boost::math::cauchy_distribution<double> * dist);
	void bmx_boost_math_cauchy_distribution_range(boost::math::cauchy_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_cauchy_distribution_variance(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_kurtosis(boost::math::cauchy_distribution<double> * dist);
	double bmx_boost_math_cauchy_distribution_kurtosisexcess(boost::math::cauchy_distribution<double> * dist);

	
	boost::math::chi_squared_distribution<double> * bmx_boost_math_chi_squared_distribution_create(double i);
	double bmx_boost_math_chi_squared_distribution_degreesoffreedom(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_mean(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_mode(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_standarddeviation(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_skewness(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_pdf(boost::math::chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_chi_squared_distribution_cdf(boost::math::chi_squared_distribution<double> * dist, double k);
	void bmx_boost_math_chi_squared_distribution_free(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_quantile(boost::math::chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_chi_squared_distribution_cdfcomplement(boost::math::chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_chi_squared_distribution_quantilecomplement(boost::math::chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_chi_squared_distribution_hazard(boost::math::chi_squared_distribution<double> * dist, double x);
	double bmx_boost_math_chi_squared_distribution_chf(boost::math::chi_squared_distribution<double> * dist, double x);
	double bmx_boost_math_chi_squared_distribution_median(boost::math::chi_squared_distribution<double> * dist);
	void bmx_boost_math_chi_squared_distribution_range(boost::math::chi_squared_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_chi_squared_distribution_variance(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_kurtosis(boost::math::chi_squared_distribution<double> * dist);
	double bmx_boost_math_chi_squared_distribution_kurtosisexcess(boost::math::chi_squared_distribution<double> * dist);

	
	boost::math::exponential_distribution<double> * bmx_boost_math_exponential_distribution_create(double lambda);
	double bmx_boost_math_exponential_distribution_lambda(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_mean(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_mode(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_standarddeviation(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_skewness(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_pdf(boost::math::exponential_distribution<double> * dist, double k);
	double bmx_boost_math_exponential_distribution_cdf(boost::math::exponential_distribution<double> * dist, double k);
	void bmx_boost_math_exponential_distribution_free(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_quantile(boost::math::exponential_distribution<double> * dist, double k);
	double bmx_boost_math_exponential_distribution_cdfcomplement(boost::math::exponential_distribution<double> * dist, double k);
	double bmx_boost_math_exponential_distribution_quantilecomplement(boost::math::exponential_distribution<double> * dist, double k);
	double bmx_boost_math_exponential_distribution_hazard(boost::math::exponential_distribution<double> * dist, double x);
	double bmx_boost_math_exponential_distribution_chf(boost::math::exponential_distribution<double> * dist, double x);
	double bmx_boost_math_exponential_distribution_median(boost::math::exponential_distribution<double> * dist);
	void bmx_boost_math_exponential_distribution_range(boost::math::exponential_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_exponential_distribution_variance(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_kurtosis(boost::math::exponential_distribution<double> * dist);
	double bmx_boost_math_exponential_distribution_kurtosisexcess(boost::math::exponential_distribution<double> * dist);

	
	boost::math::extreme_value_distribution<double> * bmx_boost_math_extreme_value_distribution_create(double location, double scale);
	double bmx_boost_math_extreme_value_distribution_location(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_scale(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_mean(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_mode(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_standarddeviation(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_skewness(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_pdf(boost::math::extreme_value_distribution<double> * dist, double k);
	double bmx_boost_math_extreme_value_distribution_cdf(boost::math::extreme_value_distribution<double> * dist, double k);
	void bmx_boost_math_extreme_value_distribution_free(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_quantile(boost::math::extreme_value_distribution<double> * dist, double k);
	double bmx_boost_math_extreme_value_distribution_cdfcomplement(boost::math::extreme_value_distribution<double> * dist, double k);
	double bmx_boost_math_extreme_value_distribution_quantilecomplement(boost::math::extreme_value_distribution<double> * dist, double k);
	double bmx_boost_math_extreme_value_distribution_hazard(boost::math::extreme_value_distribution<double> * dist, double x);
	double bmx_boost_math_extreme_value_distribution_chf(boost::math::extreme_value_distribution<double> * dist, double x);
	double bmx_boost_math_extreme_value_distribution_median(boost::math::extreme_value_distribution<double> * dist);
	void bmx_boost_math_extreme_value_distribution_range(boost::math::extreme_value_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_extreme_value_distribution_variance(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_kurtosis(boost::math::extreme_value_distribution<double> * dist);
	double bmx_boost_math_extreme_value_distribution_kurtosisexcess(boost::math::extreme_value_distribution<double> * dist);

	
	boost::math::normal_distribution<double> * bmx_boost_math_normal_distribution_create(double mean, double sd);
	double bmx_boost_math_normal_distribution_location(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_scale(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_mean(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_mode(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_standarddeviation(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_skewness(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_pdf(boost::math::normal_distribution<double> * dist, double k);
	double bmx_boost_math_normal_distribution_cdf(boost::math::normal_distribution<double> * dist, double k);
	void bmx_boost_math_normal_distribution_free(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_quantile(boost::math::normal_distribution<double> * dist, double k);
	double bmx_boost_math_normal_distribution_cdfcomplement(boost::math::normal_distribution<double> * dist, double k);
	double bmx_boost_math_normal_distribution_quantilecomplement(boost::math::normal_distribution<double> * dist, double k);
	double bmx_boost_math_normal_distribution_hazard(boost::math::normal_distribution<double> * dist, double x);
	double bmx_boost_math_normal_distribution_chf(boost::math::normal_distribution<double> * dist, double x);
	double bmx_boost_math_normal_distribution_median(boost::math::normal_distribution<double> * dist);
	void bmx_boost_math_normal_distribution_range(boost::math::normal_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_normal_distribution_variance(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_kurtosis(boost::math::normal_distribution<double> * dist);
	double bmx_boost_math_normal_distribution_kurtosisexcess(boost::math::normal_distribution<double> * dist);

	
	boost::math::pareto_distribution<double> * bmx_boost_math_pareto_distribution_create(double location, double shape);
	double bmx_boost_math_pareto_distribution_location(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_shape(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_mean(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_mode(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_standarddeviation(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_skewness(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_pdf(boost::math::pareto_distribution<double> * dist, double k);
	double bmx_boost_math_pareto_distribution_cdf(boost::math::pareto_distribution<double> * dist, double k);
	void bmx_boost_math_pareto_distribution_free(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_quantile(boost::math::pareto_distribution<double> * dist, double k);
	double bmx_boost_math_pareto_distribution_cdfcomplement(boost::math::pareto_distribution<double> * dist, double k);
	double bmx_boost_math_pareto_distribution_quantilecomplement(boost::math::pareto_distribution<double> * dist, double k);
	double bmx_boost_math_pareto_distribution_hazard(boost::math::pareto_distribution<double> * dist, double x);
	double bmx_boost_math_pareto_distribution_chf(boost::math::pareto_distribution<double> * dist, double x);
	double bmx_boost_math_pareto_distribution_median(boost::math::pareto_distribution<double> * dist);
	void bmx_boost_math_pareto_distribution_range(boost::math::pareto_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_pareto_distribution_variance(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_kurtosis(boost::math::pareto_distribution<double> * dist);
	double bmx_boost_math_pareto_distribution_kurtosisexcess(boost::math::pareto_distribution<double> * dist);

	
	boost::math::weibull_distribution<double> * bmx_boost_math_weibull_distribution_create(double shape, double scale);
	double bmx_boost_math_weibull_distribution_shape(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_scale(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_mean(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_mode(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_standarddeviation(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_skewness(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_pdf(boost::math::weibull_distribution<double> * dist, double k);
	double bmx_boost_math_weibull_distribution_cdf(boost::math::weibull_distribution<double> * dist, double k);
	void bmx_boost_math_weibull_distribution_free(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_quantile(boost::math::weibull_distribution<double> * dist, double k);
	double bmx_boost_math_weibull_distribution_cdfcomplement(boost::math::weibull_distribution<double> * dist, double k);
	double bmx_boost_math_weibull_distribution_quantilecomplement(boost::math::weibull_distribution<double> * dist, double k);
	double bmx_boost_math_weibull_distribution_hazard(boost::math::weibull_distribution<double> * dist, double x);
	double bmx_boost_math_weibull_distribution_chf(boost::math::weibull_distribution<double> * dist, double x);
	double bmx_boost_math_weibull_distribution_median(boost::math::weibull_distribution<double> * dist);
	void bmx_boost_math_weibull_distribution_range(boost::math::weibull_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_weibull_distribution_variance(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_kurtosis(boost::math::weibull_distribution<double> * dist);
	double bmx_boost_math_weibull_distribution_kurtosisexcess(boost::math::weibull_distribution<double> * dist);

	boost::math::fisher_f_distribution<double> * bmx_boost_math_fisher_f_distribution_create(double i, double j);
	double bmx_boost_math_fisher_f_distribution_degreesoffreedom1(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_degreesoffreedom2(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_mean(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_mode(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_standarddeviation(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_skewness(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_pdf(boost::math::fisher_f_distribution<double> * dist, double k);
	double bmx_boost_math_fisher_f_distribution_cdf(boost::math::fisher_f_distribution<double> * dist, double k);
	void bmx_boost_math_fisher_f_distribution_free(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_quantile(boost::math::fisher_f_distribution<double> * dist, double k);
	double bmx_boost_math_fisher_f_distribution_cdfcomplement(boost::math::fisher_f_distribution<double> * dist, double k);
	double bmx_boost_math_fisher_f_distribution_quantilecomplement(boost::math::fisher_f_distribution<double> * dist, double k);
	double bmx_boost_math_fisher_f_distribution_hazard(boost::math::fisher_f_distribution<double> * dist, double x);
	double bmx_boost_math_fisher_f_distribution_chf(boost::math::fisher_f_distribution<double> * dist, double x);
	double bmx_boost_math_fisher_f_distribution_median(boost::math::fisher_f_distribution<double> * dist);
	void bmx_boost_math_fisher_f_distribution_range(boost::math::fisher_f_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_fisher_f_distribution_variance(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_kurtosis(boost::math::fisher_f_distribution<double> * dist);
	double bmx_boost_math_fisher_f_distribution_kurtosisexcess(boost::math::fisher_f_distribution<double> * dist);
	
	boost::math::lognormal_distribution<double> * bmx_boost_math_lognormal_distribution_create(double location, double scale);
	double bmx_boost_math_lognormal_distribution_location(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_scale(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_mean(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_mode(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_standarddeviation(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_skewness(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_pdf(boost::math::lognormal_distribution<double> * dist, double k);
	double bmx_boost_math_lognormal_distribution_cdf(boost::math::lognormal_distribution<double> * dist, double k);
	void bmx_boost_math_lognormal_distribution_free(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_quantile(boost::math::lognormal_distribution<double> * dist, double k);
	double bmx_boost_math_lognormal_distribution_cdfcomplement(boost::math::lognormal_distribution<double> * dist, double k);
	double bmx_boost_math_lognormal_distribution_quantilecomplement(boost::math::lognormal_distribution<double> * dist, double k);
	double bmx_boost_math_lognormal_distribution_hazard(boost::math::lognormal_distribution<double> * dist, double x);
	double bmx_boost_math_lognormal_distribution_chf(boost::math::lognormal_distribution<double> * dist, double x);
	double bmx_boost_math_lognormal_distribution_median(boost::math::lognormal_distribution<double> * dist);
	void bmx_boost_math_lognormal_distribution_range(boost::math::lognormal_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_lognormal_distribution_variance(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_kurtosis(boost::math::lognormal_distribution<double> * dist);
	double bmx_boost_math_lognormal_distribution_kurtosisexcess(boost::math::lognormal_distribution<double> * dist);


	boost::math::negative_binomial_distribution<double> * bmx_boost_math_negative_binomial_distribution_create(int r, double p);
	double bmx_boost_math_negative_binomial_distribution_successfraction(boost::math::negative_binomial_distribution<double> * dist);
	int bmx_boost_math_negative_binomial_distribution_successes(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_findlowerboundonp(int trials, int successes, double probability);
	double bmx_boost_math_negative_binomial_distribution_findupperboundonp(int trials, int successes, double probability);
	double bmx_boost_math_negative_binomial_distribution_findminimumnumberoftrials(int k, double p, double probability);
	double bmx_boost_math_negative_binomial_distribution_findmaximumnumberoftrials(int k, double p, double probability);
	double bmx_boost_math_negative_binomial_distribution_mean(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_mode(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_standarddeviation(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_skewness(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_pdf(boost::math::negative_binomial_distribution<double> * dist, double k);
	double bmx_boost_math_negative_binomial_distribution_cdf(boost::math::negative_binomial_distribution<double> * dist, double k);
	void bmx_boost_math_negative_binomial_distribution_free(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_quantile(boost::math::negative_binomial_distribution<double> * dist, double k);
	double bmx_boost_math_negative_binomial_distribution_cdfcomplement(boost::math::negative_binomial_distribution<double> * dist, double k);
	double bmx_boost_math_negative_binomial_distribution_quantilecomplement(boost::math::negative_binomial_distribution<double> * dist, double k);
	double bmx_boost_math_negative_binomial_distribution_hazard(boost::math::negative_binomial_distribution<double> * dist, double x);
	double bmx_boost_math_negative_binomial_distribution_chf(boost::math::negative_binomial_distribution<double> * dist, double x);
	double bmx_boost_math_negative_binomial_distribution_median(boost::math::negative_binomial_distribution<double> * dist);
	void bmx_boost_math_negative_binomial_distribution_range(boost::math::negative_binomial_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_negative_binomial_distribution_variance(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_kurtosis(boost::math::negative_binomial_distribution<double> * dist);
	double bmx_boost_math_negative_binomial_distribution_kurtosisexcess(boost::math::negative_binomial_distribution<double> * dist);

	boost::math::non_central_beta_distribution<double> * bmx_boost_math_non_central_beta_distribution_create(double alpha, double beta, double lambda);
	double bmx_boost_math_non_central_beta_distribution_alpha(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_beta(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_noncentrality(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_mean(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_mode(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_standarddeviation(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_skewness(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_pdf(boost::math::non_central_beta_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_beta_distribution_cdf(boost::math::non_central_beta_distribution<double> * dist, double k);
	void bmx_boost_math_non_central_beta_distribution_free(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_quantile(boost::math::non_central_beta_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_beta_distribution_cdfcomplement(boost::math::non_central_beta_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_beta_distribution_quantilecomplement(boost::math::non_central_beta_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_beta_distribution_hazard(boost::math::non_central_beta_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_beta_distribution_chf(boost::math::non_central_beta_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_beta_distribution_median(boost::math::non_central_beta_distribution<double> * dist);
	void bmx_boost_math_non_central_beta_distribution_range(boost::math::non_central_beta_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_non_central_beta_distribution_variance(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_kurtosis(boost::math::non_central_beta_distribution<double> * dist);
	double bmx_boost_math_non_central_beta_distribution_kurtosisexcess(boost::math::non_central_beta_distribution<double> * dist);


	boost::math::non_central_chi_squared_distribution<double> * bmx_boost_math_non_central_chi_squared_distribution_create(double v, double lambda);
	double bmx_boost_math_non_central_chi_squared_distribution_degreesoffreedom(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_noncentrality(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_mean(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_mode(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_standarddeviation(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_skewness(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_pdf(boost::math::non_central_chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_chi_squared_distribution_cdf(boost::math::non_central_chi_squared_distribution<double> * dist, double k);
	void bmx_boost_math_non_central_chi_squared_distribution_free(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_quantile(boost::math::non_central_chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_chi_squared_distribution_cdfcomplement(boost::math::non_central_chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_chi_squared_distribution_quantilecomplement(boost::math::non_central_chi_squared_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_chi_squared_distribution_hazard(boost::math::non_central_chi_squared_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_chi_squared_distribution_chf(boost::math::non_central_chi_squared_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_chi_squared_distribution_median(boost::math::non_central_chi_squared_distribution<double> * dist);
	void bmx_boost_math_non_central_chi_squared_distribution_range(boost::math::non_central_chi_squared_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_non_central_chi_squared_distribution_variance(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_kurtosis(boost::math::non_central_chi_squared_distribution<double> * dist);
	double bmx_boost_math_non_central_chi_squared_distribution_kurtosisexcess(boost::math::non_central_chi_squared_distribution<double> * dist);


	boost::math::non_central_f_distribution<double> * bmx_boost_math_non_central_f_distribution_create(double v1, double v2, double lambda);
	double bmx_boost_math_non_central_f_distribution_degreesoffreedom1(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_degreesoffreedom2(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_noncentrality(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_mean(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_mode(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_standarddeviation(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_skewness(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_pdf(boost::math::non_central_f_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_f_distribution_cdf(boost::math::non_central_f_distribution<double> * dist, double k);
	void bmx_boost_math_non_central_f_distribution_free(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_quantile(boost::math::non_central_f_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_f_distribution_cdfcomplement(boost::math::non_central_f_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_f_distribution_quantilecomplement(boost::math::non_central_f_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_f_distribution_hazard(boost::math::non_central_f_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_f_distribution_chf(boost::math::non_central_f_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_f_distribution_median(boost::math::non_central_f_distribution<double> * dist);
	void bmx_boost_math_non_central_f_distribution_range(boost::math::non_central_f_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_non_central_f_distribution_variance(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_kurtosis(boost::math::non_central_f_distribution<double> * dist);
	double bmx_boost_math_non_central_f_distribution_kurtosisexcess(boost::math::non_central_f_distribution<double> * dist);


	boost::math::non_central_t_distribution<double> * bmx_boost_math_non_central_t_distribution_create(double v, double delta);
	double bmx_boost_math_non_central_t_distribution_degreesoffreedom(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_noncentrality(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_mean(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_mode(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_standarddeviation(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_skewness(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_pdf(boost::math::non_central_t_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_t_distribution_cdf(boost::math::non_central_t_distribution<double> * dist, double k);
	void bmx_boost_math_non_central_t_distribution_free(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_quantile(boost::math::non_central_t_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_t_distribution_cdfcomplement(boost::math::non_central_t_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_t_distribution_quantilecomplement(boost::math::non_central_t_distribution<double> * dist, double k);
	double bmx_boost_math_non_central_t_distribution_hazard(boost::math::non_central_t_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_t_distribution_chf(boost::math::non_central_t_distribution<double> * dist, double x);
	double bmx_boost_math_non_central_t_distribution_median(boost::math::non_central_t_distribution<double> * dist);
	void bmx_boost_math_non_central_t_distribution_range(boost::math::non_central_t_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_non_central_t_distribution_variance(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_kurtosis(boost::math::non_central_t_distribution<double> * dist);
	double bmx_boost_math_non_central_t_distribution_kurtosisexcess(boost::math::non_central_t_distribution<double> * dist);

	boost::math::poisson_distribution<double> * bmx_boost_math_poisson_distribution_create(double mean);
	double bmx_boost_math_poisson_distribution_mean(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_mode(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_standarddeviation(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_skewness(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_pdf(boost::math::poisson_distribution<double> * dist, double k);
	double bmx_boost_math_poisson_distribution_cdf(boost::math::poisson_distribution<double> * dist, double k);
	void bmx_boost_math_poisson_distribution_free(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_quantile(boost::math::poisson_distribution<double> * dist, double k);
	double bmx_boost_math_poisson_distribution_cdfcomplement(boost::math::poisson_distribution<double> * dist, double k);
	double bmx_boost_math_poisson_distribution_quantilecomplement(boost::math::poisson_distribution<double> * dist, double k);
	double bmx_boost_math_poisson_distribution_hazard(boost::math::poisson_distribution<double> * dist, double x);
	double bmx_boost_math_poisson_distribution_chf(boost::math::poisson_distribution<double> * dist, double x);
	double bmx_boost_math_poisson_distribution_median(boost::math::poisson_distribution<double> * dist);
	void bmx_boost_math_poisson_distribution_range(boost::math::poisson_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_poisson_distribution_variance(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_kurtosis(boost::math::poisson_distribution<double> * dist);
	double bmx_boost_math_poisson_distribution_kurtosisexcess(boost::math::poisson_distribution<double> * dist);

	boost::math::rayleigh_distribution<double> * bmx_boost_math_rayleigh_distribution_create(double sigma);
	double bmx_boost_math_rayleigh_distribution_sigma(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_mean(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_mode(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_standarddeviation(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_skewness(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_pdf(boost::math::rayleigh_distribution<double> * dist, double k);
	double bmx_boost_math_rayleigh_distribution_cdf(boost::math::rayleigh_distribution<double> * dist, double k);
	void bmx_boost_math_rayleigh_distribution_free(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_quantile(boost::math::rayleigh_distribution<double> * dist, double k);
	double bmx_boost_math_rayleigh_distribution_cdfcomplement(boost::math::rayleigh_distribution<double> * dist, double k);
	double bmx_boost_math_rayleigh_distribution_quantilecomplement(boost::math::rayleigh_distribution<double> * dist, double k);
	double bmx_boost_math_rayleigh_distribution_hazard(boost::math::rayleigh_distribution<double> * dist, double x);
	double bmx_boost_math_rayleigh_distribution_chf(boost::math::rayleigh_distribution<double> * dist, double x);
	double bmx_boost_math_rayleigh_distribution_median(boost::math::rayleigh_distribution<double> * dist);
	void bmx_boost_math_rayleigh_distribution_range(boost::math::rayleigh_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_rayleigh_distribution_variance(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_kurtosis(boost::math::rayleigh_distribution<double> * dist);
	double bmx_boost_math_rayleigh_distribution_kurtosisexcess(boost::math::rayleigh_distribution<double> * dist);

	boost::math::students_t_distribution<double> * bmx_boost_math_students_t_distribution_create(double v);
	double bmx_boost_math_students_t_distribution_degreesoffreedom(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_mean(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_mode(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_standarddeviation(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_skewness(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_pdf(boost::math::students_t_distribution<double> * dist, double k);
	double bmx_boost_math_students_t_distribution_cdf(boost::math::students_t_distribution<double> * dist, double k);
	void bmx_boost_math_students_t_distribution_free(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_quantile(boost::math::students_t_distribution<double> * dist, double k);
	double bmx_boost_math_students_t_distribution_cdfcomplement(boost::math::students_t_distribution<double> * dist, double k);
	double bmx_boost_math_students_t_distribution_quantilecomplement(boost::math::students_t_distribution<double> * dist, double k);
	double bmx_boost_math_students_t_distribution_hazard(boost::math::students_t_distribution<double> * dist, double x);
	double bmx_boost_math_students_t_distribution_chf(boost::math::students_t_distribution<double> * dist, double x);
	double bmx_boost_math_students_t_distribution_median(boost::math::students_t_distribution<double> * dist);
	void bmx_boost_math_students_t_distribution_range(boost::math::students_t_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_students_t_distribution_variance(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_kurtosis(boost::math::students_t_distribution<double> * dist);
	double bmx_boost_math_students_t_distribution_kurtosisexcess(boost::math::students_t_distribution<double> * dist);

	boost::math::triangular_distribution<double> * bmx_boost_math_triangular_distribution_create(double lower, double mode, double upper);
	double bmx_boost_math_triangular_distribution_lower(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_upper(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_mean(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_mode(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_standarddeviation(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_skewness(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_pdf(boost::math::triangular_distribution<double> * dist, double k);
	double bmx_boost_math_triangular_distribution_cdf(boost::math::triangular_distribution<double> * dist, double k);
	void bmx_boost_math_triangular_distribution_free(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_quantile(boost::math::triangular_distribution<double> * dist, double k);
	double bmx_boost_math_triangular_distribution_cdfcomplement(boost::math::triangular_distribution<double> * dist, double k);
	double bmx_boost_math_triangular_distribution_quantilecomplement(boost::math::triangular_distribution<double> * dist, double k);
	double bmx_boost_math_triangular_distribution_hazard(boost::math::triangular_distribution<double> * dist, double x);
	double bmx_boost_math_triangular_distribution_chf(boost::math::triangular_distribution<double> * dist, double x);
	double bmx_boost_math_triangular_distribution_median(boost::math::triangular_distribution<double> * dist);
	void bmx_boost_math_triangular_distribution_range(boost::math::triangular_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_triangular_distribution_variance(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_kurtosis(boost::math::triangular_distribution<double> * dist);
	double bmx_boost_math_triangular_distribution_kurtosisexcess(boost::math::triangular_distribution<double> * dist);

	boost::math::uniform_distribution<double> * bmx_boost_math_uniform_distribution_create(double lower, double upper);
	double bmx_boost_math_uniform_distribution_lower(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_upper(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_mean(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_mode(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_standarddeviation(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_skewness(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_pdf(boost::math::uniform_distribution<double> * dist, double k);
	double bmx_boost_math_uniform_distribution_cdf(boost::math::uniform_distribution<double> * dist, double k);
	void bmx_boost_math_uniform_distribution_free(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_quantile(boost::math::uniform_distribution<double> * dist, double k);
	double bmx_boost_math_uniform_distribution_cdfcomplement(boost::math::uniform_distribution<double> * dist, double k);
	double bmx_boost_math_uniform_distribution_quantilecomplement(boost::math::uniform_distribution<double> * dist, double k);
	double bmx_boost_math_uniform_distribution_hazard(boost::math::uniform_distribution<double> * dist, double x);
	double bmx_boost_math_uniform_distribution_chf(boost::math::uniform_distribution<double> * dist, double x);
	double bmx_boost_math_uniform_distribution_median(boost::math::uniform_distribution<double> * dist);
	void bmx_boost_math_uniform_distribution_range(boost::math::uniform_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_uniform_distribution_variance(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_kurtosis(boost::math::uniform_distribution<double> * dist);
	double bmx_boost_math_uniform_distribution_kurtosisexcess(boost::math::uniform_distribution<double> * dist);

	boost::math::gamma_distribution<double> * bmx_boost_math_gamma_distribution_create(double shape, double scale);
	double bmx_boost_math_gamma_distribution_shape(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_scale(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_mean(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_mode(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_standarddeviation(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_skewness(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_pdf(boost::math::gamma_distribution<double> * dist, double k);
	double bmx_boost_math_gamma_distribution_cdf(boost::math::gamma_distribution<double> * dist, double k);
	void bmx_boost_math_gamma_distribution_free(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_quantile(boost::math::gamma_distribution<double> * dist, double k);
	double bmx_boost_math_gamma_distribution_cdfcomplement(boost::math::gamma_distribution<double> * dist, double k);
	double bmx_boost_math_gamma_distribution_quantilecomplement(boost::math::gamma_distribution<double> * dist, double k);
	double bmx_boost_math_gamma_distribution_hazard(boost::math::gamma_distribution<double> * dist, double x);
	double bmx_boost_math_gamma_distribution_chf(boost::math::gamma_distribution<double> * dist, double x);
	double bmx_boost_math_gamma_distribution_median(boost::math::gamma_distribution<double> * dist);
	void bmx_boost_math_gamma_distribution_range(boost::math::gamma_distribution<double> * dist, double * rangeStart, double * rangeEnd);
	double bmx_boost_math_gamma_distribution_variance(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_kurtosis(boost::math::gamma_distribution<double> * dist);
	double bmx_boost_math_gamma_distribution_kurtosisexcess(boost::math::gamma_distribution<double> * dist);

}


void bmx_throw_overflow_exception() {
	bbExThrow(_bah_mathtoolkit_TOverflowException__create());
}

void bmx_throw_domain_exception() {
	bbExThrow(_bah_mathtoolkit_TDomainException__create());
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

double bmx_boost_math_sphbessel(unsigned int v, double x) {
	return boost::math::sph_bessel(v, x);
}

double bmx_boost_math_sphneumann(unsigned int v, double x) {
	return boost::math::sph_neumann(v, x);
}

double bmx_boost_math_log1p(double x) {
	return boost::math::log1p<double>(x);
}

double bmx_boost_math_expm1(double x) {
	return boost::math::expm1<double>(x);
}

double bmx_boost_math_sqrt1pm1(double x) {
	return boost::math::sqrt1pm1<double>(x);
}

double bmx_boost_math_powm1(double x, double y) {
	return boost::math::powm1<double>(x, y);
}

double bmx_boost_math_zeta(double z) {
	return boost::math::zeta<double>(z);
}

// ************************************************

boost::math::binomial_distribution<double> * bmx_boost_math_binomial_distribution_create(int n, double p) {
	try {
		return new boost::math::binomial_distribution<double>(n, p);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_successfraction(boost::math::binomial_distribution<double> * dist) {
	return dist->success_fraction();
}

int bmx_boost_math_binomial_distribution_trials(boost::math::binomial_distribution<double> * dist) {
	return dist->trials();
}

double bmx_boost_math_binomial_distribution_mean(boost::math::binomial_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_mode(boost::math::binomial_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_standarddeviation(boost::math::binomial_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_skewness(boost::math::binomial_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_pdf(boost::math::binomial_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_cdf(boost::math::binomial_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_binomial_distribution_free(boost::math::binomial_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_binomial_distribution_quantile(boost::math::binomial_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_cdfcomplement(boost::math::binomial_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_quantilecomplement(boost::math::binomial_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_hazard(boost::math::binomial_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_chf(boost::math::binomial_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_median(boost::math::binomial_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_binomial_distribution_range(boost::math::binomial_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_binomial_distribution_variance(boost::math::binomial_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_kurtosis(boost::math::binomial_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_binomial_distribution_kurtosisexcess(boost::math::binomial_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::bernoulli_distribution<double> * bmx_boost_math_bernoulli_distribution_create(double p) {
	return new boost::math::bernoulli_distribution<double>(p);
}

double bmx_boost_math_bernoulli_distribution_successfraction(boost::math::bernoulli_distribution<double> * dist) {
	return dist->success_fraction();
}

double bmx_boost_math_bernoulli_distribution_mean(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_mode(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_standarddeviation(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_skewness(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_pdf(boost::math::bernoulli_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_cdf(boost::math::bernoulli_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_bernoulli_distribution_free(boost::math::bernoulli_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_bernoulli_distribution_quantile(boost::math::bernoulli_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_cdfcomplement(boost::math::bernoulli_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_quantilecomplement(boost::math::bernoulli_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_hazard(boost::math::bernoulli_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_chf(boost::math::bernoulli_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_median(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_bernoulli_distribution_range(boost::math::bernoulli_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_bernoulli_distribution_variance(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_kurtosis(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_bernoulli_distribution_kurtosisexcess(boost::math::bernoulli_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::beta_distribution<double> * bmx_boost_math_beta_distribution_create(double a, double b) {
	try {
		return new boost::math::beta_distribution<double>(a, b);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_alpha(boost::math::beta_distribution<double> * dist) {
	return dist->alpha();
}

double bmx_boost_math_beta_distribution_beta(boost::math::beta_distribution<double> * dist) {
	return dist->beta();
}

double bmx_boost_math_beta_distribution_mean(boost::math::beta_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_mode(boost::math::beta_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_standarddeviation(boost::math::beta_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_skewness(boost::math::beta_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_pdf(boost::math::beta_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_cdf(boost::math::beta_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_beta_distribution_free(boost::math::beta_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_beta_distribution_quantile(boost::math::beta_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_cdfcomplement(boost::math::beta_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_quantilecomplement(boost::math::beta_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_hazard(boost::math::beta_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_chf(boost::math::beta_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_median(boost::math::beta_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_beta_distribution_range(boost::math::beta_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_beta_distribution_variance(boost::math::beta_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_kurtosis(boost::math::beta_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_beta_distribution_kurtosisexcess(boost::math::beta_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::cauchy_distribution<double> * bmx_boost_math_cauchy_distribution_create(double location, double scale) {
	try {
		return new boost::math::cauchy_distribution<double>(location, scale);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_location(boost::math::cauchy_distribution<double> * dist) {
	return dist->location();
}

double bmx_boost_math_cauchy_distribution_scale(boost::math::cauchy_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_cauchy_distribution_mean(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_cauchy_distribution_mode(boost::math::cauchy_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_standarddeviation(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_cauchy_distribution_skewness(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_cauchy_distribution_pdf(boost::math::cauchy_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_cdf(boost::math::cauchy_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_cauchy_distribution_free(boost::math::cauchy_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_cauchy_distribution_quantile(boost::math::cauchy_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_cdfcomplement(boost::math::cauchy_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_quantilecomplement(boost::math::cauchy_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_hazard(boost::math::cauchy_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_chf(boost::math::cauchy_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_cauchy_distribution_median(boost::math::cauchy_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_cauchy_distribution_range(boost::math::cauchy_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_cauchy_distribution_variance(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_cauchy_distribution_kurtosis(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_cauchy_distribution_kurtosisexcess(boost::math::cauchy_distribution<double> * dist) {
	bmx_throw_domain_exception();
}


// ************************************************

boost::math::chi_squared_distribution<double> * bmx_boost_math_chi_squared_distribution_create(double i) {
	try {
		return new boost::math::chi_squared_distribution<double>(i);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_degreesoffreedom(boost::math::chi_squared_distribution<double> * dist) {
	return dist->degrees_of_freedom();
}

double bmx_boost_math_chi_squared_distribution_mean(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_mode(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_standarddeviation(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_skewness(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_pdf(boost::math::chi_squared_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_cdf(boost::math::chi_squared_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_chi_squared_distribution_free(boost::math::chi_squared_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_chi_squared_distribution_quantile(boost::math::chi_squared_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_cdfcomplement(boost::math::chi_squared_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_quantilecomplement(boost::math::chi_squared_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_hazard(boost::math::chi_squared_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_chf(boost::math::chi_squared_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_median(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_chi_squared_distribution_range(boost::math::chi_squared_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_chi_squared_distribution_variance(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_kurtosis(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_chi_squared_distribution_kurtosisexcess(boost::math::chi_squared_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::exponential_distribution<double> * bmx_boost_math_exponential_distribution_create(double lambda) {
	try {
		return new boost::math::exponential_distribution<double>(lambda);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_lambda(boost::math::exponential_distribution<double> * dist) {
	return dist->lambda();
}

double bmx_boost_math_exponential_distribution_mean(boost::math::exponential_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_mode(boost::math::exponential_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_standarddeviation(boost::math::exponential_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_skewness(boost::math::exponential_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_pdf(boost::math::exponential_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_cdf(boost::math::exponential_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_exponential_distribution_free(boost::math::exponential_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_exponential_distribution_quantile(boost::math::exponential_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_cdfcomplement(boost::math::exponential_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_quantilecomplement(boost::math::exponential_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_hazard(boost::math::exponential_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_chf(boost::math::exponential_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_median(boost::math::exponential_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_exponential_distribution_range(boost::math::exponential_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_exponential_distribution_variance(boost::math::exponential_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_kurtosis(boost::math::exponential_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_exponential_distribution_kurtosisexcess(boost::math::exponential_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::extreme_value_distribution<double> * bmx_boost_math_extreme_value_distribution_create(double location, double scale) {
	try {
		return new boost::math::extreme_value_distribution<double>(location, scale);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_location(boost::math::extreme_value_distribution<double> * dist) {
	return dist->location();
}

double bmx_boost_math_extreme_value_distribution_scale(boost::math::extreme_value_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_extreme_value_distribution_mean(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_mode(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_standarddeviation(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_skewness(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_pdf(boost::math::extreme_value_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_cdf(boost::math::extreme_value_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_extreme_value_distribution_free(boost::math::extreme_value_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_extreme_value_distribution_quantile(boost::math::extreme_value_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_cdfcomplement(boost::math::extreme_value_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_quantilecomplement(boost::math::extreme_value_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_hazard(boost::math::extreme_value_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_chf(boost::math::extreme_value_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_median(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_extreme_value_distribution_range(boost::math::extreme_value_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_extreme_value_distribution_variance(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_kurtosis(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_extreme_value_distribution_kurtosisexcess(boost::math::extreme_value_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::normal_distribution<double> * bmx_boost_math_normal_distribution_create(double mean, double sd) {
	try {
		return new boost::math::normal_distribution<double>(mean, sd);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_location(boost::math::normal_distribution<double> * dist) {
	return dist->location();
}

double bmx_boost_math_normal_distribution_scale(boost::math::normal_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_normal_distribution_mean(boost::math::normal_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_mode(boost::math::normal_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_standarddeviation(boost::math::normal_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_skewness(boost::math::normal_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_pdf(boost::math::normal_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_cdf(boost::math::normal_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_normal_distribution_free(boost::math::normal_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_normal_distribution_quantile(boost::math::normal_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_cdfcomplement(boost::math::normal_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_quantilecomplement(boost::math::normal_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_hazard(boost::math::normal_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_chf(boost::math::normal_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_median(boost::math::normal_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_normal_distribution_range(boost::math::normal_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_normal_distribution_variance(boost::math::normal_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_kurtosis(boost::math::normal_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_normal_distribution_kurtosisexcess(boost::math::normal_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::pareto_distribution<double> * bmx_boost_math_pareto_distribution_create(double location, double shape) {
	try {
		return new boost::math::pareto_distribution<double>(location, shape);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_location(boost::math::pareto_distribution<double> * dist) {
	return dist->location();
}

double bmx_boost_math_pareto_distribution_shape(boost::math::pareto_distribution<double> * dist) {
	return dist->shape();
}

double bmx_boost_math_pareto_distribution_mean(boost::math::pareto_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_mode(boost::math::pareto_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_standarddeviation(boost::math::pareto_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_skewness(boost::math::pareto_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_pdf(boost::math::pareto_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_cdf(boost::math::pareto_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_pareto_distribution_free(boost::math::pareto_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_pareto_distribution_quantile(boost::math::pareto_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_cdfcomplement(boost::math::pareto_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_quantilecomplement(boost::math::pareto_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_hazard(boost::math::pareto_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_chf(boost::math::pareto_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_median(boost::math::pareto_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_pareto_distribution_range(boost::math::pareto_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_pareto_distribution_variance(boost::math::pareto_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_kurtosis(boost::math::pareto_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_pareto_distribution_kurtosisexcess(boost::math::pareto_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::weibull_distribution<double> * bmx_boost_math_weibull_distribution_create(double shape, double scale) {
	try {
		return new boost::math::weibull_distribution<double>(shape, scale);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_shape(boost::math::weibull_distribution<double> * dist) {
	return dist->shape();
}

double bmx_boost_math_weibull_distribution_scale(boost::math::weibull_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_weibull_distribution_mean(boost::math::weibull_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_mode(boost::math::weibull_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_standarddeviation(boost::math::weibull_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_skewness(boost::math::weibull_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_pdf(boost::math::weibull_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_cdf(boost::math::weibull_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_weibull_distribution_free(boost::math::weibull_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_weibull_distribution_quantile(boost::math::weibull_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_cdfcomplement(boost::math::weibull_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_quantilecomplement(boost::math::weibull_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_hazard(boost::math::weibull_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_chf(boost::math::weibull_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_median(boost::math::weibull_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_weibull_distribution_range(boost::math::weibull_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_weibull_distribution_variance(boost::math::weibull_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_kurtosis(boost::math::weibull_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_weibull_distribution_kurtosisexcess(boost::math::weibull_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::fisher_f_distribution<double> * bmx_boost_math_fisher_f_distribution_create(double i, double j) {
	try {
		return new boost::math::fisher_f_distribution<double>(i, j);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_degreesoffreedom1(boost::math::fisher_f_distribution<double> * dist) {
	return dist->degrees_of_freedom1();
}

double bmx_boost_math_fisher_f_distribution_degreesoffreedom2(boost::math::fisher_f_distribution<double> * dist) {
	return dist->degrees_of_freedom2();
}

double bmx_boost_math_fisher_f_distribution_mean(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_mode(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_standarddeviation(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_skewness(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_pdf(boost::math::fisher_f_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_cdf(boost::math::fisher_f_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_fisher_f_distribution_free(boost::math::fisher_f_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_fisher_f_distribution_quantile(boost::math::fisher_f_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_cdfcomplement(boost::math::fisher_f_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_quantilecomplement(boost::math::fisher_f_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_hazard(boost::math::fisher_f_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_chf(boost::math::fisher_f_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_median(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_fisher_f_distribution_range(boost::math::fisher_f_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_fisher_f_distribution_variance(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_kurtosis(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_fisher_f_distribution_kurtosisexcess(boost::math::fisher_f_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::lognormal_distribution<double> * bmx_boost_math_lognormal_distribution_create(double location, double scale) {
	try {
		return new boost::math::lognormal_distribution<double>(location, scale);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_location(boost::math::lognormal_distribution<double> * dist) {
	return dist->location();
}

double bmx_boost_math_lognormal_distribution_scale(boost::math::lognormal_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_lognormal_distribution_mean(boost::math::lognormal_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_mode(boost::math::lognormal_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_standarddeviation(boost::math::lognormal_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_skewness(boost::math::lognormal_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_pdf(boost::math::lognormal_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_cdf(boost::math::lognormal_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_lognormal_distribution_free(boost::math::lognormal_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_lognormal_distribution_quantile(boost::math::lognormal_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_cdfcomplement(boost::math::lognormal_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_quantilecomplement(boost::math::lognormal_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_hazard(boost::math::lognormal_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_chf(boost::math::lognormal_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_median(boost::math::lognormal_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_lognormal_distribution_range(boost::math::lognormal_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_lognormal_distribution_variance(boost::math::lognormal_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_kurtosis(boost::math::lognormal_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_lognormal_distribution_kurtosisexcess(boost::math::lognormal_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::negative_binomial_distribution<double> * bmx_boost_math_negative_binomial_distribution_create(int r, double p) {
	try {
		return new boost::math::negative_binomial_distribution<double>(r, p);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_successfraction(boost::math::negative_binomial_distribution<double> * dist) {
	return dist->success_fraction();
}

int bmx_boost_math_negative_binomial_distribution_successes(boost::math::negative_binomial_distribution<double> * dist) {
	return dist->successes();
}


double bmx_boost_math_negative_binomial_distribution_findlowerboundonp(int trials, int successes, double probability) {
	return boost::math::negative_binomial_distribution<double>::find_lower_bound_on_p(trials, successes, probability);
}

double bmx_boost_math_negative_binomial_distribution_findupperboundonp(int trials, int successes, double probability) {
	return boost::math::negative_binomial_distribution<double>::find_upper_bound_on_p(trials, successes, probability);
}

double bmx_boost_math_negative_binomial_distribution_findminimumnumberoftrials(int k, double p, double probability) {
	return boost::math::negative_binomial_distribution<double>::find_minimum_number_of_trials(k, p, probability);
}

double bmx_boost_math_negative_binomial_distribution_findmaximumnumberoftrials(int k, double p, double probability) {
	return boost::math::negative_binomial_distribution<double>::find_maximum_number_of_trials(k, p, probability);
}

double bmx_boost_math_negative_binomial_distribution_mean(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_mode(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_standarddeviation(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_skewness(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_pdf(boost::math::negative_binomial_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_cdf(boost::math::negative_binomial_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_negative_binomial_distribution_free(boost::math::negative_binomial_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_negative_binomial_distribution_quantile(boost::math::negative_binomial_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_cdfcomplement(boost::math::negative_binomial_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_quantilecomplement(boost::math::negative_binomial_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_hazard(boost::math::negative_binomial_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_chf(boost::math::negative_binomial_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_median(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_negative_binomial_distribution_range(boost::math::negative_binomial_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_negative_binomial_distribution_variance(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_kurtosis(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_negative_binomial_distribution_kurtosisexcess(boost::math::negative_binomial_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::non_central_beta_distribution<double> * bmx_boost_math_non_central_beta_distribution_create(double alpha, double beta, double lambda) {
	try {
		return new boost::math::non_central_beta_distribution<double>(alpha, beta, lambda);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_alpha(boost::math::non_central_beta_distribution<double> * dist) {
	return dist->alpha();
}

double bmx_boost_math_non_central_beta_distribution_beta(boost::math::non_central_beta_distribution<double> * dist) {
	return dist->beta();
}

double bmx_boost_math_non_central_beta_distribution_noncentrality(boost::math::non_central_beta_distribution<double> * dist) {
	return dist->non_centrality();
}

double bmx_boost_math_non_central_beta_distribution_mean(boost::math::non_central_beta_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_non_central_beta_distribution_mode(boost::math::non_central_beta_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_standarddeviation(boost::math::non_central_beta_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_skewness(boost::math::non_central_beta_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_non_central_beta_distribution_pdf(boost::math::non_central_beta_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_cdf(boost::math::non_central_beta_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_beta_distribution_free(boost::math::non_central_beta_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_non_central_beta_distribution_quantile(boost::math::non_central_beta_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_cdfcomplement(boost::math::non_central_beta_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_quantilecomplement(boost::math::non_central_beta_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_hazard(boost::math::non_central_beta_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_chf(boost::math::non_central_beta_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_median(boost::math::non_central_beta_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_beta_distribution_range(boost::math::non_central_beta_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_non_central_beta_distribution_variance(boost::math::non_central_beta_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_beta_distribution_kurtosis(boost::math::non_central_beta_distribution<double> * dist) {
	bmx_throw_domain_exception();
}

double bmx_boost_math_non_central_beta_distribution_kurtosisexcess(boost::math::non_central_beta_distribution<double> * dist) {
	bmx_throw_domain_exception();
}


// ************************************************


boost::math::non_central_chi_squared_distribution<double> * bmx_boost_math_non_central_chi_squared_distribution_create(double v, double lambda) {
	try {
		return new boost::math::non_central_chi_squared_distribution<double>(v, lambda);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_degreesoffreedom(boost::math::non_central_chi_squared_distribution<double> * dist) {
	return dist->degrees_of_freedom();
}

double bmx_boost_math_non_central_chi_squared_distribution_noncentrality(boost::math::non_central_chi_squared_distribution<double> * dist) {
	return dist->non_centrality();
}

double bmx_boost_math_non_central_chi_squared_distribution_mean(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_mode(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_standarddeviation(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_skewness(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_pdf(boost::math::non_central_chi_squared_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_cdf(boost::math::non_central_chi_squared_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_chi_squared_distribution_free(boost::math::non_central_chi_squared_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_non_central_chi_squared_distribution_quantile(boost::math::non_central_chi_squared_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_cdfcomplement(boost::math::non_central_chi_squared_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_quantilecomplement(boost::math::non_central_chi_squared_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_hazard(boost::math::non_central_chi_squared_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_chf(boost::math::non_central_chi_squared_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_median(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_chi_squared_distribution_range(boost::math::non_central_chi_squared_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_non_central_chi_squared_distribution_variance(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_kurtosis(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_chi_squared_distribution_kurtosisexcess(boost::math::non_central_chi_squared_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::non_central_f_distribution<double> * bmx_boost_math_non_central_f_distribution_create(double v1, double v2, double lambda) {
	try {
		return new boost::math::non_central_f_distribution<double>(v1, v2, lambda);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_degreesoffreedom1(boost::math::non_central_f_distribution<double> * dist) {
	return dist->degrees_of_freedom1();
}

double bmx_boost_math_non_central_f_distribution_degreesoffreedom2(boost::math::non_central_f_distribution<double> * dist) {
	return dist->degrees_of_freedom2();
}

double bmx_boost_math_non_central_f_distribution_noncentrality(boost::math::non_central_f_distribution<double> * dist) {
	return dist->non_centrality();
}

double bmx_boost_math_non_central_f_distribution_mean(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_mode(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_standarddeviation(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_skewness(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_pdf(boost::math::non_central_f_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_cdf(boost::math::non_central_f_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_f_distribution_free(boost::math::non_central_f_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_non_central_f_distribution_quantile(boost::math::non_central_f_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_cdfcomplement(boost::math::non_central_f_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_quantilecomplement(boost::math::non_central_f_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_hazard(boost::math::non_central_f_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_chf(boost::math::non_central_f_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_median(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_f_distribution_range(boost::math::non_central_f_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_non_central_f_distribution_variance(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_kurtosis(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_f_distribution_kurtosisexcess(boost::math::non_central_f_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::non_central_t_distribution<double> * bmx_boost_math_non_central_t_distribution_create(double v, double delta) {
	try {
		return new boost::math::non_central_t_distribution<double>(v, delta);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_degreesoffreedom(boost::math::non_central_t_distribution<double> * dist) {
	return dist->degrees_of_freedom();
}

double bmx_boost_math_non_central_t_distribution_noncentrality(boost::math::non_central_t_distribution<double> * dist) {
	return dist->non_centrality();
}

double bmx_boost_math_non_central_t_distribution_mean(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_mode(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_standarddeviation(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_skewness(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_pdf(boost::math::non_central_t_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_cdf(boost::math::non_central_t_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_t_distribution_free(boost::math::non_central_t_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_non_central_t_distribution_quantile(boost::math::non_central_t_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_cdfcomplement(boost::math::non_central_t_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_quantilecomplement(boost::math::non_central_t_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_hazard(boost::math::non_central_t_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_chf(boost::math::non_central_t_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_median(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_non_central_t_distribution_range(boost::math::non_central_t_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_non_central_t_distribution_variance(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_kurtosis(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_non_central_t_distribution_kurtosisexcess(boost::math::non_central_t_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::poisson_distribution<double> * bmx_boost_math_poisson_distribution_create(double mean) {
	try {
		return new boost::math::poisson_distribution<double>(mean);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_mean(boost::math::poisson_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_mode(boost::math::poisson_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_standarddeviation(boost::math::poisson_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_skewness(boost::math::poisson_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_pdf(boost::math::poisson_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_cdf(boost::math::poisson_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_poisson_distribution_free(boost::math::poisson_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_poisson_distribution_quantile(boost::math::poisson_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_cdfcomplement(boost::math::poisson_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_quantilecomplement(boost::math::poisson_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_hazard(boost::math::poisson_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_chf(boost::math::poisson_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_median(boost::math::poisson_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_poisson_distribution_range(boost::math::poisson_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_poisson_distribution_variance(boost::math::poisson_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_kurtosis(boost::math::poisson_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_poisson_distribution_kurtosisexcess(boost::math::poisson_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************



boost::math::rayleigh_distribution<double> * bmx_boost_math_rayleigh_distribution_create(double sigma) {
	try {
		return new boost::math::rayleigh_distribution<double>(sigma);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_sigma(boost::math::rayleigh_distribution<double> * dist) {
	return dist->sigma();
}

double bmx_boost_math_rayleigh_distribution_mean(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_mode(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_standarddeviation(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_skewness(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_pdf(boost::math::rayleigh_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_cdf(boost::math::rayleigh_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_rayleigh_distribution_free(boost::math::rayleigh_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_rayleigh_distribution_quantile(boost::math::rayleigh_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_cdfcomplement(boost::math::rayleigh_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_quantilecomplement(boost::math::rayleigh_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_hazard(boost::math::rayleigh_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_chf(boost::math::rayleigh_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_median(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_rayleigh_distribution_range(boost::math::rayleigh_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_rayleigh_distribution_variance(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_kurtosis(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_rayleigh_distribution_kurtosisexcess(boost::math::rayleigh_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::students_t_distribution<double> * bmx_boost_math_students_t_distribution_create(double sigma) {
	try {
		return new boost::math::students_t_distribution<double>(sigma);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_degreesoffreedom(boost::math::students_t_distribution<double> * dist) {
	return dist->degrees_of_freedom();
}

double bmx_boost_math_students_t_distribution_mean(boost::math::students_t_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_mode(boost::math::students_t_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_standarddeviation(boost::math::students_t_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_skewness(boost::math::students_t_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_pdf(boost::math::students_t_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_cdf(boost::math::students_t_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_students_t_distribution_free(boost::math::students_t_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_students_t_distribution_quantile(boost::math::students_t_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_cdfcomplement(boost::math::students_t_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_quantilecomplement(boost::math::students_t_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_hazard(boost::math::students_t_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_chf(boost::math::students_t_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_median(boost::math::students_t_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_students_t_distribution_range(boost::math::students_t_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_students_t_distribution_variance(boost::math::students_t_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_kurtosis(boost::math::students_t_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_students_t_distribution_kurtosisexcess(boost::math::students_t_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************


boost::math::triangular_distribution<double> * bmx_boost_math_triangular_distribution_create(double lower, double mode, double upper) {
	try {
		return new boost::math::triangular_distribution<double>(lower, mode, upper);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_lower(boost::math::triangular_distribution<double> * dist) {
	return dist->lower();
}

double bmx_boost_math_triangular_distribution_upper(boost::math::triangular_distribution<double> * dist) {
	return dist->upper();
}

double bmx_boost_math_triangular_distribution_mean(boost::math::triangular_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_mode(boost::math::triangular_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_standarddeviation(boost::math::triangular_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_skewness(boost::math::triangular_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_pdf(boost::math::triangular_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_cdf(boost::math::triangular_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_triangular_distribution_free(boost::math::triangular_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_triangular_distribution_quantile(boost::math::triangular_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_cdfcomplement(boost::math::triangular_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_quantilecomplement(boost::math::triangular_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_hazard(boost::math::triangular_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_chf(boost::math::triangular_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_median(boost::math::triangular_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_triangular_distribution_range(boost::math::triangular_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_triangular_distribution_variance(boost::math::triangular_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_kurtosis(boost::math::triangular_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_triangular_distribution_kurtosisexcess(boost::math::triangular_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::uniform_distribution<double> * bmx_boost_math_uniform_distribution_create(double lower, double upper) {
	try {
		return new boost::math::uniform_distribution<double>(lower, upper);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_lower(boost::math::uniform_distribution<double> * dist) {
	return dist->lower();
}

double bmx_boost_math_uniform_distribution_upper(boost::math::uniform_distribution<double> * dist) {
	return dist->upper();
}

double bmx_boost_math_uniform_distribution_mean(boost::math::uniform_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_mode(boost::math::uniform_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_standarddeviation(boost::math::uniform_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_skewness(boost::math::uniform_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_pdf(boost::math::uniform_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_cdf(boost::math::uniform_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_uniform_distribution_free(boost::math::uniform_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_uniform_distribution_quantile(boost::math::uniform_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_cdfcomplement(boost::math::uniform_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_quantilecomplement(boost::math::uniform_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_hazard(boost::math::uniform_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_chf(boost::math::uniform_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_median(boost::math::uniform_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_uniform_distribution_range(boost::math::uniform_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_uniform_distribution_variance(boost::math::uniform_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_kurtosis(boost::math::uniform_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_uniform_distribution_kurtosisexcess(boost::math::uniform_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************

boost::math::gamma_distribution<double> * bmx_boost_math_gamma_distribution_create(double shape, double scale) {
	try {
		return new boost::math::gamma_distribution<double>(shape, scale);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_shape(boost::math::gamma_distribution<double> * dist) {
	return dist->shape();
}

double bmx_boost_math_gamma_distribution_scale(boost::math::gamma_distribution<double> * dist) {
	return dist->scale();
}

double bmx_boost_math_gamma_distribution_mean(boost::math::gamma_distribution<double> * dist) {
	try {
		return mean(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_mode(boost::math::gamma_distribution<double> * dist) {
	try {
		return mode(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_standarddeviation(boost::math::gamma_distribution<double> * dist) {
	try {
		return standard_deviation(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_skewness(boost::math::gamma_distribution<double> * dist) {
	try {
		return skewness(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_pdf(boost::math::gamma_distribution<double> * dist, double k) {
	try {
		return pdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_cdf(boost::math::gamma_distribution<double> * dist, double k) {
	try {
		return cdf(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_gamma_distribution_free(boost::math::gamma_distribution<double> * dist) {
	delete dist;
}

double bmx_boost_math_gamma_distribution_quantile(boost::math::gamma_distribution<double> * dist, double k) {
	try {
		return quantile(*dist, k);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_cdfcomplement(boost::math::gamma_distribution<double> * dist, double k) {
	try {
		return cdf(complement(*dist, k));
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_quantilecomplement(boost::math::gamma_distribution<double> * dist, double k) {
	try {
		return quantile(complement(*dist, k));
	} catch(std::overflow_error) {
		bmx_throw_overflow_exception();
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_hazard(boost::math::gamma_distribution<double> * dist, double x) {
	try {
		return hazard(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_chf(boost::math::gamma_distribution<double> * dist, double x) {
	try {
		return chf(*dist, x);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_median(boost::math::gamma_distribution<double> * dist) {
	try {
		return median(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

void bmx_boost_math_gamma_distribution_range(boost::math::gamma_distribution<double> * dist, double * rangeStart, double * rangeEnd) {
	std::pair<double, double> result = range(*dist);
	*rangeStart = result.first;
	*rangeEnd = result.second;
}

double bmx_boost_math_gamma_distribution_variance(boost::math::gamma_distribution<double> * dist) {
	try {
		return variance(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_kurtosis(boost::math::gamma_distribution<double> * dist) {
	try {
		return kurtosis(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}

double bmx_boost_math_gamma_distribution_kurtosisexcess(boost::math::gamma_distribution<double> * dist) {
	try {
		return kurtosis_excess(*dist);
	} catch(...) {
		bmx_throw_domain_exception();
	}
}


// ************************************************









