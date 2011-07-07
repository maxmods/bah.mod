' Copyright (c) 2010-2011 Bruce A Henderson
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

?Not linux
Import "-liconv"
?
?win32
Import "include/*.h"
?
Import "glue.cpp"

Extern
	?win32
	Function iconv_open:Byte Ptr(toCode:Byte Ptr, fromCode:Byte Ptr)="libiconv_open"
	Function iconv_close:Int(handle:Byte Ptr)="libiconv_close"
	?Not win32
	Function iconv_open:Byte Ptr(toCode:Byte Ptr, fromCode:Byte Ptr)
	Function iconv_close:Int(handle:Byte Ptr)
	?
	Function bmx_iconv:Int(handle:Byte Ptr, inbuf:Byte Ptr Ptr, inBytesLeft:Int Ptr, outbuf:Byte Ptr Ptr, outBytesLeft:Int Ptr)
	
	Function bmx_getErrno:Int()
End Extern


Const EPERM:Int =           1                ' Operation Not permitted
Const ENOENT:Int =          2                ' No such file Or directory
Const ESRCH:Int =           3                ' No such process
Const EINTR:Int =           4                ' Interrupted system call
Const EIO:Int =             5                ' Input/output error
Const ENXIO:Int =           6                ' Device Not configured
Const E2BIG:Int =           7                ' Argument list too Long
Const ENOEXEC:Int =         8                ' Exec format error
Const EBADF:Int =           9                ' Bad file number
Const ECHILD:Int =          10               ' No spawned processes
Const EAGAIN:Int =          11               ' Resource temporarily unavailable
Const ENOMEM:Int =          12               ' Cannot allocate memory
Const EACCES:Int =          13               ' Access denied
Const EFAULT:Int =          14               ' Bad address
Const ENOTBLK:Int =         15               ' Not block device
Const EBUSY:Int =           16               ' Device busy
Const EEXIST:Int =          17               ' File exist
Const EXDEV:Int =           18               ' Cross-device link
Const ENODEV:Int =          19               ' Operation Not supported by device
Const ENOTDIR:Int =         20               ' Not a directory
Const EISDIR:Int =          21               ' Is a directory
Const EINVAL:Int =          22               ' Invalid argument
Const ENFILE:Int =          23               ' Too many open files in system
Const EMFILE:Int =          24               ' Too many files open
Const ENOTTY:Int =          25               ' Inappropriate ioctl For device
Const ETXTBSY:Int =         26               ' Unknown error
Const EFBIG:Int =           27               ' File too large
Const ENOSPC:Int =          28               ' No space Left on device
Const ESPIPE:Int =          29               ' Illegal seek
Const EROFS:Int =           30               ' Read-only file system
Const EMLINK:Int =          31               ' Too many links
Const EPIPE:Int =           32               ' Broken pipe
Const EDOM:Int =            33               ' Numerical arg out of domain
Const ERANGE:Int =          34               ' Result too large
Const EUCLEAN:Int =         35               ' Structure needs cleaning
Const EDEADLK:Int =         36               ' Resource deadlock avoided
Const EUNKNOWN:Int =        37               ' Unknown error
Const ENAMETOOLONG:Int =    38               ' File name too Long
Const ENOLCK:Int =          39               ' No locks available
Const ENOSYS:Int =          40               ' Function Not implemented
Const ENOTEMPTY:Int =       41               ' Directory Not empty
Const EILSEQ:Int =          42               ' Invalid multibyte sequence
