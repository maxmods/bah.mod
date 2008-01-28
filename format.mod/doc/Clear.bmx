SuperStrict

Framework BaH.Format
Import BRL.StandardIO

Local formatter:TFormatter = TFormatter.Create("%2d.   %.2d/%02d/%4d")

Local count:Int = 1

For Local m:Int = 1 To 12

	For Local d:Int = 1 To 21 Step 5
	
		' apply the arguments
		formatter.IntArg(count)
		formatter.IntArg(d).IntArg(m).IntArg(2007)
	
		' print the formatted string
		Print formatter.Format()
	
		' reset the arguments
		formatter.Clear()

		count:+ 1		
	Next

Next