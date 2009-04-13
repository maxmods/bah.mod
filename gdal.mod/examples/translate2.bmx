SuperStrict

Framework BaH.GDAL
Import BRL.StandardIO


Local srcFilename:String = "N57W003.hgt"
Local dstFilename:String = "N57W003.png"
Local format:String = "png"

Local outputType:Int = GDT_UInt16

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

Local oXSize:Int = anSrcWin[2]
Local oYSize:Int = anSrcWin[3]

' Make a virtual clone.  

Local vds:VRTDataset = New VRTDataset.Create(oXSize, oYSize)

' projection
Local projection:String = dataset.GetProjectionRef()
If projection Then
	vds.SetProjection( projection )
End If


Local adfGeoTransform:Double[6]

' transform?
If dataset.GetGeoTransform(adfGeoTransform) = CE_None Then

	adfGeoTransform[0] :+ anSrcWin[0] * adfGeoTransform[1] + anSrcWin[1] * adfGeoTransform[2]
	adfGeoTransform[3] :+ anSrcWin[0] * adfGeoTransform[4] + anSrcWin[1] * adfGeoTransform[5]
        
	adfGeoTransform[1] :* anSrcWin[2] / Double(oXSize)
	adfGeoTransform[2] :* anSrcWin[3] / Double(oYSize)
	adfGeoTransform[4] :* anSrcWin[2] / Double(oXSize)
	adfGeoTransform[5] :* anSrcWin[3] / Double(oYSize)
        
	vds.SetGeoTransform(adfGeoTransform)
End If

If dataset.GetGCPCount() > 0 Then

Print "***************"

End If


'  Transfer generally applicable metadata.
vds.SetMetadata(dataset.GetMetadata())

' Transfer metadata that remains valid if the spatial arrangement of the data is unaltered.
If anSrcWin[0] = 0 And anSrcWin[1] = 0 And ..
		anSrcWin[2] = dataset.GetRasterXSize() And ..
		anSrcWin[3] = dataset.GetRasterYSize() Then

	Local zmd:String[] = dataset.GetMetadata("RPC")
	
	If zmd Then
		vds.SetMetadata( zmd, "RPC" )
	End If


End If


' Process all bands.
For Local i:Int = 0 Until bandCount

	Local vrtBand:VRTSourcedRasterBand
	Local srcBand:GDALRasterBand = dataset.GetRasterBand(bandList[i])
	
	' Select output data type to match source.
	Local bandType:Int
	If outputType = GDT_Unknown Then
		bandType = srcBand.GetRasterDataType()
	Else
		bandType = outputType
	End If


	' Create this band.
	vds.AddBand(bandType, Null)
	vrtBand = vds.GetRasterBand(i + 1)
	
	' Do we need to collect scaling information?

	' Create a simple or complex data source depending on the translation type required.
	vrtBand.AddSimpleSource(srcBand, anSrcWin[0], anSrcWin[1], anSrcWin[2], anSrcWin[3], 0, 0, oXSize, oYSize)

	vrtBand.CopyCommonInfoFrom(srcBand)
	
Next


' Write to the output file using CopyCreate().

Local outDataset:GDALDataset = driver.CreateCopy(dstFilename, vds, False)

If outDataset Then
	outDataset.Close()
End If

vds.Close()
dataset.Close()

