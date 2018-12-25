SuperStrict

Framework brl.standardio
Import bah.bsdnt

Local a:TZZ = New TZZ(100)
Local b:TZZ = New TZZ(200)

Print a.ToString()
Print b.ToString()

Local c:TZZ = a + b

Print c.ToString()

c = a * b

Print c.ToString()

Local d:TZZ = New TZZ("10000000000000000000")

d :+ c

Print d.ToString()

d :/ c

Print d.ToString()

For Local i:Int = 0 Until 100
	c :+ b * a
Next

Print c.ToString()

c = -b

Print c.ToString()

Print c = -200
Print c = 100

b = -b
Print c = b

c = b / a

Print c.ToString()

c = b / 5

Print c.ToString()

c :/ -4

Print c.ToString()
