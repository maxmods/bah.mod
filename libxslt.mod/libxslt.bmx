' Copyright (c) 2006-2009 Bruce A Henderson
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
bbdoc: Libxslt
End Rem
Module BaH.Libxslt

ModuleInfo "Version: 1.02"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Libxslt - 1998-2008 Daniel Veillard"
ModuleInfo "Copyright: Wrapper - 2006-2009 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Updated to Libxslt 1.1.25"
ModuleInfo "History: 1.01"
ModuleInfo "History: Updated to Libxslt 1.1.20"
ModuleInfo "History: 1.00 Initial Release (Libxslt 1.1.16)"


Import BaH.libxml

Import "../libxml.mod/src/*.h"

Import "src/*.h"

Import "src/attrvt.c"
Import "src/xslt.c"
Import "src/xsltutils.c"
Import "src/pattern.c"
Import "src/templates.c"
Import "src/variables.c"
Import "src/keys.c"
Import "src/numbers.c"
Import "src/extensions.c"
Import "src/extra.c"
Import "src/functions.c"
Import "src/namespaces.c"
Import "src/imports.c"
Import "src/attributes.c"
Import "src/documents.c"
Import "src/preproc.c"
Import "src/transform.c"
Import "src/security.c"
Import "src/xsltlocale.c"

' Build notes
'
' xsltconfig.h : locale ifdef tweak for multi-platform build.
'
'

Extern
	Function xsltParseStylesheetFile:Byte Ptr(uri:Byte Ptr)
	Function xsltParseStylesheetDoc:Byte Ptr(docPtr:Byte Ptr)
	Function xsltFreeStylesheet(sheet:Byte Ptr)
	Function xsltNewStylesheet:Byte Ptr()
	Function xsltParseStylesheetOutput(sheet:Byte Ptr, node:Byte Ptr)
	Function xsltParseStylesheetProcess:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr)
	Function xsltParseTemplateContent(sheet:Byte Ptr, templ:Byte Ptr)
	Function xsltParseStylesheetImport:Int(sheet:Byte Ptr, node:Byte Ptr)
	Function xsltParseStylesheetInclude:Int(sheet:Byte Ptr, node:Byte Ptr)
	
	Function xsltAddKey:Int(sheet:Byte Ptr, name:Byte Ptr, nameURI:Byte Ptr, match:Byte Ptr, use:Byte Ptr, inst:Byte Ptr)
	Function xsltFreeKeys(sheet:Byte Ptr)
	
	Function xsltParseStylesheetAttributeSet(sheet:Byte Ptr, node:Byte Ptr)
	Function xsltResolveStylesheetAttributeSet(sheet:Byte Ptr)
	Function xsltFreeAttributeSetsHashes(sheet:Byte Ptr)
	
	Function xsltNewTransformContext:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr)
	Function xsltApplyStripSpaces(ctxt:Byte Ptr, node:Byte Ptr)
	Function xsltApplyStylesheet:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr, params:Byte Ptr Ptr)
	Function xsltFindDocument:Byte Ptr(ctxt:Byte Ptr, doc:Byte Ptr)
	Function xsltFreeDocuments(ctxt:Byte Ptr)
	Function xsltFreeStyleDocuments(style:Byte Ptr)
	Function xsltNextImport:Byte Ptr(style:Byte Ptr)
	
	Function xsltNewStyleDocument:Byte Ptr(style:Byte Ptr, doc:Byte Ptr)
	Function xsltNewDocument:Byte Ptr(ctxt:Byte Ptr, doc:Byte Ptr)
	Function xsltLoadStyleDocument:Byte Ptr(style:Byte Ptr, uri:Byte Ptr)
	Function xsltLoadDocument:Byte Ptr(context:Byte Ptr, uri:Byte Ptr)
	Function xsltFindElemSpaceHandling:Int(context:Byte Ptr, node:Byte Ptr)
	Function xsltNeedElemSpaceHandling:Int(context:Byte Ptr)
	Function xsltFreeDocumentKeys(doc:Byte Ptr)
	Function xsltInitCtxtKeys(context:Byte Ptr, doc:Byte Ptr)
	Function xsltGetKey:Byte Ptr(context:Byte Ptr, name:Byte Ptr, nameURI:Byte Ptr, value:Byte Ptr)
	
	Function xsltFindTemplate:Byte Ptr(context:Byte Ptr, name:Byte Ptr, nameURI:Byte Ptr)
	
	Function xsltNamespaceAlias(sheet:Byte Ptr, node:Byte Ptr)
	Function xsltFreeNamespaceAliasHashes(sheet:Byte Ptr)
	Function xsltGetNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, ns:Byte Ptr, out:Byte Ptr)
	Function xsltGetPlainNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, ns:Byte Ptr, out:Byte Ptr)
	Function xsltGetSpecialNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, uri:Byte Ptr, prefix:Byte Ptr, out:Byte Ptr)
	
	Function xsltDocumentComp:Byte Ptr(style:Byte Ptr, inst:Byte Ptr, func:Byte Ptr)
	Function xsltFreeStylePreComps(sheet:Byte Ptr)
	Function xsltStylePreCompute(sheet:Byte Ptr, node:Byte Ptr)
	
	Function xsltAllocateExtra:Int(sheet:Byte Ptr)
	Function xsltAllocateExtraCtxt:Int(context:Byte Ptr)
	Function xsltCompileAttr(sheet:Byte Ptr, attr:Byte Ptr)
	Function xsltCreateRVT:Byte Ptr(context:Byte Ptr)
	Function xsltEvalAVT:Byte Ptr(context:Byte Ptr, avt:Byte Ptr, node:Byte Ptr)
	'Function xsltElemPreCompDeallocator(precomp:Byte Ptr)
	Function xsltFreeRVTs(context:Byte Ptr)
	Function xsltRegisterPersistRVT:Int(context:Byte Ptr, rvt:Byte Ptr)
	Function xsltRegisterTmpRVT:Int(context:Byte Ptr, rvt:Byte Ptr)
	
	Function xsltFreeCompMatchList(comp:Byte Ptr)
	Function xsltFreeTemplateHashes(sheet:Byte Ptr)
	Function xsltAddTemplate:Int(sheet:Byte Ptr, cur:Byte Ptr, Mode:Byte Ptr, modeURI:Byte Ptr)
	Function xsltCleanupTemplates(sheet:Byte Ptr)
	Function xsltCompilePattern:Byte Ptr(pattern:Byte Ptr, doc:Byte Ptr, node:Byte Ptr, sheet:Byte Ptr, context:Byte Ptr)
	Function xsltGetTemplate:Byte Ptr(context:Byte Ptr, node:Byte Ptr, style:Byte Ptr)
	Function xsltTestCompMatchList:Int(context:Byte Ptr, node:Byte Ptr, comp:Byte Ptr)
	'Function xsltMatchPattern:Int(context:Byte Ptr, node:Byte Ptr, pattern:Byte Ptr, ctxtdoc:Byte Ptr, ctxtnode:Byte Ptr)
	
	Function xsltMessage(context:Byte Ptr, node:Byte Ptr, inst:Byte Ptr)
	Function xsltPrintErrorContext(context:Byte Ptr, style:Byte Ptr, node:Byte Ptr)
	Function xsltSaveResultToFilename:Int(url:Byte Ptr, result:Byte Ptr, style:Byte Ptr, compression:Int)
	
	Function xsltAttrListTemplateProcess:Byte Ptr(context:Byte Ptr, target:Byte Ptr, cur:Byte Ptr)
	Function xsltAttrTemplateProcess:Byte Ptr(context:Byte Ptr, target:Byte Ptr, cur:Byte Ptr)
	Function xsltAttrTemplateValueProcess:Byte Ptr(context:Byte Ptr, value:Byte Ptr)
	Function xsltAttrTemplateValueProcessNode:Byte Ptr(context:Byte Ptr, value:Byte Ptr, node:Byte Ptr)
	Function xsltEvalAttrValueTemplate:Byte Ptr(context:Byte Ptr, node:Byte Ptr, name:Byte Ptr, namespace:Byte Ptr)
	Function xsltEvalStaticAttrValueTemplate:Byte Ptr(context:Byte Ptr, node:Byte Ptr, name:Byte Ptr, namespace:Byte Ptr, found:Int Ptr)
	Function xsltEvalTemplateString:Byte Ptr(context:Byte Ptr, node:Byte Ptr, parent:Byte Ptr)
	Function xsltTemplateProcess:Byte Ptr(context:Byte Ptr, node:Byte Ptr)
	Function xsltEvalXPathString:Byte Ptr(context:Byte Ptr, comp:Byte Ptr)
	
	Function _xsltCleanupGlobals() = "xsltCleanupGlobals"
