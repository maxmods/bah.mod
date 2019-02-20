/*
  Copyright (c) 2016-2019 Bruce A Henderson
 
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

#include "brl.mod/blitz.mod/blitz.h"
#ifndef BMX_NG
extern unsigned short maxToLowerData[];
extern unsigned short maxToUpperData[];
#define bbToLowerData maxToLowerData
#define bbToUpperData maxToUpperData
#else
#include "brl.mod/blitz.mod/blitz_unicode.h"
#endif

struct MaxStringBuilder {
	BBChar * buffer;
	int count;
	int capacity;
};

struct MaxSplitBuffer {
	struct MaxStringBuilder * buffer;
	int count;
	int * startIndex;
	int * endIndex;
};

void bmx_stringbuilder_free(struct MaxStringBuilder * buf) {
	free(buf->buffer);
	free(buf);
}

struct MaxStringBuilder * bmx_stringbuilder_new(int initial) {
	struct MaxStringBuilder * buf = malloc(sizeof(struct MaxStringBuilder));
	
	buf->count = 0;
	buf->capacity = initial;
	buf->buffer = malloc(initial * sizeof(BBChar));
	
	return buf;
}

/* make more capacity if requested size greater */
void bmx_stringbuilder_resize(struct MaxStringBuilder * buf, int size) {
	if (buf->capacity < size) {
		
		if (buf->capacity * 2  > size) {
			size = buf->capacity * 2;
		}
		short * newBuffer = malloc(size * sizeof(BBChar));
		
		/* copy text to new buffer */
		memcpy(newBuffer, buf->buffer, buf->count * sizeof(BBChar));
		
		/* free old buffer */
		free(buf->buffer);
		
		buf->buffer = newBuffer;
		buf->capacity = size;
	}
}

int bmx_stringbuilder_count(struct MaxStringBuilder * buf) {
	return buf->count;
}

int bmx_stringbuilder_capacity(struct MaxStringBuilder * buf) {
	return buf->capacity;
}


void bmx_stringbuilder_setlength(struct MaxStringBuilder * buf, int length) {
	bmx_stringbuilder_resize(buf, length);
	if (length < buf->count) {
		buf->count = length;
	}
}

BBString * bmx_stringbuilder_tostring(struct MaxStringBuilder * buf) {
	if (!buf->count) {
		return &bbEmptyString;
	} else {
		return bbStringFromShorts(buf->buffer, buf->count);
	}
}

void bmx_stringbuilder_append_string(struct MaxStringBuilder * buf, BBString * value) {
	if (value != &bbEmptyString) {
		bmx_stringbuilder_resize(buf, buf->count + value->length);
		BBChar * p = buf->buffer + buf->count;
		memcpy(p, value->buf, value->length * sizeof(BBChar));
		
		buf->count += value->length;
	}	
}

void bmx_stringbuilder_remove(struct MaxStringBuilder * buf, int start, int end) {
	if (start < 0 || start > buf->count || start > end) {
		return;
	}
	
	/* trim end if it is too big */
	if (end > buf->count) {
		end = buf->count;
	}
	
	/* still something to remove ? */
	if (buf->count - end != 0) {
		memcpy(buf->buffer + start, buf->buffer + end, (buf->count - end) * sizeof(BBChar));
	}
	
	buf->count -= end - start;
}

void bmx_stringbuilder_insert(struct MaxStringBuilder * buf, int offset, BBString * value) {
	if (value != &bbEmptyString) {
		if (offset < 0 || offset > buf->count) {
			return;
		}
		
		int length = value->length;
		bmx_stringbuilder_resize(buf, buf->count + length);

		/* make some space for the insertion */
		/* using memmove because data overlaps */
		memmove(buf->buffer + offset + length, buf->buffer + offset, (buf->count - offset) * sizeof(BBChar));
		
		/* insert the string */
		memcpy(buf->buffer + offset, value->buf, length * sizeof(BBChar));
		
		buf->count += length;
	}
}

