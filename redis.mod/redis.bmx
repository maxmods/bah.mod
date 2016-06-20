' Copyright (c) 2016 Bruce A Henderson
' Copyright (c) 2011 Jens Riisom Schultz
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

Rem
bbdoc: Hiredis Redis Client.
End Rem
Module BaH.Redis


ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson, Jens Riisom Schultz"
ModuleInfo "License: BSD"

ModuleInfo "History: 1.00"
ModuleInfo "History: Rewritten for NG using Hiredis client library."
ModuleInfo "History: 0.5a"
ModuleInfo "History: Original BlitzMax-based API for Redis by Jens Riisom Schultz"

?win32
ModuleInfo "CC_OPTS: -D_WIN32_WINNT=0x0600"
?

?bmxng

Import BaH.StringBuffer
Import "common.bmx"


Rem
bbdoc: A Redis client.
End Rem
Type TRedisClient

	Field contextPtr:Byte Ptr


	Rem
	bbdoc: Creates and opens a new connection to a redis server.
	End Rem
	Function Create:TRedisClient(host:String = "localhost", port:Int = 6379)
		Return New TRedisClient(host, port)
	EndFunction

	Rem
	bbdoc: Creates and opens a new connection to a redis server.
	End Rem
	Method New(host:String, port:Int = 6379)
		Local ip:Byte Ptr host.ToUTF8String()
		contextPtr = redisConnect(ip, port)
		MemFree(ip)
		
		If Not contextPtr Then
			Throw TRedisError.Create("Failed to create Redis context")
		End If
		
		Local err:Int = bmx_redis_err(contextPtr)
		If err Then
			Throw TRedisError.Create("Error " + err + " : " + bmx_redis_errstr(contextPtr))
		End If
	End Method
	
	Rem
	bbdoc: Disconnects and frees the context.
	about: Immediately closes the socket and then frees the allocations done in creating the context.
	End Rem
	Method Free()
		If contextPtr Then
			redisFree(contextPtr)
			contextPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
	Method Command:TRedisReply(cmd:String, param:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local p:Byte Ptr = param.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ss(contextPtr, c, p)
		
		MemFree(p)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String)
		Local c:Byte Ptr = cmd.ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_s(contextPtr, c)
		
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, param:Int)
		Local c:Byte Ptr = cmd.ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_si(contextPtr, c, param)
		
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt:String, num:Int)
		Local c:Byte Ptr = cmd.ToCString()
		Local p:Byte Ptr = txt.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssi(contextPtr, c, p, num)
		
		MemFree(p)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, txt2:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_sss(contextPtr, c, t1, t2)
		
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, txt2:String, num:Int)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_sssi(contextPtr, c, t1, t2, num)
		
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, num:Int, txt2:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssis(contextPtr, c, t1, num, t2)
		
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, num:Double, txt2:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssds(contextPtr, c, t1, num, t2)
		
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, num1:Double, num2:Double)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssdd(contextPtr, c, t1, num1, num2)
		
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, num1:Int, num2:Int)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssii(contextPtr, c, t1, num1, num2)
		
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, txt2:String, txt3:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		Local t3:Byte Ptr = txt3.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_ssss(contextPtr, c, t1, t2, t3)
		
		MemFree(t3)
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt1:String, txt2:String, txt3:String, txt4:String)
		Local c:Byte Ptr = cmd.ToCString()
		Local t1:Byte Ptr = txt1.ToUTF8String()
		Local t2:Byte Ptr = txt2.ToUTF8String()
		Local t3:Byte Ptr = txt3.ToUTF8String()
		Local t4:Byte Ptr = txt4.ToUTF8String()
		
		Local reply:TRedisReply = bmx_redis_command_sssss(contextPtr, c, t1, t2, t3, t4)
		
		MemFree(t4)
		MemFree(t3)
		MemFree(t2)
		MemFree(t1)
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, arr:String[])
	
		Local sb:TStringBuffer = New TStringBuffer(16 * arr.length)
		
		sb.Append(cmd)
	
		For Local i:Int = 0 Until arr.length
			sb.Append(" ").Append(arr[i])
		Next
	
		Local c:Byte Ptr = sb.ToString().ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_a(contextPtr, c)
		
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, txt:String, arr:String[])
	
		Local sb:TStringBuffer = New TStringBuffer(16 * arr.length)
		
		sb.Append(cmd).Append(" ").Append(txt)
	
		For Local i:Int = 0 Until arr.length
			sb.Append(" ").Append(arr[i])
		Next
	
		Local c:Byte Ptr = sb.ToString().ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_a(contextPtr, c)
		
		MemFree(c)
		
		Return reply
	End Method

	Method Command:TRedisReply(cmd:String, arr:String[], num:Int)
	
		Local sb:TStringBuffer = New TStringBuffer(16 * arr.length)
		
		sb.Append(cmd)
	
		For Local i:Int = 0 Until arr.length
			sb.Append(" ").Append(arr[i])
		Next
		
		sb.Append(" ").Append(num)
	
		Local c:Byte Ptr = sb.ToString().ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_a(contextPtr, c)
		
		MemFree(c)
		
		Return reply

	End Method

	Method Command:TRedisReply(sb:TStringBuffer)
	
		Local c:Byte Ptr = sb.ToString().ToCString()
		
		Local reply:TRedisReply = bmx_redis_command_a(contextPtr, c)
		
		MemFree(c)
		
		Return reply

	End Method

	Rem
	bbdoc: AUTH: Authenticate to the server.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/auth
	End Rem
	Method AUTH_:TRedisReply(password:String)
		Return Command("AUTH", password)
	End Method

	Rem
	bbdoc: ECHO: Echo the given string.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/echo
	End Rem
	Method ECHO_:TRedisReply(message:String)
		Return Command("ECHO", message)
	End Method

	Rem
	bbdoc: PING: Ping the server.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/ping
	End Rem
	Method PING_:TRedisReply()
		Return Command("PING")
	End Method

	Rem
	bbdoc: QUIT: Close the connection.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/quit
	End Rem
	Method QUIT_:TRedisReply()
		Return Command("QUIT")
	End Method

	Rem
	bbdoc: SELECT: Change the selected database for the current connection.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/select
	End Rem
	Method SELECT_:TRedisReply(index:Int)
		Return Command("SELECT", index)
	End Method

	Rem
	bbdoc: DEL: Delete a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/del
	End Rem
	Method DEL_:TRedisReply(keys:String[])
		Return Command("DEL", keys)
	End Method

	Rem
	bbdoc: EXISTS: Determine if a key exists.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/exists
	End Rem
	Method EXISTS_:TRedisReply(key:String)
		Return Command("EXISTS", key)
	End Method

	Rem
	bbdoc: EXPIRE: Set a key's time to live in seconds.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/expire
	End Rem
	Method EXPIRE_:TRedisReply(key:String, seconds:Int)
		Return Command("EXPIRE", key, seconds)
	End Method

	Rem
	bbdoc: EXPIREAT: Set the expiration for a key as a UNIX timestamp.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/expireat
	End Rem
	Method EXPIREAT_:TRedisReply(key:String, timestamp:Int)
		Return Command("EXPIREAT", key, timestamp)
	End Method

	Rem
	bbdoc: KEYS: Find all keys matching the given pattern.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/keys
	End Rem
	Method KEYS_:TRedisReply(pattern:String)
		Return Command("KEYS", pattern)
	End Method

	Rem
	bbdoc: MOVE: Move a key to another database.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/move
	End Rem
	Method MOVE_:TRedisReply(key:String, db:String)
		Return Command("MOVE", key, db)
	End Method

	Rem
	bbdoc: OBJECT: Inspect the internals of Redis objects.
	Returns an Integer reply or a bulk reply, depending on the subcommand.
	Documentation: http://redis.io/commands/object
	End Rem
	Method OBJECT_:TRedisReply(subcommand:String, arguments:String[])
		Return Command("OBJECT ", subcommand, arguments)
	End Method

	Rem
	bbdoc: PERSIST: Remove the expiration from a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/persist
	End Rem
	Method PERSIST_:TRedisReply(key:String)
		Return Command("PERSIST", key)
	End Method

	Rem
	bbdoc: RANDOMKEY: Return a random key from the keyspace.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/randomkey
	End Rem
	Method RANDOMKEY_:TRedisReply()
		Return Command("RANDOMKEY")
	End Method

	Rem
	bbdoc: RENAME: Rename a key.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/rename
	End Rem
	Method RENAME_:TRedisReply(key:String, newkey:String)
		Return Command("RENAME", key, newkey)
	End Method

	Rem
	bbdoc: RENAMENX: Rename a key, only if the new key does not exist.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/renamenx
	End Rem
	Method RENAMENX_:TRedisReply(key:String, newkey:String)
		Return Command("RENAMENX", key, newkey)
	End Method

	Rem
	bbdoc: SORT: Sort the elements in a list, set or sorted set.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/sort
	End Rem
	Method SORT_:TRedisReply(key:String, BY_pattern:String = Null, offset:Int = 0, count:Int = 0, GET_patterns:String[] = Null, DESC:Int = True, ALPHA:Int = False, STORE_destination:String = Null)
		Local sb:TStringBuffer = New TStringBuffer(128)
		
		sb.Append("SORT ").Append(key)

		If BY_pattern Then
			sb.Append(" BY ").Append(BY_pattern)
		End If
		If count Then
			sb.Append(" LIMIT ").Append(offset).Append(" ").Append(count)
		End If
		If GET_patterns Then
			For Local GET_pattern:String = EachIn GET_patterns
				sb.Append(" GET ").Append(GET_pattern)
			Next
		End If
		If DESC Then
			sb.Append(" DESC ")
		Else
			sb.Append(" ASC ")
		End If
		If ALPHA Then
			sb.Append(" ALPHA ")
		End If
		If STORE_destination Then
			sb.Append(" STORE ").Append(STORE_destination)
		End If

		Return Command(sb)
	End Method

	Rem
	bbdoc: TTL: Get the time to live for a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/ttl
	End Rem
	Method TTL_:TRedisReply(key:String)
		Return Command("TTL", key)
	End Method

	Rem
	bbdoc: TYPE: Determine the type stored at key.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/type
	End Rem
	Method TYPE_:TRedisReply(key:String)
		Return Command("TYPE", key)
	End Method

	Rem
	bbdoc: HDEL: Delete a hash field.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hdel
	End Rem
	Method HDEL_:TRedisReply(key:String, _field:String)
		Return Command("HDEL", key, _field)
	End Method

	Rem
	bbdoc: HEXISTS: Determine if a hash field exists.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hexists
	End Rem
	Method HEXISTS_:TRedisReply(key:String, _field:String)
		Return Command("HEXISTS", key, _field)
	End Method

	Rem
	bbdoc: HGET: Get the value of a hash field.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/hget
	End Rem
	Method HGET_:TRedisReply(key:String, _field:String)
		Return Command("HGET", key, _field)
	End Method

	Rem
	bbdoc: HGETALL: Get all the fields and values in a hash.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/hgetall
	End Rem
	Method HGETALL_:TRedisReply(key:String)
		Return Command("HGETALL", key)
	End Method

	Rem
	bbdoc: HINCRBY: Increment the integer value of a hash field by the given number.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hincrby
	End Rem
	Method HINCRBY_:TRedisReply(key:String, _field:String, increment:Int)
		Return Command("HINCRBY", key, _field, increment)
	End Method

	Rem
	bbdoc: HKEYS: Get all the fields in a hash.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/hkeys
	End Rem
	Method HKEYS_:TRedisReply(key:String)
		Return Command("HKEYS", key)
	End Method

	Rem
	bbdoc: HLEN: Get the number of fields in a hash.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hlen
	End Rem
	Method HLEN_:TRedisReply(key:String)
		Return Command("HLEN", key)
	End Method

	Rem
	bbdoc: HMGET: Get the values of all the given hash fields.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/hmget
	End Rem
	Method HMGET_:TRedisReply(key:String, fields:String[])
		Return Command("HMGET", key, fields)
	End Method

	Rem
	bbdoc: HMSET: Set multiple hash fields to multiple values.
	Returns a Status code reply.
	The field_value parameter is an array of even length, containing pairs of fields and values: [field1, value1, field2, value2...].
	Documentation: http://www.redis.io/commands/hmset
	End Rem
	Method HMSET_:TRedisReply(key:String, field_value:String[])
		Return Command("HMSET", key, field_value)
	End Method

	Rem
	bbdoc: HSET: Set the string value of a hash field.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hset
	End Rem
	Method HSET_:TRedisReply(key:String, _field:String, value:String)
		Return Command("HSET", key, _field, value)
	End Method

	Rem
	bbdoc: HSETNX: Set the value of a hash field, only if the field does not exist.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/hsetnx
	End Rem
	Method HSETNX_:TRedisReply(key:String, _field:String, value:String)
		Return Command("HSETNX", key, _field, value)
	End Method

	Rem
	bbdoc: HVALS: Get all the values in a hash.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/hvals
	End Rem
	Method HVALS_:TRedisReply(key:String)
		Return Command("HVALS", key)
	End Method

	Rem
	bbdoc: BLPOP: Remove and get the first element in a list, or block until one is available.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/blpop
	End Rem
	Method BLPOP_:TRedisReply(keys:String[], timeout:Int)
		Return Command("BLPOP", keys, timeout)
	End Method

	Rem
	bbdoc: BRPOP: Remove and get the last element in a list, or block until one is available.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/brpop
	End Rem
	Method BRPOP_:TRedisReply(keys:String[], timeout:Int)
		Return Command("BRPOP", keys, timeout)
	End Method

	Rem
	bbdoc: BRPOPLPUSH: Pop a value from a list, push it to another list and return it; or block until one is available.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/brpoplpush
	End Rem
	Method BRPOPLPUSH_:TRedisReply(source:String, destination:String, timeout:Int)
		Return Command("BRPOPLPUSH", source, destination, timeout)
	End Method

	Rem
	bbdoc: LINDEX: Get an element from a list by its index.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/lindex
	End Rem
	Method LINDEX_:TRedisReply(key:String, index:Int)
		Return Command("LINDEX", key, index)
	End Method

	Rem
	bbdoc: LINSERT: Insert an element before or after another element in a list.
	Returns an Integer reply.
	Set before to true, to insert before. Set it to False, to insert AFTER.
	Documentation: http://www.redis.io/commands/linsert
	End Rem
	Method LINSERT_:TRedisReply(key:String, before:Int, pivot:String, value:String)
		If before Then
			Return Command("LINSERT", key, "BEFORE", pivot, value)
		Else
			Return Command("LINSERT", key, "AFTER", pivot, value)
		End If
	End Method

	Rem
	bbdoc: LLEN: Get the length of a list.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/llen
	End Rem
	Method LLEN_:TRedisReply(key:String)
		Return Command("LLEN", key)
	End Method

	Rem
	bbdoc: LPOP: Remove and get the first element in a list.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/lpop
	End Rem
	Method LPOP_:TRedisReply(key:String)
		Return Command("LPOP", key)
	End Method

	Rem
	bbdoc: LPUSH: Prepend a value to a list.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/lpush
	End Rem
	Method LPUSH_:TRedisReply(key:String, value:String)
		Return Command("LPUSH", key, value)
	End Method

	Rem
	bbdoc: LPUSHX: Prepend a value to a list, only if the list exists.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/lpushx
	End Rem
	Method LPUSHX_:TRedisReply(key:String, value:String)
		Return Command("LPUSHX", key, value)
	End Method

	Rem
	bbdoc: LRANGE: Get a range of elements from a list.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/lrange
	End Rem
	Method LRANGE_:TRedisReply(key:String, start:Int, stop:Int)
		Return Command("LRANGE", key, start, stop)
	End Method

	Rem
	bbdoc: LREM: Remove elements from a list.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/lrem
	End Rem
	Method LREM_:TRedisReply(key:String, count:Int, value:String)
		Return Command("LREM", key, count, value)
	End Method

	Rem
	bbdoc: LSET: Set the value of an element in a list by its index.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/lset
	End Rem
	Method LSET_:TRedisReply(key:String, index:Int, value:String)
		Return Command("LSET", key, index, value)
	End Method

	Rem
	bbdoc: LTRIM: Trim a list to the specified range.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/ltrim
	End Rem
	Method LTRIM_:TRedisReply(key:String, start:Int, stop:Int)
		Return Command("LTRIM", key, start, stop)
	End Method

	Rem
	bbdoc: RPOP: Remove and get the last element in a list.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/rpop
	End Rem
	Method RPOP_:TRedisReply(key:String)
		Return Command("RPOP", key)
	End Method

	Rem
	bbdoc: RPOPLPUSH: Remove the last element in a list, append it to another list and return it.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/rpoplpush
	End Rem
	Method RPOPLPUSH_:TRedisReply(source:String, destination:String)
		Return Command("RPOPLPUSH", source, destination)
	End Method

	Rem
	bbdoc: RPUSH: Append a value to a list.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/rpush
	End Rem
	Method RPUSH_:TRedisReply(key:String, value:String)
		Return Command("RPUSH", key, value)
	End Method

	Rem
	bbdoc: RPUSHX: Append a value to a list, only if the list exists.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/rpushx
	End Rem
	Method RPUSHX_:TRedisReply(key:String, value:String)
		Return Command("RPUSHX", key, value)
	End Method

	Rem
	bbdoc: PSUBSCRIBE: Listen for messages published to channels matching the given patterns.
	Documentation: http://www.redis.io/commands/psubscribe
	End Rem
	Method PSUBSCRIBE_:TRedisReply(patterns:String[])
		Return Command("PSUBSCRIBE", patterns)
	End Method

	Rem
	bbdoc: PUBLISH: Post a message to a channel.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/publish
	End Rem
	Method PUBLISH_:TRedisReply(channel:String, message:String)
		Return Command("PUBLISH", channel, message)
	End Method

	Rem
	bbdoc: PUNSUBSCRIBE: Stop listening for messages posted to channels matching the given patterns.
	Documentation: http://www.redis.io/commands/punsubscribe
	End Rem
	Method PUNSUBSCRIBE_:TRedisReply(patterns:String[])
		Return Command("PUNSUBSCRIBE", patterns)
	End Method

	Rem
	bbdoc: SUBSCRIBE: Listen for messages published to the given channels.
	Documentation: http://www.redis.io/commands/subscribe
	End Rem
	Method SUBSCRIBE_:TRedisReply(channels:String[])
		Return Command("SUBSCRIBE", channels)
	End Method

	Rem
	bbdoc: UNSUBSCRIBE: Stop listening for messages posted to the given channels.
	Documentation: http://www.redis.io/commands/unsubscribe
	End Rem
	Method UNSUBSCRIBE_:TRedisReply(channels:String[])
		Return Command("UNSUBSCRIBE", channels)
	End Method

	Rem
	bbdoc: BGREWRITEAOF: Asynchronously rewrite the append-only file.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/bgrewriteaof
	End Rem
	Method BGREWRITEAOF_:TRedisReply()
		Return Command("BGREWRITEAOF")
	End Method

	Rem
	bbdoc: BGSAVE: Asynchronously save the dataset to disk.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/bgsave
	End Rem
	Method BGSAVE_:TRedisReply()
		Return Command("BGSAVE")
	End Method

	Rem
	bbdoc: CONFIG GET: Get the value of a configuration parameter.
	Documentation: http://www.redis.io/commands/config-get
	End Rem
	Method CONFIG_GET_:TRedisReply(parameter:String)
		Return Command("CONFIG", "GET", parameter)
	End Method

	Rem
	bbdoc: CONFIG RESETSTAT: Reset the stats returned by INFO.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/config-resetstat
	End Rem
	Method CONFIG_RESETSTAT_:TRedisReply()
		Return Command("CONFIG", "RESETSTAT")
	End Method

	Rem
	bbdoc: CONFIG SET: Set a configuration parameter to the given value.
	Documentation: http://www.redis.io/commands/config-set
	End Rem
	Method CONFIG_SET_:TRedisReply(parameter:String, value:String)
		Return Command("CONFIG", "SET", parameter, value)
	End Method

	Rem
	bbdoc: DBSIZE: Return the number of keys in the selected database.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/dbsize
	End Rem
	Method DBSIZE_:TRedisReply()
		Return Command("DBSIZE")
	End Method

	Rem
	bbdoc: DEBUG OBJECT: Get debugging information about a key.
	Documentation: http://www.redis.io/commands/debug-object
	End Rem
	Method DEBUG_OBJECT_:TRedisReply(key:String)
		Return Command("DEBUG", "OBJECT", key)
	End Method

	Rem
	bbdoc: DEBUG SEGFAULT: Make the server crash.
	Documentation: http://www.redis.io/commands/debug-segfault
	End Rem
	Method DEBUG_SEGFAULT_:TRedisReply()
		Return Command("DEBUG", "SEGFAULT")
	End Method

	Rem
	bbdoc: FLUSHALL: Remove all keys from all databases.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/flushall
	End Rem
	Method FLUSHALL_:TRedisReply()
		Return Command("FLUSHALL")
	End Method

	Rem
	bbdoc: FLUSHDB: Remove all keys from the current database.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/flushdb
	End Rem
	Method FLUSHDB_:TRedisReply()
		Return Command("FLUSHDB")
	End Method

	Rem
	bbdoc: INFO: Get information and statistics about the server.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/info
	End Rem
	Method INFO_:TRedisReply()
		Return Command("INFO")
	End Method

	Rem
	bbdoc: LASTSAVE: Get the UNIX time stamp of the last successful save to disk.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/lastsave
	End Rem
	Method LASTSAVE_:TRedisReply()
		Return Command("LASTSAVE")
	End Method

	Rem
	bbdoc: MONITOR: Listen for all requests received by the server in real time.
	Documentation: http://www.redis.io/commands/monitor
	End Rem
	Method MONITOR_:TRedisReply()
		Return Command("MONITOR")
	End Method

	Rem
	bbdoc: SAVE: Synchronously save the dataset to disk.
	Documentation: http://www.redis.io/commands/save
	End Rem
	Method SAVE_:TRedisReply()
		Return Command("SAVE")
	End Method

	Rem
	bbdoc: SHUTDOWN: Synchronously save the dataset to disk and then shut down the server.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/shutdown
	End Rem
	Method SHUTDOWN_:TRedisReply()
		Return Command("SHUTDOWN")
	End Method

	Rem
	bbdoc: SLAVEOF: Make the server a slave of another instance, or promote it as master.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/slaveof
	End Rem
	Method SLAVEOF_:TRedisReply(host:String, port:Int)
		Return Command("SLAVEOF", host, port)
	End Method

	Rem
	bbdoc: SYNC: Internal command used for replication.
	Documentation: http://www.redis.io/commands/sync
	End Rem
	Method SYNC_:TRedisReply()
		Return Command("SYNC")
	End Method

	Rem
	bbdoc: SADD: Add a member to a set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/sadd
	End Rem
	Method SADD_:TRedisReply(key:String, member:String)
		Return Command("SADD", key, member)
	End Method

	Rem
	bbdoc: SCARD: Get the number of members in a set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/scard
	End Rem
	Method SCARD_:TRedisReply(key:String)
		Return Command("SCARD", key)
	End Method

	Rem
	bbdoc: SDIFF: Subtract multiple sets.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/sdiff
	End Rem
	Method SDIFF_:TRedisReply(keys:String[])
		Return Command("SDIFF", keys)
	End Method

	Rem
	bbdoc: SDIFFSTORE: Subtract multiple sets and store the resulting set in a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/sdiffstore
	End Rem
	Method SDIFFSTORE_:TRedisReply(destination:String, keys:String[])
		Return Command("SDIFFSTORE", destination , keys)
	End Method

	Rem
	bbdoc: SINTER: Intersect multiple sets.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/sinter
	End Rem
	Method SINTER_:TRedisReply(keys:String[])
		Return Command("SINTER", keys)
	End Method

	Rem
	bbdoc: SINTERSTORE: Intersect multiple sets and store the resulting set in a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/sinterstore
	End Rem
	Method SINTERSTORE_:TRedisReply(destination:String, keys:String[])
		Return Command("SINTERSTORE", destination , keys)
	End Method

	Rem
	bbdoc: SISMEMBER: Determine if a given value is a member of a set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/sismember
	End Rem
	Method SISMEMBER_:TRedisReply(key:String, member:String)
		Return Command("SISMEMBER", key, member)
	End Method

	Rem
	bbdoc: SMEMBERS: Get all the members in a set.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/smembers
	End Rem
	Method SMEMBERS_:TRedisReply(key:String)
		Return Command("SMEMBERS", key)
	End Method

	Rem
	bbdoc: SMOVE: Move a member from one set to another.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/smove
	End Rem
	Method SMOVE_:TRedisReply(source:String, destination:String, member:String)
		Return Command("SMOVE", source, destination, member)
	End Method

	Rem
	bbdoc: SPOP: Remove and return a random member from a set.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/spop
	End Rem
	Method SPOP_:TRedisReply(key:String)
		Return Command("SPOP", key)
	End Method

	Rem
	bbdoc: SRANDMEMBER: Get a random member from a set.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/srandmember
	End Rem
	Method SRANDMEMBER_:TRedisReply(key:String)
		Return Command("SRANDMEMBER", key)
	End Method

	Rem
	bbdoc: SREM: Remove a member from a set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/srem
	End Rem
	Method SREM_:TRedisReply(key:String, member:String)
		Return Command("SREM", key, member)
	End Method

	Rem
	bbdoc: SUNION: Add multiple sets.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/sunion
	End Rem
	Method SUNION_:TRedisReply(keys:String[])
		Return Command("SUNION", keys)
	End Method

	Rem
	bbdoc: SUNIONSTORE: Add multiple sets and store the resulting set in a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/sunionstore
	End Rem
	Method SUNIONSTORE_:TRedisReply(destination:String, keys:String[])
		Return Command("SUNIONSTORE", destination, keys)
	End Method

	Rem
	bbdoc: ZADD: Add a member to a sorted set, or update its score if it already exists.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zadd
	End Rem
	Method ZADD_:TRedisReply(key:String, score:Double, member:String)
		Return Command("ZADD", key, score, member)
	End Method

	Rem
	bbdoc: ZCARD: Get the number of members in a sorted set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zcard
	End Rem
	Method ZCARD_:TRedisReply(key:String)
		Return Command("ZCARD", key)
	End Method

	Rem
	bbdoc: ZCOUNT: Count the members in a sorted set with scores within the given values.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zcount
	End Rem
	Method ZCOUNT_:TRedisReply(key:String, min_:Double, max_:Double)
		Return Command("ZCOUNT", key, min_, max_)
	End Method

	Rem
	bbdoc: ZINCRBY: Increment the score of a member in a sorted set.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/zincrby
	End Rem
	Method ZINCRBY_:TRedisReply(key:String, increment:Double, member:String)
		Return Command("ZINCRBY", key, increment, member)
	End Method

	Rem
	bbdoc: ZINTERSTORE: Intersect multiple sorted sets and store the resulting sorted set in a new key.
	Returns an Integer reply.
	AGGREGATE must be set to one of "SUM", "MIN" or "MAX", if specified.
	Documentation: http://www.redis.io/commands/zinterstore
	End Rem
	Method ZINTERSTORE_:TRedisReply(destination:String, keys:String[], WEIGHTS:String[], AGGREGATE:String = Null)
		Local sb:TStringBuffer = New TStringBuffer(128)
		
		sb.Append("ZINTERSTORE ").Append(destination).Append(keys.length)
		
		For Local key:String = EachIn keys
			sb.Append(" ").Append(key)
		Next
		
		If weights Then
			sb.Append(" WEIGHTS")
			For Local weight:String = EachIn WEIGHTS
				sb.Append(" ").Append(weight)
			Next
		End If
		If AGGREGATE Then
			sb.Append(" AGGREGATE ").Append(AGGREGATE)
		End If
		
		Return Command(sb)
	End Method

	Rem
	bbdoc: ZRANGE: Return a range of members in a sorted set, by index.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/zrange
	End Rem
	Method ZRANGE_:TRedisReply(key:String, start:Int, stop:Int, withScores:Int = False)
		If withScores Then
			Return Command("ZRANGE", key, start, stop, "WITHSCORES")
		Else
			Return Command("ZRANGE", key, start, stop)
		End If
	End Method

	Rem
	bbdoc: ZRANGEBYSCORE: Return a range of members in a sorted set, by score.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/zrangebyscore
	End Rem
	Method ZRANGEBYSCORE_:TRedisReply(key:String, _min:String, _max:String, WITHSCORES:Int = False, offset:Int = 0, count:Int = 0)
		Local sb:TStringBuffer = New TStringBuffer(128)

		sb.Append("ZRANGEBYSCORE ").Append(key)
		sb.Append(" ").Append(_min)
		sb.Append(" ").Append(_max)
		
		If WITHSCORES Then
			sb.Append(" WITHSCORES")
		End If
		If count Then
			sb.Append(" LIMIT ").Append(offset).Append(" ").Append(count)
		End If
		
		Return Command(sb)
	End Method

	Rem
	bbdoc: ZRANK: Determine the index of a member in a sorted set.
	Documentation: http://www.redis.io/commands/zrank
	End Rem
	Method ZRANK_:TRedisReply(key:String, member:String)
		Return Command("ZRANK", key, member)
	End Method

	Rem
	bbdoc: ZREM: Remove a member from a sorted set.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zrem
	End Rem
	Method ZREM_:TRedisReply(key:String, member:String)
		Return Command("ZREM", key, member)
	End Method

	Rem
	bbdoc: ZREMRANGEBYRANK: Remove all members in a sorted set within the given indexes.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zremrangebyrank
	End Rem
	Method ZREMRANGEBYRANK_:TRedisReply(key:String, start:Int, stop:Int)
		Return Command("ZREMRANGEBYRANK", key, start, stop)
	End Method

	Rem
	bbdoc: ZREMRANGEBYSCORE: Remove all members in a sorted set within the given scores.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zremrangebyscore
	End Rem
	Method ZREMRANGEBYSCORE_:TRedisReply(key:String, min_:Double, max_:Double)
		Return Command("ZREMRANGEBYSCORE", key, min_, max_)
	End Method

	Rem
	bbdoc: ZREVRANGE: Return a range of members in a sorted set, by index, with scores ordered from high to low.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/zrevrange
	End Rem
	Method ZREVRANGE_:TRedisReply(key:String, start:Int, stop:Int, WITHSCORES:Int)
		If WITHSCORES Then
			Return Command("ZREVRANGE", key, start, stop, "WITHSCORES")
		Else
			Return Command("ZREVRANGE", key, start, stop)
		End If
	End Method

	Rem
	bbdoc: ZREVRANGEBYSCORE: Return a range of members in a sorted set, by score, with scores ordered from high to low.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/zrevrangebyscore
	End Rem
	Method ZREVRANGEBYSCORE_:TRedisReply(key:String, _min:String, _max:String, WITHSCORES:Int = False, offset:Int = 0, count:Int = 0)
		Local sb:TStringBuffer = New TStringBuffer(128)

		sb.Append("ZREVRANGEBYSCORE ").Append(key)
		sb.Append(" ").Append(_min)
		sb.Append(" ").Append(_max)
		
		If WITHSCORES Then
			sb.Append(" WITHSCORES")
		End If
		If count Then
			sb.Append(" LIMIT ").Append(offset).Append(" ").Append(count)
		End If
		
		Return Command(sb)
	End Method

	Rem
	bbdoc: ZREVRANK: Determine the index of a member in a sorted set, with scores ordered from high to low.
	Documentation: http://www.redis.io/commands/zrevrank
	End Rem
	Method ZREVRANK_:TRedisReply(key:String, member:String)
		Return Command("ZREVRANK", key, member)
	End Method

	Rem
	bbdoc: ZSCORE: Get the score associated with the given member in a sorted set.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/zscore
	End Rem
	Method ZSCORE_:TRedisReply(key:String, member:String)
		Return Command("ZSCORE", key, member)
	End Method

	Rem
	bbdoc: ZUNIONSTORE: Add multiple sorted sets and store the resulting sorted set in a new key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/zunionstore
	End Rem
	Method ZUNIONSTORE_:TRedisReply(destination:String, keys:String[], WEIGHTS:String[], AGGREGATE:String = Null)
		Local sb:TStringBuffer = New TStringBuffer(128)

		sb.Append("ZUNIONSTORE ").Append(destination)
		sb.Append(" ").Append(keys.length)
		
		For Local key:String = EachIn keys
			sb.Append(" ").Append(key)
		Next
		
		If weights Then
			sb.Append(" WEIGHTS")
			
			For Local weight:String = EachIn WEIGHTS
				sb.Append(" ").Append(weight)
			Next
		End If
		If AGGREGATE Then
			sb.Append(" AGGREGATE ").Append(AGGREGATE)
		End If
		
		Return Command(sb)
	End Method

	Rem
	bbdoc: APPEND: Append a value to a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/append
	End Rem
	Method APPEND_:TRedisReply(key:String, value:String)
		Return Command("APPEND", key, value)
	End Method

	Rem
	bbdoc: DECR: Decrement the integer value of a key by one.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/decr
	End Rem
	Method DECR_:TRedisReply(key:String)
		Return Command("DECR", key)
	End Method

	Rem
	bbdoc: DECRBY: Decrement the integer value of a key by the given number.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/decrby
	End Rem
	Method DECRBY_:TRedisReply(key:String, decrement:Int)
		Return Command("DECRBY", key, decrement)
	End Method

	Rem
	bbdoc: GET: Get the value of a key.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/get
	End Rem
	Method GET_:TRedisReply(key:String)
		Return Command("GET", key)
	End Method

	Rem
	bbdoc: GETBIT: Returns the bit value at offset in the string value stored at key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/getbit
	End Rem
	Method GETBIT_:TRedisReply(key:String, offset:Int)
		Return Command("GETBIT", key, offset)
	End Method

	Rem
	bbdoc: GETRANGE: Get a substring of the string stored at a key.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/getrange
	End Rem
	Method GETRANGE_:TRedisReply(key:String, start:Int, stop:Int)
		Return Command("GETRANGE", key, start, stop)
	End Method

	Rem
	bbdoc: GETSET: Set the string value of a key and return its old value.
	Returns a Bulk reply.
	Documentation: http://www.redis.io/commands/getset
	End Rem
	Method GETSET_:TRedisReply(key:String, value:String)
		Return Command("GETSET", key, value)
	End Method

	Rem
	bbdoc: INCR: Increment the integer value of a key by one.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/incr
	End Rem
	Method INCR_:TRedisReply(key:String)
		Return Command("INCR", key)
	End Method

	Rem
	bbdoc: INCRBY: Increment the integer value of a key by the given number.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/incrby
	End Rem
	Method INCRBY_:TRedisReply(key:String, increment:Int)
		Return Command("INCRBY", key, increment)
	End Method

	Rem
	bbdoc: MGET: Get the values of all the given keys.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/mget
	End Rem
	Method MGET_:TRedisReply(keys:String[])
		Return Command("MGET", keys)
	End Method

	Rem
	bbdoc: MSET: Set multiple keys to multiple values.
	Returns a Status code reply.
	The key_value parameter is an array of even length, containing pairs of keys and values: [key1, value1, key2, value2...].
	Documentation: http://www.redis.io/commands/mset
	End Rem
	Method MSET_:TRedisReply(key_value:String[])
		Return Command("MSET", key_value)
	End Method

	Rem
	bbdoc: MSETNX: Set multiple keys to multiple values, only if none of the keys exist.
	Returns an Integer reply.
	The key_value parameter is an array of even length, containing pairs of keys and values: [key1, value1, key2, value2...].
	Documentation: http://www.redis.io/commands/msetnx
	End Rem
	Method MSETNX_:TRedisReply(key_value:String[])
		Return Command("MSETNX", key_value)
	End Method

	Rem
	bbdoc: SET: Set the string value of a key.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/set
	End Rem
	Method SET_:TRedisReply(key:String, value:String)
		Return Command("SET", key, value)
	End Method

	Rem
	bbdoc: SETBIT: Sets or clears the bit at offset in the string value stored at key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/setbit
	End Rem
	Method SETBIT_:TRedisReply(key:String, offset:Int, value:Int)
		Return Command("SETBIT", key, offset, value)
	End Method

	Rem
	bbdoc: SETEX: Set the value and expiration of a key.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/setex
	End Rem
	Method SETEX_:TRedisReply(key:String, seconds:Int, value:String)
		Return Command("SETEX", key, seconds, value)
	End Method

	Rem
	bbdoc: SETNX: Set the value of a key, only if the key does not exist.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/setnx
	End Rem
	Method SETNX_:TRedisReply(key:String, value:String)
		Return Command("SETNX", key, value)
	End Method

	Rem
	bbdoc: SETRANGE: Overwrite part of a string at key starting at the specified offset.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/setrange
	End Rem
	Method SETRANGE_:TRedisReply(key:String, offset:Int, value:String)
		Return Command("SETRANGE", key, offset, value)
	End Method

	Rem
	bbdoc: STRLEN: Get the length of the value stored in a key.
	Returns an Integer reply.
	Documentation: http://www.redis.io/commands/strlen
	End Rem
	Method STRLEN_:TRedisReply(key:String)
		Return Command("STRLEN", key)
	End Method

	Rem
	bbdoc: DISCARD: Discard all commands issued after MULTI.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/discard
	End Rem
	Method DISCARD_:TRedisReply()
		Return Command("DISCARD")
	End Method

	Rem
	bbdoc: EXEC: Execute all commands issued after MULTI.
	Returns a Multi-bulk reply.
	Documentation: http://www.redis.io/commands/exec
	End Rem
	Method EXEC_:TRedisReply()
		Return Command("EXEC")
	End Method

	Rem
	bbdoc: MULTI: Mark the start of a transaction block.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/multi
	End Rem
	Method MULTI_:TRedisReply()
		Return Command("MULTI")
	End Method

	Rem
	bbdoc: UNWATCH: Forget about all watched keys.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/unwatch
	End Rem
	Method UNWATCH_:TRedisReply()
		Return Command("UNWATCH")
	End Method

	Rem
	bbdoc: WATCH: Watch the given keys to determine execution of the MULTI/EXEC block.
	Returns a Status code reply.
	Documentation: http://www.redis.io/commands/watch
	End Rem
	Method WATCH_:TRedisReply(keys:String[])
		Return Command("WATCH", keys)
	End Method

