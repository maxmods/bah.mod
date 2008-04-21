SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local pt:TTime = TTime.Create(TDate.Create(2004, Nov, 5), TDHours(10))

Local zone:TTimeZone = TTimeZone.Create("MST-07")

Local az:TLocalDateTime = TLocalDateTime.CreateFromTime(pt, zone)

Print az.UTCTime().toString()
Print az.localTime().toString()
