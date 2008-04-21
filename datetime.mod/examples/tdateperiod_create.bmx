SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local startDate:TDate = TDate.Create(2007, 1, 1)
Local endDate:TDate = TDate.localDay()

Local p:TDatePeriod = TDatePeriod.Create(startDate, endDate)

Print p.toString()