End Type

Rem
bbdoc: The reply from a successfully executed redis command.
End Rem
Type TRedisReply

	Field replyPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Field kind:Int
	Rem
	bbdoc: 
	End Rem
	Field intValue:Long
	Rem
	bbdoc: 
	End Rem
	Field elements:Size_T
	
	Function _create:TRedisReply(replyPtr:Byte Ptr, kind:Int, intValue:Long, elements:Size_T) { nomangle }
		If replyPtr Then
			Local this:TRedisReply = New TRedisReply
			this.replyPtr = replyPtr
			this.kind = kind
			this.intValue = intValue
			this.elements = elements
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetString:String()
		If REDIS_REPLY_STRING Then
			Return bmx_redis_reply_getstring(replyPtr)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetElement:TRedisReply(index:Int)
		If index < elements Then
			Return bmx_redis_reply_element(replyPtr, index)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ToString:String()
		Select kind
			Case REDIS_REPLY_STRING
				Return GetString()
			Case REDIS_REPLY_ARRAY
				Local sb:TStringBuffer = New TStringBuffer
				For Local i:Int = 0 Until elements
					If i Then
						sb.Append("~n")
					End If
					sb.Append(GetElement(i).ToString())
				Next
				Return sb.ToString()
			Case REDIS_REPLY_INTEGER
				Return intValue
			Case REDIS_REPLY_STATUS
				Return GetString()
			Case REDIS_REPLY_ERROR
				Return "ERROR : " + GetString()
		End Select
		
		Return ""
	End Method

	Method Delete()
		If replyPtr Then
			freeReplyObject(replyPtr)
			replyPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRedisError Extends TRuntimeException

	Function Create:TRedisError( error$ )
		Local t:TRedisError = New TRedisError
		t.error = error
		Return t
	End Function

