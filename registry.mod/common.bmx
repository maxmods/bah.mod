' Copyright (c) 2016-2019 Bruce A Henderson
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

Import "glue.c"

Extern


	Function bmx_reg_RegCreateKey:Int(hKey:Int, lpSubKey:String, lpClass:String, dwOptions:Int, samDesired:Int, lpSecurityAttributes:Int, phkResult:Int Ptr, lpdwDisposition:Int Ptr)
	Function bmx_reg_RegOpenKey:Int(hKey:Int, lpSubKey:String, samDesired:Int, phkResult:Int Ptr)

	Function bmx_reg_GetInt:Int(key:Int, name:String, value:Int Ptr)
	Function bmx_reg_GetLong:Int(key:Int, name:String, value:Long Ptr)
	Function bmx_reg_GetString:String(key:Int, name:String, res:Int Ptr)

	Function bmx_reg_SetInt:Int(key:Int, name:String, value:Int)
	Function bmx_reg_SetLong:Int(key:Int, name:String, value:Long)
	Function bmx_reg_SetString:Int(key:Int, name:String, value:String)
	
	Function bmx_reg_RegCloseKey:Int(hKey:Int)
	Function bmx_reg_RegEnumKey:String(key:Int, index:Int, hasNext:Int Ptr)
	Function bmx_reg_RegDeleteKey:Int(key:Int, subKey:String, samDesired:Int)
	Function bmx_reg_RegDeleteValue:Int(key:Int, valueName:String)

	Function bmx_reg_RegEnumValue:Int(key:Int, index:Int, value:Object)
End Extern


Rem
bbdoc: Registry entries subordinate to this key define types (or classes) of documents and the properties associated with those types.
about: Shell and COM applications use the information stored under this key.
End Rem
Const HKEY_CLASSES_ROOT:Int = $80000000
Rem
bbdoc: Registry entries subordinate to this key define the preferences of the current user.
about: These preferences include the settings of environment variables, data about program groups, colors, printers, network connections,
and application preferences. This key makes it easier to establish the current user's settings; the key maps to the current user's branch
in HKEY_USERS. In HKEY_CURRENT_USER, software vendors store the current user-specific preferences to be used within their applications.
Microsoft, for example, creates the HKEY_CURRENT_USER\Software\Microsoft key for its applications to use, with each application creating
its own subkey under the Microsoft key.
End Rem
Const HKEY_CURRENT_USER:Int = $80000001
Rem
bbdoc: Registry entries subordinate to this key define the physical state of the computer, including data about the bus type, system memory, and installed hardware and software.
about: It contains subkeys that hold current configuration data, including Plug and Play information (the Enum branch, which includes a
complete list of all hardware that has ever been on the system), network logon preferences, network security information,
software-related information (such as server names and the location of the server), and other system information.
End Rem
Const HKEY_LOCAL_MACHINE:Int = $80000002
Rem
bbdoc: Registry entries subordinate to this key define the default user configuration for new users on the local computer and the user configuration for the current user.
End Rem
Const HKEY_USERS:Int = $80000003
Rem
bbdoc: Contains information about the current hardware profile of the local computer system.
about: The information under HKEY_CURRENT_CONFIG describes only the differences between the current hardware configuration and the
standard configuration. Information about the standard hardware configuration is stored under the Software and System keys
of HKEY_LOCAL_MACHINE.
HKEY_CURRENT_CONFIG is an alias for HKEY_LOCAL_MACHINE\System\CurrentControlSet\Hardware Profiles\Current.
End Rem
Const HKEY_CURRENT_CONFIG:Int = $80000005



