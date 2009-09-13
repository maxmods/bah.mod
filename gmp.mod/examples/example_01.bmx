SuperStrict

Framework BaH.GMP
Import BRL.StandardIO

Local num:GMPInt = GMPInt.Init()

num.SetInt(100)

Print num.GetStr()

Local neg:GMPInt = num.Neg()

Print neg.GetStr()

Local zero:GMPInt = num.Add(neg)

Print zero.GetStr()
