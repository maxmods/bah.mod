SuperStrict

Framework brl.glmax2d
Import brl.standardio
Import brl.linkedlist

Local file:String = "spiral6.csv"

Local txt:String = LoadText(file)
Local commands:TCommand[] = LoadCommands(txt.split("~n"))


Graphics 1024, 768, 0

Local index:Int
Local centerX:Float = GraphicsWidth()/2
Local centerY:Float = GraphicsHeight()/2

Local scale:Float = 10
Local ended:Int = False

While Not KeyDown(key_escape)

	If Not ended And index < commands.length Then
	
		If commands[index].cmd = "JUMP" Then
			index :+ 1
		End If
		
		If commands[index].cmd = "END" Then
			ended = True
			Continue
		End If
		
		Local lastCommand:TCommand = commands[index - 1]
		Local currentCommand:TCommand = commands[index]
		
		Local x0:Float = centerX + (lastCommand.x * scale)
		Local y0:Float = centerY + (lastCommand.y * scale)
		Local x1:Float = centerX + (currentCommand.x * scale)
		Local y1:Float = centerY + (currentCommand.y * scale)
		
		DrawLine x0, y0, x1, y1

		Flip 0

		DrawLine x0, y0, x1, y1

		Flip 0
	
		index :+ 1
	
	End If
	
Wend




Type TCommand

	Field cmd:String
	Field x:Double
	Field y:Double

End Type


Function LoadCommands:TCommand[](commands:String[])

	Local cmd:TCommand[] = New TCommand[commands.length]
	
	For Local i:Int = 0 Until commands.length
	
		Local line:String[] = commands[i].Replace("~q", "").Split(",")
	
		If line.length = 4 Then	
			Local c:TCommand = New TCommand
			cmd[i] = c
			
			c.cmd = line[1]
			c.x = line[2].ToDouble()
			c.y = line[3].ToDouble()
		End If	
	Next

	Return cmd	

End Function
