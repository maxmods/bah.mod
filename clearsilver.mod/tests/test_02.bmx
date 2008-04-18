SuperStrict

Framework BaH.ClearSilver
Import BRL.StandardIO

Local s:String = "BlitzMax is Cool & Rocking !! £$%^&*()'"

Print s

s = URLEscape(s)

Print s

Print URLUnescape(s)

