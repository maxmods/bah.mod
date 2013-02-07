
#include "glue.h"



boost::locale::generator * bmx_boostlocale_generator_create() {
	boost::locale::generator * gen = new boost::locale::generator();
	
	gen->locale_cache_enabled(true);
	
	return gen;
}

MaxLocale * bmx_boostlocale_generator_generate(boost::locale::generator * gen, BBString * id) {

	char * d = bbStringToUTF8String(id);
	
	MaxLocale * loc = new MaxLocale(gen->generate(d));
	
	bbMemFree(d);
	
	return loc;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_boostlocale_locale_name(MaxLocale * loc) {
	return bbStringFromUTF8String(std::use_facet<boost::locale::info>(loc->Locale()).name().c_str());
}

BBString * bmx_boostlocale_locale_language(MaxLocale * loc) {
	return bbStringFromUTF8String(std::use_facet<boost::locale::info>(loc->Locale()).language().c_str());
}

BBString * bmx_boostlocale_locale_country(MaxLocale * loc) {
	return bbStringFromUTF8String(std::use_facet<boost::locale::info>(loc->Locale()).country().c_str());
}

BBString * bmx_boostlocale_locale_variant(MaxLocale * loc) {
	return bbStringFromUTF8String(std::use_facet<boost::locale::info>(loc->Locale()).variant().c_str());
}

BBString * bmx_boostlocale_locale_encoding(MaxLocale * loc) {
	return bbStringFromUTF8String(std::use_facet<boost::locale::info>(loc->Locale()).encoding().c_str());
}

int bmx_boostlocale_locale_utf8(MaxLocale * loc) {
	return std::use_facet<boost::locale::info>(loc->Locale()).utf8();
}

void bmx_boostlocale_locale_free(MaxLocale * loc) {
	delete loc;
}


