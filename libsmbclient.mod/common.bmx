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
Import BRL.Map
Import BRL.LinkedList
Import BRL.Stream

Import "-lsmbclient"


Import "glue.cpp"


Extern
	Function smbc_free_context:Int(handle:Byte Ptr, shutdown:Int)

	Function bmx_smbc_new_context:Byte Ptr(handle:Object, smbDebug:Int)
	Function bmx_smbc_free_context(handle:Byte Ptr)
	
	Function bmx_smbc_opendir:Object(handle:Byte Ptr, path:String)
	Function bmx_smbc_readdir:Object(handle:Byte Ptr, dir:Byte Ptr)
	Function bmx_smbc_closedir:Object(handle:Byte Ptr, dir:Byte Ptr)

	Function bmx_smbc_filetype:Int(handle:Byte Ptr, path:String)
	
	Function bmx_smbc_purgecachedservers(handle:Byte Ptr)
	Function bmx_smbc_getoptioncasesensitive:Int(handle:Byte Ptr)
	Function bmx_smbc_setoptioncasesensitive(handle:Byte Ptr, value:Int)

	Function bmx_smbc_open:Byte Ptr(context:Byte Ptr, path:String, _mode:Int, handle:Object)
	Function bmx_smbc_seek:Int(context:Byte Ptr, file:Byte Ptr, pos:Int)
	Function bmx_smbc_read:Int(context:Byte Ptr, file:Byte Ptr, buf:Byte Ptr, count:Int)
	Function bmx_smbc_write:Int(context:Byte Ptr, file:Byte Ptr, buf:Byte Ptr, count:Int)
	Function bmx_smbc_close(context:Byte Ptr, file:Byte Ptr)
	
End Extern

Const SMBC_WORKGROUP:Int = 1
Const SMBC_SERVER:Int = 2
Const SMBC_FILE_SHARE:Int = 3
Const SMBC_PRINTER_SHARE:Int = 4
Const SMBC_COMMS_SHARE:Int = 5
Const SMBC_IPC_SHARE:Int = 6
Const SMBC_DIR:Int = 7
Const SMBC_FILE:Int = 8
Const SMBC_LINK:Int = 9
