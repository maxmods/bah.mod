/*
  Copyright (c) 2007-2009 Bruce A Henderson
 
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

#include "expat.h"

extern "C" {

#include "blitz.h"

	void _bah_expat_TXMLParser__StartElementHandler(BBObject * handle, BBString * name, BBArray * attrs);


	XML_Parser bmx_expat_XML_ParserCreate(BBString * encoding);
	XML_Status bmx_expat_XML_Parse(XML_Parser parser, BBString * text, int isFinal);
	void bmx_expat_XML_SetStartElementHandler(XML_Parser parser);

	BBString * bmx_expat_XML_ErrorString(enum XML_Error code);

	void XMLCALL bmx_expat_StartElementHandler(void *userData, const char *name, const char **atts);

}

// *******************************************************************


XML_Parser bmx_expat_XML_ParserCreate(BBString * encoding) {
	char * c = NULL;
	if (encoding != &bbEmptyString) {
		c = bbStringToCString(encoding);
	}
	
	XML_Parser parser = XML_ParserCreate(c);
	
	if (c) bbMemFree(c);
	
	return parser;
}

XML_Status bmx_expat_XML_Parse(XML_Parser parser, BBString * text, int isFinal) {

	char * c = bbStringToUTF8String(text);

	XML_Status res = XML_Parse(parser, c, strlen(c), isFinal);
	
	bbMemFree(c);
	return res;
}

BBString * bmx_expat_XML_ErrorString(enum XML_Error code) {
	return bbStringFromCString(XML_ErrorString(code));
}

// *******************************************************************

void bmx_expat_XML_SetStartElementHandler(XML_Parser parser) {
	XML_SetStartElementHandler(parser, bmx_expat_StartElementHandler);
}

void XMLCALL bmx_expat_StartElementHandler(void *userData, const char *name, const char **atts) {

	int n;
	for (n = 0; atts[n]; n += 2) {}

	BBArray *p = bbArrayNew1D( "$",n );
	BBString **s = (BBString**)BBARRAYDATA( p,p->dims );
	for( int i = 0; i < n; ++i ){
		s[i] = bbStringFromUTF8String(atts[n]);
		BBRETAIN(s[i]);
	}

	_bah_expat_TXMLParser__StartElementHandler((BBObject *)userData, bbStringFromUTF8String(name), p);

}


