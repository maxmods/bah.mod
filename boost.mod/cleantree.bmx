
SuperStrict

Framework BRL.FileSystem
Import BRL.StandardIO

Local base:String = "/Applications/BlitzMax/mod/bah.mod/boost.mod/src/boost/"

processDir(base)


Function processDir(base:String, indent:Int = 0)

	Local fromDir:Int = ReadDir(base)
	
	If fromDir Then
	
		Local ind:String = ""
		For Local i:Int = 0 Until indent
			ind:+ " "
		Next

		Print ind + "Processing dir : " + base
	
		Local file:String = NextFile(fromDir)
		
		While file
		
			If Not file.StartsWith(".") Then
			
				Local fromFile:String = base + file
		
				Select FileType(fromFile)
					Case FILETYPE_FILE
						'Local toFile:String = base + file
					
						Print ind + "Removing : " + fromFile
						DeleteFile fromFile
						
					Case FILETYPE_DIR

						Local toDir:String = base + file
						
						If Not FileType(toDir) Then
							' create folder
							'Print ind + "** Creating : " + toDir
							'CreateDir(toDir)
							
						End If
						
						processDir(base + file + "/", indent + 2)
					
				End Select
			
			'Else
			'	Print ind + "IGNORING : " + file
			End If
					
			file = NextFile(fromDir)
		
		Wend
	
		CloseDir(fromDir)
	
	EndIf

End Function


