Rem
An example of a Bernoulli process is coin flipping. (see http://en.wikipedia.org/wiki/Bernoulli_process)
A variable in such a sequence may be called a Bernoulli variable.

This example shows using the Binomial distribution to predict the probability of heads and
tails when throwing a coin.

The number of correct answers (say heads), X, is distributed as a binomial random variable
with binomial distribution parameters number of trials (flips) n = 10 and probability
(successFraction) of getting a head p = 0.5 (a 'fair' coin).

(Our coin is assumed fair, but we could easily change the successFraction parameter p
from 0.5 to some other value to simulate an unfair coin,
say 0.6 for one with chewing gum on the tail,
so it is more likely to fall tails down and heads up).

End Rem
SuperStrict

Framework BaH.MathToolkit
Import BRL.StandardIO
Import BRL.Math

Import "example_helper.bmx"

SetPrecision(4)

Print "~nUsing Binomial distribution to predict how many heads and tails."

Try

	
	' First, construct a binomial distribution with parameters success_fraction
	' 1/2, and how many flips.

	Const successFraction:Double = 0.5 ' = 50% = 1/2 for a 'fair' coin.
	Local flips:Int = 10
	Local coinFlip:TBinomialDistribution = New TBinomialDistribution.Create(flips, successFraction)
	
	' Then some examples of using Binomial moments (and echoing the parameters).

	Print "From " + flips + " one can expect to get on average " + nice(Mean(coinFlip)) + " heads (or tails)."
	Print "Mode is " + nice(Mode(coinFlip))
	Print "Standard deviation is " + nice(StandardDeviation(coinFlip))
	Print "So about 2/3 will lie within 1 standard deviation and get between " + ..
		nice(Ceil(Mean(coinFlip) - StandardDeviation(coinFlip)))  + " and " + ..
		nice(Floor(Mean(coinFlip) + StandardDeviation(coinFlip))) + " correct."
	Print "Skewness is " + nice(Skewness(coinFlip))
	' Skewness of binomial distributions is only zero (symmetrical)
	' if success_fraction is exactly one half,
	' for example, when flipping 'fair' coins.
	Print "Skewness if successFraction is " + nice(coinFlip.successFraction()) + ..
		" is " + nice(Skewness(coinFlip)) + "~n" ' Expect zero for a 'fair' coin.

	' Now we show a variety of predictions on the probability of heads:

	Print "For " + coinFlip.trials() + " coin flips: "
	Print "Probability of getting no heads is " + nice(Pdf(coinFlip, 0))
	Print "Probability of getting at least one head is " + nice(1 - Pdf(coinFlip, 0))
	
	' When we want to calculate the probability for a range or values we can sum the PDF's:

	Print "Probability of getting 0 or 1 heads is " + ..
		nice(Pdf(coinFlip, 0) + Pdf(coinFlip, 1)) ' sum of exactly == probabilities

	' Or we can use the cdf.

	Print "Probability of getting 0 or 1 (<= 1) heads is " + nice(Cdf(coinFlip, 1))
	Print "Probability of getting 9 or 10 heads is " + nice(Pdf(coinFlip, 9) + Pdf(coinFlip, 10))
	

	' Note that using
	
	Print "Probability of getting 9 or 10 heads is " + nice(1.0 - Cdf(coinFlip, 8))
	
	' is less accurate than using the complement
	
	Print "Probability of getting 9 or 10 heads is " + nice(CdfComplement(coinFlip, 8))
	
	' Since the subtraction may involve
	' http://docs.sun.com/source/806-3568/ncg_goldberg.html cancellation error,
	' where as `CdfComplement(coinFlip, 8)`
	' does not use such a subtraction internally, and so does not exhibit the problem.
	
	' To get the probability for a range of heads, we can either add the pdfs for each number of heads
	
	Print "Probability of between 4 and 6 heads (4 or 5 or 6) is " + ..
		nice(Pdf(coinFlip, 4) + Pdf(coinFlip, 5) + Pdf(coinFlip, 6))
	
	' But this is probably less efficient than using the cdf
	
	Print "Probability of between 4 and 6 heads (4 or 5 or 6) is " + ..
		nice(Cdf(coinFlip, 6) - Cdf(coinFlip, 3))
	
	' Certainly for a bigger range like, 3 to 7
	
	Print "Probability of between 3 and 7 heads (3, 4, 5, 6 or 7) is " + ..
		nice(Cdf(coinFlip, 7) - Cdf(coinFlip, 2))
	Print ""
	
	' Finally, print two tables of probability for the /exactly/ and /at least/ a number of heads.

	' Print a table of probability for the exactly a number of heads.
	Print "Probability of getting exactly (==) heads"
	For Local successes:Int = 0 To flips
		' Say success means getting a head (or equally success means getting a tail).
		Local probability:Double = Pdf(coinFlip, successes)
		Print successes + "     " + nice(probability) + " or 1 in " + nice(1 / probability) + ..
			", or " + nice(probability * 100.0) + "%"
	Next
    	Print ""

	' Tabulate the probability of getting between zero heads and 0 upto 10 heads.
	Print "Probability of getting upto (<=) heads"
	For Local successes:Int = 0 To flips
		' Say success means getting a head
		' (equally success could mean getting a tail).
		Local probability:Double = Cdf(coinFlip, successes) ' P(X <= heads)
		Print successes + "        " + nice(probability) + " or 1 in " + nice(1.0 / probability) + ", or " + ..
			nice(probability * 100.0) + "%"
	Next

Catch e:Object

	Print e.ToString()
	
End Try


