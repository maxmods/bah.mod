' Copyright (c) 2007,2008 Bruce A Henderson
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

Rem
bbdoc: ClearSilver Template Engine
End Rem
Module BaH.ClearSilver

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: ClearSilver - 2001-2008 Brandon Long"
ModuleInfo "Copyright: Wrapper - 2007, 2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (ClearSilver 0.10.4)"

Import BRL.LinkedList

Import "common.bmx"

Rem
bbdoc: A ClearSilver HDF (Hierarchical Data Format) dataset.
End Rem
Type THDF

	Field hdfPtr:Byte Ptr
	Field root:THDF

	Rem
	bbdoc: Constructs an empty HDF dataset.
	End Rem
	Function Create:THDF()
	
		Local this:THDF = New THDF
		
		this.hdfPtr = bmx_hdf_new()
		
		If Not this.hdfPtr Then
			Return Null
		End If
		
		Return this
	
	End Function
	
	Function _create:THDF(hdfPtr:Byte Ptr, parent:THDF)
		Local this:THDF = New THDF
		
		this.hdfPtr = hdfPtr
		
		If parent And parent.root Then
			this.root = parent.root
		Else
			this.root = parent
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Loads the contents of the specified HDF file from disk into the current #THDF object.
	about: The loaded contents are merged with the existing contents.
	End Rem
	Method ReadFile(filename:String)
	
		If hdfPtr Then
		
			hdf_read_file(hdfPtr, filename)
		
		End If
	
	End Method
	
	Rem
	bbdoc: Parses/loads the contents of the given string as HDF into the current #THDF object.
	about: The loaded contents are merged with the existing contents.
	End Rem
	Method ReadString(data:String)
		hdf_read_string(hdfPtr, data)
	End Method
	
	Rem
	bbdoc: Serializes HDF contents to a file (readable by readFile).
	End Rem
	Method WriteFile(filename:String)
		hdf_write_file(hdfPtr, filename)
	End Method
	
	Rem
	bbdoc: Serializes HDF contents to a string (readable by readString).
	End Rem
	Method WriteString:String()
		Local ret:String
		Local s:Byte Ptr
		
		hdf_write_string(hdfPtr, Varptr s)
		
		If s Then
			ret = String.fromCString(s)
			bmx_free(s)
		End If
		
		Return ret
	End Method
	
	Rem
	bbdoc: Set the value of a named node.
	about: All of the intermediate nodes which don't exist will be created with a value of Null.
	Existing nodes are not modified. New nodes are created at the end of the list.
	A copy of the value will be made which the dataset will own.
	End Rem
	Method setValue(name:String, value:String)
		hdf_set_value(hdfPtr, name, value)
	End Method
	
	Rem
	bbdoc: Retrieves the value at the specified path in this HDF node's subtree.
	End Rem
	Method getValue:String(name:String, defaultValue:String = "")
		Return String.fromCString(hdf_get_value(hdfPtr, name, defaultValue))
	End Method
	
	Rem
	bbdoc: Set the integer value of a named node.
	about: All of the intermediate nodes which don't exist will be created with a value of Null.
	Existing nodes are not modified. New nodes are created at the end of the list.
	A copy of the value will be made which the dataset will own.
	End Rem
	Method setIntValue(name:String, value:Int)
		hdf_set_int_value(hdfPtr, name, value)
	End Method
	
	Rem
	bbdoc: Retrieves the integer value at the specified path in this HDF node's subtree.
	about: If the value does not exist, or cannot be converted to an integer, @defaultValue will be returned.
	End Rem
	Method getIntValue:Int(name:String, defaultValue:Int = 0)
		Return hdf_get_int_value(hdfPtr, name, defaultValue)
	End Method
	
	Rem
	bbdoc: Links the src hdf name to the dest.
	End Rem
	Method setSymLink(src:String, dest:String)
		hdf_set_symlink(hdfPtr, src, dest)
	End Method
	
	Rem
	bbdoc: Removes the specified subtree.
	End Rem
	Method removeTree(hdfName:String)
		hdf_remove_tree(hdfPtr, hdfName)
	End Method
	
	Rem
	bbdoc: Dumps the dataset to a String.
	End Rem
	Method dump:String()
		Local ret:String

		Local s:Byte Ptr = bmx_hdf_dump_str(hdfPtr)
		If s Then
			ret = String.fromBytes(bmx_string_buf(s), bmx_string_len(s))
			bmx_string_clear(s)
		End If
		
		Return ret
	End Method
	
	Rem
	bbdoc: A deep copy of an HDF tree pointed to by @src to the named node @path.
	End Rem
	Method copy(path:String, src:THDF)
		hdf_copy(hdfPtr, path, src.hdfPtr)
	End Method
	
	Rem
	bbdoc: Return the root of the tree where the current node lies.
	about: If the current node is the root, returns this object.
	End Rem
	Method getRoot:THDF()
		If root Then
			Return root
		End If
		
		Return Self
	End Method
	
	Rem
	bbdoc: Retrieves the #THDF object that is the root of the subtree at @hdfpath, or Null if no object exists at that path.
	End Rem
	Method getObj:THDF(hdfPath:String)
		Local hdf:Byte Ptr = hdf_get_obj(hdfPtr, hdfPath)

		If hdf Then
			Return THDF._create(hdf, Self)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Retrieves the #THDF for the first child of the root of the subtree at @hdfpath, or Null if no child exists of that path or if the path doesn't exist.
	End Rem
	Method GetChild:THDF(hdfPath:String)
		Local hdf:Byte Ptr = hdf_get_child(hdfPtr, hdfPath)

		If hdf Then
			Return THDF._create(hdf, Self)
		End If
		
		Return Null
	End Method

	Rem
	bbdoc: Retrieves the #THDF object that is the root of the subtree at @hdfpath, create the subtree if it doesn't exist
	End Rem	
	Method getOrCreateObj:THDF(hdfPath:String)
		Local hdf:Byte Ptr = hdf_get_obj(hdfPtr, hdfPath)

		If Not hdf Then
		
			hdf_set_value(hdfPtr, hdfPath, "")
			hdf = hdf_get_obj(hdfPtr, hdfPath)
			
			If hdf Then
				Return THDF._create(hdf, Self)
			End If
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Returns the name of this HDF node.
	about: The root node has no name, so calling this on the root node will return Null.
	End Rem
	Method name:String()
		Return String.fromCString(hdf_obj_name(hdfPtr))
	End Method
	
	Rem
	bbdoc: Returns the value of this HDF node, or Null if this node has no value.
	about: Every node in the tree can have a value, a child, and a next peer.
	End Rem
	Method value:String()
		Return String.fromCString(hdf_obj_value(hdfPtr))
	End Method
	
	Rem
	bbdoc: Returns the child of this HDF node, or Null if there is no child.
	about: Use this in conjunction with #objNext to walk the HDF tree.  Every node in the tree can
	have a value, a child, and a next peer.
	End Rem
	Method objChild:THDF()
		Local child:Byte Ptr = hdf_obj_child(hdfPtr)
		
		If child Then
			Return THDF._create(child, Self)
		End If
		
		Return Null
	End Method
	
	Rem
	bbdoc: Returns the next sibling of this HDF node, or Null if there is no next sibling.
	about: Use this in conjunction with #objChild to walk the HDF tree. Every node in the tree can
	have a value, a child, and a next peer.
	End Rem
	Method objNext:THDF()
		Local child:Byte Ptr = hdf_obj_next(hdfPtr)
		
		If child Then
			Return THDF._create(child, Self)
		End If
		
		Return Null
	End Method

	Method free()
	
		hdf_destroy(hdfPtr)
		
	End Method

	Method Delete()
		
		If Not root Then
			free()
		End If
		
	End Method

