' Copyright (c) 2007-2013 Bruce A Henderson
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

SuperStrict

Import BRL.Map
Import BRL.Stream
Import BRL.LinkedList

Import "common.bmx"


Rem
bbdoc: Sets up the program environment that libcurl needs.
about: This will be run internally on the creation of the first #TCurlEasy with default values, but you can
call it yourself if you need to set anything specifically.
<p>
The flags option is a bit pattern that tells libcurl exactly what features to init, as described below.
Set the desired bits by ORing the values together. In normal operation, you must specify CURL_GLOBAL_ALL.
Don't use any other value unless you are familiar with and mean to control internal operations of libcurl.
</p>
<p>Available options:</p>
<ul>
<li>CURL_GLOBAL_ALL</li>
<li>CURL_GLOBAL_SSL</li>
<li>CURL_GLOBAL_WIN32</li>
<li>CURL_GLOBAL_NOTHING</li>
</ul>
End Rem
Function CurlGlobalInit:Int(flags:Int)
	Return curl_global_init(flags)
End Function

Rem
bbdoc: Returns a string describing a libcurl error code.
End Rem
Function CurlError:String(errorCode:Int)
	Return String.fromCString(curl_easy_strerror(errorCode))
End Function

Type TCurlHasLists Abstract

	Field sLists:Byte Ptr[]

	Method freeLists()
		If sLists Then
		
			For Local i:Int = 0 Until sLists.length
			
				bmx_curl_slist_free(sLists[i])
			
			Next
			
			sLists = Null
		
		End If
	End Method

End Type

