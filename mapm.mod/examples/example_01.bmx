SuperStrict


Framework BaH.MAPM
Import BRL.StandardIO
Import BRL.Math


Local value:TMAPM = New TMAPM.Create("45")

Print value.Sin(32).ToFixtPtString(32)
Print (Sin(45))