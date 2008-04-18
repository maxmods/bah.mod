SuperStrict

Framework BaH.Random
Import BRL.StandardIO

Print "Int~n"

For Local i:Int = 0 Until 20
	Print Rand(1, 100)
Next

Print "Long~n"

' need to reseed for 64bit
SeedRnd(0)

For Local i:Int = 0 Until 20
	Print Rand64($FFFFFFFFFFFFF:Long)
Next

Print "Float~n"

' need to reseed for non-64bit
SeedRnd(0)

For Local i:Int = 0 Until 20
	Print RndFloat()
Next

Print "Double~n"


For Local i:Int = 0 Until 20
	Print RndDouble()
Next

Print "Rnd~n"

Print Rnd()
Print Rnd(5)
Print Rnd(6, 10)
