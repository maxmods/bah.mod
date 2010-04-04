SuperStrict

Framework BaH.GDAL
Import BRL.StandardIO

GDALAllRegister()

Local poDataset:GDALDataset = New GDALDataset.Open("SouthAfrica.wind.grb", GA_ReadOnly)

If poDataset = Null Then

	Print "Could not open file"
	End

End If

Print "Driver: " + poDataset.GetDriver().GetDescription() + "/" + ..
	poDataset.GetDriver().GetMetadataItem( GDAL_DMD_LONGNAME )

Print "Size is " + poDataset.GetRasterXSize() + "x" + ..
	poDataset.GetRasterYSize() + "x" + ..
	poDataset.GetRasterCount()

If poDataset.GetProjectionRef() Then
	Print "Projection is '" + poDataset.GetProjectionRef() + "'"
End If

Local adfGeoTransform:Double[6]

If poDataset.GetGeoTransform( adfGeoTransform ) = CE_None Then
	Print "Origin = (" + adfGeoTransform[0] + ", " + adfGeoTransform[3] + ")"
	Print "Pixel Size = (" + adfGeoTransform[1] + ", " + adfGeoTransform[5] + ")"
End If

For Local i:Int = 1 To poDataset.GetRasterCount()

	Local band:GDALRasterBand = poDataset.GetRasterBand(i)
	
	Print band.GetDescription()
	Local meta:String[] = band.GetMetadata()
	For Local n:Int = 0 Until meta.length
		Print "   " + meta[n]
	Next
	
	
	Local xSize:Int = band.GetXSize()
	
	Local scanline:Float Ptr = Float Ptr(MemAlloc(xSize * 4))
	
	For Local n:Int = 0 Until band.GetYSize()
	
		band.RasterIO(GF_Read, 0, 0, xSize, 1, scanLine, xSize, 1, GDT_Float32, 0, 0)
		
		Local s:String
		For Local f:Int = 0 Until xSize
			s:+ scanline[i] + " "
		Next
		Print s
	
	Next
	
	MemFree(scanLine)

Next
