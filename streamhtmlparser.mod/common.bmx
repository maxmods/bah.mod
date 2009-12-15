' Copyright (c) 2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
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

Rem
bbdoc: Parses html text.
End Rem
Const HTMLPARSER_MODE_HTML:Int = 0
Rem
bbdoc: Parses javascript files.
End Rem
Const HTMLPARSER_MODE_JS:Int = 1
Rem
bbdoc: Parses CSS files.
about: No actual parsing is actually done but #inCSS() always returns true.
End Rem
Const HTMLPARSER_MODE_CSS:Int = 2
Rem
bbdoc: Parses an attribute list inside a tag.
about: To be used in a template expanded in the following context: <code>&lt;a $template&gt;</code>
End Rem
Const HTMLPARSER_MODE_HTML_IN_TAG:Int = 3

Rem
bbdoc: Not inside an attribute.
End Rem
Const HTMLPARSER_ATTR_NONE:Int = 0
Rem
bbdoc: Inside a normal attribute.
End Rem
Const HTMLPARSER_ATTR_REGULAR:Int = 1
Rem
bbdoc: Inside an attribute that accepts a uri.
End Rem
Const HTMLPARSER_ATTR_URI:Int = 2
Rem
bbdoc: Inside a javascript attribute.
End Rem
Const HTMLPARSER_ATTR_JS:Int = 3
Rem
bbdoc: Inside a css style attribute.
End Rem
Const HTMLPARSER_ATTR_STYLE:Int = 4

