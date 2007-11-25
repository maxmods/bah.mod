/*
  Copyright (c) 2007, Bruce A Henderson
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the auther nor the names of its contributors may be used to 
        endorse or promote products derived from this software without specific
        prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

extern "C" {

#include "blitz.h"

	void _calcDateValue(BBInt64 * value, int y, int m, int d, int hh, int mm, int ss);
	BBString * _formatDate(BBString * format, int y, int m, int d, int hh, int mm, int ss);
	bool _parseDate(BBString * date, BBString * format, int * y, int * m, int * d);
	bool _parseDateTime(BBString * date, BBString * format, int * y, int * m, int * d, int * hh, int * mm, int * ss);
	bool _parseTime(BBString * date, BBString * format, int * hh, int * mm, int * ss);

}


void _calcDateValue(BBInt64 * value, int y, int m, int d, int hh, int mm, int ss) {

	tm stm;
	stm.tm_year = y - 1900;
	stm.tm_mon = m - 1;
	stm.tm_mday = d;
	stm.tm_hour = hh;
	stm.tm_min = mm;
	stm.tm_sec = ss;

	*value = static_cast<BBInt64>(mktime(&stm));
}

BBString * _formatDate(BBString * format, int y, int m, int d, int hh, int mm, int ss) {

	char buffer [1024];
	tm stm;
	char * p = bbStringToCString(format);

	stm.tm_year = y - 1900;
	stm.tm_mon = m - 1;
	stm.tm_mday = d;
	stm.tm_hour = hh;
	stm.tm_min = mm;
	stm.tm_sec = ss;

	strftime (buffer, 1024, p, &stm);
	
	bbMemFree(p);
	
	return bbStringFromCString(buffer);
}

bool _parseDate(BBString * date, BBString * format, int * y, int * m, int * d) {

	tm stm;
	char * sdate = bbStringToCString(date);
	char * sformat = bbStringToCString(format);

	char * res = strptime(sdate, sformat, &stm);

	bbMemFree(sdate);
	bbMemFree(sformat);
	
	if (res != NULL) {
		*y = stm.tm_year + 1900;
		*m = stm.tm_mon + 1;
		*d = stm.tm_mday;

		return true;
	} else {
		return false;
	}
}

bool _parseDateTime(BBString * date, BBString * format, int * y, int * m, int * d, int * hh, int * mm, int * ss) {

	tm stm;
	char * sdate = bbStringToCString(date);
	char * sformat = bbStringToCString(format);

	char * res = strptime(sdate, sformat, &stm);

	bbMemFree(sdate);
	bbMemFree(sformat);
	
	if (res != NULL) {
		*y = stm.tm_year + 1900;
		*m = stm.tm_mon + 1;
		*d = stm.tm_mday;
		*hh = stm.tm_hour;
		*mm = stm.tm_min;
		*ss = stm.tm_sec;

		return true;
	} else {
		return false;
	}

}

bool _parseTime(BBString * date, BBString * format, int * hh, int * mm, int * ss) {

	tm stm;
	char * sdate = bbStringToCString(date);
	char * sformat = bbStringToCString(format);

	char * res = strptime(sdate, sformat, &stm);

	bbMemFree(sdate);
	bbMemFree(sformat);
	
	if (res != NULL) {
		*hh = stm.tm_hour;
		*mm = stm.tm_min;
		*ss = stm.tm_sec;

		return true;
	} else {
		return false;
	}


}