void bmx_stringbuilder_reverse(struct MaxStringBuilder * buf) {
	int i = buf->count >> 1;
	int n = buf->count - i;
	while (--i >= 0) {
		BBChar c = buf->buffer[i];
		buf->buffer[i] = buf->buffer[n];
		buf->buffer[n] = c;
		n++;
	}
}

BBString * bmx_stringbuilder_substring(struct MaxStringBuilder * buf, int beginIndex, int endIndex) {
	if (!endIndex) {
		endIndex = buf->count;
	}
	
	if (beginIndex < 0 || endIndex > buf->count || endIndex < beginIndex) {
		return &bbEmptyString;
	}
	
	return bbStringFromShorts(buf->buffer + beginIndex, endIndex - beginIndex);
}

void bmx_stringbuilder_append_stringbuffer(struct MaxStringBuilder * buf, struct MaxStringBuilder * other) {
	if (other->count > 0) {
		bmx_stringbuilder_resize(buf, buf->count + other->count);
	
		memcpy(buf->buffer + buf->count, other->buffer, other->count * sizeof(BBChar));
	
		buf->count += other->count;
	}
}

int bmx_stringbuilder_matches(struct MaxStringBuilder * buf, int offset, BBString * subString) {
	int length = subString->length;
	int index = 0;
	while (--length >= 0) {
		if (buf->buffer[offset++] != subString->buf[index++]) {
			return 0;
		}
	}
	return 1;
}

int bmx_stringbuilder_startswith(struct MaxStringBuilder * buf, BBString * subString) {
	if (subString->length <= buf->count) {
		return bmx_stringbuilder_matches(buf, 0, subString);
	}
	return 0;
}

int bmx_stringbuilder_endswith(struct MaxStringBuilder * buf, BBString * subString) {
	if (subString->length <= buf->count) {
		return bmx_stringbuilder_matches(buf, buf->count - subString->length, subString);
	}
}

int bmx_stringbuilder_find(struct MaxStringBuilder * buf, BBString * subString, int startIndex) {
	if (startIndex < 0) {
		startIndex = 0;
	}
	
	int limit = buf->count - subString->length;
	while (startIndex <= limit) {
		if (bmx_stringbuilder_matches(buf, startIndex, subString)) {
			return startIndex;
		}
		startIndex++;
	}
	return -1;
}

int bmx_stringbuilder_findlast(struct MaxStringBuilder * buf, BBString * subString, int startIndex) {
	if (startIndex < 0) {
		startIndex = 0;
	}
	
	startIndex = buf->count - startIndex;

	if (startIndex + subString->length > buf->count) {
		startIndex = buf->count - subString->length;
	}
	
	while (startIndex >= 0) {
		if (bmx_stringbuilder_matches(buf, startIndex, subString)) {
			return startIndex;
		}
		startIndex--;
	}
	return -1;
}

void bmx_stringbuilder_tolower(struct MaxStringBuilder * buf) {
	int i;
	for (i = 0; i < buf->count; i++ ) {
		int c = buf->buffer[i];
		if (c < 192) {
			c = ( c >= 'A' && c <= 'Z') ? (c|32) : c;
		} else {
			int lo = 0, hi = 3828 / 4 - 1;
			while (lo <= hi) {
				int mid = (lo+hi)/2;
				if (c < bbToLowerData[mid*2]) {
					hi = mid-1;
				} else if (c > bbToLowerData[mid*2]) {
					lo = mid + 1;
				} else {
					c = bbToLowerData[mid*2+1];
					break;
				}
			}
		}
		buf->buffer[i]=c;
	}
}

