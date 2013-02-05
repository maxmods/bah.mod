/*
 Copyright (c) 2007-2013 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <blitz.h>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"

#include <iostream>
#include <algorithm>
#include <vector>

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;

extern "C" {

	date * bmx_datetime_localday();
	date * bmx_datetime_universalday();

	date * bmx_datetime_newdate(int year, int month, int day);	
	void bmx_datetime_delete(date * d);

	bool bmx_datetime_before(date * myDate, date * yourDate);
	bool bmx_datetime_after(date * myDate, date * yourDate);
	bool bmx_datetime_equals(date * myDate, date * yourDate);
	
	int bmx_datetime_year(date * d);
	int bmx_datetime_month(date * d);
	int bmx_datetime_day(date * d);
	void bmx_datetime_ymd(date * dt, int * y, int * m, int * d);
	
	date * bmx_datetime_fromstring(BBString * d);
	date * bmx_datetime_fromundelimitedstring(BBString * d);
	
	int bmx_datetime_day_of_week(date * d);
	int bmx_datetime_day_of_year(date * d);
	date * bmx_datetime_end_of_month(date * d);
	int bmx_datetime_week_number(date * d);
	date * bmx_datetime_date_add(date * d, long duration);
	date * bmx_datetime_date_subtract(date * d, long duration);
	long bmx_datetime_date_subdate(date * d1, date * d2);
	
	date_period * bmx_datetime_period_datedate(date * d1, date * d2);
	date_period * bmx_datetime_period_withdays(date * d, int length);
	void bmx_datetime_period_shift(date_period * p, int length);
	void bmx_datetime_period_delete(date_period * p);
	
	date * bmx_datetime_period_begin(date_period * p);
	date * bmx_datetime_period_last(date_period * p);
	date * bmx_datetime_period_end(date_period * p);
	int bmx_datetime_period_length(date_period * p);
	
	BBString * bmx_datetime_to_simple_string(date * d);
	BBString * bmx_datetime_period_to_simple_string(date_period * p);
	BBString * bmx_datetime_to_iso_string(date * d);
	BBString * bmx_datetime_to_iso_extended_string(date * d);
	BBString * bmx_datetime_to_string(date * d, std::locale * loc, date_facet * facet);
	BBString * bmx_datetime_period_to_string(date_period * p);
	
	bool bmx_datetime_period_isnull(date_period * p);
	bool bmx_datetime_period_containsdate(date_period * p, date * d);
	bool bmx_datetime_period_contains(date_period * p1, date_period * p2);
	bool bmx_datetime_period_intersects(date_period * p1, date_period * p2);
	date_period * bmx_datetime_period_intersection(date_period * p1, date_period * p2);
	bool bmx_datetime_period_adjacent(date_period * p1, date_period * p2);
	bool bmx_datetime_period_after(date_period * p, date * d);
	bool bmx_datetime_period_before(date_period * p, date * d);
	date_period * bmx_datetime_period_merge(date_period * p1, date_period * p2);
	date_period * bmx_datetime_period_span(date_period * p1, date_period * p2);
	bool bmx_datetime_period_isless(date_period * p1, date_period * p2);
	bool bmx_datetime_period_isgreater(date_period * p1, date_period * p2);
	bool bmx_datetime_period_isequal(date_period * p1, date_period * p2);
	
	void bmx_datetime_iter_forward(date_iterator * d);
	void bmx_datetime_iter_backward(date_iterator * d);
	day_iterator * bmx_datetime_dayiter(date * d, int offset);
	month_iterator * bmx_datetime_monthiter(date * d, int offset);
	year_iterator * bmx_datetime_yeariter(date * d, int offset);
	void bmx_datetime_iter_delete(date_iterator * d);

	bool bmx_datetime_iter_before(date_iterator * myDate, date * yourDate);
	bool bmx_datetime_iter_after(date_iterator * myDate, date * yourDate);
	bool bmx_datetime_iter_equals(date_iterator * myDate, date * yourDate);	
	int bmx_datetime_iter_year(date_iterator * d);
	int bmx_datetime_iter_month(date_iterator * d);
	int bmx_datetime_iter_day(date_iterator * d);
	void bmx_datetime_iter_ymd(date_iterator * dt, int * y, int * m, int * d);
	int bmx_datetime_iter_day_of_week(date_iterator * d);
	int bmx_datetime_iter_day_of_year(date_iterator * d);
	date * bmx_datetime_iter_end_of_month(date_iterator * d);
	int bmx_datetime_iter_week_number(date_iterator * d);
	date * bmx_datetime_iter_date_add(date_iterator * d, long duration);
	date * bmx_datetime_iter_date_subtract(date_iterator * d, long duration);
	long bmx_datetime_iter_date_subdate(date_iterator * d1, date * d2);
	BBString * bmx_datetime_iter_to_simple_string(date_iterator * d);
	BBString * bmx_datetime_iter_to_iso_string(date_iterator * d);
	BBString * bmx_datetime_iter_to_iso_extended_string(date_iterator * d);
	BBString * bmx_datetime_iter_to_string(date_iterator * d, std::locale * loc, date_facet * facet);
	BBString * bmx_datetime_iter_asformat(date_iterator * d, BBString * format, std::locale * loc, date_facet * facet);
	
	time_duration * bmx_time_duration(int hours, int minutes, int seconds, int fraction);
	void bmx_time_duration_delete(time_duration * d);
	time_duration * bmx_time_duration_new_hours(int value);
	time_duration * bmx_time_duration_new_minutes(int value);
	time_duration * bmx_time_duration_new_seconds(int value);
	time_duration * bmx_time_duration_new_milliseconds(int value);
	int bmx_time_duration_hours(time_duration * d);
	int bmx_time_duration_minutes(time_duration * d);
	int bmx_time_duration_seconds(time_duration * d);
	int bmx_time_duration_total_seconds(time_duration * d);
	int bmx_time_duration_total_milliseconds(time_duration * d);
	int bmx_time_duration_fractional_seconds(time_duration * d);
	bool bmx_time_duration_is_negative(time_duration * d);
	time_duration * bmx_time_duration_invert_sign(time_duration * d);
	BBString * bmx_time_duration_to_string(time_duration * d);
	BBString * bmx_time_duration_to_iso_string(time_duration * d);
	time_duration * bmx_time_duration_add(time_duration * d1, time_duration * d2);
	time_duration * bmx_time_duration_subtract(time_duration * d1, time_duration * d2);
	time_duration * bmx_time_duration_divide(time_duration * d1, int value);
	time_duration * bmx_time_duration_multiply(time_duration * d1, int value);
	bool bmx_time_duration_less(time_duration * d1, time_duration * d2);
	bool bmx_time_duration_greater(time_duration * d1, time_duration * d2);
	bool bmx_time_duration_equal(time_duration * d1, time_duration * d2);
	BBString * bmx_time_duration_asformat(time_duration * d, BBString * format, std::locale * loc, time_facet * facet);

	int bmx_time_ticks_per_second();
	int bmx_time_num_fractional_digits();
	
	ptime * bmx_ptime_new(date * d, time_duration * t);
	void bmx_ptime_delete(ptime * p);
	ptime * bmx_ptime_local_new();
	ptime * bmx_ptime_universal_new();
	ptime * bmx_ptime_local_microsecond_new();
	ptime * bmx_ptime_universal_microsecond_new();
	date * bmx_ptime_date(ptime * p);
	time_duration * bmx_ptime_time_of_day(ptime * p);
	BBString * bmx_ptime_to_simple_string(ptime * p);
	BBString * bmx_ptime_to_iso_string(ptime * p);
	BBString * bmx_ptime_to_iso_extended_string(ptime * p);
	BBString * bmx_ptime_to_string(ptime * p, std::locale * loc, time_facet * facet);
	ptime * bmx_ptime_add_days(ptime * p, int d);
	ptime * bmx_ptime_subtract_days(ptime * p, int d);
	ptime * bmx_ptime_add_duration(ptime * p, time_duration * d);
	ptime * bmx_ptime_subtract_duration(ptime * p, time_duration * d);
	time_duration * bmx_ptime_subtract(ptime * p1, ptime * p2);
	bool bmx_ptime_less(ptime * p1, ptime * p2);
	bool bmx_ptime_greater(ptime * p1, ptime * p2);
	bool bmx_ptime_equal(ptime * p1, ptime * p2);
	ptime * bmx_ptime_from_time_t(std::time_t * t);
	BBString * bmx_ptime_asformat(ptime * p, BBString * f, std::locale * loc, time_facet * facet);

	partial_date * bmx_partial_date_new(int day, int month);
	date * bmx_partial_date_get_date(partial_date *  p, int year);
	void bmx_partial_date_delete(partial_date *  p);
	last_day_of_the_week_in_month * bmx_last_day_of_week_in_month_new(int weekday, int month);
	date * bmx_last_day_of_week_in_month_get_date(last_day_of_the_week_in_month *  p, int year);
	void bmx_last_day_of_week_in_month_delete(last_day_of_the_week_in_month *  p);
	first_day_of_the_week_in_month * bmx_first_day_of_week_in_month_new(int weekday, int month);
	date * bmx_first_day_of_week_in_month_get_date(first_day_of_the_week_in_month *  p, int year);
	void bmx_first_day_of_week_in_month_delete(first_day_of_the_week_in_month *  p);
	
	BBString * bmx_weekday_to_string(int wd);
	
	date_facet * bmx_datefacet_new();
	BBString * bmx_date_asformat(date * d, BBString * format, std::locale * loc, date_facet * facet);
	std::locale * bmx_locale_new(date_facet * d, BBString * loc);
	time_facet * bmx_timefacet_new();
	
	void bmx_char_free(char * s);
	
	time_period * bmx_time_period_timetime(ptime * p1, ptime * p2);
	time_period * bmx_time_period_withduration(ptime * p, time_duration * d);
	void bmx_time_period_shift(time_period * tp, time_duration * d);
	void bmx_time_period_delete(time_period * tp);
	ptime * bmx_time_period_begin(time_period * tp);
	ptime * bmx_time_period_last(time_period * tp);
	ptime * bmx_time_period_end(time_period * tp);
	time_duration * bmx_time_period_length(time_period * tp);
	bool bmx_time_period_is_null(time_period * tp);
	bool bmx_time_period_contains(time_period * tp, ptime * t);
	bool bmx_time_period_containsPeriod(time_period * tp1, time_period * tp2);
	bool bmx_time_period_intersects(time_period * tp1, time_period * tp2);
	time_period * bmx_time_period_intersection(time_period * tp1, time_period * tp2);
	time_period * bmx_time_period_merge(time_period * tp1, time_period * tp2);
	time_period * bmx_time_period_span(time_period * tp1, time_period * tp2);
	bool bmx_time_period_isless(time_period * tp1, time_period * tp2);
	bool bmx_time_period_isgreater(time_period * tp1, time_period * tp2);
	bool bmx_time_period_isequal(time_period * tp1, time_period * tp2);
	
	posix_time_zone * bmx_posix_time_zone(BBString * id);
	BBString * bmx_time_zone_dst_zone_abbrev(time_zone * tz);
	BBString * bmx_time_zone_std_zone_abbrev(time_zone * tz);
	BBString * bmx_time_zone_dst_zone_name(time_zone * tz);
	BBString * bmx_time_zone_std_zone_name(time_zone * tz);
	bool bmx_time_zone_has_dst(time_zone * tz);
	ptime * bmx_time_zone_dst_local_start_time(time_zone * tz, int year);
	ptime * bmx_time_zone_dst_local_end_time(time_zone * tz, int year);
	time_duration * bmx_time_zone_base_utc_offset(time_zone * tz);
	time_duration * bmx_time_zone_dst_offset(time_zone * tz);
	BBString * bmx_time_zone_to_posix_string(time_zone * tz);
	
	tz_database * bmx_tz_database();
	tz_database * bmx_tz_load_from_file(BBString * filename);
	time_zone_ptr bmx_tz_time_zone_from_region(tz_database * db, BBString * id);
	
	local_date_time * bmx_local_date_time_new_sec_clock(time_zone * tz);
	local_date_time * bmx_local_date_time_new_time(ptime * p, time_zone * tz);
	
	BBString * bmx_month_to_string(int m);
	void bmx_date_facet_format(date_facet * f, BBString * fmt);
	void bmx_date_facet_set_iso_format(date_facet * f);
	void bmx_date_facet_set_iso_extended_format(date_facet * f);
	
	void bmx_date_facet_short_month_names(date_facet * f, const char ** names);
	void bmx_date_facet_long_month_names(date_facet * f, const char ** names);
	void bmx_date_facet_short_weekday_names(date_facet * f, const char ** names);
	void bmx_date_facet_long_weekday_names(date_facet * f, const char ** names);
	void bmx_date_facet_month_format(date_facet * f, BBString * fmt);
	void bmx_date_facet_weekday_format(date_facet * f, BBString * fmt);

	void bmx_time_facet_format(time_facet * f, BBString * fmt);
	void bmx_time_facet_set_iso_format(time_facet * f);
	void bmx_time_facet_set_iso_extended_format(time_facet * f);
	void bmx_time_facet_month_format(time_facet * f, BBString * fmt);
	void bmx_time_facet_weekday_format(time_facet * f, BBString * fmt);
	void bmx_time_facet_time_duration_format(time_facet * f, BBString * fmt);
	
	nth_day_of_the_week_in_month * bmx_nth_day_of_week_in_month_new(int nth, int weekday, int month);
	date * bmx_nth_day_of_week_in_month_get_date(nth_day_of_the_week_in_month *  p, int year);
	void bmx_nth_day_of_week_in_month_delete(nth_day_of_the_week_in_month *  p);

	first_day_of_the_week_after * bmx_first_day_of_week_after_new(int weekday);
	date * bmx_first_day_of_week_after_get_date(first_day_of_the_week_after *  p, date * d);
	void bmx_first_day_of_week_after_delete(first_day_of_the_week_after *  p);
	first_day_of_the_week_before * bmx_first_day_of_week_before_new(int weekday);
	date * bmx_first_day_of_week_before_get_date(first_day_of_the_week_before *  p, date * d);
	void bmx_first_day_of_week_before_delete(first_day_of_the_week_before *  p);

	long bmx_days_until_weekday(date * d, int weekday);
	long bmx_days_before_weekday(date * d, int weekday);
	date * bmx_next_weekday(date * d, int weekday);
	date * bmx_previous_weekday(date * d, int weekday);
	
	time_zone_ptr bmx_local_date_time_zone(local_date_time * ldt);
	bool bmx_local_date_time_is_dst(local_date_time * ldt);
	ptime * bmx_local_date_time_utc_time(local_date_time * ldt);
	ptime * bmx_local_date_time_local_time(local_date_time * ldt);
	
	BBString * bmx_local_date_time_to_string(local_date_time * ldt);
	bool bmx_local_date_time_less(local_date_time * ldt1, local_date_time * ldt2);
	bool bmx_local_date_time_greater(local_date_time * ldt1, local_date_time * ldt2);
	bool bmx_local_date_time_equal(local_date_time * ldt1, local_date_time * ldt2);
	
	local_date_time * bmx_local_date_time_add_days(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_add_months(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_add_years(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_subtract_days(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_subtract_months(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_subtract_years(local_date_time * ldt, int value);
	local_date_time * bmx_local_date_time_add_duration(local_date_time * ldt, time_duration * td);
	local_date_time * bmx_local_date_time_subtract_duration(local_date_time * ldt, time_duration * td);
	void bmx_local_date_time_delete(local_date_time * ldt);
	
	local_time_period * bmx_local_time_period_new(local_date_time * ldt1, local_date_time * ldt2);
	local_time_period * bmx_local_time_period_new_duration(local_date_time * ldt, time_duration * d);
	void bmx_local_time_period_delete(local_time_period * ldt);
	local_date_time * bmx_local_time_period_begin(local_time_period * ldt);
	local_date_time * bmx_local_time_period_last(local_time_period * ldt);
	local_date_time * bmx_local_time_period_end(local_time_period * ldt);
	time_duration * bmx_local_time_period_length(local_time_period * ldt);
	bool bmx_local_time_period_is_null(local_time_period * ldt);
	bool bmx_local_time_period_contains_time(local_time_period * ldt, local_date_time * t);
	bool bmx_local_time_period_contains(local_time_period * ldt1, local_time_period * ldt2);
	bool bmx_local_time_period_intersects(local_time_period * ldt1, local_time_period * ldt2);
	local_time_period * bmx_local_time_period_intersection(local_time_period * ldt1, local_time_period * ldt2);
	local_time_period * bmx_local_time_period_merge(local_time_period * ldt1, local_time_period * ldt2);
	local_time_period * bmx_local_time_period_span(local_time_period * ldt1, local_time_period * ldt2);
	void bmx_local_time_period_shift(local_time_period * ldt, time_duration * d);
	bool bmx_local_time_period_less(local_time_period * ldt1, local_time_period * ldt2);
	bool bmx_local_time_period_greater(local_time_period * ldt1, local_time_period * ldt2);
	bool bmx_local_time_period_equal(local_time_period * ldt1, local_time_period * ldt2);
	
	int bmx_end_of_month_day(int y, int m);
}

//static std::stringstream outputStringStream;


BBString * bmx_BBString_from_stream(std::stringstream & stream) {
	BBString * s = bbStringFromUTF8String(stream.str().c_str());
//	outputStringStream.str("");
	return s;
}

char * bmx_cstr_from_stream(std::stringstream & stream) {
	char * s = strdup(stream.str().c_str());
//	outputStringStream.str("");
	return s;
}


date * bmx_datetime_newdate(int year, int month, int day) {
	try {
		return new date(year, month, day);
	} catch (...) {
		return 0;
	}
}

void bmx_datetime_delete(date * d) {
	delete d;
}


date * bmx_datetime_localday() {
	return new date(day_clock::local_day());
}

date * bmx_datetime_universalday() {
	return new date(day_clock::universal_day());
}

bool bmx_datetime_before(date * myDate, date * yourDate) {
	return *myDate < *yourDate;
}

bool bmx_datetime_after(date * myDate, date * yourDate) {
	return *myDate > *yourDate;
}

bool bmx_datetime_equals(date * myDate, date * yourDate) {
	return *myDate == *yourDate;
}

int bmx_datetime_year(date * d) {
	return (int)d->year();
}

int bmx_datetime_month(date * d) {
	return (int)d->month();
}

int bmx_datetime_day(date * d) {
	return (int)d->day();
}


void bmx_datetime_ymd(date * dt, int * y, int * m, int * d) {
	date::ymd_type ymd = dt->year_month_day();
	*y = ymd.year;
	*m = ymd.month;
	*d = ymd.day;
}

date * bmx_datetime_fromstring(BBString * dt) {
	char * d = bbStringToUTF8String(dt);
	try {
		date * _date = new date(from_string(std::string(d)));
		bbMemFree(d);
		return _date;
	} catch (...) {
		bbMemFree(d);
		return 0;
	}
}

date * bmx_datetime_fromundelimitedstring(BBString * dt) {
	char * d = bbStringToUTF8String(dt);
	try {
		date * _date = new date(from_undelimited_string(std::string(d)));
		bbMemFree(d);
		return _date;
	} catch (...) {
		bbMemFree(d);
		return 0;
	}
}

int bmx_datetime_day_of_week(date * d) {
	return (int)d->day_of_week();
}

int bmx_datetime_day_of_year(date * d) {
	return (int)d->day_of_year();
}

date * bmx_datetime_end_of_month(date * d) {
	return new date(d->end_of_month());
}

int bmx_datetime_week_number(date * d) {
	return (int)d->week_number();
}

date * bmx_datetime_date_add(date * d, long duration) {
	return new date(*d + date_duration(duration));
}

date * bmx_datetime_date_subtract(date * d, long duration) {
	return new date(*d - date_duration(duration));
}

long bmx_datetime_date_subdate(date * d1, date * d2) {
	date_duration duration = *d1 - *d2;
	return duration.days();
}

date_period * bmx_datetime_period_datedate(date * d1, date * d2) {
	return new date_period(*d1, *d2);
}

date_period * bmx_datetime_period_withdays(date * d, int length) {
	return new date_period(*d, days(length));
}

void bmx_datetime_period_shift(date_period * p, int length) {
	p->shift(days(length));
}

void bmx_datetime_period_delete(date_period * p) {
	delete p;
}

date * bmx_datetime_period_begin(date_period * p) {
	return new date(p->begin());
}

date * bmx_datetime_period_last(date_period * p) {
	return new date(p->last());
}

date * bmx_datetime_period_end(date_period * p) {
	return new date(p->end());
}

int bmx_datetime_period_length(date_period * p) {
	return p->length().days();
}

BBString * bmx_datetime_to_simple_string(date * d) {
	return bbStringFromUTF8String(to_simple_string(*d).c_str());
}

BBString * bmx_datetime_to_string(date * d, std::locale * loc, date_facet * facet) {
	std::stringstream outputStringStream;
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << *d;
	BBString * ret = bmx_BBString_from_stream(outputStringStream);
	return ret;
}


BBString * bmx_datetime_period_to_simple_string(date_period * p) {
	return bbStringFromUTF8String(to_simple_string(*p).c_str());
}

BBString * bmx_datetime_to_iso_string(date * d) {
	return bbStringFromUTF8String(to_iso_string(*d).c_str());
}

BBString * bmx_datetime_to_iso_extended_string(date * d) {
	return bbStringFromUTF8String(to_iso_extended_string(*d).c_str());
}

BBString * bmx_datetime_period_to_string(date_period * p) {
	std::stringstream outputStringStream;
	outputStringStream << *p;
	return bmx_BBString_from_stream(outputStringStream);
}

bool bmx_datetime_period_isnull(date_period * p) {
	return p->is_null();
}

bool bmx_datetime_period_containsdate(date_period * p, date * d) {
	return p->contains(*d);
}

bool bmx_datetime_period_contains(date_period * p1, date_period * p2) {
	return p1->contains(*p2);
}

bool bmx_datetime_period_intersects(date_period * p1, date_period * p2) {
	return p1->intersects(*p2);
}

date_period * bmx_datetime_period_intersection(date_period * p1, date_period * p2) {
	return new date_period(p1->intersection(*p2));
}

bool bmx_datetime_period_adjacent(date_period * p1, date_period * p2) {
	return p1->is_adjacent(*p2);
}

bool bmx_datetime_period_after(date_period * p, date * d) {
	return p->is_after(*d);
}

bool bmx_datetime_period_before(date_period * p, date * d) {
	return p->is_before(*d);
}

date_period * bmx_datetime_period_merge(date_period * p1, date_period * p2) {
	return new date_period(p1->merge(*p2));
}

date_period * bmx_datetime_period_span(date_period * p1, date_period * p2) {
	return new date_period(p1->span(*p2));
}

bool bmx_datetime_period_isless(date_period * p1, date_period * p2) {
	return *p1 < *p2;
}

bool bmx_datetime_period_isgreater(date_period * p1, date_period * p2) {
	return *p1 > *p2;
}

bool bmx_datetime_period_isequal(date_period * p1, date_period * p2) {
	return *p1 == *p2;
}

day_iterator * bmx_datetime_dayiter(date * d, int offset) {
	return new day_iterator(*d, offset);
}

month_iterator * bmx_datetime_monthiter(date * d, int offset) {
	return new month_iterator(*d, offset);
}

year_iterator * bmx_datetime_yeariter(date * d, int offset) {
	return new year_iterator(*d, offset);
}

void bmx_datetime_iter_delete(date_iterator * d) {
	delete d;
}

void bmx_datetime_iter_forward(date_iterator * d) {
	++*d;
}

void bmx_datetime_iter_backward(date_iterator * d) {
	--*d;
}


bool bmx_datetime_iter_before(date_iterator * myDate, date * yourDate) {
	return (*(day_iterator *)myDate) < *yourDate;
}

bool bmx_datetime_iter_after(date_iterator * myDate, date * yourDate) {
	return (*(day_iterator *)myDate) > *yourDate;
}

bool bmx_datetime_iter_equals(date_iterator * myDate, date * yourDate) {
	return (*(day_iterator *)myDate) == *yourDate;
}

int bmx_datetime_iter_year(date_iterator * d) {
	return (int)(*(day_iterator *)d)->year();
}

int bmx_datetime_iter_month(date_iterator * d) {
	return (int)(*(day_iterator *)d)->month();
}

int bmx_datetime_iter_day(date_iterator * d) {
	return (int)(*(day_iterator *)d)->day();
}

void bmx_datetime_iter_ymd(date_iterator * dt, int * y, int * m, int * d) {
	date::ymd_type ymd = (*(day_iterator *)dt)->year_month_day();
	*y = ymd.year;
	*m = ymd.month;
	*d = ymd.day;
}

int bmx_datetime_iter_day_of_week(date_iterator * d) {
	return (int)(*(day_iterator *)d)->day_of_week();
}

int bmx_datetime_iter_day_of_year(date_iterator * d) {
	return (int)(*(day_iterator *)d)->day_of_year();
}

date * bmx_datetime_iter_end_of_month(date_iterator * d) {
	return new date((*(day_iterator *)d)->end_of_month());
}

int bmx_datetime_iter_week_number(date_iterator * d) {
	return (int)(*(day_iterator *)d)->week_number();
}

date * bmx_datetime_iter_date_add(date_iterator * d, long duration) {
	date dd(**d);
	return new date(dd + date_duration(duration));
}

date * bmx_datetime_iter_date_subtract(date_iterator * d, long duration) {
	return new date(**d - date_duration(duration));
}

long bmx_datetime_iter_date_subdate(date_iterator * d1, date * d2) {
	date_duration duration = (**d1) - *d2;
	return duration.days();
}

BBString * bmx_datetime_iter_to_simple_string(date_iterator * d) {
	return bbStringFromUTF8String(to_simple_string((**d)).c_str());
}

BBString * bmx_datetime_iter_to_iso_string(date_iterator * d) {
	return bbStringFromUTF8String(to_iso_string((**d)).c_str());
}

BBString * bmx_datetime_iter_to_iso_extended_string(date_iterator * d) {
	return bbStringFromUTF8String(to_iso_extended_string((**d)).c_str());
}

BBString * bmx_datetime_iter_to_string(date_iterator * d, std::locale * loc, date_facet * facet) {
	std::stringstream outputStringStream;
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << (**d);
	return bmx_BBString_from_stream(outputStringStream);
}

BBString * bmx_datetime_iter_asformat(date_iterator * d, BBString * format, std::locale * loc, date_facet * facet) {
	std::stringstream outputStringStream;
	char * f = bbStringToUTF8String(format);
	facet->format(f);
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << **d;
	BBString * ret = bmx_BBString_from_stream(outputStringStream);
	bbMemFree(f);
	return ret;
}


time_duration * bmx_time_duration(int hours, int minutes, int seconds, int fraction) {
	return new time_duration(hours, minutes, seconds, fraction);
}

void bmx_time_duration_delete(time_duration * d) {
	delete d;
}

time_duration * bmx_time_duration_new_hours(int value) {
	return new time_duration(hours(value));
}

time_duration * bmx_time_duration_new_minutes(int value) {
	return new time_duration(minutes(value));
}

time_duration * bmx_time_duration_new_seconds(int value) {
	return new time_duration(seconds(value));
}

time_duration * bmx_time_duration_new_milliseconds(int value) {
	return new time_duration(milliseconds(value));
}

int bmx_time_duration_hours(time_duration * d) {
	return d->hours();
}

int bmx_time_duration_minutes(time_duration * d) {
	return d->minutes();
}

int bmx_time_duration_seconds(time_duration * d) {
	return d->seconds();
}

int bmx_time_duration_total_seconds(time_duration * d) {
	return d->total_seconds();
}

int bmx_time_ticks_per_second() {
	return time_duration::ticks_per_second();
}

int bmx_time_num_fractional_digits() {
	return (int)time_duration::num_fractional_digits();
}

int bmx_time_duration_total_milliseconds(time_duration * d) {
	return d->total_milliseconds();
}

int bmx_time_duration_fractional_seconds(time_duration * d) {
	return d->fractional_seconds();
}

bool bmx_time_duration_is_negative(time_duration * d) {
	return d->is_negative();
}

time_duration * bmx_time_duration_invert_sign(time_duration * d) {
	return new time_duration(d->invert_sign());
}

BBString * bmx_time_duration_to_string(time_duration * d) {
	return bbStringFromUTF8String(to_simple_string(*d).c_str());
}

BBString * bmx_time_duration_to_iso_string(time_duration * d) {
	return bbStringFromUTF8String(to_iso_string(*d).c_str());
}

time_duration * bmx_time_duration_add(time_duration * d1, time_duration * d2) {
	return new time_duration(*d1 + *d2);
}

time_duration * bmx_time_duration_subtract(time_duration * d1, time_duration * d2) {
	return new time_duration(*d1 - *d2);
}

time_duration * bmx_time_duration_divide(time_duration * d1, int value) {
	return new time_duration(*d1 / value);
}

time_duration * bmx_time_duration_multiply(time_duration * d1, int value) {
	return new time_duration(*d1 * value);
}

bool bmx_time_duration_less(time_duration * d1, time_duration * d2) {
	return *d1 < *d2;
}

bool bmx_time_duration_greater(time_duration * d1, time_duration * d2) {
	return *d1 > *d2;
}

bool bmx_time_duration_equal(time_duration * d1, time_duration * d2) {
	return *d1 == *d2;
}

BBString * bmx_time_duration_asformat(time_duration * d, BBString * format, std::locale * loc, time_facet * facet) {
	std::stringstream outputStringStream;
	char * f = bbStringToUTF8String(format);
	facet->time_duration_format(f);
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << *d;
	BBString * ret = bmx_BBString_from_stream(outputStringStream);
	bbMemFree(f);
	return ret;
}

ptime * bmx_ptime_new(date * d, time_duration * t) {
	return new ptime(*d, *t);
}

void bmx_ptime_delete(ptime * p) {
	delete p;
}

ptime * bmx_ptime_local_new() {
	return new ptime(second_clock::local_time());
}

ptime * bmx_ptime_universal_new() {
	return new ptime(second_clock::universal_time());
}

ptime * bmx_ptime_local_microsecond_new() {
	return new ptime(microsec_clock::local_time());
}

ptime * bmx_ptime_universal_microsecond_new() {
	return new ptime(microsec_clock::universal_time());
}

date * bmx_ptime_date(ptime * p) {
	return new date(p->date());
}

time_duration * bmx_ptime_time_of_day(ptime * p) {
	return new time_duration(p->time_of_day());
}

BBString * bmx_ptime_to_simple_string(ptime * p) {
	return bbStringFromUTF8String(to_simple_string(*p).c_str());
}

BBString * bmx_ptime_to_string(ptime * p, std::locale * loc, time_facet * facet) {
	std::stringstream outputStringStream;
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << (*p);
	return bmx_BBString_from_stream(outputStringStream);
}

BBString * bmx_ptime_to_iso_string(ptime * p) {
	return bbStringFromUTF8String(to_iso_string(*p).c_str());
}

BBString * bmx_ptime_to_iso_extended_string(ptime * p) {
	return bbStringFromUTF8String(to_iso_extended_string(*p).c_str());
}

ptime * bmx_ptime_add_days(ptime * p, int d) {
	return new ptime(*p + days(d));
}

ptime * bmx_ptime_subtract_days(ptime * p, int d) {
	return new ptime(*p - days(d));
}

ptime * bmx_ptime_add_duration(ptime * p, time_duration * d) {
	return new ptime(*p + *d);
}

ptime * bmx_ptime_subtract_duration(ptime * p, time_duration * d) {
	return new ptime(*p - *d);
}

time_duration * bmx_ptime_subtract(ptime * p1, ptime * p2) {
	return new time_duration(*p1 - *p2);
}

bool bmx_ptime_less(ptime * p1, ptime * p2) {
	return *p1 < *p2;
}

bool bmx_ptime_greater(ptime * p1, ptime * p2) {
	return *p1 > *p2;
}

bool bmx_ptime_equal(ptime * p1, ptime * p2) {
	return *p1 == *p2;
}

BBString * bmx_ptime_asformat(ptime * p, BBString * format, std::locale * loc, time_facet * facet) {
	std::stringstream outputStringStream;
	char * f = bbStringToUTF8String(format);
	facet->format(f);
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << *p;
	BBString * ret = bmx_BBString_from_stream(outputStringStream);
	bbMemFree(f);
	return ret;
}

partial_date * bmx_partial_date_new(int day, int month) {
	return new partial_date(day, month);
}

date * bmx_partial_date_get_date(partial_date *  p, int year) {
	return new date(p->get_date(year));
}

void bmx_partial_date_delete(partial_date *  p) {
	delete p;
}

last_day_of_the_week_in_month * bmx_last_day_of_week_in_month_new(int weekday, int month) {
	return new last_day_of_the_week_in_month(weekday, month);
}

date * bmx_last_day_of_week_in_month_get_date(last_day_of_the_week_in_month *  p, int year) {
	return new date(p->get_date(year));
}

void bmx_last_day_of_week_in_month_delete(last_day_of_the_week_in_month *  p) {
	delete p;
}

first_day_of_the_week_in_month * bmx_first_day_of_week_in_month_new(int weekday, int month) {
	return new first_day_of_the_week_in_month(weekday, month);
}

date * bmx_first_day_of_week_in_month_get_date(first_day_of_the_week_in_month *  p, int year) {
	return new date(p->get_date(year));
}

void bmx_first_day_of_week_in_month_delete(first_day_of_the_week_in_month *  p) {
	delete p;
}

BBString * bmx_weekday_to_string(int wd) {
	std::stringstream outputStringStream;
	outputStringStream << greg_weekday((unsigned short)wd);
	return bmx_BBString_from_stream(outputStringStream);
}


date_facet * bmx_datefacet_new() {
	return new date_facet;
}

time_facet * bmx_timefacet_new() {
	return new time_facet;
}

BBString * bmx_date_asformat(date * d, BBString * format, std::locale * loc, date_facet * facet) {
	std::stringstream outputStringStream;
	char * f = bbStringToUTF8String(format);
	facet->format(f);
	outputStringStream.imbue(std::locale(*loc, facet));
	outputStringStream << *d;
	BBString * ret = bmx_BBString_from_stream(outputStringStream);
	bbMemFree(f);
	return ret;
}

std::locale * bmx_locale_new(date_facet * d, BBString * loc) {
	std::locale * _loc = 0;
	char * l = bbStringToUTF8String(loc);
	try {
		_loc = new std::locale(std::locale(l), d);
	} catch (std::exception & e) {
		_loc = new std::locale(std::locale(), d);
	}
	bbMemFree(l);
	return _loc;
}

void bmx_char_free(char * s) {
	free(s);
}

time_period * bmx_time_period_timetime(ptime * p1, ptime * p2) {
	return new time_period(*p1, *p2);
}

time_period * bmx_time_period_withduration(ptime * p, time_duration * d) {
	return new time_period(*p, *d);
}

void bmx_time_period_shift(time_period * tp, time_duration * d) {
	tp->shift(*d);
}

ptime * bmx_time_period_begin(time_period * tp) {
	return new ptime(tp->begin());
}

void bmx_time_period_delete(time_period * tp) {
	delete tp;
}

ptime * bmx_time_period_last(time_period * tp) {
	return new ptime(tp->last());
}

ptime * bmx_time_period_end(time_period * tp) {
	return new ptime(tp->end());
}

time_duration * bmx_time_period_length(time_period * tp) {
	return new time_duration(tp->length());
}

bool bmx_time_period_is_null(time_period * tp) {
	return tp->is_null();
}

bool bmx_time_period_contains(time_period * tp, ptime * t) {
	return tp->contains(*t);
}

bool bmx_time_period_containsPeriod(time_period * tp1, time_period * tp2) {
	return tp1->contains(*tp2);
}

bool bmx_time_period_intersects(time_period * tp1, time_period * tp2) {
	return tp1->intersects(*tp2);
}

time_period * bmx_time_period_intersection(time_period * tp1, time_period * tp2) {
	return new time_period(tp1->intersection(*tp2));
}

time_period * bmx_time_period_merge(time_period * tp1, time_period * tp2) {
	return new time_period(tp1->merge(*tp2));
}

time_period * bmx_time_period_span(time_period * tp1, time_period * tp2) {
	return new time_period(tp1->span(*tp2));
}

bool bmx_time_period_isless(time_period * tp1, time_period * tp2) {
	return *tp1 < *tp2;
}

bool bmx_time_period_isgreater(time_period * tp1, time_period * tp2) {
	return *tp1 > *tp2;
}

bool bmx_time_period_isequal(time_period * tp1, time_period * tp2) {
	return *tp1 == *tp2;
}

ptime * bmx_ptime_from_time_t(std::time_t * t) {
	return new ptime(from_time_t(*t));
}

posix_time_zone * bmx_posix_time_zone(BBString * id) {
	char * d = bbStringToUTF8String(id);
	posix_time_zone * _zone = new posix_time_zone(std::string(d));
	bbMemFree(d);
	return _zone;
}

BBString * bmx_time_zone_dst_zone_abbrev(time_zone * tz) {
	return bbStringFromUTF8String(tz->dst_zone_abbrev().c_str());
}

BBString * bmx_time_zone_std_zone_abbrev(time_zone * tz) {
	return bbStringFromUTF8String(tz->std_zone_abbrev().c_str());
}

BBString * bmx_time_zone_dst_zone_name(time_zone * tz) {
	return bbStringFromUTF8String(tz->dst_zone_name().c_str());
}

BBString * bmx_time_zone_std_zone_name(time_zone * tz) {
	return bbStringFromUTF8String(tz->std_zone_name().c_str());
}

bool bmx_time_zone_has_dst(time_zone * tz) {
	return tz->has_dst();
}

ptime * bmx_time_zone_dst_local_start_time(time_zone * tz, int year) {
	return new ptime(tz->dst_local_start_time(year));
}

ptime * bmx_time_zone_dst_local_end_time(time_zone * tz, int year) {
	return new ptime(tz->dst_local_end_time(year));
}


time_duration * bmx_time_zone_base_utc_offset(time_zone * tz) {
	return new time_duration(tz->base_utc_offset());
}

time_duration * bmx_time_zone_dst_offset(time_zone * tz) {
	return new time_duration(tz->dst_offset());
}

BBString * bmx_time_zone_to_posix_string(time_zone * tz) {
	return bbStringFromUTF8String(tz->to_posix_string().c_str());
}

tz_database * bmx_tz_database() {
	return new tz_database();
}

tz_database * bmx_tz_load_from_file(BBString * filename) {
	char * f = bbStringToUTF8String(filename);
	tz_database * db = new tz_database();
	db->load_from_file(std::string(f));
	bbMemFree(f);
	return db;
}

time_zone_ptr bmx_tz_time_zone_from_region(tz_database * db, BBString * id) {
	char * d = bbStringToUTF8String(id);
	time_zone_ptr p = db->time_zone_from_region(std::string(d));
	bbMemFree(d);
	return p;
}

local_date_time * bmx_local_date_time_new_sec_clock(time_zone * tz) {
	return new local_date_time(local_sec_clock::local_time(time_zone_ptr(tz)));
}

local_date_time * bmx_local_date_time_new_time(ptime * p, time_zone * tz) {
	return new local_date_time(*p, time_zone_ptr(tz));
}

BBString * bmx_month_to_string(int m) {
	std::stringstream outputStringStream;
	outputStringStream << greg_month((unsigned short)m);
	return bmx_BBString_from_stream(outputStringStream);
}

void bmx_date_facet_format(date_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->format(s);
	bbMemFree(s);
}

void bmx_date_facet_set_iso_format(date_facet * f) {
	f->set_iso_format();
}

void bmx_date_facet_set_iso_extended_format(date_facet * f) {
	f->set_iso_extended_format();
}

void bmx_date_facet_short_month_names(date_facet * f, const char ** names) {
	std::string m_names[12];
	for (int i = 0; i < 12; i++) {
		m_names[i] = std::string(names[i]);
	}
	
	std::vector<std::string> v_names(&m_names[0], &m_names[12]);
	
	f->short_month_names(v_names);
}

void bmx_date_facet_long_month_names(date_facet * f, const char ** names) {
	std::string m_names[12];
	for (int i = 0; i < 12; i++) {
		m_names[i] = std::string(names[i]);
	}
	
	std::vector<std::string> v_names(&m_names[0], &m_names[12]);
	
	f->long_month_names(v_names);
}

void bmx_date_facet_short_weekday_names(date_facet * f, const char ** names) {
	std::string m_names[7];
	for (int i = 0; i < 7; i++) {
		m_names[i] = std::string(names[i]);
	}
	
	std::vector<std::string> v_names(&m_names[0], &m_names[7]);
	
	f->short_weekday_names(v_names);
}

void bmx_date_facet_long_weekday_names(date_facet * f, const char ** names) {
	std::string m_names[7];
	for (int i = 0; i < 7; i++) {
		m_names[i] = std::string(names[i]);
	}
	
	std::vector<std::string> v_names(&m_names[0], &m_names[7]);
	
	f->long_weekday_names(v_names);
}

void bmx_date_facet_month_format(date_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->month_format(s);
	bbMemFree(s);
}

void bmx_date_facet_weekday_format(date_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->weekday_format(s);
	bbMemFree(s);
}

void bmx_time_facet_format(time_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->format(s);
	bbMemFree(s);
}

void bmx_time_facet_set_iso_format(time_facet * f) {
	f->set_iso_format();
}

void bmx_time_facet_set_iso_extended_format(time_facet * f) {
	f->set_iso_extended_format();
}

void bmx_time_facet_month_format(time_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->month_format(s);
	bbMemFree(s);
}

void bmx_time_facet_weekday_format(time_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->weekday_format(s);
	bbMemFree(s);
}

void bmx_time_facet_time_duration_format(time_facet * f, BBString * fmt) {
	char * s = bbStringToUTF8String(fmt);
	f->time_duration_format(s);
	bbMemFree(s);
}

nth_day_of_the_week_in_month * bmx_nth_day_of_week_in_month_new(int nth, int weekday, int month) {
	nth_day_of_the_week_in_month::week_num w;
	switch (nth) {
		case 1:
			w = nth_day_of_the_week_in_month::first;
			break;
		case 2:
			w = nth_day_of_the_week_in_month::second;
			break;
		case 3:
			w = nth_day_of_the_week_in_month::third;
			break;
		case 4:
			w = nth_day_of_the_week_in_month::fourth;
			break;
		case 5:
			w = nth_day_of_the_week_in_month::fifth;
			break;
		default:
			w = nth_day_of_the_week_in_month::first;
	}
	return new nth_day_of_the_week_in_month(w, weekday, month);
}

date * bmx_nth_day_of_week_in_month_get_date(nth_day_of_the_week_in_month *  p, int year) {
	return new date(p->get_date(year));
}

void bmx_nth_day_of_week_in_month_delete(nth_day_of_the_week_in_month *  p) {
	delete p;
}

first_day_of_the_week_after * bmx_first_day_of_week_after_new(int weekday) {
	return new first_day_of_the_week_after(weekday);
}

date * bmx_first_day_of_week_after_get_date(first_day_of_the_week_after *  p, date * d) {
	return new date(p->get_date(*d));
}

void bmx_first_day_of_week_after_delete(first_day_of_the_week_after *  p) {
	delete p;
}

first_day_of_the_week_before * bmx_first_day_of_week_before_new(int weekday) {
	return new first_day_of_the_week_before(greg_weekday(weekday));
}

date * bmx_first_day_of_week_before_get_date(first_day_of_the_week_before *  p, date * d) {
	return new date(p->get_date(*d));
}

void bmx_first_day_of_week_before_delete(first_day_of_the_week_before *  p) {
	delete p;
}

long bmx_days_until_weekday(date * d, int weekday) {
	return days_until_weekday(*d, greg_weekday(weekday)).days();
}
long bmx_days_before_weekday(date * d, int weekday) {
	return days_before_weekday(*d, greg_weekday(weekday)).days();
}

date * bmx_next_weekday(date * d, int weekday) {
	return new date(next_weekday(*d, greg_weekday(weekday)));
}

date * bmx_previous_weekday(date * d, int weekday) {
	return new date(previous_weekday(*d, greg_weekday(weekday)));
}

time_zone_ptr bmx_local_date_time_zone(local_date_time * ldt) {
	return ldt->zone();
}

bool bmx_local_date_time_is_dst(local_date_time * ldt) {
	return ldt->is_dst();
}

ptime * bmx_local_date_time_utc_time(local_date_time * ldt) {
	return new ptime(ldt->utc_time());
}

ptime * bmx_local_date_time_local_time(local_date_time * ldt) {
	return new ptime(ldt->local_time());
}

BBString * bmx_local_date_time_to_string(local_date_time * ldt) {
	std::stringstream outputStringStream;
	outputStringStream << *ldt;
	return bmx_BBString_from_stream(outputStringStream);
}

bool bmx_local_date_time_less(local_date_time * ldt1, local_date_time * ldt2) {
	return *ldt1 < *ldt2;
}

bool bmx_local_date_time_greater(local_date_time * ldt1, local_date_time * ldt2) {
	return *ldt1 > *ldt2;
}

bool bmx_local_date_time_equal(local_date_time * ldt1, local_date_time * ldt2) {
	return *ldt1 == *ldt2;
}

local_date_time * bmx_local_date_time_add_days(local_date_time * ldt, int value) {
	return new local_date_time(*ldt + days(value));
}

local_date_time * bmx_local_date_time_add_months(local_date_time * ldt, int value) {
	return new local_date_time(*ldt + months(value));
}

local_date_time * bmx_local_date_time_add_years(local_date_time * ldt, int value) {
	return new local_date_time(*ldt + years(value));
}

local_date_time * bmx_local_date_time_subtract_days(local_date_time * ldt, int value) {
	return new local_date_time(*ldt - days(value));
}

local_date_time * bmx_local_date_time_subtract_months(local_date_time * ldt, int value) {
	return new local_date_time(*ldt - months(value));
}

local_date_time * bmx_local_date_time_subtract_years(local_date_time * ldt, int value) {
	return new local_date_time(*ldt - years(value));
}

local_date_time * bmx_local_date_time_add_duration(local_date_time * ldt, time_duration * td) {
	return new local_date_time(*ldt + *td);
}

local_date_time * bmx_local_date_time_subtract_duration(local_date_time * ldt, time_duration * td) {
	return new local_date_time(*ldt - *td);
}

void bmx_local_date_time_delete(local_date_time * ldt) {
	delete ldt;
}

local_time_period * bmx_local_time_period_new(local_date_time * ldt1, local_date_time * ldt2) {
	return new local_time_period(*ldt1, *ldt2);
}

local_time_period * bmx_local_time_period_new_duration(local_date_time * ldt, time_duration * d) {
	return new local_time_period(*ldt, *d);
}

void bmx_local_time_period_delete(local_time_period * ldt) {
	delete ldt;
}

local_date_time * bmx_local_time_period_begin(local_time_period * ldt) {
	return new local_date_time(ldt->begin());
}

local_date_time * bmx_local_time_period_last(local_time_period * ldt) {
	return new local_date_time(ldt->last());
}

local_date_time * bmx_local_time_period_end(local_time_period * ldt) {
	return new local_date_time(ldt->end());
}

time_duration * bmx_local_time_period_length(local_time_period * ldt) {
	return new time_duration(ldt->length());
}

bool bmx_local_time_period_is_null(local_time_period * ldt) {
	return ldt->is_null();
}

bool bmx_local_time_period_contains_time(local_time_period * ldt, local_date_time * t) {
	return ldt->contains(*t);
}

bool bmx_local_time_period_contains(local_time_period * ldt1, local_time_period * ldt2) {
	return ldt1->contains(*ldt2);
}

bool bmx_local_time_period_intersects(local_time_period * ldt1, local_time_period * ldt2) {
	return ldt1->intersects(*ldt2);
}

local_time_period * bmx_local_time_period_intersection(local_time_period * ldt1, local_time_period * ldt2) {
	return new local_time_period(ldt1->intersection(*ldt2));
}

local_time_period * bmx_local_time_period_merge(local_time_period * ldt1, local_time_period * ldt2) {
	return new local_time_period(ldt1->merge(*ldt2));
}

local_time_period * bmx_local_time_period_span(local_time_period * ldt1, local_time_period * ldt2) {
	return new local_time_period(ldt1->span(*ldt2));
}

void bmx_local_time_period_shift(local_time_period * ldt, time_duration * d) {
	ldt->shift(*d);
}

bool bmx_local_time_period_less(local_time_period * ldt1, local_time_period * ldt2) {
	return *ldt1 < *ldt2;
}

bool bmx_local_time_period_greater(local_time_period * ldt1, local_time_period * ldt2) {
	return *ldt1 > *ldt2;
}

bool bmx_local_time_period_equal(local_time_period * ldt1, local_time_period * ldt2) {
	return *ldt1 == *ldt2;
}

int bmx_end_of_month_day(int y, int m) {
	return gregorian_calendar::end_of_month_day(y, m);
}