End Type

Rem
bbdoc: A ClearSilver template rendering context.
End Rem
Type TCSParse

	Field csPtr:Byte Ptr
	
	Field globalHDF:THDF
	
	Rem
	bbdoc: Creates a new ClearSilver template rendering context.
	about: You should have already created and populated a #THDF dataset which you provide as the
	only argument to the constructor.
	End Rem
	Function Create:TCSParse(hdf:THDF)
	
		Local this:TCSParse = New TCSParse
		
		this.csPtr = bmx_cs_new(hdf.hdfPtr)
		
		If Not this.csPtr Then
			Return Null
		End If
		
		Return this
	
	End Function
	
	Rem
	bbdoc: Parses the provided string template fragment into the parse tree.
	End Rem
	Method parseString(content:String)
		bmx_cs_parse_string(csPtr, content, content.length)
	End Method

	Rem
	bbdoc: Evaluates the parse tree and produces the rendered output as a string.
	End Rem
	Method render:String()
		Local ret:String

		Local s:Byte Ptr = bmx_cs_render(csPtr)
		If s Then
			ret = String.fromBytes(bmx_string_buf(s), bmx_string_len(s))
			bmx_string_clear(s)
		End If
		
		Return ret
	End Method
	
	Rem
	bbdoc: Parses the provided template file into the parse tree.
	End Rem
	Method parseFile(filename:String)
		cs_parse_file(csPtr, filename)
	End Method
	
	Method Delete()
		If csPtr Then
			cs_destroy(csPtr)
			csPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A ClearSilver CGI handler.
