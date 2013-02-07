' Copyright (c) 2007-2013 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Date Time
End Rem
Module BaH.DateTime

ModuleInfo "Version: 1.03"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2007-2013 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.03"
ModuleInfo "History: Updated to boost 1.52"
ModuleInfo "History: Added TTimeFacet, and format methods for TTime and TTimeDuration."
ModuleInfo "History: Reworked format functionality to accept facets."
ModuleInfo "History: Rewrote glue String handling."
ModuleInfo "History: New BaH.BoostLocale requirement - for much improved localised date/time support."
ModuleInfo "History: 1.02"
ModuleInfo "History: Updated to boost 1.42"
ModuleInfo "History: 1.01"
ModuleInfo "History: Updated to boost 1.38"
ModuleInfo "History: Improved exception handling."
ModuleInfo "History: Fixed format() method for iterators."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions"

Import BaH.BoostLocale

Import "source.bmx"


Rem
bbdoc: The primary interface for date programming.
about: In general, the date class is immutable once constructed although it does allow assignment
from another date.
End Rem
Type TDate

	Field datePtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new #TDate.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(1984, 4, 23)
	
	Print d.toString()
	
	Print TDate.Create(1990, Mar, 12).toString()
	</pre>
	<a href="../examples/tdate_create.bmx">Example source</a>
	End Rem
	Function Create:TDate(year:Int, Month:Int, day:Int)
		Return _create(bmx_datetime_newdate(year, Month, day))
	End Function
	
	Function _create:TDate(datePtr:Byte Ptr)
		If datePtr Then
			Local this:TDate = New TDate
			
			this.datePtr = datePtr
			
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Creates a new #TDate for the current local time.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.toString()
	</pre>
	<a href="../examples/tdate_localDay.bmx">Example source</a>
	End Rem
	Function localDay:TDate()
		Return _create(bmx_datetime_localday())
	End Function
	
	Rem
	bbdoc: Creates a new #TDate for the current UCT time.
	End Rem
	Function universalDay:TDate()
		Return _create(bmx_datetime_universalday())
	End Function
	
	Rem
	bbdoc: From delimited date string where with order year-month-day eg: 2002-1-25
	returns: A #TDate of the string entered, or Null if the date was invalid.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.fromString("2004-09-25")
	
	Print d.toString()
	</pre>
	<a href="../examples/tdate_fromString.bmx">Example source</a>
	End Rem
	Function fromString:TDate(date:String)
		Return _create(bmx_datetime_fromstring(date))
	End Function
	
	Rem
	bbdoc: From iso type date string where with order year-month-day eg: 20020125
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.fromUndelimitedString("19760130")
	
	Print d.toString()
	</pre>
	<a href="../examples/tdate_fromUndelimitedString.bmx">Example source</a>
	End Rem
	Function fromUndelimitedString:TDate(date:String)
		Return _create(bmx_datetime_fromundelimitedstring(date))
	End Function

	Rem
	bbdoc: Returns true if the date is before @when.
	End Rem
	Method isBefore:Int(when:TDate)
		Return bmx_datetime_before(datePtr, when.datePtr)
	End Method

	Rem
	bbdoc: Returns true if the date is after @when.
	End Rem
	Method isAfter:Int(when:TDate)
		Return bmx_datetime_after(datePtr, when.datePtr)
	End Method

	Rem
	bbdoc: Returns true if the two dates are equal.
	End Rem
	Method isEqual:Int(when:TDate)
		Return bmx_datetime_equals(datePtr, when.datePtr)
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TDate(obj) Then
			If isBefore(TDate(obj)) Then Return -1
			If isEqual(TDate(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method
	
	Rem
	bbdoc: Returns the year part of the date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2007, Jul, 17)
	
	Print d.year()
	</pre>
	<a href="../examples/tdate_year.bmx">Example source</a>
	End Rem
	Method year:Int()
		Return bmx_datetime_year(datePtr)
	End Method
	
	Rem
	bbdoc: Returns the month part of the date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2007, Jul, 17)
	
	Print d.month()
	</pre>
	<a href="../examples/tdate_month.bmx">Example source</a>
	End Rem
	Method Month:Int()
		Return bmx_datetime_month(datePtr)
	End Method
	
	Rem
	bbdoc: Returns the day (of the month) part of the date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2007, Jul, 17)
	
	Print d.day()
	</pre>
	<a href="../examples/tdate_day.bmx">Example source</a>
	End Rem
	Method day:Int()
		Return bmx_datetime_day(datePtr)
	End Method
	
	Rem
	bbdoc: Populates @year, @month and @day with the date parts.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2006, Dec, 24)
	
	Local year:Int, month:Int, day:Int
	
	d.ymd(year, month, day)
	
	Print year
	Print month
	Print day
	</pre>
	<a href="../examples/tdate_ymd.bmx">Example source</a>
	End Rem
	Method ymd(year:Int Var, Month:Int Var, day:Int Var)
		bmx_datetime_ymd(datePtr, Varptr year, Varptr Month, Varptr day)
	End Method
	
	Rem
	bbdoc: Get the day of the week.
	about: Sunday = 0, Monday = 1, etc.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.dayOfWeek()
	</pre>
	<a href="../examples/tdate_dayOfWeek.bmx">Example source</a>
	End Rem
	Method dayOfWeek:Int()
		Return bmx_datetime_day_of_week(datePtr)
	End Method
	
	Rem
	bbdoc: Get the weekday as a string.
	about: This is based on the current weekday format as specified by #TDateFacet.
	End Rem
	Method WeekDay:String()
		Return bmx_weekday_to_string(dayOfWeek())
	End Method
	
	Rem
	bbdoc: Get the day of the year.
	about: Number from 1 to 366.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.dayOfYear()
	</pre>
	<a href="../examples/tdate_dayOfYear.bmx">Example source</a>
	End Rem
	Method dayOfYear:Int()
		Return bmx_datetime_day_of_year(datePtr)
	End Method
	
	Rem
	bbdoc: Returns a #TDate object set to the last day of the calling objects current month.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	Local lastDay:TDate = d.lastDayOfMonth()
	
	Print lastDay.toString()
	</pre>
	<a href="../examples/tdate_lastDayOfMonth.bmx">Example source</a>
	End Rem
	Method lastDayOfMonth:TDate()
		Return _create(bmx_datetime_end_of_month(datePtr))
	End Method
	
	Rem
	bbdoc: Returns the ISO 8601 week number for the date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.weekNumber()
	</pre>
	<a href="../examples/tdate_weekNumber.bmx">Example source</a>
	End Rem
	Method weekNumber:Int()
		Return bmx_datetime_week_number(datePtr)
	End Method
	
	Rem
	bbdoc: Return a #TDate adding a day offset.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.add(10).toString()
	</pre>
	<a href="../examples/tdate_add.bmx">Example source</a>
	End Rem
	Method add:TDate(days:Int)
		Return _create(bmx_datetime_date_add(datePtr, days))
	End Method
	
	Rem
	bbdoc: Return a #TDate by adding a day offset.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.subtract(14).toString()
	</pre>
	<a href="../examples/tdate_subtract.bmx">Example source</a>
	End Rem
	Method subtract:TDate(days:Int)
		Return _create(bmx_datetime_date_subtract(datePtr, days))
	End Method
	
	Rem
	bbdoc: Returns the number of days difference.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d1:TDate = TDate.Create(2000, Jan, 1)
	Local d2:TDate = TDate.localDay()
	
	Print d2.subtractDate(d1)
	</pre>
	<a href="../examples/tdate_subtractDate.bmx">Example source</a>
	End Rem
	Method subtractDate:Int(date:TDate)
		Return bmx_datetime_date_subdate(datePtr, date.datePtr)
	End Method
	
	Rem
	bbdoc: To YYYY-mmm-DD string where mmm 3 char month name.
	End Rem
	Method toString:String()
		Return bmx_datetime_to_string(datePtr, currentDateFacet.localePtr, currentDateFacet.facetPtr)
	End Method

	Rem
	bbdoc: To YYYY-mmm-DD string where mmm 3 char month name.
	End Rem
	Method toSimpleString:String()
		Return bmx_datetime_to_simple_string(datePtr)
	End Method
	
	Rem
	bbdoc: To YYYYMMDD where all components are integers.
	End Rem
	Method toISOString:String()
		Return bmx_datetime_to_iso_string(datePtr)
	End Method
	
	Rem
	bbdoc: To YYYY-MM-DD where all components are integers.s
	End Rem
	Method toISOExtendedString:String()
		Return bmx_datetime_to_iso_extended_string(datePtr)
	End Method
	
	Rem
	bbdoc: Returns a #TDateDayIterator for this date.
	about: @offset is the number of days moved in each forward/backward.
	End Rem
	Method dayIterator:TDateDayIterator(offset:Int = 1)
		Return TDateDayIterator.CreateIterator(Self, offset)
	End Method

	Rem
	bbdoc: Returns a #TDateMonthIterator for this date.
	about: @offset is the number of months moved in each forward/backward.
	End Rem
	Method monthIterator:TDateMonthIterator(offset:Int = 1)
		Return TDateMonthIterator.CreateIterator(Self, offset)
	End Method

	Rem
	bbdoc: Returns a #TDateYearIterator for this date.
	about: @offset is the number of years moved in each forward/backward.
	End Rem
	Method yearIterator:TDateYearIterator(offset:Int = 1)
		Return TDateYearIterator.CreateIterator(Self, offset)
	End Method
	
	Rem
	bbdoc: Output the date in the specified format.
	about: Formatting characters are as follows :
	<table align="center" width="90%">
	<tr><th>Format Specifier</th><th>Description</th></tr>
	<tr><td>%a</td><td>Abbreviated weekday name</td></tr>
	<tr><td>%A</td><td>Long weekday name</td></tr>
	<tr><td>%b</td><td>Abbreviated month name</td></tr>
	<tr><td>%B</td><td>Full month name</td></tr>
	<tr><td>%c</td><td>The preferred date and time representation for the current locale.</td></tr>
	<tr><td>%C</td><td>The century number (year/100) as a 2-digit integer.</td></tr>
	<tr><td>%d</td><td>Day of the month as decimal 01 to 31</td></tr>
	<tr><td>%D</td><td>Equivalent to %m/%d/%y</td></tr>
	<tr><td>%e</td><td>Like %d, the day of the month as a decimal number, but a leading zero is replaced by a
		space</td></tr>
	<tr><td>%G</td><td>This has the same format and value as %y, except that if the ISO week number belongs to
		the previous or next year, that year is used instead.</td></tr>
	<tr><td>%g</td><td>Like %G, but without century.</td></tr>
	<tr><td>%h</td><td>Equivalent to %b</td></tr>
	<tr><td>%j</td><td>Day of year as decimal from 001 to 366 for leap years, 001 - 365 for non-leap years.</td></tr>
	<tr><td>%m</td><td>Month name as a decimal 01 to 12</td></tr>
	<tr><td>%u</td><td>The day of the week as a decimal, range 1 to 7, Monday being 1.</td></tr>
	<tr><td>%U</td><td>The week number of the current year as a decimal number, range 00 to 53, starting with the
		first Sunday as the first day of week 01. In 2005, Jan 1st falls on a Saturday, so therefore it falls within
		week 00 of 2005 (week 00 spans 2004-Dec-26 to 2005-Jan-01. This also happens to be week 53 of 2004).</td></tr>
	<tr><td>%V</td><td>The ISO 8601:1988 week number of the current year as a decimal number, range 01 to 53,
		where week 1 is the first week that has at least 4 days in the current year, and with Monday as the first
		day of the week.</td></tr>
	<tr><td>%w</td><td>Weekday as decimal number 0 to 6</td></tr>
	<tr><td>%W</td><td>Week number 00 to 53 where Monday is first day of week 1</td></tr>
	<tr><td>%x</td><td>Implementation defined date format from the locale.</td></tr>
	<tr><td>%y</td><td>Two digit year</td></tr>
	<tr><td>%Y</td><td>Four digit year</td></tr>
	<tr><td>%Y-%b-%d</td><td>Default date format</td></tr>
	<tr><td>%Y%m%d</td><td>ISO format</td></tr>
	<tr><td>%Y-%m-%d</td><td>ISO extended format</td></tr>
	</table>
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.localDay()
	
	Print d.format("This is day %d of month %m, of the year %G.")
	</pre>
	<a href="../examples/tdate_format.bmx">Example source</a>
	End Rem
	Method format:String(f:String, facet:TDateFacet = Null)
		If facet Then
			Return bmx_date_asformat(datePtr, f, facet.localePtr, facet.facetPtr)
		Else
			Return bmx_date_asformat(datePtr, f, currentDateFacet.localePtr, currentDateFacet.facetPtr)
		End If
	End Method
	
	' deletes the date instance
	Method Delete()
		If datePtr Then
			bmx_datetime_delete(datePtr)
			datePtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: #TDatePeriod provides direct representation for ranges between two dates.
