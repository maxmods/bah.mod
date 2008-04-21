SuperStrict

Import BaH.Locale


' Load the BLF
LoadLocaleFile("datetimes_locale.blf")


Function GetShortMonths:String[](locale:String = Null)

	Global names:String[] = ["jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"]

	Local list:String[] = New String[names.length]

	For Local i:Int = 0 Until names.length

		list[i] = GetLocaleText(names[i], locale)
		
	Next

	Return list

End Function

Function GetLongMonths:String[](locale:String = Null)

	' note that long months for May uses : "may_long"
	Global names:String[] = ["january", "february", "march", "april", "may_long", "june", "july", "august", ..
		"september", "october", "november", "december"]

	Local list:String[] = New String[names.length]

	For Local i:Int = 0 Until names.length

		list[i] = GetLocaleText(names[i], locale)

	Next

	Return list

End Function

Function GetShortDaysOfWeek:String[](locale:String = Null)

	Global names:String[] = ["sun", "mon", "tue", "wed", "thu", "fri", "sat"]

	Local list:String[] = New String[names.length]

	For Local i:Int = 0 Until names.length

		list[i] = GetLocaleText(names[i], locale)
		
	Next

	Return list

End Function

Function GetLongDaysOfWeek:String[](locale:String = Null)

	Global names:String[] = ["sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"]

	Local list:String[] = New String[names.length]

	For Local i:Int = 0 Until names.length

		list[i] = GetLocaleText(names[i], locale)

	Next

	Return list

End Function

