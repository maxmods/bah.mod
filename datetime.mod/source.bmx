' Copyright (c) 2007-2009 Bruce A Henderson
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

Import BRL.Blitz
Import Pub.StdC

Import "../boost.mod/src/*.hpp"

Import "lib/src/gregorian/*.hpp"

Import "lib/src/gregorian/date_generators.cpp"
Import "lib/src/gregorian/greg_month.cpp"
Import "lib/src/gregorian/greg_weekday.cpp"
Import "lib/src/gregorian/gregorian_types.cpp"

Import "glue.cpp"

Extern

	Function bmx_datetime_newdate:Byte Ptr(year:Int, Month:Int, day:Int)
	Function bmx_datetime_delete(date:Byte Ptr)

	Function bmx_datetime_before:Int(myDate:Byte Ptr, yourDate:Byte Ptr)
	Function bmx_datetime_after:Int(myDate:Byte Ptr, yourDate:Byte Ptr)
	Function bmx_datetime_equals:Int(myDate:Byte Ptr, yourDate:Byte Ptr)


	Function bmx_datetime_localday:Byte Ptr()
	Function bmx_datetime_universalday:Byte Ptr()
	
	Function bmx_datetime_year:Int(date:Byte Ptr)
	Function bmx_datetime_month:Int(date:Byte Ptr)
	Function bmx_datetime_day:Int(date:Byte Ptr)
	Function bmx_datetime_ymd(date:Byte Ptr, year:Int Ptr, Month:Int Ptr, day:Int Ptr)
	
	Function bmx_datetime_fromstring:Byte Ptr(date:Byte Ptr)
	Function bmx_datetime_fromundelimitedstring:Byte Ptr(date:Byte Ptr)
	
	Function bmx_datetime_day_of_week:Int(date:Byte Ptr)
	Function bmx_datetime_day_of_year:Int(date:Byte Ptr)
	Function bmx_datetime_end_of_month:Byte Ptr(date:Byte Ptr)
	Function bmx_datetime_week_number:Int(date:Byte Ptr)
	Function bmx_datetime_date_add:Byte Ptr(date:Byte Ptr, duration:Int)
	Function bmx_datetime_date_subtract:Byte Ptr(date:Byte Ptr, duration:Int)
	Function bmx_datetime_date_subdate:Int(date:Byte Ptr, when:Byte Ptr)
	
	Function bmx_datetime_period_datedate:Byte Ptr(d1:Byte Ptr, d2:Byte Ptr)
	Function bmx_datetime_period_withdays:Byte Ptr(date:Byte Ptr, days:Int)
	Function bmx_datetime_period_shift(period:Byte Ptr, days:Int)
	Function bmx_datetime_period_delete(period:Byte Ptr)
	
	Function bmx_datetime_period_begin:Byte Ptr(period:Byte Ptr)
	Function bmx_datetime_period_last:Byte Ptr(period:Byte Ptr)
	Function bmx_datetime_period_end:Byte Ptr(period:Byte Ptr)
	Function bmx_datetime_period_length:Int(period:Byte Ptr)
	
	Function bmx_datetime_to_simple_string:String(date:Byte Ptr)
	Function bmx_datetime_period_to_string:String(period:Byte Ptr)
	Function bmx_datetime_to_iso_string:String(date:Byte Ptr)
	Function bmx_datetime_to_iso_extended_string:String(date:Byte Ptr)
	
	Function bmx_datetime_period_isnull:Int(period:Byte Ptr)
	Function bmx_datetime_period_containsdate:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_contains:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_intersects:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_intersection:Byte Ptr(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_adjacent:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_after:Int(p1:Byte Ptr, d:Byte Ptr)
	Function bmx_datetime_period_before:Int(p:Byte Ptr, d:Byte Ptr)
	Function bmx_datetime_period_merge:Byte Ptr(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_span:Byte Ptr(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_isless:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_isgreater:Int(p1:Byte Ptr, p2:Byte Ptr)
	Function bmx_datetime_period_isequal:Int(p1:Byte Ptr, p2:Byte Ptr)

	Function bmx_datetime_iter_delete(iter:Byte Ptr)
	
	Function bmx_datetime_dayiter:Byte Ptr(date:Byte Ptr, offset:Int)
	Function bmx_datetime_monthiter:Byte Ptr(date:Byte Ptr, offset:Int)
	Function bmx_datetime_yeariter:Byte Ptr(date:Byte Ptr, offset:Int)
	
	Function bmx_datetime_iter_forward(iter:Byte Ptr)
	Function bmx_datetime_iter_backward(iter:Byte Ptr)
	
	Function bmx_datetime_iter_before:Int(myDate:Byte Ptr, yourDate:Byte Ptr)
	Function bmx_datetime_iter_after:Int(myDate:Byte Ptr, yourDate:Byte Ptr)
	Function bmx_datetime_iter_equals:Int(myDate:Byte Ptr, yourDate:Byte Ptr)
	Function bmx_datetime_iter_year:Int(date:Byte Ptr)
	Function bmx_datetime_iter_month:Int(date:Byte Ptr)
	Function bmx_datetime_iter_day:Int(date:Byte Ptr)
	Function bmx_datetime_iter_ymd(date:Byte Ptr, year:Int Ptr, Month:Int Ptr, day:Int Ptr)
	Function bmx_datetime_iter_day_of_week:Int(date:Byte Ptr)
	Function bmx_datetime_iter_day_of_year:Int(date:Byte Ptr)
	Function bmx_datetime_iter_end_of_month:Byte Ptr(date:Byte Ptr)
	Function bmx_datetime_iter_week_number:Int(date:Byte Ptr)
	Function bmx_datetime_iter_date_add:Byte Ptr(date:Byte Ptr, duration:Int)
	Function bmx_datetime_iter_date_subtract:Byte Ptr(date:Byte Ptr, duration:Int)
	Function bmx_datetime_iter_date_subdate:Int(date:Byte Ptr, when:Byte Ptr)
	Function bmx_datetime_iter_to_simple_string:String(date:Byte Ptr)
	Function bmx_datetime_iter_to_iso_string:String(date:Byte Ptr)
	Function bmx_datetime_iter_to_iso_extended_string:String(date:Byte Ptr)
	Function bmx_datetime_iter_to_string:String(date:Byte Ptr)
	Function bmx_datetime_iter_asformat:String(date:Byte Ptr, f:Byte Ptr)

	Function bmx_time_duration:Byte Ptr(hours:Int, minutes:Int, seconds:Int, fraction:Int)
	Function bmx_time_duration_delete(duration:Byte Ptr)
	Function bmx_time_duration_new_hours:Byte Ptr(value:Int)
	Function bmx_time_duration_new_minutes:Byte Ptr(value:Int)
	Function bmx_time_duration_new_seconds:Byte Ptr(value:Int)
	Function bmx_time_duration_new_milliseconds:Byte Ptr(value:Int)
	Function bmx_time_duration_hours:Int(duration:Byte Ptr)
	Function bmx_time_duration_minutes:Int(duration:Byte Ptr)
	Function bmx_time_duration_seconds:Int(duration:Byte Ptr)
	Function bmx_time_duration_total_seconds:Int(duration:Byte Ptr)
	Function bmx_time_duration_total_milliseconds:Int(duration:Byte Ptr)
	Function bmx_time_duration_fractional_seconds:Int(duration:Byte Ptr)
	Function bmx_time_duration_is_negative:Int(duration:Byte Ptr)
	Function bmx_time_duration_invert_sign:Byte Ptr(duration:Byte Ptr)
	Function bmx_time_duration_to_string:String(duration:Byte Ptr)
	Function bmx_time_duration_to_iso_string:String(duration:Byte Ptr)
	Function bmx_time_duration_add:Byte Ptr(d1:Byte Ptr, d2:Byte Ptr)
	Function bmx_time_duration_subtract:Byte Ptr(d1:Byte Ptr, d2:Byte Ptr)
	Function bmx_time_duration_divide:Byte Ptr(duration:Byte Ptr, value:Int)
	Function bmx_time_duration_multiply:Byte Ptr(duration:Byte Ptr, value:Int)
	Function bmx_time_duration_less:Int(d1:Byte Ptr, d2:Byte Ptr)
	Function bmx_time_duration_greater:Int(d1:Byte Ptr, d2:Byte Ptr)
	Function bmx_time_duration_equal:Int(d1:Byte Ptr, d2:Byte Ptr)
	
	Function bmx_time_ticks_per_second:Int()
	Function bmx_time_num_fractional_digits:Int()
	
	Function bmx_ptime_new:Byte Ptr(date:Byte Ptr, duration:Byte Ptr)
	Function bmx_ptime_delete(time:Byte Ptr)
	Function bmx_ptime_local_new:Byte Ptr()
	Function bmx_ptime_universal_new:Byte Ptr()
	Function bmx_ptime_local_microsecond_new:Byte Ptr()
	Function bmx_ptime_universal_microsecond_new:Byte Ptr()
	Function bmx_ptime_date:Byte Ptr(time:Byte Ptr)
	Function bmx_ptime_time_of_day:Byte Ptr(time:Byte Ptr)
	Function bmx_ptime_to_string:String(time:Byte Ptr)
	Function bmx_ptime_to_iso_string:String(time:Byte Ptr)
	Function bmx_ptime_to_iso_extended_string:String(time:Byte Ptr)
	Function bmx_ptime_add_days:Byte Ptr(time:Byte Ptr, days:Int)
	Function bmx_ptime_subtract_days:Byte Ptr(time:Byte Ptr, days:Int)
	Function bmx_ptime_add_duration:Byte Ptr(time:Byte Ptr, duration:Byte Ptr)
	Function bmx_ptime_subtract_duration:Byte Ptr(time:Byte Ptr, duration:Byte Ptr)
	Function bmx_ptime_subtract:Byte Ptr(t1:Byte Ptr, t2:Byte Ptr)
	Function bmx_ptime_less:Int(t1:Byte Ptr, t2:Byte Ptr)
	Function bmx_ptime_greater:Int(t1:Byte Ptr, t2:Byte Ptr)
	Function bmx_ptime_equal:Int(t1:Byte Ptr, t2:Byte Ptr)
	Function bmx_ptime_from_time_t:Byte Ptr(time:Byte Ptr)
	
	Function bmx_partial_date_new:Byte Ptr(day:Int, Month:Int)
	Function bmx_partial_date_get_date:Byte Ptr(ybg:Byte Ptr, year:Int)
	Function bmx_partial_date_delete(ybg:Byte Ptr)
	
	Function bmx_last_day_of_week_in_month_new:Byte Ptr(WeekDay:Int, Month:Int)
	Function bmx_last_day_of_week_in_month_get_date:Byte Ptr(ybg:Byte Ptr, year:Int)
	Function bmx_last_day_of_week_in_month_delete(ybg:Byte Ptr)

	Function bmx_first_day_of_week_in_month_new:Byte Ptr(WeekDay:Int, Month:Int)
	Function bmx_first_day_of_week_in_month_get_date:Byte Ptr(ybg:Byte Ptr, year:Int)
	Function bmx_first_day_of_week_in_month_delete(ybg:Byte Ptr)
	
	Function bmx_weekday_to_string:String(WeekDay:Int)
	
	Function bmx_datefacet_new:Byte Ptr()
	Function bmx_locale_new:Byte Ptr(facet:Byte Ptr, locale:Byte Ptr)
	Function bmx_datefacet_setcurrent(locale:Byte Ptr, facet:Byte Ptr)
	Function bmx_date_asformat:String(date:Byte Ptr, format:Byte Ptr)

	Function _strlen:Int(s:Byte Ptr) = "strlen"
	Function bmx_char_free(s:Byte Ptr)
	
	Function bmx_time_period_timetime:Byte Ptr(beginTime:Byte Ptr, endTime:Byte Ptr)
	Function bmx_time_period_withduration:Byte Ptr(beginTime:Byte Ptr, duration:Byte Ptr)
	Function bmx_time_period_shift(time:Byte Ptr, duration:Byte Ptr)
	Function bmx_time_period_begin:Byte Ptr(time:Byte Ptr)
	Function bmx_time_period_delete(time:Byte Ptr)
	Function bmx_time_period_last:Byte Ptr(time:Byte Ptr)
	Function bmx_time_period_end:Byte Ptr(time:Byte Ptr)
	Function bmx_time_period_length:Byte Ptr(time:Byte Ptr)
	Function bmx_time_period_is_null:Int(time:Byte Ptr)
	Function bmx_time_period_contains:Int(tp:Byte Ptr, time:Byte Ptr)
	Function bmx_time_period_containsPeriod:Int(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_intersects:Int(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_intersection:Byte Ptr(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_merge:Byte Ptr(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_span:Byte Ptr(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_isless:Int(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_isgreater:Int(tp1:Byte Ptr, tp2:Byte Ptr)
	Function bmx_time_period_isequal:Int(tp1:Byte Ptr, tp2:Byte Ptr)
	
	Function bmx_posix_time_zone:Byte Ptr(id:Byte Ptr)
	Function bmx_time_zone_dst_zone_abbrev:String(tz:Byte Ptr)
	Function bmx_time_zone_std_zone_abbrev:String(tz:Byte Ptr)
	Function bmx_time_zone_dst_zone_name:String(tz:Byte Ptr)
	Function bmx_time_zone_std_zone_name:String(tz:Byte Ptr)
	Function bmx_time_zone_has_dst:Int(tz:Byte Ptr)
	Function bmx_time_zone_dst_local_start_time:Byte Ptr(tz:Byte Ptr, year:Int)
	Function bmx_time_zone_dst_local_end_time:Byte Ptr(tz:Byte Ptr, year:Int)
	Function bmx_time_zone_base_utc_offset:Byte Ptr(tz:Byte Ptr)
	Function bmx_time_zone_dst_offset:Byte Ptr(tz:Byte Ptr)
	Function bmx_time_zone_to_posix_string:String(tz:Byte Ptr)
	
	Function bmx_tz_database:Byte Ptr()
	Function bmx_tz_load_from_file:Byte Ptr(filename:Byte Ptr)
	Function bmx_tz_time_zone_from_region:Byte Ptr(db:Byte Ptr, id:Byte Ptr)
	
	Function bmx_local_date_time_new_sec_clock:Byte Ptr(zone:Byte Ptr)
	Function bmx_local_date_time_new_time:Byte Ptr(time:Byte Ptr, zone:Byte Ptr)
	
	Function bmx_month_to_string:String(Month:Int)
	Function bmx_date_facet_format(facet:Byte Ptr, fmt:Byte Ptr)
	Function bmx_date_facet_set_iso_format(facet:Byte Ptr)
	Function bmx_date_facet_set_iso_extended_format(facet:Byte Ptr)
	
	Function bmx_datetime_to_string:String(datetime:Byte Ptr)
	Function bmx_date_facet_short_month_names(facet:Byte Ptr, names:Byte Ptr)
	Function bmx_date_facet_long_month_names(facet:Byte Ptr, names:Byte Ptr)
	Function bmx_date_facet_short_weekday_names(facet:Byte Ptr, names:Byte Ptr)
	Function bmx_date_facet_long_weekday_names(facet:Byte Ptr, names:Byte Ptr)
	Function bmx_date_facet_month_format(facet:Byte Ptr, fmt:Byte Ptr)
	Function bmx_date_facet_weekday_format(facet:Byte Ptr, fmt:Byte Ptr)
	
	Function bmx_nth_day_of_week_in_month_new:Byte Ptr(nth:Int, WeekDay:Int, Month:Int)
	Function bmx_nth_day_of_week_in_month_get_date:Byte Ptr(ybg:Byte Ptr, year:Int)
	Function bmx_nth_day_of_week_in_month_delete(ybg:Byte Ptr)
	
	Function bmx_first_day_of_week_after_new:Byte Ptr(WeekDay:Int)
	Function bmx_first_day_of_week_after_get_date:Byte Ptr(ybg:Byte Ptr, date:Byte Ptr)
	Function bmx_first_day_of_week_after_delete(ybg:Byte Ptr)
	Function bmx_first_day_of_week_before_new:Byte Ptr(WeekDay:Int)
	Function bmx_first_day_of_week_before_get_date:Byte Ptr(ybg:Byte Ptr, date:Byte Ptr)
	Function bmx_first_day_of_week_before_delete(ybg:Byte Ptr)
	
	Function bmx_days_until_weekday:Int(date:Byte Ptr, WeekDay:Int)
	Function bmx_days_before_weekday:Int(date:Byte Ptr, WeekDay:Int)
	Function bmx_next_weekday:Byte Ptr(date:Byte Ptr, WeekDay:Int)
	Function bmx_previous_weekday:Byte Ptr(date:Byte Ptr, WeekDay:Int)
	
	Function bmx_local_date_time_zone:Byte Ptr(ldt:Byte Ptr)
	Function bmx_local_date_time_is_dst:Int(ldt:Byte Ptr)
	Function bmx_local_date_time_utc_time:Byte Ptr(ldt:Byte Ptr)
	Function bmx_local_date_time_local_time:Byte Ptr(ldt:Byte Ptr)
	
	Function bmx_local_date_time_to_string:String(ldt:Byte Ptr)
	Function bmx_local_date_time_less:Int(ldt1:Byte Ptr, ldt2:Byte Ptr)
	Function bmx_local_date_time_greater:Int(ldt1:Byte Ptr, ldt2:Byte Ptr)
	Function bmx_local_date_time_equal:Int(ldt1:Byte Ptr, ldt2:Byte Ptr)
	
	Function bmx_local_date_time_add_days:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_add_months:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_add_years:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_subtract_days:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_subtract_months:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_subtract_years:Byte Ptr(ldt:Byte Ptr, value:Int)
	Function bmx_local_date_time_add_duration:Byte Ptr(ldt:Byte Ptr, duration:Byte Ptr)
	Function bmx_local_date_time_subtract_duration:Byte Ptr(ldt:Byte Ptr, duration:Byte Ptr)
	Function bmx_local_date_time_delete(ldt:Byte Ptr)
	
	Function bmx_local_time_period_new:Byte Ptr(ldt1:Byte Ptr, ldt2:Byte Ptr)
	Function bmx_local_time_period_new_duration:Byte Ptr(ldt:Byte Ptr, duration:Byte Ptr)
	Function bmx_local_time_period_delete(ltp:Byte Ptr)
	Function bmx_local_time_period_begin:Byte Ptr(ltp:Byte Ptr)
	Function bmx_local_time_period_last:Byte Ptr(ltp:Byte Ptr)
	Function bmx_local_time_period_end:Byte Ptr(ltp:Byte Ptr)
	Function bmx_local_time_period_length:Byte Ptr(ltp:Byte Ptr)
	Function bmx_local_time_period_is_null:Int(ltp:Byte Ptr)
	Function bmx_local_time_period_contains_time:Int(ltp:Byte Ptr, time:Byte Ptr)
	Function bmx_local_time_period_contains:Int(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_intersects:Int(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_intersection:Byte Ptr(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_merge:Byte Ptr(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_span:Byte Ptr(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_shift(ltp:Byte Ptr, duration:Byte Ptr)
	Function bmx_local_time_period_less:Int(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_greater:Int(ltp1:Byte Ptr, ltp2:Byte Ptr)
	Function bmx_local_time_period_equal:Int(ltp1:Byte Ptr, ltp2:Byte Ptr)

	Function bmx_end_of_month_day:Int(y:Int, m:Int)
	
End Extern

' Convert from utf8 to Max
Function convertUTF8toISO8859:String(s:String)

	Local l:Int = s.length

	Local b:Short[] = New Short[l]
	Local bc:Int = -1
	Local c:Int
	Local d:Int
	Local e:Int
	For Local i:Int = 0 Until l
		bc:+1
		c = s[i]
		If c<128 
			b[bc] = c
			Continue
		End If
		i:+1
		d=s[i]
		If c<224 
			b[bc] = (c-192)*64+(d-128)
			Continue
		End If
		i:+1
		e = s[i]
		If c < 240 
			b[bc] = (c-224)*4096+(d-128)*64+(e-128)
			If b[bc] = 8233 Then
				b[bc] = 10
			End If
			Continue
		End If
	Next

	Return String.fromshorts(b, bc + 1)
End Function

Function convertISO8859toUTF8:String(text:String)
	If Not text Then
		Return ""
	End If
	
	Local l:Int = text.length
	If l = 0 Then
		Return ""
	End If
	
	Local count:Int = 0
	Local s:Byte[] = New Byte[l * 3]
	
	For Local i:Int = 0 Until l
		Local char:Int = text[i]

		If char < 128 Then
			s[count] = char
			count:+ 1
			Continue
		Else If char<2048
			s[count] = char/64 | 192
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		Else
			s[count] =  char/4096 | 224
			count:+ 1
			s[count] = char/64 Mod 64 | 128
			count:+ 1
			s[count] = char Mod 64 | 128
			count:+ 1
			Continue
		EndIf
		
	Next

	Return String.fromBytes(s, count)
End Function

Function arrayToCStrings:Byte Ptr(stringArray:String[])

	Local cStrings:Byte Ptr = MemAlloc(4 * stringArray.length)
	Local p:Int Ptr = Int Ptr(cStrings)
		
	For Local i:Int = 0 Until stringArray.length
		
		p[i] = Int(convertISO8859toUTF8(stringArray[i]).toCString())
		
	Next

	Return cStrings

End Function

' custom function to free a previously created "C" array of strings.
Function freeCStringArray(cStrings:Byte Ptr, length:Int)

	Local p:Int Ptr = Int Ptr(cStrings)

	For Local i:Int = 0 Until length
	
		MemFree(Byte Ptr(p[i]))
	
	Next

	MemFree(cStrings)
	
End Function
