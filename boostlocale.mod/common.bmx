SuperStrict

Import "source.bmx"



Extern

	Function bmx_boostlocale_generator_create:Byte Ptr()
	Function bmx_boostlocale_generator_generate:Byte Ptr(handle:Byte Ptr, id:String)

	Function bmx_boostlocale_locale_name:String(handle:Byte Ptr)
	Function bmx_boostlocale_locale_language:String(handle:Byte Ptr)
	Function bmx_boostlocale_locale_country:String(handle:Byte Ptr)
	Function bmx_boostlocale_locale_variant:String(handle:Byte Ptr)
	Function bmx_boostlocale_locale_encoding:String(handle:Byte Ptr)
	Function bmx_boostlocale_locale_utf8:Int(handle:Byte Ptr)
	Function bmx_boostlocale_locale_free(handle:Byte Ptr)

End Extern
