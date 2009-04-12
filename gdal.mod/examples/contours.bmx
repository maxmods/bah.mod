SuperStrict

Framework BaH.GDAL
Import BRL.StandardIO

Local srcFilename:String = "N57W003.hgt"
Local dstFilename:String = "N57W003.kml"
Local format:String = "kml"

GDALAllRegister()
OGRRegisterAll()

' Open source raster file.

Local dataset:GDALDataset = GDALOpen(srcFilename, GA_ReadOnly)

If Not dataset Then

	Print "Could not open file"
	End

End If

Local i:Int = 1
Local band:GDALRasterBand = dataset.GetRasterBand(i)

If Not band Then

	Print "Band " + i + " does not exist on dataset"
	End

End If

' Try to get a coordinate system from the raster.

Local srs:OGRSpatialReference
Local ref:String = dataset.GetProjectionRef()

If ref Then

	srs = New OGRSpatialReference.Create(ref)

End If

' Create the outputfile.
Local driver:OGRSFDriver = OGRSFDriverRegistrar.GetDriverByName(format)

If Not driver Then
	Print "Unable to find format driver named " + format
	End
End If

Local dataSource:OGRDataSource = driver.CreateDataSource(dstFilename)

If Not dataSource Then
	Print "Failed to create datasource"
	End
End If

Local layer:OGRLayer = dataSource.CreateLayer("contour", srs, wkbLineString)

If Not layer Then
	Print "Failed to create layer"
	End
End If

Local fld:OGRFieldDefn = New OGRFieldDefn.Create("ID", OFTInteger)
fld.SetWidth(8)
layer.CreateField(fld, False)
fld.Free()


Local res:Int = band.GenerateContour(10, 0, Null, False, 0, layer, 0, -1)

dataSource.Free()
dataset.Close()

