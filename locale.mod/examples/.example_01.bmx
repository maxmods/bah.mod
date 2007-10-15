SuperStrict


Framework BaH.Locale
Import BRL.StandardIO

' Load the BLF
LoadLocaleFile("example_01.blf")

' get a string array of loaded language codes
Local locales:String[] = GetAvailableLocales()

Print "Languages = "
For Local i:Int = 0 Until locales.length
	Print "            " + locales[i] + " - " + GetLanguage(locales[i])
Next

Print "~n"

' for each language code, output all the numbers
For Local i:Int = 0 Until locales.length
	SetCurrentLocale(locales[i])
	Print GetLanguage(locales[i], True) + " :~n"
	Print "~t 1 - " + GetLocaleText("one")
	Print "~t 2 - " + GetLocaleText("two")
	Print "~t 3 - " + GetLocaleText("three")
	Print "~t 4 - " + GetLocaleText("four")
	Print "~t 5 - " + GetLocaleText("five")
	Print "~t 6 - " + GetLocaleText("six")
	Print "~t 7 - " + GetLocaleText("seven")
	Print "~t 8 - " + GetLocaleText("eight")
	Print "~t 9 - " + GetLocaleText("nine")
	Print "~t10 - " + GetLocaleText("ten")
	Print "~n"
Next