about: The range is from the begin date up to, but not including, the end date.<br>
A period that is created with beginning and end points being equal, or with a duration of zero,
is known as a zero length period. Zero length periods are considered invalid (it is perfectly
legal to construct an invalid period). For these periods, the last point will always be one unit less
that the begin point.
End Rem
Type TDatePeriod
	
	Field datePeriodPtr:Byte Ptr
	
	Rem
	bbdoc: Create a period as [beginDate, endDate].
	about: If endDate is <= beginDate then the period will be invalid.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.Create(2007, 1, 1)
	Local endDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.Create(startDate, endDate)
	
	Print p.toString()
	</pre>
	<a href="../examples/tdateperiod_create.bmx">Example source</a>
	End Rem
	Function Create:TDatePeriod(beginDate:TDate, endDate:TDate)
		Return _create(bmx_datetime_period_datedate(beginDate.datePtr, endDate.datePtr))
	End Function
	
	Rem
	bbdoc: Create a period as [beginDate, beginDate + length] where end point would be beginDate + length.
	about: If length is <= zero then the period will be defined as invalid.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 150)
	
	Print p.toString()
	</pre>
	<a href="../examples/tdateperiod_createWithDays.bmx">Example source</a>
	End Rem
	Function CreateWithDays:TDatePeriod(beginDate:TDate, length:Int)
		Return _create(bmx_datetime_period_withdays(beginDate.datePtr, length))
	End Function
	
	Function _create:TDatePeriod(datePeriodPtr:Byte Ptr)
	
		If datePeriodPtr Then
			Local this:TDatePeriod = New TDatePeriod
			
			this.datePeriodPtr = datePeriodPtr
			
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Add @days to both begin and end.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.Create(2007, Feb, 1)
	Local endDate:TDate = TDate.Create(2007, Mar, 1)
	
	Local p:TDatePeriod = TDatePeriod.Create(startDate, endDate)
	
	Print p.toString()
	
	p.shift(15)
	
	Print p.toString()
	</pre>
	<a href="../examples/tdateperiod_shift.bmx">Example source</a>
	End Rem
	Method shift(days:Int)
		bmx_datetime_period_shift(datePeriodPtr, days)
	End Method
	
	Rem
	bbdoc: Returns first day of period.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 100)
	
	Print p.begin().toString()
	</pre>
	<a href="../examples/tdateperiod_begin.bmx">Example source</a>
	End Rem
	Method begin:TDate()
		Return TDate._create(bmx_datetime_period_begin(datePeriodPtr))
	End Method
	
	Rem
	bbdoc: Returns the last date in the period.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 100)
	
	Print p.last().toString()
	</pre>
	<a href="../examples/tdateperiod_last.bmx">Example source</a>
	End Rem
	Method last:TDate()
		Return TDate._create(bmx_datetime_period_last(datePeriodPtr))
	End Method
	
	Rem
	bbdoc: Returns one past the last in the period.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 100)
	
	Print p.periodEnd().toString()
	</pre>
	<a href="../examples/tdateperiod_periodEnd.bmx">Example source</a>
	End Rem
	Method periodEnd:TDate()
		Return TDate._create(bmx_datetime_period_end(datePeriodPtr))
	End Method
	
	Rem
	bbdoc: Returns the length (number of days) in the period.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local startDate:TDate = TDate.localDay()
	
	Local p:TDatePeriod = TDatePeriod.CreateWithDays(startDate, 100)
	
	Print p.length()
	</pre>
	<a href="../examples/tdateperiod_length.bmx">Example source</a>
	End Rem
	Method length:Int()
		Return bmx_datetime_period_length(datePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if period is not well formed.
	about: eg. end less than or equal to begin.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2007, 2, 1), TDate.Create(2007, 3, 1))
	Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2007, 2, 1), TDate.Create(2007, 1, 1))
	
	Print p1.isNull()
	Print p2.isNull()
	</pre>
	<a href="../examples/tdateperiod_isNull.bmx">Example source</a>
	End Rem
	Method isNull:Int()
		Return bmx_datetime_period_isnull(datePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if @date is within the period.
	about: Zero length periods cannot contain any points.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))
	
	Local goodDate:TDate = TDate.Create(2006, 6, 1)
	Local badDate:TDate = TDate.create(2005, 1, 1)
	
	Print p.containsDate(goodDate)
	Print p.containsDate(badDate)
	</pre>
	<a href="../examples/tdateperiod_containsDate.bmx">Example source</a>
	End Rem
	Method containsDate:Int(date:TDate)
		Return bmx_datetime_period_containsdate(datePeriodPtr, date.datePtr)
	End Method
	
	Rem
	bbdoc: True if @period is within this period.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))
	
	Local pGood:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 4, 4), TDate.Create(2006, 5, 5))
	Local pBad:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))
	
	Print p.contains(pGood)
	Print p.contains(pBad)
	</pre>
	<a href="../examples/tdateperiod_contains.bmx">Example source</a>
	End Rem
	Method contains:Int(period:TDatePeriod)
		Return bmx_datetime_period_contains(datePeriodPtr, period.datePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if periods overlap.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))
	
	Local pGood:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))
	Local pBad:TDatePeriod = TDatePeriod.Create(TDate.Create(2005, 1, 1), TDate.Create(2005, 12, 1))
	
	Print p.intersects(pGood)
	Print p.intersects(pBad)
	</pre>
	<a href="../examples/tdateperiod_intersects.bmx">Example source</a>
	End Rem
	Method intersects:Int(period:TDatePeriod)
		Return bmx_datetime_period_intersects(datePeriodPtr, period.datePeriodPtr)
	End Method
	
	Rem
	bbdoc: Calculates the intersection of 2 periods.
	about: Null if no intersection.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))
	
	Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))
	
	Local intersection:TDatePeriod = p1.intersection(p2)
	
	If intersection Then
		Print intersection.toString()
	End If
	</pre>
	<a href="../examples/tdateperiod_intersection.bmx">Example source</a>
	End Rem
	Method intersection:TDatePeriod(period:TDatePeriod)
		Return _create(bmx_datetime_period_intersection(datePeriodPtr, period.datePeriodPtr))
	End Method
	
	Rem
	bbdoc: Checks if two periods are adjacent, but not overlapping.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))
	Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 10, 1))
	
	Local p3:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 9, 1)) ' a gap here!
	
	Print p1.isAdjacent(p2)
	Print p1.isAdjacent(p3)
	</pre>
	<a href="../examples/tdateperiod_isAdjacent.bmx">Example source</a>
	End Rem
	Method isAdjacent:Int(period:TDatePeriod)
		Return bmx_datetime_period_adjacent(datePeriodPtr, period.datePeriodPtr)
	End Method
	
	Rem
	bbdoc: Determines if the period is after a given date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))
	
	Print p.isAfter(TDate.Create(2000, 1, 1))
	Print p.isAfter(TDate.Create(2008, 1, 1))
	</pre>
	<a href="../examples/tdateperiod_isAfter.bmx">Example source</a>
	End Rem
	Method isAfter:Int(date:TDate)
		Return bmx_datetime_period_after(datePeriodPtr, date.datePtr)
	End Method
	
	Rem
	bbdoc: Determines if the period is before a given date.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))
	
	Print p.isBefore(TDate.Create(2000, 1, 1))
	Print p.isBefore(TDate.Create(2008, 1, 1))
	</pre>
	<a href="../examples/tdateperiod_isBefore.bmx">Example source</a>
	End Rem
	Method isBefore:Int(date:TDate)
		Return bmx_datetime_period_before(datePeriodPtr, date.datePtr)
	End Method
	
	Rem
	bbdoc: Returns union of two periods.
	about: Null if no intersection.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 1, 1))
	Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 12, 1), TDate.Create(2007, 2, 1))
	
	Local p3:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 5, 1)) ' doesn't overlap !
	
	
	Local merged:TDatePeriod = p1.merge(p2)
	
	If Not merged.isNull() Then
		Print merged.toString()
	End If
	
	merged = p1.merge(p3)
	
	If merged.isNull() Then
		Print p1.toString() + " and " + p3.toString() + " do NOT overlap!"
	End If
	</pre>
	<a href="../examples/tdateperiod_merge.bmx">Example source</a>
	End Rem
	Method merge:TDatePeriod(period:TDatePeriod)
		Return _create(bmx_datetime_period_merge(datePeriodPtr, period.datePeriodPtr))
	End Method
	
	Rem
	bbdoc: Combines two periods and any gap between them such that begin = min(begin, period.begin) and end = max(end , period.end)
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 10, 1))
	Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 12, 1), TDate.Create(2007, 2, 1))
	
	Local span:TDatePeriod = p1.span(p2)
	
	If Not span.isNull() Then
		Print span.toString()
	End If
	</pre>
	<a href="../examples/tdateperiod_span.bmx">Example source</a>
	End Rem
	Method span:TDatePeriod(period:TDatePeriod)
		Return _create(bmx_datetime_period_span(datePeriodPtr, period.datePeriodPtr))
	End Method
	
	Rem
	bbdoc: True if end is less than period.begin.
	End Rem
	Method isLess:Int(period:TDatePeriod)
		Return bmx_datetime_period_isless(datePeriodPtr, period.datePeriodPtr)
	End Method

	Rem
	bbdoc: True if begin greater than period.end
	End Rem
	Method isGreater:Int(period:TDatePeriod)
		Return bmx_datetime_period_isgreater(datePeriodPtr, period.datePeriodPtr)
	End Method

	Rem
	bbdoc: True if periods are the same.
	End Rem
	Method isEqual:Int(period:TDatePeriod)
		Return bmx_datetime_period_isequal(datePeriodPtr, period.datePeriodPtr)
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TDatePeriod(obj) Then
			If isLess(TDatePeriod(obj)) Then Return -1
			If isEqual(TDatePeriod(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method

	Rem
	bbdoc: To [YYYY-mmm-DD/YYYY-mmm-DD] string where mmm is 3 char month name.
	End Rem
	Method toString:String()
		Return bmx_datetime_period_to_string(datePeriodPtr)
	End Method

	
	Method Delete()
		If datePeriodPtr Then
			bmx_datetime_period_delete(datePeriodPtr)
			datePeriodPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Base type for date iterators.
about: The methods #forward and #backward will move along the time-line based on the size of the offset
as defined when the iterator is created.
See #TDateDayIterator, #TDateMonthIterator and #TDateYearIterator for implementations.
End Rem
Type TDateIterator Extends TDate Abstract

	Rem
	bbdoc: Move date forward one step.
	End Rem
	Method forward()
		bmx_datetime_iter_forward(datePtr)
	End Method
	
	Rem
	bbdoc: Move date backward one step.
	End Rem
	Method backward()
		bmx_datetime_iter_backward(datePtr)
	End Method

	' following methods are overriden.... it's a tad messy, but it was the only way it would work.

	Method isBefore:Int(when:TDate)
		Return bmx_datetime_iter_before(datePtr, when.datePtr)
	End Method

	Method isAfter:Int(when:TDate)
		Return bmx_datetime_iter_after(datePtr, when.datePtr)
	End Method

	Method isEqual:Int(when:TDate)
		Return bmx_datetime_iter_equals(datePtr, when.datePtr)
	End Method

	Method year:Int()
		Return bmx_datetime_iter_year(datePtr)
	End Method
	
	Method Month:Int()
		Return bmx_datetime_iter_month(datePtr)
	End Method
	
	Method day:Int()
		Return bmx_datetime_iter_day(datePtr)
	End Method
	
	Method ymd(year:Int Var, Month:Int Var, day:Int Var)
		bmx_datetime_iter_ymd(datePtr,Varptr year,Varptr Month,Varptr day)
	End Method
	
	Method dayOfWeek:Int()
		Return bmx_datetime_iter_day_of_week(datePtr)
	End Method
	
	Method dayOfYear:Int()
		Return bmx_datetime_iter_day_of_year(datePtr)
	End Method
	
	Method lastDayOfMonth:TDate()
		Return _create(bmx_datetime_iter_end_of_month(datePtr))
	End Method
	
	Method weekNumber:Int()
		Return bmx_datetime_iter_week_number(datePtr)
	End Method

	Method add:TDate(duration:Int)
		Return _create(bmx_datetime_iter_date_add(datePtr, duration))
	End Method
	
	Method subtract:TDate(duration:Int)
		Return _create(bmx_datetime_iter_date_subtract(datePtr, duration))
	End Method
	
	Method subtractDate:Int(date:TDate)
		Return bmx_datetime_iter_date_subdate(datePtr, date.datePtr)
	End Method
	
	Method toString:String()
		Return bmx_datetime_iter_to_string(datePtr, currentDateFacet.localePtr, currentDateFacet.facetPtr)
	End Method
	
	Method toISOString:String()
		Return bmx_datetime_iter_to_iso_string(datePtr)
	End Method
	
	Method toISOExtendedString:String()
		Return bmx_datetime_iter_to_iso_extended_string(datePtr)
	End Method

	Method format:String(f:String, facet:TDateFacet = Null)
		If facet Then
			Return bmx_datetime_iter_asformat(datePtr, f, facet.localePtr, facet.facetPtr)
		Else
			Return bmx_datetime_iter_asformat(datePtr, f, currentDateFacet.localePtr, currentDateFacet.facetPtr)
		End If
	End Method

	Method Delete()
		If datePtr Then
			bmx_datetime_iter_delete(datePtr)
			datePtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A #TDateIterator for stepping over days.
End Rem
Type TDateDayIterator Extends TDateIterator

	Rem
	bbdoc: Creates a new #TDateDayIterator from @date for the step size of @offset days.
	End Rem
	Function CreateIterator:TDateDayIterator(date:TDate, offset:Int = 1)
		Local this:TDateDayIterator = New TDateDayIterator
		
		this.datePtr = bmx_datetime_dayiter(date.datePtr, offset)
		
		Return this
	End Function

End Type

Rem
bbdoc: A #TDateIterator for stepping over months.
End Rem
Type TDateMonthIterator Extends TDateIterator

	Rem
	bbdoc: Creates a new #TDateMonthIterator from @date for the step size of @offset months.
	End Rem
	Function CreateIterator:TDateMonthIterator(date:TDate, offset:Int = 1)
		Local this:TDateMonthIterator = New TDateMonthIterator
		
		this.datePtr = bmx_datetime_monthiter(date.datePtr, offset)
		
		Return this
	End Function

End Type

Rem
bbdoc: A #TDateIterator for stepping over years.
End Rem
Type TDateYearIterator Extends TDateIterator

	Rem
	bbdoc: Creates a new #TDateYearIterator from @date for the step size of @offset years.
	End Rem
	Function CreateIterator:TDateYearIterator(date:TDate, offset:Int = 1)
		Local this:TDateYearIterator = New TDateYearIterator
		
		this.datePtr = bmx_datetime_yeariter(date.datePtr, offset)
		
		Return this
	End Function

End Type

Rem
bbdoc: #TTime is the primary interface for time point manipulation.
End Rem
Type TTime

	Field ptimePtr:Byte Ptr
	
	Rem
	bbdoc: Constructs a new #TTime from a date and offset.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t:TTime = TTime.Create(d, TDHours(14))
	
	Print t.toString()
	</pre>
	<a href="../examples/ttime_create.bmx">Example source</a>
	End Rem
	Function Create:TTime(date:TDate, offset:TTimeDuration)
		Return _create(bmx_ptime_new(date.datePtr, offset.durationPtr))
	End Function
	
	Function _create:TTime(ptimePtr:Byte Ptr)
		If ptimePtr Then
			Local this:TTime = New TTime
			
			this.ptimePtr = ptimePtr
			
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Get the local time, second level resolution, based on the time zone settings of the computer.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local t:TTime = TTime.CreateLocal()
	
	Print t.toString()
	</pre>
	<a href="../examples/ttime_createLocal.bmx">Example source</a>
	End Rem
	Function CreateLocal:TTime()
		Return _create(bmx_ptime_local_new())
	End Function
	
	Rem
	bbdoc: Get the UTC time.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local t:TTime = TTime.CreateUniversal()
	
	Print t.toString()
	</pre>
	<a href="../examples/ttime_createUniversal.bmx">Example source</a>
	End Rem
	Function CreateUniversal:TTime()
		Return _create(bmx_ptime_universal_new())
	End Function
	
	Rem
	bbdoc: Get the local time using a sub second resolution clock.
	about: On Unix systems this is implemented using <tt>GetTimeOfDay</tt>.
	On most Win32 platforms it is implemented using <tt>ftime</tt>. Win32 systems often do not achieve
	microsecond resolution via this API. If higher resolution is critical to your application
	test your platform to see the achieved resolution.
	End Rem
	Function CreateLocalMS:TTime()
		Return _create(bmx_ptime_local_microsecond_new())
	End Function
	
	Rem
	bbdoc: Get the UTC time using a sub second resolution clock.
	about: On Unix systems this is implemented using <tt>GetTimeOfDay</tt>. On most Win32 platforms it is
	implemented using <tt>ftime</tt>. Win32 systems often do not achieve microsecond resolution via this API.
	If higher resolution is critical to your application test your platform to see the achieved resolution.
	End Rem
	Function CreateUniversalMS:TTime()
		Return _create(bmx_ptime_universal_microsecond_new())
	End Function
	
	Rem
	bbdoc: Converts a #FileTime value to a #TTime.
	about: This value is the number of seconds from 1-Jan-1970.<br>
	#FileTime returns the time at UTC, so you will need to apply your local offset to get
	the correct local system time.
	End Rem
	Function CreateFromFileTime:TTime(time:Int)
		' since the "proper" call didn't work, we do what it does internally, and add seconds to base date!
		Return Create(TDate.Create(1970, 1, 1), TDSeconds(time))
	End Function
	
	Rem
	bbdoc: Converts a time_t struct to a #TTime.
	End Rem
	Function CreateFromTimeT:TTime(time:Byte Ptr)
		' since the "proper" call didn't work, we construct the date + time from the struct ourselves!
		Local tm:Int Ptr = Int Ptr(time)
		Return Create(TDate.Create(tm[5] + 1900, tm[4] + 1, tm[3]), TDHours(tm[2]).add(TDMinutes(tm[1])).add(TDSeconds(tm[0])))
	End Function
	
	Rem
	bbdoc: Get the date part of a time.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 10)
	Local t:TTime = TTime.Create(d, TDHours(1))
	Print t.date().toString()
	</pre>
	<a href="../examples/ttime_date.bmx">Example source</a>
	End Rem
	Method date:TDate()
		Return TDate._create(bmx_ptime_date(ptimePtr))
	End Method
	
	Rem
	bbdoc: Get the time offset in the day.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 10)
	Local t:TTime = TTime.Create(d, TDHours(1))
	Print t.timeOfDay().toString()
	</pre>
	<a href="../examples/ttime_timeOfDay.bmx">Example source</a>
	End Rem
	Method timeOfDay:TTimeDuration()
		Return TTimeDuration._create(bmx_ptime_time_of_day(ptimePtr))
	End Method
	
	Rem
	bbdoc: To YYYY-mmm-DD HH:MM:SS.fffffffff string where mmm 3 char month name.
	about: Fractional seconds only included if non-zero.
	End Rem
	Method toString:String()
		Return bmx_ptime_to_string(ptimePtr, currentDateFacet.localePtr, currentDateFacet.facetPtr)
	End Method
	
	Rem
	bbdoc: Convert to form YYYYMMDDTHHMMSS,fffffffff where T is the date-time separator.
	End Rem
	Method toISOString:String()
		Return bmx_ptime_to_iso_string(ptimePtr)
	End Method
	
	Rem
	bbdoc: Convert to form YYYY-MM-DDTHH:MM:SS,fffffffff where T is the date-time separator.
	End Rem
	Method toISOExtendedString:String()
		Return bmx_ptime_to_iso_extended_string(ptimePtr)
	End Method
	
	Rem
	bbdoc: Adds @days to time, returning a new #TTime.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t:TTime = TTime.Create(d, TDMinutes(5))
	
	Local t2:TTime = t.addDays(1)
	
	Print t.toString()
	Print t2.toString()	
	</pre>
	<a href="../examples/ttime_addDays.bmx">Example source</a>
	End Rem
	Method addDays:TTime(days:Int)
		Return _create(bmx_ptime_add_days(ptimePtr, days))
	End Method
	
	Rem
	bbdoc: Subtracts @days from time, returning a new #TTime.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t:TTime = TTime.Create(d, TDMinutes(5))
	
	Local t2:TTime = t.subtractDays(1)
	
	Print t.toString()
	Print t2.toString()
	</pre>
	<a href="../examples/ttime_subtractDays.bmx">Example source</a>
	End Rem
	Method subtractDays:TTime(days:Int)
		Return _create(bmx_ptime_subtract_days(ptimePtr, days))
	End Method
	
	Rem
	bbdoc: Adds @duration to the time, returning a new #TTime.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t:TTime = TTime.Create(d, TDMinutes(5))
	
	Local t2:TTime = t.addDuration(TDHours(1).add(TDMinutes(2)))
	
	Print t.toString()
	Print t2.toString()
	</pre>
	<a href="../examples/ttime_addDuration.bmx">Example source</a>
	End Rem
	Method addDuration:TTime(duration:TTimeDuration)
		Return _create(bmx_ptime_add_duration(ptimePtr, duration.durationPtr))
	End Method

	Rem
	bbdoc: Subtracts @duration from the time, returning a new #TTime.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t:TTime = TTime.Create(d, TDMinutes(5))
	
	Local t2:TTime = t.subtractDuration(TDMinutes(2))
	
	Print t.toString()
	Print t2.toString()
	</pre>
	<a href="../examples/ttime_subtractDuration.bmx">Example source</a>
	End Rem
	Method subtractDuration:TTime(duration:TTimeDuration)
		Return _create(bmx_ptime_subtract_duration(ptimePtr, duration.durationPtr))
	End Method

	Rem
	bbdoc: Subtracts @time from the time, returning a #TTimeDuration
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local d:TDate = TDate.Create(2002, Jan, 1)
	Local t1:TTime = TTime.Create(d, TDMinutes(5))
	Local t2:TTime = TTime.Create(d, TDMinutes(2))
	
	Print t2.subtract(t1).toString() ' negative result
	</pre>
	<a href="../examples/ttime_subtract.bmx">Example source</a>
	End Rem
	Method subtract:TTimeDuration(time:TTime)
		Return TTimeDuration._create(bmx_ptime_subtract(ptimePtr, time.ptimePtr))
	End Method

	Rem
	bbdoc: True if this time is less than the parameter.
	End Rem
	Method isLess:Int(time:TTime)
		Return bmx_ptime_less(ptimePtr, time.ptimePtr)
	End Method

	Rem
	bbdoc: True if this time is greater than the parameter.
	End Rem
	Method isGreater:Int(time:TTime)
		Return bmx_ptime_greater(ptimePtr, time.ptimePtr)
	End Method

	Rem
	bbdoc: True if the two times are equal.
	End Rem
	Method isEqual:Int(time:TTime)
		Return bmx_ptime_equal(ptimePtr, time.ptimePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method format:String(f:String, facet:TTimeFacet = Null)
		If facet Then
			Return bmx_ptime_asformat(ptimePtr, f, facet.localePtr, facet.facetPtr)
		Else
			Return bmx_ptime_asformat(ptimePtr, f, currentTimeFacet.localePtr, currentTimeFacet.facetPtr)
		End If
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TTime(obj) Then
			If isLess(TTime(obj)) Then Return -1
			If isEqual(TTime(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method
	
	Method Delete()
		If ptimePtr Then
			bmx_ptime_delete(ptimePtr)
			ptimePtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Represents a length of time.
End Rem
Type TTimeDuration

	Field durationPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new #TTimeDuration from the counts.
	about: The @fractions parameter is a number of units and is therefore affected by the resolution
	the application is compiled with. If the @fractions argument exceeds the limit of the compiled
	precision, the excess value will be "carried over" into the seconds field.<br>
	Ths following is a technique using #TicksPerSecond for creating a resolution independent
	fractions count :
	<pre>
	Local numberOfTenths:Int = 5
	' create a resolution independent count -- divide by 10 since there are 10 tenths in a second.
	Local count:Int = numberOfTenths * (TicksPerSecond() / 10)
	
	Local duration:TTimeDuration = TTimeDuration.Create(1, 2, 3, count) ' 01:02:03.5 - no matter the resolution settings
	</pre>
	End Rem
	Function Create:TTimeDuration(hours:Int = 0, minutes:Int = 0, seconds:Int = 0, fractions:Int = 0)
		Return _create(bmx_time_duration(hours, minutes, seconds, fractions))
	End Function
	
	Rem
	bbdoc: Creates a new #TTimeDuration for the number of hours.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Hour(10)
	
	Print td.toString()
	</pre>
	<a href="../examples/ttimeduration_hour.bmx">Example source</a>
	End Rem
	Function Hour:TTimeDuration(hours:Int)
		Return _create(bmx_time_duration_new_hours(hours))
	End Function

	Rem
	bbdoc: Creates a new #TTimeDuration for the number of minutes.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Minute(7)
	
	Print td.toString()
	</pre>
	<a href="../examples/ttimeduration_minute.bmx">Example source</a>
	End Rem
	Function Minute:TTimeDuration(minutes:Int)
		Return _create(bmx_time_duration_new_minutes(minutes))
	End Function

	Rem
	bbdoc: Creates a new #TTimeDuration for the number of seconds.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Second(45)
	
	Print td.toString()
	</pre>
	<a href="../examples/ttimeduration_second.bmx">Example source</a>
	End Rem
	Function Second:TTimeDuration(seconds:Int)
		Return _create(bmx_time_duration_new_seconds(seconds))
	End Function

	Rem
	bbdoc: Creates a new #TTimeDuration for the number of milliseconds.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Millisecond(180)
	
	Print td.toString()
	</pre>
	<a href="../examples/ttimeduration_millisecond.bmx">Example source</a>
	End Rem
	Function Millisecond:TTimeDuration(milliseconds:Int)
		Return _create(bmx_time_duration_new_milliseconds(milliseconds))
	End Function
	
	Function _create:TTimeDuration(durationPtr:Byte Ptr)
		If durationPtr Then
			Local this:TTimeDuration = New TTimeDuration
		
			this.durationPtr = durationPtr
		
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Get the number of normalized hours.
	End Rem
	Method hours:Int()
		Return bmx_time_duration_hours(durationPtr)
	End Method

	Rem
	bbdoc: Get the number of minutes normalized +/-(0..59).
	End Rem
	Method minutes:Int()
		Return bmx_time_duration_minutes(durationPtr)
	End Method

	Rem
	bbdoc: Get the normalized number of second +/-(0..59).
	End Rem
	Method seconds:Int()
		Return bmx_time_duration_seconds(durationPtr)
	End Method

	Rem
	bbdoc: Get the total number of seconds truncating any fractional seconds.
	End Rem
	Method totalSeconds:Int()
		Return bmx_time_duration_total_seconds(durationPtr)
	End Method
	
	Rem
	bbdoc: Get the total number of milliseconds truncating any remaining digits.
	End Rem
	Method totalMilliseconds:Int()
		Return bmx_time_duration_total_milliseconds(durationPtr)
	End Method
	
	Rem
	bbdoc: Get the number of fractional seconds.
	End Rem
	Method fractionalSeconds:Int()
		Return bmx_time_duration_fractional_seconds(durationPtr)
	End Method
	
	Rem
	bbdoc: True if duration is negative.
	End Rem
	Method isNegative:Int()
		Return bmx_time_duration_is_negative(durationPtr)
	End Method
	
	Rem
	bbdoc: Generate a new duration with the sign inverted.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TDHours(12)
	
	Print td.toString()
	Print td.invertSign().toString()
	</pre>
	<a href="../examples/ttimeduration_invertSign.bmx">Example source</a>
	End Rem
	Method invertSign:TTimeDuration()
		Return _create(bmx_time_duration_invert_sign(durationPtr))
	End Method
	
	Rem
	bbdoc: To HH:MM:SS.fffffffff where fff is fractional seconds that are only included if non-zero.
	End Rem
	Method toString:String()
		Return bmx_time_duration_to_string(durationPtr)
	End Method
	
	Rem
	bbdoc: Converts to a string in the form HHMMSS,fffffffff.
	End Rem
	Method toISOString:String()
		Return bmx_time_duration_to_iso_string(durationPtr)
	End Method
	
	Rem
	bbdoc: Adds @duration to this duration, returning the result in a new #TTimeDuration
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TDHours(12).add(TDMinutes(35)).add(TDSeconds(12))
	
	Print td.toString()
	</pre>
	<a href="../examples/ttimeduration_add.bmx">Example source</a>
	End Rem
	Method add:TTimeDuration(duration:TTimeDuration)
		Return _create(bmx_time_duration_add(durationPtr, duration.durationPtr))
	End Method
	
	Rem
	bbdoc: Subtracts @duration from this duration, returning the result in a new #TTimeDuration
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td1:TTimeDuration = TTimeDuration.Create(12, 30, 20)
	Local td2:TTimeDuration = TTimeDuration.Create(1, 30, 20)
	
	Print td1.subtract(td2).toString()
	</pre>
	<a href="../examples/ttimeduration_subtract.bmx">Example source</a>
	End Rem
	Method subtract:TTimeDuration(duration:TTimeDuration)
		Return _create(bmx_time_duration_subtract(durationPtr, duration.durationPtr))
	End Method
	
	Rem
	bbdoc: Divides the length of the duration by @value.
	about: Discards any remainder.
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Create(18, 30) ' 18 hours and 30 mins
	
	Print td.divide(2).toString() ' 9 hours and 15 mins
	</pre>
	<a href="../examples/ttimeduration_divide.bmx">Example source</a>
	End Rem
	Method divide:TTimeDuration(value:Int)
		Return _create(bmx_time_duration_divide(durationPtr, value))
	End Method
	
	Rem
	bbdoc: Multiplies the length of the duration by @value.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local td:TTimeDuration = TTimeDuration.Create(4, 10, 6)
	
	Print td.multiply(3).toString()
	</pre>
	<a href="../examples/ttimeduration_multiply.bmx">Example source</a>
	End Rem
	Method multiply:TTimeDuration(value:Int)
		Return _create(bmx_time_duration_multiply(durationPtr, value))
	End Method
	
	Rem
	bbdoc: True if this duration is less than the parameter.
	End Rem
	Method isLess:Int(duration:TTimeDuration)
		Return bmx_time_duration_less(durationPtr, duration.durationPtr)
	End Method
	
	Rem
	bbdoc: True if this duration is greater than the parameter.
	End Rem
	Method isGreater:Int(duration:TTimeDuration)
		Return bmx_time_duration_greater(durationPtr, duration.durationPtr)
	End Method
	
	Rem
	bbdoc: True if the durations are equal.
	End Rem
	Method isEqual:Int(duration:TTimeDuration)
		Return bmx_time_duration_equal(durationPtr, duration.durationPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method format:String(f:String, facet:TTimeFacet = Null)
		If facet Then
			Return bmx_time_duration_asformat(durationPtr, f, facet.localePtr, facet.facetPtr)
		Else
			Return bmx_time_duration_asformat(durationPtr, f, currentTimeFacet.localePtr, currentTimeFacet.facetPtr)
		End If
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TTimeDuration(obj) Then
			If isLess(TTimeDuration(obj)) Then Return -1
			If isEqual(TTimeDuration(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method
	

	Method Delete()
		If durationPtr Then
			bmx_time_duration_delete(durationPtr)
			durationPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: #TimePeriod provides direct representation for ranges between two times.
about: Periods provide the ability to simplify some types of calculations by simplifying the
conditional logic of the program.<br>
A period that is created with beginning and end points being equal, or with a duration of zero,
is known as a zero length period. Zero length periods are considered invalid (it is perfectly legal to
construct an invalid period). For these periods, the last point will always be one unit less that the
begin point.
End Rem
Type TTimePeriod
	Field timePeriodPtr:Byte Ptr
	
	Rem
	bbdoc: Create a period as [begin, end).
	about: If end is <= begin then the period will be defined as invalid.
	End Rem
	Function Create:TTimePeriod(beginTime:TTime, endTime:TTime)
		Return _create(bmx_time_period_timetime(beginTime.ptimePtr, endTime.ptimePtr))
	End Function
	
	Rem
	bbdoc: Create a period as [begin, begin + duration) where end would be begin + duration.
	about: If duration is <= zero then the period will be defined as invalid.
	End Rem
	Function CreateWithDuration:TTimePeriod(beginTime:TTime, duration:TTimeDuration)
		Return _create(bmx_time_period_withduration(beginTime.ptimePtr, duration.durationPtr))
	End Function

	Function _create:TTimePeriod(timePeriodPtr:Byte Ptr)
	
		If timePeriodPtr Then
			Local this:TTimePeriod = New TTimePeriod
			
			this.timePeriodPtr = timePeriodPtr
			
			Return this
		End If
		
		Return Null
	End Function

	Rem
	bbdoc: Add @duration to both begin and end.
	End Rem
	Method shift(duration:TTimeDuration)
		bmx_time_period_shift(timePeriodPtr, duration.durationPtr)
	End Method

	Rem
	bbdoc: Return first time of period.
	End Rem
	Method begin:TTime()
		Return TTime._create(bmx_time_period_begin(timePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return last time in the period
	End Rem
	Method last:TTime()
		Return TTime._create(bmx_time_period_last(timePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return one past the last in period
	End Rem
	Method periodEnd:TTime()
		Return TTime._create(bmx_time_period_end(timePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return the length of the time period.
	End Rem
	Method length:TTimeDuration()
		Return TTimeDuration._create(bmx_time_period_length(timePeriodPtr))
	End Method
	
	Rem
	bbdoc: True if period is not well formed.
	about: eg. end is less than or equal to begin.
	End Rem
	Method isNull:Int()
		Return bmx_time_period_is_null(timePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if @time is within the period.
	about: Zero length periods cannot contain any points.
	End Rem
	Method contains:Int(time:TTime)
		Return bmx_time_period_contains(timePeriodPtr, time.ptimePtr)
	End Method
	
	Rem
	bbdoc: True if @period is within the period
	End Rem
	Method containsPeriod:Int(period:TTimePeriod)
		Return bmx_time_period_containsPeriod(timePeriodPtr, period.timePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if periods overlap.
	End Rem
	Method intersects:Int(period:TTimePeriod)
		Return bmx_time_period_intersects(timePeriodPtr, period.timePeriodPtr)
	End Method
	
	Rem
	bbdoc: Calculate the intersection of 2 periods.
	about: Null if no intersection.
	End Rem
	Method intersection:TTimePeriod(period:TTimePeriod)
		Return _create(bmx_time_period_intersection(timePeriodPtr, period.timePeriodPtr))
	End Method
	
	Rem
	bbdoc: Returns union of two periods.
	about: Null if no intersection.
	End Rem
	Method merge:TTimePeriod(period:TTimePeriod)
		Return _create(bmx_time_period_merge(timePeriodPtr, period.timePeriodPtr))
	End Method
	
	Rem
	bbdoc: Combines two periods and any gap between them such that begin = min(p1.begin, p2.begin) and end = max(p1.end , p2.end).
	End Rem
	Method span:TTimePeriod(period:TTimePeriod)
		Return _create(bmx_time_period_span(timePeriodPtr, period.timePeriodPtr))
	End Method
	
	Rem
	bbdoc: To [YYYY-mmm-DD hh:mm:ss.fffffffff/YYYY-mmm-DD hh:mm:ss.fffffffff] string where mmm is 3 char month name.
	End Rem
	Method toString:String()
' TODO
' convertUTF8toISO8859(xxxxxx)
	End Method

	Rem
	bbdoc: True if end is less than period.begin.
	End Rem
	Method isLess:Int(period:TTimePeriod)
		Return bmx_time_period_isless(timePeriodPtr, period.timePeriodPtr)
	End Method

	Rem
	bbdoc: True if begin greater than period.end
	End Rem
	Method isGreater:Int(period:TTimePeriod)
		Return bmx_time_period_isgreater(timePeriodPtr, period.timePeriodPtr)
	End Method

	Rem
	bbdoc: True if periods are the same.
	End Rem
	Method isEqual:Int(period:TTimePeriod)
		Return bmx_time_period_isequal(timePeriodPtr, period.timePeriodPtr)
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TTimePeriod(obj) Then
			If isLess(TTimePeriod(obj)) Then Return -1
			If isEqual(TTimePeriod(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method

	Method Delete()
		If timePeriodPtr Then
			bmx_time_period_delete(timePeriodPtr)
			timePeriodPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The Time Zone Database (#TTZDatabase) is a store of time zone information.
about: The specifications for many time zones (377 at this time) are provided.
These specifications are based on data found in the
<a href="http://www.twinsun.com/tz/tz-link.htm">zoneinfo database</a>. The specifications are stored in
the file: <tt>data/date_time_zonespec.csv</tt>
End Rem
Type TTZDatabase

	Field tzDatabasePtr:Byte Ptr
	
	Rem
	bbdoc: Creates an empty #TTZDatabase.
	End Rem
	Function Create:TTZDatabase()
		Return _create(bmx_tz_database())
	End Function
	
	Function _create:TTZDatabase(tzDatabasePtr:Byte Ptr)
		If tzDatabasePtr Then
			Local this:TTZDatabase = New TTZDatabase
	
			this.tzDatabasePtr = tzDatabasePtr
			
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Creates and populates a #TTZDatabase with time zone records found in the zone spec file.
	End Rem
	Function LoadFromFile:TTZDatabase(filename:String)
		Return _create(bmx_tz_load_from_file(filename))
	End Function
	
	Rem
	bbdoc: Adds a time zone, to the database.
	about: @id is the region name for this zone (e.g. "America/Phoenix").
	End Rem
	Method addRecord(id:String, zone:TTimeZone)
' TODO
	End Method
	
	Rem
	bbdoc: Returns a time zone that matches the region listed in the data file.
	returns: a #TTimeZone or Null if not found.
	End Rem
	Method timeZoneFromRegion:TTimeZone(id:String)
		Return TTimeZone._create(bmx_tz_time_zone_from_region(tzDatabasePtr, id))
	End Method
	
	Method regionList:String[]()
' TODO !!
	End Method
	
	
End Type

Rem
bbdoc: A #TLocalDateTime object is a point in time and an associated time zone.
about: The time is represented internally as UTC.
End Rem
Type TLocalDateTime

	Field localDateTimePtr:Byte Ptr
	
	Function CreateFromClock:TLocalDateTime(zone:TTimeZone)
		Return _create(bmx_local_date_time_new_sec_clock(zone.timeZonePtr))
	End Function

	Rem
	bbdoc:
	abtou: The given time is expected to be UTC. Therefore, the given time will be adjusted
	according to the offset described in the time zone.
	End Rem
	Function CreateFromTime:TLocalDateTime(time:TTime, zone:TTimeZone)
		Return _create(bmx_local_date_time_new_time(time.ptimePtr, zone.timeZonePtr))
	End Function
	
	Function _create:TLocalDateTime(localDateTimePtr:Byte Ptr)
	
		If localDateTimePtr Then
			Local this:TLocalDateTime = New TLocalDateTime
		
			this.localDateTimePtr = localDateTimePtr
		
			Return this
		End If
		
		Return Null
	End Function

	Rem
	bbdoc: Returns associated #TTimeZone object.
	End Rem
	Method zone:TTimeZone()
		Return TTimeZone._create(bmx_local_date_time_zone(localDateTimePtr))
	End Method
	
	Rem
	bbdoc: Determines if time value is in DST for associated zone.
	End Rem
	Method isDST:Int()
		Return bmx_local_date_time_is_dst(localDateTimePtr)
	End Method
	
	Rem
	bbdoc: Converts the local time value to a UTC value.
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local pt:TTime = TTime.Create(TDate.Create(2004, Nov, 5), TDHours(10))
	
	Local zone:TTimeZone = TTimeZone.Create("MST-07")
	
	Local az:TLocalDateTime = TLocalDateTime.CreateFromTime(pt, zone)
	
	Print az.UTCTime().toString()
	</pre>
	<a href="../examples/tlocaldatetime_UTCTime.bmx">Example source</a>
	End Rem
	Method UTCTime:TTime()
		Return TTime._create(bmx_local_date_time_utc_time(localDateTimePtr))
	End Method
	
	Rem
	bbdoc: Returns the local time for this object (Wall-clock).
	about:
	<pre>
	SuperStrict
	
	Framework BaH.DateTime
	Import BRL.StandardIO
	
	Local pt:TTime = TTime.Create(TDate.Create(2004, Nov, 5), TDHours(10))
	
	Local zone:TTimeZone = TTimeZone.Create("MST-07")
	
	Local az:TLocalDateTime = TLocalDateTime.CreateFromTime(pt, zone)
	
	Print az.UTCTime().toString()
	Print az.localTime().toString()
	</pre>
	<a href="../examples/tlocaldatetime_localTime.bmx">Example source</a>
	End Rem
	Method localTime:TTime()
		Return TTime._create(bmx_local_date_time_local_time(localDateTimePtr))
	End Method
	
	Method toString:String()
		Return bmx_local_date_time_to_string(localDateTimePtr)
	End Method
	
	Rem
	bbdoc: True if this local date time is less than the parameter.
	End Rem
	Method isLess:Int(ldt:TLocalDateTime)
		Return bmx_local_date_time_less(localDateTimePtr, ldt.localDateTimePtr)
	End Method

	Rem
	bbdoc: True if this local date time is greater than the parameter.
	End Rem
	Method isGreater:Int(ldt:TLocalDateTime)
		Return bmx_local_date_time_greater(localDateTimePtr, ldt.localDateTimePtr)
	End Method

	Rem
	bbdoc: True if the two local date times are equal.
	End Rem
	Method isEqual:Int(ldt:TLocalDateTime)
		Return bmx_local_date_time_equal(localDateTimePtr, ldt.localDateTimePtr)
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TLocalDateTime(obj) Then
			If isLess(TLocalDateTime(obj)) Then Return -1
			If isEqual(TLocalDateTime(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method

	Rem
	bbdoc: Adds @days to local date time, returning a new #TLocalDateTime.
	End Rem
	Method addDays:TLocalDateTime(days:Int)
		Return _create(bmx_local_date_time_add_days(localDateTimePtr, days))
	End Method
	
	Rem
	bbdoc: Adds @months to local date time, returning a new #TLocalDateTime.
	End Rem
	Method addMonths:TLocalDateTime(months:Int)
		Return _create(bmx_local_date_time_add_months(localDateTimePtr, months))
	End Method
	
	Rem
	bbdoc: Adds @years to local date time, returning a new #TLocalDateTime.
	End Rem
	Method addYears:TLocalDateTime(years:Int)
		Return _create(bmx_local_date_time_add_years(localDateTimePtr, years))
	End Method
	
	Rem
	bbdoc: Subtracts @days from local date time, returning a new #TLocalDateTime.
	End Rem
	Method subtractDays:TLocalDateTime(days:Int)
		Return _create(bmx_local_date_time_subtract_days(localDateTimePtr, days))
	End Method
	
	Rem
	bbdoc: Subtracts @months from local date time, returning a new #TLocalDateTime.
	End Rem
	Method subtractMonths:TLocalDateTime(months:Int)
		Return _create(bmx_local_date_time_subtract_months(localDateTimePtr, months))
	End Method
	
	Rem
	bbdoc: Subtracts @years from local date time, returning a new #TLocalDateTime.
	End Rem
	Method subtractYears:TLocalDateTime(years:Int)
		Return _create(bmx_local_date_time_subtract_years(localDateTimePtr, years))
	End Method
	
	Rem
	bbdoc: Adds @duration to the local date time, returning a new #TLocalDateTime.
	End Rem
	Method addDuration:TLocalDateTime(duration:TTimeDuration)
		Return _create(bmx_local_date_time_add_duration(localDateTimePtr, duration.durationPtr))
	End Method
	
	Rem
	bbdoc: Subtracts @duration from the local date time, returning a new #TLocalDateTime.
	End Rem
	Method subtractDuration:TLocalDateTime(duration:TTimeDuration)
		Return _create(bmx_local_date_time_subtract_duration(localDateTimePtr, duration.durationPtr))
	End Method
	
	Method Delete()
		If localDateTimePtr Then
			bmx_local_date_time_delete(localDateTimePtr)
			localDateTimePtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: #TLocalTimePeriod provides direct representation for ranges between two local times.
about: Periods provide the ability to simplify some types of calculations by simplifying the
conditional logic of the program.
End Rem
Type TLocalTimePeriod

	Field localTimePeriodPtr:Byte Ptr
	
	Rem
	bbdoc: Create a period as [begin, end).
	about: If end is <= begin then the period will be defined as invalid.
	End Rem
	Function Create:TLocalTimePeriod(beginTime:TLocalDateTime, endTime:TLocalDateTime)
		Return _create(bmx_local_time_period_new(beginTime.localDateTimePtr, endTime.localDateTimePtr))
	End Function
	
	Function _create:TLocalTimePeriod(localTimePeriodPtr:Byte Ptr)
		If localTimePeriodPtr Then
			Local this:TLocalTimePeriod = New TLocalTimePeriod
			
			this.localTimePeriodPtr = localTimePeriodPtr
			
			Return this
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Create a period as [begin, begin + duration) where end would be begin + duration.
	about: If duration is <= zero then the period will be defined as invalid.
	End Rem
	Function CreateWithDuration:TLocalTimePeriod(beginTime:TLocalDateTime, duration:TTimeDuration)
		Return _create(bmx_local_time_period_new_duration(beginTime.localDateTimePtr, duration.durationPtr))
	End Function

	Rem
	bbdoc: Return first local date time of the period.
	End Rem
	Method begin:TLocalDateTime()
		Return TLocalDateTime._create(bmx_local_time_period_begin(localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return last local date time in the period.
	End Rem
	Method last:TLocalDateTime()
		Return TLocalDateTime._create(bmx_local_time_period_last(localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return one past the last in period.
	End Rem
	Method periodEnd:TLocalDateTime()
		Return TLocalDateTime._create(bmx_local_time_period_end(localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Return the length of the local time period.
	End Rem
	Method length:TTimeDuration()
		Return TTimeDuration._create(bmx_local_time_period_length(localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: True if period is not well formed.
	about: eg. end less than or equal to begin.
	End Rem
	Method isNull:Int()
		Return bmx_local_time_period_is_null(localTimePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if @time is within the period.
	about: Zero length periods cannot contain any points.
	End Rem
	Method containsTime:Int(time:TLocalDateTime)
		Return bmx_local_time_period_contains_time(localTimePeriodPtr, time.localDateTimePtr)
	End Method
	
	Rem
	bbdoc: True if @period is within the period.
	End Rem
	Method contains:Int(period:TLocalTimePeriod)
		Return bmx_local_time_period_contains(localTimePeriodPtr, period.localTimePeriodPtr)
	End Method
	
	Rem
	bbdoc: True if periods overlap.
	End Rem
	Method intersects:Int(period:TLocalTimePeriod)
		Return bmx_local_time_period_intersects(localTimePeriodPtr, period.localTimePeriodPtr)
	End Method
	
	Rem
	bbdoc: Calculate the intersection of 2 periods.
	about: Null if no intersection.
	End Rem
	Method intersection:TLocalTimePeriod(period:TLocalTimePeriod)
		Return _create(bmx_local_time_period_intersection(localTimePeriodPtr, period.localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Returns union of two periods.
	about: Null if no intersection.
	End Rem
	Method merge:TLocalTimePeriod(period:TLocalTimePeriod)
		Return _create(bmx_local_time_period_merge(localTimePeriodPtr, period.localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Combines two periods and any gap between them such that begin = min(p1.begin, p2.begin) and end = max(p1.end , p2.end).
	End Rem
	Method span:TLocalTimePeriod(period:TLocalTimePeriod)
		Return _create(bmx_local_time_period_span(localTimePeriodPtr, period.localTimePeriodPtr))
	End Method
	
	Rem
	bbdoc: Add duration to both begin and end.
	End Rem
	Method shift(duration:TTimeDuration)
		bmx_local_time_period_shift(localTimePeriodPtr, duration.durationPtr)
	End Method
	
	Rem
	bbdoc: True if this local time period is less than the parameter.
	End Rem
	Method isLess:Int(period:TLocalTimePeriod)
		Return bmx_local_time_period_less(localTimePeriodPtr, period.localTimePeriodPtr)
	End Method

	Rem
	bbdoc: True if this local time period is greater than the parameter.
	End Rem
	Method isGreater:Int(period:TLocalTimePeriod)
		Return bmx_local_time_period_greater(localTimePeriodPtr, period.localTimePeriodPtr)
	End Method

	Rem
	bbdoc: True if the two local time periods are equal.
	about: Periods are equal if ltp1.begin = ltp2.begin and ltp1.last = ltp2.last
	End Rem
	Method isEqual:Int(period:TLocalTimePeriod)
		Return bmx_local_time_period_equal(localTimePeriodPtr, period.localTimePeriodPtr)
	End Method

	' for sorting !
	Method compare:Int(obj:Object)
	
		If TLocalTimePeriod(obj) Then
			If isLess(TLocalTimePeriod(obj)) Then Return -1
			If isEqual(TLocalTimePeriod(obj)) Then Return 0
			Return 1
		End If
		
		Return Super.compare(obj)
	End Method
	
	Method Delete()
		If localTimePeriodPtr Then
			bmx_local_time_period_delete(localTimePeriodPtr)
			localTimePeriodPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A posix #TTimeZone is a set of data and rules that provide information about a time zone.
about: Information includes the offset from UTC, its name and abbreviation, as well as daylight
savings rules.
<p>A posix time zone is unique in that the object is created from a Posix time zone string
(IEEE Std 1003.1). A POSIX time zone string takes the form of:
</p>
<pre>    "std offset dst [offset],start[/time],end[/time]" (with no spaces).</pre>
<p>'std' specifies the abbrev of the time zone. 'offset' is the offset from UTC. 'dst' specifies the
abbrev of the time zone during daylight savings time. The second offset is how many hours changed during
DST. 'start' and 'end' are the dates when DST goes into (and out of) effect. 'offset' takes the form of: 
</p>
<pre>    [+|-]hh[:mm[:ss]] {h=0-23, m/s=0-59}</pre>
<p>'time' and 'offset' take the same form. 'start' and 'end' can be one of three forms: 
</p>
<pre>
    Mm.w.d {month=1-12, week=1-5 (5 is always last), day=0-6}
    Jn {n=1-365 Feb29 is never counted}
    n {n=0-365 Feb29 is counted in leap years}
</pre>
<p>Exceptions will be thrown under the following conditions: </p>
<ul type="disc">
<li>An exception will be thrown for an invalid date spec (see
<a href="gregorian.html#date_time.gregorian.date_class" title="Date">date class</a>).</li>
<li>A boost::local_time::bad_offset exception will be thrown for:</li>
<li>A DST start or end offset that is negative or more than 24 hours.</li>
<li>A UTC zone that is greater than +12 or less than -12 hours.</li>
<li>A boost::local_time::bad_adjustment exception will be thrown for a DST adjustment that is 24 hours or
more (positive or negative)</li>
</ul>
<p>As stated above, the 'offset' and '/time' portions of the string are not required. If they are not
given they default to 01:00 for 'offset', and 02:00 for both occurrences of '/time'.
</p>
<p>
Some examples are:</p>
<pre>
    "PST-8PDT01:00:00,M4.1.0/02:00:00,M10.1.0/02:00:00"
    "PST-8PDT,M4.1.0,M10.1.0"
</pre>
<p>These two are actually the same specification (defaults were used in the second string). 
This zone lies eight hours west of GMT and makes a one hour shift forward during daylight savings
time. Daylight savings for this zone starts on the first Sunday of April at 2am, and ends
on the first Sunday of October at 2am.
</p>
<pre>    "MST-7"</pre>
<p>This zone is as simple as it gets. This zone lies seven hours west of GMT and has no daylight savings.
</p>
<pre>    "EST10EDT,M10.5.0,M3.5.0/03"</pre>
<p>This string describes the time zone for Sydney Australia. It lies ten hours east of GMT and makes
a one hour shift forward during daylight savings. Being located in the southern hemisphere, daylight
savings begins on the last Sunday in October at 2am and ends on the last Sunday in March at 3am.
</p>
<pre>"FST+3FDT02:00,J60/00,J304/02"</pre>
<p>This specification describes a fictitious zone that lies three hours east of GMT. It makes a two hour
shift forward for daylight savings which begins on March 1st at midnight, and ends on October 31st at
2am. The 'J' designation in the start/end specs signifies that counting starts at one and February
29th is never counted.
</p>
<pre>"FST+3FDT,59,304"</pre>
<p>This specification is significant because of the '59'. The lack of 'J' for the start and end dates,
indicates that the Julian day-count begins at zero and ends at 365. If you do the math, you'll see
that allows for a total of 366 days. This is fine in leap years, but in non-leap years '59' (Feb-29)
does not exist. This will construct a valid posix_time_zone object but an exception will be thrown if
the date of '59' is accessed in a non-leap year. Ex:
</p>
<pre>posix_time_zone leap_day(std::string("FST+3FDT,59,304"));
leap_day.dst_local_start_time(2004); // ok
leap_day.dst_local_start_time(2003); // Exception thrown
</pre>
End Rem
Type TTimeZone

	Field timeZonePtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function Create:TTimeZone(description:String)
		Return _create(bmx_posix_time_zone(description))
	End Function
	
	Function _create:TTimeZone(timeZonePtr:Byte Ptr)
		Local this:TTimeZone = New TTimeZone
		
		this.timeZonePtr = timeZonePtr
	
		Return this
	End Function
	
	Rem
	bbdoc: Returns the daylight savings abbreviation for the represented time zone.
	End Rem
	Method DSTZoneAbbrev:String()
		Return bmx_time_zone_dst_zone_abbrev(timeZonePtr)
	End Method

	Rem
	bbdoc: Returns the standard abbreviation for the represented time zone.
	End Rem
	Method STDZoneAbbrev:String()
		Return bmx_time_zone_std_zone_abbrev(timeZonePtr)
	End Method

	Rem
	bbdoc: Returns the daylight savings name for the represented time zone.
	End Rem
	Method DSTZoneName:String()
		Return bmx_time_zone_dst_zone_name(timeZonePtr)
	End Method

	Rem
	bbdoc: Returns the standard name for the represented time zone.
	End Rem
	Method STDZoneName:String()
		Return bmx_time_zone_std_zone_name(timeZonePtr)
	End Method
	
	Rem
	bbdoc: Returns True if this time zone makes a daylight savings shift.
	End Rem
	Method hasDST:Int()
		Return bmx_time_zone_has_dst(timeZonePtr)
	End Method
	
	Rem
	bbdoc: The date and time daylight savings time begins in given year.
	End Rem
	Method DSTLocalStartTime:TTime(year:Int)
		Return TTime._create(bmx_time_zone_dst_local_start_time(timeZonePtr, year))
	End Method

	Rem
	bbdoc: The date and time daylight savings time ends in given year.
	End Rem
	Method DSTLocalEndTime:TTime(year:Int)
		Return TTime._create(bmx_time_zone_dst_local_end_time(timeZonePtr, year))
	End Method
	
	Rem
	bbdoc: The amount of time offset from UTC (typically in hours).
	End Rem
	Method baseUTCOffset:TTimeDuration()
		Return TTimeDuration._create(bmx_time_zone_base_utc_offset(timeZonePtr))
	End Method
	
	Rem
	bbdoc: The amount of time shifted during daylight savings.
	End Rem
	Method DSTOffset:TTimeDuration()
		Return TTimeDuration._create(bmx_time_zone_dst_offset(timeZonePtr))
	End Method
	
	Rem
	bbdoc: Returns a posix time zone string representation of this time zone object.
	End Rem
	Method toString:String()
		Return bmx_time_zone_to_posix_string(timeZonePtr)
	End Method
	
End Type


Rem
bbdoc: Date Generator base type for #TLastDayOfWeekInMonth, #TFirstDayOfWeekInMonth, #TNthDayOfWeekInMonth and #TPartialDate.
End Rem
Type TYearBasedGenerator Abstract

	Field ybgPtr:Byte Ptr
	
	Rem
	bbdoc: Returns the date for the specified @year.
	End Rem
	Method getDate:TDate(year:Int) Abstract
	
End Type


Rem
bbdoc: Date Generator for a partial date.
End Rem
Type TPartialDate Extends TYearBasedGenerator
	
	Rem
	bbdoc: Creates a new #TPartialDate for the given day and month.
	End Rem
	Function Create:TPartialDate(day:Int, Month:Int)
		Local this:TPartialDate = New TPartialDate
		
		this.ybgPtr = bmx_partial_date_new(day, Month)
	
		Return this
	End Function

	Method getDate:TDate(year:Int)
		Return TDate._create(bmx_partial_date_get_date(ybgPtr, year))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_partial_date_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Date Generator for last day of the week in month.
End Rem
Type TLastDayOfWeekInMonth Extends TYearBasedGenerator
	
	Rem
	bbdoc: Creates a new #TLastDayOfWeekInMonth for the given weekday and month.
	about: A weekday may be one of #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday,
	#Friday or #Saturday.<br>
	Month may be one of #Jan, #Feb, #Mar, #Apr, #May, #Jun, #Jul, #Aug, #Sep, #Oct, 
	#Nov or #Dec.
	End Rem
	Function Create:TLastDayOfWeekInMonth(WeekDay:Int, Month:Int)
		Local this:TLastDayOfWeekInMonth = New TLastDayOfWeekInMonth
		
		this.ybgPtr = bmx_last_day_of_week_in_month_new(WeekDay, Month)
	
		Return this
	End Function

	Method getDate:TDate(year:Int)
		Return TDate._create(bmx_last_day_of_week_in_month_get_date(ybgPtr, year))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_last_day_of_week_in_month_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Date Generator for first day of the week in month.
End Rem
Type TFirstDayOfWeekInMonth Extends TYearBasedGenerator
	
	Rem
	bbdoc: Creates a new #TFirstDayOfWeekInMonth for the given weekday and month.
	about: A weekday may be one of #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday,
	#Friday or #Saturday.<br>
	Month may be one of #Jan, #Feb, #Mar, #Apr, #May, #Jun, #Jul, #Aug, #Sep, #Oct, 
	#Nov or #Dec.
	End Rem
	Function Create:TFirstDayOfWeekInMonth(WeekDay:Int, Month:Int)
		Local this:TFirstDayOfWeekInMonth = New TFirstDayOfWeekInMonth
		
		this.ybgPtr = bmx_first_day_of_week_in_month_new(WeekDay, Month)
	
		Return this
	End Function

	Method getDate:TDate(year:Int)
		Return TDate._create(bmx_first_day_of_week_in_month_get_date(ybgPtr, year))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_first_day_of_week_in_month_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Date Generator for nth day of the week in month.
about: Calculate something like first Monday of January, second Tuesday of March, Third Sunday
of December, etc.
End Rem
Type TNthDayOfWeekInMonth Extends TYearBasedGenerator
	
	Rem
	bbdoc: Creates a new #TNthDayOfWeekInMonth for the given nth, weekday and month.
	about: Nth may be one of #First, #Second, #Third, #Fourth, or #Fifth (1-5).
	A weekday may be one of #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday,
	#Friday or #Saturday (0-6).<br>
	Month may be one of #Jan, #Feb, #Mar, #Apr, #May, #Jun, #Jul, #Aug, #Sep, #Oct, 
	#Nov or #Dec (1-12).
	End Rem
	Function Create:TNthDayOfWeekInMonth(nth:Int, WeekDay:Int, Month:Int)
		Local this:TNthDayOfWeekInMonth = New TNthDayOfWeekInMonth
		
		this.ybgPtr = bmx_nth_day_of_week_in_month_new(nth, WeekDay, Month)
	
		Return this
	End Function

	Method getDate:TDate(year:Int)
		Return TDate._create(bmx_nth_day_of_week_in_month_get_date(ybgPtr, year))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_nth_day_of_week_in_month_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Date Generator for first day of the week after a date.
about: Calculate something like First Sunday after Jan 1,2002.
End Rem
Type TFirstDayOfWeekAfter

	Field ybgPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new #TFirstDayOfWeekAfter for the given weekday.
	about: A weekday may be one of #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday,
	#Friday or #Saturday (0-6).
	End Rem
	Function Create:TFirstDayOfWeekAfter(WeekDay:Int)
		Local this:TFirstDayOfWeekAfter = New TFirstDayOfWeekAfter
		
		this.ybgPtr = bmx_first_day_of_week_after_new(WeekDay)
	
		Return this
	End Function

	Rem
	bbdoc: Returns the date for the first day after the specified @date.
	End Rem
	Method getDate:TDate(date:TDate)
		Return TDate._create(bmx_first_day_of_week_after_get_date(ybgPtr, date.datePtr))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_first_day_of_week_after_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Date Generator for first day of the week before a date.
about: Calculate something like First Monday before Feb 1,2002
End Rem
Type TFirstDayOfWeekBefore

	Field ybgPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new #TFirstDayOfWeekBefore for the given weekday.
	about: A weekday may be one of #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday,
	#Friday or #Saturday (0-6).
	End Rem
	Function Create:TFirstDayOfWeekBefore(WeekDay:Int)
		Local this:TFirstDayOfWeekBefore = New TFirstDayOfWeekBefore
		
		this.ybgPtr = bmx_first_day_of_week_before_new(WeekDay)
	
		Return this
	End Function

	Rem
	bbdoc: Returns the date for the first day before the specified @date.
	End Rem
	Method getDate:TDate(date:TDate)
		Return TDate._create(bmx_first_day_of_week_before_get_date(ybgPtr, date.datePtr))
	End Method

	Method Delete()
		If ybgPtr Then
			bmx_first_day_of_week_before_delete(ybgPtr)
			ybgPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: The default date facet
End Rem
Global defaultDateFacet:TDateFacet = TDateFacet.Create()

Rem
bbdoc: The current date facet (for use with toString() type formatting)
End Rem
Global currentDateFacet:TDateFacet = defaultDateFacet

Rem
bbdoc: The default time facet
End Rem
Global defaultTimeFacet:TTimeFacet = TTimeFacet.Create()

Rem
bbdoc: The current time facet (for use with toString() type formatting)
End Rem
Global currentTimeFacet:TTimeFacet = defaultTimeFacet

'SetCurrentDateFacet(defaultDateFacet)
'SetCurrentTimeFacet(defaultTimeFacet)

Type TLocaleFacet
?linux
	Global defaultLocale:String = "en_US"
?win32
	Global defaultLocale:String = "English United States"
?macos
	Global defaultLocale:String = "en_US"
?
	Field localePtr:Byte Ptr
	Field facetPtr:Byte Ptr
	Field locale:String

	Global generator:TBLGenerator
	
	Method New()
		If Not generator Then
			generator = New TBLGenerator.Create()
		End If
	End Method
	
End Type

Rem
bbdoc: The #TDateFacet enables users to have significant control over the output of dates (and other gregorian objects).
End Rem
Type TDateFacet Extends TLocaleFacet

	Function Create:TDateFacet()
		Return CreateForLocale(TLocaleFacet.defaultLocale)
	End Function

	Function CreateForLocale:TDateFacet(locale:String)
		Local this:TDateFacet = New TDateFacet
		
		this.facetPtr = bmx_datefacet_new()
		this.localePtr = bmx_locale_new(this.facetPtr, generator.genPtr, locale)
		this.locale = locale
		
		Return this
	End Function
	
	Rem
	bbdoc: Set the format for dates.
	End Rem
	Method format(fmt:String)
		bmx_date_facet_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Sets the date format to ISO.
	End Rem
	Method setISOFormat()
		bmx_date_facet_set_iso_format(facetPtr)
	End Method
	
	Rem
	bbdoc: Sets the date format to ISO Extended.
	End Rem
	Method setISOExtendedFormat()
		bmx_date_facet_set_iso_extended_format(facetPtr)
	End Method
	
	Rem
	bbdoc: Set the format for months when they are output individually.
	End Rem
	Method monthFormat(fmt:String)
		bmx_date_facet_month_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Set the format for weekdays when they are output individually.
	End Rem
	Method weekdayFormat(fmt:String)
		bmx_date_facet_weekday_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Replace strings used when outputting short months.
	End Rem
	Method setShortMonthNames(names:String[])
		Assert names, "short names array cannot be null"
		Assert names.length = 12, "short names array requires 12 entries"
		
		Local a:Byte Ptr = arrayToCStrings(names)
		
		bmx_date_facet_short_month_names(facetPtr, a)
		
		freeCStringArray(a, 12)
		
	End Method

	Rem
	bbdoc: Replace strings used when outputting long months.
	End Rem
	Method setLongMonthNames(names:String[])
		Assert names, "names array cannot be null"
		Assert names.length = 12, "names array requires 12 entries"

		Local a:Byte Ptr = arrayToCStrings(names)
		
		bmx_date_facet_long_month_names(facetPtr, a)
		
		freeCStringArray(a, 12)

	End Method

	Rem
	bbdoc: Replace strings used when outputting short weekdays.
	End Rem
	Method setShortWeekdayNames(names:String[])
		Assert names, "short weekday names array cannot be null"
		Assert names.length = 7, "short weekday names array requires 12 entries"

		Local a:Byte Ptr = arrayToCStrings(names)
		
		bmx_date_facet_short_weekday_names(facetPtr, a)
		
		freeCStringArray(a, 7)

	End Method
	
	Rem
	bbdoc: Replace strings used when outputting long weekdays.
	End Rem
	Method setLongWeekdayNames(names:String[])	
		Assert names, "weekday names array cannot be null"
		Assert names.length = 7, "weekday names array requires 7 entries"

		Local a:Byte Ptr = arrayToCStrings(names)
		
		bmx_date_facet_long_weekday_names(facetPtr, a)
		
		freeCStringArray(a, 7)

	End Method
	
End Type

Type TTimeFacet Extends TDateFacet

	Function Create:TTimeFacet()
		Return CreateForLocale(defaultLocale)
	End Function

	Function CreateForLocale:TTimeFacet(locale:String)
		Local this:TTimeFacet = New TTimeFacet

		this.facetPtr = bmx_timefacet_new()
		this.localePtr = bmx_locale_new(this.facetPtr, generator.genPtr, locale)
		this.locale = locale
		
		Return this
	End Function
	
	Rem
	bbdoc: Set the format for dates.
	End Rem
	Method format(fmt:String)
		bmx_time_facet_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Sets the date format to ISO.
	End Rem
	Method setISOFormat()
		bmx_time_facet_set_iso_format(facetPtr)
	End Method
	
	Rem
	bbdoc: Sets the date format to ISO Extended.
	End Rem
	Method setISOExtendedFormat()
		bmx_time_facet_set_iso_extended_format(facetPtr)
	End Method
	
	Rem
	bbdoc: Set the format for months when they are output individually.
	End Rem
	Method monthFormat(fmt:String)
		bmx_time_facet_month_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Set the format for weekdays when they are output individually.
	End Rem
	Method weekdayFormat(fmt:String)
		bmx_time_facet_weekday_format(facetPtr, fmt)
	End Method
	
	Rem
	bbdoc: Sets the time_duration format.
	about: The time duration format has the ability to display the sign of the duration.
	The '%+' flag will always display the sign. The '%-' will only display if the sign is negative. Currently the '-' and '+' characters are used to denote the sign.
	End Rem
	Method timeDurationFormat(fmt:String)
		bmx_time_facet_time_duration_format(facetPtr, fmt)
	End Method
	
End Type

Rem
bbdoc: Convenience function for TTimeDuration.Hour().
End Rem
Function TDHours:TTimeDuration(amount:Int)
	Return TTimeDuration.Hour(amount)
End Function

Rem
bbdoc: Convenience function for TTimeDuration.Minute().
End Rem
Function TDMinutes:TTimeDuration(amount:Int)
	Return TTimeDuration.Minute(amount)
End Function

Rem
bbdoc: Convenience function for TTimeDuration.Second().
End Rem
Function TDSeconds:TTimeDuration(amount:Int)
	Return TTimeDuration.Second(amount)
End Function

Rem
bbdoc: Sets the current date facet.
about: This controls the appearance of date information.
End Rem
Function SetCurrentDateFacet(facet:TDateFacet)
	currentDateFacet = facet
End Function

Rem
bbdoc: Sets the current time facet.
about: This controls the appearance of time and time duration information.
End Rem
Function SetCurrentTimeFacet(facet:TTimeFacet)
	currentTimeFacet = facet
End Function

Rem
bbdoc: Return the number of ticks in a second.
about: For example, if the duration supports nanoseconds then the returned result will be 1,000,000,000 (1e+9).
End Rem
Function TicksPerSecond:Int()
	Return bmx_time_ticks_per_second()
End Function

Rem
bbdoc: Returns the number of fractional digits the time resolution has.
about: 9 for nano, 6 for micro, etc.
End Rem
Function NumFractionalDigits:Int()
	Return bmx_time_num_fractional_digits()
End Function

Rem
bbdoc: Get the weekday text for the specified @weekday (0 - 6).
about: This is based on the current weekday format as specified by #TDateFacet.<br>
Valid weekdays include #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday, #Friday, #Saturday (0-6).
End Rem
Function WeekDay:String(WeekDay:Int)
	Return bmx_weekday_to_string(WeekDay)
End Function

Rem
bbdoc: Get the month text for the specified @month (1 - 12).
about: This is based on the current month format as specified by #TDateFacet.
End Rem
Function Month:String(Month:Int)
	Return bmx_month_to_string(Month)
End Function

Rem
bbdoc: Calculates the number of days from given date until given @weekday.
about: Valid weekdays include #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday, #Friday, #Saturday (0-6).
End Rem
Function DaysUntilWeekday:Int(date:TDate, WeekDay:Int)
	Return bmx_days_until_weekday(date.datePtr, WeekDay)
End Function

Rem
bbdoc: Calculates the number of day from given date to previous given @weekday.
about: Valid weekdays include #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday, #Friday, #Saturday (0-6).
End Rem
Function DaysBeforeWeekday:Int(date:TDate, WeekDay:Int)
	Return bmx_days_before_weekday(date.datePtr, WeekDay)
End Function

Rem
bbdoc: Generates a #TDate object representing the date of the following @weekday from the given @date.
about: Valid weekdays include #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday, #Friday, #Saturday (0-6).
End Rem
Function NextWeekday:TDate(date:TDate, WeekDay:Int)
	Return TDate._create(bmx_next_weekday(date.datePtr, WeekDay))
End Function

Rem
bbdoc: Generates a #TDate object representing the date of the previous @weekday from the given @date.
about: Valid weekdays include #Sunday, #Monday, #Tuesday, #Wednesday, #Thursday, #Friday, #Saturday (0-6).
End Rem
Function PreviousWeekday:TDate(date:TDate, WeekDay:Int)
	Return TDate._create(bmx_previous_weekday(date.datePtr, WeekDay))
End Function

Rem
bbdoc: 
End Rem
Function EndOfMonthDay:Int(year:Int, Month:Int)
	Return bmx_end_of_month_day(year, Month)
End Function

Rem
bbdoc: Week Day - Sunday
End Rem
Const Sunday:Int = 0
Rem
bbdoc: Week Day - Monday
End Rem
Const Monday:Int = 1
Rem
bbdoc: Week Day - Tuesday
End Rem
Const Tuesday:Int = 2
Rem
bbdoc: Week Day - Wednesday
End Rem
Const Wednesday:Int = 3
Rem
bbdoc: Week Day - Thursday
End Rem
Const Thursday:Int = 4
Rem
bbdoc: Week Day - Friday
End Rem
Const Friday:Int = 5
Rem
bbdoc: Week Day - Saturday
End Rem
Const Saturday:Int = 6

Rem
bbdoc: Month - January
End Rem
Const Jan:Int = 1
Rem
bbdoc: Month - February
End Rem
Const Feb:Int = 2
Rem
bbdoc: Month - March
End Rem
Const Mar:Int = 3
Rem
bbdoc: Month - April
End Rem
Const Apr:Int = 4
Rem
bbdoc: Month - May
End Rem
Const May:Int = 5
Rem
bbdoc: Month - June
End Rem
Const Jun:Int = 6
Rem
bbdoc: Month - July
End Rem
Const Jul:Int = 7
Rem
bbdoc: Month - August
End Rem
Const Aug:Int = 8
Rem
bbdoc: Month - September
End Rem
Const Sep:Int = 9
Rem
bbdoc: Month - October
End Rem
Const Oct:Int = 10
Rem
bbdoc: Month - November
End Rem
Const Nov:Int = 11
Rem
bbdoc: Month - December
End Rem
Const Dec:Int = 12

Rem
bbdoc: Nth Day - First
End Rem
Const First:Int = 1
Rem
bbdoc: Nth Day - Second
End Rem
Const Second:Int = 2
Rem
bbdoc: Nth Day - Third
End Rem
Const Third:Int = 3
Rem
bbdoc: Nth Day - Fourth
End Rem
Const Fourth:Int = 4
Rem
bbdoc: Nth Day - Fifth
End Rem
Const Fifth:Int = 5