Rem
bbdoc: Combines the STANDARD_RIGHTS_REQUIRED, KEY_QUERY_VALUE, KEY_SET_VALUE, KEY_CREATE_SUB_KEY, KEY_ENUMERATE_SUB_KEYS, KEY_NOTIFY, and KEY_CREATE_LINK access rights.
End Rem
Const KEY_ALL_ACCESS:Int = $F003F
Rem
bbdoc: Reserved for system use.
End Rem
Const KEY_CREATE_LINK:Int = $0020
Rem
bbdoc: Required to create a subkey of a registry key.
End Rem
Const KEY_CREATE_SUB_KEY:Int = $0004
Rem
bbdoc: Required to enumerate the subkeys of a registry key.
End Rem
Const KEY_ENUMERATE_SUB_KEYS:Int = $0008
Rem
bbdoc: Equivalent to KEY_READ.
End Rem
Const KEY_EXECUTE:Int = $20019
Rem
bbdoc: Required to request change notifications for a registry key or for subkeys of a registry key.
End Rem
Const KEY_NOTIFY:Int = $0010
Rem
bbdoc: Required to query the values of a registry key.
End Rem
Const KEY_QUERY_VALUE:Int = $0001
Rem
bbdoc: Combines the STANDARD_RIGHTS_READ, KEY_QUERY_VALUE, KEY_ENUMERATE_SUB_KEYS, and KEY_NOTIFY values.
End Rem
Const KEY_READ:Int = $20019
Rem
bbdoc: Required to create, delete, or set a registry value.
End Rem
Const KEY_SET_VALUE:Int = $0002
Rem
bbdoc: Indicates that an application on 64-bit Windows should operate on the 32-bit registry view.
about: This flag is ignored by 32-bit Windows. For more information, see Accessing an Alternate Registry View.
This flag must be combined using the OR operator with the other flags in this table that either query or access registry values.
End Rem
Const KEY_WOW64_32KEY:Int = $0200
Rem
bbdoc: Indicates that an application on 64-bit Windows should operate on the 64-bit registry view.
about: This flag is ignored by 32-bit Windows. For more information, see Accessing an Alternate Registry View.
This flag must be combined using the | operator with the other flags in this table that either query or access registry values.
End Rem
Const KEY_WOW64_64KEY:Int = $0100
Rem
bbdoc: Combines the STANDARD_RIGHTS_WRITE, KEY_SET_VALUE, and KEY_CREATE_SUB_KEY access rights.
End Rem
Const KEY_WRITE:Int = $20006


Rem
bbdoc: If this flag is set, the function ignores the samDesired parameter and attempts to open the key with the access required to backup or restore the key.
about: If the calling thread has the SE_BACKUP_NAME privilege enabled, the key is opened with the ACCESS_SYSTEM_SECURITY and KEY_READ access rights. If the calling thread has the SE_RESTORE_NAME privilege enabled, beginning with Windows Vista, the key is opened with the ACCESS_SYSTEM_SECURITY, DELETE and KEY_WRITE access rights. If both privileges are enabled, the key has the combined access rights for both privileges. For more information, see Running with Special Privileges.
End Rem
Const REG_OPTION_BACKUP_RESTORE:Int = $00000004
Rem
bbdoc: Note  Registry symbolic links should only be used for for application compatibility when absolutely necessary.
about: This key is a symbolic link. The target path is assigned to the L"SymbolicLinkValue" value of the key. The target path must be an absolute registry path.
End Rem
Const REG_OPTION_CREATE_LINK:Int = $00000002
Rem
bbdoc: This key is not volatile; this is the default.
about: The information is stored in a file and is preserved when the system is restarted. The RegSaveKey function saves keys that are not volatile.
End Rem
Const REG_OPTION_NON_VOLATILE:Int = $00000000
Rem
bbdoc: All keys created by the function are volatile.
about: The information is stored in memory and is not preserved when the corresponding registry hive is unloaded. For HKEY_LOCAL_MACHINE, this occurs only when the system initiates a full shutdown. For registry keys loaded by the RegLoadKey function, this occurs when the corresponding RegUnLoadKey is performed. The RegSaveKey function does not save volatile keys. This flag is ignored for keys that already exist.
End Rem
Const REG_OPTION_VOLATILE:Int = $00000001


Const REG_SZ:Int = 1
Const REG_EXPAND_SZ:Int = 2
Const REG_BINARY:Int = 3
Const REG_DWORD:Int = 4
Const REG_MULTI_SZ:Int = 7
Const REG_QWORD:Int = 11