Rem
bbdoc: The libcurl easy interface.
about: When using libcurl's "easy" interface you create your new session and get a #TCurlEasy handle
(often referred to as an "easy handle").
<p>
You then set all the options you want in the upcoming transfer, the most important among them is the
URL itself (you can't transfer anything without a specified URL as you may have figured out yourself).
You might want to set some callbacks as well that will be called from the library when data is
available etc. There are various methods to use depending on the type of option you want to set. They are,
#setOptInt, #setOptLong, #setOptBytePtr, #setOptString, #setPrivate, #setProgressCallback, 
#setDebugCallback, #setWriteStream, #setWriteCallback, #setWriteString, #setReadStream, #setReadCallback,
#setHeaderCallback, #httpPost, #httpHeader, #http200Aliases, #preQuote, #quote and #postQuote.
</p>
<p>
When all is setup, you tell libcurl to perform the transfer using #perform. It will then do the entire
operation and won't return until it is done (successfully or not).
</p>
<p>
After the transfer has been made, you can set new options and make another transfer, or if you're done,
cleanup the session by calling #cleanup. If you want persistent connections, you don't cleanup immediately,
but instead run ahead and perform other transfers using the same easy handle.
</p>
End Rem
Type TCurlEasy Extends TCurlHasLists

	Field easyHandlePtr:Byte Ptr
	
	' keeps strings alive for as long as we need them...
	Field stringMap:TMap = New TMap
	
	' the transfer data as a string...
	Field data:String
	
	Rem
	bbdoc: 
	returns: A new instance of #TCurlEasy or Null if the libcurl handle could not be created.
	End Rem
	Function Create:TCurlEasy()
	
		Local this:TCurlEasy = New TCurlEasy
		
		this.easyHandlePtr = curl_easy_init()
		
		If Not this.easyHandlePtr Then
			Return Null
		End If
		
		Return this
	
	End Function

	Method setOpt(option:Int, parameter:Byte Ptr)
		If easyHandlePtr Then
			curl_easy_setopt(easyHandlePtr, option, parameter)
		End If
	End Method

	Rem
	bbdoc: Data that should be associated with this curl handle.
	about: The data can subsequently be retrieved using #getInfo with the #privateData method.
	libcurl itself does nothing with this data.
	<p>
	Use this instead of setopt #CURLOPT_PRIVATE
	</p>
	End Rem
	Method setPrivate(data:Object)
		If easyHandlePtr Then
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_PRIVATE, data)
		End If
	End Method

	Rem
	bbdoc: Sets a particular curl @option with the int @parameter.
	End Rem
	Method setOptInt(option:Int, parameter:Int)
		If easyHandlePtr Then
			bmx_curl_easy_setopt_long(easyHandlePtr, option, parameter)
		End If
	End Method

	Rem
	bbdoc: Sets a particular curl @option with the long @parameter.
	End Rem
	Method setOptLong(option:Int, parameter:Long)
		If easyHandlePtr Then
			bmx_curl_easy_setopt_bbint64(easyHandlePtr, option, parameter)
		End If
	End Method

	Rem
	bbdoc: Sets a particular curl @option with the byte ptr @parameter.
	End Rem
	Method setOptBytePtr(option:Int, parameter:Byte Ptr)
		If easyHandlePtr Then
			curl_easy_setopt(easyHandlePtr, option, parameter)
		End If
	End Method

	Rem
	bbdoc: Sets a particular curl @option with the String @parameter.
	End Rem
	Method setOptString(option:Int, parameter:String)
		If easyHandlePtr Then
			' strings need to be alive for as long as libcurl needs them... so we cache them.
			
			Local s:Byte Ptr
			Local opt:TCurlInt = TCurlInt(stringMap.ValueForKey(String(option)))
			
			If opt Then
				' done with this one... free it up.
				If opt.s Then
					MemFree(opt.s)
				End If
			Else
				opt = New TCurlInt
				opt.opt = option
				stringMap.insert(String(option), opt)
			End If
		
			If parameter Then
				opt.s = parameter.toCString()
				curl_easy_setopt(easyHandlePtr, option, opt.s)
			Else
				opt.s = Null
				curl_easy_setopt(easyHandlePtr, option, Null)
			End If
		End If
	End Method

	Rem
	bbdoc: Perform a file transfer.
	returns: 0 when successful, or non-zero when an error occured.
	about: This method is called after all the @setOpt calls are made, and will perform the transfer as described in the options.
	You can do any amount of calls to #perform while using the same handle. If you intend to transfer more than one
	file, you are even encouraged to do so. libcurl will then attempt to re-use the same connection for the
	following transfers, thus making the operations faster, less CPU intense and using less network resources.<br>
	Just note that you will have to use @setopt between the invokes to set options for the following #perform.
	End Rem
	Method perform:Int()
		If easyHandlePtr Then
			Return curl_easy_perform(easyHandlePtr)
		End If
	End Method
	
	Rem
	bbdoc: End a libcurl easy session.
	about: This function must be the last function to call for an easy session.<br>
	This will effectively close all connections this handle has used and possibly has kept open until now.
	Don't call this method if you intend to transfer more files.<br>
	BlitzMax will call this method when the instance of the #TCurlEasy object is garbage collected, but you should
	probably call it yourself when you are done with the session. Once called, create yourself a new #TCurlEasy
	object when you need a new connection.
	End Rem
	Method cleanup()
	
		If easyHandlePtr Then
			curl_easy_cleanup(easyHandlePtr)
			easyHandlePtr = Null
		End If
		
		' free up the slists
		freeLists()

		' cleanup strings
		For Local i:TCurlInt = EachIn stringMap.values()
		
			MemFree(i.s)
		
		Next
		stringMap.clear()
		
		' cleanup callbacks and related data
		dbData = Null
		dbFunction = Null
		wrData = Null
		wrFunction = Null
		rdData = Null
		rdFunction = Null
		hrData = Null
		hrFunction = Null
		
	End Method
	
	Rem
	bbdoc: Reset all options of a libcurl session handle.
	about: Re-initializes all options previously set on a specified CURL handle to the default values.
	This puts back the handle to the same state as it was in when it was first created.<br>
	It does not change the following information kept in the handle: live connections, the Session ID cache,
	the DNS cache, the cookies and shares.
	End Rem
	Method reset()
		If easyHandlePtr Then
			curl_easy_reset(easyHandlePtr)
		End If
	End Method
	
	Rem
	bbdoc: Get transfer progress information.
	about: This function gets called by libcurl instead of its internal equivalent with a frequent
	interval during operation (roughly once per second) no matter if data is being transfered or not.
	Unknown/unused argument values passed to the callback will be set to zero (like if you only download
	data, the upload size will remain 0). Returning a non-zero value from this callback will cause libcurl
	to abort the transfer and return #CURLE_ABORTED_BY_CALLBACK.
	<p>
	If you transfer data with the multi interface, this function will not be called during periods of
	idleness unless you call the appropriate libcurl function that performs transfers. Usage of the
	this callback is not recommended when using the multi interface.
	</p>
	<p><i>Notes: </i>This is a convenience method for using setopt with #CURLOPT_PROGRESSFUNCTION and #CURLOPT_PROGRESSDATA.</p>
	End Rem
	Method setProgressCallback(progressFunction:Int(data:Object, dltotal:Double, dlnow:Double, ultotal:Double, ulnow:Double), data:Object = Null)
		If easyHandlePtr Then
			' enable progress callback
			setOptInt(CURLOPT_NOPROGRESS, 0)
			
			' set the callback
			curl_easy_setopt(easyHandlePtr, CURLOPT_PROGRESSFUNCTION, progressFunction)
			
			' set user data, if required
			If data Then
				bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_PROGRESSDATA, data)
			End If
		End If
	End Method
	
	Rem
	bbdoc: Allows the replacement of the standard debug function used when #CURLOPT_VERBOSE is in effect.
	about: This callback receives debug information, as specified with the msgType argument. The function must
	return 0.
	<p>
	Possible msgTypes include, #CURLINFO_TEXT, #CURLINFO_HEADER_IN, #CURLINFO_HEADER_OUT, #CURLINFO_DATA_IN
	and #CURLINFO_DATA_OUT.
	</p>
	<p><i>Notes: </i>This is a convenience method for using setopt with #CURLOPT_DEBUGFUNCTION and #CURLOPT_DEBUGDATA.</p>
	End Rem
	Method setDebugCallback(debugFunction:Int(data:Object, msgType:Int, message:String), data:Object = Null)
		If easyHandlePtr Then
		
			dbFunction = debugFunction
			dbData = data
		
			curl_easy_setopt(easyHandlePtr, CURLOPT_DEBUGFUNCTION, dbCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_DEBUGDATA, Self)
		End If
	End Method
	
	' +++++
	Field dbData:Object
	Field dbFunction:Int(data:Object, msgType:Int, message:String)
	
	Function dbCallback:Int(handle:Byte Ptr, infotype:Int, msg:Byte Ptr, size:Int, data:Object)

		Return TCurlEasy(data).dbFunction(TCurlEasy(data).dbData, infotype, String.fromBytes(msg, size))
		
	End Function
	' +++++
	
	Rem
	bbdoc: Specify the stream to use for writing.
	about: Setting this will override a previous call to #setWriteCallback or #setWriteString.
	End Rem
	Method setWriteStream(stream:TStream)
		If easyHandlePtr Then

			curl_easy_setopt(easyHandlePtr, CURLOPT_WRITEFUNCTION, writeStreamCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_WRITEDATA, stream)
	
		End If		
	End Method

	
	Function writeStreamCallback:Int(buffer:Byte Ptr, size:Int, nmemb:Int, stream:Object)
		Return TStream(stream).writeBytes(buffer, size * nmemb)
	End Function

	Rem
	bbdoc: Sets up a callback for writing (incoming data).
	about: This function gets called by libcurl as soon as there is data received that needs to be saved.
	The size of the data pointed to by @buffer is @size, it will not be zero terminated. Return the
	number of bytes actually taken care of. If that amount differs from the amount passed to your
	function, it'll signal an error to the library and it will abort the transfer and return
	#CURLE_WRITE_ERROR.
	<p>
	This function may be called with zero bytes data if the transfered file is empty. 
	</p>
	<p>Setting this will override a previous call to #setWriteStream or #setWriteString.</p>
	End Rem
	Method setWriteCallback(writeFunction:Int(buffer:Byte Ptr, size:Int, data:Object), data:Object = Null)
		If easyHandlePtr Then
		
			wrFunction = writeFunction
			wrData = data
		
			curl_easy_setopt(easyHandlePtr, CURLOPT_WRITEFUNCTION, wrCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_WRITEDATA, Self)
		End If
	End Method

	' +++++
	Field wrData:Object
	Field wrFunction:Int(buffer:Byte Ptr, size:Int, data:Object)

	Function wrCallback:Int(buffer:Byte Ptr, size:Int, nmemb:Int, data:Object)

		Return TCurlEasy(data).wrFunction(buffer, size * nmemb, TCurlEasy(data).wrData)
		
	End Function
	' +++++
	
	Rem
	bbdoc: Data is retrieved into a String.
	about: Use toString() to get the data once the transfer is complete.
	<p>Setting this will override a previous call to #setWriteStream or #setWriteCallback.</p>
	End Rem
	Method setWriteString()
		data = Null
		setWriteCallback(writeStringFunction, Self)
	End Method
	
	Function writeStringFunction:Int(buffer:Byte Ptr, size:Int, curl:Object)
		TCurlEasy(curl).data:+ String.FromBytes( buffer, size )
		Return size
	End Function
	
	Rem
	bbdoc: Specify the stream to use for reading.
	about: Setting this will override a previous call to #setReadCallback.
	End Rem
	Method setReadStream(stream:TStream)
		If easyHandlePtr Then

			curl_easy_setopt(easyHandlePtr, CURLOPT_READFUNCTION, readStreamCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_READDATA, stream)
	
		End If		
	End Method

	Function readStreamCallback:Int(buffer:Byte Ptr, size:Int, nmemb:Int, stream:Object)
		Try
			Return TStream(stream).read(buffer, size * nmemb)
		Catch e:TStreamReadException
			Return CURL_READFUNC_ABORT
		End Try
	End Function

	Rem
	bbdoc: Sets up a callback for reading (outgoing data).
	about: This function gets called by libcurl as soon as it needs to read data in order to send it
	to the peer. The data area pointed at by the buffer may be filled with at most @size bytes. Your
	function must return the actual number of bytes that you stored in that memory area.<br>
	Returning 0 will signal end-of-file to the library and cause it to stop the current transfer.
	<p>
	If you stop the current transfer by returning 0 "pre-maturely" (i.e before the server expected it,
	like when you've told you will upload N bytes and you upload less than N bytes), you may experience
	that the server "hangs" waiting for the rest of the data that won't come.
	</p>
	<p>
	The read callback may return #CURL_READFUNC_ABORT to stop the current operation immediately,
	resulting in a #CURLE_ABORTED_BY_CALLBACK error code from the transfer.
	</p>
	<p>Setting this will override a previous call to #setReadStream.</p>
	End Rem
	Method setReadCallback(readFunction:Int(buffer:Byte Ptr, size:Int, data:Object), data:Object = Null)
		If easyHandlePtr Then
		
			rdFunction = readFunction
			rdData = data
		
			curl_easy_setopt(easyHandlePtr, CURLOPT_READFUNCTION, rdCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_READDATA, Self)
		End If
	End Method

	' +++++
	Field rdData:Object
	Field rdFunction:Int(buffer:Byte Ptr, size:Int, data:Object)

	Function rdCallback:Int(buffer:Byte Ptr, size:Int, nmemb:Int, data:Object)

		Return TCurlEasy(data).rdFunction(buffer, size * nmemb, TCurlEasy(data).rdData)
		
	End Function

	Rem
	bbdoc: Sets up a callback for header data (incoming).
	about: This function gets called by libcurl as soon as it has received header data. The header
	callback will be called once for each header and only complete header lines are passed on to the
	callback. Parsing headers should be easy enough using this. The size of the data pointed to by buffed
	is @size bytes. Do not assume that the header line is zero terminated! The callback function must
	return the number of bytes actually taken care of, or return -1 to signal error to the library
	(it will cause it to abort the transfer with a #CURLE_WRITE_ERROR return code).
	<p>
	When a server sends a chunked encoded transfer, it may contain a trailer. That trailer is identical
	to a HTTP header and if such a trailer is received it is passed to the application using this
	callback as well. There are several ways to detect it being a trailer and not an ordinary header:
	<ol>
	<li>it comes after the response-body.</li>
	<li>it comes after the final header line (CR LF)</li>
	<li>a Trailer: header among the response-headers mention what header to expect in the trailer.</li>
	</ol>
	</p>
	End Rem
	Method setHeaderCallback(headerFunction:Int(buffer:Byte Ptr, size:Int, data:Object), data:Object = Null)
		If easyHandlePtr Then
		
			hrFunction = headerFunction
			hrData = data
		
			curl_easy_setopt(easyHandlePtr, CURLOPT_HEADERFUNCTION, hrCallback)
			bmx_curl_easy_setopt_obj(easyHandlePtr, CURLOPT_WRITEHEADER, Self)
		End If
	End Method

	' +++++
	Field hrData:Object
	Field hrFunction:Int(buffer:Byte Ptr, size:Int, data:Object)

	Function hrCallback:Int(buffer:Byte Ptr, size:Int, nmemb:Int, data:Object)

		Return TCurlEasy(data).hrFunction(buffer, size * nmemb, TCurlEasy(data).hrData)
		
	End Function
	' +++++

	Rem
	bbdoc: Tells libcurl you want a multipart/formdata HTTP POST to be made, with the specified data in @formData.
	about: Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue" header. You can disable
	this header with #CURLOPT_HTTPHEADER as usual.
	<p>See #TCurlFormData for details of specific data. The #TCurlFormData object MUST be available until
	this curl object is cleaned up / deleted.</p>
	<p>
	Calling this method automatically sets #CURLOPT_NOBODY to 0.<br>
	Use this instead of setopt #CURLOPT_HTTPPOST.
	</p>
	End Rem
	Method httpPost(formData:TCurlFormData)
		If easyHandlePtr Then
			bmx_curl_easy_setopt_httppost(easyHandlePtr, formData.httppostPtr);
		End If
	End Method
	
	Rem
	bbdoc: A list of HTTP headers to pass to the server in your HTTP request.
	about: If you add a header that is otherwise generated and used by libcurl internally, your added one will be
	used instead. If you add a header with no contents as in 'Accept:' (no data on the right side of the colon),
	the internally used header will get disabled. Thus, using this option you can add new headers, replace
	internal headers and remove internal headers. To add a header with no contents, make the contents be
	two quotes: "" (~q~q). The headers included in the list must not be CRLF-terminated, because curl
	adds CRLF after each header item. Failure to comply with this will result in strange bugs because the server
	will most likely ignore part of the headers you specified.
	<p>
	The first line in a request (containing the method, usually a GET or POST) is not a header and cannot be
	replaced using this option. Only the lines following the request-line are headers. Adding this method
	line in this list of headers will only cause your request to send an invalid header.
	</p>
	<p>Pass a Null to this to reset back to no custom headers.</p>
	<p>
	The most commonly replaced headers have "shortcuts" in the options #CURLOPT_COOKIE, #CURLOPT_USERAGENT and
	#CURLOPT_REFERER.
	<p>
	<p>Use this instead of setopt #CURLOPT_HTTPHEADER.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method httpHeader(headers:String[])
		If easyHandlePtr Then
			If headers Then
				processArray(CURLOPT_HTTPHEADER, headers)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_HTTPHEADER, Null)
			End If
		End If
	End Method
	
	Rem
	bbdoc: A list of aliases to be treated as valid HTTP 200 responses.
	about: Some servers respond with a custom header response line. For example, IceCast servers respond with
	"ICY 200 OK". By including this string in your list of aliases, the response will be treated as a
	valid HTTP header line such as "HTTP/1.0 200 OK".
	<p>
	The alias itself is not parsed for any version strings. The protocol is assumed to match HTTP 1.0 when an
	alias matched. 
	</p>
	<p>Use this instead of setopt #CURLOPT_HTTP200ALIASES.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method http200Aliases(aliases:String[])
		If easyHandlePtr Then
			If aliases Then
				processArray(CURLOPT_HTTP200ALIASES, aliases)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_HTTP200ALIASES, Null)
			End If
		End If
	End Method
	
	Rem
	bbdoc: A list of FTP commands to pass to the server after the transfer type is set.
	about: Disable this operation again by passing Null to this method.
	<p>Use this instead of setopt #CURLOPT_PREQUOTE.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method preQuote(commands:String[])
		If easyHandlePtr Then
			If commands Then
				processArray(CURLOPT_PREQUOTE, commands)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_PREQUOTE, Null)
			End If
		End If
	End Method
	
	Rem
	bbdoc: A list of FTP or SFTP commands to pass to the server prior to your ftp request.
	about: This will be done before any other commands are issued (even before the CWD command for FTP).
	Disable this operation again by passing Null to this method. The valid SFTP commands are: chgrp, chmod, chown,
	ln, mkdir, pwd, rename, rm, rmdir, symlink.
	<p>Use this instead of setopt #CURLOPT_QUOTE.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method quote(commands:String[])
		If easyHandlePtr Then
			If commands Then
				processArray(CURLOPT_QUOTE, commands)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_QUOTE, Null)
			End If
		End If
	End Method

	Rem
	bbdoc: A list of FTP or SFTP commands to pass to the server after your ftp transfer request.
	about: Disable this operation again by passing Null to this method.
	<p>Use this instead of setopt #CURLOPT_POSTQUOTE.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method postQuote(commands:String[])
		If easyHandlePtr Then
			If commands Then
				processArray(CURLOPT_POSTQUOTE, commands)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_POSTQUOTE, Null)
			End If
		End If
	End Method

	Rem
	bbdoc: A list of Telnet variables to pass to the telnet negotiations.
	about: The variables should be in the format <option=value>. libcurl supports the options
	'TTYPE', 'XDISPLOC' and 'NEW_ENV'. See the TELNET standard for details.
	<p>Use this instead of setopt #CURLOPT_TELNETOPTIONS.</p>
	<p>After the call to #perform, you should call #freeLists to free up the internal structs.</p>
	End Rem
	Method telnetOptions(variables:String[])
		If easyHandlePtr Then
			If variables Then
				processArray(CURLOPT_TELNETOPTIONS, variables)
			Else
				curl_easy_setopt(easyHandlePtr, CURLOPT_TELNETOPTIONS, Null)
			End If
		End If
	End Method
	
	Rem
	bbdoc: Returns a #TCurlInfo which provides access to internal curl session information.
	about: Use this method AFTER a performed transfer if you want to get transfer- oriented data.
	End Rem
	Method getInfo:TCurlInfo()
		If easyHandlePtr Then
			Return TCurlInfo._create(easyHandlePtr)
		End If
		Return Null
	End Method
	
	Rem
	bbdoc: Call this to free up the internal lists after calling #perform.
	about: The methods #httpHeader, #http200Aliases, #preQuote and #quote all create lists that are required to
	persist during #perform, but after which will need to be freed up.<br>
	Of course, if you intend reusing them for a subsequent perform, then don't call this method until you are ready.
	<p>
	This method is also called as part of #cleanup.
	</p>
	End Rem
	Method freeLists()
		Super.freeLists()
	End Method
	
	Rem
	bbdoc: Converts the given input string to an URL encoded string and returns that as a new allocated string.
	returns: The escaped string, or Null if it failed.
	about: All input characters that are not a-z, A-Z or 0-9 are converted to their "URL escaped" version
	(%NN where NN is a two-digit hexadecimal number).
	End Rem
	Method escape:String(text:String)
		If easyHandlePtr Then
			If Not text Then
				text = ""
			End If
			
			Local s:Byte Ptr = text.toCString()
			
			Local s2:Byte Ptr = curl_easy_escape(easyHandlePtr, s, text.length)
			
			' free c string
			MemFree(s)

			If s2 Then

				text = String.fromCString(s2)

				' free curl string
				curl_free(s2)
				
				Return text
			End If
		
		End If	
		Return Null
	End Method
	
	Rem
	bbdoc: Converts the given URL encoded input string to a "plain string" and returns that in an allocated memory area.
	returns: The unescaped string, or Null if it failed.
	about: All input characters that are URL encoded (%XX where XX is a two-digit hexadecimal number)
	are converted to their binary versions.
	End Rem
	Method unEscape:String(text:String)
		If easyHandlePtr Then
			If Not text Then
				text = ""
			End If
			
			Local s:Byte Ptr = text.toCString()
			Local out:Int
			
			Local s2:Byte Ptr = curl_easy_unescape(easyHandlePtr, s, text.length, Varptr out)

			' free c string
			MemFree(s)
			
			If s2 Then

				text = String.fromBytes(s2, out)

				' free curl string
				curl_free(s2)
				
				Return text
			End If
		
		End If	
		Return Null
	End Method
	
	Rem
	bbdoc: Returns the contents of the transfer as a string, if #setWriteString was called.
	End Rem
	Method toString:String()
		Return data
	End Method

	Method Delete()
		cleanup()
	End Method

	Method processArray(option:Int, array:String[])

		If array And array.length > 0 Then	

			Local slist:Byte Ptr = bmx_curl_slist_new()
			
			For Local i:Int = 0 Until array.length
			
				Local txt:Byte Ptr = array[i].toCString()
				
				bmx_curl_add_element(slist, txt)
				
				MemFree(txt)
			
			Next
			
			bmx_curl_easy_setopt_slist(easyHandlePtr, option, slist)
			
			sLists = sLists[..sLists.length + 1]
			
			sLists[sLists.length - 1] = slist
	
		End If
	End Method
	
