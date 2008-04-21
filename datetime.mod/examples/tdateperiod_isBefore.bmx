SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))

Print p.isBefore(TDate.Create(2000, 1, 1))
Print p.isBefore(TDate.Create(2008, 1, 1))
