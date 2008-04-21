SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local startDate:TDate = TDate.localDay()

Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 100)

Print p.last().toString()
