' Copyright (c) 2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Math Toolkit
End Rem
Module BaH.MathToolkit

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions"


Import "common.bmx"



Rem
bbdoc: Returns i!.
about: If @i is so large that the result can not be represented in a double, then throws TOverflowException.
End Rem
Function Factorial:Double(i:Int)
	Return bmx_boost_math_factorial(i)
End Function

Rem
bbdoc: Returns i!.
about: Internally this function performs table lookup of the result. Further it performs no range checking on the value of i:
it is up to the caller to ensure that i <= MaxFactorial(). This function is intended to be used inside inner loops that require
fast table lookup of factorials, but requires care to ensure that argument i never grows too large.
<p>
If @i is so large that the result can not be represented in a double, then throws TOverflowException.
</p>
End Rem
Function UncheckedFactorial:Double(i:Int)
	Return bmx_boost_math_uncheckedfactorial(i)
End Function

Rem
bbdoc: Defines the largest value that can be passed to UncheckedFactorial().
about: The value can be used where integral constant expressions are required: for example to define the size of
further tables that depend on the factorials.
End Rem
Function MaxFactorial:Int()
	Return bmx_boost_math_maxfactorial()
End Function

Rem
bbdoc: Returns i!!.
about: May throw TOverflowException if the result is too large to be represented by a Double. The implementation is designed to
be optimised for small @i where table lookup of @i! is possible.
<p>
The double factorial is implemented in terms of the factorial and gamma
functions using the relations:
</p>
<p>
(2n)!! = 2<sup>n </sup> * n!
</p>
<p>
(2n+1)!! = (2n+1)! / (2<sup>n </sup> n!)
</p>
<p>
and
</p>
<p>
(2n-1)!! = &#915;((2n+1)/2) * 2<sup>n </sup> / sqrt(pi)
</p>
End Rem
Function DoubleFactorial:Double(i:Int)
	Return bmx_boost_math_doublefactorial(i)
End Function

Rem
bbdoc: Returns the rising factorial of @x and @i.
about: rising_factorial(x, i) = &#915;(x + i) / &#915;(x);
<p>
or
</p>
<p>
rising_factorial(x, i) = x(x+1)(x+2)(x+3)...(x+i)
</p>
<p>
Note that both @x and @i can be negative as well as positive.
</p>
<p>
May throw TOverflowException if the result is too large to be represented by a Double.
</p>
End Rem
Function RisingFactorial:Double(x:Double, i:Int)
	Return bmx_boost_math_risingfactorial(x, i)
End Function

Rem
bbdoc: Returns the falling factorial of @x and @i.
about: falling_factorial(x, i) = x(x-1)(x-2)(x-3)...(x-i+1)
<p>
Note that this function is only defined for positive @i, hence the unsigned second argument. Argument @x can be either positive
or negative however.
</p>
<p>
May throw TOverflowException if the result is too large to be represented by a Double.
</p>
End Rem
Function FallingFactorial:Double(x:Double, i:Int)
	Return bmx_boost_math_fallingfactorial(x, i)
End Function

Rem
bbdoc: Returns the binomial coefficient: <sub>n</sub>C<sub>k</sub>.
about: Requires k <= n.
<p>
May throw TOverflowException if the result is too large to be represented by a Double.
</p>
End Rem
Function BinomialCoefficient:Double(n:Int, k:Int)
	Return bmx_boost_math_binomialcoefficient(n, k)
End Function

Rem
bbdoc: Returns the cubed root of @x: x<sup>1/3</sup>.
about: Implemented using Halley iteration.
<p>
The following graph illustrates the behaviour of cbrt:
</p>
<p>
<img src="cbrt.png" align="center">
</p>
End Rem
Function Cbrt:Double(x:Double)
	Return bmx_boost_math_cbrt(x)
End Function

Rem
bbdoc: Computes <img src="hypot.png">in such a way as to avoid undue underflow and overflow.
about: When calculating <img src="hypot.png"> it's quite easy for the intermediate terms to
either overflow or underflow, even though the result is in fact perfectly representable.
End Rem
Function Hypot:Double(x:Double, y:Double)
	Return bmx_boost_math_hypot(x, y)
End Function

Rem
bbdoc: 
End Rem
Function Log1p:Double(x:Double)
	Return bmx_boost_math_log1p(x)
End Function

Rem
bbdoc: 
End Rem
Function Expm1:Double(x:Double)
	Return bmx_boost_math_expm1(x)
End Function

Rem
bbdoc: 
End Rem
Function Sqrt1pm1:Double(x:Double)
	Return bmx_boost_math_sqrt1pm1(x)
End Function

Rem
bbdoc: 
End Rem
Function Powm1:Double(x:Double, y:Double)
	Return bmx_boost_math_powm1(x, y)
End Function

Rem
bbdoc: Returns the result of the Spherical Bessel functions of the first kind.
about: sph_bessel(v, x) = j<sub>v</sub>(x)
<p>
where:
</p>
<p>
<img src="sbessel.png">
</p>
<p>
The function throws TDomainException whenever the result is undefined or complex: this occurs when <tt>x &lt; 0</tt>.
</p>
<p>
The j<sub>v</sub> function is cyclic like J<sub>v</sub> but differs in its behaviour at the origin:
</p>
<p>
<img src="sph_bessel.png" align="middle">
</p>
End Rem
Function SphBessel:Double(v:Int, x:Double)
	Return bmx_boost_math_sphbessel(v, x)
End Function

Rem
bbdoc: Returns the result of the Spherical Bessel functions of the second kind.
about: sph_neumann(v, x) = y<sub>v</sub>(x) = n<sub>v</sub>(x)
<p>
where:
</p>
<p>
<img src="sneumann.png">
</p>
<p>
The function throws TDomainException whenever the result is undefined or complex: this occurs when <tt>x &lt; 0</tt>.
</p>
<p>
The j<sub>v</sub> function is cyclic like J<sub>v</sub> but differs in its behaviour at the origin:
</p>
<p>
<img src="sph_neumann.png" align="middle">
</p>
End Rem
Function SphNeumann:Double(v:Int, x:Double)
	Return bmx_boost_math_sphneumann(v, x)
End Function

