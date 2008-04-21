SuperStrict

Framework BaH.DateTime
Import BaH.Locale
Import BRL.StandardIO

Import "locale_plugin.bmx"

' create a custom date facet, for which we will localize the text.
Local df:TDateFacet = TDateFacet.Create()
SetCurrentDateFacet(df)

' hungarian locale ;-)
SetCurrentLocale("hu")

' localize the texts
df.setShortMonthNames(GetShortMonths())
df.setLongMonthNames(GetLongMonths())
df.setShortWeekdayNames(GetShortDaysOfWeek())
df.setLongWeekdayNames(GetLongDaysOfWeek())

' long weekdays
df.weekdayFormat("%A")

' date format (long months)
df.format("%Y-%B-%d")

' start date
Local start:TDate = TDate.Create(2007, 1, 1)

' a month iterator with one month increments
Local dateIterator:TDateIterator = TDateMonthIterator.CreateIterator(start, 1)

' loop through 12 months...
For Local i:Int = 0 Until 12
	
	Print dateIterator.toString() + " ... " + dateIterator.weekDay()
	
	dateIterator.forward()

Next


