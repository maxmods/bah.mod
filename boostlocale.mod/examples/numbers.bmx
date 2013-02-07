SuperStrict

Framework BaH.BoostLocale
Import BRL.StandardIO

numbers("en_US")
numbers("de_DE")
numbers("ru_RU")
numbers("zh_CN")


Function numbers(locale:String)
	SetLocale(locale)
	
	Print "Locale : " + locale + "~n"

	Print "This is how we show numbers in this locale  :  " + FloatAsNumber(5103.34)
	Print "This is how we show currency in this locale :  " + FloatAsCurrency(5103.34)

	Print ""
	
End Function