Rem
bbdoc: Calculate the mean of @dist.
about: This function may throw a TDomainException if the distribution does not have a defined mean (for example the Cauchy distribution).
End Rem
Function Mean:Double(dist:TDistribution)
	Return dist.Mean()
End Function

Rem
bbdoc: Calculate the mode of @dist.
about: The mode is the value that occurs the most frequently in a data set or a probability distribution.
<p>
This function may throw a TDomainException if the distribution does not have a defined mode.
</p>
End Rem
Function Mode:Double(dist:TDistribution)
	Return dist.Mode()
End Function

Rem
bbdoc: Calculate the standard deviation of @dist.
about: Standard deviation is a simple measure of the variability or dispersion of a data set.
<p>
This function may throw a TDomainException if the distribution does not have a defined standard deviation.
</p>
End Rem
Function StandardDeviation:Double(dist:TDistribution)
	Return dist.StandardDeviation()
End Function

Rem
bbdoc: Determine skewness on @dist.
about: Skewness is a measure of the asymmetry of the probability distribution of a real-valued random variable.
<p>
This function may throw a TDomainException if the distribution does not have a defined skewness.
</p>
End Rem
Function Skewness:Double(dist:TDistribution)
	Return dist.Skewness()
End Function

Rem
bbdoc: Perform a probability density function on @dist.
about: For a continuous function, the probability density function (pdf) returns the probability that the variate has the value @x.
Since for continuous distributions the probability at a single point is actually zero, the probability is better expressed as the
integral of the pdf between two points: see the Cumulative Distribution Function (Cdf).
<p>
For a discrete distribution, the pdf is the probability that the variate takes the value x.
</p>
<p>
This function may throw a TDomainException if the random variable is outside the defined range for the distribution.
</p>
<p>
For example for a standard normal distribution the pdf looks like this:
</p>
<p>
<img src="pdf.png" align="middle">
</p>
End Rem
Function Pdf:Double(dist:TDistribution, k:Double)
	Return dist.Pdf(k)
End Function

Rem
bbdoc: Perform a cumulative distribution function on @dist.
about: The Cumulative Distribution Function is the probability that the variable takes a value less than or equal to @x. It is
equivalent to the integral from -infinity to x of the Probability Density Function.
<p>
This function may throw a TDomainException if the random variable is outside the defined range for the distribution.
</p>
<p>
For example the following graph shows the cdf for the normal distribution:
</p>
<p>
<img src="cdf.png" align="middle">
</p>
End Rem
Function Cdf:Double(dist:TDistribution, k:Double)
	Return dist.Cdf(k)
End Function

Rem
bbdoc: Calculate the complement of the cumulative distribution function on @dist.
about: The complement of the Cumulative Distribution Function is the probability that the variable takes a value greater than @x.
It is equivalent to the integral from x to infinity of the Probability Density Function, or 1 minus the Cumulative Distribution Function of @x.
<p>
This is also known as the survival function.
</p>
<p>
This function may throw a TDomainException if the random variable is outside the defined range for the distribution.
</p>
<p>
For example the following graph shows the complement of the cdf for the normal distribution:
</p>
<p>
<img src="survival.png" align="middle">
</p>
End Rem
Function CdfComplement:Double(dist:TDistribution, k:Double)
	Return dist.CdfComplement(k)
End Function

Rem
bbdoc: Calculate the quantile of @dist, for @p.
about: The quantile is best viewed as the inverse of the Cumulative Distribution Function, it returns a value x such that cdf(dist, x) == p.
<p>
This is also known as the percent point function, or a percentile, it is also the same as calculating the lower critical value of a
distribution.
</p>
<p>
This function throws a TDomainException if the probability lies outside [0,1]. The function may return an overflow_error if there is no
finite value that has the specified probability.
</p>
<p>
The following graph shows the quantile function for a standard normal distribution:
</p>
<p>
<img src="quantile.png" align="middle">
</p>
End Rem
Function Quantile:Double(dist:TDistribution, p:Double)
	Return dist.Quantile(p)
End Function

Rem
bbdoc: 
End Rem
Function QuantileComplement:Double(dist:TDistribution, k:Double)
	Return dist.QuantileComplement(k)
End Function

Rem
bbdoc: Returns the Hazard Function of @x and distribution @dist.
about: This function may throw a TDomainException if the random variable is outside the defined range for the distribution.
<p>
<img src="hazard.png">
</p>
<table border="0">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Caution]" src="caution.png"></td>
<th align="left">Caution</th>
</tr>
<tr><td align="left" valign="top"><p>
            Some authors refer to this as the conditional failure density function
            rather than the hazard function.
          </p></td></tr>
</table>
End Rem
Function Hazard:Double(dist:TDistribution, x:Double)
	Return dist.Hazard(x)
End Function

Rem
bbdoc: Returns the Cumulative Hazard Function of @x and distibution @dist.
about: This function may throw a TDomainException if the random variable is outside the defined range for the distribution.
<p>
<img src="chf.png">
</p>
<table border="0" summary="Caution">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Caution]" src="caution.png"></td>
<th align="left">Caution</th>
</tr>
<tr><td align="left" valign="top"><p>
            Some authors refer to this as simply the "Hazard Function".
          </p></td></tr>
</table>
End Rem
Function Chf:Double(dist:TDistribution, x:Double)
	Return dist.Chf(x)
End Function

Rem
bbdoc: Returns the median of the distribution @dist.
End Rem
Function Median:Double(dist:TDistribution)
	Return dist.Median()
End Function

Rem
bbdoc: Returns the valid range of the random variable over distribution @dist.
End Rem
Function Range(dist:TDistribution, rangeStart:Double Var, rangeEnd:Double Var)
	dist.Range(rangeStart, rangeEnd)
End Function

Rem
bbdoc: Returns the variance of the distribution @dist.
about: This function may throw a TDomainException if the distribution does not have a defined variance.
End Rem
Function Variance:Double(dist:TDistribution)
	Return dist.Variance()
End Function

