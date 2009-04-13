SuperStrict

Framework BaH.GDAL
Import BRL.StandardIO


Local srcFilename:String = "N57W003.hgt"
Local dstFilename:String = "N57W003.png"
Local format:String = "png"

Local anSrcWin:Int[4]
Local rasterXSize:Int, rasterYSize:Int


GDALAllRegister()


' Attempt to open source file.

Local dataset:GDALDataset = GDALOpenShared(srcFilename, GA_ReadOnly)

If Not dataset Then
	Print "Could not open file"
	End
End If

' Handle subdatasets.
Local meta:String[] = dataset.GetMetadata("SUBDATASETS")
If meta.length > 0 Then
	Print "Woops - not supported in this example"
	End
End If

' Collect some information from the source file.
rasterXSize = dataset.GetRasterXSize()
rasterYSize = dataset.GetRasterYSize()

Print "Input file size is " + rasterXSize + ", " + rasterYSize

anSrcWin[2] = rasterXSize
anSrcWin[3] = rasterYSize


' Build band list to translate.
Local bandCount:Int = dataset.GetRasterCount()

If bandCount = 0 Then
	Print "Input file has no bands, and so cannot be translated."
	End
End If

Local bandList:Int[] = New Int[bandCount]
For Local i:Int = 0 Until bandCount
	bandList[i] = i + 1
Next


' Find the output driver.

Local driver:GDALDriver = GDALDriverManager.GetDriverByName(format)

If Not driver Then

	Print "Output driver '" + format + "' not recognised."
	Print "The following format drivers are configured and support output:"
 
	For Local i:Int = 0 Until GDALDriverManager.GetDriverCount()
		driver = GDALDriverManager.GetDriver(i)
		
		If driver Then
			Print "   " + driver.GetShortName() + "     :     " + driver.GetLongName()
		End If
	Next

	End

End If

' create a copy and save...

Local outDataset:GDALDataset = driver.CreateCopy(dstFilename, dataset, False)

If outDataset Then
	outDataset.Close()
End If


