SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2007, 1, 1))

Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 10, 1), TDate.Create(2007, 2, 1))

Local intersection:TDatePeriod = p1.intersection(p2)

If intersection Then
	Print intersection.toString()
End If
