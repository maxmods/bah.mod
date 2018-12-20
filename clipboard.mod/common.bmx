' Copyright (c) 2018 Bruce A Henderson
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

Import "source.bmx"

Extern

	Function bmx_clipboard_new_win32:Byte Ptr(maxRetries:Int, retryDelay:Int)
	Function bmx_clipboard_new_x11:Byte Ptr(actionTimeout:Int, transferSize:Int, displayName:String)
	Function bmx_clipboard_new:Byte Ptr()
	Function bmx_clipboard_free(handle:Byte Ptr)
	Function bmx_clipboard_clear(handle:Byte Ptr, clipboardMode:Int)
	Function bmx_clipboard_has_ownership:Int(handle:Byte Ptr, clipboardMode:Int)
	Function bmx_clipboard_text:String(handle:Byte Ptr)
	Function bmx_clipboard_text_ex:String(handle:Byte Ptr, length:Int Ptr, clipboardMode:Int)
	Function bmx_clipboard_set_text:Int(handle:Byte Ptr, src:String)
	Function bmx_clipboard_set_text_ex:Int(handle:Byte Ptr, src:String, clipboardMode:Int)
	
End Extern

Rem
bbdoc: The primary (global) clipboard.
End Rem
Const LCB_CLIPBOARD:Int = 0

Rem
bbdoc: The (global) mouse selection clipboard.
End Rem
Const LCB_SELECTION:Int = 1