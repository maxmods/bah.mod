SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d:TDate = TDate.Create(2002, Jan, 1)
Local t:TTime = TTime.Create(d, TDHours(14))

Print t.toString()
