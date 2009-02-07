/*
  Copyright (c) 2009 Bruce A Henderson
 
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

#include <common.h>
#include <fribidi.h>



class MaxRTLConverter;

extern "C" {

#include "blitz.h"

	MaxRTLConverter * bmx_rtlconverter_new();
	BBString * bmx_rtlconverter_convert(MaxRTLConverter * converter, BBString * text);

}

#define ALLOCATE(tp,ln) ((tp *) fribidi_malloc (sizeof (tp) * (ln)))


#define MAX_STR_LEN 65000

#define UNI_SUR_HIGH_START  0xD800
#define UNI_SUR_HIGH_END    0xDBFF
#define UNI_SUR_LOW_START   0xDC00
#define UNI_SUR_LOW_END     0xDFFF

#define halfBase 0x0010000
#define halfMask 0x3FF
#define halfShift 10

// ***************************************

class MaxRTLConverter
{
public:
	MaxRTLConverter()
	{
		base = FRIBIDI_PAR_ON;
	}
	
	~MaxRTLConverter()
	{
	}
	
	BBString * convert(BBString * text) {
	
		target = &logical[0];
		len = 0;

		BBChar *p = text->buf;
		int i = text->length;
	
	
		// ++++++++++
		// Convert to UTF-32
		// ++++++++++
		while (i--) {
			ch = *p++;
	
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
				ch2 = *p;
		
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
			     }
	
				p++;
				i--;
			}
		
			*target++ = ch;
			len++;	
		}
		
		*target = 0;
	
		visual = ALLOCATE (FriBidiChar, len + 1);
		fribidi_boolean log2vis = fribidi_log2vis(logical, len, &base, visual, 0, 0, 0);
	
		p = &outstring[0];
		FriBidiChar *v = visual;
		int plen = 0;

		// ++++++++++
		// Convert to UTF-16
		// ++++++++++
		for (i = 0; i < len; i++) {
			ch = *v++;
	
			plen++;
			if (ch < 0xFFFF) {
				*p++ = (BBChar)ch;
			} else {
				ch -= halfBase;
				*p++ = (BBChar)((ch >> halfShift) + UNI_SUR_HIGH_START);
				*p++ = (BBChar)((ch & halfMask) + UNI_SUR_LOW_START);
				plen++;
			}
		}

		free (visual);
		return bbStringFromShorts(outstring, plen);

	}

private:

	FriBidiChar logical[MAX_STR_LEN];
	BBChar outstring[MAX_STR_LEN];

	FriBidiChar * target;
	FriBidiChar ch, ch2;
	FriBidiStrIndex len;
	FriBidiChar *visual;

	FriBidiParType base;
};

// ***************************************

MaxRTLConverter * bmx_rtlconverter_new() {
	return new MaxRTLConverter();
}

BBString * bmx_rtlconverter_convert(MaxRTLConverter * converter, BBString * text) {
	return converter->convert(text);
}

