SuperStrict

Import BRL.LinkedList

Type BlitzMaxType

	Const _BYTE:Int = 0
	Const _SHORT:Int = 1
	Const _INT:Int = 2
	Const _LONG:Int = 3
	Const _FLOAT:Int = 4
	Const _DOUBLE:Int = 5
	Const _STRING:Int = 6
	Const _OBJECT:Int = 7
	
	Function map:Int(_type:String)
		If _type = "Byte" Then
			Return _BYTE
		Else If _type = "Short" Then
			Return _SHORT
		Else If _type = "Int" Then
			Return _INT
		Else If _type = "Long" Then
			Return _LONG
		Else If _type = "Float" Then
			Return _FLOAT
		Else If _type = "Double" Then
			Return _DOUBLE
		Else If _type = "String" Then
			Return _STRING
		End If
		
		Return _OBJECT
	End Function


	Function isPrimitive:Int(_type:Int)
		Select _type
			Case _OBJECT, _STRING
				Return False
			Default
				Return True
		End Select
	End Function
	
End Type

Type ScopeType

	Const _LOCAL:Int = 0
	Const _GLOBAL:Int = 1
	Const _CONST:Int = 2
	Const _FIELD:Int = 3
	
	Function map:Int(scope:String)
		If scope = "Local" Then
			Return _LOCAL
		Else If scope = "Global" Then
			Return _GLOBAL
		Else If scope = "Const" Then
			Return _CONST
		Else If scope = "Field" Then
			Return _FIELD
		End If
		
		Return _LOCAL
	End Function
	

End Type


Type TBlitzMaxStackScope

	Field name:String
	
	Field where:String
	Field source:String
	Field line:Int
	Field column:Int
	
	Field level:Int
	
	Field variables:TList
	
	Method Create:TBlitzMaxStackScope(name:String, level:Int)
		Self.name = name
		Self.level = level
		Return Self
	End Method
	
	Method setSource(source:String)
		Local parts:String[] = source.split("\<")
		Self.source = convertPath(parts[0])
		
		Local lc:String[] = parts[1][0..parts[1].length - 1].split(",")
		
		line = lc[0].ToInt()
		column = lc[1].ToInt()
	End Method

	Method convertPath:String(path:String)
?win32
		Return path.Replace("/", "\")
?Not win32
		Return path
?
	End Method
	
	Method addVariable(variable:String)
		If variable.StartsWith("Function") Then
			where = variable[variable.Find(" ") + 1..]
		Else
			If Not variables Then
				variables = New TList
			End If
			
			variables.AddLast(New TBlitzMaxScopeVariable.Create(variable))
		End If
	End Method
	
	Method getName:String()
		Return name
	End Method
	
	Method getSource:String()
		Return source
	End Method
	
	Method getLine:Int()
		Return line
	End Method
	
	Method getColumn:Int()
		Return column
	End Method
	
	Method getWhere:String()
		Return where
	End Method
	
	Method getVariables:TList()
		Return variables
	End Method
	
	Method getLevel:Int()
		Return level
	End Method
	
End Type


Type TBlitzMaxScopeVariable

	Field scope:Int

	Field name:String
	Field classname:String
	Field page:Int
	Field pagesize:Int
	Field baseType:Int
	Field _type:String
	Field facet:String
	Field size:Int
	Field children:Int
	Field key:String
	
	Field value:String
	

	Method Create:TBlitzMaxScopeVariable(line:String)

		If Not line.StartsWith("[")
		
			' the scope type (Local, Const etc)
			Local i:Int = line.Find(" ")
			scope = ScopeType.map(line[0..i])
			
			Local variable:String = line[i + 1..]
			
			' the variable name
			i = variable.Find(":")
			name = variable[0..i]
			
			variable = variable[i + 1..]
			
			' the variable type
			i = variable.Find("=")
			_type = variable[0..i]
			baseType = BlitzMaxType.map(_type)
			
			If baseType = BlitzMaxType._OBJECT Then
				children = True
			End If
			
			value = variable[i + 1..]
			
			If value.StartsWith("$") Then
				key = value[1..]
				value = Null
			Else If value = "Null" Then
				value = Null
			End If
		Else
		' TODO : add array element support.
		End If
		
		Return Self
	End Method
	
	Method getScope:Int()
		Return scope
	End Method
	
	Method getName:String()
		Return name
	End Method
	
	Method getType:String()
		Return _type
	End Method
	
	Method getBaseType:Int()
		Return baseType
	End Method
	
	Method hasChildren:Int()
		Return children
	End Method
	
	Method getKey:String()
		Return key
	End Method
	
	Method getValue:String()
		Return value
	End Method
	
End Type

