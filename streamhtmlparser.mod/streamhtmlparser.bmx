SuperStrict


Module BaH.StreamHTMLParser


Import "common.bmx"

Rem
bbdoc: 
End Rem
Type THtmlParser

	Field parserPtr:Byte Ptr
	
	Method New()
		parserPtr = htmlparser_new()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method reset()
		htmlparser_reset(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method resetMode(_mode:Int)
		htmlparser_reset_mode(parserPtr, _mode)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method copy:THtmlParser()
		Local dest:THtmlParser = New THtmlParser
		htmlparser_copy(dest.parserPtr, parserPtr)
		Return dest
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method state:Int()
		Return htmlparser_state(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method parse:Int(text:String)
		Local s:Byte Ptr = text.ToUTF8String()
		Local ret:Int = htmlparser_parse(parserPtr, s, strlen_(s))
		MemFree(s)
		Return ret
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method parseChr:Int(char:Byte)
		Return htmlparser_parse(parserPtr, Varptr char, 1)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isAttrQuoted:Int()
		Return htmlparser_is_attr_quoted(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method inJS:Int()
		Return htmlparser_in_js(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method tag:String()
		Return String.FromUTF8String(htmlparser_tag(parserPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method attr:String()
		Return String.FromUTF8String(htmlparser_attr(parserPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method value:String()
		Return String.FromUTF8String(htmlparser_value(parserPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method inCSS:Int()
		Return htmlparser_in_css(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method JSState:Int()
		Return htmlparser_js_state(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isJSQuoted:Int()
		Return htmlparser_is_js_quoted(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method valueIndex:Int()
		Return htmlparser_value_index(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isURLStart:Int()
		Return htmlparser_is_url_start(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method attrType:Int()
		Return htmlparser_attr_type(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getLineNumber:Int()
		Return htmlparser_get_line_number(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setLineNumber(line:Int)
		htmlparser_set_line_number(parserPtr, line)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getColumnNumber:Int()
		Return htmlparser_get_column_number(parserPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setColumnNumber(col:Int)
		htmlparser_set_column_number(parserPtr, col)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getErrorMsg:String()
		Return String.FromUTF8String(htmlparser_get_error_msg(parserPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method insertText:Int()
		Return htmlparser_insert_text(parserPtr)
	End Method
	
	Method Delete()
		If parserPtr Then
			htmlparser_delete(parserPtr)
			parserPtr = Null
		End If
	End Method

End Type
