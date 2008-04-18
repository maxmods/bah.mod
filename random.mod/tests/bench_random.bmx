SuperStrict

Framework BRL.Random
Import BRL.StandardIO


SeedRnd(100)

Local start:Long = MilliSecs()

For Local i:Int = 0 Until 100000
	Local r:Int = Rand(0)
	r = Rand(1, 50)
	r = Rand(200, 10000)
Next

Print "Time Rand() = " + (MilliSecs() - start)


SeedRnd(100)


start = MilliSecs()

For Local i:Int = 0 Until 100000
	Local r:Double = Rnd()
	r = Rnd(1, 50)
	r = Rnd(200, 10000)
Next

Print "Time Rnd() = " + (MilliSecs() - start)


SeedRnd(100)

start = MilliSecs()

For Local i:Int = 0 Until 100000
	Local r:Float = RndFloat()
Next

Print "Time RndFloat() = " + (MilliSecs() - start)


SeedRnd(100)

start = MilliSecs()

For Local i:Int = 0 Until 100000
	Local r:Double = RndDouble()
Next

Print "Time RndDouble() = " + (MilliSecs() - start)

