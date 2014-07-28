SuperStrict

Framework BaH.Diff
Import BRL.StandardIO

Local a:String = "10~n20~n30~n40~n50~n50~nunmodified~nmodified"
Local b:String = "10~n20~n30~n40~n50~n50~nmodified~nmodified"

Local diff:TDiff = New TDiffLines.Create(a.Split("~n"), b.Split("~n"))

diff.compose()
diff.composeUnifiedHunks()

diff.printUnifiedFormat()

