SuperStrict

Framework BaH.BoostLocale
Import BRL.StandardIO

Local gen:TBLGenerator = New TBLGenerator.Create()

Local loc:TBLLocale = gen.generate("ru_RU.UTF-8")

If loc Then

	Print "Name     : " + loc.name()
	Print "Language : " + loc.language()
	Print "Country  : " + loc.country()
	Print "Variant  : " + loc.variant()
	Print "Encoding : " + loc.encoding()

End If
