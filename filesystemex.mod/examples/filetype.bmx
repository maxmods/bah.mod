SuperStrict

Framework BaH.FileSystemEX
Import BRL.FileSystem
Import BRL.StandardIO

Print BRL.FileSystem.FileType("filetype.bmx")
Print BaH.FileSystemEx.FileType("filetype.bmx")

Print "---"

Print BRL.FileSystem.FileType("../examples")
Print BaH.FileSystemEx.FileType("../examples")


Print "---"

Print BRL.FileSystem.FileType("nofile.txt")
Print BaH.FileSystemEx.FileType("nofile.txt")

