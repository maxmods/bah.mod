SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d:TDate = TDate.Create(2002, Jan, 1)
Local t:TTime = TTime.Create(d, TDMinutes(5))

Local t2:TTime = t.addDuration(TDHours(1).add(TDMinutes(2)))

Print t.toString()
Print t2.toString()

