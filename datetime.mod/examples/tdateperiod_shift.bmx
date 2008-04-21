SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local startDate:TDate = TDate.Create(2007, Feb, 1)
Local endDate:TDate = TDate.Create(2007, Mar, 1)

Local p:TDatePeriod = TDatePeriod.Create(startDate, endDate)

Print p.toString()

p.shift(15)

Print p.toString()