Rem
bbdoc: Returns the 'proper' kurtosis (normalized fourth moment) of the distribution @dist.
about: kertosis = &#946;<sub>2</sub>= &#956;<sub>4</sub> / &#956;<sub>2</sub><sup>2</sup>
<p>
Where &#956;<sub>i</sub> is the i'th central moment of the distribution, and in particular
&#956;<sub>2</sub> is the variance of the distribution.
</p>
<p>
The kurtosis is a measure of the "peakedness" of a distribution.
</p>
<p>
Note that the literature definition of kurtosis is confusing. The definition used here is that used by for example 
<a href="http://mathworld.wolfram.com/Kurtosis.html">Wolfram MathWorld</a> (that includes a table of formulae for kurtosis excess
for various distributions) but NOT the definition of <a href="http://en.wikipedia.org/wiki/Kurtosis">kurtosis used by Wikipedia</a> which
treats "kurtosis" and "kurtosis excess" as the same quantity.
</p>
<pre>kurtosis_excess = 'proper' kurtosis - 3
</pre>
<p>
This subtraction of 3 is convenient so that the <em>kurtosis excess</em> of a normal distribution is zero.
</p>
<p>
This function may throw a TDomainException if the distribution does not have a defined kurtosis.
</p>
<p>
'Proper' kurtosis can have a value from zero to + infinity.
</p>
End Rem
Function Kurtosis:Double(dist:TDistribution)
	Return dist.Kurtosis()
End Function

Rem
bbdoc: Returns the kurtosis excess of the distribution @dist.
<p>
kurtosis excess = &#947;<sub>2</sub>= &#956;<sub>4</sub> / &#956;<sub>2</sub><sup>2</sup>- 3 = kurtosis - 3
</p>
<p>
Where &#956;<sub>i</sub> is the i'th central moment of the distribution, and in particular
&#956;<sub>2</sub> is the variance of the distribution.
</p>
<p>
The kurtosis excess is a measure of the "peakedness" of a distribution, and is more widely used than the "kurtosis proper". It is defined
so that the kurtosis excess of a normal distribution is zero.
</p>
<p>
This function may throw a TDomainException if the distribution does not have a defined kurtosis excess.
</p>
<p>
Kurtosis excess can have a value from -2 to + infinity.
</p>
<pre>kurtosis = kurtosis_excess + 3
</pre>
<p>
The kurtosis excess of a normal distribution is zero.
</p>
End Rem
Function KurtosisExcess:Double(dist:TDistribution)
	Return dist.KurtosisExcess()
End Function



