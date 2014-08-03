' Copyright (c) 2014 Bruce A Henderson
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

Import BRL.Blitz

?linux
Import "include/linux/*.h"
?macos
Import "include/macos/*.h"
?win32
Import "include/win32/*.h"
?

Import "jansson/src/*.h"

Import "jansson/src/dump.c"
Import "jansson/src/error.c"
Import "jansson/src/hashtable.c"
Import "jansson/src/hashtable_seed.c"
Import "jansson/src/load.c"
Import "jansson/src/memory.c"
Import "jansson/src/pack_unpack.c"
Import "jansson/src/strbuffer.c"
Import "jansson/src/strconv.c"
Import "jansson/src/utf.c"
Import "jansson/src/value.c"

Import "glue.cpp"

