
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