End Type

Extern
	Function bmx_redis_reply_element:TRedisReply(handle:Byte Ptr, index:Int)
	
	Function bmx_redis_command_ss:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt:Byte Ptr)
	Function bmx_redis_command_s:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr)
	Function bmx_redis_command_si:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, param:Int)
	Function bmx_redis_command_ssi:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt:Byte Ptr, num:Int)
	Function bmx_redis_command_sss:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, txt2:Byte Ptr)
	Function bmx_redis_command_sssi:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, txt2:Byte Ptr, num:Int)
	Function bmx_redis_command_ssis:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, num:Int, txt2:Byte Ptr)
	Function bmx_redis_command_ssds:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, num:Double, txt2:Byte Ptr)
	Function bmx_redis_command_ssii:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, num1:Int, num2:Int)
	Function bmx_redis_command_ssdd:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, num1:Double, num2:Double)
	Function bmx_redis_command_ssss:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, txt2:Byte Ptr, txt3:Byte Ptr)
	Function bmx_redis_command_sssss:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr, txt1:Byte Ptr, txt2:Byte Ptr, txt3:Byte Ptr, txt4:Byte Ptr)
	Function bmx_redis_command_a:TRedisReply(handle:Byte Ptr, cmd:Byte Ptr)
End Extern

?

