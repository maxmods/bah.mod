/*
 Copyright (c) 2006-2012 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/
#include "blitz.h"

#include "libxml/tree.h"
#include "libxslt/xslt.h"
#include "libxslt/xsltInternals.h"
#include "libxslt/xsltutils.h"
#include "libxslt/transform.h"
#include "libxslt/templates.h"
#include "libxslt/namespaces.h"
#include "libxslt/pattern.h"
#include "libxslt/imports.h"
#include "libxslt/keys.h"
#include "libxslt/attributes.h"
#include "libxslt/documents.h"
#include "libxslt/preproc.h"


extern "C" {
	// from bah.libxml
	BBString * bbStringFromXmlChar(xmlChar * text);
	char * bbStringToUTF8StringOrNull(BBString * text);
	//

	xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetFile(BBString * uri);
	xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetDoc(xmlDocPtr doc);
	void bmx_libxslt_xsltFreeStylesheet(xsltStylesheetPtr sheet);
	xsltStylesheetPtr bmx_libxslt_xsltNewStylesheet();
	void bmx_libxslt_xsltParseStylesheetOutput(xsltStylesheetPtr sheet, xmlNodePtr node);
	xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetProcess(xsltStylesheetPtr sheet, xmlDocPtr doc);
	void bmx_libxslt_xsltParseTemplateContent(xsltStylesheetPtr sheet, xmlNodePtr templ);
	int bmx_libxslt_xsltParseStylesheetImport(xsltStylesheetPtr sheet, xmlNodePtr node);
	int bmx_libxslt_xsltParseStylesheetInclude(xsltStylesheetPtr sheet, xmlNodePtr node);
	
	int bmx_libxslt_xsltAddKey(xsltStylesheetPtr sheet, BBString * name, BBString * nameURI, BBString * match, BBString * use, xmlNodePtr inst);
	void bmx_libxslt_xsltFreeKeys(xsltStylesheetPtr sheet);
	
	void bmx_libxslt_xsltParseStylesheetAttributeSet(xsltStylesheetPtr sheet, xmlNodePtr node);
	void bmx_libxslt_xsltResolveStylesheetAttributeSet(xsltStylesheetPtr sheet);
	void bmx_libxslt_xsltFreeAttributeSetsHashes(xsltStylesheetPtr sheet);
	
	xsltTransformContextPtr bmx_libxslt_xsltNewTransformContext(xsltStylesheetPtr sheet, xmlDocPtr doc);
	void bmx_libxslt_xsltApplyStripSpaces(xsltTransformContextPtr ctxt, xmlNodePtr node);
	xmlDocPtr bmx_libxslt_xsltApplyStylesheet(xsltStylesheetPtr sheet, xmlDocPtr doc);
	xsltDocumentPtr bmx_libxslt_xsltFindDocument(xsltTransformContextPtr ctxt, xmlDocPtr doc);
	void bmx_libxslt_xsltFreeDocuments(xsltTransformContextPtr ctxt);
	void bmx_libxslt_xsltFreeStyleDocuments(xsltStylesheetPtr style);
	xsltStylesheetPtr bmx_libxslt_xsltNextImport(xsltStylesheetPtr style);
	
	xsltDocumentPtr bmx_libxslt_xsltNewStyleDocument(xsltStylesheetPtr style, xmlDocPtr doc);
	xsltDocumentPtr bmx_libxslt_xsltNewDocument(xsltTransformContextPtr ctxt, xmlDocPtr doc);
	xsltDocumentPtr bmx_libxslt_xsltLoadStyleDocument(xsltStylesheetPtr style, BBString * uri);
	xsltDocumentPtr bmx_libxslt_xsltLoadDocument(xsltTransformContextPtr context, BBString * uri);
	int bmx_libxslt_xsltFindElemSpaceHandling(xsltTransformContextPtr context, xmlNodePtr node);
	int bmx_libxslt_xsltNeedElemSpaceHandling(xsltTransformContextPtr context);
	void bmx_libxslt_xsltFreeDocumentKeys(xsltDocumentPtr doc);
	void bmx_libxslt_xsltInitCtxtKeys(xsltTransformContextPtr context, xsltDocumentPtr doc);
	xmlNodeSetPtr bmx_libxslt_xsltGetKey(xsltTransformContextPtr context, BBString * name, BBString * nameURI, BBString * value);
	
	xsltTemplatePtr bmx_libxslt_xsltFindTemplate(xsltTransformContextPtr context, BBString * name, BBString * nameURI);
	
	void bmx_libxslt_xsltNamespaceAlias(xsltStylesheetPtr sheet, xmlNodePtr node);
	void bmx_libxslt_xsltFreeNamespaceAliasHashes(xsltStylesheetPtr sheet);
	xmlNsPtr bmx_libxslt_xsltGetNamespace(xsltTransformContextPtr context, xmlNodePtr cur, xmlNsPtr ns, xmlNodePtr out);
	xmlNsPtr bmx_libxslt_xsltGetPlainNamespace(xsltTransformContextPtr context, xmlNodePtr cur, xmlNsPtr ns, xmlNodePtr out);
	xmlNsPtr bmx_libxslt_xsltGetSpecialNamespace(xsltTransformContextPtr context, xmlNodePtr cur, BBString * uri, BBString * prefix, xmlNodePtr out);
	
	xsltElemPreCompPtr bmx_libxslt_xsltDocumentComp(xsltStylesheetPtr style, xmlNodePtr inst, xsltTransformFunction func);
	void bmx_libxslt_xsltFreeStylePreComps(xsltStylesheetPtr sheet);
	void bmx_libxslt_xsltStylePreCompute(xsltStylesheetPtr sheet, xmlNodePtr node);
	
	int bmx_libxslt_xsltAllocateExtra(xsltStylesheetPtr sheet);
	int bmx_libxslt_xsltAllocateExtraCtxt(xsltTransformContextPtr context);
	void bmx_libxslt_xsltCompileAttr(xsltStylesheetPtr sheet, xmlAttrPtr attr);
	xmlDocPtr bmx_libxslt_xsltCreateRVT(xsltTransformContextPtr context);
	BBString * bmx_libxslt_xsltEvalAVT(xsltTransformContextPtr context, BBString * avt, xmlNodePtr node);

	void bmx_libxslt_xsltFreeRVTs(xsltTransformContextPtr context);
	int bmx_libxslt_xsltRegisterPersistRVT(xsltTransformContextPtr context, xmlDocPtr rvt);
	int bmx_libxslt_xsltRegisterTmpRVT(xsltTransformContextPtr context, xmlDocPtr rvt);
	
	void bmx_libxslt_xsltFreeCompMatchList(xsltCompMatchPtr comp);
	void bmx_libxslt_xsltFreeTemplateHashes(xsltStylesheetPtr sheet);
	int bmx_libxslt_xsltAddTemplate(xsltStylesheetPtr sheet, xsltTemplatePtr cur, BBString * Mode, BBString * modeURI);
	void bmx_libxslt_xsltCleanupTemplates(xsltStylesheetPtr sheet);
	xsltCompMatchPtr bmx_libxslt_xsltCompilePattern(BBString * pattern, xmlDocPtr doc, xmlNodePtr node, xsltStylesheetPtr sheet, xsltTransformContextPtr context);
	xsltTemplatePtr bmx_libxslt_xsltGetTemplate(xsltTransformContextPtr context, xmlNodePtr node, xsltStylesheetPtr style);
	int bmx_libxslt_xsltTestCompMatchList(xsltTransformContextPtr context, xmlNodePtr node, xsltCompMatchPtr comp);
	
	void bmx_libxslt_xsltMessage(xsltTransformContextPtr context, xmlNodePtr node, xmlNodePtr inst);
	void bmx_libxslt_xsltPrintErrorContext(xsltTransformContextPtr context, xsltStylesheetPtr style, xmlNodePtr node);
	int bmx_libxslt_xsltSaveResultToFilename(BBString * url, xmlDocPtr result, xsltStylesheetPtr style, int compression);
	
	xmlAttrPtr bmx_libxslt_xsltAttrListTemplateProcess(xsltTransformContextPtr context, xmlNodePtr target, xmlAttrPtr cur);
	xmlAttrPtr bmx_libxslt_xsltAttrTemplateProcess(xsltTransformContextPtr context, xmlNodePtr target, xmlAttrPtr cur);
	BBString * bmx_libxslt_xsltAttrTemplateValueProcess(xsltTransformContextPtr context, BBString * value);
	BBString * bmx_libxslt_xsltAttrTemplateValueProcessNode(xsltTransformContextPtr context, BBString * value, xmlNodePtr node);
	BBString * bmx_libxslt_xsltEvalAttrValueTemplate(xsltTransformContextPtr context, xmlNodePtr node, BBString * name, BBString * ns);
	BBString * bmx_libxslt_xsltEvalStaticAttrValueTemplate(xsltStylesheetPtr style, xmlNodePtr node, BBString * name, BBString * ns, int * found);
	BBString * bmx_libxslt_xsltEvalTemplateString(xsltTransformContextPtr context, xmlNodePtr node, xmlNodePtr parent);
	BBString * bmx_libxslt_xsltEvalXPathString(xsltTransformContextPtr context, xmlXPathCompExprPtr comp);
	
	int bmx_libxslt_xsltelemprecomp_type(xsltElemPreCompPtr handle);

}


xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetFile(BBString * uri) {
	char * u = bbStringToUTF8String(uri);
	xsltStylesheetPtr sheet = xsltParseStylesheetFile((xmlChar * )u);
	bbMemFree(u);
	return sheet;
}

xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetDoc(xmlDocPtr doc) {
	return xsltParseStylesheetDoc(doc);
}

void bmx_libxslt_xsltFreeStylesheet(xsltStylesheetPtr sheet) {
	xsltFreeStylesheet(sheet);
}

xsltStylesheetPtr bmx_libxslt_xsltNewStylesheet() {
	return xsltNewStylesheet();
}

void bmx_libxslt_xsltParseStylesheetOutput(xsltStylesheetPtr sheet, xmlNodePtr node) {
	xsltParseStylesheetOutput(sheet, node);
}

xsltStylesheetPtr bmx_libxslt_xsltParseStylesheetProcess(xsltStylesheetPtr sheet, xmlDocPtr doc) {
	return xsltParseStylesheetProcess(sheet, doc);
}

void bmx_libxslt_xsltParseTemplateContent(xsltStylesheetPtr sheet, xmlNodePtr templ) {
	xsltParseTemplateContent(sheet, templ);
}

int bmx_libxslt_xsltParseStylesheetImport(xsltStylesheetPtr sheet, xmlNodePtr node) {
	return xsltParseStylesheetImport(sheet, node);
}

int bmx_libxslt_xsltParseStylesheetInclude(xsltStylesheetPtr sheet, xmlNodePtr node) {
	return xsltParseStylesheetInclude(sheet, node);
}


int bmx_libxslt_xsltAddKey(xsltStylesheetPtr sheet, BBString * name, BBString * nameURI, BBString * match, BBString * use, xmlNodePtr inst) {
	char * n = bbStringToUTF8StringOrNull(name);
	char * u = bbStringToUTF8StringOrNull(nameURI);
	char * m = bbStringToUTF8String(match);
	char * s = bbStringToUTF8String(use);
	int ret = xsltAddKey(sheet, (xmlChar *)n, (xmlChar *)u, (xmlChar *)m, (xmlChar *)s, inst);
	bbMemFree(s);
	bbMemFree(m);
	bbMemFree(u);
	bbMemFree(n);
	return ret;
}

void bmx_libxslt_xsltFreeKeys(xsltStylesheetPtr sheet) {
	xsltFreeKeys(sheet);
}


void bmx_libxslt_xsltParseStylesheetAttributeSet(xsltStylesheetPtr sheet, xmlNodePtr node) {
	xsltParseStylesheetAttributeSet(sheet, node);
}

void bmx_libxslt_xsltResolveStylesheetAttributeSet(xsltStylesheetPtr sheet) {
	xsltResolveStylesheetAttributeSet(sheet);
}

void bmx_libxslt_xsltFreeAttributeSetsHashes(xsltStylesheetPtr sheet) {
	xsltFreeAttributeSetsHashes(sheet);
}


xsltTransformContextPtr bmx_libxslt_xsltNewTransformContext(xsltStylesheetPtr sheet, xmlDocPtr doc) {
	return xsltNewTransformContext(sheet, doc);
}

void bmx_libxslt_xsltApplyStripSpaces(xsltTransformContextPtr ctxt, xmlNodePtr node) {
	xsltApplyStripSpaces(ctxt, node);
}

xmlDocPtr bmx_libxslt_xsltApplyStylesheet(xsltStylesheetPtr sheet, xmlDocPtr doc) {
	return xsltApplyStylesheet(sheet, doc, 0);
}

xsltDocumentPtr bmx_libxslt_xsltFindDocument(xsltTransformContextPtr ctxt, xmlDocPtr doc) {
	return xsltFindDocument(ctxt, doc);
}

void bmx_libxslt_xsltFreeDocuments(xsltTransformContextPtr ctxt) {
	xsltFreeDocuments(ctxt);
}

void bmx_libxslt_xsltFreeStyleDocuments(xsltStylesheetPtr style) {
	xsltFreeStyleDocuments(style);
}

xsltStylesheetPtr bmx_libxslt_xsltNextImport(xsltStylesheetPtr style) {
	return xsltNextImport(style);
}


xsltDocumentPtr bmx_libxslt_xsltNewStyleDocument(xsltStylesheetPtr style, xmlDocPtr doc) {
	return xsltNewStyleDocument(style, doc);
}

xsltDocumentPtr bmx_libxslt_xsltNewDocument(xsltTransformContextPtr ctxt, xmlDocPtr doc) {
	return xsltNewDocument(ctxt, doc);
}

xsltDocumentPtr bmx_libxslt_xsltLoadStyleDocument(xsltStylesheetPtr style, BBString * uri) {
	char * u = bbStringToUTF8String(uri);
	xsltDocumentPtr doc = xsltLoadStyleDocument(style, (xmlChar * )u);
	bbMemFree(u);
	return doc;
}

xsltDocumentPtr bmx_libxslt_xsltLoadDocument(xsltTransformContextPtr context, BBString * uri) {
	char * u = bbStringToUTF8String(uri);
	xsltDocumentPtr doc = xsltLoadDocument(context, (xmlChar * )u);
	bbMemFree(u);
	return doc;
}

int bmx_libxslt_xsltFindElemSpaceHandling(xsltTransformContextPtr context, xmlNodePtr node) {
	return xsltFindElemSpaceHandling(context, node);
}

int bmx_libxslt_xsltNeedElemSpaceHandling(xsltTransformContextPtr context) {
	return xsltNeedElemSpaceHandling(context);
}

void bmx_libxslt_xsltFreeDocumentKeys(xsltDocumentPtr doc) {
	xsltFreeDocumentKeys(doc);
}

void bmx_libxslt_xsltInitCtxtKeys(xsltTransformContextPtr context, xsltDocumentPtr doc) {
	xsltInitCtxtKeys(context, doc);
}

xmlNodeSetPtr bmx_libxslt_xsltGetKey(xsltTransformContextPtr context, BBString * name, BBString * nameURI, BBString * value) {
	char * n = bbStringToUTF8StringOrNull(name);
	char * u = bbStringToUTF8StringOrNull(nameURI);
	char * v = bbStringToUTF8String(value);
	xmlNodeSetPtr ns = xsltGetKey(context, (xmlChar *)n, (xmlChar *)u, (xmlChar *)v);
	bbMemFree(v);
	bbMemFree(u);
	bbMemFree(n);
	return ns;
}


xsltTemplatePtr bmx_libxslt_xsltFindTemplate(xsltTransformContextPtr context, BBString * name, BBString * nameURI) {
	char * n = bbStringToUTF8String(name);
	char * u = bbStringToUTF8StringOrNull(nameURI);
	xsltTemplatePtr t = xsltFindTemplate(context, (xmlChar *)n, (xmlChar *)u);
	bbMemFree(u);
	bbMemFree(n);
	return t;
}


void bmx_libxslt_xsltNamespaceAlias(xsltStylesheetPtr sheet, xmlNodePtr node) {
	xsltNamespaceAlias(sheet, node);
}

void bmx_libxslt_xsltFreeNamespaceAliasHashes(xsltStylesheetPtr sheet) {
	xsltFreeNamespaceAliasHashes(sheet);
}

xmlNsPtr bmx_libxslt_xsltGetNamespace(xsltTransformContextPtr context, xmlNodePtr cur, xmlNsPtr ns, xmlNodePtr out) {
	return xsltGetNamespace(context, cur, ns, out);
}

xmlNsPtr bmx_libxslt_xsltGetPlainNamespace(xsltTransformContextPtr context, xmlNodePtr cur, xmlNsPtr ns, xmlNodePtr out) {
	return xsltGetPlainNamespace(context, cur, ns, out);
}

xmlNsPtr bmx_libxslt_xsltGetSpecialNamespace(xsltTransformContextPtr context, xmlNodePtr cur, BBString * uri, BBString * prefix, xmlNodePtr out) {
	char * u = bbStringToUTF8StringOrNull(uri);
	char * p = bbStringToUTF8StringOrNull(prefix);
	xmlNsPtr ns = xsltGetSpecialNamespace(context, cur, (xmlChar *)u, (xmlChar *)p, out);
	bbMemFree(p);
	bbMemFree(u);
	return ns;
}


xsltElemPreCompPtr bmx_libxslt_xsltDocumentComp(xsltStylesheetPtr style, xmlNodePtr inst, xsltTransformFunction func) {
	return xsltDocumentComp(style, inst, func);
}

void bmx_libxslt_xsltFreeStylePreComps(xsltStylesheetPtr sheet) {
	xsltFreeStylePreComps(sheet);
}

void bmx_libxslt_xsltStylePreCompute(xsltStylesheetPtr sheet, xmlNodePtr node) {
	xsltStylePreCompute(sheet, node);
}


int bmx_libxslt_xsltAllocateExtra(xsltStylesheetPtr sheet) {
	return xsltAllocateExtra(sheet);
}

int bmx_libxslt_xsltAllocateExtraCtxt(xsltTransformContextPtr context) {
	return xsltAllocateExtraCtxt(context);
}

void bmx_libxslt_xsltCompileAttr(xsltStylesheetPtr sheet, xmlAttrPtr attr) {
	xsltCompileAttr(sheet, attr);
}

xmlDocPtr bmx_libxslt_xsltCreateRVT(xsltTransformContextPtr context) {
	return xsltCreateRVT(context);
}

BBString * bmx_libxslt_xsltEvalAVT(xsltTransformContextPtr context, BBString * avt, xmlNodePtr node) {
	char * a = bbStringToUTF8String(avt);
	BBString * s = bbStringFromXmlChar(xsltEvalAVT(context, (xmlChar *)a, node));
	bbMemFree(a);
	return s;
}


void bmx_libxslt_xsltFreeRVTs(xsltTransformContextPtr context) {
	xsltFreeRVTs(context);
}

int bmx_libxslt_xsltRegisterPersistRVT(xsltTransformContextPtr context, xmlDocPtr rvt) {
	return xsltRegisterPersistRVT(context, rvt);
}

int bmx_libxslt_xsltRegisterTmpRVT(xsltTransformContextPtr context, xmlDocPtr rvt) {
	return xsltRegisterTmpRVT(context, rvt);
}


void bmx_libxslt_xsltFreeCompMatchList(xsltCompMatchPtr comp) {
	xsltFreeCompMatchList(comp);
}

void bmx_libxslt_xsltFreeTemplateHashes(xsltStylesheetPtr sheet) {
	xsltFreeTemplateHashes(sheet);
}

int bmx_libxslt_xsltAddTemplate(xsltStylesheetPtr sheet, xsltTemplatePtr cur, BBString * Mode, BBString * modeURI) {
	char * m = bbStringToUTF8StringOrNull(Mode);
	char * u = bbStringToUTF8StringOrNull(modeURI);
	int ret = xsltAddTemplate(sheet, cur, (xmlChar *)m, (xmlChar *)u);
	bbMemFree(u);
	bbMemFree(m);
	return ret;
}

void bmx_libxslt_xsltCleanupTemplates(xsltStylesheetPtr sheet) {
	xsltCleanupTemplates(sheet);
}

xsltCompMatchPtr bmx_libxslt_xsltCompilePattern(BBString * pattern, xmlDocPtr doc, xmlNodePtr node, xsltStylesheetPtr sheet, xsltTransformContextPtr context) {
	char * p = bbStringToUTF8String(pattern);
	xsltCompMatchPtr m = xsltCompilePattern((xmlChar *)p, doc, node, sheet, context);
	bbMemFree(p);
	return m;
}

xsltTemplatePtr bmx_libxslt_xsltGetTemplate(xsltTransformContextPtr context, xmlNodePtr node, xsltStylesheetPtr style) {
	return xsltGetTemplate(context, node, style);
}

int bmx_libxslt_xsltTestCompMatchList(xsltTransformContextPtr context, xmlNodePtr node, xsltCompMatchPtr comp) {
	return xsltTestCompMatchList(context, node, comp);
}


void bmx_libxslt_xsltMessage(xsltTransformContextPtr context, xmlNodePtr node, xmlNodePtr inst) {
	xsltMessage(context, node, inst);
}

void bmx_libxslt_xsltPrintErrorContext(xsltTransformContextPtr context, xsltStylesheetPtr style, xmlNodePtr node) {
	xsltPrintErrorContext(context, style, node);
}

int bmx_libxslt_xsltSaveResultToFilename(BBString * url, xmlDocPtr result, xsltStylesheetPtr style, int compression) {
	char * u = bbStringToUTF8String(url);
	int ret = xsltSaveResultToFilename(u, result, style, compression);
	bbMemFree(u);
	return ret;
}


xmlAttrPtr bmx_libxslt_xsltAttrListTemplateProcess(xsltTransformContextPtr context, xmlNodePtr target, xmlAttrPtr cur) {
	return xsltAttrListTemplateProcess(context, target, cur);
}

xmlAttrPtr bmx_libxslt_xsltAttrTemplateProcess(xsltTransformContextPtr context, xmlNodePtr target, xmlAttrPtr cur) {
	return xsltAttrTemplateProcess(context, target, cur);
}

BBString * bmx_libxslt_xsltAttrTemplateValueProcess(xsltTransformContextPtr context, BBString * value) {
	char * v = bbStringToUTF8String(value);
	BBString * s = bbStringFromXmlChar(xsltAttrTemplateValueProcess(context, (xmlChar *)v));
	bbMemFree(v);
	return s;
}

BBString * bmx_libxslt_xsltAttrTemplateValueProcessNode(xsltTransformContextPtr context, BBString * value, xmlNodePtr node) {
	char * v = bbStringToUTF8String(value);
	BBString * s = bbStringFromXmlChar(xsltAttrTemplateValueProcessNode(context, (xmlChar *)v, node));
	bbMemFree(v);
	return s;
}

BBString * bmx_libxslt_xsltEvalAttrValueTemplate(xsltTransformContextPtr context, xmlNodePtr node, BBString * name, BBString * ns) {
	char * n = bbStringToUTF8String(name);
	char * nss = bbStringToUTF8String(ns);
	BBString * s = bbStringFromXmlChar(xsltEvalAttrValueTemplate(context, node, (xmlChar *)n, (xmlChar *)nss));
	bbMemFree(nss);
	bbMemFree(n);
	return s;
}

BBString * bmx_libxslt_xsltEvalStaticAttrValueTemplate(xsltStylesheetPtr style, xmlNodePtr node, BBString * name, BBString * ns, int * found) {
	char * n = bbStringToUTF8String(name);
	char * nss = bbStringToUTF8String(ns);
	BBString * s = bbStringFromXmlChar((xmlChar *)xsltEvalStaticAttrValueTemplate(style, node, (xmlChar *)n, (xmlChar *)nss, found));
	bbMemFree(nss);
	bbMemFree(n);
	return s;
}

BBString * bmx_libxslt_xsltEvalTemplateString(xsltTransformContextPtr context, xmlNodePtr node, xmlNodePtr parent) {
	return bbStringFromXmlChar(xsltEvalTemplateString(context, node, parent));
}

BBString * bmx_libxslt_xsltEvalXPathString(xsltTransformContextPtr context, xmlXPathCompExprPtr comp) {
	return bbStringFromXmlChar(xsltEvalXPathString(context, comp));
}


int bmx_libxslt_xsltelemprecomp_type(xsltElemPreCompPtr handle) {
	return handle->type;
}

