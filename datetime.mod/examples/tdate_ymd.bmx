SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local d:TDate = TDate.Create(2006, Dec, 24)

Local year:Int, month:Int, day:Int

d.ymd(year, month, day)

Print year
Print month
Print day
