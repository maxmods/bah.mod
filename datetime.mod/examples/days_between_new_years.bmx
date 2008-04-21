SuperStrict

' calculates the number of days till new years.
' Provides a simple example of using a date generator, and simple
' mathematical operations, to calculate the days since 
' New Years day of this year, and days until next New Years day.

Framework BaH.DateTime
Import BRL.StandardIO


Local today:TDate = TDate.localDay()

Local newYearsDay:TPartialDate = TPartialDate.Create(1, Jan)

' Subtract two dates to get a duration
Local daysSinceYearStart:Int = today.subtractDate(newYearsDay.getDate(today.year()))

Print "Days since Jan 1: " + daysSinceYearStart

Local daysUntilYearStart:Int = newYearsDay.getDate(today.year() + 1).subtractDate(today)

Print "Days until next Jan 1: " + daysUntilYearStart

