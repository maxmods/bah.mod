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
bbdoc: Returns the natural logarithm of x+1.
about: There are many situations where it is desirable to compute <code>log(x+1)</code>. However, for small <code>x</code>
then <code>x+1</code> suffers from catastrophic cancellation errors so that <code>x+1 == 1</code>
and <code>log(x+1) == 0</code>, when in fact for very small x, the best approximation to <code>log(x+1)</code> would be
 <code>x</code>. <code>log1p</code> calculates the best approximation to <code>log(1+x)</code> using a Taylor series expansion for accuracy
(less than 2&#603;). Alternatively note that there are faster methods available, for example using the equivalence:
<pre>log(1+x) == (log(1+x) * x) / ((1-x) - 1)
</pre>
<p>
However, experience has shown that these methods tend to fail quite spectacularly once the compiler's optimizations are turned on,
consequently they are used only when known not to break with a particular compiler. In contrast, the series expansion method seems to
be reasonably immune to optimizer-induced
errors.
</p>
<p>
The following graph illustrates the behaviour of log1p:
</p>
<p>
<img src="log1p.png" align="middle">
</p>
End Rem
Function Log1p:Double(x:Double)
	Return bmx_boost_math_log1p(x)
End Function

Rem
bbdoc: Returns e<sup>x</sup> - 1.
about: For small x, then <code>e<sup>x</sup></code> is very close to 1, as a result calculating <code>e<sup>x</sup> - 1</code> results in
catastrophic cancellation errors when x is small. <code>expm1</code> calculates <code>e<sup>x</sup> - 1</code> using rational approximations
(for up to 128-bit long doubles), otherwise via a series expansion when x is small (giving an accuracy of less than 2&#603;).
<p>
The following graph illustrates the behaviour of expm1:
</p>
<p>
<img src="expm1.png" align="middle">
</p>
End Rem
Function Expm1:Double(x:Double)
	Return bmx_boost_math_expm1(x)
End Function

Rem
bbdoc: Returns sqrt(1+x) - 1.
about: This function is useful when you need the difference between sqrt(x) and 1, when x is itself close to 1.
<p>
Implemented in terms of <code>log1p</code> and <code>expm1</code>.
</p>
<p>
The following graph illustrates the behaviour of sqrt1pm1:
</p>
<p>
<img src="sqrt1pm1.png" align="middle">
</p>
End Rem
Function Sqrt1pm1:Double(x:Double)
	Return bmx_boost_math_sqrt1pm1(x)
End Function

Rem
bbdoc: Returns x<sup>y </sup> - 1.
about: There are two domains where this is useful: when y is very small, or when x is close to 1.
<p>
Implemented in terms of <code>expm1</code>.
</p>
<p>
The following graph illustrates the behaviour of powm1:
</p>
<p>
<img src="powm1.png" align="middle">
</p>
End Rem
Function Powm1:Double(x:Double, y:Double)
	Return bmx_boost_math_powm1(x, y)
End Function

Rem
bbdoc: Returns the <a href="http://mathworld.wolfram.com/RiemannZetaFunction.html">zeta function</a> of z: <img src="zeta1.png">
about: <img src="zeta1.png" align="middle">
<p>
<img src="zeta2.png" align="middle">
</p>
End Rem
Function Zeta:Double(z:Double)
	Return bmx_boost_math_zeta(z)
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
bbdoc: Returns the "true gamma" of value z:
about: <img src="gamm1.png">
<p>
<img src="tgamma.png" align="middle">
</p>
End Rem
Function Gamma:Double(z:Double)
	Return bmx_boost_math_gamma(z)
End Function

Rem
bbdoc: Returns <code>Gamma(dz + 1) - 1. Internally the implementation does not make use of the addition and subtraction implied
by the definition, leading to accurate results even for very small <code>dz</code>. However, the implementation is capped to
either 35 digit accuracy.
End Rem
Function Gamma1pm1:Double(z:Double)
	Return bmx_boost_math_gamma1pm1(z)
End Function

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Gamma_function">LogGamma function</a>.
about: It is defined by:
<p>
<img src="lgamm1.png">
</p>
<p>
The second form of the function takes a pointer to an integer, which if non-null is set on output to the sign of tgamma(z).
</p>
<p>
<img src="lgamma.png" align="middle">
</p>
End Rem
Function LogGamma:Double(z:Double)
	Return bmx_boost_math_loggamma(z)
End Function

Rem
bbdoc: Returns the digamma or psi function of <em>x</em>.
about: Digamma is defined as the logarithmic derivative of the gamma function:
<p>
<img src="digamma1.png">
</p>
<p>
<img src="digamma.png" align="middle">
</p>
End Rem
Function Digamma:Double(z:Double)
	Return bmx_boost_math_digamma(z)
End Function

Rem
bbdoc: Returns the ratio of gamma functions:
about: <img src="gamma_ratio0.png">
<p>
Internally this just calls GammaDeltaRatio(a, b-a)
</p>
End Rem
Function GammaRatio:Double(a:Double, b:Double)
	Return bmx_boost_math_gammaratio(a, b)
End Function

Rem
bbdoc: Returns the ratio of gamma functions:
about: <img src="gamma_ratio1.png">
<p>
Note that the result is calculated accurately even when <em>delta</em> is small compared to <em>a</em>: indeed even if
<em>a+delta ~ a</em>. The function is typically used when <em>a</em> is large and <em>delta</em> is very small.
</p>
<p>
<img src="tgamma_delta_ratio.png" align="middle">
</p>
End Rem
Function GammaDeltaRatio:Double(a:Double, delta:Double)
	Return bmx_boost_math_gammadeltaratio(a, delta)
End Function

Rem
bbdoc: Returns the normalised lower incomplete gamma function of a and z:
about: <img src="igamma4.png">
<p>
This function changes rapidly from 0 to 1 around the point z == a:
</p>
<p>
<img src="gamma_p.png" align="middle">
</p>
End Rem
Function GammaP:Double(a:Double, z:Double)
	Return bmx_boost_math_gammap(a, z)
End Function

Rem
bbdoc: Returns the normalised upper incomplete gamma function of a and z:
about: <img src="igamma3.png">
<p>
This function changes rapidly from 1 to 0 around the point z == a:
</p>
<p>
<img src="gamma_q.png" align="middle">
</p>
End Rem
Function GammaQ:Double(a:Double, z:Double)
	Return bmx_boost_math_gammaq(a, z)
End Function

Rem
bbdoc: Returns the full (non-normalised) lower incomplete gamma function of a and z:
about: <img src="igamma2.png">
End Rem
Function GammaLower:Double(a:Double, z:Double)
	Return bmx_boost_math_gammalower(a, z)
End Function

Rem
bbdoc: Returns a value x such that: p = GammaP(a, x)
about: Requires: <em>a &gt; 0</em> and <em>1 &gt;= p,q &gt;= 0</em>.
<p>
This is an <a href="http://mathworld.wolfram.com/IncompleteGammaFunction.html">incomplete gamma function</a> inverse which computes <em>x</em> given <em>a</em> and <em>p</em>.
</p>
<table border="0" summary="Tip">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Tip]" src="tip.png"></td>
<th align="left">Tip</th>
</tr>
<tr><td align="left" valign="top">
<p>
When people normally talk about the inverse of the incomplete gamma function, they are talking about inverting on parameter <em>x</em>.
These are implemented here as gamma_p_inv and gamma_q_inv, and are by far the most efficient of the inverses presented here.
</p>
<p>
The inverse on the <em>a</em> parameter finds use in some statistical applications but has to be computed by rather brute force
numerical techniques and is consequently several times slower. These are implemented here as GammaPInvA and GammeQInvA.
</p>
</td></tr>
</table>

End Rem
Function GammaPInv:Double(a:Double, p:Double)
	Return bmx_boost_math_gammapinv(a, p)
End Function

Rem
bbdoc: Returns a value x such that: q = GammaQ(a, x)
about: Requires: <em>a &gt; 0</em> and <em>1 &gt;= p,q &gt;= 0</em>.
<p>
This is an <a href="http://mathworld.wolfram.com/IncompleteGammaFunction.html">incomplete gamma function</a> inverse which computes <em>x</em> given <em>a</em> and <em>p</em>.
</p>
<table border="0" summary="Tip">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Tip]" src="tip.png"></td>
<th align="left">Tip</th>
</tr>
<tr><td align="left" valign="top">
<p>
When people normally talk about the inverse of the incomplete gamma function, they are talking about inverting on parameter <em>x</em>.
These are implemented here as gamma_p_inv and gamma_q_inv, and are by far the most efficient of the inverses presented here.
</p>
<p>
The inverse on the <em>a</em> parameter finds use in some statistical applications but has to be computed by rather brute force
numerical techniques and is consequently several times slower. These are implemented here as GammaPInvA and GammeQInvA.
</p>
</td></tr>
</table>

