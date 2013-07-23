' Copyright (c) 2013 Bruce A Henderson
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

Import "../../pub.mod/zlib.mod/*.h"

Import "libwebsockets/lib/*.h"

Import "libwebsockets/lib/base64-decode.c"
Import "libwebsockets/lib/client-handshake.c"
Import "libwebsockets/lib/client-parser.c"
Import "libwebsockets/lib/client.c"
Import "libwebsockets/lib/extension-deflate-frame.c"
Import "libwebsockets/lib/extension-deflate-stream.c"
Import "libwebsockets/lib/extension.c"
?Not win32
Import "libwebsockets/lib/getifaddrs.c"
?
Import "libwebsockets/lib/handshake.c"
Import "libwebsockets/lib/libwebsockets.c"
Import "libwebsockets/lib/minilex.c"
Import "libwebsockets/lib/output.c"
Import "libwebsockets/lib/parsers.c"
Import "libwebsockets/lib/server-handshake.c"
Import "libwebsockets/lib/server.c"
Import "libwebsockets/lib/sha-1.c"

Import "glue.cpp"
