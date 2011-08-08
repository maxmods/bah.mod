' Copyright (c) 2006-2011 Bruce A Henderson
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
bbdoc: Volumes
End Rem
Module BaH.Volumes

ModuleInfo "Version: 1.08"
ModuleInfo "License: MIT"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "Copyright: (c) 2006-2011 Bruce A Henderson"

ModuleInfo "History: 1.08"
ModuleInfo "History: Added support for FOLDERID paths for Win32. Automatically maps CSIDL to FOLDERID."
ModuleInfo "History: 1.07"
ModuleInfo "History: Added support for all native CSIDL paths on Win32 using GetCustomDir."
ModuleInfo "History: Added support for 'k' paths on Mac using GetCustomDir."
ModuleInfo "History: Updated documentation."
ModuleInfo "History: 1.06"
ModuleInfo "History: Improved unicode support."
ModuleInfo "History: Added custom dir access for Pictures, Movies and Music."
ModuleInfo "History: Added Win32 volume search code. (Artemis)"
ModuleInfo "History: Fixed Linux folder localisation (xdg-user-dirs)."
ModuleInfo "History: 1.05"
ModuleInfo "History: Added GetCustomDir function."
ModuleInfo "History: 1.04"
ModuleInfo "History: Updated Win32 to use SHGetFolderPathW instead of SHGetFolderPathA"
ModuleInfo "History: 1.03"
ModuleInfo "History: Added new functions : GetUserHomeDir, GetUserDesktopDir, GetUserAppDir, GetUserDocumentsDir."
ModuleInfo "History: 1.02"
ModuleInfo "History: Added Mac support."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed win32 type returning the wrong string."
ModuleInfo "History: 1.00 Initial Release"

?linux
Import "vol_linux.bmx"
?win32
Import "vol_win.bmx"
?macos
Import "vol_mac.bmx"
?