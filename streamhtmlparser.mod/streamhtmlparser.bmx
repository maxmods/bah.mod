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
	bbdoc: Resets the parser to its initial state and to the default mode, which is HTMLPARSER_MODE_HTML.
	about: All internal context like tag name, attribute name or the state of the statemachine are reset
	to its original values as if the object was just created.
	End Rem
	Method reset()
		htmlparser_reset(parserPtr)
	End Method
	
	Rem
	bbdoc: Resets the parser to its initial state and changes the parser mode.
	about: All internal context like tag name, attribute name or the state of the statemachine are reset
	to their original values as if the object was just created.
	<p>Available modes:
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td> HTMLPARSER_MODE_HTML </td><td> Parses html text </td></tr>
	<tr><td> HTMLPARSER_MODE_JS </td><td> Parses javascript files </td></tr>
	<tr><td> HTMLPARSER_MODE_CSS </td><td> Parses CSS files. No actual parsing is actually done but htmlparser_in_css() always returns true.</td></tr>
	<tr><td> HTMLPARSER_MODE_HTML_IN_TAG </td><td> Parses an attribute list inside a tag. To be used in a template expanded in the following context: &lt;a $template&gt;</td></tr>
	</table>
	</p>
	End Rem
	Method resetMode(_mode:Int)
		htmlparser_reset_mode(parserPtr, _mode)
	End Method
	
	Rem
	bbdoc: Copies the context of the htmlparser to a new htmlparser.
	about: Also copies over the instances of the state machine, the jsparser and the entity filter but
	not the statemachine definition since this one is read only.
	End Rem
	Method copy:THtmlParser()
		Local dest:THtmlParser = New THtmlParser
		htmlparser_copy(dest.parserPtr, parserPtr)
		Return dest
	End Method
	
	Rem
	bbdoc: Receives an htmlparser context and returns the current html state.
	about: The return value will be one of HTMLPARSER_STATE_TEXT, HTMLPARSER_STATE_TAG, HTMLPARSER_STATE_ATTR,
	HTMLPARSER_STATE_VALUE, HTMLPARSER_STATE_COMMENT, HTMLPARSER_STATE_JS_FILE, HTMLPARSER_STATE_CSS_FILE or HTMLPARSER_STATE_ERROR.
	End Rem
	Method state:Int()
		Return htmlparser_state(parserPtr)
	End Method
	
	Rem
	bbdoc: Parses the input html stream and returns the finishing state.
	about: Returns HTMLPARSER_ERROR if unable to parse the input. If #parse() is called after an error
	situation was encountered the behaviour is unspecified. At this point, #reset() or #resetMode() can be called to reset the state.
	End Rem
	Method parse:Int(text:String)
		Local s:Byte Ptr = text.ToUTF8String()
		Local ret:Int = htmlparser_parse(parserPtr, s, strlen_(s))
		MemFree(s)
		Return ret
	End Method
	
	Rem
	bbdoc: Parses the input html stream of a single character and returns the finishing state.
	about: Returns HTMLPARSER_ERROR if unable to parse the input. If #parse() is called after an error
	situation was encountered the behaviour is unspecified. At this point, #reset() or #resetMode() can be called to reset the state.
	End Rem
	Method parseChr:Int(char:Byte)
		Return htmlparser_parse(parserPtr, Varptr char, 1)
	End Method
	
	Rem
	bbdoc: Returns true if the parser is inside an attribute value and the value is surrounded by single or double quotes.
	End Rem
	Method isAttrQuoted:Int()
		Return htmlparser_is_attr_quoted(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns true if the parser is currently in javascript.
	about: This can be a an attribute that takes javascript, a javascript block or the parser can just be in HTMLPARSER_MODE_JS.
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
