SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d1:TDate = TDate.Create(2000, Jan, 1)
Local d2:TDate = TDate.localDay()

Print d2.subtractDate(d1)
