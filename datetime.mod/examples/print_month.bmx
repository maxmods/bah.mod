SuperStrict

' This example prints all the dates in a month. It demonstrates
' the use of iterators as well as various date functions

Framework BaH.DateTime
Import BRL.StandardIO


Local y:Int = Input("Enter Year: ").toInt()
Local m:Int = Min(12, Max(1, Input("Enter Month: ").toInt()))

' Use the calendar to get the last day of the month
Local eomDay:Int = EndOfMonthDay(y, m)

Local endOfMonth:TDate = TDate.Create(y, m, eomDay)

' construct an iterator starting with first day of the month
Local ditr:TDateDayIterator = TDateDayIterator.CreateIterator(TDate.Create(y, m, 1))

' loop thru the days and print each one
While Not ditr.isAfter(endOfMonth)

	Print ditr.toString() + " [" + WeekDay(ditr.dayOfWeek()) + "]"
	
	ditr.forward()

Wend 