End Type

' internal :-)
Type TCurlInt
	Field opt:Int
	Field s:Byte Ptr
	Function set:TCurlInt(opt:Int, s:Byte Ptr)
		Local this:TCurlInt = New TCurlInt
		this.opt = opt
		this.s = s
		Return this
	End Function
End Type

Rem
bbdoc: Used to create a set of multipart/formdata for HTTP posts.
about: This is used in conjunction with the #httpPost method.
End Rem
Type TCurlFormData Extends TCurlHasLists 

	Field httppostPtr:Byte Ptr
	
	Field count:Int = 0
	Field ptrs:Byte Ptr[] = New Byte Ptr[0]
	
	Rem
	bbdoc: Creates a new #TCurlFormData object
	End Rem
	Function Create:TCurlFormData()
		Local this:TCurlFormData = New TCurlFormData
		
		this.httppostPtr = bmx_curl_new_httppostPtr()
		
		Return this
	End Function
	
	Rem
	bbdoc: Add simple name/content section, with optional contenttype and/or headers.
	End Rem
	Method addContent(name:String, contents:String, contentType:String = Null, headers:String[] = Null)
		Local n:Byte Ptr = name.toCString()
		Local c:Byte Ptr = contents.toCString()
		Local t:Byte Ptr
		
		addPtr(n)
		addPtr(c)
		
		If contentType Then
			t = contentType.toCString()
			addPtr(t)
			
			bmx_curl_formadd_name_content_type(httppostPtr, n, c, t)
		Else
			bmx_curl_formadd_name_content(httppostPtr, n, c)	
		End If
		
	End Method

	Rem
	bbdoc: Add simple file section, with optional contenttype.
	End Rem
	Method addFile(name:String, file:String, contentType:String = Null, headers:String[] = Null)
		Local n:Byte Ptr = name.toCString()
		Local f:Byte Ptr = file.toCString()
		Local t:Byte Ptr
		
		addPtr(n)
		addPtr(f)
		
		If contentType Then
			t = contentType.toCString()
			addPtr(t)
			
			bmx_curl_formadd_name_file_type(httppostPtr, n, f, t, 1)
		Else
			bmx_curl_formadd_name_file(httppostPtr, n, f, 1)	
		End If
	
	End Method

	Rem
	bbdoc: Add the @content of a file as a normal post text value, with optional contenttype.
	End Rem
	Method addFileContent(name:String, file:String, contentType:String = Null, headers:String[] = Null)
		Local n:Byte Ptr = name.toCString()
		Local f:Byte Ptr = file.toCString()
		Local t:Byte Ptr
		
		addPtr(n)
		addPtr(f)
		
		If contentType Then
			t = contentType.toCString()
			addPtr(t)
			
			bmx_curl_formadd_name_file_type(httppostPtr, n, f, t, 2)
		Else
			bmx_curl_formadd_name_file(httppostPtr, n, f, 2)	
		End If
	
	End Method
	
	Rem
	bbdoc: Add a @buffer of @length bytes to the post.
	End Rem
	Method addBuffer(name:String, bufName:String, buffer:Byte Ptr, length:Int, headers:String[] = Null)
		Local n:Byte Ptr = name.toCString()
		Local b:Byte Ptr = bufName.toCString()
		
		addPtr(n)
		addPtr(b)
		
		bmx_curl_formadd_name_buffer(httppostPtr, n, b, buffer, length)	
	End Method
	
	
	Method addPtr(newPtr:Byte Ptr)
	
		' extend if necessary
		If count = ptrs.length Then
			
			Local tmpPtrs:Byte Ptr[] = New Byte Ptr[ptrs.length + 5]
			
			For Local i:Int = 0 Until count
				tmpPtrs[i] = ptrs[i]
			Next
		
			ptrs = tmpPtrs
		End If
		
		ptrs[count] = newPtr
	
		count:+ 1
	End Method
	
	Method free()
		If httppostPtr Then
			bmx_curl_httppostPtr_delete(httppostPtr)
			httppostPtr = Null
		End If
		
		If ptrs And count > 0 Then
			For Local i:Int = 0 Until count
				MemFree(ptrs[i])
			Next
			ptrs = Null
		End If
		
		freeLists()
	End Method
	
	Method Delete()
		free()
	End Method
	
