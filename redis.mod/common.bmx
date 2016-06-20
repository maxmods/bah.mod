' Copyright (c) 2016 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "source.bmx"

?win32
Import "-lws2_32"
?

Extern

	Function redisConnect:Byte Ptr(ip:Byte Ptr, port:Int)
	Function freeReplyObject(handle:Byte Ptr)
	Function redisFree(handle:Byte Ptr)
	Function bmx_redis_err:Int(handle:Byte Ptr)
	Function bmx_redis_errstr:String(handle:Byte Ptr)

	Function bmx_redis_reply_getstring:String(handle:Byte Ptr)
	
End Extern




Rem
bbdoc: Error in read or write.
End Rem
Const REDIS_ERR_IO:Int = 1
Rem
bbdoc: End of file.
End Rem
Const REDIS_ERR_EOF:Int = 3
Rem
bbdoc: Protocol error.
End Rem
Const REDIS_ERR_PROTOCOL:Int = 4
Rem
bbdoc: Out of memory.
End Rem
Const REDIS_ERR_OOM:Int = 5
Rem
bbdoc: Everything else.
End Rem
Const REDIS_ERR_OTHER:Int = 2

Rem
bbdoc: A bulk (string) reply.
about: The value of the reply can be accessed using GetString().
End Rem
Const REDIS_REPLY_STRING:Int = 1
Rem
bbdoc: A multi bulk reply.
about: The number of elements in the multi bulk reply is stored in the elements field.
Every element in the multi bulk reply is a TRedisReply object as well and can be accessed via GetElement()
End Rem
Const REDIS_REPLY_ARRAY:Int = 2
Rem
bbdoc: The command replied with an integer.
about: The integer value can be accessed using the intValue field.
End Rem
Const REDIS_REPLY_INTEGER:Int = 3
Rem
bbdoc: The command replied with a nil object.
about: There is no data to access.
End Rem
Const REDIS_REPLY_NIL:Int = 4
Rem
bbdoc: The command replied with a status reply.
about: The status string can be accessed using GetString().
End Rem
Const REDIS_REPLY_STATUS:Int = 5
Rem
bbdoc: The command replied with an error.
about: The error string can be accessed using GetString().
End Rem
Const REDIS_REPLY_ERROR:Int = 6

