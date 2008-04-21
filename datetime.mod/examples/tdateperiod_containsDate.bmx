SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))

Local goodDate:TDate = TDate.Create(2006, 6, 1)
Local badDate:TDate = TDate.create(2005, 1, 1)

Print p.containsDate(goodDate)
Print p.containsDate(badDate)
