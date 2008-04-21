SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local td1:TTimeDuration = TTimeDuration.Create(12, 30, 20)
Local td2:TTimeDuration = TTimeDuration.Create(1, 30, 20)

Print td1.subtract(td2).toString()