End Rem
Type TCGI
	
	Field cgiPtr:Byte Ptr
	Field hdf:THDF
	
	Field cbUserData:Object
	Field callback:Byte Ptr(cgi:TCGI, httpMethod:String, contentType:String, userData:Object)
	
	Global cbHandles:TList = New TList
	
	Global uploadCallback:Int(cgi:TCGI, nread:Int, expected:Int)
	
	Rem
	bbdoc: Creates a new #TCGI instance.
	End Rem
	Function Create:TCGI()
	
		Local this:TCGI = New TCGI
		
		this.cgiPtr = bmx_cgi_new()
		
		If Not this.cgiPtr Then
			Return Null
		End If
		
		this.init()
				
		Return this
	
	End Function
	
	' internal
	Function _create:TCGI(cgiPtr:Byte Ptr)
		Local this:TCGI = New TCGI
		
		this.cgiPtr = cgiPtr
		
		this.init()
				
		Return this
	End Function
	
	Method Init()
		hdf = THDF._create(bmx_cgi_hdf(cgiPtr), Null)
	End Method
	
	Rem
	bbdoc: Render and display the CGI output to the user.
	about: Renders the CS template pointed to by @file using the CGI's HDF data set, and sends the
	output to the user. Note that the output is actually rendered into memory first.
	<p>Parameters :
	<ul>
	<li><b>file</b> : a ClearSilver template file</li>
	</ul>
	</p>
	End Rem
	Method display(file:String)
		cgi_display(cgiPtr, file)
	End Method
	
	Rem
	bbdoc: cookieClead will send back a Set-Cookie string that will attempt to stop a browser from continuing to send back a cookie.
	about: Note that the cookie has to match in name, domain, and path, and the luck of the Irish has to
	be with you for this work all the time, but at the least it will make the browser send back a
	cookie with no value, which the ClearSilver cookie parsing code will ignore.
	<p>Parameters :
	<ul>
	<li><b>name</b> : the cookie name to clear</li>
	<li><b>domain</b> : the domain to clear, Null for none</li>
	<li><b>path</b> : the cookie's path</li>
	</ul>
	</p>
	End Rem
	Method cookieClear(name:String, domain:String, path:String)
		If domain Then
			cgi_cookie_clear(cgiPtr, name, domain, path)
		Else
			cgi_cookie_clear(cgiPtr, name, Null, path)
		End If
	End Method
	
	Rem
	bbdoc: Set a browser Cookie.
	about: Will issue a Set-Cookie header that should cause a browser to return a cookie when required.
	Note this function does no escaping of anything, you have to take care of that first.<br>
	<p>Parameters :
	<ul>
	<li><b>name</b> : the name of the cookie</li>
	<li><b>value</b> : the value to set the cookie to</li>
	<li><b>path</b> : optional path for which the cookie is valid. Default is /</li>
	<li><b>domain</b> : optional domain for which the cookie is valid.  You can use #cookieAuthority
	to determine this domain.  Default is none, which is interpreted by the browser as the sending
	domain only</li>
	<li><b>timeStr</b> : expiration time string in the following format Wdy, DD-Mon-YYYY HH:MM:SS GMT.
	Only used if @persistent.  Default is one year from time of call.</li>
	<li><b>persistent</b> : cookie will be stored by the browser between sessions</li>
	<li><b>secure</b> : cookie will only be sent over secure connections</li>
	</ul>
	</p>
	End Rem
	Method cookieSet(name:String, value:String, path:String, domain:String, timeStr:String, persistent:Int, secure:Int)
	
		If path Then
			If domain Then
				If timeStr Then
					cgi_cookie_set(cgiPtr, name, value, path, domain, timeStr, persistent, secure)
				Else
					cgi_cookie_set(cgiPtr, name, value, path, domain, Null, persistent, secure)				
				End If
			Else
				If timeStr Then
					cgi_cookie_set(cgiPtr, name, value, path, Null, timeStr, persistent, secure)
				Else
					cgi_cookie_set(cgiPtr, name, value, path, Null, Null, persistent, secure)
				End If
			End If
		Else
			If domain Then
				If timeStr Then
					cgi_cookie_set(cgiPtr, name, value, Null, domain, timeStr, persistent, secure)
				Else
					cgi_cookie_set(cgiPtr, name, value, Null, domain, Null, persistent, secure)
				End If
			Else
				If timeStr Then
					cgi_cookie_set(cgiPtr, name, value, Null, Null, timeStr, persistent, secure)
				Else
					cgi_cookie_set(cgiPtr, name, value, Null, Null, Null, persistent, secure)
				End If
			End If
		End If
		
	End Method
	
	Rem
	bbdoc: Determine the cookie authority for a domain.
	returns: The authority domain, or Null if none found. 
	about: Will walk the CookieAuthority portion of the CGI HDF data set, and return the matching
	domain if it exists. The purpose of this is so that you set domain specific cookies.
	For instance, you might have<br>
	CookieAuthority.0 = neotonic.com<br>
	In which case, any webserver using a hostname ending in neotonic.com will generate a cookie
	authority of neotonic.com.
	<p>Parameters :
	<ul>
	<li><b>host</b> : optional host to match against.  If Null, the function will use the HTTP.Host
	HDF variable.</li>
	</ul>
	</p>
	End Rem
	Method cookieAuthority:String(host:String)
		If host Then
			Return String.fromCString(cgi_cookie_authority(cgiPtr, host))
		Else
			Return String.fromCString(cgi_cookie_authority(cgiPtr, Null))
		End If
	End Method
	
	Rem
	bbdoc: Redirects the user to another page on your site.
	End Rem
	Method redirect(path:String)
		cgi_redirect(cgiPtr, "%s", path)
	End Method
	
	Rem
	bbdoc: Redirects the user to another web page.
	about: This version takes the full URL, including protocol/domain/port/path.
	End Rem
	Method redirectUri(uri:String)
		cgi_redirect_uri(cgiPtr, "%s", uri)
	End Method
	
	Rem
	bbdoc: Used to read incoming data from the client, usually from a POST or PUT HTTP request.
	about: It wraps the part of fread(stdin). 
	<p>Parameters :
	<ul>
	<li><b>buffer</b> : a pre-allocated buffer to read the data into</li>
	<li><b>length</b> : the size of the pre-allocated buffer</li>
	<li><b>readLength</b> : the number of bytes read into the buffer</li>
	</ul>
	</p>
	End Rem
	Method read(buffer:Byte Ptr, length:Int, readLength:Int Ptr)
		cgiwrap_read(buffer, length, readLength)
	End Method
	
	Rem
	bbdoc: Return a file pointer to an uploaded file.
	about: Returns the stdio FILE pointer associated with a file that was uploaded using
	multipart/form-data.  The FILE pointer is positioned at the start of the file when first available.
	<p>Parameters :
	<ul>
	<li><b>formName</b> : the form name that the file was uploaded as (not the filename)
	(if Null, we're asking for the file handle for the PUT upload)</li>
	</ul>
	</p>
	End Rem
	Method fileHandle:Int(formName:String = Null)
		If formName Then
			Return cgi_filehandle(cgiPtr, formName)
		Else
			Return cgi_filehandle(cgiPtr, Null)
		End If
	End Method
	
	Rem
	bbdoc: Sets the upload callback function.
	End Rem
	Method setUploadCB:Int(callback(cgi:TCGI, nread:Int, expected:Int))
	
		uploadCallback = callback
		
		bmx_cgi_setuploadcb(cgiPtr, _uploadCallback)

	End Method
	
	Function _uploadCallback:Int(cgiPtr:Byte Ptr, nread:Int, expected:Int)
	
		Local cgi:TCGI = TCGI._create(cgiPtr)
		
		Return TCGI.uploadCallback(cgi, nread, expected)
		
	End Function
	
	Rem
	bbdoc: Register a parse callback.
	about: The ClearSilver CGI Kit has built-in functionality to handle the following methods:
	<ul>
	<li>GET -> doesn't have any data except query string, which is processed for all methods</li>
 	<li>POST w/ application/x-www-form-urlencoded</li>
	<li>POST w/ multipart/form-data processed as RFC2388 data into files and HDF (see cgi_filehandle())</li>
	<li>PUT (any type) The entire data chunk is stored as a file, with meta data in HDF
	(similar to single files in RFC2388).  The data is accessible via cgi_filehandle with Null for name.</li>
	</ul>
	<p>To handle other methods/content types, you have to register your own parse function.
	This isn't necessary if you aren't expecting any data, and technically HTTP only allows data
	on PUT/POST requests (and presumably user defined methods).  In particular, if you want to 
	implement XML-RPC or SOAP, you'll have to register a callback here to grab the XML data chunk.
	Usually you'll want to register POST w/ application/xml or POST w/ text/xml (you either need to
	register both or register POST w/ * and check the ctype yourself, remember to
	nerrRaise(CGIParseNotHandled) if you aren't handling the POST). eg.
	<pre>Return nerrRaise(CGIParseNotHandled)</pre>
	</p>
	<p>In general, your callback should:<br>
	Find out how much data is available:<br>
	<pre>length = hdf.getIntValue("CGI.ContentLength")</pre>
	<p>And read/handle all of the data using #read.<br>
	Note that #read is not guaranteed to return all of the data you request (just like fread(3)) since
	it might be reading of a socket.  Sorry.</p>
	<p>
	You should be careful when reading the data to watch for short reads (ie, end of file) and cases
	where the client sends you data ad infinitum.
	</p>
	End Rem
	Method registerParseCallback(callback:Byte Ptr(cgi:TCGI, httpMethod:String, contentType:String, userData:Object), httpMethod:String = "*", contentType:String = "*", userData:Object = Null)
		
		Local cgi:TCGI = _create(cgiPtr)
		cgi.cbUserData = userData
		cgi.callback = callback
		
		cbHandles.addLast(cgi)
		
		cgi_register_parse_cb(cgiPtr, httpMethod, contentType, cgi, parseCallback)
	End Method
	
	Function parseCallback:Byte Ptr(cgiPtr:Byte Ptr, httpMethod:Byte Ptr, ctype:Byte Ptr, rock:Object)
		
		If TCGI(rock) Then
		
			Return TCGI(rock).callback(TCGI(rock), String.fromCString(httpMethod), String.fromCString(ctype), TCGI(rock).cbUserData)
		
		End If
		
		' hmmm... guess we should ignore this then...
		Return nerrRaise(CGIParseNotHandled)
	End Function
	
	Rem
	bbdoc: Parse incoming CGI data.
	about: Responsible for parsing the entity body of the HTTP request.  This payload is typically
	only sent (expected) on POST/PUT requests, but generally this is called on all incoming requests.
	This function walks the list of registered parse callbacks (see #registerParseCallback), and if
	none of those matches or handles the request, it falls back to the builtin handlers:
	<ul>
	<li>POST w/ application/x-www-form-urlencoded</li>
	<li>POST w/ application/form-data</li>
	<li>PUT w/ any content type</li>
	</ul>
	<p>In general, if there is no Content-Length, then #parse ignores the payload and doesn't raise an
	error.</p>
	End Rem
	Method parse()
		cgi_parse(cgiPtr)
	End Method
	
	Method free()
		cbHandles.remove(Self)
		
	End Method
	
End Type

Rem
bbdoc: URLUnescape will do URL unescaping on the passed in string.
about: This function will decode any %XX character, and will decode + as space.
End Rem
Function URLUnescape:String(text:String)
	Return String.fromCString(cgi_url_unescape(text))
End Function

Rem
bbdoc: URLEscape will do URL escaping on the passed in string, and return a new string that is escaped.
about: Characters which are escaped include control characters,
%, ?, +, space, =, &amp;, /, and "
End Rem
Function URLEscape:String(text:String)
	Local ret:String
	Local s:Byte Ptr
	
	cgi_url_escape(text, Varptr s)
	
	If s Then
		ret = String.fromCString(s)
		bmx_free(s)
	End If
	
	Return ret

End Function

Rem
bbdoc: URLEscapeMore will do URL escaping on the passed in String, And Return a New String that is escaped.
about: Characters which are escaped include control characters,
%, ?, +, space, =, &, /, and " and any characters in @other.
End Rem
Function URLEscapeMore:String(text:String, other:String)
	
	Local ret:String
	Local s:Byte Ptr
	
	If other Then
		cgi_url_escape_more(text, Varptr s, other)
	Else
		cgi_url_escape_more(text, Varptr s, Null)
	End If
	
	If s Then
		ret = String.fromCString(s)
		bmx_free(s)
	End If
	
	Return ret
	
End Function

Rem
bbdoc: Use this method to create an error "exception" for return up the call chain.
End Rem
Function nerrRaise:Byte Ptr(errType:Int)
	Return bmx_nerr_raise(errType)
End Function
