SuperStrict

Framework BaH.GDAL
Import BRL.StandardIO

GDALAllRegister()

Local poDataset:GDALDataset = GDALOpen("N57W003.hgt", GA_ReadOnly)

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