End Type


Rem
bbdoc: Retrieves internal information from a curl session, via the #getInfo method.
about: This object will remain valid as long as the curl handle exists. You've been warned ;-)
End Rem
Type TCurlInfo

	Field easyHandlePtr:Byte Ptr
	
	Field error:Int
	
	Function _create:TCurlInfo(easyHandlePtr:Byte Ptr)
		Local this:TCurlInfo = New TCurlInfo
		
		this.easyHandlePtr = easyHandlePtr
	
		Return this
	End Function
	
	Rem
	bbdoc: The last used effective URL.
	End Rem
	Method effectiveURL:String()
		Local s:Byte Ptr
		
		error = bmx_curl_easy_getinfo_string(easyHandlePtr, CURLINFO_EFFECTIVE_URL, Varptr s)
		
		If Not error Then
			Return String.fromCString(s)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: The last received HTTP or FTP code.
	about: This will be zero if no server response code has been received. Note that a proxy's CONNECT response
	should be read with #httpConnectCode and not this.
	End Rem
	Method responseCode:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_RESPONSE_CODE, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The last received proxy response code to a CONNECT request.
	End Rem
	Method httpConnectCode:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_FILETIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The remote time of the retrieved document (in number of seconds since 1 jan 1970 in the GMT/UTC time zone).
	about: If you get -1, it can be because of many reasons (unknown, the server hides it or the server doesn't
	support the command that tells document time etc) and the time of the document is unknown. Note that you must
	tell the server to collect this information before the transfer is made, by using the #CURLOPT_FILETIME option
	to #setOptInt or you will unconditionally get a -1 back.
	End Rem
	Method FileTime:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_HTTPAUTH_AVAIL, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The total time in seconds for the previous transfer, including name resolving, TCP connect etc.
	End Rem
	Method totalTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_TOTAL_TIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The time, in seconds, it took from the start until the name resolving was completed.
	End Rem
	Method namelookupTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_NAMELOOKUP_TIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The time, in seconds, it took from the start until the connect to the remote host (or proxy) was completed.
	End Rem
	Method connectTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_CONNECT_TIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The time, in seconds, it took from the start until the file transfer is just about to begin.
	about: This includes all pre-transfer commands and negotiations that are specific to the particular
	protocol(s) involved.
	End Rem
	Method preTransferTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_PRETRANSFER_TIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The time, in seconds, it took from the start until the first byte is just about to be transferred.
	about: This includes #preTransferTime and also the time the server needs to calculate the result.
	End Rem
	Method startTransferTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_STARTTRANSFER_TIME, Varptr value)
		
		Return value
	End Method

	Rem
	bbdoc: The total time, in seconds, it took for all redirection steps include name lookup, connect, pretransfer and transfer before final transaction was started.
	about: Contains the complete execution time for multiple redirections.
	End Rem
	Method redirectTime:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_REDIRECT_TIME, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The total number of redirections that were actually followed.
	End Rem
	Method redirectCount:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_REDIRECT_COUNT, Varptr value)
		
		Return value
	End Method

	Rem
	bbdoc: The total amount of bytes that were uploaded.
	End Rem
	Method sizeUpload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_SIZE_UPLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The total amount of bytes that were downloaded.
	about: The amount is only for the latest transfer and will be reset again for each new transfer.
	End Rem
	Method sizeDownload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_SIZE_DOWNLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The average download speed that curl measured for the complete download.
	about: Measured in bytes/second.
	End Rem
	Method speedDownload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_SPEED_DOWNLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The average upload speed that curl measured for the complete upload.
	about: Measured in bytes/second.
	End Rem
	Method speedUpload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_SPEED_UPLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The total size of all the headers received.
	about: Measured in number of bytes.
	End Rem
	Method headerSize:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_HEADER_SIZE, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The total size of the issued requests.
	about: This is so far only for HTTP requests. Note that this may be more than one request if
	FOLLOWLOCATION is true.
	End Rem
	Method requestSize:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_REQUEST_SIZE, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The result of the certification verification that was requested (using the #CURLOPT_SSL_VERIFYPEER option).
	End Rem
	Method sslVerifyResult:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_SSL_VERIFYRESULT, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: List of OpenSSL crypto-engines supported.
	about: Note that engines are normally implemented in separate dynamic libraries. Hence not all the returned
	engines may be available at run-time.
	End Rem
	Method sslEngines:String[]()
		
		Return curlProcessSlist(bmx_curl_easy_getinfo_slist(easyHandlePtr, CURLINFO_SSL_ENGINES, Varptr error))
		
	End Method
	
	Rem
	bbdoc: The content-length of the download.
	about: This is the value read from the Content-Length: field.
	End Rem
	Method contentLengthDownload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_CONTENT_LENGTH_DOWNLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The specified size of the upload.
	End Rem
	Method contentLengthUpload:Double()
		Local value:Double
		
		error = bmx_curl_easy_getinfo_double(easyHandlePtr, CURLINFO_CONTENT_LENGTH_UPLOAD, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: The content-type of the downloaded object.
	about: This is the value read from the Content-Type: field. If you get Null, it means that the server didn't
	send a valid Content-Type header or that the protocol used doesn't support this.
	End Rem
	Method contentType:String()
		Local s:Byte Ptr
		
		error = bmx_curl_easy_getinfo_string(easyHandlePtr, CURLINFO_CONTENT_TYPE, Varptr s)
		
		If Not error Then
			If s Then
				Return String.fromCString(s)
			End If
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: The private data associated with the curl handle (set with the #setPrivate ).
	End Rem
	Method privateData:Object()
		Return bmx_curl_easy_getinfo_obj(easyHandlePtr, CURLINFO_PRIVATE, Varptr error)
	End Method
	
	Rem
	bbdoc: A bitmask indicating the authentication method(s) available.
	about: The meaning of the bits is explained in the #CURLOPT_HTTPAUTH option
	End Rem
	Method httpAuthAvail:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_HTTPAUTH_AVAIL, Varptr value)
		
		Return value
	End Method

	Rem
	bbdoc: A bitmask indicating the authentication method(s) available for your proxy authentication.
	about: The meaning of the bits is explained in the #CURLOPT_HTTPAUTH option
	End Rem
	Method proxyAuthAvail:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_PROXYAUTH_AVAIL, Varptr value)
		
		Return value
	End Method
		
	Rem
	bbdoc: The errno variable from a connect failure. 
	End Rem
	Method osErrno:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_OS_ERRNO, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: How many new connections libcurl had to create to achieve the previous transfer (only the successful connects are counted).
	about: Combined with #redirectCount you are able to know how many times libcurl successfully reused existing
	connection(s) or not.
	End Rem
	Method numConnects:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_NUM_CONNECTS, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: A list of all cookies curl knows (expired ones, too).
	about: If there are no cookies (cookies for the handle have not been enabled or simply none have been received),
	the method returns Null.
	End Rem
	Method cookieList:String[]()

		Return curlProcessSlist(bmx_curl_easy_getinfo_slist(easyHandlePtr, CURLINFO_COOKIELIST, Varptr error))

	End Method
	
	Rem
	bbdoc: The last socket used by this curl session.
	about: If the socket is no longer valid, -1 is returned. When you finish working with the socket,
	you must call #cleanup as usual and let libcurl close the socket and cleanup other resources associated
	with the handle. This is typically used in combination with #CURLOPT_CONNECT_ONLY.
	End Rem
	Method lastSocket:Int()
		Local value:Int
		
		error = bmx_curl_easy_getinfo_long(easyHandlePtr, CURLINFO_LASTSOCKET, Varptr value)
		
		Return value
	End Method
	
	Rem
	bbdoc: A string holding the path of the entry path.
	about: That is the initial path libcurl ended up in when logging on to the remote FTP server.
	This will return Null if something is wrong.
	End Rem
	Method ftpEntryPath:String()
		Local s:Byte Ptr
		
		error = bmx_curl_easy_getinfo_string(easyHandlePtr, CURLINFO_FTP_ENTRY_PATH, Varptr s)
		
		If Not error Then
			If s Then
				Return String.fromCString(s)
			End If
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: If the operation was successful, CURLE_OK is returned, otherwise an appropriate error code will be returned.
	End Rem
	Method errorCode:Int()
		Return error
	End Method

End Type

Rem
bbdoc: The libcurl multi interface.
about: The multi interface offers several abilities that the easy interface doesn't. They are mainly:
<ul>
<li>Enable a "pull" interface. The application that uses libcurl decides where and when to ask libcurl
to get/send data.</li>
<li>Enable multiple simultaneous transfers in the same thread without making it complicated for the
application.</li>
<li>Enable the application to wait for action on its own file descriptors and curl's file descriptors
simultaneous easily.</li>
</ul>
<p>
Each single transfer is built up with a #TCurlEasy easy handle. You must create them, and setup the appropriate
options for each easy handle, using the usual "setOptXXX" methods.
</p>
<p>
When the easy handle is setup for a transfer, then instead of using #perform (as when using
the easy interface for transfers), you should instead add the easy handle to the multi handle
using #add. The multi handle is sometimes referred to as a 'multi stack' because of
the fact that it may hold a large amount of easy handles.
</p>
<p>
Should you change your mind, the easy handle is again removed from the multi stack using
#multiRemove. Once removed from the multi handle, you can again use other easy interface functions
like #perform on the handle or whatever you think is necessary.
</p>
<p>
Adding the easy handle to the multi handle does not start the transfer. Remember that one of the main
ideas with this interface is to let your application drive. You drive the transfers by invoking
#multiPerform. libcurl will then transfer data if there is anything available to transfer. It'll use
the callbacks and everything else you have setup in the individual easy handles. It'll transfer data
on all current transfers in the multi stack that are ready to transfer anything. It may be all, it may
be none. 
</p>
<p>
Your application can acquire knowledge from libcurl when it would like to get invoked to transfer data,
so that you don't have to busy-loop and call that #multiPerform like crazy.
#multiSelect offers an interface which allows libcurl to know when the transfers in the multi stack might
need attention, without requiring your application to wait.
</p>
<p>
A little note here about the return codes from the multi functions, and especially the #multiPerform :
if you receive #CURLM_CALL_MULTI_PERFORM, this basically means that you should call #multiPerform again,
before you #multiSelect on more actions. You don't have to do it immediately, but the return code means
that libcurl may have more data available to return or that there may be more data to send off before
it is "satisfied".
</p>
<p>
#multiPerform stores the number of still running transfers in one of its input arguments,
and by reading that you can figure out when all the transfers in the multi handles are done.
'done' does not mean successful. One or more of the transfers may have failed. Tracking when this number
changes, you know when one or more transfers are done.
</p>
<p>
To get information about completed transfers, to figure out success or not and similar,
#multiInfoRead should be called. It can return a message about a current or previous transfer.
Repeated invokes of the function get more messages until the message queue is empty. The information
you receive there includes an easy handle reference which you may use to identify which easy handle
the information regards.
</p>
<p>
When a single transfer is completed, the easy handle is still left added to the multi stack. You need to
first remove the easy handle with #multiRemove and then close it with #cleanup, or possibly set new options
to it and add it again with #multiAdd to start another transfer.
</p>
<p>
When all transfers in the multi stack are done, cleanup the multi handle with #multiCleanup. Be careful
and please note that you MUST invoke separate #cleanup calls on every single easy handle to clean
them up properly.
</p>
<p>
If you want to re-use an easy handle that was added to the multi handle for transfer, you must first
remove it from the multi stack and then re-add it again (possibly after having altered some options
at your own choice).
</p>
End Rem
Type TCurlMulti

	Field multiHandlePtr:Byte Ptr
	
	Field easyHandles:TList = New TList
	
	Rem
	bbdoc: Creates a new #TCurlMulti object.
	End Rem
	Function Create:TCurlMulti()
		Local this:TCurlMulti = New TCurlMulti
		
		this.multiHandlePtr = curl_multi_init()
		
		If Not this.multiHandlePtr Then
			Return Null
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new #TCurlEasy, automatically adding it to the multi stack.
	returns: The new #TCurlEasy object.
	End Rem
	Method newEasy:TCurlEasy()
		Local easy:TCurlEasy = TCurlEasy.Create()
		multiAdd(easy)
		Return easy
	End Method
	
	Rem
	bbdoc: Adds a standard easy object to the multi stack.
	about: This method call will make this multi handle control the specified easy handle.
	<p>
	When an easy object has been added to a multi stack, you can not and you must not use
	#perform on that handle! 
	</p>
	End Rem
	Method multiAdd:Int(easy:TCurlEasy)
		If multiHandlePtr And easy And easy.easyHandlePtr Then
			easyHandles.addLast(easy)
			Return curl_multi_add_handle(multiHandlePtr, easy.easyHandlePtr)
		End If
	End Method
	
	Rem
	bbdoc: Removes a given #TCurlEasy object from the multi handle.
	about: This will make the specified easy handle be removed from this multi handle's control.
	<p>
	When the easy object has been removed from a multi stack, it is again perfectly legal to
	invoke #perform on it.
	</p>
	<p>
	Removing a handle while being used, will effectively halt all transfers in progress.
	</p>
	End Rem
	Method multiRemove:Int(easy:TCurlEasy)
		If multiHandlePtr And easy And easy.easyHandlePtr Then
			' remove from the list
			easyHandles.remove(easy)
		
			Return curl_multi_remove_handle(multiHandlePtr, easy.easyHandlePtr)
		End If
	End Method

	Rem
	bbdoc: Reads/writes available data from each easy handle.
	about: When the app thinks there's data available for the multi handle, it should call this function
	to read/write whatever there is to read or write right now. #multiPerform returns as soon as the
	reads/writes are done. This function does not require that there actually is any data available
	for reading or that data can be written, it can be called just in case. It will write the
	number of handles that still transfer data in into @runningHandles.
	<p>
	When you call #multiPerform and the amount of @runningHandles is changed from the previous call
	(or is less than the amount of easy handles you've added to the multi handle), you know that there
	is one or more transfers less "running". You can then call #multiInfoRead to get information about
	each individual completed transfer, and that returned info includes CURLcode and more.
	</p>
	<p>
	If you receive #CURLM_CALL_MULTI_PERFORM, this basically means that you should call
	#multiPerform again, before you #multiSelect on more actions. You don't have to do it immediately,
	but the return code means that libcurl may have more data available to return or that there may be
	more data to send off before it is "satisfied". Do note that #multiPerform will return
	#CURLM_CALL_MULTI_PERFORM only when it wants to be called again immediately.
	When things are fine and there are nothing immediate it wants done, it'll return #CURLM_OK and you
	need to wait for "action" and then call this function again.
	</p>
	<p>
	NOTE that this only returns errors etc regarding the whole multi stack. There might still have
	occurred problems on individual transfers even when this function returns #CURLM_OK.
	</p>
	End Rem
	Method multiPerform:Int(runningHandles:Int Var)
		If multiHandlePtr Then
		
			Return curl_multi_perform(multiHandlePtr, Varptr runningHandles)
		
		End If
	End Method
	
	Rem
	bbdoc: Waits for network connections to become ready, applying a @timeout seconds delay if required.
	End Rem
	Method multiSelect:Int(timeout:Double = 1.0)
		If multiHandlePtr Then
		
			Return bmx_curl_multiselect(multiHandlePtr, timeout)
		
		End If
	End Method

	Rem
	bbdoc: Close down a multi session.
	about: Cleans up and removes a whole multi stack. It does not free or touch any individual
	easy handles in any way - they still need to be closed individually, using the usual #cleanup way.
	The order of cleaning up should be:
	<ol>
	<li>#multiRemove before any easy handles are cleaned up</li>
	<li>#cleanup can now be called independently since the easy handle is no longer connected to the
	multi handle</li>
	<li>#multiCleanup should be called when all easy handles are removed</li>
	</ol>
	End Rem
	Method multiCleanup()
	
		If multiHandlePtr Then
			If easyHandles Then
				For Local easy:TCurlEasy = EachIn easyHandles
				
					If easy.easyHandlePtr Then
						curl_multi_remove_handle(multiHandlePtr, easy.easyHandlePtr)
					End If
					
				Next
			End If
			
			easyHandles.clear()
	
			curl_multi_cleanup(multiHandlePtr)
			multiHandlePtr = Null
		End If
		
	End Method
	
	Rem
	bbdoc: Ask the multi handle if there are any messages/informationals from the individual transfers.
	about: Messages may include informationals such as an error code from the transfer or just the fact that a
	transfer is completed. More details on these should be written down as well.
	<p>
	Repeated calls to this function will return a new #TCurlMultiMsg each time, until a Null is returned as a signal
	that there is no more to get at this point. The integer pointed to messagesInQueue will contain the number of
	remaining messages after this method was called.
	</p>
	<p>
	When you fetch a message using this method, it is removed from the internal queue so calling this method again
	will not return the same message again. It will instead return new messages at each new invoke until
	the queue is emptied. 
	</p>
	End Rem
	Method multiInfoRead:TCurlMultiMsg(messagesInQueue:Int Var)
		If multiHandlePtr Then
			Local infoPtr:Byte Ptr = curl_multi_info_read(multiHandlePtr, Varptr messagesInQueue)
			
			If infoPtr Then
			
				Local info:TCurlMultiMsg = New TCurlMultiMsg
			
				info.message = bmx_curl_CURLMsg_msg(infoPtr)
				
				If info.message = CURLMSG_DONE Then
					info.result = bmx_curl_CURLMsg_result(infoPtr)
				End If
				
				Local ePtr:Byte Ptr = bmx_curl_CURLMsg_easy_handle(infoPtr)
				
				For Local e:TCurlEasy = EachIn easyHandles
					If ePtr = e.easyHandlePtr Then
						info.easy = e
						Exit
					End If
				Next
				
				Return info
				
			End If
			
		End If
		
		Return Null
	
	End Method
	
	Rem
	bbdoc: Used to tell a libcurl multi handle how to behave.
	End Rem
	Method multiSetOptInt(option:Int, parameter:Int)
		If multiHandlePtr Then
			bmx_curl_multi_setopt_long(multiHandlePtr, option, parameter)
		End If
	End Method
	
	Method Delete()
		multiCleanup()
	End Method

End Type

Rem
bbdoc: A message type as retrieved from #multiInfoRead.
about: It provides specific information for finished transfers.
End Rem
Type TCurlMultiMsg

	Rem
	bbdoc: What this message means
	End Rem
	Field message:Int
	Rem
	bbdoc: The easy handle it concerns
	End Rem
	Field easy:TCurlEasy
	
	Rem
	bbdoc: Return code for transfer
	End Rem
	Field result:Int

End Type
