SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))

Local pGood:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 4, 4), TDate.Create(2006, 5, 5))
Local pBad:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))

Print p.contains(pGood)
Print p.contains(pBad)
