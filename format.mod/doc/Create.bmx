SuperStrict

Framework BaH.Format
Import BRL.StandardIO

Local formatter:TFormatter = TFormatter.Create("Value = %2.1f%%")

Print formatter.FloatArg(46.4).Format()

