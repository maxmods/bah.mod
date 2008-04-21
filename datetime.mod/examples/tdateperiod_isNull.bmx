SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2007, 2, 1), TDate.Create(2007, 3, 1))
Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2007, 2, 1), TDate.Create(2007, 1, 1))

Print p1.isNull()
Print p2.isNull()
