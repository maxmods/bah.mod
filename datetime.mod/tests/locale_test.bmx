SuperStrict


Framework BaH.DateTime
Import BRL.StandardIO


' this is today
Local today:TDate = TDate.Create(2007, Mar, 15)


Local g1:TDateFacet = TDateFacet.CreateForLocale("de-DE")
Local g2:TDateFacet = TDateFacet.CreateForLocale("German-Germany")
Local g3:TDateFacet = TDateFacet.CreateForLocale("de_DE")

SetCurrentDateFacet(defaultDateFacet)
Print "def (en_US)         : " + today.toString()

SetCurrentDateFacet(g1)
Print "g1  (de-DE)         : " + today.toString()

SetCurrentDateFacet(g2)
Print "g2  (German-Germany): " + today.toString()

SetCurrentDateFacet(g3)
Print "g3  (de_DE)         : " + today.toString()

SetCurrentDateFacet(defaultDateFacet)
Print "def (en_US)         : " + today.toString()

'Delay(5000)