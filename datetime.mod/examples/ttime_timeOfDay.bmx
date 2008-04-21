SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d:TDate = TDate.Create(2002, Jan, 10)
Local t:TTime = TTime.Create(d, TDHours(1))

Print t.timeOfDay().toString()