End Rem
Function GammaQInv:Double(a:Double, q:Double)
	Return bmx_boost_math_gammaqinv(a, q)
End Function

Rem
bbdoc: Returns a value @a such that: p = GammaP(a, x)
about: Requires: <em>a &gt; 0</em> and <em>1 &gt;= p,q &gt;= 0</em>.
<p>
This is an <a href="http://mathworld.wolfram.com/IncompleteGammaFunction.html">incomplete gamma function</a> inverse which computes <em>x</em> given <em>a</em> and <em>p</em>.
</p>
<table border="0" summary="Tip">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Tip]" src="tip.png"></td>
<th align="left">Tip</th>
</tr>
<tr><td align="left" valign="top">
<p>
When people normally talk about the inverse of the incomplete gamma function, they are talking about inverting on parameter <em>x</em>.
These are implemented here as gamma_p_inv and gamma_q_inv, and are by far the most efficient of the inverses presented here.
</p>
<p>
The inverse on the <em>a</em> parameter finds use in some statistical applications but has to be computed by rather brute force
numerical techniques and is consequently several times slower. These are implemented here as GammaPInvA and GammeQInvA.
</p>
</td></tr>
</table>

End Rem
Function GammaPInvA:Double(a:Double, p:Double)
	Return bmx_boost_math_gammapinva(a, p)
End Function

Rem
bbdoc: Returns a value @a such that: q = GammaQ(a, x)
about: Requires: <em>a &gt; 0</em> and <em>1 &gt;= p,q &gt;= 0</em>.
<p>
This is an <a href="http://mathworld.wolfram.com/IncompleteGammaFunction.html">incomplete gamma function</a> inverse which computes <em>x</em> given <em>a</em> and <em>p</em>.
</p>
<table border="0" summary="Tip">
<tr>
<td rowspan="2" align="center" valign="top" width="25"><img alt="[Tip]" src="tip.png"></td>
<th align="left">Tip</th>
</tr>
<tr><td align="left" valign="top">
<p>
When people normally talk about the inverse of the incomplete gamma function, they are talking about inverting on parameter <em>x</em>.
These are implemented here as gamma_p_inv and gamma_q_inv, and are by far the most efficient of the inverses presented here.
</p>
<p>
The inverse on the <em>a</em> parameter finds use in some statistical applications but has to be computed by rather brute force
numerical techniques and is consequently several times slower. These are implemented here as GammaPInvA and GammeQInvA.
</p>
</td></tr>
</table>