void bmx_stringbuilder_toupper(struct MaxStringBuilder * buf) {
	int i;
	for (i = 0; i < buf->count; i++) {
		int c = buf->buffer[i];
		if (c < 181) {
			c = (c >= 'a' && c <= 'z') ? (c&~32) : c;
		} else {
			int lo = 0, hi = 3860/4-1;
			while (lo<=hi) {
				int mid=(lo+hi)/2;
				if (c < bbToUpperData[mid*2]) {
					hi = mid - 1;
				} else if (c > bbToUpperData[mid*2]) {
					lo = mid + 1;
				} else {
					c = bbToUpperData[mid*2+1];
					break;
				}
			}
		}
		buf->buffer[i]=c;
	}
}

void bmx_stringbuilder_trim(struct MaxStringBuilder * buf) {
	int start = 0;
	int end = buf->count;
	while (start < end && buf->buffer[start] <= ' ') {
		++start;
	}
	if (start == end ) {
		buf->count = 0;
		return;
	}
	while (buf->buffer[end - 1] <= ' ') {
		--end;
	}
	if (end - start == buf->count) {
		return;
	}

	memmove(buf->buffer, buf->buffer + start, (end - start) * sizeof(BBChar));
	buf->count = end - start;	
}

void bmx_stringbuilder_replace(struct MaxStringBuilder * buf, BBString * subString, BBString *  withString) {
	if (!subString->length) {
		return;
	}
	
	struct MaxStringBuilder * newbuf = bmx_stringbuilder_new(16);
	
	int j, n;
	int i = 0;
	int p = 0;
	
	while( (j = bmx_stringbuilder_find(buf, subString, i)) != -1) {
		n = j - i;
		if (n) {
			bmx_stringbuilder_resize(newbuf, newbuf->count + n);
			memcpy(newbuf->buffer + p, buf->buffer + i, n * sizeof(BBChar));
			newbuf->count += n;
			p += n;
		}
		n = withString->length;
		bmx_stringbuilder_resize(newbuf, newbuf->count + n);
		memcpy(newbuf->buffer + p, withString->buf, n * sizeof(BBChar));
		newbuf->count += n;
		p += n;
		i = j + subString->length;
	}

	n = buf->count - i;
	if (n) {
		bmx_stringbuilder_resize(newbuf, newbuf->count + n);
		memcpy(newbuf->buffer + p, buf->buffer + i, n*sizeof(BBChar));
		newbuf->count += n;
	}

	bmx_stringbuilder_setlength(buf, 0);
	bmx_stringbuilder_append_stringbuffer(buf, newbuf);
	bmx_stringbuilder_free(newbuf);
}

void bmx_stringbuilder_join(struct MaxStringBuilder * buf, BBArray * bits, struct MaxStringBuilder * newbuf) {
	if(bits == &bbEmptyArray) {
		return;
	}

	int i;
	int n_bits = bits->scales[0];
	BBString **p = (BBString**)BBARRAYDATA( bits,1 );
	for(i = 0; i < n_bits; ++i) {
		if (i) {
			bmx_stringbuilder_append_stringbuffer(newbuf, buf);
		}
		BBString *bit = *p++;
		bmx_stringbuilder_append_string(newbuf, bit);
	}
}

struct MaxSplitBuffer * bmx_stringbuilder_split(struct MaxStringBuilder * buf, BBString * separator) {
	struct MaxSplitBuffer * splitBuffer = malloc(sizeof(struct MaxSplitBuffer));
	splitBuffer->buffer = buf;
	
	int count = 1;
	int i = 0;
	int offset = 0;
	if (separator->length > 0) {
	
		/* get a count of fields */
		while ((offset = bmx_stringbuilder_find(buf, separator, i)) != -1 ) {
			++count;
			i = offset + separator->length;
		}

		splitBuffer->count = count;
		splitBuffer->startIndex = malloc(count * sizeof(int));
		splitBuffer->endIndex = malloc(count * sizeof(int));

		i = 0;
		
		int * bufferStartIndex = splitBuffer->startIndex;
		int * bufferEndIndex = splitBuffer->endIndex;
		
		while( count-- ){
			offset = bmx_stringbuilder_find(buf, separator, i);
			if (offset == -1) {
				offset = buf->count;
			}
			
			*bufferStartIndex++ = i;
			*bufferEndIndex++ = offset;

			i = offset + separator->length;
		}

	} else {
		// TODO - properly handle Null separator
		
		splitBuffer->count = count;
		splitBuffer->startIndex = malloc(count * sizeof(int));
		splitBuffer->endIndex = malloc(count * sizeof(int));
		
		*splitBuffer->startIndex = 0;
		*splitBuffer->endIndex = buf->count;

	}
	
