SuperStrict


Framework BaH.DateTime
Import BRL.StandardIO
'DebugStop
Local german:TDateFacet = TDateFacet.CreateForLocale("de_DE")
SetCurrentDateFacet(german)

' this is today
Local today:TDate = TDate.localDay()

' the date in parts
Print "day   = " + today.day()
Print "month = " + today.month()
Print "year  = " + today.year()

Print today.format("%x")

Print "~n"

' now for a date period / range
Local date1:TDate = TDate.Create(2007, 1, 1)
Local date2:TDate = TDate.Create(2007, 3, 1)

Print date1.toString()
Print date2.toString()

' the period
Local period:TDatePeriod = TDatePeriod.Create(date1, date2)

' show the period
Print period.toString()
' the number of days in the period
Print "length = " + period.length() + " days"

' shift the period forward 1 day
period.shift(1)
Print "Shifted : " + period.toString()

Print "~n"

' create a day iterator for today
Local dayIter:TDateDayIterator = today.dayIterator()

Print dayIter.toString()

dayIter.forward()
dayIter.forward()

Print dayIter.toString() + " : + 2 days"

dayIter.backward()
dayIter.backward()
dayIter.backward()
dayIter.backward()

Print dayIter.toString() + " : - 2 days"

Print "~n"

' month iterator with iterator set to step every 3 months
Local monthIter:TDateMonthIterator = today.monthIterator(3)

Print monthIter.toString()

monthIter.forward()
monthIter.forward()

Print monthIter.toString() + " : + 6 months"

monthIter.backward()
monthIter.backward()
monthIter.backward()
monthIter.backward()

Print monthIter.toString() + " : - 6 months"
