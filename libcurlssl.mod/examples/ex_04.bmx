SuperStrict

' example showing connection to ftp server, then processing the directory list using the FtpParser
' module. Symbolic links are also tested to see whether they are dirs or possibly files.

Framework BaH.libcurlssl
Import BaH.FTPParser
Import BRL.StandardIO

Local parser:TFTPParserFactory = TFTPParserFactory.Create()


Local curl:TCurlEasy = TCurlEasy.Create()

'Local stream:TStringStream = New TStringStream

' redirect data to stream
curl.setWriteString()

' ftp connect and initial directory
curl.setOptString(CURLOPT_URL, "ftp.sunsite.dk/pub/")

curl.setOptInt(CURLOPT_FTP_FILEMETHOD, CURLFTPMETHOD_SINGLECWD)

' go ! !
Local res:Int = curl.perform()

If res Then
	Print "***** " + CurlError(res) + " *****"
	End
End If

Local base:TFilePath = TFilePath.Create()
base.absolutePath = "/pub/"

Local entries:TList = parser.parseList(base, curl.toString())

Print curl.toString()

If entries Then

	For Local file:TFilePath = EachIn entries

		' some basic symbolic link resolving....	
		If file.isSymbolic() Then
			' try and discover if this is a dir or file
			curl.setOptString(CURLOPT_URL, "ftp.sunsite.dk" + file.absolutePath + "/")
			res = curl.perform()

			If Not res Then ' a dir :-)
				file.attributes.fType:| FTP_TYPE_DIRECTORY
			Else				
				file.attributes.fType:| FTP_TYPE_FILE
			End If
		End If
	
		If file.isFile() Then
			Print "F : " + file.absolutepath + " - " + file.attributes.modified.toString()
		Else
			Print "D : " + file.absolutepath + " - " + file.attributes.modified.toString()
		End If
	
	Next

End If


curl.cleanup()

End
