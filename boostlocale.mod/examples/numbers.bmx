SuperStrict

Framework BaH.BoostLocale
Import BRL.StandardIO

numbers("en_US.UTF-8")
numbers("de_DE.UTF-8")
numbers("ru_RU.UTF-8")
numbers("zh_CN.UTF-8")


Function numbers(locale:String)
	SetLocale(locale)
	
	Print "Locale : " + locale + "~n"

	Print "This is how we show numbers in this locale  :  " + FloatAsNumber(5103.34)
	Print "This is how we show currency in this locale :  " + FloatAsCurrency(5103.34)

	Print ""

End Function