	return splitBuffer;
}

void bmx_stringbuilder_setcharat(struct MaxStringBuilder * buf, int index, int ch) {
	if (index < 0 || index > buf->count) {
		return;
	}

	buf->buffer[index] = ch;
}

int bmx_stringbuilder_charat(struct MaxStringBuilder * buf, int index) {
	if (index < 0 || index > buf->count) {
		return 0;
	}

	return buf->buffer[index];
}

void bmx_stringbuilder_removecharat(struct MaxStringBuilder * buf, int index) {
	if (index < 0 || index >= buf->count) {
		return;
	}

	if (index < buf->count - 1) {
		memcpy(buf->buffer + index, buf->buffer + index + 1, (buf->count - index - 1) * sizeof(BBChar));
	}
	
	buf->count--;

}

void bmx_stringbuilder_append_cstring(struct MaxStringBuilder * buf, const char * chars) {
	int length = strlen(chars);
	if (length > 0) {
		int count = length;
		
		bmx_stringbuilder_resize(buf, buf->count + length);
		
		char * p = chars;
		BBChar * b = buf->buffer + buf->count;
		while (length--) {
			*b++ = *p++;
		}
		
		buf->count += count;
	}
}

void bmx_stringbuilder_append_utf8string(struct MaxStringBuilder * buf, const char * chars) {
	int length = strlen(chars);
	if (length > 0) {
		int count = 0;
		
		bmx_stringbuilder_resize(buf, buf->count + length);
		
		int c;
		char * p = chars;
		BBChar * b = buf->buffer + buf->count;
		
		while( c=*p++ & 0xff ){
			if( c<0x80 ){
				*b++=c;
			}else{
				int d=*p++ & 0x3f;
				if( c<0xe0 ){
					*b++=((c&31)<<6) | d;
				}else{
					int e=*p++ & 0x3f;
					if( c<0xf0 ){
						*b++=((c&15)<<12) | (d<<6) | e;
					}else{
						int f=*p++ & 0x3f;
						int v=((c&7)<<18) | (d<<12) | (e<<6) | f;
						if( v & 0xffff0000 ) bbExThrowCString( "Unicode character out of UCS-2 range" );
						*b++=v;
					}
				}
			}
			count++;
		}

		buf->count += count;
	}
}

/* ----------------------------------------------------- */

int bmx_stringbuilder_splitbuffer_length(struct MaxSplitBuffer * buf) {
	return buf->count;
}

BBString * bmx_stringbuilder_splitbuffer_text(struct MaxSplitBuffer * buf, int index) {
	if (index < 0 || index >= buf->count) {
		return &bbEmptyString;
	}

	return bmx_stringbuilder_substring(buf->buffer, buf->startIndex[index], buf->endIndex[index]);
}

void bmx_stringbuilder_splitbuffer_free(struct MaxSplitBuffer * buf) {
	free(buf->startIndex);
	free(buf->endIndex);
	free(buf);
}

BBArray * bmx_stringbuilder_splitbuffer_toarray(struct MaxSplitBuffer * buf) {
	int i, n;
	BBString **p,*bit;
	BBArray *bits;

	n = buf->count;
	
	bits = bbArrayNew1D("$", n);
	p = (BBString**)BBARRAYDATA(bits, 1);

	i = 0;
	while (n--) {
		bit = bmx_stringbuilder_substring(buf->buffer, buf->startIndex[i], buf->endIndex[i]);
		BBINCREFS( bit );
		*p++ = bit;
		i++;
	}
	return bits;
}
