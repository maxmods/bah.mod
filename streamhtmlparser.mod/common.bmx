SuperStrict

Import "source.bmx"


Extern

	Function htmlparser_new:Byte Ptr()
	Function htmlparser_delete(handle:Byte Ptr)
	Function htmlparser_reset(handle:Byte Ptr)
	Function htmlparser_reset_mode(handle:Byte Ptr, _mode:Int)
	Function htmlparser_copy(handle:Byte Ptr, source:Byte Ptr)
	Function htmlparser_state:Int(handle:Byte Ptr)
	Function htmlparser_parse:Int(handle:Byte Ptr, text:Byte Ptr, size:Int)
	Function htmlparser_is_attr_quoted:Int(handle:Byte Ptr)
	Function htmlparser_in_js:Int(handle:Byte Ptr)
	Function htmlparser_tag:Byte Ptr(handle:Byte Ptr)
	Function htmlparser_attr:Byte Ptr(handle:Byte Ptr)
	Function htmlparser_value:Byte Ptr(handle:Byte Ptr)
	Function htmlparser_in_css:Int(handle:Byte Ptr)
	Function htmlparser_js_state:Int(handle:Byte Ptr)
	Function htmlparser_is_js_quoted:Int(handle:Byte Ptr)
	Function htmlparser_value_index:Int(handle:Byte Ptr)
	Function htmlparser_is_url_start:Int(handle:Byte Ptr)
	Function htmlparser_attr_type:Int(handle:Byte Ptr)
	Function htmlparser_get_line_number:Int(handle:Byte Ptr)
	Function htmlparser_set_line_number(handle:Byte Ptr, line:Int)
	Function htmlparser_get_column_number:Int(handle:Byte Ptr)
	Function htmlparser_set_column_number(handle:Byte Ptr, col:Int)
	Function htmlparser_get_error_msg:Byte Ptr(handle:Byte Ptr)
	Function htmlparser_insert_text:Int(handle:Byte Ptr)
	
	Function strlen_:Int(s:Byte Ptr) = "strlen"
	
End Extern


Const HTMLPARSER_STATE_TEXT:Int = 0
Const HTMLPARSER_STATE_TAG:Int = 1
Const HTMLPARSER_STATE_ATTR:Int = 2
Const HTMLPARSER_STATE_VALUE:Int = 3
Const HTMLPARSER_STATE_COMMENT:Int = 4
Const HTMLPARSER_STATE_JS_FILE:Int = 5
Const HTMLPARSER_STATE_CSS_FILE:Int = 6
Const HTMLPARSER_STATE_ERROR:Int = 7

Const HTMLPARSER_MODE_HTML:Int = 0
Const HTMLPARSER_MODE_JS:Int = 1
Const HTMLPARSER_MODE_CSS:Int = 2
Const HTMLPARSER_MODE_HTML_IN_TAG:Int = 3

Const HTMLPARSER_ATTR_NONE:Int = 0
Const HTMLPARSER_ATTR_REGULAR:Int = 1
Const HTMLPARSER_ATTR_URI:Int = 2
Const HTMLPARSER_ATTR_JS:Int = 3
Const HTMLPARSER_ATTR_STYLE:Int = 4

