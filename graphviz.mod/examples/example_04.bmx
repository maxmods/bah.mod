' 
'

SuperStrict

Framework BaH.Graphviz

Import BRL.StandardIO
Import brl.glmax2d

Local renderer:TGVGraphviz = TGVGraphviz.Create(800, 480)

' TODO : a MaxGUI example

Graphics 800, 600, 0

Cls

Flip 0


Print _max2dDriver.ToString()