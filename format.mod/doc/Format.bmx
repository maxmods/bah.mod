SuperStrict

Framework BaH.Format
Import BRL.StandardIO

Local formatter:TFormatter = TFormatter.Create("->%-10s<->%10s<->%.5s<-")

formatter.StringArg("Left").StringArg("Right").StringArg("Trimmed")

Print formatter.Format()