End Rem
Function GammaQInvA:Double(a:Double, q:Double)
	Return bmx_boost_math_gammaqinva(a, q)
End Function

Rem
bbdoc: This function find some uses in statistical distributions.
about: It implements the partial derivative with respect to <em>x</em> of the incomplete gamma function.
<img src="derivative1.png">
<p>
Note that the derivative of the function GammaQ</a> can be obtained by negating the result of this function.
</p>
End Rem
Function GammaPDerivative:Double(a:Double, x:Double)
	Return bmx_boost_math_gammapderivative(a, x)
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
bbdoc: Calculates the quantile of @dist, for @p.
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
bbdoc: Calculates the inverse of the Complement of the Cumulative Distribution Function.
about: The function computes a value <em>x</em> such that <code>CdfComplement(dist, x)) == q</code> where <em>q</em> is complement of the
probability.
<p>
This function is also called the inverse survival function, and is the same as calculating the <em>upper critical value</em> of a
distribution.
</p>
<p>
This function throws a TDomainException if the probablity lies outside [0,1]. The function may throw a TOverflowException
if there is no finite value that has the specified probability.
</p>
<p>
The following graph show the inverse survival function for the normal distribution:
</p>
<p>
<img src="survival_inv.png" alt="survival_inv">
</p>
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
	Requires 0 &lt;= p &lt;= 1, and n &gt;= 0, otherwise throws TDomainException. 
	</p>
	End Rem
	Function CreateBinomial:TBinomialDistribution(n:Int, p:Double)
		Return New TBinomialDistribution.Create(n, p)
	End Function
	
	Rem
	bbdoc: Creates a new TBinomialDistribution instance.
	about: n is the total number of trials, p  is the probability of success of a single trial.
	<p>
	Requires 0 &lt;= p &lt;= 1, and n &gt;= 0, otherwise throws TDomainException. 
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