End Extern

Rem
bbdoc: The set of options to pass to an xmlReadxxx when loading files for XSLT consumption.
End Rem
Const XSLT_PARSE_OPTIONS:Int = XML_PARSE_NOENT | XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_NOCDATA

Rem
bbdoc: The XSLT "vendor" string for this processor.
End Rem
Const XSLT_DEFAULT_VENDOR:String = "libxslt"

Rem
bbdoc: The XSLT "vendor" URL for this processor.
End Rem
Const XSLT_DEFAULT_URL:String = "http://xmlsoft.org/XSLT/"

Rem
bbdoc: The XSLT specification namespace.
End Rem
Const XSLT_NAMESPACE:String = "http://www.w3.org/1999/XSL/Transform"

' internal error message
Const XSLT_ERROR_PARAM:String = "Mandatory parameter is Null"

' xsltTransformState
Const XSLT_STATE_OK:Int = 0
Const XSLT_STATE_ERROR:Int = 1
Const XSLT_STATE_STOPPED:Int = 2

' xsltOutputType
Const XSLT_OUTPUT_XML:Int = 0
Const XSLT_OUTPUT_HTML:Int = 1
Const XSLT_OUTPUT_TEXT:Int = 2

' xsltStyleType
Const XSLT_FUNC_COPY:Int = 1
Const XSLT_FUNC_SORT:Int = 2
Const XSLT_FUNC_TEXT:Int = 3
Const XSLT_FUNC_ELEMENT:Int = 4
Const XSLT_FUNC_ATTRIBUTE:Int = 5
Const XSLT_FUNC_COMMENT:Int = 6
Const XSLT_FUNC_PI:Int = 7
Const XSLT_FUNC_COPYOF:Int = 8
Const XSLT_FUNC_VALUEOF:Int = 9
Const XSLT_FUNC_NUMBER:Int = 10
Const XSLT_FUNC_APPLYIMPORTS:Int = 11
Const XSLT_FUNC_CALLTEMPLATE:Int = 12
Const XSLT_FUNC_APPLYTEMPLATES:Int = 13
Const XSLT_FUNC_CHOOSE:Int = 14
Const XSLT_FUNC_IF:Int = 15
Const XSLT_FUNC_FOREACH:Int = 16
Const XSLT_FUNC_DOCUMENT:Int = 17
Const XSLT_FUNC_WITHPARAM:Int = 18
Const XSLT_FUNC_PARAM:Int = 19
Const XSLT_FUNC_VARIABLE:Int = 20
Const XSLT_FUNC_WHEN:Int = 21
Const XSLT_FUNC_EXTENSION:Int = 22


Rem
bbdoc: Unregister all global variables set up by the XSLT library.
End Rem
Function xsltCleanupGlobals()
	_xsltCleanupGlobals()
End Function

