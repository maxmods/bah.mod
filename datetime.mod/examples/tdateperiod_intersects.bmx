SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))

Local pGood:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))
Local pBad:TDatePeriod = TDatePeriod.Create(TDate.Create(2005, 1, 1), TDate.Create(2005, 12, 1))

Print p.intersects(pGood)
Print p.intersects(pBad)
