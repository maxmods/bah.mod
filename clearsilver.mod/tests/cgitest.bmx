SuperStrict

Framework BaH.ClearSilver
Import BRL.StandardIo


Local cgi:TCGI = TCGI.Create()

Print "Content-type: text/html~n"

'DebugStop
Print cgi.hdf.writestring()
