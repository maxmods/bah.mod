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
	void _bah_expat_TXMLParser__EndElementHandler(BBObject * handle, BBString * name);
	void _bah_expat_TXMLParser__CharacterDataHandler(BBObject * handle, BBString * text);
	void _bah_expat_TXMLParser__ProcessingInstructionHandler(BBObject * handle, BBString * target, BBString * data);


	XML_Parser bmx_expat_XML_ParserCreate(BBString * encoding);
	XML_Status bmx_expat_XML_Parse(XML_Parser parser, BBString * text, int isFinal);
	void bmx_expat_XML_SetStartElementHandler(XML_Parser parser);
	void bmx_expat_XML_SetEndElementHandler(XML_Parser parser);
	void bmx_expat_XML_GetParsingStatus(XML_Parser parser, XML_Parsing * parsing, int * finalBuffer);
	void bmx_expat_XML_SetElementHandler(XML_Parser parser, int hasStart, int hasEnd);
	void bmx_expat_XML_SetCharacterDataHandler(XML_Parser parser);
	void bmx_expat_XML_SetProcessingInstructionHandler(XML_Parser parser);

	BBString * bmx_expat_XML_ErrorString(enum XML_Error code);
	BBString * bmx_expat_XML_ExpatVersion();
	void bmx_expat_XML_ExpatVersionInfo(int * major, int * minor, int * micro);
	XML_Status bmx_expat_XML_SetBase(XML_Parser parser, BBString * base);
	BBString * bmx_expat_XML_GetBase(XML_Parser parser);
	XML_Status bmx_expat_XML_SetEncoding(XML_Parser parser, BBString * encoding);

	void XMLCALL bmx_expat_StartElementHandler(void *userData, const char *name, const char **atts);
	void XMLCALL bmx_expat_EndElementHandler(void *userData, const char *name);
	void XMLCALL bmx_expat_CharacterDataHandler(void *userData, const char *text, int len);
	void XMLCALL bmx_expat_ProcessingInstructionHandler(void *userData, const char *target, const char *data);

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

BBString * bmx_expat_XML_ExpatVersion() {
	return bbStringFromCString(XML_ExpatVersion());
}

void bmx_expat_XML_ExpatVersionInfo(int * major, int * minor, int * micro) {
	XML_Expat_Version v = XML_ExpatVersionInfo();
	*major = v.major;
	*minor = v.minor;
	*micro = v.micro;
}

XML_Status bmx_expat_XML_SetBase(XML_Parser parser, BBString * base) {

	char * c = bbStringToUTF8String(base);

	XML_Status res = XML_SetBase(parser, c);
	
	bbMemFree(c);
	return res;
}

BBString * bmx_expat_XML_GetBase(XML_Parser parser) {
	return bbStringFromCString(XML_GetBase(parser));
}

XML_Status bmx_expat_XML_SetEncoding(XML_Parser parser, BBString * encoding) {

	char * c = bbStringToUTF8String(encoding);

	XML_Status res = XML_SetEncoding(parser, c);
	
	bbMemFree(c);
	return res;
}

void bmx_expat_XML_GetParsingStatus(XML_Parser parser, XML_Parsing * parsing, int * finalBuffer) {
	XML_ParsingStatus status;
	XML_GetParsingStatus(parser, &status);
	*parsing = status.parsing;
	*finalBuffer = status.finalBuffer;
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

void bmx_expat_XML_SetEndElementHandler(XML_Parser parser) {
	XML_SetEndElementHandler(parser, bmx_expat_EndElementHandler);
}

void XMLCALL bmx_expat_EndElementHandler(void *userData, const char *name) {

	_bah_expat_TXMLParser__EndElementHandler((BBObject *)userData, bbStringFromUTF8String(name));

}

void bmx_expat_XML_SetElementHandler(XML_Parser parser, int hasStart, int hasEnd) {
	if (hasStart) {
		if (hasEnd) {
			XML_SetElementHandler(parser, bmx_expat_StartElementHandler, bmx_expat_EndElementHandler);
		} else {
			XML_SetElementHandler(parser, bmx_expat_StartElementHandler, NULL);
		}
	} else {
		if (hasEnd) {
			XML_SetElementHandler(parser, NULL, bmx_expat_EndElementHandler);
		} else {
			XML_SetElementHandler(parser, NULL, NULL);
		}
	}
}

void XMLCALL bmx_expat_CharacterDataHandler(void *userData, const char *text, int len) {
	char buf[len+1];
	memcpy(&buf, text, len);
	_bah_expat_TXMLParser__CharacterDataHandler((BBObject *)userData, bbStringFromUTF8String((char*)&buf));

}

void bmx_expat_XML_SetCharacterDataHandler(XML_Parser parser) {
	XML_SetCharacterDataHandler(parser, bmx_expat_CharacterDataHandler);
}

void XMLCALL bmx_expat_ProcessingInstructionHandler(void *userData, const char *target, const char *data) {
	
	_bah_expat_TXMLParser__ProcessingInstructionHandler((BBObject *)userData, bbStringFromUTF8String(target), bbStringFromUTF8String(data));

}

void bmx_expat_XML_SetProcessingInstructionHandler(XML_Parser parser) {
	XML_SetProcessingInstructionHandler(parser, bmx_expat_ProcessingInstructionHandler);
}