Rem
bbdoc: An XSLT Stylesheet
End Rem
Type TxsltStylesheet

	Const _parent:Int = 0			' parent (byte ptr)
	Const _next:Int = 4				' next (byte ptr)
	Const _imports:Int = 8			' imports (byte ptr)
	Const _docList:Int = 12			' General data on the style sheet document (byte ptr)
	Const _doc	:Int = 16				' the parsed XML stylesheet (byte ptr)
	Const _stripSpaces:Int = 20		' the hash table of the strip-space and p (byte ptr)
	Const _stripAll:Int = 24			' strip-space = 1, preserve-space = -1 (int)
	Const _cdataSection:Int = 28		' Global variable or parameters. (byte ptr)
	Const _variables:Int = 32			' Template descriptions. (byte ptr)
	Const _templates:Int = 36			' the ordered list of templates (byte ptr)
	Const _templatesHash:Int = 40		' hash table or wherever compiled template (byte ptr)
	Const _rootMatch:Int = 44			' template based on /  (byte ptr)
	Const _keyMatch:Int = 48			' template based on key()  (byte ptr)
	Const _elemMatch:Int = 52			' template based on *  (byte ptr)
	Const _attrMatch:Int = 56			' template based on @*  (byte ptr)
	Const _parentMatch:Int = 60		' template based on ..  (byte ptr)
	Const _textMatch:Int = 64			' template based on text()  (byte ptr)
	Const _piMatch:Int = 68			' template based on processing-instruction  (byte ptr)
	Const _commentMatch:Int = 72		' Namespace aliases. (byte ptr)
	Const _nsAliases:Int = 76			' Attribute sets. (byte ptr)
	Const _attributeSets:Int = 80		' Namespaces. (byte ptr)
	Const _nsHash:Int = 84			' the set of namespaces in use (byte ptr)
	Const _nsDefs:Int = 88			' * Key definitions. * (byte ptr)
	Const _keys:Int = 92				' * Output related stuff. * (byte ptr)
	Const _method:Int = 96			' the output method (byte ptr)
	Const _methodURI:Int = 100		' associated namespace if any (byte ptr)
	Const _version:Int = 104			' version string (byte ptr)
	Const _encoding:Int = 108			' encoding string (byte ptr)
	Const _omitXmlDeclaration:Int = 112	' Number formatting.  (int)
	Const _decimalFormat:Int = 116		' (byte ptr)
	Const _standalone:Int = 120		' standalone = "yes" | "no"  (int)
	Const _doctypePublic:Int = 124		' doctype-public string (byte ptr)
	Const _doctypeSystem:Int = 128		' doctype-system string (byte ptr)
	Const _indent:Int = 132			' should output being indented (int)
	Const _mediaType:Int = 136		' * Precomputed blocks. * (byte ptr)
	Const _preComps:Int = 140			' list of precomputed blocks (byte ptr)
	Const _warnings:Int = 144			' number of warnings found at compilation (int)
	Const _errors:Int = 148			' number of errors found at compilation (int)
	Const _exclPrefix:Int = 152		' last excluded prefixes (byte ptr)
	Const _exclPrefixTab:Int = 156		' array of excluded prefixes (byte ptr array)
	Const _exclPrefixNr:Int = 160		' number of excluded prefixes in scope (int)
	Const _exclPrefixMax:Int = 164		' size of the array (int)
	Const _private:Int = 168			' * Extensions. * (byte ptr)
	Const _extInfos:Int = 172			' the extension data (byte ptr)
	Const _extrasNr:Int = 176			' For keeping track of nested includes * (int)
	Const _includes:Int = 180			' * dictionnary: shared between stylesheet (byte ptr)
	Const _dict:Int = 184			' * precompiled attribute value templates. (byte ptr)
	Const _attVTs:Int = 188			' * if namespace-alias has an alias for th (byte ptr)
	Const _defaultAlias:Int = 192		' bypass pre-processing (already done) (byte ptr)
	Const _nopreproc:Int = 196		' all document text strings were interna (int)
	Const _internalized:Int = 200		' Literal Result Element as Stylesheet c (int)
	Const _literal_result:Int = 204	' (int)

	' reference to the actual stylesheet
	Field _xsltStylesheetPtr:Byte Ptr

	Function _create:TxsltStylesheet(_xsltStylesheetPtr:Byte Ptr)
		If _xsltStylesheetPtr <> Null Then
			Local this:TxsltStylesheet = New TxsltStylesheet
			
			this._xsltStylesheetPtr = _xsltStylesheetPtr
			
			Return this
		Else
			Return Null
		End If
	End Function

	Rem
	bbdoc: Create a new XSLT Stylesheet.
	returns: The new TxsltStylesheet or Null in case of error.
	End Rem
	Function newStylesheet:TxsltStylesheet()
		Return TxsltStylesheet._create(xsltNewStylesheet())
	End Function
	
	Rem
	bbdoc: Load and parse an XSLT stylesheet.
	returns: A new XSLT stylesheet structure.
	about:Parameters:
	<ul>
	<li><b>filename</b> : the filename/URL to the stylesheet</li>
	</ul>
	End Rem
	Function parseStylesheetFile:TxsltStylesheet(filename:String)
		Local cStr:Byte Ptr = filename.toCString()
		Local _xsltStylesheetPtr:Byte Ptr = xsltParseStylesheetFile(cStr)
		MemFree cStr
		
		If _xsltStylesheetPtr = Null Then
			Return Null
		Else
			Return TxsltStylesheet._create(_xsltStylesheetPtr)
		End If
	End Function

	Rem
	bbdoc: Parse an XSLT stylesheet building the associated structures.
	returns: A new XSLT stylesheet structure.
	about: Parameters:
	<ul>
	<li><b>doc</b> : an TxmlDoc parsed XML</li>
	</ul>
	End Rem
	Function parseStylesheetDoc:TxsltStylesheet(doc:TxmlDoc)
		Assert doc, XSLT_ERROR_PARAM
		
		Return TxsltStylesheet._create(xsltParseStylesheetDoc(doc._xmlDocPtr))
	End Function
	
	Rem
	bbdoc: Parse an XSLT stylesheet output element and record information related to the stylesheet output.
	about: Parameters:
	<ul>
	<li><b>node</b> : the "output" element</li>
	</ul>
	End Rem
	Method parseStylesheetOutput(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		xsltParseStylesheetOutput(_xsltStylesheetPtr, node._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Parse the XSLT stylesheet adding the associated structures.
	returns: the value of the 'ret' parameter if everything went right, Null if something went amiss.
	about: Parameters:
	<ul>
	<li><b>doc</b> : an TxmlDoc parsed XML</li>
	</ul>
	End Rem
	Method parseStylesheetProcess:TxsltStylesheet(doc:TxmlDoc)
		Assert doc, XSLT_ERROR_PARAM
		
		Return TxsltStylesheet._create(xsltParseStylesheetProcess(_xsltStylesheetPtr, doc._xmlDocPtr))
	End Method
	
	Rem
	bbdoc: parse a template content-model Clean-up the template content from unwanted ignorable blank nodes and process xslt:text.
	about: Parameters:
	<ul>
	<li><b>templ</b> : the container node (can be a document for literal results)</li>
	</ul>
	End Rem
	Method parseTemplateContent(templ:TxmlNode)
		Assert templ, XSLT_ERROR_PARAM

		xsltParseTemplateContent(_xsltStylesheetPtr, templ._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Save the result @res obtained by applying the stylesheet to a @URL / filename.
	returns: The number of byte written or -1 in case of failure.
	about: Parameters:
	<ul>
	<li><b>url</b> : a filename or URL</li>
	<li><b>res</b> : the result TxmlDoc</li>
	<li><b>compression</b> : the compression factor (0 - 9 inclusive)</li>
	</ul>
	End Rem
	Method saveResultToFilename:Int(url:String, res:TxmlDoc, compression:Int = 0)
		Assert url, XSLT_ERROR_PARAM
		Assert res, XSLT_ERROR_PARAM
		compression = Min(Max(0, compression), 9)
	
		Local cStr:Byte Ptr = url.toCString()
		
		Local ret:Int = xsltSaveResultToFilename(cStr, res._xmlDocPtr, _xsltStylesheetPtr, compression)
		
		MemFree(cStr)
		
		Return ret
	End Method
	Rem
	bbdoc: Apply the stylesheet to the document.
	returns: The result document or Null in case of error.
	about: NOTE: This may lead to a non-wellformed output, XML wise !
	<p>Parameters:
	<ul>
	<li><b>doc</b> : a parsed XML document</li>
	</ul>
	</p>
	End Rem
	Method applyStylesheet:TxmlDoc(doc:TxmlDoc)
		Assert doc, XSLT_ERROR_PARAM
		
		Return TxmlDoc._create(xsltApplyStylesheet(_xsltStylesheetPtr, doc._xmlDocPtr, Null))
	End Method
	
	Rem
	bbdoc: Add a key definition to the stylesheet.
	returns: 0 in case of success, and -1 in case of failure.
	about: Parameters:
	<ul>
	<li><b>name</b> : the key name or Null</li>
	<li><b>nameURI</b> : the name URI or Null</li>
	<li><b>match</b> : the match value</li>
	<li><b>use</b> : the use value</li>
	<li><b>inst</b> : the key instruction</li>
	</ul>
	End Rem
	Method addKey:Int(name:String, nameURI:String, match:String, use:String, inst:TxmlNode)
		Assert inst, XSLT_ERROR_PARAM
		Assert match, XSLT_ERROR_PARAM
		Assert use, XSLT_ERROR_PARAM
	
		Local res:Int
		
		Local cStr1:Byte Ptr = match.toCString()
		Local cStr2:Byte Ptr = use.toCString()
		
		If name = Null Then
			If nameURI = Null Then
				res = xsltAddKey(_xsltStylesheetPtr, Null, Null, cStr1, cStr2, inst._xmlNodePtr)
			Else
				Local cStr3:Byte Ptr = nameURI.toCString()
				
				res = xsltAddKey(_xsltStylesheetPtr, Null, cStr3, cStr1, cStr2, inst._xmlNodePtr)
				
				MemFree(cStr3)
			End If
		Else
			Local cStr4:Byte Ptr = name.toCString()
			
			If nameURI = Null Then
				res = xsltAddKey(_xsltStylesheetPtr, cStr4, Null, cStr1, cStr2, inst._xmlNodePtr)
			
			Else
				Local cStr3:Byte Ptr = nameURI.toCString()
				
				res = xsltAddKey(_xsltStylesheetPtr, cStr4, cStr3, cStr1, cStr2, inst._xmlNodePtr)
				
				MemFree(cStr3)
			End If
			
			MemFree(cStr4)
		End If
		
		MemFree(cStr1)
		MemFree(cStr2)
		
		Return res
	End Method
	
	Rem
	bbdoc: Free up the memory used by XSLT keys in the stylesheet.
	End Rem
	Method freeKeys()
		xsltFreeKeys(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Resolve the references between attribute sets.
	End Rem
	Method resolveStylesheetAttributeSet()
		xsltResolveStylesheetAttributeSet(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Parse an XSLT stylesheet attribute-set element.
	about: Parameters:
	<ul>
	<li><b>node</b> : the "attribute-set" element</li>
	</ul>
	End Rem
	Method parseStylesheetAttributeSet(node:TxmlNode)
		If node <> Null Then
			xsltParseStylesheetAttributeSet(_xsltStylesheetPtr, node._xmlNodePtr)
		End If
	End Method
	
	Rem
	bbdoc: Parse an XSLT stylesheet import element.
	returns: 0 in case of success -1 in case of failure.
	about: Parameters:
	<ul>
	<li><b>node</b> : the import element</li>
	</ul>
	End Rem
	Method parseStylesheetImport:Int(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		Return xsltParseStylesheetImport(_xsltStylesheetPtr, node._xmlNodePtr)
	End Method

	Rem
	bbdoc: Parse an XSLT stylesheet include element.
	returns: 0 in case of success -1 in case of failure.
	about: Parameters:
	<ul>
	<li><b>node</b> : the include node</li>
	</ul>
	End Rem
	Method parseStylesheetInclude:Int(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		Return xsltParseStylesheetInclude(_xsltStylesheetPtr, node._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Find the next stylesheet in import precedence.
	returns: The next stylesheet or Null if it was the last one
	End Rem
	Method nextImport:TxsltStylesheet()
		Return TxsltStylesheet._create(xsltNextImport(_xsltStylesheetPtr))
	End Method
	
	Rem
	bbdoc: Read the stylesheet-prefix and result-prefix attributes, register them as well as the corresponding namespace.
	about: Parameters:
	<ul>
	<li><b>node</b> : the xsl:namespace-alias node</li>
	</ul>
	End Rem
	Method namespaceAlias(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		xsltNamespaceAlias(_xsltStylesheetPtr, node._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Pre process an XSLT-1.1 document element.
	returns: A precompiled data structure for the element.
	about: Parameters:
	<ul>
	<li><b>inst</b> : the instruction in the stylesheet</li>
	</ul>
	End Rem
	Method documentComp:TxsltElemPreComp(inst:TxmlNode)
		Assert inst, XSLT_ERROR_PARAM
	
		Return TxsltElemPreComp._create(xsltDocumentComp(_xsltStylesheetPtr, inst._xmlNodePtr, Null))
	End Method
	
	Rem
	bbdoc: Precompute an XSLT stylesheet element.
	about: Parameters:
	<ul>
	<li><b>inst</b> : the instruction in the stylesheet</li>
	</ul>
	End Rem
	Method stylePreCompute(inst:TxmlNode)
		Assert inst, XSLT_ERROR_PARAM
		
		xsltStylePreCompute(_xsltStylesheetPtr, inst._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Allocate an extra runtime information slot statically while compiling the stylesheet and return its number.
	returns: The number of the slot.
	End Rem
	Method allocateExtra:Int()
		Return xsltAllocateExtra(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Register the XSLT pattern associated to @cur.
	returns: -1 in case of error, 0 otherwise.
	about: Parameters:
	<ul>
	<li><b>cur</b> : an XSLT template</li>
	<li><b>mode</b> : the mode name or Null</li>
	<li><b>modeURI</b> : the mode URI or Null</li>
	</ul>
	End Rem
	Method addTemplate:Int(cur:TxsltTemplate, Mode:String, modeURI:String)
		Assert cur, XSLT_ERROR_PARAM

		Local res:Int = -1
						
		If Mode <> Null Then
			Local cStr1:Byte Ptr = Mode.toCString()
			
			If modeURI <> Null Then
				Local cStr2:Byte Ptr = modeURI.toCString()
				
				res = xsltAddTemplate(_xsltStylesheetPtr, cur._xsltTemplatePtr, cStr1, cStr2)
				
				MemFree(cStr2)
			Else
				res = xsltAddTemplate(_xsltStylesheetPtr, cur._xsltTemplatePtr, cStr1, Null)
			End If
			
			MemFree(cStr1)
		Else
			If modeURI <> Null Then
				Local cStr2:Byte Ptr = modeURI.toCString()
				
				res = xsltAddTemplate(_xsltStylesheetPtr, cur._xsltTemplatePtr, Null, cStr2)
				
				MemFree(cStr2)
			Else
				res = xsltAddTemplate(_xsltStylesheetPtr, cur._xsltTemplatePtr, Null, Null)
			End If
		End If
		
		Return res
	End Method
	
	Rem
	bbdoc: Cleanup the state of the templates used by the stylesheet and the ones it imports.
	End Rem 
	Method cleanupTemplates()
		xsltCleanupTemplates(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated by all precomputed blocks.
	End Rem
	Method freeStylePreComps()
		xsltFreeStylePreComps(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by namespaces aliases.
	End Rem
	Method freeNamespaceAliasHashes()
		xsltFreeNamespaceAliasHashes(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by attribute sets.
	End Rem
	Method freeAttributeSetsHashes()
		xsltFreeAttributeSetsHashes(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up all the space used by the loaded documents.
	End Rem
	Method freeStyleDocuments()
		xsltFreeStyleDocuments(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by @addTemplate / @getTemplate mechanism.
	End Rem
	Method freeTemplateHashes()
		xsltFreeTemplateHashes(_xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Precompile an attribute in a stylesheet.
	about: Basically it checks if it is an attrubute value template, and if yes establish
	some structures needed to process it at transformation time.
	<p>Parameters:
	<ul>
	<li><b>attr</b> : the attribute coming from the stylesheet</li>
	</ul>
	</p>
	End Rem
	Method compileAttr(attr:TxmlAttribute)
		Assert attr, XSLT_ERROR_PARAM
		
		xsltCompileAttr(_xsltStylesheetPtr, attr._xmlAttrPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated to the sheet.
	End Rem
	Method free()
		xsltFreeStylesheet(_xsltStylesheetPtr)
	End Method
End Type

Rem
bbdoc: An XSLT transformation context
End Rem
Type TxsltTransformContext

	Const _style:Int = 0			' the stylesheet used (byte ptr)
	Const _type:Int = 4			' the type of output (int)
	Const _templ:Int = 8			' the current template (byte ptr)
	Const _templNr:Int = 12			' Nb of templates in the stack (int)
	Const _templMax:Int = 16		' Size of the templtes stack (int)
	Const _templTab:Int = 20		' the template stack (byte ptr)
	Const _vars:Int = 24			' the current variable list (byte ptr)
	Const _varsNr:Int = 28			' Nb of variable list in the stack (int)
	Const _varsMax:Int = 32			' Size of the variable list stack (int)
	Const _varsTab:Int = 36			' the variable list stack (byte ptr)
	Const _varsBase:Int = 40		' * Extensions * (int)
	Const _extFunctions:Int = 44		' the extension functions (byte ptr)
	Const _extElements:Int = 48		' the extension elements (byte ptr)
	Const _extInfos:Int = 52		' the extension data (byte ptr)
	Const _mode:Int = 56			' the current mode (byte ptr)
	Const _modeURI:Int = 60			' the current mode URI (byte ptr)
	Const _docList:Int = 64			' the document list (byte ptr)
	Const _document:Int = 68		' the current document (byte ptr)
	Const _node:Int = 72			' the current node being processed (byte ptr)
	Const _nodeList:Int = 76		' the current node list xmlNodePtr curren (byte ptr)
	Const _output:Int = 80			' the resulting document (byte ptr)
	Const _insert:Int = 84			' the insertion node (byte ptr)
	Const _xpathCtxt:Int = 88		' the XPath context (byte ptr)
	Const _state:Int = 92			' * Global variables * (byte ptr)
	Const _globalVars:Int = 96		' the global variables and params (byte ptr)
	Const _inst:Int = 100			' the instruction in the stylesheet (byte ptr)
	Const _xinclude:Int = 104		' should XInclude be processed (int)
	Const _outputFile:Int = 108		' the output URI if known (byte ptr)
	Const _profile:Int = 112			' is this run profiled (int)
	Const _prof:Int = 116			' the current profiled value (long)
	Const _profNr:Int = 124			' Nb of templates in the stack (int)
	Const _profMax:Int = 128			' Size of the templtaes stack (int)
	Const _profTab:Int = 132			' the profile template stack (long ptr)
	Const _private:Int = 140			' user defined data (byte ptr)
	Const _extrasNr:Int = 144		' the number of extras used (int)
	Const _extrasMax:Int = 148		' the number of extras allocated (int)
	Const _extras:Int = 152			' extra per runtime informations (byte ptr)
	Const _styleList:Int = 156		' the stylesheet docs list (byte ptr)
	Const _sec:Int = 160				' the security preferences if any (byte ptr)
	Const _errorfunc:Int = 164		' a specific error handler (byte ptr)
	Const _errctx:Int = 168			' context for the error handler (byte ptr)
	Const _sortfunc:Int = 172		' * handling of temporary Result Value Tre
	Const _tmpRVT:Int = 0			' list of RVT without persistance (byte ptr)
	Const _persistRVT:Int = 0		' list of persistant RVTs (byte ptr)
	Const _ctxtflags:Int = 0		' * Speed optimization when coalescing tex (int)
	Const _lasttext:Int = 0		' last text node content (byte ptr)
	Const _lasttsize:Int = 0		' last text node size (int)
	Const _lasttuse:Int = 0		' * Per Context Debugging * (int)
	Const _debugStatus:Int = 0		' the context level debug status (int)
	Const _traceCode:Int = 0		' pointer to the variable holding the mas (long)
	Const _parserOptions:Int = 0	' * dictionnary: shared between stylesheet (int)
	Const _dict:Int = 0			' * temporary storage for doc ptr, current (byte ptr)
	Const _tmpDoc:Int = 0			' * all document text strings are internal (byte ptr)
	Const _internalized:Int = 0		'
	

	Field _xsltTransformContextPtr:Byte Ptr

	Function _create:TxsltTransformContext(_xsltTransformContextPtr:Byte Ptr)
		If _xsltTransformContextPtr<> Null Then
			Local this:TxsltTransformContext= New TxsltTransformContext
			
			this._xsltTransformContextPtr= _xsltTransformContextPtr
			
			Return this
		Else
			Return Null
		End If
	End Function

	Rem
	bbdoc: Create a new XSLT TransformContext.
	returns: The new xsltTransformContext or Null in case of error.
	about: Parameters:
	<ul>
	<li><b>stylesheet</b> : a parsed XSLT stylesheet</li>
	<li><b>doc</b> : the input document</li>
	</ul>
	End Rem
	Function newTransformContext:TxsltTransformContext(stylesheet:TxsltStylesheet, doc:TxmlDoc)
		Assert stylesheet, XSLT_ERROR_PARAM
		Assert doc, XSLT_ERROR_PARAM
	
		Return TxsltTransformContext._create(xsltNewTransformContext(stylesheet._xsltStylesheetPtr, doc._xmlDocPtr))
	End Function

	Rem
	bbdoc: Strip the unwanted ignorable spaces from the input tree.
	about: Parameters:
	<ul>
	<li><b>node</b> : the root of the XML tree</li>
	</ul>
	end rem	
	Method applyStripSpaces(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		xsltApplyStripSpaces(_xsltTransformContextPtr, node._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Try to find a document within the XSLT transformation context.
	returns: The desired xsltDocument or Null in case of error.
	about: Parameters:
	<ul>
	<li><b>doc</b> : a parsed XML document</li>
	</ul>
	End Rem
	Method findDocument:TxsltDocument(doc:TxmlDoc)
		Assert doc, XSLT_ERROR_PARAM

		Return TxsltDocument._create(xsltFindDocument(_xsltTransformContextPtr, doc._xmlDocPtr))
	End Method
	
	Rem
	bbdoc: Free up all the space used by the loaded documents.
	End Rem
	Method freeDocuments()
		xsltFreeDocuments(_xsltTransformContextPtr)
	End Method
	
	Rem
	bbdoc: Find strip-space or preserve-space informations for an element respect the import precedence or the wildcards.
	returns: 1 if space should be stripped, 0 if not, and 2 if everything should be CDTATA wrapped.
	about: Parameters:
	<ul>
	<li><b>node</b> : an XML node</li>
	</ul>
	End Rem
	Method findElemSpaceHandling:Int(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		
		Return xsltFindElemSpaceHandling(_xsltTransformContextPtr, node._xmlNodePtr)
	End Method
	
	Rem
	bbdoc: Finds the named template, apply import precedence rule.
	returns: The TxsltTemplate or Null if not found.
	about: Parameters:
	<ul>
	<li><b>name</b> : the template name</li>
	<li><b>nameURI</b> : the template name URI</li>
	</ul>
	End Rem
	Method findTemplate:TxsltTemplate(name:String, nameURI:String)
		Assert name, XSLT_ERROR_PARAM
		Assert nameURI, XSLT_ERROR_PARAM
		
		Local cStr1:Byte Ptr = name.toCString()
		Local cStr2:Byte Ptr = nameURI.toCString()
		
		Local tmpl:TxsltTemplate = TxsltTemplate._create(xsltFindTemplate(_xsltTransformContextPtr, cStr1, cStr2))
		
		MemFree(cStr1)
		MemFree(cStr2)
		
		Return tmpl
	End Method
	
	Rem
	bbdoc: Checks whether that stylesheet requires white-space stripping.
	returns: 1 if space should be stripped, 0 if not
	End Rem
	Method needElemSpaceHandling:Int()
		Return xsltNeedElemSpaceHandling(_xsltTransformContextPtr)
	End Method
	
	Rem
	bbdoc: Computes all the keys tables for the current input document.
	about: Should be done before global varibales are initialized.
	<p>Parameters:
	<ul>
	<li><b>doc</b> : an XSLT document</li>
	</ul>
	</p>
	End Rem
	Method initKeys(doc:TxsltDocument)
		Assert doc, XSLT_ERROR_PARAM
		
		xsltInitCtxtKeys(_xsltTransformContextPtr, doc._xsltDocumentPtr)
	End Method
	
	Rem
	bbdoc: Find the right namespace value for this prefix.
	returns: The namespace node to use or Null.
	about: If needed create and add a new namespace decalaration on the node Handle namespace aliases.
	<p>Parameters:
	<ul>
	<li><b>curNode</b> : the input node</li>
	<li><b>namespace</b> : the namespace</li>
	<li><b>out</b> : the output node (or its parent)</li>
	</ul>
	</p>
	End Rem
	Method getNamespace:TxmlNs(curNode:TxmlNode, namespace:TxmlNs, out:TxmlNode)
		Assert curNode, XSLT_ERROR_PARAM
		Assert namespace, XSLT_ERROR_PARAM
		Assert out, XSLT_ERROR_PARAM

		Return TxmlNs._create(xsltGetNamespace(_xsltTransformContextPtr, curNode._xmlNodePtr, namespace._xmlNsPtr, out._xmlNodePtr))
	End Method
	
	Rem
	bbdoc: Find the right namespace value for this prefix.
	returns: The namespace node to use or Null.
	about: If needed create and add a new namespace decalaration on the node Handle namespace
	aliases and make sure the prefix is not NULL, this is needed for attributes.
	<p>Parameters:
	<ul>
	<li><b>curNode</b> : the input node</li>
	<li><b>namespace</b> : the namespace</li>
	<li><b>out</b> : the output node (or its parent)</li>
	</ul>
	</p>
	End Rem
	Method getPlainNamespace:TxmlNs(curNode:TxmlNode, namespace:TxmlNs, out:TxmlNode)
		Assert curNode, XSLT_ERROR_PARAM
		Assert namespace, XSLT_ERROR_PARAM
		Assert out, XSLT_ERROR_PARAM

		Return TxmlNs._create(xsltGetPlainNamespace(_xsltTransformContextPtr, curNode._xmlNodePtr, namespace._xmlNsPtr, out._xmlNodePtr))
	End Method
	
	Rem
	bbdoc: Find the right namespace value for this URI.
	returns: The namespace node to use or Null.
	about: If needed create and add a new namespace decalaration on the node.
	<p>Parameters:
	<ul>
	<li><b>curNode</b> : the input node</li>
	<li><b>uri</b> : the namespace URI</li>
	<li><b>prefic</b> : the suggested prefix</li>
	<li><b>out</b> : the output node (or its parent)</li>
	</ul>
	</p>
	End Rem
	Method getSpecialNamespace:TxmlNs(curNode:TxmlNode, uri:String, prefix:String, out:TxmlNode)
		Assert curNode, XSLT_ERROR_PARAM
		Assert uri, XSLT_ERROR_PARAM
		Assert prefix, XSLT_ERROR_PARAM
		Assert out, XSLT_ERROR_PARAM
	
		Local cStr1:Byte Ptr = uri.toCString()
		Local cStr2:Byte Ptr = prefix.toCString()
		Local ns:TxmlNs = TxmlNs._create(xsltGetSpecialNamespace(_xsltTransformContextPtr, curNode._xmlNodePtr, cStr1, cStr2, out._xmlNodePtr))
		
		MemFree(cStr1)
		MemFree(cStr2)
		
		Return ns
	End Method

	Rem
	bbdoc: Finds the template applying to this node.
	returns: The xslt Template or Null if not found.
	about: If @style is non-Null it means one needs to look for the next imported template in scope.
	<p>Parameters:
	<ul>
	<li><b>node</b> : the node being processed</li>
	<li><b>style</b> : the current style</li>
	</ul>
	</p>
	End Rem
	Method getTemplate:TxsltTemplate(node:TxmlNode, style:TxsltStylesheet)
		Assert node, XSLT_ERROR_PARAM
		
		Local tmpl:TxsltTemplate
		
		If style <> Null Then
			tmpl = TxsltTemplate._create(xsltGetTemplate(_xsltTransformContextPtr, node._xmlNodePtr, style._xsltStylesheetPtr))
		Else
			tmpl = TxsltTemplate._create(xsltGetTemplate(_xsltTransformContextPtr, node._xmlNodePtr, Null))
		End If

		Return tmpl		
	End Method
	
	Rem
	bbdoc: Undocumented...
	about: Parameters:
	<ul>
	<li><b>node</b> : </li>
	<li><b>pattern</b> : </li>
	<li><b>ctxtdoc</b> : </li>
	<li><b>ctxtnode</b> : </li>
	</ul>
	'End Rem
	Method matchPattern:Int(node:TxmlNode, pattern:String, ctxtdoc:TxmlDoc, ctxtnode:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		Assert pattern, XSLT_ERROR_PARAM
		Assert ctxtdoc, XSLT_ERROR_PARAM
		Assert ctxtnode, XSLT_ERROR_PARAM
		
		Local ret:Int = -1
		Local cStr1:Byte Ptr = pattern.toCString()
		
		ret = xsltMatchPattern(_xsltTransformContextPtr, node._xmlNodePtr, cStr1, ctxtdoc._xmlDocPtr, ctxtnode._xmlNodePtr)
		
		MemFree(cStr1)
		
		Return ret
	End Method
	End Rem
	
	Rem
	bbdoc: Lookup a key.
	returns: The nodeset resulting from the query or Null.
	about: Parameters:
	<ul>
	<li><b>name</b> : the key name or Null</li>
	<li><b>nameURI</b> : the name URI or Null</li>
	<li><b>value</b> : the key value to look for</li>
	</ul>
	End Rem
	Method getKey:TxmlNodeSet(name:String, nameURI:String, value:String)
		Assert value, XSLT_ERROR_PARAM
		
		Local nodeset:TxmlNodeSet
		
		Local cStr3:Byte Ptr = value.toCString()
		
		If name <> Null Then
		
			Local cStr1:Byte Ptr = name.toCString()
		
			If nameURI <> Null Then
				Local cStr2:Byte Ptr = nameURI.toCString()
				nodeset = TxmlNodeSet._create(xsltGetKey(_xsltTransformContextPtr, cStr1, cStr2, cStr3))
				MemFree(cStr2)
			Else
				nodeset = TxmlNodeSet._create(xsltGetKey(_xsltTransformContextPtr, cStr1, Null, cStr3))
			End If
			
			MemFree(cStr1)
		Else
			If nameURI <> Null Then
				Local cStr2:Byte Ptr = nameURI.toCString()
				nodeset = TxmlNodeSet._create(xsltGetKey(_xsltTransformContextPtr, Null, cStr2, cStr3))
				MemFree(cStr2)
			Else
				nodeset = TxmlNodeSet._create(xsltGetKey(_xsltTransformContextPtr, Null, Null, cStr3))
			End If
		End If
		
		MemFree(cStr3)
		
		Return nodeset
	End Method
	
	Rem
	bbdoc: Allocate an extra runtime information slot at run-time and return its number.
	returns: the number of the slot
	about: This makes sure there is a slot ready in the transformation context.
	End Rem
	Method allocateExtraCtxt:Int()
		Return xsltAllocateExtraCtxt(_xsltTransformContextPtr)
	End Method
	
	Rem
	bbdoc: Create a result value tree.
	returns: The result value tree or Null in case of error.
	End Rem
	Method createRVT:TxmlDoc()
		Return TxmlDoc._create(xsltCreateRVT(_xsltTransformContextPtr))
	End Method
	
	Rem
	bbdoc: Process the given AVT, and return the new string value.
	returns: The computed string value or Null.
	about: Parameters:
	<ul>
	<li><b>avt</b> : the precompiled attribute value template info</li>
	<li><b>node</b> : the node hosting the attribute</li>
	</ul>
	End Rem
	Method evalAVT:String(avt:String, node:TxmlNode)
		Assert avt, XSLT_ERROR_PARAM
		Assert node, XSLT_ERROR_PARAM
	
		Local ret:String = Null
		
		Local cStr1:Byte Ptr = avt.toCString()
		Local r:Byte Ptr = xsltEvalAVT(_xsltTransformContextPtr, cStr1, node._xmlNodePtr)
		
		MemFree(cStr1)
		If r <> Null Then
			ret = String.fromCString(r)
		End If
		MemFree(r)
		
		Return ret
	End Method
	
	Rem
	bbdoc: Register the result value tree for destruction at the end of the processing.
	returns: 0 in case of success and -1 in case of error.
	about: Parameters:
	<ul>
	<li><b>rvt</b> : a result value tree</li>
	</ul>
	End Rem
	Method registerPersistRVT:Int(rvt:TxmlDoc)
		Assert rvt, XSLT_ERROR_PARAM

		Return xsltRegisterPersistRVT(_xsltTransformContextPtr, rvt._xmlDocPtr)
	End Method
	
	Rem
	bbdoc: Register the result value tree for destruction at the end of the context.
	returns: 0 in case of success and -1 in case of error.
	about: Parameters:
	<ul>
	<li><b>rvt</b> : a result value tree</li>
	</ul>
	End Rem
	Method registerTmpRVT:Int(rvt:TxmlDoc)
		Assert rvt, XSLT_ERROR_PARAM
		
		Return xsltRegisterTmpRVT(_xsltTransformContextPtr, rvt._xmlDocPtr)
	End Method
	
	Rem
	bbdoc: Free all the registered result value tree of the transformation.
	End Rem
	Method freeRVTs()
		xsltFreeRVTs(_xsltTransformContextPtr)
	End Method
	
	Rem
	bbdoc: Do a copy of an attribute list with attribute template processing.
	returns: A new TxmlAttribute object, or Null in case of error.
	about: Parameters:
	<ul>
	<li><b>target</b> : the element where the attributes will be grafted</li>
	<li><b>cur</b> : the first attribute</li>
	</ul>
	End Rem
	Method attrListTemplateProcess:TxmlAttribute(target:TxmlNode, cur:TxmlAttribute)
		Assert target, XSLT_ERROR_PARAM
		Assert cur, XSLT_ERROR_PARAM
		
		Return TxmlAttribute._create(xsltAttrListTemplateProcess(_xsltTransformContextPtr, target._xmlNodePtr, cur._xmlAttrPtr))
	End Method

	Rem
	bbdoc: Process the given attribute and return the new processed copy.
	returns: The attribute replacement.
	about: Parameters:
	<ul>
	<li><b>target</b> : the result node</li>
	<li><b>cur</b> : the attribute template node</li>
	</ul>
	End Rem
	Method attrTemplateProcess:TxmlAttribute(target:TxmlNode, cur:TxmlAttribute)
		Assert target, XSLT_ERROR_PARAM
		Assert cur, XSLT_ERROR_PARAM
		
		Return TxmlAttribute._create(xsltAttrTemplateProcess(_xsltTransformContextPtr, target._xmlNodePtr, cur._xmlAttrPtr))
	End Method
	
	Rem
	bbdoc: Process the given node and return the new string value.
	returns: The computed string value or Null.
	about: Parameters:
	<ul>
	<li><b>value</b> : the attribute template node value</li>
	</ul>
	End Rem
	Method attrTemplateValueProcess:String(value:String)
		Assert value, XSLT_ERROR_PARAM
		Local ret:String = Null
	
		Local cStr:Byte Ptr = value.toCString()
		Local s:Byte Ptr = xsltAttrTemplateValueProcess(_xsltTransformContextPtr, cStr)
		
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		MemFree(cStr)
		
		Return ret
	End Method

	Rem
	bbdoc: Process the given string, allowing to pass a namespace mapping context and return the new string value.
	returns: The computed string value or Null.
	about: Parameters:
	<ul>
	<li><b>value</b> : the attribute template node value</li>
	<li><b>node</b> : the node hosting the attribute</li>
	</ul>
	End Rem
	Method attrTemplateValueProcessNode:String(value:String, node:TxmlNode)
		Assert value, XSLT_ERROR_PARAM
		Assert node, XSLT_ERROR_PARAM
		
		Local ret:String = Null
	
		Local cStr:Byte Ptr = value.toCString()
		Local s:Byte Ptr = xsltAttrTemplateValueProcessNode(_xsltTransformContextPtr, cStr, node._xmlNodePtr)
		
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		MemFree(cStr)
		
		Return ret
	End Method
	
	Rem
	bbdoc: Evaluate a attribute value template.
	returns: The computed string value or Null.
	about: i.e. the attribute value can contain expressions contained in curly braces ({}) and those
	are substituted by they computed value.
	<p>Parameters:
	<ul>
	<li><b>node</b> : the stylesheet node</li>
	<li><b>name</b> : the attribute QName</li>
	<li><b>namespace</b> : the attribute namespace URI</li>
	</ul>
	</p>
	End Rem
	Method evalAttrValueTemplate:String(node:TxmlNode, name:String, namespace:String)
		Assert node, XSLT_ERROR_PARAM
	
		Local ret:String = Null
		Local s:Byte Ptr
		
		If name <> Null Then
			Local cStr1:Byte Ptr = name.toCString()
			
			If namespace <> Null Then
				Local cStr2:Byte Ptr = namespace.toCString()
				
				s = xsltEvalAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, cStr1, cStr2)
				
				MemFree(cStr2)
			Else
				s = xsltEvalAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, cStr1, Null)
			
			End If
			
			MemFree(cStr1)

		Else

			If namespace <> Null Then
				Local cStr2:Byte Ptr = namespace.toCString()
				
				s = xsltEvalAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, Null, cStr2)

				MemFree(cStr2)
			Else
				s = xsltEvalAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, Null, Null)
			
			End If
		
		End If
		
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		
		Return ret
	End Method

	Rem
	bbdoc: Check if an attribute value template has a static value.
	returns: The static string value or Null.
	about: i.e. the attribute value does not contain expressions contained in curly braces ({})
	<p>Parameters:
	<ul>
	<li><b>node</b> : the stylesheet node</li>
	<li><b>name</b> : the attribute Name</li>
	<li><b>namespace</b> : the attribute namespace URI</li>
	</ul>
	</p>
	End Rem
	Method evalStaticAttrValueTemplate:String(node:TxmlNode, name:String, namespace:String)
		Assert node, XSLT_ERROR_PARAM
	
		Local ret:String = Null
		Local s:Byte Ptr
		Local found:Int
		
		If name <> Null Then
			Local cStr1:Byte Ptr = name.toCString()
			
			If namespace <> Null Then
				Local cStr2:Byte Ptr = namespace.toCString()
				
				s = xsltEvalStaticAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, cStr1, cStr2, Varptr found)
				
				MemFree(cStr2)
			Else
				s = xsltEvalStaticAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, cStr1, Null, Varptr found)
			
			End If
			
			MemFree(cStr1)

		Else

			If namespace <> Null Then
				Local cStr2:Byte Ptr = namespace.toCString()
				
				s = xsltEvalStaticAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, Null, cStr2, Varptr found)

				MemFree(cStr2)
			Else
				s = xsltEvalStaticAttrValueTemplate(_xsltTransformContextPtr, node._xmlNodePtr, Null, Null, Varptr found)
			
			End If
		
		End If
		
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		
		Return ret
	End Method

	Rem
	bbdoc: Evaluate a template string value.
	returns: The computed string value or Null.
	about: i.e. the parent list is interpreter as template content and the resulting tree string value is
	returned. This is needed for example by xsl:comment and xsl:processing-instruction.
	<p>Parameters:
	<ul>
	<li><b>node</b> : the stylesheet node</li>
	<li><b>parent</b> : the content parent</li>
	</ul>
	</p>
	End Rem
	Method evalTemplateString:String(node:TxmlNode, parent:TxmlNode)
		Assert node, XSLT_ERROR_PARAM
		Assert parent, XSLT_ERROR_PARAM
		
		Local ret:String = Null
	
		Local s:Byte Ptr = xsltEvalTemplateString(_xsltTransformContextPtr, node._xmlNodePtr, parent._xmlNodePtr)
		
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		
		Return ret
	End Method

	Rem
	bbdoc: Process the expression using XPath and get a string.
	returns: The computed string value or Null.
	about: Parameters:
	<ul>
	<li><b>comp</b> : the compiled XPath expression</li>
	</ul>
	End Rem
	Method evalXPathString:String(comp:TxmlXPathCompExpr)
		Assert comp, XSLT_ERROR_PARAM
		
		Local ret:String = Null
	
		Local s:Byte Ptr = xsltEvalXPathString(_xsltTransformContextPtr, comp._xmlXPathCompExprPtr)
		If s <> Null Then
			ret = String.fromCString(s)
			MemFree(s)
		End If
		
		Return ret
	End Method
	
	Rem
	bbdoc: Process the given node and return the new string value.
	returns: The computed tree replacement.
	<p>Parameters:
	<ul>
	<li><b>node</b> : the attribute template node</li>
	</ul>
	</p>
	End Rem
	Method templateProcess:TxmlNode(node:TxmlNode)
		Assert node, XSLT_ERROR_PARAM

		Return TxmlNode._create(xsltTemplateProcess(_xsltTransformContextPtr, node._xmlNodePtr))
	End Method

End Type

Rem
bbdoc: An XSLT Document
End Rem
Type TxsltDocument

	Field _xsltDocumentPtr:Byte Ptr
	
	Function _create:TxsltDocument(_xsltDocumentPtr:Byte Ptr)
		If _xsltDocumentPtr<> Null Then
			Local this:TxsltDocument= New TxsltDocument
			
			this._xsltDocumentPtr= _xsltDocumentPtr
			
			Return this
		Else
			Return Null
		End If
	End Function

	Rem
	bbdoc: Register a new document, apply key computations.
	returns: The new TxsltDocument.
	about: Parameters:
	<ul>
	<li><b>context</b> : an XSLT transformation context (or Null)</li>
	<li><b>doc</b> : a parsed XML document</li>
	</ul>
	End Rem
	Function newDocument:TxsltDocument(context:TxsltTransformContext, doc:TxmlDoc)
		Assert doc, XSLT_ERROR_PARAM
		
		If context <> Null Then
			Return TxsltDocument._create(xsltNewDocument(context._xsltTransformContextPtr, doc._xmlDocPtr))
		Else
			Return TxsltDocument._create(xsltNewDocument(Null, doc._xmlDocPtr))
		End If
	End Function
	
	Rem
	bbdoc: Register a new document, apply key computations.
	returns: The new TxsltDocument.
	about: Parameters:
	<ul>
	<li><b>style</b> : an XSLT style sheet</li>
	<li><b>doc</b> : a parsed XML document</li>
	</ul>
	End Rem
	Function newStyleDocument:TxsltDocument(style:TxsltStylesheet, doc:TxmlDoc)
		Assert style, XSLT_ERROR_PARAM
		Assert doc, XSLT_ERROR_PARAM
		
		Return TxsltDocument._create(xsltNewStyleDocument(style._xsltStylesheetPtr, doc._xmlDocPtr))
	End Function
	
	Rem
	bbdoc: Try to load a stylesheet document within the XSLT transformation context.
	returns: The new TxsltDocument or Null in case of error.
	about: Parameters:
	<ul>
	<li><b>style</b> : an XSLT style sheet</li>
	<li><b>uri</b> : the computed URI of the document</li>
	</ul>
	End Rem
	Function loadStyleDocument:TxsltDocument(style:TxsltStylesheet , uri:String)
		Assert style, XSLT_ERROR_PARAM
		Assert uri, XSLT_ERROR_PARAM
		
		Local cStr:Byte Ptr = uri.toCString()
		Local doc:TxsltDocument = TxsltDocument._create(xsltLoadStyleDocument(style._xsltStylesheetPtr, cStr))
		MemFree(cStr)
		
		Return doc
	End Function
	
	Rem
	bbdoc: Try to load a document (not a stylesheet) within the XSLT transformation context.
	returns: The new TxsltDocument or Null in case of error
	about: Parameters:
	<ul>
	<li><b>context</b> : an XSLT transformation context</li>
	<li><b>uri</b> : the computed URI of the document</li>
	</ul>
	End Rem
	Function loadDocument:TxsltDocument(context:TxsltTransformContext , uri:String)
		Assert context, XSLT_ERROR_PARAM
		Assert uri, XSLT_ERROR_PARAM

		Local cStr:Byte Ptr = uri.toCString()
		Local doc:TxsltDocument = TxsltDocument._create(xsltLoadDocument(context._xsltTransformContextPtr, cStr))
		MemFree(cStr)
		
		Return doc
	End Function

	Rem
	bbdoc: Free the keys associated to a document.
	End Rem
	Method freeDocumentKeys()
		xsltFreeDocumentKeys(_xsltDocumentPtr)
	End Method
	
End Type

Rem
bbdoc: An XSLT ElemPreComp
End Rem
Type TxsltElemPreComp

	Const _next:Int = 0	' chained list (byte ptr)
	Const _type:Int = 4	' type of the element (int)
	Const _func:Int = 8	' handling function (func?)
	Const _inst:Int = 12	' the instruction end of common part (byte ptr)
	Const _free:Int = 16	' the deallocator (byte ptr)

	Field _xsltElemPreCompPtr:Byte Ptr
	
	Function _create:TxsltElemPreComp(_xsltElemPreCompPtr:Byte Ptr)
		If _xsltElemPreCompPtr<> Null Then
			Local this:TxsltElemPreComp= New TxsltElemPreComp
			
			this._xsltElemPreCompPtr= _xsltElemPreCompPtr
			
			Return this
		Else
			Return Null
		End If
	End Function
	
	Rem
	bbdoc: Returns the type of this elemement.
	about: The following lists possible types:<br>
	<table>
	<tr><th>Constant</th></tr>
	<tr><td>XSLT_FUNC_COPY</td></tr>
	<tr><td>XSLT_FUNC_SORT</td></tr>
	<tr><td>XSLT_FUNC_TEXT</td></tr>
	<tr><td>XSLT_FUNC_ELEMENT</td></tr>
	<tr><td>XSLT_FUNC_ATTRIBUTE</td></tr>
	<tr><td>XSLT_FUNC_COMMENT</td></tr>
	<tr><td>XSLT_FUNC_PI</td></tr>
	<tr><td>XSLT_FUNC_COPYOF</td></tr>
	<tr><td>XSLT_FUNC_VALUEOF</td></tr>
	<tr><td>XSLT_FUNC_NUMBER</td></tr>
	<tr><td>XSLT_FUNC_APPLYIMPORTS</td></tr>
	<tr><td>XSLT_FUNC_CALLTEMPLATE</td></tr>
	<tr><td>XSLT_FUNC_APPLYTEMPLATES</td></tr>
	<tr><td>XSLT_FUNC_CHOOSE</td></tr>
	<tr><td>XSLT_FUNC_IF</td></tr>
	<tr><td>XSLT_FUNC_FOREACH</td></tr>
	<tr><td>XSLT_FUNC_DOCUMENT</td></tr>
	<tr><td>XSLT_FUNC_WITHPARAM</td></tr>
	<tr><td>XSLT_FUNC_PARAM</td></tr>
	<tr><td>XSLT_FUNC_VARIABLE</td></tr>
	<tr><td>XSLT_FUNC_WHEN</td></tr>
	<tr><td>XSLT_FUNC_EXTENSION</td></tr>
	</table>
	End Rem
	Method getType:Int()
		Return Int Ptr(_xsltElemPreCompPtr + _type)[0]
	End Method
	
	Rem
	bbdoc: Frees (deallocates) a TxsltElemPreComp structure.
	'End Rem
	Method free()
		xsltElemPreCompDeallocator(_xsltElemPreCompPtr)
	End Method
	End Rem
End Type

Rem
bbdoc: An XSLT Template
End Rem
Type TxsltTemplate

	Field _xsltTemplatePtr:Byte Ptr
	
	Function _create:TxsltTemplate(_xsltTemplatePtr:Byte Ptr)
		If _xsltTemplatePtr<> Null Then
			Local this:TxsltTemplate= New TxsltTemplate
			
			this._xsltTemplatePtr= _xsltTemplatePtr
			
			Return this
		Else
			Return Null
		End If
	End Function
	
End Type

Rem
bbdoc: An XSLT CompMatch
End Rem
Type TxsltCompMatch

	Field _xsltCompMatchPtr:Byte Ptr
	
	Function _create:TxsltCompMatch(_xsltCompMatchPtr:Byte Ptr)
		If _xsltCompMatchPtr<> Null Then
			Local this:TxsltCompMatch= New TxsltCompMatch
			
			this._xsltCompMatchPtr= _xsltCompMatchPtr
			
			Return this
		Else
			Return Null
		End If
	End Function
	
	Rem
	bbdoc: Compile the XSLT pattern and generates a list of precompiled form suitable for fast matching.
	returns: The generated pattern list or Null in case of failure.
	about: Parameters:
	<ul>
	<li><b>pattern</b> : an XSLT pattern</li>
	<li><b>doc</b> : the containing document</li>
	<li><b>node</b> : the containing element</li>
	<li><b>style</b> : the stylesheet</li>
	<li><b>runtime</b> : the transformation context, if done at run-time</li>
	</ul>
	End Rem
	Function compilePattern:TxsltCompMatch(pattern:String, doc:TxmlDoc, node:TxmlNode, style:TxsltStylesheet, runtime:TxsltTransformContext)
		Assert pattern, XSLT_ERROR_PARAM
		Assert doc, XSLT_ERROR_PARAM
		Assert node, XSLT_ERROR_PARAM
		Assert style, XSLT_ERROR_PARAM

		Local cStr1:Byte Ptr = pattern.toCString()
		Local comp:TxsltCompMatch

		If runtime <> Null Then
			comp = TxsltCompMatch._create(xsltCompilePattern(cStr1, doc._xmlDocPtr, node._xmlNodePtr, style._xsltStylesheetPtr, runtime._xsltTransformContextPtr))
		Else
			comp = TxsltCompMatch._create(xsltCompilePattern(cStr1, doc._xmlDocPtr, node._xmlNodePtr, style._xsltStylesheetPtr, Null))
		End If
		
		MemFree(cStr1)
		
		Return comp
	End Function
	
	Rem
	bbdoc: Test whether the node matches one of the patterns in the list.
	returns: 1 if it matches, 0 if it doesn't and -1 in case of failure.
	about: Parameters:
	<ul>
	<li><b>context</b> : an XSLT process context</li>
	<li><b>node</b> : a node</li>
	</ul>
	End Rem
	Method testCompMatchList:Int(context:TxsltTransformContext, node:TxmlNode)
		Return xsltTestCompMatchList(context._xsltTransformContextPtr, node._xmlNodePtr, _xsltCompMatchPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated by all the elements of the CompMatch.
	End Rem
	Method free()
		xsltFreeCompMatchList(_xsltCompMatchPtr)
	End Method
	
End Type
