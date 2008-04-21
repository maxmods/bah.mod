SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d:TDate = TDate.Create(2002, Jan, 1)
Local t1:TTime = TTime.Create(d, TDMinutes(5))
Local t2:TTime = TTime.Create(d, TDMinutes(2))

Print t2.subtract(t1).toString() ' negative result
