' Copyright (c) 2007-2009 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Rem
bbdoc: Expat XML Parser
End Rem
Module BaH.Expat

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007-2009 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Implemented API for BlitzMax."
ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes Expat 2.01 source."

ModuleInfo "CC_OPTS: -DHAVE_EXPAT_CONFIG_H"
?ppc
ModuleInfo "CC_OPTS: -DWORDS_BIGENDIAN"
?win32
ModuleInfo "CC_OPTS: -DXML_STATIC"
?

' expat_config.h
' added ifdef WORDS_BIGENDIAN test to set BYTEORDER definition

Import "common.bmx"

Rem
bbdoc: An XML document parser.
about: A parser if good for parsing only one XML document or external parsed entity.
If your application needs to parse many XML documents, then it needs to create a parser
object for each one.
End Rem
Type TXMLParser

	Field parserPtr:Byte Ptr
	
	Field userData:Object
	
	' callbacks
	Field userStartElementHandler(userData:Object, name:String, attribs:String[])
	Field userEndElementHandler(userData:Object, name:String)
	Field userCharacterDataHandler(userData:Object, text:String)
	Field userProcessingInstructionHandler(userData:Object, target:String, data:String)
	Field userCommentHandler(userData:Object, data:String)
	Field userStartCdataSectionHandler(userData:Object)
	Field userEndCdataSectionHandler(userData:Object)
	Field userDefaultHandler(userData:Object, data:String)
	Field userDefaultHandlerExpand(userData:Object, text:String)
	Field userSkippedEntityHandler(userData:Object, name:String, isParameterEntity:Int)
	Field userStartNamespaceDeclHandler(userData:Object, prefix:String, uri:String)
	Field userEndNamespaceDeclHandler(userData:Object, prefix:String)
	Field userXmlDeclHandler(userData:Object, version:String, encoding:String, standalone:Int)
	Field userStartDoctypeDeclHandler(userData:Object, docTypeName:String, sysid:String, pubid:String, hasInternalSubset:Int)
	Field userEndDoctypeDeclHandler(userData:Object)
	Field userAttlistDeclHandler(userData:Object, elname:String, attname:String, attType:String, dflt:String, isRequired:Int)
	Field userEntityDeclHandler(userData:Object, _entityName:String, isParameterEntity:Int, value:String, base:String, systemId:String, ..
		publicId:String, notationName:String)
	Field userNotationDeclHandler(userData:Object, notationName:String, base:String, systemId:String, publicId:String)
	Field userNotStandaloneHandler:Int(userData:Object)

	Rem
	bbdoc: Creates a new parser with the specified @encoding.
	End Rem
	Function CreateParser:TXMLParser(encoding:String = "UTF-8")
		Return New TXMLParser.Create(encoding)
	End Function
	
	Rem
	bbdoc: Creates a new parser with the specified @encoding.
	End Rem
	Method Create:TXMLParser(encoding:String = "UTF-8")
		parserPtr = bmx_expat_XML_ParserCreate(encoding)
		XML_SetUserData(parserPtr, Self)
		Return Self
	End Method


	Rem
	bbdoc: Parses some more of the document.
	about: The string @text is a string containing part (or perhaps all) of the document.
	The @isFinal parameter informs the parser that this is the last piece of the document.
	Frequently, the last piece is empty (i.e. len is zero.) If a parse error occurred, it
	returns XML_STATUS_ERROR. Otherwise it returns XML_STATUS_OK value. 
	End Rem
	Method Parse:Int(text:String, isFinal:Int = True)
		Return bmx_expat_XML_Parse(parserPtr, text, isFinal)
	End Method


	Rem
	bbdoc: Stops parsing, causing #Parse or #ParseBuffer to return.
	about: Must be called from within a call-back handler, except when aborting (when resumable is False)
	an already suspended parser. Some call-backs may still follow because they would otherwise get lost, including
	<ul>
	<li>the end element handler for empty elements when stopped in the start element handler,</li>
	<li>the end namespace declaration handler when stopped in the end element handler,</li>
	<li>the character data handler when stopped in the character data handler while making multiple
	call-backs on a contiguous chunk of characters,</li>
	</ul>
	and possibly others. 
	<p>
	This can be called from most handlers, including DTD related call-backs, except when parsing an external parameter
	entity and resumable is XML_TRUE. Returns XML_STATUS_OK when successful, XML_STATUS_ERROR otherwise. The
	possible error codes are:
	<dl>
	<dt><code>XML_ERROR_SUSPENDED</code></dt>
	<dd>when suspending an already suspended parser.</dd>
	<dt><code>XML_ERROR_FINISHED</code></dt>
	<dd>when the parser has already finished.</dd>
	<dt><code>XML_ERROR_SUSPEND_PE</code></dt>
	<dd>when suspending while parsing an external PE.</dd>
	</dl>
	</p>
	<p>
	Since the stop/resume feature requires application support in the outer parsing loop, it is an error to call this
	method for a parser not being handled appropriately; see Temporarily Stopping Parsing for more information.
	</p>
	<p>
	When resumable is XML_TRUE then parsing is suspended, that is, #Parse and #ParseBuffer return
	XML_STATUS_SUSPENDED. Otherwise, parsing is aborted, that is, #Parse and #ParseBuffer return
	XML_STATUS_ERROR with error code XML_ERROR_ABORTED.
	</p>
	<p>
	Note: This will be applied to the current parser instance only, that is, if there is a parent parser then
	it will continue parsing when the external entity reference handler returns. It is up to the implementation
	of that handler to call #StopParser on the parent parser (recursively), if one wants to stop parsing
	altogether.
	</p>
	<p>
	When suspended, parsing can be resumed by calling #ResumeParser.
	</p>
	End Rem
	Method StopParser:Int(resumable:Int)
		Return XML_StopParser(parserPtr, resumable)
	End Method
	
	Rem
	bbdoc: Resumes parsing after it has been suspended with #StopParser.
	about: Must not be called from within a handler call-back.
	Returns same status codes as #Parse or #ParseBuffer. An additional error code, XML_ERROR_NOT_SUSPENDED, will
	be returned if the parser was not currently suspended.
	<p>
	Note: This must be called on the most deeply nested child parser instance first, and on its parent parser only
	after the child parser has finished, to be applied recursively until the document entity's parser is restarted.
	That is, the parent parser will not resume by itself and it is up to the application to call #ResumeParser on
	it at the appropriate moment.
	</p>
	End Rem
	Method ResumeParser:Int()
		Return XML_ResumeParser(parserPtr)
	End Method

	Rem
	bbdoc: Returns status of parser with respect to being initialized, parsing, finished, or suspended, and whether the final buffer is being processed.
	about: Possible return values for @parsing are : XML_INITIALIZED, XML_PARSING, XML_FINISHED or XML_SUSPENDED.
	End Rem
	Method GetParsingStatus(parsing:Int Var, finalBuffer:Int Var)
		bmx_expat_XML_GetParsingStatus(parserPtr, Varptr parsing, Varptr finalBuffer)
	End Method
	
	Rem
	bbdoc: Sets handler for start (and empty) tags.
	about: Each attribute seen in a start (or empty) tag occupies 2 consecutive places in the array: the attribute name
	followed by the attribute value.
	<p>
	Note that an empty tag generates a call to both start and end handlers (in that order).
	</p>
	End Rem
	Method SetStartElementHandler(handler(userData:Object, name:String, attribs:String[]))
		userStartElementHandler = handler
		If handler Then
			bmx_expat_XML_SetStartElementHandler(parserPtr)
		Else
			XML_SetStartElementHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _StartElementHandler(parser:TXMLParser, name:String, attrs:String[])
		parser.userStartElementHandler(parser.userData, name, attrs)
	End Function
	
	Rem
	bbdoc: Sets handler for end (and empty) tags.
	about: As noted for #SetStartElementHandler, an empty tag generates a call to both start and end handlers.
	End Rem
	Method SetEndElementHandler(handler(userData:Object, name:String))
		userEndElementHandler = handler
		If handler Then
			bmx_expat_XML_SetEndElementHandler(parserPtr)
		Else
			XML_SetEndElementHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _EndElementHandler(parser:TXMLParser, name:String)
		parser.userEndElementHandler(parser.userData, name)
	End Function

	Rem
	bbdoc: Sets handlers for start and end tags with one call.
	End Rem
	Method SetElementHandler(starthandler(userData:Object, name:String, attribs:String[]), endhandler(userData:Object, name:String))
		userStartElementHandler = starthandler
		userEndElementHandler = endhandler
		If starthandler Or endhandler Then
			If starthandler Then
				If endhandler
					bmx_expat_XML_SetElementHandler(parserPtr, True, True)
				Else
					bmx_expat_XML_SetElementHandler(parserPtr, True, False)
				End If
			Else
				bmx_expat_XML_SetElementHandler(parserPtr, False, True)
			End If
		Else
			XML_SetElementHandler(parserPtr, Null, Null)
		End If
	End Method
	
	Rem
	bbdoc: Sets a text handler.
	about: A single block of contiguous text free of markup may still result in a sequence of calls to this handler.
	In other words, if you're searching for a pattern in the text, it may be split across calls to this handler.
	End Rem
	Method SetCharacterDataHandler(handler(userData:Object, text:String))
		userCharacterDataHandler = handler
		If handler Then
			bmx_expat_XML_SetCharacterDataHandler(parserPtr)
		Else
			XML_SetCharacterDataHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _CharacterDataHandler(parser:TXMLParser, text:String)
		If parser.userCharacterDataHandler Then
			parser.userCharacterDataHandler(parser.userData, text)
		End If
	End Function
	
	Rem
	bbdoc: Sets a handler for processing instructions.
	about: The target is the first word in the processing instruction. The data is the rest of the characters
	in it after skipping all whitespace after the initial word.
	End Rem
	Method SetProcessingInstructionHandler(handler(userData:Object, target:String, data:String))
		userProcessingInstructionHandler = handler
		If handler Then
			bmx_expat_XML_SetProcessingInstructionHandler(parserPtr)
		Else
			XML_SetProcessingInstructionHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _ProcessingInstructionHandler(parser:TXMLParser, target:String, data:String)
		parser.userProcessingInstructionHandler(parser.userData, target, data)
	End Function

	Rem
	bbdoc: Sets a handler for comments.
	about: The data is all text inside the comment delimiters.
	End Rem
	Method SetCommentHandler(handler(userData:Object, data:String))
		userCommentHandler = handler
		If handler Then
			bmx_expat_XML_SetCommentHandler(parserPtr)
		Else
			XML_SetCommentHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _CommentHandler(parser:TXMLParser, data:String)
		parser.userCommentHandler(parser.userData, data)
	End Function
	
	Rem
	bbdoc: Sets a handler that gets called at the beginning of a CDATA section.
	End Rem
	Method SetStartCdataSectionHandler(handler(userData:Object))
		userStartCdataSectionHandler = handler
		If handler Then
			bmx_expat_XML_SetStartCdataSectionHandler(parserPtr)
		Else
			XML_SetStartCdataSectionHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _StartCdataSectionHandler(parser:TXMLParser)
		parser.userStartCdataSectionHandler(parser.userData)
	End Function
	
	Rem
	bbdoc: Sets a handler that gets called at the end of a CDATA section.
	End Rem
	Method SetEndCdataSectionHandler(handler(userData:Object))
		userEndCdataSectionHandler = handler
		If handler Then
			bmx_expat_XML_SetEndCdataSectionHandler(parserPtr)
		Else
			XML_SetEndCdataSectionHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _EndCdataSectionHandler(parser:TXMLParser)
		parser.userEndCdataSectionHandler(parser.userData)
	End Function
	
	Rem
	bbdoc: Sets both CDATA section handlers with one call.
	End Rem
	Method SetCdataSectionHandler(starthandler(userData:Object), endhandler(userData:Object))
		userStartCdataSectionHandler = starthandler
		userEndCdataSectionHandler = endhandler
		If starthandler Or endhandler Then
			If starthandler Then
				If endhandler
					bmx_expat_XML_SetCdataSectionHandler(parserPtr, True, True)
				Else
					bmx_expat_XML_SetCdataSectionHandler(parserPtr, True, False)
				End If
			Else
				bmx_expat_XML_SetCdataSectionHandler(parserPtr, False, True)
			End If
		Else
			XML_SetCdataSectionHandler(parserPtr, Null, Null)
		End If
	End Method
	
	Rem
	bbdoc: Sets a handler for any characters in the document which wouldn't otherwise be handled.
	about: This includes both data for which no handlers can be set (like some kinds of DTD declarations) and data which
	could be reported but which currently has no handler set. The characters are passed exactly as they were present in the
	XML document except that they will be encoded in UTF-8. Line boundaries are not normalized. Note that a byte order mark
	character is not passed to the default handler. There are no guarantees about how characters are divided between calls
	to the default handler: for example, a comment might be split between multiple calls. Setting the handler with this call
	has the side effect of turning off expansion of references to internally defined general entities. Instead these references
	are passed to the default handler.
	End Rem
	Method SetDefaultHandler(handler(userData:Object, text:String))
		userDefaultHandler = handler
		If handler Then
			bmx_expat_XML_SetDefaultHandler(parserPtr)
		Else
			XML_SetDefaultHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _DefaultHandler(parser:TXMLParser, text:String)
		parser.userDefaultHandler(parser.userData, text)
	End Function
	
	Rem
	bbdoc: Sets a default handler, but doesn't inhibit the expansion of internal entity references.
	about: The entity reference will not be passed to the default handler.
	End Rem
	Method SetDefaultHandlerExpand(handler(userData:Object, text:String))
		userDefaultHandlerExpand = handler
		If handler Then
			bmx_expat_XML_SetDefaultHandlerExpand(parserPtr)
		Else
			XML_SetDefaultHandlerExpand(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _DefaultHandlerExpand(parser:TXMLParser, text:String)
		parser.userDefaultHandlerExpand(parser.userData, text)
	End Function
	
	Rem
	bbdoc: Sets a skipped entity handler.
	about: This is called in two situations:
	<ol>
	<li>An entity reference is encountered for which no declaration has been read and this is not an error.</li>
	<li>An internal entity reference is read, but not expanded, because XML_SetDefaultHandler has been called.</li>
	</ol>
	The @isParameterEntity argument will be non-zero for a parameter entity and zero for a general entity.
	<p>
	Note: skipped parameter entities in declarations and skipped general entities in attribute values cannot be reported,
	because the event would be out of sync with the reporting of the declarations or attribute values.
	</p>
	End Rem
	Method SetSkippedEntityHandler(handler(userData:Object, name:String, isParameterEntity:Int))
		userSkippedEntityHandler = handler
		If handler Then
			bmx_expat_XML_SetSkippedEntityHandler(parserPtr)
		Else
			XML_SetSkippedEntityHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _SkippedEntityHandler(parser:TXMLParser, name:String, isParameterEntity:Int)
		parser.userSkippedEntityHandler(parser.userData, name, isParameterEntity)
	End Function
	
	Rem
	bbdoc: Sets a handler to be called when a namespace is declared.
	about: Namespace declarations occur inside start tags. But the namespace declaration start handler is called before the
	start tag handler for each namespace declared in that start tag.
	End Rem
	Method SetStartNamespaceDeclHandler(handler(userData:Object, prefix:String, uri:String))
		userStartNamespaceDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetStartNamespaceDeclHandler(parserPtr)
		Else
			XML_SetStartNamespaceDeclHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _StartNamespaceDeclHandler(parser:TXMLParser, prefix:String, uri:String)
		parser.userStartNamespaceDeclHandler(parser.userData, prefix, uri)
	End Function
	
	Rem
	bbdoc: Sets a handler to be called when leaving the scope of a namespace declaration.
	about: This will be called, for each namespace declaration, after the handler for the end tag of the element in which
	the namespace was declared.
	End Rem
	Method SetEndNamespaceDeclHandler(handler(userData:Object, prefix:String))
		userEndNamespaceDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetEndNamespaceDeclHandler(parserPtr)
		Else
			XML_SetEndNamespaceDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _EndNamespaceDeclHandler(parser:TXMLParser, prefix:String)
		parser.userEndNamespaceDeclHandler(parser.userData, prefix)
	End Function
	
	Rem
	bbdoc: Sets both namespace declaration handlers with a single call.
	End Rem
	Method SetNamespaceDeclHandler(starthandler(userData:Object, prefix:String, uri:String), endhandler(userData:Object, prefix:String))
		userStartNamespaceDeclHandler = starthandler
		userEndNamespaceDeclHandler = endhandler
		If starthandler Or endhandler Then
			If starthandler Then
				If endhandler
					bmx_expat_XML_SetNamespaceDeclHandler(parserPtr, True, True)
				Else
					bmx_expat_XML_SetNamespaceDeclHandler(parserPtr, True, False)
				End If
			Else
				bmx_expat_XML_SetNamespaceDeclHandler(parserPtr, False, True)
			End If
		Else
			XML_SetNamespaceDeclHandler(parserPtr, Null, Null)
		End If
	End Method
	
	Rem
	bbdoc: Sets a handler that is called for XML declarations and also for text declarations discovered in external entities.
	about: The way to distinguish is that the version parameter will be NULL for text declarations. The encoding parameter
	may be NULL for an XML declaration. The standalone argument will contain -1, 0, or 1 indicating respectively that
	there was no standalone parameter in the declaration, that it was given as no, or that it was given as yes.
	End Rem
	Method SetXmlDeclHandler(handler(userData:Object, version:String, encoding:String, standalone:Int))
		userXmlDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetXmlDeclHandler(parserPtr)
		Else
			XML_SetXmlDeclHandler(parserPtr, Null)
		End If
	End Method

	' internal callback
	Function _XmlDeclHandler(parser:TXMLParser, version:String, encoding:String, standalone:Int)
		parser.userXmlDeclHandler(parser.userData, version, encoding, standalone)
	End Function
	
	Rem
	bbdoc: Sets a handler that is called at the start of a DOCTYPE declaration, before any external or internal subset is parsed.
	about: Both sysid and pubid may be NULL. The has_internal_subset will be non-zero if the DOCTYPE declaration has an
	internal subset.
	End Rem
	Method SetStartDoctypeDeclHandler(handler(userData:Object, docTypeName:String, sysid:String, pubid:String, hasInternalSubset:Int))
		userStartDoctypeDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetStartDoctypeDeclHandler(parserPtr)
		Else
			XML_SetStartDoctypeDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _StartDoctypeDeclHandler(parser:TXMLParser, docTypeName:String, sysid:String, pubid:String, hasInternalSubset:Int)
		parser.userStartDoctypeDeclHandler(parser.userData, docTypeName, sysid, pubid, hasInternalSubset)
	End Function
	
	Rem
	bbdoc: Sets a handler that is called at the end of a DOCTYPE declaration, after parsing any external subset.
	End Rem
	Method SetEndDoctypeDeclHandler(handler(userData:Object))
		userEndDoctypeDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetEndDoctypeDeclHandler(parserPtr)
		Else
			XML_SetEndDoctypeDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _EndDoctypeDeclHandler(parser:TXMLParser)
		parser.userEndDoctypeDeclHandler(parser.userData)
	End Function

	Rem
	bbdoc: Sets both doctype handlers with one call.
	End Rem
	Method SetDoctypeDeclHandler(starthandler(userData:Object, docTypeName:String, sysid:String, pubid:String, hasInternalSubset:Int), ..
			endhandler(userData:Object))
		userStartDoctypeDeclHandler = starthandler
		userEndDoctypeDeclHandler = endhandler
		If starthandler Or endhandler Then
			If starthandler Then
				If endhandler
					bmx_expat_XML_SetDoctypeDeclHandler(parserPtr, True, True)
				Else
					bmx_expat_XML_SetDoctypeDeclHandler(parserPtr, True, False)
				End If
			Else
				bmx_expat_XML_SetDoctypeDeclHandler(parserPtr, False, True)
			End If
		Else
			XML_SetDoctypeDeclHandler(parserPtr, Null, Null)
		End If
	End Method

	'Method SetElementDeclHandler(.....
	' TODO
	'End method
	
	Rem
	bbdoc: Sets a handler for attlist declarations in the DTD.
	about: This handler is called for each attribute. So a single attlist declaration with multiple attributes declared
	will generate multiple calls to this handler. The @elname parameter returns the name of the element for which the attribute
	is being declared. The attribute name is in the @attname parameter. The attribute type is in the @attType parameter. It
	is the string representing the type in the declaration with whitespace removed.
	<p>
	The @dflt parameter holds the default value. It will be NULL in the case of "#IMPLIED" or "#REQUIRED" attributes. You can
	distinguish these two cases by checking the @isRequired parameter, which will be true in the case of "#REQUIRED" attributes.
	Attributes which are "#FIXED" will have also have a true isrequired, but they will have the non-NULL fixed value
	in the @dflt parameter.
	</p>
	End Rem
	Method SetAttlistDeclHandler(handler(userData:Object, elname:String, attname:String, attType:String, ..
			dflt:String, isRequired:Int))
		userAttlistDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetAttlistDeclHandler(parserPtr)
		Else
			XML_SetAttlistDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _AttlistDeclHandler(parser:TXMLParser, elname:String, attname:String, attType:String, ..
			dflt:String, isRequired:Int)
		parser.userAttlistDeclHandler(parser.userData, elname, attname, attType, dflt, isRequired)
	End Function

	Rem
	bbdoc: Sets a handler that will be called for all entity declarations.
	about: The @isParameterEntity argument will be non-zero in the case of parameter entities and zero otherwise.
	<p>
	For internal entities (&ltg;!ENTITY foo "bar"&gt;), @value will be non-NULL and @systemId, @publicId, and @notationName
	will all be NULL.
	</p>
	<p>
	The @notationName argument will have a non-NULL value only for unparsed entity declarations.
	</p>
	End Rem
	Method SetEntityDeclHandler(handler(userData:Object, _entityName:String, isParameterEntity:Int, value:String, ..
			base:String, systemId:String, publicId:String, notationName:String))
		userEntityDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetEntityDeclHandler(parserPtr)
		Else
			XML_SetEntityDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _EntityDeclHandler(parser:TXMLParser, _entityName:String, isParameterEntity:Int, value:String, ..
			base:String, systemId:String, publicId:String, notationName:String)
		parser.userEntityDeclHandler(parser.userData, _entityName, isParameterEntity, value, base, systemId, publicId, notationName)
	End Function

	Rem
	bbdoc: Sets a handler that receives notation declarations.
	End Rem
	Method SetNotationDeclHandler(handler(userData:Object, notationName:String, base:String, systemId:String, ..
			publicId:String))
		userNotationDeclHandler = handler
		If handler Then
			bmx_expat_XML_SetNotationDeclHandler(parserPtr)
		Else
			XML_SetNotationDeclHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _NotationDeclHandler(parser:TXMLParser, notationName:String, base:String, systemId:String, publicId:String)
		parser.userNotationDeclHandler(parser.userData, notationName, base, systemId, publicId)
	End Function

	Rem
	bbdoc: Sets a handler that is called if the document is not "standalone".
	about: This happens when there is an external subset or a reference to a parameter entity, but does not have standalone
	set to "yes" in an XML declaration. If this handler returns XML_STATUS_ERROR, then the parser will throw an
	XML_ERROR_NOT_STANDALONE error.
	End Rem
	Method SetNotStandaloneHandler(handler:Int(userData:Object))
		userNotStandaloneHandler = handler
		If handler Then
			bmx_expat_XML_SetNotStandaloneHandler(parserPtr)
		Else
			XML_SetNotStandaloneHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _NotStandaloneHandler:Int(parser:TXMLParser)
		Return parser.userNotStandaloneHandler(parser.userData)
	End Function

	Rem
	bbdoc: Returns what type of error has occurred.
	End Rem
	Method GetErrorCode:Int()
		Return XML_GetErrorCode(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns the line number of the position.
	about: The first line is reported as 1.
	End Rem
	Method GetCurrentLineNumber:Int()
		Return XML_GetCurrentLineNumber(parserPtr)
	End Method
	
	Rem
	bbdoc: Sets the user data that gets passed to handlers.
	about: It overwrites any previous value. 
	End Rem
	Method SetUserData(data:Object)
		userData = data
	End Method
	
	Rem
	bbdoc: Returns the user data that gets passed to handlers.
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method
	
	Rem
	bbdoc: Sets the base to be used for resolving relative URIs in system identifiers.
	about: The return value is XML_STATUS_ERROR if there's no memory to store base, otherwise
	it's XML_STATUS_OK.
	End Rem
	Method SetBase:Int(base:String)
		Return bmx_expat_XML_SetBase(parserPtr, base)
	End Method
	
	Rem
	bbdoc: Returns the base for resolving relative URIs. 
	End Rem
	Method GetBase:String()
		Return bmx_expat_XML_GetBase(parserPtr)
	End Method
	
	Rem
	bbdoc: When attributes are reported to the start handler in the atts vector, attributes that were explicitly set in the element occur before any attributes that receive their value from default information in an ATTLIST declaration.
	about: This function returns the number of attributes that were explicitly set times two,
	thus giving the offset in the atts array passed to the start tag handler of the first attribute
	set due to defaults. It supplies information for the last call to a start handler. If called
	inside a start handler, then that means the current call. 
	End Rem
	Method GetSpecifiedAttributeCount:Int()
		Return XML_GetSpecifiedAttributeCount(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns the index of the ID attribute passed in the atts array in the last call to #StartElementHandler, or -1 if there is no ID attribute.
	about: If called inside a start handler, then that means the current call.
	End Rem
	Method GetIdAttributeIndex:Int()
		Return XML_GetIdAttributeIndex(parserPtr)
	End Method
	
	Rem
	bbdoc: Sets the encoding to be used by the parser.
	returns: XML_STATUS_OK on success or XML_STATUS_ERROR on error.
	about: It is equivalent to passing a non-null encoding argument to the parser creation functions. It must not be
	called after #Parse or #ParseBuffer have been called on the parser. 
	End Rem
	Method SetEncoding:Int(encoding:String)
		Return bmx_expat_XML_SetEncoding(parserPtr, encoding)
	End Method
	
	Rem
	bbdoc: This enables parsing of parameter entities, including the external parameter entity that is the external DTD subset, according to code.
	about: The choices for code are:
	<ul>
	<li>XML_PARAM_ENTITY_PARSING_NEVER</li>
	<li>XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE</li>
	<li>XML_PARAM_ENTITY_PARSING_ALWAYS</li>
	</ul>
	End Rem
	Method SetParamEntityParsing:Int(code:Int)
		Return XML_SetParamEntityParsing(parserPtr, code)
	End Method
	
	Rem
	bbdoc: Allows an application to provide an external subset for the document type declaration for documents which do not specify an external subset of their own.
	about: For documents which specify an external subset in their DOCTYPE declaration, the application-provided subset
	will be ignored. If the document does not contain a DOCTYPE declaration at all and useDTD is true, the
	application-provided subset will be parsed, but the StartDoctypeDeclHandler and EndDoctypeDeclHandler functions,
	if set, will not be called. The setting of parameter entity parsing, controlled using #SetParamEntityParsing, will be
	honored.
	<p>
	The application-provided external subset is read by calling the external entity reference handler set via
	#SetExternalEntityRefHandler with both publicId and systemId set to NULL.
	</p>
	<p>
	If this function is called after parsing has begun, it returns XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING and ignores
	useDTD. If called when Expat has been compiled without DTD support, it returns XML_ERROR_FEATURE_REQUIRES_XML_DTD.
	Otherwise, it returns XML_ERROR_NONE.
	</p>
	<p>
	Note: For the purpose of checking WFC: Entity Declared, passing useDTD as True will make the parser behave as
	if the document had a DTD with an external subset. This holds true even if the external entity reference handler
	returns without action.
	</p>
	End Rem
	Method UseForeignDTD:Int(useDTD:Int)
		Return XML_UseForeignDTD(parserPtr, useDTD)
	End Method
	
	Rem
	bbdoc: Sets whether or not prefixes are returned with names qualified with a namespace prefix. 
	about: This function only has an effect when using a parser created with XML_ParserCreateNS, i.e. when
	namespace processing is in effect. If it is called with doNST non-zero, then afterwards namespace qualified
	names (that is qualified with a prefix as opposed to belonging to a default namespace) are returned as a
	triplet with the three parts separated by the namespace separator specified when the parser was created.
	The order of returned parts is URI, local name, and prefix.
	<p>
	If doNST is zero, then namespaces are reported in the default manner, URI then local_name separated by the namespace separator.
	</p>
	End Rem
	Method SetReturnNSTriplet(doNST:Int)
		XML_SetReturnNSTriplet(parserPtr, doNST)
	End Method
	
	Rem
	bbdoc: This can be called within a handler for a start element, end element, processing instruction or character data.
	about: It causes the corresponding markup to be passed to the default handler set by
	#SetDefaultHandler or #SetDefaultHandlerExpand. It does nothing if there is not a default
	handler.
	End Rem
	Method DefaultCurrent()
		XML_DefaultCurrent(parserPtr)
	End Method
	
	Rem
	bbdoc: Frees memory used by the parser. 
	End Rem
	Method Free()
		If parserPtr Then
			XML_ParserFree(parserPtr)
			parserPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: Returns a string describing the error corresponding to code.
about: The code should be one of the values that can be returned from #GetErrorCode.
End Rem
Function XMLErrorString:String(code:Int)
	Return bmx_expat_XML_ErrorString(code)
End Function

Rem
bbdoc: Returns the library version as a string (e.g. "expat_1.95.1"). 
End Rem
Function XMLExpatVersion:String()
	Return bmx_expat_XML_ExpatVersion()
End Function

Rem
bbdoc: Returns the library version information as a set of values. 
End Rem
Function XMLExpatVersinInfo(major:Int Var, minor:Int Var, micro:Int Var)
	bmx_expat_XML_ExpatVersionInfo(Varptr major, Varptr minor, Varptr micro)
End Function


