SuperStrict

Framework BaH.GMP
Import BRL.StandardIO

Local num:GMPInt = GMPInt.Init()

num.SetInt(100)

Print "num = " + num.GetStr()

Local neg:GMPInt = num.Neg()

Print "neg = " + neg.GetStr()

Local zero:GMPInt = num.Add(neg)

Print "num + neg = " + zero.GetStr()

' ---------

Local ot:GMPRational = GMPRational.Init()
ot.SetDM(1, 3)

Local tq:GMPRational = GMPRational.Init()
tq.SetDM(3, 4)

Local mul:GMPRational = ot.Mul(tq)

Print "~n1/3 * 3/4 = " + mul.GetStr()
