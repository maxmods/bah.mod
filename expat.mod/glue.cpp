/*
  Copyright (c) 2007-2014 Bruce A Henderson
 
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

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

#include "blitz.h"
	void CB_PREF(bah_expat_TXMLParser__StartElementHandler)(BBObject * handle, BBString * name, BBArray * attrs);
	void CB_PREF(bah_expat_TXMLParser__EndElementHandler)(BBObject * handle, BBString * name);
	void CB_PREF(bah_expat_TXMLParser__CharacterDataHandler)(BBObject * handle, BBString * text);
	void CB_PREF(bah_expat_TXMLParser__ProcessingInstructionHandler)(BBObject * handle, BBString * target, BBString * data);
	void CB_PREF(bah_expat_TXMLParser__CommentHandler)(BBObject * handle, BBString *data);
	void CB_PREF(bah_expat_TXMLParser__StartCdataSectionHandler)(BBObject * handle);
	void CB_PREF(bah_expat_TXMLParser__EndCdataSectionHandler)(BBObject * handle);
	void CB_PREF(bah_expat_TXMLParser__DefaultHandler)(BBObject * handle, BBString *s);
	void CB_PREF(bah_expat_TXMLParser__DefaultHandlerExpand)(BBObject * handle, BBString *s);
	void CB_PREF(bah_expat_TXMLParser__SkippedEntityHandler)(BBObject * handle, BBString *entityName, int is_parameter_entity);
	void CB_PREF(bah_expat_TXMLParser__StartNamespaceDeclHandler)(BBObject * handle, BBString *prefix, BBString *uri);
	void CB_PREF(bah_expat_TXMLParser__EndNamespaceDeclHandler)(BBObject * handle, BBString *prefix);
	void CB_PREF(bah_expat_TXMLParser__XmlDeclHandler)(BBObject * handle, BBString *version, BBString *encoding, int standalone);
	void CB_PREF(bah_expat_TXMLParser__StartDoctypeDeclHandler)(BBObject * handle, BBString *doctypeName, BBString *sysid, BBString *pubid,
		int has_internal_subset);
	void CB_PREF(bah_expat_TXMLParser__EndDoctypeDeclHandler)(BBObject * handle);
	void CB_PREF(bah_expat_TXMLParser__AttlistDeclHandler)(BBObject * handle, BBString *elname, BBString *attname, BBString *att_type,
		BBString *dflt, int isrequired);
	void CB_PREF(bah_expat_TXMLParser__EntityDeclHandler)(BBObject * handle, BBString *entityName, int is_parameter_entity, BBString *value, BBString *base,
		BBString *systemId, BBString *publicId, BBString *notationName);
	void CB_PREF(bah_expat_TXMLParser__NotationDeclHandler)(BBObject * handle, BBString *notationName, BBString *base, BBString *systemId,
		BBString *publicId);
	int CB_PREF(bah_expat_TXMLParser__NotStandaloneHandler)(BBObject * handle);

	XML_Parser bmx_expat_XML_ParserCreate(BBString * encoding);
	XML_Status bmx_expat_XML_Parse(XML_Parser parser, BBString * text, int isFinal);
	void bmx_expat_XML_SetStartElementHandler(XML_Parser parser);
	void bmx_expat_XML_SetEndElementHandler(XML_Parser parser);
	void bmx_expat_XML_GetParsingStatus(XML_Parser parser, XML_Parsing * parsing, int * finalBuffer);
	void bmx_expat_XML_SetElementHandler(XML_Parser parser, int hasStart, int hasEnd);
	void bmx_expat_XML_SetCharacterDataHandler(XML_Parser parser);
	void bmx_expat_XML_SetProcessingInstructionHandler(XML_Parser parser);
	void bmx_expat_XML_SetCommentHandler(XML_Parser parser);
	void bmx_expat_XML_SetStartCdataSectionHandler(XML_Parser parser);
	void bmx_expat_XML_SetEndCdataSectionHandler(XML_Parser parser);
	void bmx_expat_XML_SetCdataSectionHandler(XML_Parser parser, int hasStart, int hasEnd);
	void bmx_expat_XML_SetDefaultHandler(XML_Parser parser);
	void bmx_expat_XML_SetDefaultHandlerExpand(XML_Parser parser);
	void bmx_expat_XML_SetSkippedEntityHandler(XML_Parser parser);
	void bmx_expat_XML_SetStartNamespaceDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetEndNamespaceDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetNamespaceDeclHandler(XML_Parser parser, int hasStart, int hasEnd);
	void bmx_expat_XML_SetXmlDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetStartDoctypeDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetEndDoctypeDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetDoctypeDeclHandler(XML_Parser parser, int hasStart, int hasEnd);
	void bmx_expat_XML_SetAttlistDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetEntityDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetNotationDeclHandler(XML_Parser parser);
	void bmx_expat_XML_SetNotStandaloneHandler(XML_Parser parser);

	
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
	void XMLCALL bmx_expat_CommentHandler(void *userData, const char *data);
	void XMLCALL bmx_expat_StartCdataSectionHandler(void *userData);
	void XMLCALL bmx_expat_EndCdataSectionHandler(void *userData);
	void XMLCALL bmx_expat_DefaultHandler(void *userData, const char *s, int len);
	void XMLCALL bmx_expat_DefaultHandlerExpand(void *userData, const char *s, int len);
	void XMLCALL bmx_expat_SkippedEntityHandler(void *userData, const char *entityName, int is_parameter_entity);
	void XMLCALL bmx_expat_StartNamespaceDeclHandler(void *userData, const char *prefix, const char *uri);
	void XMLCALL bmx_expat_EndNamespaceDeclHandler(void *userData, const char *prefix);
	void XMLCALL bmx_expat_XmlDeclHandler(void *userData, const char *version, const char *encoding, int standalone);
	void XMLCALL bmx_expat_StartDoctypeDeclHandler(void *userData, const char *doctypeName, const char *sysid, const char *pubid,
		int has_internal_subset);
	void XMLCALL bmx_expat_EndDoctypeDeclHandler(void *userData);
	void XMLCALL bmx_expat_AttlistDeclHandler(void *userData, const char *elname, const char *attname, const char *att_type,
		const char *dflt, int isrequired);
	void XMLCALL bmx_expat_EntityDeclHandler(void *userData, const char *entityName, int is_parameter_entity, const char *value,
		int value_length, const char *base, const char *systemId, const char *publicId, const char *notationName);
	void XMLCALL bmx_expat_NotationDeclHandler(void *userData, const char *notationName, const char *base, const char *systemId,
		const char *publicId);
	int XMLCALL bmx_expat_NotStandaloneHandler(void *userData);

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
		s[i] = bbStringFromUTF8String(atts[i]);
		BBRETAIN(s[i]);
	}

	CB_PREF(bah_expat_TXMLParser__StartElementHandler)((BBObject *)userData, bbStringFromUTF8String(name), p);

}

void bmx_expat_XML_SetEndElementHandler(XML_Parser parser) {
	XML_SetEndElementHandler(parser, bmx_expat_EndElementHandler);
}

void XMLCALL bmx_expat_EndElementHandler(void *userData, const char *name) {

	CB_PREF(bah_expat_TXMLParser__EndElementHandler)((BBObject *)userData, bbStringFromUTF8String(name));

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
	CB_PREF(bah_expat_TXMLParser__CharacterDataHandler)((BBObject *)userData, bbStringFromUTF8String((char*)&buf));

}

void bmx_expat_XML_SetCharacterDataHandler(XML_Parser parser) {
	XML_SetCharacterDataHandler(parser, bmx_expat_CharacterDataHandler);
}

void XMLCALL bmx_expat_ProcessingInstructionHandler(void *userData, const char *target, const char *data) {
	
	CB_PREF(bah_expat_TXMLParser__ProcessingInstructionHandler)((BBObject *)userData, bbStringFromUTF8String(target), bbStringFromUTF8String(data));

}

void bmx_expat_XML_SetProcessingInstructionHandler(XML_Parser parser) {
	XML_SetProcessingInstructionHandler(parser, bmx_expat_ProcessingInstructionHandler);
}

void XMLCALL bmx_expat_CommentHandler(void *userData, const char *data) {
	CB_PREF(bah_expat_TXMLParser__CommentHandler)((BBObject *)userData, bbStringFromUTF8String(data));
}

void bmx_expat_XML_SetCommentHandler(XML_Parser parser) {
	XML_SetCommentHandler(parser, bmx_expat_CommentHandler);
}

void XMLCALL bmx_expat_StartCdataSectionHandler(void *userData) {
	CB_PREF(bah_expat_TXMLParser__StartCdataSectionHandler)((BBObject *)userData);
}

void bmx_expat_XML_SetStartCdataSectionHandler(XML_Parser parser) {
	XML_SetStartCdataSectionHandler(parser, bmx_expat_StartCdataSectionHandler);
}

void XMLCALL bmx_expat_EndCdataSectionHandler(void *userData) {
	CB_PREF(bah_expat_TXMLParser__EndCdataSectionHandler)((BBObject *)userData);
}

void bmx_expat_XML_SetEndCdataSectionHandler(XML_Parser parser) {
	XML_SetEndCdataSectionHandler(parser, bmx_expat_EndCdataSectionHandler);
}

void bmx_expat_XML_SetCdataSectionHandler(XML_Parser parser, int hasStart, int hasEnd) {
	if (hasStart) {
		if (hasEnd) {
			XML_SetCdataSectionHandler(parser, bmx_expat_StartCdataSectionHandler, bmx_expat_EndCdataSectionHandler);
		} else {
			XML_SetCdataSectionHandler(parser, bmx_expat_StartCdataSectionHandler, NULL);
		}
	} else {
		if (hasEnd) {
			XML_SetCdataSectionHandler(parser, NULL, bmx_expat_EndCdataSectionHandler);
		} else {
			XML_SetCdataSectionHandler(parser, NULL, NULL);
		}
	}
}

void XMLCALL bmx_expat_DefaultHandler(void *userData, const char *s, int len) {
	char buf[len+1];
	memcpy(&buf, s, len);
	CB_PREF(bah_expat_TXMLParser__DefaultHandler)((BBObject *)userData, bbStringFromUTF8String((char*)&buf));
}

void bmx_expat_XML_SetDefaultHandler(XML_Parser parser) {
	XML_SetDefaultHandler(parser, bmx_expat_DefaultHandler);
}

void XMLCALL bmx_expat_DefaultHandlerExpand(void *userData, const char *s, int len) {
	char buf[len+1];
	memcpy(&buf, s, len);
	CB_PREF(bah_expat_TXMLParser__DefaultHandlerExpand)((BBObject *)userData, bbStringFromUTF8String((char*)&buf));
}

void bmx_expat_XML_SetDefaultHandlerExpand(XML_Parser parser) {
	XML_SetDefaultHandlerExpand(parser, bmx_expat_DefaultHandlerExpand);
}

void XMLCALL bmx_expat_SkippedEntityHandler(void *userData, const char *entityName, int is_parameter_entity) {
	CB_PREF(bah_expat_TXMLParser__SkippedEntityHandler)((BBObject *)userData, bbStringFromUTF8String(entityName), is_parameter_entity);
}

void bmx_expat_XML_SetSkippedEntityHandler(XML_Parser parser) {
	XML_SetSkippedEntityHandler(parser, bmx_expat_SkippedEntityHandler);
}

void XMLCALL bmx_expat_StartNamespaceDeclHandler(void *userData, const char *prefix, const char *uri) {
	CB_PREF(bah_expat_TXMLParser__StartNamespaceDeclHandler)((BBObject *)userData, bbStringFromUTF8String(prefix), bbStringFromUTF8String(uri));
}

void bmx_expat_XML_SetStartNamespaceDeclHandler(XML_Parser parser) {
	XML_SetStartNamespaceDeclHandler(parser, bmx_expat_StartNamespaceDeclHandler);
}

void XMLCALL bmx_expat_EndNamespaceDeclHandler(void *userData, const char *prefix) {
	CB_PREF(bah_expat_TXMLParser__EndNamespaceDeclHandler)((BBObject *)userData, bbStringFromUTF8String(prefix));
}

void bmx_expat_XML_SetEndNamespaceDeclHandler(XML_Parser parser) {
	XML_SetEndNamespaceDeclHandler(parser, bmx_expat_EndNamespaceDeclHandler);
}

void bmx_expat_XML_SetNamespaceDeclHandler(XML_Parser parser, int hasStart, int hasEnd) {
	if (hasStart) {
		if (hasEnd) {
			XML_SetNamespaceDeclHandler(parser, bmx_expat_StartNamespaceDeclHandler, bmx_expat_EndNamespaceDeclHandler);
		} else {
			XML_SetNamespaceDeclHandler(parser, bmx_expat_StartNamespaceDeclHandler, NULL);
		}
	} else {
		if (hasEnd) {
			XML_SetNamespaceDeclHandler(parser, NULL, bmx_expat_EndNamespaceDeclHandler);
		} else {
			XML_SetNamespaceDeclHandler(parser, NULL, NULL);
		}
	}
}

void XMLCALL bmx_expat_XmlDeclHandler(void *userData, const char *version, const char *encoding, int standalone) {
	CB_PREF(bah_expat_TXMLParser__XmlDeclHandler)((BBObject *)userData, bbStringFromUTF8String(version), bbStringFromUTF8String(encoding),
		standalone);
}

void bmx_expat_XML_SetXmlDeclHandler(XML_Parser parser) {
	XML_SetXmlDeclHandler(parser, bmx_expat_XmlDeclHandler);
}

void XMLCALL bmx_expat_StartDoctypeDeclHandler(void *userData, const char *doctypeName, const char *sysid, const char *pubid,
		int has_internal_subset) {


	CB_PREF(bah_expat_TXMLParser__StartDoctypeDeclHandler)((BBObject *)userData, bbStringFromUTF8String(doctypeName),
		bbStringFromUTF8String(sysid), bbStringFromUTF8String(pubid), has_internal_subset);

}

void bmx_expat_XML_SetStartDoctypeDeclHandler(XML_Parser parser) {
	XML_SetStartDoctypeDeclHandler(parser, bmx_expat_StartDoctypeDeclHandler);
}

void XMLCALL bmx_expat_EndDoctypeDeclHandler(void *userData) {
	CB_PREF(bah_expat_TXMLParser__EndDoctypeDeclHandler)((BBObject *)userData);
}

void bmx_expat_XML_SetEndDoctypeDeclHandler(XML_Parser parser) {
	XML_SetEndDoctypeDeclHandler(parser, bmx_expat_EndDoctypeDeclHandler);
}

void bmx_expat_XML_SetDoctypeDeclHandler(XML_Parser parser, int hasStart, int hasEnd) {
	if (hasStart) {
		if (hasEnd) {
			XML_SetDoctypeDeclHandler(parser, bmx_expat_StartDoctypeDeclHandler, bmx_expat_EndDoctypeDeclHandler);
		} else {
			XML_SetDoctypeDeclHandler(parser, bmx_expat_StartDoctypeDeclHandler, NULL);
		}
	} else {
		if (hasEnd) {
			XML_SetDoctypeDeclHandler(parser, NULL, bmx_expat_EndDoctypeDeclHandler);
		} else {
			XML_SetDoctypeDeclHandler(parser, NULL, NULL);
		}
	}
}

void XMLCALL bmx_expat_AttlistDeclHandler(void *userData, const char *elname, const char *attname, const char *att_type,
		const char *dflt, int isrequired) {

	CB_PREF(bah_expat_TXMLParser__AttlistDeclHandler)((BBObject *)userData, bbStringFromUTF8String(elname), bbStringFromUTF8String(attname),
		bbStringFromUTF8String(att_type), bbStringFromUTF8String(dflt), isrequired);

}

void bmx_expat_XML_SetAttlistDeclHandler(XML_Parser parser) {
	XML_SetAttlistDeclHandler(parser, bmx_expat_AttlistDeclHandler);
}

void XMLCALL bmx_expat_EntityDeclHandler(void *userData, const char *entityName, int is_parameter_entity, const char *value,
		int value_length, const char *base, const char *systemId, const char *publicId, const char *notationName) {

	if (value_length > 0) {
		char buf[value_length +1];
		memcpy(&buf, value, value_length);

		CB_PREF(bah_expat_TXMLParser__EntityDeclHandler)((BBObject *)userData, bbStringFromUTF8String(entityName), is_parameter_entity,
			bbStringFromUTF8String((char*)&buf), bbStringFromUTF8String(base), bbStringFromUTF8String(systemId),
			bbStringFromUTF8String(publicId), bbStringFromUTF8String(notationName));
	} else {
		CB_PREF(bah_expat_TXMLParser__EntityDeclHandler)((BBObject *)userData, bbStringFromUTF8String(entityName), is_parameter_entity,
			&bbEmptyString, bbStringFromUTF8String(base), bbStringFromUTF8String(systemId),
			bbStringFromUTF8String(publicId), bbStringFromUTF8String(notationName));
	}

}

void bmx_expat_XML_SetEntityDeclHandler(XML_Parser parser) {
	XML_SetEntityDeclHandler(parser, bmx_expat_EntityDeclHandler);
}

void XMLCALL bmx_expat_NotationDeclHandler(void *userData, const char *notationName, const char *base, const char *systemId,
		const char *publicId) {


	CB_PREF(bah_expat_TXMLParser__NotationDeclHandler)((BBObject *)userData, bbStringFromUTF8String(notationName),
		bbStringFromUTF8String(base), bbStringFromUTF8String(systemId), bbStringFromUTF8String(publicId));

}

void bmx_expat_XML_SetNotationDeclHandler(XML_Parser parser) {
	XML_SetNotationDeclHandler(parser, bmx_expat_NotationDeclHandler);
}

int XMLCALL bmx_expat_NotStandaloneHandler(void *userData) {
	return CB_PREF(bah_expat_TXMLParser__NotStandaloneHandler)((BBObject *)userData);
}

void bmx_expat_XML_SetNotStandaloneHandler(XML_Parser parser) {
	XML_SetNotStandaloneHandler(parser, bmx_expat_NotStandaloneHandler);
}



