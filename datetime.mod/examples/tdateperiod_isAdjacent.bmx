SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))
Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 10, 1))

Local p3:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 9, 1)) ' a gap here!

Print p1.isAdjacent(p2)
Print p1.isAdjacent(p3)
