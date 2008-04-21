SuperStrict

' calculates the number of days since the user was born...

Framework BaH.DateTime
Import BRL.StandardIO


Local s:String = Input("Enter birth day YYYY-MM-DD (eg: 2002-02-01): ")

Local birthday:TDate = TDate.fromString(s)

If birthday Then
	Local today:TDate = TDate.localDay()
	
	Local daysAlive:Int = today.subtractDate(birthday)
	
	If daysAlive = 1 Then
		Print "Born yesterday, very funny"
	Else If daysAlive < 0 Then
		Print "Not born yet, hmm: " + daysAlive + " days"
	Else
		Print "Days alive: " + daysAlive
	End If
Else
	Print "An invalid date was entered...."
End If