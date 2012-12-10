' Copyright (c) 2006-2012 Bruce A Henderson
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

ModuleInfo "Version: 2.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Libxslt - 1998-2012 Daniel Veillard"
ModuleInfo "Copyright: Wrapper - 2006-2012 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 2.00"
ModuleInfo "History: Updated to Libxslt 1.1.28"
ModuleInfo "History: Complete rewrite of API glue to match standard module layout."
ModuleInfo "History: Moved source into libxslt dir."
ModuleInfo "History: 1.03"
ModuleInfo "History: Updated to Libxslt 1.1.26"
ModuleInfo "History: 1.02"
ModuleInfo "History: Updated to Libxslt 1.1.25"
ModuleInfo "History: 1.01"
ModuleInfo "History: Updated to Libxslt 1.1.20"
ModuleInfo "History: 1.00 Initial Release (Libxslt 1.1.16)"

Import "common.bmx"


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

	' reference to the actual stylesheet
	Field xsltStylesheetPtr:Byte Ptr

	Function _create:TxsltStylesheet(xsltStylesheetPtr:Byte Ptr)
		If xsltStylesheetPtr <> Null Then
			Local this:TxsltStylesheet = New TxsltStylesheet
			
			this.xsltStylesheetPtr = xsltStylesheetPtr
			
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
		Return TxsltStylesheet._create(bmx_libxslt_xsltNewStylesheet())
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
		'Local cStr:Byte Ptr = filename.toCString()
		Return TxsltStylesheet._create(bmx_libxslt_xsltParseStylesheetFile(filename))
		'MemFree cStr
		
		'If xsltStylesheetPtr = Null Then
		'	Return Null
		'Else
		'	Return TxsltStylesheet._create(xsltStylesheetPtr)
		'End If
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
		
		Return TxsltStylesheet._create(bmx_libxslt_xsltParseStylesheetDoc(doc.basePtr))
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
		
		bmx_libxslt_xsltParseStylesheetOutput(xsltStylesheetPtr, node.basePtr)
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
		
		Return TxsltStylesheet._create(bmx_libxslt_xsltParseStylesheetProcess(xsltStylesheetPtr, doc.basePtr))
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

		bmx_libxslt_xsltParseTemplateContent(xsltStylesheetPtr, templ.basePtr)
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
	
		'Local cStr:Byte Ptr = url.toCString()
		
		Return bmx_libxslt_xsltSaveResultToFilename(url, res.basePtr, xsltStylesheetPtr, compression)
		
		'MemFree(cStr)
		
		'Return ret
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
		
		Return TxmlDoc._create(bmx_libxslt_xsltApplyStylesheet(xsltStylesheetPtr, doc.basePtr))
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
	
		'Local res:Int
		
		'Local cStr1:Byte Ptr = match.toCString()
		'Local cStr2:Byte Ptr = use.toCString()
		
		'If name = Null Then
		'	If nameURI = Null Then
		'		res = xsltAddKey(xsltStylesheetPtr, Null, Null, cStr1, cStr2, inst.basePtr)
		'	Else
		'		Local cStr3:Byte Ptr = nameURI.toCString()
		'		
		'		res = xsltAddKey(xsltStylesheetPtr, Null, cStr3, cStr1, cStr2, inst.basePtr)
		'		
		'		MemFree(cStr3)
		'	End If
		'Else
		'	Local cStr4:Byte Ptr = name.toCString()
		'	
		'	If nameURI = Null Then
		'		res = xsltAddKey(xsltStylesheetPtr, cStr4, Null, cStr1, cStr2, inst.basePtr)
		'	
		'	Else
		'		Local cStr3:Byte Ptr = nameURI.toCString()
				
		Return bmx_libxslt_xsltAddKey(xsltStylesheetPtr, name, nameURI, match, use, inst.basePtr)
				
		'		MemFree(cStr3)
		'	End If
		'	
		'	MemFree(cStr4)
		'End If
		
		'MemFree(cStr1)
		'MemFree(cStr2)
		
		'Return res
	End Method
	
	Rem
	bbdoc: Free up the memory used by XSLT keys in the stylesheet.
	End Rem
	Method freeKeys()
		bmx_libxslt_xsltFreeKeys(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Resolve the references between attribute sets.
	End Rem
	Method resolveStylesheetAttributeSet()
		bmx_libxslt_xsltResolveStylesheetAttributeSet(xsltStylesheetPtr)
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
			bmx_libxslt_xsltParseStylesheetAttributeSet(xsltStylesheetPtr, node.basePtr)
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
		
		Return bmx_libxslt_xsltParseStylesheetImport(xsltStylesheetPtr, node.basePtr)
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
		
		Return bmx_libxslt_xsltParseStylesheetInclude(xsltStylesheetPtr, node.basePtr)
	End Method
	
	Rem
	bbdoc: Find the next stylesheet in import precedence.
	returns: The next stylesheet or Null if it was the last one
	End Rem
	Method nextImport:TxsltStylesheet()
		Return TxsltStylesheet._create(bmx_libxslt_xsltNextImport(xsltStylesheetPtr))
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
		
		bmx_libxslt_xsltNamespaceAlias(xsltStylesheetPtr, node.basePtr)
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
	
		Return TxsltElemPreComp._create(bmx_libxslt_xsltDocumentComp(xsltStylesheetPtr, inst.basePtr, Null))
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
		
		bmx_libxslt_xsltStylePreCompute(xsltStylesheetPtr, inst.basePtr)
	End Method
	
	Rem
	bbdoc: Allocate an extra runtime information slot statically while compiling the stylesheet and return its number.
	returns: The number of the slot.
	End Rem
	Method allocateExtra:Int()
		Return bmx_libxslt_xsltAllocateExtra(xsltStylesheetPtr)
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
						
		'If Mode <> Null Then
		'	Local cStr1:Byte Ptr = Mode.toCString()
		'	
		'	If modeURI <> Null Then
		'		Local cStr2:Byte Ptr = modeURI.toCString()
				
		Return bmx_libxslt_xsltAddTemplate(xsltStylesheetPtr, cur.xsltTemplatePtr, Mode, modeURI)
				
		'		MemFree(cStr2)
		'	Else
		'		res = xsltAddTemplate(xsltStylesheetPtr, cur.xsltTemplatePtr, cStr1, Null)
		'	End If
		'	
		'	MemFree(cStr1)
		'Else
		'	If modeURI <> Null Then
		'		Local cStr2:Byte Ptr = modeURI.toCString()
		'		
		'		res = xsltAddTemplate(xsltStylesheetPtr, cur.xsltTemplatePtr, Null, cStr2)
		'		
		'		MemFree(cStr2)
		'	Else
		'		res = xsltAddTemplate(xsltStylesheetPtr, cur.xsltTemplatePtr, Null, Null)
		'	End If
		'End If
		
		'Return res
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
	
		'Local ret:String = Null
		'Local s:Byte Ptr
		Local found:Int
		
		'If name <> Null Then
		'	Local cStr1:Byte Ptr = name.toCString()
		'	
		'	If namespace <> Null Then
		'		Local cStr2:Byte Ptr = namespace.toCString()
				
		Return bmx_libxslt_xsltEvalStaticAttrValueTemplate(xsltStylesheetPtr, node.basePtr, name, namespace, Varptr found)
				
		'		MemFree(cStr2)
		'	Else
		'		s = xsltEvalStaticAttrValueTemplate(xsltTransformContextPtr, node.basePtr, cStr1, Null, Varptr found)
		'	
		'	End If
		'	
		'	MemFree(cStr1)

		'Else
'
'			If namespace <> Null Then
'				Local cStr2:Byte Ptr = namespace.toCString()
'				
'				s = xsltEvalStaticAttrValueTemplate(xsltTransformContextPtr, node.basePtr, Null, cStr2, Varptr found)
'
'				MemFree(cStr2)
'			Else
'				s = xsltEvalStaticAttrValueTemplate(xsltTransformContextPtr, node.basePtr, Null, Null, Varptr found)
'			
'			End If
'		
'		End If
'		
'		If s <> Null Then
'			ret = String.fromCString(s)
'			MemFree(s)
'		End If
'		
'		Return ret
	End Method
	
	Rem
	bbdoc: Cleanup the state of the templates used by the stylesheet and the ones it imports.
	End Rem 
	Method cleanupTemplates()
		bmx_libxslt_xsltCleanupTemplates(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated by all precomputed blocks.
	End Rem
	Method freeStylePreComps()
		bmx_libxslt_xsltFreeStylePreComps(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by namespaces aliases.
	End Rem
	Method freeNamespaceAliasHashes()
		bmx_libxslt_xsltFreeNamespaceAliasHashes(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by attribute sets.
	End Rem
	Method freeAttributeSetsHashes()
		bmx_libxslt_xsltFreeAttributeSetsHashes(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up all the space used by the loaded documents.
	End Rem
	Method freeStyleDocuments()
		bmx_libxslt_xsltFreeStyleDocuments(xsltStylesheetPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory used by @addTemplate / @getTemplate mechanism.
	End Rem
	Method freeTemplateHashes()
		bmx_libxslt_xsltFreeTemplateHashes(xsltStylesheetPtr)
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
		
		bmx_libxslt_xsltCompileAttr(xsltStylesheetPtr, attr.basePtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated to the sheet.
	End Rem
	Method free()
		bmx_libxslt_xsltFreeStylesheet(xsltStylesheetPtr)
	End Method
End Type

Rem
bbdoc: An XSLT transformation context
End Rem
Type TxsltTransformContext

	Field xsltTransformContextPtr:Byte Ptr

	Function _create:TxsltTransformContext(xsltTransformContextPtr:Byte Ptr)
		If xsltTransformContextPtr<> Null Then
			Local this:TxsltTransformContext= New TxsltTransformContext
			
			this.xsltTransformContextPtr= xsltTransformContextPtr
			
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
	
		Return TxsltTransformContext._create(bmx_libxslt_xsltNewTransformContext(stylesheet.xsltStylesheetPtr, doc.basePtr))
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
		
		bmx_libxslt_xsltApplyStripSpaces(xsltTransformContextPtr, node.basePtr)
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

		Return TxsltDocument._create(bmx_libxslt_xsltFindDocument(xsltTransformContextPtr, doc.basePtr))
	End Method
	
	Rem
	bbdoc: Free up all the space used by the loaded documents.
	End Rem
	Method freeDocuments()
		bmx_libxslt_xsltFreeDocuments(xsltTransformContextPtr)
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
		
		Return bmx_libxslt_xsltFindElemSpaceHandling(xsltTransformContextPtr, node.basePtr)
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
		
		'Local cStr1:Byte Ptr = name.toCString()
		'Local cStr2:Byte Ptr = nameURI.toCString()
		
		Return TxsltTemplate._create(bmx_libxslt_xsltFindTemplate(xsltTransformContextPtr, name, nameURI))
		
		'MemFree(cStr1)
		'MemFree(cStr2)
		
		'Return tmpl
	End Method
	
	Rem
	bbdoc: Checks whether that stylesheet requires white-space stripping.
	returns: 1 if space should be stripped, 0 if not
	End Rem
	Method needElemSpaceHandling:Int()
		Return bmx_libxslt_xsltNeedElemSpaceHandling(xsltTransformContextPtr)
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
		
		bmx_libxslt_xsltInitCtxtKeys(xsltTransformContextPtr, doc.xsltDocumentPtr)
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

		Return TxmlNs._create(bmx_libxslt_xsltGetNamespace(xsltTransformContextPtr, curNode.basePtr, namespace.xmlNsPtr, out.basePtr))
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

		Return TxmlNs._create(bmx_libxslt_xsltGetPlainNamespace(xsltTransformContextPtr, curNode.basePtr, namespace.xmlNsPtr, out.basePtr))
	End Method
	
	Rem
	bbdoc: Find the right namespace value for this URI.
	returns: The namespace node to use or Null.
	about: If needed create and add a new namespace decalaration on the node.
	<p>Parameters:
	<ul>
	<li><b>curNode</b> : the input node</li>
	<li><b>name</b> : the namespace name, or Null </li>
	<li><b>prefix</b> : the suggested namespace prefix, or Null </li>
	<li><b>out</b> : the output node (or its parent)</li>
	</ul>
	</p>
	End Rem
	Method getSpecialNamespace:TxmlNs(curNode:TxmlNode, name:String, prefix:String, out:TxmlNode)
		Assert curNode, XSLT_ERROR_PARAM
		'Assert name, XSLT_ERROR_PARAM
		'Assert prefix, XSLT_ERROR_PARAM
		Assert out, XSLT_ERROR_PARAM
	
		'Local cStr1:Byte Ptr = uri.toCString()
		'Local cStr2:Byte Ptr = prefix.toCString()
		Return TxmlNs._create(bmx_libxslt_xsltGetSpecialNamespace(xsltTransformContextPtr, curNode.basePtr, name, prefix, out.basePtr))
		
		'MemFree(cStr1)
		'MemFree(cStr2)
		
		'Return ns
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
			tmpl = TxsltTemplate._create(bmx_libxslt_xsltGetTemplate(xsltTransformContextPtr, node.basePtr, style.xsltStylesheetPtr))
		Else
			tmpl = TxsltTemplate._create(bmx_libxslt_xsltGetTemplate(xsltTransformContextPtr, node.basePtr, Null))
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
		
		ret = xsltMatchPattern(xsltTransformContextPtr, node.basePtr, cStr1, ctxtdoc.basePtr, ctxtnode.basePtr)
		
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
		
		'Local cStr3:Byte Ptr = value.toCString()
		
		'If name <> Null Then
		
		'	Local cStr1:Byte Ptr = name.toCString()
		
		'	If nameURI <> Null Then
		'		Local cStr2:Byte Ptr = nameURI.toCString()
		Return TxmlNodeSet._create(bmx_libxslt_xsltGetKey(xsltTransformContextPtr, name, nameURI, value))
		'		MemFree(cStr2)
		'	Else
		'		nodeset = TxmlNodeSet._create(xsltGetKey(xsltTransformContextPtr, cStr1, Null, cStr3))
		'	End If
		'	
		'	MemFree(cStr1)
		'Else
		'	If nameURI <> Null Then
		'		Local cStr2:Byte Ptr = nameURI.toCString()
		'		nodeset = TxmlNodeSet._create(xsltGetKey(xsltTransformContextPtr, Null, cStr2, cStr3))
		'		MemFree(cStr2)
		'	Else
		'		nodeset = TxmlNodeSet._create(xsltGetKey(xsltTransformContextPtr, Null, Null, cStr3))
		'	End If
		'End If
		
		'MemFree(cStr3)
		
		'Return nodeset
	End Method
	
	Rem
	bbdoc: Allocate an extra runtime information slot at run-time and return its number.
	returns: the number of the slot
	about: This makes sure there is a slot ready in the transformation context.
	End Rem
	Method allocateExtraCtxt:Int()
		Return bmx_libxslt_xsltAllocateExtraCtxt(xsltTransformContextPtr)
	End Method
	
	Rem
	bbdoc: Create a result value tree.
	returns: The result value tree or Null in case of error.
	End Rem
	Method createRVT:TxmlDoc()
		Return TxmlDoc._create(bmx_libxslt_xsltCreateRVT(xsltTransformContextPtr))
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
	
		'Local ret:String = Null
		
		'Local cStr1:Byte Ptr = avt.toCString()
		Return bmx_libxslt_xsltEvalAVT(xsltTransformContextPtr, avt, node.basePtr)
		
		'MemFree(cStr1)
		'If r <> Null Then
		'	ret = String.fromCString(r)
		'End If
		'MemFree(r)
		
		'Return ret
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

		Return bmx_libxslt_xsltRegisterPersistRVT(xsltTransformContextPtr, rvt.basePtr)
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
		
		Return bmx_libxslt_xsltRegisterTmpRVT(xsltTransformContextPtr, rvt.basePtr)
	End Method
	
	Rem
	bbdoc: Free all the registered result value tree of the transformation.
	End Rem
	Method freeRVTs()
		bmx_libxslt_xsltFreeRVTs(xsltTransformContextPtr)
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
		
		Return TxmlAttribute._create(bmx_libxslt_xsltAttrListTemplateProcess(xsltTransformContextPtr, target.basePtr, cur.basePtr))
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
		
		Return TxmlAttribute._create(bmx_libxslt_xsltAttrTemplateProcess(xsltTransformContextPtr, target.basePtr, cur.basePtr))
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
		'Local ret:String = Null
	
		'Local cStr:Byte Ptr = value.toCString()
		Return bmx_libxslt_xsltAttrTemplateValueProcess(xsltTransformContextPtr, value)
		
		'If s <> Null Then
		'	ret = String.fromCString(s)
		'	MemFree(s)
		'End If
		'MemFree(cStr)
		
		'Return ret
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
		
		'Local ret:String = Null
	
		'Local cStr:Byte Ptr = value.toCString()
		Return bmx_libxslt_xsltAttrTemplateValueProcessNode(xsltTransformContextPtr, value, node.basePtr)
		
		'If s <> Null Then
		'	ret = String.fromCString(s)
		'	MemFree(s)
		'End If
		'MemFree(cStr)
		
		'Return ret
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
	
		'Local ret:String = Null
		'Local s:Byte Ptr
		
		'If name <> Null Then
		'	Local cStr1:Byte Ptr = name.toCString()
		'	
		'	If namespace <> Null Then
		'		Local cStr2:Byte Ptr = namespace.toCString()
		'		
		Return bmx_libxslt_xsltEvalAttrValueTemplate(xsltTransformContextPtr, node.basePtr, name, namespace)
		'		
		'		MemFree(cStr2)
		'	Else
		'		s = xsltEvalAttrValueTemplate(xsltTransformContextPtr, node.basePtr, cStr1, Null)
		'	
		'	End If
		'	
		'	MemFree(cStr1)
'
		'Else
'
'			If namespace <> Null Then
'				Local cStr2:Byte Ptr = namespace.toCString()
'				
'				s = xsltEvalAttrValueTemplate(xsltTransformContextPtr, node.basePtr, Null, cStr2)
'
'				MemFree(cStr2)
'			Else
'				s = xsltEvalAttrValueTemplate(xsltTransformContextPtr, node.basePtr, Null, Null)
'			
'			End If
'		
'		End If
'		
'		If s <> Null Then
'			ret = String.fromCString(s)
'			MemFree(s)
'		End If
'		
'		Return ret
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
		
		'Local ret:String = Null
	
		Return bmx_libxslt_xsltEvalTemplateString(xsltTransformContextPtr, node.basePtr, parent.basePtr)
		
		'If s <> Null Then
		'	ret = String.fromCString(s)
		'	MemFree(s)
		'End If
		
		'Return ret
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
		
		'Local ret:String = Null
	
		Return bmx_libxslt_xsltEvalXPathString(xsltTransformContextPtr, comp.xmlXPathCompExprPtr)
		'If s <> Null Then
		'	ret = String.fromCString(s)
		'	MemFree(s)
		'End If
		
		'Return ret
	End Method
	
End Type

Rem
bbdoc: An XSLT Document
End Rem
Type TxsltDocument

	Field xsltDocumentPtr:Byte Ptr
	
	Function _create:TxsltDocument(xsltDocumentPtr:Byte Ptr)
		If xsltDocumentPtr<> Null Then
			Local this:TxsltDocument= New TxsltDocument
			
			this.xsltDocumentPtr= xsltDocumentPtr
			
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
			Return TxsltDocument._create(bmx_libxslt_xsltNewDocument(context.xsltTransformContextPtr, doc.basePtr))
		Else
			Return TxsltDocument._create(bmx_libxslt_xsltNewDocument(Null, doc.basePtr))
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
		
		Return TxsltDocument._create(bmx_libxslt_xsltNewStyleDocument(style.xsltStylesheetPtr, doc.basePtr))
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
		
		'Local cStr:Byte Ptr = uri.toCString()
		Return TxsltDocument._create(bmx_libxslt_xsltLoadStyleDocument(style.xsltStylesheetPtr, uri))
		'MemFree(cStr)
		
		'Return doc
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

		'Local cStr:Byte Ptr = uri.toCString()
		Return TxsltDocument._create(bmx_libxslt_xsltLoadDocument(context.xsltTransformContextPtr, uri))
		'MemFree(cStr)
		
		'Return doc
	End Function

	Rem
	bbdoc: Free the keys associated to a document.
	End Rem
	Method freeDocumentKeys()
		bmx_libxslt_xsltFreeDocumentKeys(xsltDocumentPtr)
	End Method
	
End Type

Rem
bbdoc: An XSLT ElemPreComp
End Rem
Type TxsltElemPreComp

	Field xsltElemPreCompPtr:Byte Ptr
	
	Function _create:TxsltElemPreComp(xsltElemPreCompPtr:Byte Ptr)
		If xsltElemPreCompPtr<> Null Then
			Local this:TxsltElemPreComp= New TxsltElemPreComp
			
			this.xsltElemPreCompPtr= xsltElemPreCompPtr
			
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
		Return bmx_libxslt_xsltelemprecomp_type(xsltElemPreCompPtr)
	End Method
	
	Rem
	bbdoc: Frees (deallocates) a TxsltElemPreComp structure.
	'End Rem
	Method free()
		xsltElemPreCompDeallocator(xsltElemPreCompPtr)
	End Method
	End Rem
End Type

Rem
bbdoc: An XSLT Template
End Rem
Type TxsltTemplate

	Field xsltTemplatePtr:Byte Ptr
	
	Function _create:TxsltTemplate(xsltTemplatePtr:Byte Ptr)
		If xsltTemplatePtr <> Null Then
			Local this:TxsltTemplate = New TxsltTemplate
			
			this.xsltTemplatePtr = xsltTemplatePtr
			
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

	Field xsltCompMatchPtr:Byte Ptr
	
	Function _create:TxsltCompMatch(xsltCompMatchPtr:Byte Ptr)
		If xsltCompMatchPtr <> Null Then
			Local this:TxsltCompMatch = New TxsltCompMatch
			
			this.xsltCompMatchPtr = xsltCompMatchPtr
			
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

		'Local cStr1:Byte Ptr = pattern.toCString()
		'Local comp:TxsltCompMatch

		If runtime <> Null Then
			Return TxsltCompMatch._create(bmx_libxslt_xsltCompilePattern(pattern, doc.basePtr, node.basePtr, style.xsltStylesheetPtr, runtime.xsltTransformContextPtr))
		Else
			Return TxsltCompMatch._create(bmx_libxslt_xsltCompilePattern(pattern, doc.basePtr, node.basePtr, style.xsltStylesheetPtr, Null))
		End If
		
		'MemFree(cStr1)
		
		'Return comp
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
		Return bmx_libxslt_xsltTestCompMatchList(context.xsltTransformContextPtr, node.basePtr, xsltCompMatchPtr)
	End Method
	
	Rem
	bbdoc: Free up the memory allocated by all the elements of the CompMatch.
	End Rem
	Method free()
		bmx_libxslt_xsltFreeCompMatchList(xsltCompMatchPtr)
	End Method
	
End Type
