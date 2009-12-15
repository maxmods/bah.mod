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

Rem
bbdoc: HTML and JavaScript context parser.
End Rem
Module BaH.StreamHTMLParser

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."


Import "common.bmx"

Rem
bbdoc: HTML Parser
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
	bbdoc: Returns the current tag or NULL if not available or we haven't seen the entire tag yet.
 	about: There is no stack implemented because we currently don't have a need for
	it, which means tag names are tracked only one level deep.
	<p>
	This is better understood by looking at the following example:
 	<pre>
	&lt;b [tag=b]&gt;
	  [tag=b]
	  &lt;i&gt;
	   [tag=i]
	  &lt;/i&gt;
	 [tag=NULL]
	&lt;/b&gt;
 	</pre>
	The tag is correctly filled inside the tag itself and before any new inner tag is closed, at
	which point the tag will be null.
	</p>
	<p>
	For our current purposes this is not a problem, but we may implement a tag tracking stack in
	the future for completeness.
	End Rem
	Method tag:String()
		Return String.FromUTF8String(htmlparser_tag(parserPtr))
	End Method
	
	Rem
	bbdoc: Returns the current attribute name if after an attribute name or in an attribute value, or NULL otherwise.
	End Rem
	Method attr:String()
		Return String.FromUTF8String(htmlparser_attr(parserPtr))
	End Method
	
	Rem
	bbdoc: Returns the contents of the current attribute value.
	about: Returns NULL if not inside an attribute value.
	End Rem
	Method value:String()
		Return String.FromUTF8String(htmlparser_value(parserPtr))
	End Method
	
	Rem
	bbdoc: Returns true if the parser is currently inside a CSS construct.
	about: Currently this can be either a STYLE tag, a STYLE attribute or the fact that
	the parser was reset in HTMLPARSER_MODE_CSS using resetMode().
	End Rem
	Method inCSS:Int()
		Return htmlparser_in_css(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns the current state of the javascript state machine.
	about: Currently only present for testing purposes.
	End Rem
	Method JSState:Int()
		Return htmlparser_js_state(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if currently inside a javascript string literal and zero otherwise.
	End Rem
	Method isJSQuoted:Int()
		Return htmlparser_is_js_quoted(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns non-zero if currently inside an attribute value and zero otherwise.
	End Rem
	Method valueIndex:Int()
		Return htmlparser_value_index(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns true if this is the first character of a url inside an attribute.
	about: This method can be used by an html sanitizer or auto escaping system as a
	hint that it should validate the url for a whitelist of protocol handlers and
	for well-formedness, or that it should just escape a component of it.
	<p>
	For attributes that expect a URL, this will return true if we are at the
	first character of the URL, false otherwise.<br>
	For most attributes, this is the same as checking that we are at the first
	character of the attribute value but it also works correctly for the
	"content" attribute of the "meta" tag where the URL follows some earlier content.
	e.g: <code>&lt;meta http-equiv="refresh" * content="5; URL=http://bla."&gt;</code>
	</p>
	<p>
	For any other attributes, the result will always be false.
	</p>
	End Rem
	Method isURLStart:Int()
		Return htmlparser_is_url_start(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns the current attribute type.
	about: The attribute type can be one of:
	<table>
	<tr><th>Constant</th><th>Meaning</th></tr>
	<tr><td>HTMLPARSER_ATTR_NONE </td><td>not inside an attribute.</td></tr>
	<tr><td>HTMLPARSER_ATTR_REGULAR </td><td>Inside a normal attribute.</td></tr>
	<tr><td>HTMLPARSER_ATTR_URI </td><td>Inside an attribute that accepts a uri.</td></tr>
	<tr><td>HTMLPARSER_ATTR_JS </td><td>Inside a javascript attribute.</td></tr>
	<tr><td>HTMLPARSER_ATTR_STYLE </td><td>Inside a css style attribute.</td></tr>
	</table>
	End Rem
	Method attrType:Int()
		Return htmlparser_attr_type(parserPtr)
	End Method
	
	Rem
	bbdoc: Return the current line number.
	End Rem
	Method getLineNumber:Int()
		Return htmlparser_get_line_number(parserPtr)
	End Method
	
	Rem
	bbdoc: Set the current line number.
	End Rem
	Method setLineNumber(line:Int)
		htmlparser_set_line_number(parserPtr, line)
	End Method
	
	Rem
	bbdoc: Return the current column number.
	End Rem
	Method getColumnNumber:Int()
		Return htmlparser_get_column_number(parserPtr)
	End Method
	
	Rem
	bbdoc: Set the current column number.
	End Rem
	Method setColumnNumber(col:Int)
		htmlparser_set_column_number(parserPtr, col)
	End Method
	
	Rem
	bbdoc: Retrieve a human readable error message in case an error occurred.
	about: NULL is returned if the parser didn't encounter an error.
	End Rem
	Method getErrorMsg:String()
		Return String.FromUTF8String(htmlparser_get_error_msg(parserPtr))
	End Method
	
	Rem
	bbdoc: Invoked by the caller when text is expanded by the caller.
	about: Should be invoked when a template directive that expands to content is
	executed but we don't provide this content to the parser itself. This changes
	the current state by following the default rule, ensuring we stay in sync
	with the template.
	<p>
	Returns 1 if template directives are accepted for this state and 0 if they
	are not, which should result in an error condition.
	</p>
	<p>
	Right now the only case being handled are unquoted attribute values and it
	always returns 1. When insert_text() is called after the equals sign, we
	assume some text was consumed and we are now in the middle of the attribute
	value itself. Example:
	<pre>
	&lt;a href=$HREF_VALUE alt=alternate_text&gt;
	</pre>
	The template calls insert_text() when it encounters $HREF_VALUE. If it didn't
	the parser would only have seen the following html:
 	<pre>
	&lt;a href= alt=alternate_text&gt;
	</pre>
 	and would interpret alt=alternate_text as the value of the href attribute.
	</p>
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
