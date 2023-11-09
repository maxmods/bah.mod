SuperStrict

Framework BaH.BigInt
Import BRL.StandardIO

'create a
Local bigUnsigned:TBigUnsigned = New TBigUnsigned("100000000000000000000000000000000000000000000000000000000")

Print "100 + 500 = " + new TBigUnsigned(100).Add( new TBigUnsigned(500) ).ToString()
Print "500 : 10 = " + new TBigUnsigned(500).Divide(new TBigUnsigned(10)).ToString()


'now with signs (allows negative numbers)
Print "-100 + 500 = " + new TBigInteger(-100).Add( new TBigInteger(500) ).ToString()
Print "500 : 10 = " + new TBigInteger(500).Divide(new TBigInteger(10)).ToString()
Print "5 : 2 = 2 remaining " + new TBigInteger(5).Modulo(new TBigInteger(2)).ToString()
Print "500 * 20 = " + new TBigInteger(500).Multiply(new TBigInteger(20)).ToString()

local big1:TBigInteger = new TBigInteger(500)
'do something with it
big1.Add(new TBigInteger(20))
print big1.ToString()

'keep big1 intact ()
Local big2:TBigInteger = big1.CreateAddResult(new TBigInteger(20))
print big1.ToString()
print big2.ToString()


'checkout overloaded operators
Local op1:TBigInteger = new TBigInteger(100)
Local op2:TBigInteger = new TBigInteger(50)
Local op3:TBigInteger = op1 + op2
print op3.ToString()
op1 :+ op2 'op1 now 150
print ((op1 + op2).ToString())

'modulo?
print ((new TBigInteger(50) mod new TBigInteger(8)).ToString())

'bigger?
if new TBigInteger(50) > new TBigInteger(5)
	print "50 > 5"
else
	print "50 < 5"
endif

'same value?
if new TBigInteger(50) = new TBigInteger(50)
	print "50 = 50"
else
	print "50 <> 50"
endif