Rem
bbdoc: Base type for Distributions.
End Rem
Type TDistribution

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method Mean:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method Mode:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method StandardDeviation:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method Skewness:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method Pdf:Double(k:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Cdf:Double(k:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method CdfComplement:Double(k:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Quantile:Double(p:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method QuantileComplement:Double(p:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Hazard:Double(x:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Chf:Double(x:Double) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Median:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method Range(rangeStart:Double Var, rangeEnd:Double Var) Abstract
	Rem
	bbdoc: 
	End Rem
	Method Variance:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method Kurtosis:Double() Abstract
	Rem
	bbdoc: 
	End Rem
	Method KurtosisExcess:Double() Abstract

End Type

Rem
bbdoc: Represents a <a href="http://mathworld.wolfram.com/BinomialDistribution.html">binomial distribution</a>.
about: It is used when there are exactly two mutually exclusive outcomes of a trial. These outcomes are
labelled "success" and "failure". The binomial distribution is used to obtain the probability of observing @k
successes in @N trials, with the probability of success on a single trial denoted by p. The binomial
distribution assumes that p is fixed for all trials.
          </p>
<table border="0" summary="Note">
<tr>
<th align="left">Note</th>
</tr>
<tr><td align="left" valign="top">
<p>
The random variable for the binomial distribution is the number of
successes, (the number of trials is a fixed property of the distribution)
whereas for the negative binomial, the random variable is the number
of trials, for a fixed number of successes.
</p>
</td></tr>
</table>
<p>
The PDF for the binomial distribution is given by:
</p>
<p>
<img src="binomial_ref2.png">
</p>
<p>
The following two graphs illustrate how the PDF changes depending upon
the distributions parameters, first we'll keep the success fraction
<em>p</em> fixed at 0.5, and vary the sample size:
</p>
<p>
<img src="binomial_pdf_1.png" align="middle">
</p>
<p>
Alternatively, we can keep the sample size fixed at N=20 and vary the
success fraction <em>p</em>:
</p>
<p>
<img src="binomial_pdf_2.png" align="middle">
</p>
<table border="0" summary="Caution">
<tr>
<th align="left">Caution</th>
</tr>
<tr><td align="left" valign="top">
<p>
The Binomial distribution is a discrete distribution: internally functions like the <tt>cdf</tt> and
<tt>pdf</tt> are treated "as if" they are continuous functions, but in reality the
results returned from these functions only have meaning if an integer
value is provided for the random variate argument.
</p>
<p>
The quantile function will by default return an integer result that
has been <em>rounded outwards</em>. That is to say lower
quantiles (where the probability is less than 0.5) are rounded downward,
and upper quantiles (where the probability is greater than 0.5) are
rounded upwards. This behaviour ensures that if an X% quantile is
requested, then <em>at least</em> the requested coverage
will be present in the central region, and <em>no more than</em>
the requested coverage will be present in the tails.
</p>
</td></tr>
</table>
End Rem
Type TBinomialDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TBinomialDistribution instance.
	about: n is the total number of trials, p  is the probability of success of a single trial.
	<p>
	Requires 0 <= p <= 1, and n >= 0, otherwise throws TDomainException. 
	</p>
	End Rem
	Function CreateBinomial:TBinomialDistribution(n:Int, p:Double)
		Return New TBinomialDistribution.Create(n, p)
	End Function
	
	Rem
	bbdoc: Creates a new TBinomialDistribution instance.
	about: n is the total number of trials, p  is the probability of success of a single trial.
	<p>
	Requires 0 <= p <= 1, and n >= 0, otherwise throws TDomainException. 
	</p>
	End Rem
	Method Create:TBinomialDistribution(n:Int, p:Double)
		objectPtr = bmx_boost_math_binomial_distribution_create(n, p)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SuccessFraction:Double()
		Return bmx_boost_math_binomial_distribution_successfraction(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Trials:Int()
		Return bmx_boost_math_binomial_distribution_trials(objectPtr)
	End Method
	
	Method Mean:Double()
		Return bmx_boost_math_binomial_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_binomial_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_binomial_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_binomial_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_binomial_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_binomial_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_binomial_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_binomial_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_binomial_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_binomial_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_binomial_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_binomial_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_binomial_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_binomial_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_binomial_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_binomial_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_binomial_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The Bernoulli distribution is a discrete distribution of the outcome of a single trial with only two results, 0 (failure) or 1 (success), with a probability of success p.
about: The Bernoulli distribution is the simplest building block on which other discrete distributions of sequences of
independent Bernoulli trials can be based.
<p>
The Bernoulli is the binomial distribution (k = 1, p) with only one trial.
</p>
<p>
The following graph illustrates how the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability
density function pdf</a> varies with the outcome of the single trial:
</p>
<p>
<img src="bernoulli_pdf.png" align="middle">
</p>
<p>
and the <a href="http://en.wikipedia.org/wiki/Cumulative_Distribution_Function" >Cumulative
distribution function</a>
</p>
<p>
<img src="bernoulli_cdf.png" align="middle">
</p>
End Rem
Type TBernoulliDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TBernoulliDistribution instance.
	End Rem
	Function CreateBernoulli:TBernoulliDistribution(p:Double)
		Return New TBernoulliDistribution.Create(p)
	End Function
	
	Rem
	bbdoc: Creates a new TBernoulliDistribution instance.
	End Rem
	Method Create:TBernoulliDistribution(p:Double)
		objectPtr = bmx_boost_math_bernoulli_distribution_create(p)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SuccessFraction:Double()
		Return bmx_boost_math_bernoulli_distribution_successfraction(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_bernoulli_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_bernoulli_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_bernoulli_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_bernoulli_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_bernoulli_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_bernoulli_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_bernoulli_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_bernoulli_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_bernoulli_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_bernoulli_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_bernoulli_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_bernoulli_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_bernoulli_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_bernoulli_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_bernoulli_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_bernoulli_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_bernoulli_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Represents a beta probability distribution function.
about: The beta distribution is used as a prior distribution for binomial proportions in Bayesian analysis.
<p>
The <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability
density function PDF</a> for the <a href="http://en.wikipedia.org/wiki/Beta_distribution" >beta
distribution</a> defined on the interval [0,1] is given by:
</p>
<p>
f(x;&#945;,&#946;) = x<sup>&#945; - 1</sup> (1 - x)<sup>&#946; -1</sup> / B(&#945;, &#946;)
</p>
<p>
where B(&#945;, &#946;) is the <a href="http://en.wikipedia.org/wiki/Beta_function">beta
function</a>, implemented in this library as Beta().
Division by the beta function ensures that the pdf is normalized to the
range zero to unity.
</p>
<p>
The following graph illustrates examples of the pdf for various values
of the shape parameters. Note the &#945; = &#946; = 2 (blue line) is dome-shaped, and
might be approximated by a symmetrical triangular distribution.
</p>
<p>
<img src="beta_pdf.png" align="middle">
</p>
<p>
If &#945; = &#946; = 1, then it is a &#8203;
<a href="http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29" >uniform
distribution</a>, equal to unity in the entire interval x = 0 to
1. If &#945; &#8203; and &#946; &#8203; are &lt; 1, then the pdf is U-shaped. If &#945; != &#946;, then the shape
is asymmetric and could be approximated by a triangle whose apex is away
from the centre (where x = half).
</p>
End Rem
Type TBetaDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TBetaDistribution instance with shape parameters @alpha and @beta.
	about: Requires @alpha, @beta &gt; 0, otherwise TDomainException is thrown. Note that technically the beta distribution is defined
	for alpha,beta &gt;= 0, but it's not clear whether any program can actually make use of that latitude or how many of
	the non-member functions can be usefully defined in that case. Therefore for now, we regard it as an error if @alpha or @beta is zero.
	End Rem
	Function CreateBeta:TBetaDistribution(a:Double, b:Double)
		Return New TBetaDistribution.Create(a, b)
	End Function
	
	Rem
	bbdoc: Creates a new TBetaDistribution instance with shape parameters @alpha and @beta.
	about: Requires @alpha, @beta &gt; 0, otherwise TDomainException is thrown. Note that technically the beta distribution is defined
	for alpha,beta &gt;= 0, but it's not clear whether any program can actually make use of that latitude or how many of
	the non-member functions can be usefully defined in that case. Therefore for now, we regard it as an error if @alpha or @beta is zero.
	End Rem
	Method Create:TBetaDistribution(a:Double, b:Double)
		objectPtr = bmx_boost_math_beta_distribution_create(a, b)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @alpha from which this distribution was constructed.
	End Rem
	Method Alpha:Double()
		Return bmx_boost_math_beta_distribution_alpha(objectPtr)
	End Method

	Rem
	bbdoc: Returns the parameter @beta from which this distribution was constructed.
	End Rem
	Method Beta:Double()
		Return bmx_boost_math_beta_distribution_beta(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_beta_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_beta_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_beta_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_beta_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_beta_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_beta_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_beta_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_beta_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_beta_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_beta_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_beta_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_beta_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_beta_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_beta_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_beta_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_beta_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_beta_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The Cauchy-Lorentz distribution is a <a href="http://en.wikipedia.org/wiki/Probability_distribution">continuous probability distribution</a>.
about: It has a <a href="http://en.wikipedia.org/wiki/Probability_distribution">probability
distribution function PDF</a> given by:
<p>
<img src="cauchy_ref1.png">
</p>
<p>
The location parameter x<sub>0</sub> is the location of the peak of the distribution
(the mode of the distribution), while the scale parameter &#947; specifies half
the width of the PDF at half the maximum height. If the location is zero,
and the scale 1, then the result is a standard Cauchy distribution.
</p>
<p>
The distribution is important in physics as it is the solution to the
differential equation describing forced resonance, while in spectroscopy
it is the description of the line shape of spectral lines.
</p>
<p>
The following graph shows how the distributions moves as the location
parameter changes:
</p>
<p>
<img src="cauchy_pdf1.png" align="middle">
</p>
<p>
While the following graph shows how the shape (scale) parameter alters
the distribution:
</p>
<p>
<img src="cauchy_pdf2.png" align="middle">
</p>
<p>
Note : The functions mean, standardDeviation, variance, skewness, kurtosis  and kurtosisExcess  will
all throw a TDomainException if called.
</p>
End Rem
Type TCauchyDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TCauchyDistribution instance.
	End Rem
	Function CreateCauchy:TCauchyDistribution(location:Double = 0, scale:Double = 1)
		Return New TCauchyDistribution.Create(location, scale)
	End Function
	
	Rem
	bbdoc: Creates a new TCauchyDistribution instance.
	End Rem
	Method Create:TCauchyDistribution(location:Double = 0, scale:Double = 1)
		objectPtr = bmx_boost_math_cauchy_distribution_create(location, scale)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the @location parameter of the distribution. 
	End Rem
	Method Location:Double()
		Return bmx_boost_math_cauchy_distribution_location(objectPtr)
	End Method

	Rem
	bbdoc: Returns the @scale parameter of the distribution. 
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_cauchy_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_cauchy_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_cauchy_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_cauchy_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_cauchy_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_cauchy_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_cauchy_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_cauchy_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_cauchy_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_cauchy_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_cauchy_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_cauchy_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_cauchy_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_cauchy_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_cauchy_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_cauchy_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_cauchy_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_cauchy_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The Chi-Squared distribution is one of the most widely used distributions in statistical tests.
about: If &#967;<sub>i</sub> are &#957; independent, normally distributed random
variables with means &#956;<sub>i</sub> and variances &#963;<sub>i</sub><sup>2</sup>, then the random variable:
<p>
<img src="chi_squ_ref1.png">
</p>
<p>
is distributed according to the Chi-Squared distribution.
</p>
<p>
The Chi-Squared distribution is a special case of the gamma distribution
and has a single parameter &#957; that specifies the number of degrees of freedom.
The following graph illustrates how the distribution changes for different
values of &#957;:
</p>
<p>
<img src="chi_squared_pdf.png" align="middle">
</p>
End Rem
Type TChiSquaredDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TChiSquaredDistribution instance, with @v degrees of freedom.
	about: Requires @v &gt; 0, otherwise throws TDomainException. 
	End Rem
	Function CreateChiSquared:TChiSquaredDistribution(v:Double)
		Return New TChiSquaredDistribution.Create(v)
	End Function
	
	Rem
	bbdoc: Creates a new TChiSquaredDistribution instance, with @v degrees of freedom. .
	about: Requires @v &gt; 0, otherwise throws TDomainException. 
	End Rem
	Method Create:TChiSquaredDistribution(v:Double)
		objectPtr = bmx_boost_math_chi_squared_distribution_create(v)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @v from which this object was constructed.
	End Rem
	Method DegreesOfFreedom:Double()
		Return bmx_boost_math_chi_squared_distribution_degreesoffreedom(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_chi_squared_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_chi_squared_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_chi_squared_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_chi_squared_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_chi_squared_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_chi_squared_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_chi_squared_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_chi_squared_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_chi_squared_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_chi_squared_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_chi_squared_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_chi_squared_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_chi_squared_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_chi_squared_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_chi_squared_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_chi_squared_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_chi_squared_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Exponential_distribution">exponential distribution</a>.
about: It is a <a href="http://en.wikipedia.org/wiki/Probability_distribution">continuous
probability distribution</a> with PDF:
<p>
<img src="exponential_dist_ref1.png">
</p>
<p>
It is often used to model the time between independent events that happen at a constant average rate.
</p>
<p>
The following graph shows how the distribution changes for different values of the rate parameter lambda:
</p>
<p>
<img src="exponential_pdf.png" align="middle">
</p>
End Rem
Type TExponentialDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TExponentialDistribution instance.
	End Rem
	Function CreateExponential:TExponentialDistribution(lambda:Double = 1)
		Return New TExponentialDistribution.Create(lambda)
	End Function
	
	Rem
	bbdoc: Creates a new TExponentialDistribution instance.
	End Rem
	Method Create:TExponentialDistribution(lambda:Double = 1)
		objectPtr = bmx_boost_math_exponential_distribution_create(lambda)
		Return Self
	End Method
	
	Rem
	bbdoc: Accessor function returns the @lambda parameter of the distribution.
	End Rem
	Method Lambda:Double()
		Return bmx_boost_math_exponential_distribution_lambda(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_exponential_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_exponential_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_exponential_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_exponential_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_exponential_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_exponential_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_exponential_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_exponential_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_exponential_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_exponential_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_exponential_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_exponential_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_exponential_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_exponential_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_exponential_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_exponential_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_exponential_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Extreme value theory is important for assessing risk for highly unusual events, such as 100-year floods.
about: There are various <a href="http://mathworld.wolfram.com/ExtremeValueDistribution.html">extreme
value distributions</a> : this implementation represents the maximum
case, and is variously known as a Fisher-Tippett distribution, a log-Weibull
distribution or a Gumbel distribution.
</p>
<p>
More information can be found on the <a href="http://www.itl.nist.gov/div898/handbook/eda/section3/eda366g.htm">NIST</a>,
<a href="http://en.wikipedia.org/wiki/Extreme_value_distribution">Wikipedia</a>,
<a href="http://mathworld.wolfram.com/ExtremeValueDistribution.html">Mathworld</a>,
and <a href="http://en.wikipedia.org/wiki/Extreme_value_theory">Extreme
value theory</a> websites.
</p>
<p>
The relationship of the types of extreme value distributions, of which
this is but one, is discussed by <a href="http://www.worldscibooks.com/mathematics/p191.html">Extreme
Value Distributions, Theory and Applications Samuel Kotz &amp; Saralees
Nadarajah</a>.
</p>
<p>
The distribution has a PDF given by:
</p>
<p>
f(x) = (1/scale) e<sup>-(x-location)/scale</sup> e<sup>-e<sup>-(x-location)/scale</sup></sup>
</p>
<p>
Which in the standard case (scale = 1, location = 0) reduces to:
</p>
<p>
f(x) = e<sup>-x</sup>e<sup>-e<sup>-x</sup></sup>
</p>
<p>
The following graph illustrates how the PDF varies with the location
parameter:
</p>
<p>
<img src="extreme_value_pdf1.png" align="middle">
</p>
<p>
And this graph illustrates how the PDF varies with the shape parameter:
</p>
<p>
<img src="extreme_value_pdf2.png" align="middle">
</p>
End Rem
Type TExtremeValueDistribution Extends TDistribution

	Rem
	bbdoc: Creates a new TExtremeValueDistribution instance with the specified @location and @scale parameters.
	about: Requires scale &gt; 0, otherwise throws TDomainException.
	End Rem
	Function CreateExtremeValue:TExtremeValueDistribution(location:Double = 0, scale:Double = 1)
		Return New TExtremeValueDistribution.Create(location, scale)
	End Function
	
	Rem
	bbdoc: Creates a new TExtremeValueDistribution instance with the specified @location and @scale parameters.
	about: Requires scale &gt; 0, otherwise throws TDomainException.
	End Rem
	Method Create:TExtremeValueDistribution(location:Double = 0, scale:Double = 1)
		objectPtr = bmx_boost_math_extreme_value_distribution_create(location, scale)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Location:Double()
		Return bmx_boost_math_extreme_value_distribution_location(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_extreme_value_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_extreme_value_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_extreme_value_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_extreme_value_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_extreme_value_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_extreme_value_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_extreme_value_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_extreme_value_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_extreme_value_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_extreme_value_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_extreme_value_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_extreme_value_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_extreme_value_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_extreme_value_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_extreme_value_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_extreme_value_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_extreme_value_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_extreme_value_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The normal distribution is probably the most well known statistical distribution
about: It is also known as the Gaussian Distribution. A normal distribution with mean zero and standard deviation one is known as
the <em>Standard Normal Distribution</em>.
<p>
Given mean &#956; and standard deviation &#963; it has the PDF:
</p>
<p>
<img src="normal_ref1.png">
</p>
<p>
The variation the PDF with its parameters is illustrated in the following graph:
</p>
<p>
<img src="normal_pdf.png" align="middle">
</p>
End Rem
Type TNormalDistribution Extends TDistribution

	Rem
	bbdoc: Creates a TNormalDistribution with mean @_mean and standard deviation @sd.
	about: Requires @sd &gt; 0, otherwise throws TDomainException.
	End Rem
	Function CreateNormal:TNormalDistribution(_mean:Double = 0, sd:Double = 1)
		Return New TNormalDistribution.Create(_mean, sd)
	End Function
	
	Rem
	bbdoc: Creates a TNormalDistribution with mean @_mean and standard deviation @sd.
	about: Requires @sd &gt; 0, otherwise throws TDomainException.
	End Rem
	Method Create:TNormalDistribution(_mean:Double = 0, sd:Double = 1)
		objectPtr = bmx_boost_math_normal_distribution_create(_mean, sd)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the location parameter of this distribution.
	End Rem
	Method Location:Double()
		Return bmx_boost_math_normal_distribution_location(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_normal_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_normal_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_normal_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_normal_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_normal_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_normal_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_normal_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_normal_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_normal_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_normal_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_normal_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_normal_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_normal_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_normal_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_normal_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_normal_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_normal_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_normal_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/pareto_distribution">Pareto distribution</a> is a continuous distribution.
about: It has the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function</a> (pdf) :
<p>
f(x; &#945;, &#946;) = &#945;&#946;<sup>&#945;</sup> / x<sup>&#945;+ 1</sup>
</p>
<p>
For shape parameter &#945; &gt; 0, and location parameter &#946; &gt; 0, and &#945; &gt; 0.
</p>
<p>
The <a href="http://mathworld.wolfram.com/paretoDistribution.html">Pareto distribution</a> often describes the larger compared to the smaller.
A classic example is that 80% of the wealth is owned by 20% of the population.
</p>
<p>
The following graph illustrates how the PDF varies with the location parameter &#946;:
</p>
<p>
<img src="pareto_pdf1.png" align="middle">
</p>
<p>
And this graph illustrates how the PDF varies with the shape parameter &#945;:
</p>
<p>
<img src="pareto_pdf2.png" align="middle">
</p>
End Rem
Type TParetoDistribution Extends TDistribution

	Rem
	bbdoc: Creates a TParetoDistribution with location @location and scale @scale.
	about: Requires that the @location and @scale parameters are both greater than zero, otherwise throws TDomainException.
	End Rem
	Function CreatePareto:TParetoDistribution(location:Double = 1, shape:Double = 1)
		Return New TParetoDistribution.Create(location, shape)
	End Function
	
	Rem
	bbdoc: Creates a TParetoDistribution with shape @location and scale @scale.
	about: Requires that the @location and @scale parameters are both greater than zero, otherwise throws TDomainException.
	End Rem
	Method Create:TParetoDistribution(location:Double = 1, shape:Double = 1)
		objectPtr = bmx_boost_math_pareto_distribution_create(location, shape)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the location parameter of this distribution.
	End Rem
	Method Location:Double()
		Return bmx_boost_math_pareto_distribution_location(objectPtr)
	End Method

	Rem
	bdoc: Returns the shape parameter of this distribution.
	End Rem
	Method Shape:Double()
		Return bmx_boost_math_pareto_distribution_shape(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_pareto_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_pareto_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_pareto_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_pareto_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_pareto_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_pareto_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_pareto_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_pareto_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_pareto_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_pareto_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_pareto_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_pareto_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_pareto_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_pareto_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_pareto_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_pareto_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_pareto_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Weibull_distribution">Weibull distribution</a> is a continuous distribution.
about: It has the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function</a>:
<p>
f(x; &#945;, &#946;) = (&#945;/&#946;) * (x / &#946;)<sup>&#945; - 1</sup> * e<sup>-(x/&#946;)<sup>&#945;</sup></sup>
</p>
<p>
For shape parameter &#945; &gt; 0, and scale parameter &#946; &gt; 0, and x &gt; 0.
</p>
<p>
The Weibull distribution is often used in the field of failure analysis; in particular it can mimic distributions where the failure rate varies
over time. If the failure rate is:
</p>
<ul type="disc">
<li>constant over time, then &#945; = 1, suggests that items are failing from random events.</li>
<li>decreases over time, then &#945; &lt; 1, suggesting "infant mortality".</li>
<li>increases over time, then &#945; &gt; 1, suggesting "wear out" - more likely to fail as time goes by.</li>
</ul>
<p>
The following graph illustrates how the PDF varies with the shape parameter &#945;:
</p>
<p>
<img src="weibull_pdf1.png" align="middle">
</p>
<p>
While this graph illustrates how the PDF varies with the scale parameter &#946;:
</p>
<p>
<img src="weibull_pdf2.png" align="middle">
</p>
End Rem
Type TWeibullDistribution Extends TDistribution

	Rem
	bbdoc: Creates a TWeibullDistribution with shape @shape and scale @scale.
	about: Requires that the @shape and @scale parameters are both greater than zero, otherwise throws TDomainException.
	End Rem
	Function CreateWeibull:TWeibullDistribution(shape:Double, scale:Double = 1)
		Return New TWeibullDistribution.Create(shape, scale)
	End Function
	
	Rem
	bbdoc: Creates a TWeibullDistribution with shape @shape and scale @scale.
	about: Requires that the @shape and @scale parameters are both greater than zero, otherwise throws TDomainException.
	End Rem
	Method Create:TWeibullDistribution(shape:Double, scale:Double = 1)
		objectPtr = bmx_boost_math_weibull_distribution_create(shape, scale)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the shape parameter of this distribution.
	End Rem
	Method Shape:Double()
		Return bmx_boost_math_weibull_distribution_shape(objectPtr)
	End Method

	Rem
	bdoc: Returns the scale parameter of this distribution.
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_weibull_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_weibull_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_weibull_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_weibull_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_weibull_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_weibull_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_weibull_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_weibull_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_weibull_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_weibull_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_weibull_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_weibull_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_weibull_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_weibull_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_weibull_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_weibull_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_weibull_distribution_kurtosisexcess(objectPtr)
	End Method

	
	Method Delete()
		If objectPtr Then
			bmx_boost_math_weibull_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The F distribution is a continuous distribution.
about: It arises when testing whether two samples have the same variance. If &#967;<sup>2</sup><sub>m</sub> and
&#967;<sup>2</sup><sub>n</sub> are independent variates each distributed as Chi-Squared with <em>m</em>
and <em>n</em> degrees of freedom, then the test statistic:
</p>
<p>
F<sub>n,m</sub> = (&#967;<sup>2</sup><sub>n</sub> / n) / (&#967;<sup>2</sup><sub>m</sub> / m)
</p>
<p>
Is distributed over the range [0, &#8734;] with an F distribution, and has the
PDF:
</p>
<p>
<img src="fisher_pdf.png">
</p>
<p>
The following graph illustrates how the PDF varies depending on the two degrees of freedom parameters.
</p>
<p>
<img src="fisher_f_pdf.png" align="middle">
</p>
End Rem
Type TFisherFDistribution Extends TDistribution

	Rem
	bbdoc: Creates a TFisherFDistribution with numerator degrees of freedom @i and denominator degrees of freedom @j.
	about: Requires that @i and @j are both greater than zero, otherwise throws TDomainException.
	End Rem
	Function Createfisherf:TFisherFDistribution(i:Double, j:Double)
		Return New TFisherFDistribution.Create(i, j)
	End Function
	
	Rem
	bbdoc: Creates a TFisherFDistribution with numerator degrees of freedom @i and denominator degrees of freedom @j.
	about: Requires that @i and @j are both greater than zero, otherwise throws TDomainException.
	End Rem
	Method Create:TFisherFDistribution(i:Double, j:Double)
		objectPtr = bmx_boost_math_fisher_f_distribution_create(i, j)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the numerator degrees of freedom parameter of the distribution.
	End Rem
	Method DegreesOfFreedom1:Double()
		Return bmx_boost_math_fisher_f_distribution_degreesoffreedom1(objectPtr)
	End Method

	Rem
	bbdoc: Returns the denominator degrees of freedom parameter of the distribution.
	End Rem
	Method DegreesOfFreedom2:Double()
		Return bmx_boost_math_fisher_f_distribution_degreesoffreedom2(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_fisher_f_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_fisher_f_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_fisher_f_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_fisher_f_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_fisher_f_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_fisher_f_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_fisher_f_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_fisher_f_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_fisher_f_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_fisher_f_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_fisher_f_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_fisher_f_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_fisher_f_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_fisher_f_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_fisher_f_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_fisher_f_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_fisher_f_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type


Rem
bbdoc: The lognormal distribution is the distribution that arises when the logarithm of the random variable is normally distributed.
about: A lognormal distribution results when the variable is the product of a large number of independent, identically-distributed variables.
<p>
For location and scale parameters <em>m</em> and <em>s</em> it is defined by the probability density function:
</p>
<p>
<img src="lognormal_ref.png">
</p>
<p>
The location and scale parameters are equivalent to the mean and standard deviation of the logarithm of the random variable.
</p>
<p>
The following graph illustrates the effect of the location parameter on the PDF, note that the range of the random variable remains [0,+&#8734;]
irrespective of the value of the location parameter:
</p>
<p>
<img src="lognormal_pdf1.png" align="middle">
</p>
<p>
The next graph illustrates the effect of the scale parameter on the PDF:
</p>
<p>
<img src="lognormal_pdf2.png" align="middle">
</p>
End Rem
Type TLogNormalDistribution Extends TDistribution

	Rem
	bbdoc: 
	End Rem
	Function CreateLogNormal:TLogNormalDistribution(location:Double = 0, scale:Double = 1)
		Return New TLogNormalDistribution.Create(location, scale)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TLogNormalDistribution(location:Double = 0, scale:Double = 1)
		objectPtr = bmx_boost_math_lognormal_distribution_create(location, scale)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Location:Double()
		Return bmx_boost_math_lognormal_distribution_location(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_lognormal_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_lognormal_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_lognormal_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_lognormal_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_lognormal_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_lognormal_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_lognormal_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_lognormal_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_lognormal_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_lognormal_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_lognormal_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_lognormal_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_lognormal_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_lognormal_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_lognormal_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_lognormal_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_lognormal_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_lognormal_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Represents a <a href="http://en.wikipedia.org/wiki/Negative_binomial_distribution">negative_binomial distribution</a>.
about: It is used when there are exactly two mutually exclusive outcomes of a <a href="http://en.wikipedia.org/wiki/Bernoulli_trial">Bernoulli
trial</a>: these outcomes are labelled "success" and "failure".
<p>
For k + r Bernoulli trials each with success fraction p, the negative_binomial distribution gives the probability of observing k failures
and r successes with success on the last trial. The negative_binomial distribution assumes that success_fraction p is fixed for
all (k + r) trials.
</p>
<table border="0" summary="Note">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Note]" src="note.png"></td>
<th align="left">Note</th>
</tr>
<tr><td align="left" valign="top"><p>
The random variable for the negative binomial distribution is the number of trials, (the number of successes is a fixed property of
the distribution) whereas for the binomial, the random variable is the number of successes, for a fixed number of trials.
</p></td></tr>
</table>
<p>
It has the PDF:
</p>
<p>
<img src="neg_binomial_ref.png">
</p>
<p>
The following graph illustrate how the PDF varies as the success fraction <em>p</em> changes:
</p>
<p>
<img src="negative_binomial_pdf_1.png" align="middle">
</p>
<p>
Alternatively, this graph shows how the shape of the PDF varies as the number of successes changes:
</p>
<p>
<img src="negative_binomial_pdf_2.png" align="middle">
</p>
<h5>Related Distributions</h5>
<p>
The name negative binomial distribution is reserved by some to the case
where the successes parameter r is an integer. This integer version is
also called the <a class="external" href="http://mathworld.wolfram.com/PascalDistribution.html" target="_top">Pascal
distribution</a>.
</p>
<p>
This implementation uses real numbers for the computation throughout (because it uses the <strong>real-valued</strong> incomplete
beta function family of functions). This real-valued version is also called the Polya Distribution.
</p>
<p>
The Poisson distribution is a generalization of the Pascal distribution, where the success parameter r is an integer: to obtain the
Pascal distribution you must ensure that an integer value is provided for r, and take integer values (floor or ceiling) from functions
that return a number of successes.
</p>
<p>
For large values of r (successes), the negative binomial distribution converges to the Poisson distribution.
</p>
<p>
The geometric distribution is a special case where the successes parameter r = 1, so only a first and only success is required.
geometric(p) = negative_binomial(1,
p).
</p>
<p>
The Poisson distribution is a special case for large successes
</p>
<p>
poisson(&#955;) = lim <sub>r &#8594; &#8734;</sub>  negative_binomial(r, r / (&#955; + r)))
</p>
<p>
</p>
<table border="0" summary="Caution">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Caution]" src="caution.png"></td>
<th align="left">Caution</th>
</tr>
<tr><td align="left" valign="top">
<p>
The Negative Binomial distribution is a discrete distribution: internally functions like the cdf
and pdf are treated "as if" they are continuous functions, but in reality the results returned from these functions only have meaning
if an integer value is provided for the random variate argument.
</p>
<p>
The quantile function will by default return an integer result that has been <em>rounded outwards</em>. That is to say lower
quantiles (where the probability is less than 0.5) are rounded downward, and upper quantiles (where the probability is greater than 0.5) are
rounded upwards. This behaviour ensures that if an X% quantile is requested, then <em>at least</em> the requested coverage
will be present in the central region, and <em>no more than</em> the requested coverage will be present in the tails.
</p>
</td></tr>
</table>
End Rem
Type TNegativeBinomialDistribution Extends TDistribution

	Rem
	bbdoc: 
	End Rem
	Function CreateNegativeBinomial:TNegativeBinomialDistribution(r:Int, p:Double)
		Return New TNegativeBinomialDistribution.Create(r, p)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TNegativeBinomialDistribution(r:Int, p:Double)
		objectPtr = bmx_boost_math_negative_binomial_distribution_create(r, p)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SuccessFraction:Double()
		Return bmx_boost_math_negative_binomial_distribution_successfraction(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Successes:Int()
		Return bmx_boost_math_negative_binomial_distribution_successes(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function FindLowerBoundOnP:Double(trials:Int, successes:Int, probability:Double)
		Return bmx_boost_math_negative_binomial_distribution_findlowerboundonp(trials, successes, probability)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function FindUpperBoundOnP:Double(trials:Int, successes:Int, probability:Double)
		Return bmx_boost_math_negative_binomial_distribution_findupperboundonp(trials, successes, probability)
	End Function

	Rem
	bbdoc: Estimates the number of trials required to achieve a certain probability that more than k failures will be observed.
	about: Parameters:
	<ul>
	<li><b>k</b> : The target number of failures to be observed. </li>
	<li><b>p</b> : The probability of success for each trial.</li>
	<li><b>probability</b> : The maximum acceptable risk that only k failures or fewer will be observed.</li>
	</ul>
	For example:
	<pre>
	TNegativeBinomialDistribution.FindMinimumNumberOfTrials(10, 0.5, 0.05)
	</pre>
	Returns the smallest number of trials we must conduct to be 95% sure of seeing 10 failures that occur with frequency one half.
	<p>
	This function uses numeric inversion of the negative binomial distribution to obtain the result: another interpretation of the
	result, is that it finds the number of trials (success+failures) that will lead to an alpha probability of observing k failures or fewer.
	</p>
	End Rem
	Function FindMinimumNumberOfTrials:Double(k:Int, p:Double, probability:Double)
		Return bmx_boost_math_negative_binomial_distribution_findminimumnumberoftrials(k, p, probability)
	End Function
 
	Rem
	bbdoc: 
	End Rem
 	Function FindMaximumNumberOfTrials:Double(k:Int, p:Double, probability:Double)
		Return bmx_boost_math_negative_binomial_distribution_findmaximumnumberoftrials(k, p, probability)
	End Function

	Method Mean:Double()
		Return bmx_boost_math_negative_binomial_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_negative_binomial_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_negative_binomial_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_negative_binomial_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_negative_binomial_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_negative_binomial_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_negative_binomial_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_negative_binomial_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_negative_binomial_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_negative_binomial_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_negative_binomial_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_negative_binomial_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_negative_binomial_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_negative_binomial_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_negative_binomial_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_negative_binomial_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_negative_binomial_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type






