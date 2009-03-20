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

Function Log1p:Double(x:Double)
' TODO
End Function

Function Expm1:Double(x:Double)
' TODO
End Function

Function Sqrt1pm1:Double(x:Double)
' TODO
End Function

Function Powm1:Double(x:Double, y:Double)
' TODO
End Function


Rem
bbdoc: Calculate the mean of @dist.
End Rem
Function Mean:Double(dist:TDistribution)
	Return dist.Mean()
End Function

Rem
bbdoc: Calculate the mode of @dist.
about: The mode is the value that occurs the most frequently in a data set or a probability distribution.
End Rem
Function Mode:Double(dist:TDistribution)
	Return dist.Mode()
End Function

Rem
bbdoc: Calculate the standard deviation of @dist.
about: Standard deviation is a simple measure of the variability or dispersion of a data set.
End Rem
Function StandardDeviation:Double(dist:TDistribution)
	Return dist.StandardDeviation()
End Function

Rem
bbdoc: Determine skewness on @dist.
about: Skewness is a measure of the asymmetry of the probability distribution of a real-valued random variable.
End Rem
Function Skewness:Double(dist:TDistribution)
	Return dist.Skewness()
End Function

Rem
bbdoc: Perform a probability density function on @dist.
about: A probability density function (pdf) is a function that represents a probability distribution in terms of integrals.
End Rem
Function Pdf:Double(dist:TDistribution, k:Double)
	Return dist.Pdf(k)
End Function

Rem
bbdoc: Perform a cumulative distribution function on @dist.
about: The cumulative distribution function (CDF) or just distribution function, completely describes the probability
distribution of a real-valued random variable.
End Rem
Function Cdf:Double(dist:TDistribution, k:Double)
	Return dist.Cdf(k)
End Function


Rem
bbdoc: 
End Rem
Type TDistribution

	Field objectPtr:Byte Ptr
	
	Method Mean:Double() Abstract
	Method Mode:Double() Abstract
	Method StandardDeviation:Double() Abstract
	Method Skewness:Double() Abstract
	Method Pdf:Double(k:Double) Abstract
	Method Cdf:Double(k:Double) Abstract

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
	about: Requires @alpha, @beta > 0, otherwise TDomainException is thrown. Note that technically the beta distribution is defined
	for alpha,beta >= 0, but it's not clear whether any program can actually make use of that latitude or how many of
	the non-member functions can be usefully defined in that case. Therefore for now, we regard it as an error if @alpha or @beta is zero.
	End Rem
	Function CreateBeta:TBetaDistribution(a:Double, b:Double)
		Return New TBetaDistribution.Create(a, b)
	End Function
	
	Rem
	bbdoc: Creates a new TBetaDistribution instance with shape parameters @alpha and @beta.
	about: Requires @alpha, @beta > 0, otherwise TDomainException is thrown. Note that technically the beta distribution is defined
	for alpha,beta >= 0, but it's not clear whether any program can actually make use of that latitude or how many of
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
	about: Requires @v > 0, otherwise throws TDomainException. 
	End Rem
	Function CreateChiSquared:TChiSquaredDistribution(v:Double)
		Return New TChiSquaredDistribution.Create(v)
	End Function
	
	Rem
	bbdoc: Creates a new TChiSquaredDistribution instance, with @v degrees of freedom. .
	about: Requires @v > 0, otherwise throws TDomainException. 
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
	about: Requires scale >  0, otherwise throws TDomainException.
	End Rem
	Function CreateExtremeValue:TExtremeValueDistribution(location:Double = 0, scale:Double = 1)
		Return New TExtremeValueDistribution.Create(location, scale)
	End Function
	
	Rem
	bbdoc: Creates a new TExtremeValueDistribution instance with the specified @location and @scale parameters.
	about: Requires scale >  0, otherwise throws TDomainException.
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

	Method Delete()
		If objectPtr Then
			bmx_boost_math_extreme_value_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type





