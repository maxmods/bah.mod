SuperStrict

Framework BaH.xls
Import BRL.Filesystem
Import BRL.StandardIO
Import BRL.Map
Import BRL.LinkedList

Local generator:TGenerator = New TGenerator.Create(RealPath("../sdk/Profile.xls"))

generator.Parse()


Type TGenerator

	Field profilePath:String

	Field xls:TBasicExcel

	Field typeMap:TMap = New TMap

	Method Create:TGenerator(path:String)
		profilePath = path
		Return Self
	End Method

	Method Parse()
		xls = New TBasicExcel.Create(profilePath)

		If xls Then
			ParseTypes()
			ParseMessages()

			xls.Close()
		End If
	End Method

	Method ParseTypes()
		Print "Parsing types..."

		Local sheet:TBasicExcelWorksheet = xls.GetWorksheetByName("Types")

		Local row:Int = 1
		Local rows:Int = sheet.GetTotalRows()

		Local currentType:TType

		While row < rows

			Local typeName:TBasicExcelCell = sheet.Cell(row, 0)

			If typeName.GetType() = CELL_STRING Then
				currentType = New TType
				currentType.name = typeName.GetString()

				Local baseType:TBasicExcelCell = sheet.Cell(row, 1)
				currentType.baseType = baseType.GetString()

				typeMap.Insert(currentType.name, currentType)

				Print "~t" + currentType.name
			Else
				Local valueName:TBasicExcelCell = sheet.Cell(row, 2)

				If valueName.GetType() = CELL_STRING Then
					Local value:TBasicExcelCell = sheet.Cell(row, 3)

					Local v:String

					Select value.GetType()
						Case CELL_INT
							v = value.Get()
						Case CELL_DOUBLE
							v = value.GetDouble()
						Case CELL_STRING
							v = value.GetString()
					End Select

					currentType.AddValue(valueName.GetString(), v)

				End If
			End If

			row:+ 1

		Wend
	End Method

	Method ParseMessages()
		Print "Parsing messages..."

		Local sheet:TBasicExcelWorksheet = xls.GetWorksheetByName("Messages")
	End Method

End Type

Type TType

	Field name:String
	Field baseType:String
	Field values:TList = New TList

	Method AddValue(name:String, value:String)
		Local v:TTypeValue = New TTypeValue
		v.name = name
		v.value = value
		values.AddLast(v)
	End Method

End Type

Type TTypeValue
	Field name:String
	Field value:String
End Type

