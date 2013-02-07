/*
 Copyright (c) 2013 Bruce A Henderson
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

#ifndef BAH_BOOST_LOCALE_GLUE_H
#define BAH_BOOST_LOCALE_GLUE_H


#include "boost/locale.hpp"

class MaxLocale;

extern "C" {

#include "blitz.h"

	boost::locale::generator * bmx_boostlocale_generator_create();
	MaxLocale * bmx_boostlocale_generator_generate(boost::locale::generator * gen, BBString * id);

	BBString * bmx_boostlocale_locale_name(MaxLocale * loc);
	BBString * bmx_boostlocale_locale_language(MaxLocale * loc);
	BBString * bmx_boostlocale_locale_country(MaxLocale * loc);
	BBString * bmx_boostlocale_locale_variant(MaxLocale * loc);
	BBString * bmx_boostlocale_locale_encoding(MaxLocale * loc);
	int bmx_boostlocale_locale_utf8(MaxLocale * loc);
	void bmx_boostlocale_locale_free(MaxLocale * loc);

	BBString * bmx_boostlocale_intasnumber(MaxLocale * loc, int value);
	BBString * bmx_boostlocale_floatasnumber(MaxLocale * loc, float value);
	BBString * bmx_boostlocale_intascurrency(MaxLocale * loc, int value);
	BBString * bmx_boostlocale_floatascurrency(MaxLocale * loc, float value);

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MaxLocale
{
public:
	MaxLocale(std::locale loc)
		: loc(loc)
	{
	}
	
	~MaxLocale()
	{
	}

	const std::locale & Locale() {
		return loc;
	}

private:
	std::locale loc;
};


#endif // BAH_BOOST_LOCALE_GLUE_H

