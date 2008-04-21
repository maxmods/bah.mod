SuperStrict

Framework BaH.DateTime
Import BRL.StandardIO

Local p1:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 1, 1), TDate.Create(2006, 10, 1))
Local p2:TDatePeriod = TDatePeriod.Create(TDate.Create(2006, 12, 1), TDate.Create(2007, 2, 1))

Local span:TDatePeriod = p1.span(p2)

If Not span.isNull() Then
	Print span.toString()
End If