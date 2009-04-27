Rem
Simple example of computing probabilities and quantiles for a binomial random variable
representing the correct guesses on a multiple-choice test.

source http:'www.stat.wvu.edu/SRS/Modules/Binomial/test.html

A multiple choice test has four possible answers to each of 16 questions.
A student guesses the answer to each question,
so the probability of getting a correct answer on any given question is
one in four, a quarter, 1/4, 25% or fraction 0.25.
The conditions of the binomial experiment are assumed to be met:
n = 16 questions constitute the trials;
each question results in one of two possible outcomes (correct or incorrect);
the probability of being correct is 0.25 and is constant if no knowledge about the subject is assumed;
the questions are answered independently if the student's answer to a question
in no way influences his/her answer to another question.

First, we need to be able to use the binomial distribution constructor
(and some std input/output, of course).
End Rem
SuperStrict

Framework BaH.MathToolkit
Import BRL.StandardIO
Import BRL.Math

Import "example_helper.bmx"


Print "~nBinomial distribution example - guessing in a quiz."

Try

	' The number of correct answers, X, is distributed as a binomial random variable
	' with binomial distribution parameters: questions n = 16 and success fraction probability p = 0.25.
	' So we construct a binomial distribution:

	Local questions:Int = 16 ' All the questions in the quiz.
	Local answers:Int = 4 ' Possible answers to each question.
	Local success_fraction:Double = answers / Double(questions) ' If a random guess.
	' Caution:  = answers / questions would be zero (because they are integers)!
	Local quiz:TBinomialDistribution = New TBinomialDistribution.Create(questions, success_fraction)
	
	' and display the distribution parameters we used thus:
	
	Print "In a quiz with " + quiz.Trials() +  " questions and with a probability of guessing right of " + ..
		nice(quiz.SuccessFraction() * 100) + " %" + " or 1 in " + Int(1.0 / quiz.SuccessFraction())

	' Show a few probabilities of just guessing:

	Print "Probability of getting none right is " + nice(Pdf(quiz, 0)) ' 0.010023
	Print "Probability of getting exactly one right is " + nice(Pdf(quiz, 1))
	Print "Probability of getting exactly two right is " + nice(Pdf(quiz, 2))
	Local pass_score:Int = 11
	Print "Probability of getting exactly " + pass_score + " answers right by chance is "  + nice(Pdf(quiz, questions))	


	' > Probability of getting none right is 0.0100226
	' > Probability of getting exactly one right is 0.0534538
	' > Probability of getting exactly two right is 0.133635
	' > Probability of getting exactly 11 answers right by chance is 2.32831e-010
	' 
	' These don't give any encouragement to guessers!
	' 
	' We can tabulate the 'getting exactly right' ( == ) probabilities thus:

	Print "~nGuessed Probability"
	For Local successes:Int = 0 To questions
	    Local probability:Double = Pdf(quiz, successes)
	    Print successes + "      " + nice(probability)
	Next
	Print ""
	
	' > Guessed Probability
	' >  0      0.0100226
	' >  1      0.0534538
	' >  2      0.133635
	' >  3      0.207876
	' >  4      0.225199
	' >  5      0.180159
	' >  6      0.110097
	' >  7      0.0524273
	' >  8      0.0196602
	' >  9      0.00582526
	' > 10      0.00135923
	' > 11      0.000247132
	' > 12      3.43239e-005
	' > 13      3.5204e-006
	' > 14      2.51457e-007
	' > 15      1.11759e-008
	' > 16      2.32831e-010
	'
	' Then we can add the probabilities of some 'exactly right' like this: 
	
	Print "Probability of getting none or one right is " + nice(Pdf(quiz, 0) + Pdf(quiz, 1))

	' > Probability of getting none or one right is 0.0634764
	' 
	' But if more than a couple of scores are involved, it is more convenient (and may be more accurate)
	' to use the Cumulative Distribution Function (cdf) instead:

	Print "Probability of getting none or one right is " + nice(Cdf(quiz, 1))
	
	' > Probability of getting none or one right is 0.0634764
	' 
	' Since the cdf is inclusive, we can get the probability of getting up to 10 right ( <= ) 
	
	Print "Probability of getting <= 10 right (to fail) is " + nice(Cdf(quiz, 10))
		
	' > Probability of getting <= 10 right (to fail) is 0.999715
	' 
	' To get the probability of getting 11 or more right (to pass),
	' it is tempting to use ``1 - cdf(quiz, 10)`` to get the probability of > 10
	
	Print "Probability of getting > 10 right (to pass) is " + nice(1 - Cdf(quiz, 10))
	
	' > Probability of getting > 10 right (to pass) is 0.000285239
	' 
	' But this should be resisted in favor of using the complement function.
	
	Print "Probability of getting > 10 right (to pass) is " + nice(CdfComplement(quiz, 10))

	' > Probability of getting > 10 right (to pass) is 0.000285239
	' 
	' And we can check that these two, <= 10 and > 10,  add up to unity.
	
	If (Cdf(quiz, 10) + CdfComplement(quiz, 10)) <> 1.0 Then
		Print "NO UNITY!! (quiz)"
	End If
	
	' if we want a < rather than a <= test, because the CDF is inclusive, we must subtract one from the score.
	
	Print "Probability of getting less than " + pass_score + " (< " + pass_score + ..
		") answers right by guessing is " + nice(Cdf(quiz, pass_score -1))
	
	' > Probability of getting less than 11 (< 11) answers right by guessing is 0.999715
	' 
	' and similarly to get a >= rather than a > test
	' we also need to subtract one from the score (and can again check the sum is unity).
	' This is because if the cdf is /inclusive/, 
	' then its complement must be /exclusive/ otherwise there would be one possible
	' outcome counted twice!
	
	Print "Probability of getting at least " + pass_score + "(>= " + pass_score + ..
		") answers right by guessing is " + nice(CdfComplement(quiz, pass_score-1)) + ..
		", only 1 in " + nice(1/CdfComplement(quiz, pass_score-1))
	
	If (Cdf(quiz, pass_score -1) + CdfComplement(quiz, pass_score-1)) <> 1 Then
		Print "NO UNITY!! (pass_score)"
	End If
	
	' > Probability of getting at least 11 (>= 11) answers right by guessing is 0.000285239, only 1 in 3505.83
	' 
	' Finally we can tabulate some probabilities:
	
	SetPrecision(10)
	
	Print "~nAt most (<=)~nGuessed OK   Probability"
	For Local score:Int = 0 To questions
		Print score + "           " + nice(Cdf(quiz, score))
	Next
	Print ""
	
	' At most (<=)
	' Guessed OK   Probability
	'  0           0.01002259576
	'  1           0.0634764398
	'  2           0.1971110499
	'  3           0.4049871101
	'  4           0.6301861752
	'  5           0.8103454274
	'  6           0.9204427481
	'  7           0.9728700437
	'  8           0.9925302796
	'  9           0.9983555346
	' 10           0.9997147608
	' 11           0.9999618928
	' 12           0.9999962167
	' 13           0.9999997371
	' 14           0.9999999886
	' 15           0.9999999998
	' 16           1
	
	Print "~nAt least (>)~nGuessed OK   Probability"
	For Local score:Int = 0 To questions
		Print score + "           "  + nice(CdfComplement(quiz, score))
	Next
	
	' At least (>)
	' Guessed OK   Probability
	'  0           0.9899774042
	'  1           0.9365235602
	'  2           0.8028889501
	'  3           0.5950128899
	'  4           0.3698138248
	'  5           0.1896545726
	'  6           0.07955725188
	'  7           0.02712995629
	'  8           0.00746972044
	'  9           0.001644465374
	' 10           0.0002852391917
	' 11           3.810715862e-005
	' 12           3.783265129e-006
	' 13           2.628657967e-007
	' 14           1.140870154e-008
	' 15           2.328306437e-010
	' 16           0
	
	' We now consider the probabilities of *ranges* of correct guesses.
	' 
	' First, calculate the probability of getting a range of guesses right,
	' by adding the exact probabilities of each from low ... high.
	
	SetPrecision(4)
	
	Local low:Int = 3 ' Getting at least 3 Right.
	Local high:Int = 5 ' Getting as most 5 Right.
	Local sum:Double = 0.0
	For Local i:Int = low To high
		sum :+ Pdf(quiz, i)
	Next

	Print "Probability of getting between " + low + " and " + high + " answers right by guessing is " + nice(sum) ' 0.61323
	
	' > Probability of getting between 3 and 5 answers right by guessing is 0.6132
	' 
	' Or, usually better, we can use the difference of cdfs instead:
	
	Print "Probability of getting between " + low + " and " + high + " answers right by guessing is " + ..
		nice(Cdf(quiz, high) - Cdf(quiz, low - 1)) ' 0.61323
	
	' > Probability of getting between 3 and 5 answers right by guessing is 0.6132
	' 
	' And we can also try a few more combinations of high and low choices:
	
	low = 1
	high = 6
	Print "Probability of getting between " + low + " and " + high + " answers right by guessing is " + ..
		nice(Cdf(quiz, high) - Cdf(quiz, low - 1)) ' 1 And 6 P= 0.91042
	low = 1
	high = 8
	Print "Probability of getting between " + low + " and " + high + " answers right by guessing is " + ..
		nice(Cdf(quiz, high) - Cdf(quiz, low - 1)) ' 1 <= x 8 P = 0.9825
	low = 4
	high = 4
	Print "Probability of getting between " + low + " and " + high + " answers right by guessing is " + ..
		nice(Cdf(quiz, high) - Cdf(quiz, low - 1)) ' 4 <= x 4 P = 0.22520
	






Catch e:Object

	Print e.ToString()
	
End Try