Rem
bbdoc: The noncentral beta distribution is a generalization of the Beta Distribution.
about: It is defined as the ratio X = &#967;<sub>m</sub><sup>2</sup>(&#955;) / (&#967;<sub>m</sub><sup>2</sup>(&#955;) + &#967;<sub>n</sub><sup>2</sup>) where &#967;<sub>m</sub><sup>2</sup>(&#955;) is a noncentral
&#967;<sup>2</sup> random variable with <em>m</em> degrees of freedom, and &#967;<sub>n</sub><sup>2</sup> is a central
&#967;<sup>2</sup> random variable with <em>n</em> degrees of freedom.
<p>
This gives a PDF that can be expressed as a Poisson mixture of beta distribution PDFs:
</p>
<p>
<img src="nc_beta_ref1.png">
</p>
<p>
where P(i;&#955;/2) is the discrete Poisson probablity at <em>i</em>, with mean &#955;/2, and I<sub>x</sub><sup>'</sup>(&#945;, &#946;) is
the derivative of the incomplete beta function. This leads to the usual form of the CDF as:
</p>
<p>
<img src="nc_beta_ref2.png">
</p>
<p>
The following graph illustrates how the distribution changes for different values of &#955;:
</p>
<p>
<img src="nc_beta_pdf.png" align="middle">
</p>
End Rem
Type TNonCentralBetaDistribution Extends TDistribution

	Rem
	bbdoc: Creates a noncentral beta distribution with shape parameters @alpha and @beta and non-centrality parameter @lambda.
	about: Requires a &gt; 0, b &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException.
	End Rem
	Function CreateNonCentralBeta:TNonCentralBetaDistribution(alpha:Double, beta:Double, lambda:Double)
		Return New TNonCentralBetaDistribution.Create(alpha, beta, lambda)
	End Function
	
	Rem
	bbdoc: Creates a noncentral beta distribution with shape parameters @alpha and @beta and non-centrality parameter @lambda.
	about: Requires a &gt; 0, b &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException.
	End Rem
	Method Create:TNonCentralBetaDistribution(alpha:Double, beta:Double, lambda:Double)
		objectPtr = bmx_boost_math_non_central_beta_distribution_create(alpha, beta, lambda)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @alpha from which this object was constructed.
	End Rem
	Method Alpha:Double()
		Return bmx_boost_math_non_central_beta_distribution_alpha(objectPtr)
	End Method

	Rem
	bbdoc: Returns the parameter @beta from which this object was constructed.
	End Rem
	Method Beta:Double()
		Return bmx_boost_math_non_central_beta_distribution_beta(objectPtr)
	End Method

	Rem
	bbdoc: Returns the parameter @lambda from which this object was constructed.
	End Rem
	Method NonCentrality:Double()
		Return bmx_boost_math_non_central_beta_distribution_noncentrality(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_non_central_beta_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_non_central_beta_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_non_central_beta_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_non_central_beta_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_non_central_beta_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_non_central_beta_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_non_central_beta_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_non_central_beta_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_non_central_beta_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_non_central_beta_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_non_central_beta_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_non_central_beta_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_non_central_beta_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_non_central_beta_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_non_central_beta_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_non_central_beta_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_non_central_beta_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type


Rem
bbdoc: The noncentral chi-squared distribution is a generalization of the Chi Squared Distribution.
about: If X<sub>i</sub> are &#957; independent, normally distributed random variables with means &#956;<sub>i</sub> and variances
&#963;<sub>i</sub><sup>2</sup>, then the random variable
<p>
<img src="nc_chi_squ_ref1.png">
</p>
<p>
is distributed according to the noncentral chi-squared distribution.
</p>
<p>
The noncentral chi-squared distribution has two parameters: &#957; which specifies the number of degrees of freedom (i.e. the number
of X<sub>i</sub>), and &#955; which is related to the mean of the random variables X<sub>i</sub> by:
</p>
<p>
<img src="nc_chi_squ_ref2.png">
</p>
<p>
(Note that some references define &#955; as one half of the above sum).
</p>
<p>
This leads to a PDF of:
</p>
<p>
<img src="nc_chi_squ_ref3.png">
</p>
<p>
where <em>f(x;k)</em> is the central chi-squared distribution PDF, and <em>I<sub>v</sub>(x)</em> is a modified Bessel function of
the first kind.
</p>
<p>
The following graph illustrates how the distribution changes for different values of &#955;:
</p>
<p>
<img src="nccs_pdf.png" align="middle">
</p>
End Rem
Type TNonCentralChiSquaredDistribution Extends TDistribution

	Rem
	bbdoc: Creates a Chi-Squared distribution with @v degrees of freedom and non-centrality parameter @lambda.
	about: Requires v &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException
	End Rem
	Function CreateNonCentralChiSquared:TNonCentralChiSquaredDistribution(v:Double, lambda:Double)
		Return New TNonCentralChiSquaredDistribution.Create(v, lambda)
	End Function
	
	Rem
	bbdoc: Creates a Chi-Squared distribution with @v degrees of freedom and non-centrality parameter @lambda.
	about: Requires v &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException
	End Rem
	Method Create:TNonCentralChiSquaredDistribution(v:Double, lambda:Double)
		objectPtr = bmx_boost_math_non_central_chi_squared_distribution_create(v, lambda)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @v from which this object was constructed.
	End Rem
	Method DegreesOfFreedom:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_degreesoffreedom(objectPtr)
	End Method

	Rem
	bbdoc: Returns the parameter @lambda from which this object was constructed.
	End Rem
	Method NonCentrality:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_noncentrality(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_non_central_chi_squared_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_non_central_chi_squared_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_non_central_chi_squared_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_non_central_chi_squared_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type


Rem
bbdoc: The noncentral F distribution is a generalization of the Fisher F Distribution.
about: It is defined as the ratio 
<pre>F = (X/v1) / (Y/v2)
</pre>
<p>
where X is a noncentral &#967;<sup>2</sup> random variable with <em>v1</em> degrees of freedom and non-centrality parameter
&#955;, and Y is a central &#967;<sup>2</sup> random variable with <em>v2</em> degrees of freedom.
</p>
<p>
This gives the following PDF:
</p>
<p>
<img src="nc_f_ref1.png">
</p>
<p>
where L<sub>a</sub><sup>b</sup>(c) is a generalised Laguerre polynomial and B(a,b) is the beta function, or
</p>
<p>
<img src="nc_f_ref2.png">
</p>
<p>
The following graph illustrates how the distribution changes for different values of &#955;:
</p>
<p>
<img src="nc_f_pdf.png" align="middle">
</p>
End Rem
Type TNonCentralFDistribution Extends TDistribution

	Rem
	bbdoc: Creates a non-central beta distribution with parameters @v1 and @v2 and non-centrality parameter @lambda.
	about: Requires v1 &gt; 0, v2 &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException.
	End Rem
	Function CreateNonCentralF:TNonCentralFDistribution(v1:Double, v2:Double, lambda:Double)
		Return New TNonCentralFDistribution.Create(v1, v2, lambda)
	End Function
	
	Rem
	bbdoc: Creates a non-central beta distribution with parameters @v1 and @v2 and non-centrality parameter @lambda.
	about: Requires v1 &gt; 0, v2 &gt; 0 and lambda &gt;= 0, otherwise throws TDomainException.
	End Rem
	Method Create:TNonCentralFDistribution(v1:Double, v2:Double, lambda:Double)
		objectPtr = bmx_boost_math_non_central_f_distribution_create(v1, v2, lambda)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @v1 from which this object was constructed.
	End Rem
	Method DegreesOfFreedom1:Double()
		Return bmx_boost_math_non_central_f_distribution_degreesoffreedom1(objectPtr)
	End Method

	Rem
	bbdoc: Returns the parameter @v2 from which this object was constructed.
	End Rem
	Method DegreesOfFreedom2:Double()
		Return bmx_boost_math_non_central_f_distribution_degreesoffreedom2(objectPtr)
	End Method

	Rem
	bbdoc: Returns the non-centrality parameter @lambda from which this object was constructed.
	End Rem
	Method NonCentrality:Double()
		Return bmx_boost_math_non_central_f_distribution_noncentrality(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_non_central_f_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_non_central_f_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_non_central_f_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_non_central_f_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_non_central_f_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_non_central_f_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_non_central_f_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_non_central_f_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_non_central_f_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_non_central_f_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_non_central_f_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_non_central_f_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_non_central_f_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_non_central_f_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_non_central_f_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_non_central_f_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_non_central_f_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type


Rem
bbdoc: The noncentral T distribution is a generalization of the Students t Distribution</a>.
about: Let X have a normal distribution with mean &#948; and variance 1, and let &#957; S<sup>2</sup> have a chi-squared distribution
with degrees of freedom &#957;. Assume that X and S<sup>2</sup> are independent. The distribution of t<sub>&#957;</sub>(&#948;)=X/S
is called a noncentral t distribution with degrees of freedom &#957; and noncentrality parameter &#948;.
</p>
<p>
This gives the following PDF:
</p>
<p>
<img src="nc_t_ref1.png">
</p>
<p>
where <sub>1</sub>F<sub>1</sub>(a;b;x) is a confluent hypergeometric function.
</p>
<p>
The following graph illustrates how the distribution changes for different values of &#948;:
</p>
<p>
<img src="nc_t_pdf.png" align="middle">
</p>
End Rem
Type TNonCentralTDistribution Extends TDistribution

	Rem
	bbdoc: Creates a non-central t distribution with degrees of freedom parameter @v and non-centrality parameter @delta.
	about: Requires v &gt; 0 and finite delta, otherwise throws TDomainException.
	End Rem
	Function CreateNonCentralT:TNonCentralTDistribution(v:Double, delta:Double)
		Return New TNonCentralTDistribution.Create(v, delta)
	End Function
	
	Rem
	bbdoc: Creates a non-central t distribution with degrees of freedom parameter @v and non-centrality parameter @delta.
	about: Requires v &gt; 0 and finite delta, otherwise throws TDomainException.
	End Rem
	Method Create:TNonCentralTDistribution(v:Double, delta:Double)
		objectPtr = bmx_boost_math_non_central_t_distribution_create(v, delta)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the parameter @v from which this object was constructed.
	End Rem
	Method DegreesOfFreedom:Double()
		Return bmx_boost_math_non_central_t_distribution_degreesoffreedom(objectPtr)
	End Method

	Rem
	bbdoc: Returns the non-centrality parameter @delta from which this object was constructed.
	End Rem
	Method NonCentrality:Double()
		Return bmx_boost_math_non_central_t_distribution_noncentrality(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_non_central_t_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_non_central_t_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_non_central_t_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_non_central_t_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_non_central_t_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_non_central_t_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_non_central_t_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_non_central_t_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_non_central_t_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_non_central_t_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_non_central_t_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_non_central_t_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_non_central_t_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_non_central_t_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_non_central_t_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_non_central_t_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_non_central_t_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Poisson_distribution">Poisson distribution</a> is a well-known statistical discrete distribution.
about: It expresses the probability of a number of events (or failures, arrivals, occurrences ...) occurring in a fixed period of
time, provided these events occur with a known mean rate &#955; (events/time), and are independent of the time since the last event.
<p>
The distribution was discovered by Simé on-Denis Poisson (1781 to 1840).
</p>
<p>
It has the Probability Mass Function:
</p>
<p>
<img src="poisson_ref1.png">
</p>
<p>
for k events, with an expected number of events &#955;.
</p>
<p>
The following graph illustrates how the PDF varies with the parameter &#955;:
</p>
<p>
<img src="poisson_pdf_1.png" align="middle">
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
The Poisson distribution is a discrete distribution: internally functions
like the <code>cdf</code> and <code>pdf</code> are treated "as
if" they are continuous functions, but in reality the results
returned from these functions only have meaning if an integer value
is provided for the random variate argument.
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
Type TPoissonDistribution Extends TDistribution

	Rem
	bbdoc: Creates a poisson distribution with mean @_mean.
	End Rem
	Function CreatePoisson:TPoissonDistribution(_mean:Double = 1)
		Return New TPoissonDistribution.Create(_mean)
	End Function
	
	Rem
	bbdoc: Creates a poisson distribution with mean @_mean.
	End Rem
	Method Create:TPoissonDistribution(_mean:Double = 1)
		objectPtr = bmx_boost_math_poisson_distribution_create(_mean)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the mean of this distribution.
	End Rem
	Method Mean:Double()
		Return bmx_boost_math_poisson_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_poisson_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_poisson_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_poisson_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_poisson_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_poisson_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_poisson_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_poisson_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_poisson_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_poisson_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_poisson_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_poisson_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_poisson_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_poisson_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_poisson_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_poisson_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_poisson_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Rayleigh_distribution">Rayleigh distribution</a> is a continuous distribution.
about: It has a <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function</a>:
<p>
f(x; sigma) = x * exp(-x<sup>2</sup>/2 &#963;<sup>2</sup>) / &#963;<sup>2</sup>
</p>
<p>
For sigma parameter &#963; &gt; 0, and x &gt; 0.
</p>
<p>
The Rayleigh distribution is often used where two orthogonal components have an absolute value, for example, wind velocity and direction may
be combined to yield a wind speed, or real and imaginary components may have absolute values that are Rayleigh distributed.
</p>
<p>
The following graph illustrates how the Probability density Function(pdf) varies with the shape parameter &#963;:
</p>
<p>
<img src="rayleigh_pdf.png" align="middle">
</p>
<p>
and the Cumulative Distribution Function (cdf)
</p>
<p>
<img src="rayleigh_cdf.png" align="middle">
</p>
End Rem
Type TRayleighDistribution Extends TDistribution

	Rem
	bbdoc: Constructs a <a href="http://en.wikipedia.org/wiki/Rayleigh_distribution">Rayleigh distribution</a> with &#963; <em>sigma</em>.
	about: Requires that the &#963; parameter is greater than zero, otherwise throws TDomainException.
	End Rem
	Function CreateRayleigh:TRayleighDistribution(sigma:Double = 1)
		Return New TRayleighDistribution.Create(sigma)
	End Function
	
	Rem
	bbdoc: Constructs a <a href="http://en.wikipedia.org/wiki/Rayleigh_distribution">Rayleigh distribution</a> with &#963; <em>sigma</em>.
	about: Requires that the &#963; parameter is greater than zero, otherwise throws TDomainException.
	End Rem
	Method Create:TRayleighDistribution(sigma:Double = 1)
		objectPtr = bmx_boost_math_rayleigh_distribution_create(sigma)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the @sigma parameter of this distribution.
	End Rem
	Method Sigma:Double()
		Return bmx_boost_math_rayleigh_distribution_sigma(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_rayleigh_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_rayleigh_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_rayleigh_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_rayleigh_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_rayleigh_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_rayleigh_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_rayleigh_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_rayleigh_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_rayleigh_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_rayleigh_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_rayleigh_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_rayleigh_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_rayleigh_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_rayleigh_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_rayleigh_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_rayleigh_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_rayleigh_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: A statistical distribution.
about: Published by William Gosset in 1908. His employer, Guinness Breweries, required him to publish under a pseudonym, so he
chose "Student". Given N independent measurements, let
<p>
<img src="students_t_dist.png">
</p>
<p>
where <em>M</em> is the population mean, <em>&#956;</em> is the sample mean, and <em>s</em> is the sample variance.
</p>
<p>
Student's t-distribution is defined as the distribution of the random variable t which is - very loosely - the "best" that we can
do not knowing the true standard deviation of the sample. It has the PDF:
</p>
<p>
<img src="students_t_ref1.png">
</p>
<p>
The Student's t-distribution takes a single parameter: the number of degrees of freedom of the sample. When the degrees of freedom
is <em>one</em> then this distribution is the same as the Cauchy-distribution. As the number of degrees of freedom tends towards
infinity, then this distribution approaches the normal-distribution. The following graph illustrates how the PDF varies with the degrees 
of freedom &#957;:
</p>
<p>
<img src="students_t_pdf.png" align="middle">
</p>

End Rem
Type TStudentsTDistribution Extends TDistribution

	Rem
	bbdoc: Creates a Student's t-distribution with @v degrees of freedom.
	about: Requires @v &gt; 0, otherwise throws TDomainException. Note that non-integral degrees of freedom are supported, and
	meaningful under certain circumstances.
	End Rem
	Function CreateStudentsT:TStudentsTDistribution(v:Double)
		Return New TStudentsTDistribution.Create(v)
	End Function
	
	Rem
	bbdoc: Creates a Student's t-distribution with @v degrees of freedom.
	about: Requires @v &gt; 0, otherwise throws TDomainException. Note that non-integral degrees of freedom are supported, and
	meaningful under certain circumstances.
	End Rem
	Method Create:TStudentsTDistribution(v:Double)
		objectPtr = bmx_boost_math_students_t_distribution_create(v)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the number of degrees of freedom of this distribution.
	End Rem
	Method DegreesOfFreedom:Double()
		Return bmx_boost_math_students_t_distribution_degreesoffreedom(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_students_t_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_students_t_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_students_t_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_students_t_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_students_t_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_students_t_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_students_t_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_students_t_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_students_t_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_students_t_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_students_t_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_students_t_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_students_t_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_students_t_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_students_t_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_students_t_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_students_t_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The <a href="http://en.wikipedia.org/wiki/Triangular_distribution">triangular distribution</a> is a <a href="http://en.wikipedia.org/wiki/Continuous_distribution">continuous</a> <a href="http://en.wikipedia.org/wiki/Probability_distribution">probability distribution</a> with a lower limit a, <a class="external" href="http://en.wikipedia.org/wiki/Mode_%28statistics%29">mode c</a>, and upper limit b.
about: The triangular distribution is often used where the distribution is only vaguely known, but, like the 
<a href="http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29">uniform distribution</a>, upper and limits are 'known', but
a 'best guess', the mode or center point, is also added. It has been recommended as a
<a href="http://www.worldscibooks.com/mathematics/etextbook/5720/5720_chap1.pdf">proxy for the beta distribution.</a> The distribution
is used in business decision making and project planning.
<p>
The <a href="http://en.wikipedia.org/wiki/Triangular_distribution">triangular distribution</a> is a distribution with the
<a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function</a>:
</p>
<p>
f(x) =
</p>
<ul type="disc">
<li>2(x-a)/(b-a) (c-a) for a &lt;= x &lt;= c</li>
<li>2(b-x)/(b-a)(b-c) for c &lt; x &lt;= b</li>
</ul>
<p>
Parameter a (lower) can be any finite value. Parameter b (upper) can be any finite value &gt; a (lower). Parameter c (mode) a &lt;= c &lt;=
b. This is the most probable value.
</p>
<p>
The <a href="http://en.wikipedia.org/wiki/Random_variate">random variate</a> x must also be finite, and is supported lower &lt;= x &lt;= upper.
</p>
<p>
The triangular distribution may be appropriate when an assumption of a normal distribution is unjustified because uncertainty is caused by
rounding and quantization from analog to digital conversion. Upper and lower limits are known, and the most probable value lies midway.
</p>
<p>
The distribution simplifies when the 'best guess' is either the lower or upper limit - a 90 degree angle triangle. The default chosen is the
001 triangular distribution which expresses an estimate that the lowest value is the most likely; for example, you believe that the next-day
quoted delivery date is most likely (knowing that a quicker delivery is impossible - the postman only comes once a day), and that longer delays
are decreasingly likely, and delivery is assumed to never take more than your upper limit.
</p>
<p>
The following graph illustrates how the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function PDF</a>
varies with the various parameters:
</p>
<p>
<img src="triangular_pdf.png" align="middle">
</p>
<p>
and cumulative distribution function
</p>
<p>
<img src="triangular_cdf.png" align="middle">
</p>

End Rem
Type TTriangularDistribution Extends TDistribution

	Rem
	bbdoc: Creates a triangular distribution with lower _lower and upper @_upper.
	about: Requires that the @_lower, @_mode and @_upper parameters are all finite, otherwise throws TDomainException.
	End Rem
	Function CreateTriangular:TTriangularDistribution(_lower:Double = -1, _mode:Double = 0, _upper:Double = 1)
		Return New TTriangularDistribution.Create(_lower, _mode, _upper)
	End Function
	
	Rem
	bbdoc: Creates a triangular distribution with lower _lower and upper @_upper.
	about: Requires that the @_lower, @_mode and @_upper parameters are all finite, otherwise throws TDomainException.
	End Rem
	Method Create:TTriangularDistribution(_lower:Double = -1, _mode:Double = 0, _upper:Double = 1)
		objectPtr = bmx_boost_math_triangular_distribution_create(_lower, _mode, _upper)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the lower parameter of this distribution (default -1).
	End Rem
	Method Lower:Double()
		Return bmx_boost_math_triangular_distribution_lower(objectPtr)
	End Method

	Rem
	bbdoc: Returns the upper parameter of this distribution (default+1).
	End Rem
	Method Upper:Double()
		Return bmx_boost_math_triangular_distribution_upper(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_triangular_distribution_mean(objectPtr)
	End Method

	Rem
	bbdoc: Returns the mode parameter of this distribution (default 0).
	End Rem
	Method Mode:Double()
		Return bmx_boost_math_triangular_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_triangular_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_triangular_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_triangular_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_triangular_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_triangular_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_triangular_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_triangular_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_triangular_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_triangular_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_triangular_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_triangular_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_triangular_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_triangular_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_triangular_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_triangular_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The uniform distribution, also known as a rectangular distribution, is a probability distribution that has constant probability.
about: The <a href="http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29">continuous uniform distribution</a> is a
distribution with the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function</a>:
<p>
f(x) =
</p>
<ul type="disc">
<li>1 / (upper - lower) for lower &lt; x &lt; upper</li>
<li>zero for x &lt; lower or x &gt; upper</li>
</ul>
<p>
and in this implementation:
</p>
<ul type="disc">
<li>1 / (upper - lower) for x = lower or x = upper</li>
</ul>
<p>
The choice of x = lower or x = upper is made because statistical use of this distribution judged is most likely: the method of maximum
likelihood uses this definition.
</p>
<p>
There is also a <a href="http://en.wikipedia.org/wiki/Discrete_uniform_distribution"><strong>discrete</strong> uniform distribution</a>.
</p>
<p>
Parameters lower and upper can be any finite value.
</p>
<p>
The <a href="http://en.wikipedia.org/wiki/Random_variate">random variate</a> x must also be finite, and is supported lower &lt;= x &lt;= upper.
</p>
<p>
The lower parameter is also called the <a href="http://www.itl.nist.gov/div898/handbook/eda/section3/eda364.htm">location parameter</a>,
<a href="http://en.wikipedia.org/wiki/Location_parameter">that is where the origin of a plot will lie</a>, and (upper - lower) is
also called the <a href="http://en.wikipedia.org/wiki/Scale_parameter">scale parameter</a>.
</p>
<p>
The following graph illustrates how the <a href="http://en.wikipedia.org/wiki/Probability_density_function">probability density function
PDF</a> varies with the shape parameter:
</p>
<p>
<img src="uniform_pdf.png" align="middle">
</p>
<p>
Likewise for the CDF:
</p>
<p>
<img src="uniform_cdf.png" align="middle">
</p>

End Rem
Type TUniformDistribution Extends TDistribution

	Rem
	bbdoc: Creates a uniform distribution with lower @_lower and upper @_upper.
	about: Requires that the lower and upper parameters are both finite; otherwise if infinity or NaN then throws TDomainException.
	End Rem
	Function CreateUniform:TUniformDistribution(_lower:Double = 0, _upper:Double = 1)
		Return New TUniformDistribution.Create(_lower, _upper)
	End Function
	
	Rem
	bbdoc: Creates a uniform distribution with lower @_lower and upper @_upper.
	about: Requires that the lower and upper parameters are both finite; otherwise if infinity or NaN then throws TDomainException.
	End Rem
	Method Create:TUniformDistribution(_lower:Double = 0, _upper:Double = 1)
		objectPtr = bmx_boost_math_uniform_distribution_create(_lower, _upper)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the lower parameter of this distribution.
	End Rem
	Method Lower:Double()
		Return bmx_boost_math_uniform_distribution_lower(objectPtr)
	End Method

	Rem
	bbdoc: Returns the upper parameter of this distribution.
	End Rem
	Method Upper:Double()
		Return bmx_boost_math_uniform_distribution_upper(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_uniform_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_uniform_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_uniform_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_uniform_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_uniform_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_uniform_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_uniform_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_uniform_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_uniform_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_uniform_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_uniform_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_uniform_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_uniform_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_uniform_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_uniform_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_uniform_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_uniform_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The gamma distribution is a continuous probability distribution.
about: When the shape parameter is an integer then it is known as the Erlang Distribution. It is also closely related to the Poisson and
Chi Squared Distributions.
<p>
When the shape parameter has an integer value, the distribution is the <a href="http://en.wikipedia.org/wiki/Erlang_distribution">Erlang
distribution</a>. Since this can be produced by ensuring that the shape parameter has an integer value &gt; 0, the Erlang distribution
is not separately implemented.
</p>
<p>
For shape parameter <em>k</em> and scale parameter &#952; it is defined by the probability density function:
</p>
<p>
<img src="gamma_dist_ref1.png">
</p>
<p>
Sometimes an alternative formulation is used: given parameters &#945;= k and &#946;= 1 / &#952;, then the distribution can be defined
by the PDF:
</p>
<p>
<img src="gamma_dist_ref2.png">
</p>
<p>
In this form the inverse scale parameter is called a <em>rate parameter</em>.
</p>
<p>
Both forms are in common usage: this library uses the first definition throughout. Therefore to construct a Gamma Distribution from
a <em>rate parameter</em>, you should pass the reciprocal of the rate as the scale parameter.
</p>
<p>
The following two graphs illustrate how the PDF of the gamma distribution varies as the parameters vary:
</p>
<p>
<img src="gamma1_pdf.png" align="middle">
</p>
<p>
<img src="gamma2_pdf.png" align="middle">
</p>
<p>
The <strong>Erlang Distribution</strong> is the same as the Gamma, but with the shape parameter an integer. It is often expressed
using a <em>rate</em> rather than a <em>scale</em> as the second parameter (remember that the rate is the reciprocal of the scale).
</p>
<p>
Internally the functions used to implement the Gamma Distribution are already optimised for small-integer arguments, so in general there should
be no great loss of performance from using a Gamma Distribution rather than a dedicated Erlang Distribution.
</p>

End Rem
Type TGammaDistribution Extends TDistribution

	Rem
	bbdoc: Constructs a gamma distribution with shape @shape and scale @scale.
	about: Requires that the shape and scale parameters are greater than zero, otherwise throws TDomainException.
	End Rem
	Function CreateGamma:TGammaDistribution(shape:Double, scale:Double = 1)
		Return New TGammaDistribution.Create(shape, scale)
	End Function
	
	Rem
	bbdoc: Constructs a gamma distribution with shape @shape and scale @scale.
	about: Requires that the shape and scale parameters are greater than zero, otherwise throws TDomainException.
	End Rem
	Method Create:TGammaDistribution(shape:Double, scale:Double = 1)
		objectPtr = bmx_boost_math_gamma_distribution_create(shape, scale)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the @shape parameter of this distribution.
	End Rem
	Method Shape:Double()
		Return bmx_boost_math_gamma_distribution_shape(objectPtr)
	End Method

	Rem
	bbdoc: Returns the @scale parameter of this distribution.
	End Rem
	Method Scale:Double()
		Return bmx_boost_math_gamma_distribution_scale(objectPtr)
	End Method

	Method Mean:Double()
		Return bmx_boost_math_gamma_distribution_mean(objectPtr)
	End Method

	Method Mode:Double()
		Return bmx_boost_math_gamma_distribution_mode(objectPtr)
	End Method
	
	Method StandardDeviation:Double()
		Return bmx_boost_math_gamma_distribution_standarddeviation(objectPtr)
	End Method

	Method Skewness:Double()
		Return bmx_boost_math_gamma_distribution_skewness(objectPtr)
	End Method

	Method Pdf:Double(k:Double)
		Return bmx_boost_math_gamma_distribution_pdf(objectPtr, k)
	End Method
	
	Method Cdf:Double(k:Double)
		Return bmx_boost_math_gamma_distribution_cdf(objectPtr, k)
	End Method

	Method CdfComplement:Double(k:Double)
		Return bmx_boost_math_gamma_distribution_cdfcomplement(objectPtr, k)
	End Method
	
	Method Quantile:Double(p:Double)
		Return bmx_boost_math_gamma_distribution_quantile(objectPtr, p)
	End Method

	Method QuantileComplement:Double(p:Double)
		Return bmx_boost_math_gamma_distribution_quantilecomplement(objectPtr, p)
	End Method

	Method Hazard:Double(x:Double)
		Return bmx_boost_math_gamma_distribution_hazard(objectPtr, x)
	End Method

	Method Chf:Double(x:Double)
		Return bmx_boost_math_gamma_distribution_chf(objectPtr, x)
	End Method

	Method Median:Double()
		Return bmx_boost_math_gamma_distribution_median(objectPtr)
	End Method

	Method Range(rangeStart:Double Var, rangeEnd:Double Var)
		bmx_boost_math_gamma_distribution_range(objectPtr, Varptr rangeStart, Varptr rangeEnd)
	End Method

	Method Variance:Double()
		Return bmx_boost_math_gamma_distribution_variance(objectPtr)
	End Method

	Method Kurtosis:Double()
		Return bmx_boost_math_gamma_distribution_kurtosis(objectPtr)
	End Method

	Method KurtosisExcess:Double()
		Return bmx_boost_math_gamma_distribution_kurtosisexcess(objectPtr)
	End Method

	Method Delete()
		If objectPtr Then
			bmx_boost_math_gamma_distribution_free(objectPtr)
			objectPtr = Null
		End If
	End Method

End Type







