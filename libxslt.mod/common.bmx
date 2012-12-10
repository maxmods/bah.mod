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

Import "source.bmx"

Extern
	Function bmx_libxslt_xsltParseStylesheetFile:Byte Ptr(uri:String)
	Function bmx_libxslt_xsltParseStylesheetDoc:Byte Ptr(docPtr:Byte Ptr)
	Function bmx_libxslt_xsltFreeStylesheet(sheet:Byte Ptr)
	Function bmx_libxslt_xsltNewStylesheet:Byte Ptr()
	Function bmx_libxslt_xsltParseStylesheetOutput(sheet:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltParseStylesheetProcess:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltParseTemplateContent(sheet:Byte Ptr, templ:Byte Ptr)
	Function bmx_libxslt_xsltParseStylesheetImport:Int(sheet:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltParseStylesheetInclude:Int(sheet:Byte Ptr, node:Byte Ptr)
	
	Function bmx_libxslt_xsltAddKey:Int(sheet:Byte Ptr, name:String, nameURI:String, match:String, use:String, inst:Byte Ptr)
	Function bmx_libxslt_xsltFreeKeys(sheet:Byte Ptr)
	
	Function bmx_libxslt_xsltParseStylesheetAttributeSet(sheet:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltResolveStylesheetAttributeSet(sheet:Byte Ptr)
	Function bmx_libxslt_xsltFreeAttributeSetsHashes(sheet:Byte Ptr)
	
	Function bmx_libxslt_xsltNewTransformContext:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltApplyStripSpaces(ctxt:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltApplyStylesheet:Byte Ptr(sheet:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltFindDocument:Byte Ptr(ctxt:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltFreeDocuments(ctxt:Byte Ptr)
	Function bmx_libxslt_xsltFreeStyleDocuments(style:Byte Ptr)
	Function bmx_libxslt_xsltNextImport:Byte Ptr(style:Byte Ptr)
	
	Function bmx_libxslt_xsltNewStyleDocument:Byte Ptr(style:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltNewDocument:Byte Ptr(ctxt:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltLoadStyleDocument:Byte Ptr(style:Byte Ptr, uri:String)
	Function bmx_libxslt_xsltLoadDocument:Byte Ptr(context:Byte Ptr, uri:String)
	Function bmx_libxslt_xsltFindElemSpaceHandling:Int(context:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltNeedElemSpaceHandling:Int(context:Byte Ptr)
	Function bmx_libxslt_xsltFreeDocumentKeys(doc:Byte Ptr)
	Function bmx_libxslt_xsltInitCtxtKeys(context:Byte Ptr, doc:Byte Ptr)
	Function bmx_libxslt_xsltGetKey:Byte Ptr(context:Byte Ptr, name:String, nameURI:String, value:String)
	
	Function bmx_libxslt_xsltFindTemplate:Byte Ptr(context:Byte Ptr, name:String, nameURI:String)
	
	Function bmx_libxslt_xsltNamespaceAlias(sheet:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltFreeNamespaceAliasHashes(sheet:Byte Ptr)
	Function bmx_libxslt_xsltGetNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, ns:Byte Ptr, out:Byte Ptr)
	Function bmx_libxslt_xsltGetPlainNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, ns:Byte Ptr, out:Byte Ptr)
	Function bmx_libxslt_xsltGetSpecialNamespace:Byte Ptr(context:Byte Ptr, cur:Byte Ptr, uri:String, prefix:String, out:Byte Ptr)
	
	Function bmx_libxslt_xsltDocumentComp:Byte Ptr(style:Byte Ptr, inst:Byte Ptr, func:Byte Ptr)
	Function bmx_libxslt_xsltFreeStylePreComps(sheet:Byte Ptr)
	Function bmx_libxslt_xsltStylePreCompute(sheet:Byte Ptr, node:Byte Ptr)
	
	Function bmx_libxslt_xsltAllocateExtra:Int(sheet:Byte Ptr)
	Function bmx_libxslt_xsltAllocateExtraCtxt:Int(context:Byte Ptr)
	Function bmx_libxslt_xsltCompileAttr(sheet:Byte Ptr, attr:Byte Ptr)
	Function bmx_libxslt_xsltCreateRVT:Byte Ptr(context:Byte Ptr)
	Function bmx_libxslt_xsltEvalAVT:String(context:Byte Ptr, avt:String, node:Byte Ptr)
	'Function bmx_libxslt_xsltElemPreCompDeallocator(precomp:Byte Ptr)
	Function bmx_libxslt_xsltFreeRVTs(context:Byte Ptr)
	Function bmx_libxslt_xsltRegisterPersistRVT:Int(context:Byte Ptr, rvt:Byte Ptr)
	Function bmx_libxslt_xsltRegisterTmpRVT:Int(context:Byte Ptr, rvt:Byte Ptr)
	
	Function bmx_libxslt_xsltFreeCompMatchList(comp:Byte Ptr)
	Function bmx_libxslt_xsltFreeTemplateHashes(sheet:Byte Ptr)
	Function bmx_libxslt_xsltAddTemplate:Int(sheet:Byte Ptr, cur:Byte Ptr, Mode:String, modeURI:String)
	Function bmx_libxslt_xsltCleanupTemplates(sheet:Byte Ptr)
	Function bmx_libxslt_xsltCompilePattern:Byte Ptr(pattern:String, doc:Byte Ptr, node:Byte Ptr, sheet:Byte Ptr, context:Byte Ptr)
	Function bmx_libxslt_xsltGetTemplate:Byte Ptr(context:Byte Ptr, node:Byte Ptr, style:Byte Ptr)
	Function bmx_libxslt_xsltTestCompMatchList:Int(context:Byte Ptr, node:Byte Ptr, comp:Byte Ptr)
	'Function bmx_libxslt_xsltMatchPattern:Int(context:Byte Ptr, node:Byte Ptr, pattern:Byte Ptr, ctxtdoc:Byte Ptr, ctxtnode:Byte Ptr)
	
	Function bmx_libxslt_xsltMessage(context:Byte Ptr, node:Byte Ptr, inst:Byte Ptr)
	Function bmx_libxslt_xsltPrintErrorContext(context:Byte Ptr, style:Byte Ptr, node:Byte Ptr)
	Function bmx_libxslt_xsltSaveResultToFilename:Int(url:String, result:Byte Ptr, style:Byte Ptr, compression:Int)
	
	Function bmx_libxslt_xsltAttrListTemplateProcess:Byte Ptr(context:Byte Ptr, target:Byte Ptr, cur:Byte Ptr)
	Function bmx_libxslt_xsltAttrTemplateProcess:Byte Ptr(context:Byte Ptr, target:Byte Ptr, cur:Byte Ptr)
	Function bmx_libxslt_xsltAttrTemplateValueProcess:String(context:Byte Ptr, value:String)
	Function bmx_libxslt_xsltAttrTemplateValueProcessNode:String(context:Byte Ptr, value:String, node:Byte Ptr)
	Function bmx_libxslt_xsltEvalAttrValueTemplate:String(context:Byte Ptr, node:Byte Ptr, name:String, namespace:String)
	Function bmx_libxslt_xsltEvalStaticAttrValueTemplate:String(style:Byte Ptr, node:Byte Ptr, name:String, namespace:String, found:Int Ptr)
	Function bmx_libxslt_xsltEvalTemplateString:String(context:Byte Ptr, node:Byte Ptr, parent:Byte Ptr)
	Function bmx_libxslt_xsltEvalXPathString:String(context:Byte Ptr, comp:Byte Ptr)
	
	Function bmx_libxslt_xsltelemprecomp_type:Int(handle:Byte Ptr)
	
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

