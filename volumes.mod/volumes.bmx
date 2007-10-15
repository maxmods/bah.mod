SuperStrict

Rem
bbdoc: Volumes
End Rem
Module BaH.Volumes

ModuleInfo "Version: 1.04"
ModuleInfo "License: MIT"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "Copyright: (c) 2006, 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

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