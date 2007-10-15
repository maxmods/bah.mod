SuperStrict


Framework BaH.Locale
Import BRL.StandardIO

Local spacer:String = "                    "

Local locales:String[] = GetSystemLocales()

Print "~nLocales count = " + locales.length

Print "~n~tISO~tLanguage             Native~n"

For Local i:Int = 0 Until locales.length
	Local lang:String = (GetLanguage(locales[i]) + spacer)[..20]
	Print "~t" + locales[i] + "~t" + lang + " " + GetLanguage(locales[i], True)
Next
