/*
  Copyright (c) 2016 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "windows.h"

#include "brl.mod/blitz.mod/blitz.h"

/* 32-bit XP doesn't have RegDeleteKeyEx and app loading will fail if we use it */
typedef LONG (WINAPI * MaxRegDeleteKeyEx)(HKEY hKey ,LPCTSTR lpSubKey, REGSAM samDesired, DWORD Reserved);
MaxRegDeleteKeyEx _RegDeleteKeyEx;

int bmx_reg_RegCreateKey(int hKey, BBString * lpSubKey, BBString * lpClass, int dwOptions, int samDesired, int lpSecurityAttributes, int * phkResult, int * lpdwDisposition) {

	BBChar * s = bbStringToWString(lpSubKey);
	BBChar * c = 0;
	
	if (lpClass != &bbEmptyString) {
		c = bbStringToWString(lpClass);
	}
	
	HKEY resultKey;

	int res = RegCreateKeyExW(hKey, s, 0, c, dwOptions, samDesired, lpSecurityAttributes, &resultKey, lpdwDisposition);
	
	*phkResult = resultKey;
	
	if (c) bbMemFree(c);
	bbMemFree(s);

	return res;
}

int bmx_reg_RegOpenKey(int hKey, BBString * lpSubKey, int samDesired, int * phkResult) {

	BBChar * s = 0;
	
	if (lpSubKey != &bbEmptyString) {
		s = bbStringToWString(lpSubKey);
	}

	HKEY resultKey;
	
	int res = RegOpenKeyExW(hKey, s, 0, samDesired, &resultKey);
	
	*phkResult = resultKey;

	if (s) bbMemFree(s);

	return res;
}

int bmx_reg_RegCloseKey(int hKey) {
	return RegCloseKey(hKey);
}


int bmx_reg_GetInt(int key, BBString * name, int * value) {
	BBChar * n = 0;
	int size = 4;
	int type;
	
	if (name != &bbEmptyString) {
		n = bbStringToWString(name);
	}

	// check type
	int res = RegQueryValueExW(key, n, 0, &type, 0, 0);

	if (type == REG_DWORD) {
		res = RegQueryValueExW(key, n, 0, &type, value, &size);
	} else {
		res = -1;
	}
	
	if (n) bbMemFree(n);
	
	return res;
}

int bmx_reg_GetLong(int key, BBString * name, BBInt64 * value) {
	BBChar * n = 0;
	int size = 8;
	int type;
	
	if (name != &bbEmptyString) {
		n = bbStringToWString(name);
	}

	// check type
	int res = RegQueryValueExW(key, n, 0, &type, 0, 0);

	if (type == REG_QWORD) {
		res = RegQueryValueExW(key, n, 0, &type, value, &size);
	} else {
		res = -1;
	}
	
	if (n) bbMemFree(n);
	
	return res;
}

BBString * bmx_reg_GetString(int key, BBString * name, int * res) {
	BBChar * n = 0;
	BBString * value = &bbEmptyString;
	int size;
	int type;
	BBChar * buf;

	if (name != &bbEmptyString) {
		n = bbStringToWString(name);
	}
	
	// check type and size
	*res = RegQueryValueExW(key, n, 0, &type, NULL, &size);


	if (type == REG_SZ) {
		buf = (BBChar*)malloc(size);
	
		*res = RegQueryValueExW(key, n, 0, &type, buf, &size);
		
		value = bbStringFromWString(buf);
		
		free(buf);
		
	} else {
		*res = -1;
	}
	
	if (n) bbMemFree(n);

	return value;
}


int bmx_reg_SetInt(int key, BBString * name, int value) {
	BBChar * n = bbStringToWString(name);
	
	int res = RegSetValueExW(key, n, 0, REG_DWORD, &value, 4);
	
	bbMemFree(n);
	
	return res;
}

int bmx_reg_SetLong(int key, BBString * name, BBInt64 value) {
	BBChar * n = bbStringToWString(name);
	
	int res = RegSetValueExW(key, n, 0, REG_QWORD, &value, 8);
	
	bbMemFree(n);
	
	return res;
}

int bmx_reg_SetString(int key, BBString * name, BBString * value) {
	BBChar * n = bbStringToWString(name);
	BBChar * v = bbStringToWString(value);
	
	int res = RegSetValueExW(key, n, 0, REG_SZ, v, value->length * 2 + 1);
	
	bbMemFree(v);
	bbMemFree(n);
	
	return res;
}

BBString * bmx_reg_RegEnumKey(int key, int index, int * hasNext) {
	BBString * value = &bbEmptyString;
	BBChar subKey[255];
	int size = 255;

	int res = RegEnumKeyExW(key, index, subKey, &size, 0, 0, 0, 0);

	if (res == ERROR_SUCCESS) {
		value = bbStringFromWString(subKey);
		*hasNext = 1;
	} else {
		*hasNext = 0;
	}

	return value;
}

int bmx_reg_RegDeleteKey(int key, BBString * subKey, int samDesired) {

	int desired = samDesired;

	if (desired == 0) {
#ifdef __x86_64__
		desired = KEY_WOW64_64KEY;
#else
		desired = KEY_WOW64_32KEY;
#endif
	}

	BBChar * s = bbStringToWString(subKey);
	int res = 0;

	static int regDeleteInited = 0;

	if (!regDeleteInited) {
		regDeleteInited = 1;
  		 HINSTANCE hAdvAPI32 = LoadLibrary("AdvAPI32.dll");

		_RegDeleteKeyEx = (MaxRegDeleteKeyEx)GetProcAddress(hAdvAPI32 , "RegDeleteKeyExW");
	}
	
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if ((info.dwMajorVersion > 5) || ((info.dwMajorVersion == 5) && (info.dwMinorVersion > 1))) {
		res = _RegDeleteKeyEx(key, s, desired, 0);
	} else {
		res = RegDeleteKeyW(key, s);
	}
	
	bbMemFree(s);
	
	return res;
}

int bmx_reg_RegDeleteValue(int key, BBString * valueName) {
	BBChar * v = bbStringToWString(valueName);
	
	int res = RegDeleteValueW(key, v);
	
	bbMemFree(v);
	
	return res;
}

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

void CB_PREF(bah_registry_TRegistryValue__setName)(BBObject * value, BBString * name);
void CB_PREF(bah_registry_TRegistryValue__setValueType)(BBObject * value, int valueType);
void CB_PREF(bah_registry_TRegistryValue__setInteger)(BBObject * value, int v);
void CB_PREF(bah_registry_TRegistryValue__setLong)(BBObject * value, BBInt64 v);
void CB_PREF(bah_registry_TRegistryValue__setString)(BBObject * value, BBString * v);

int bmx_reg_RegEnumValue(int key, int index, BBObject * value) {

	BBChar nameChar[2048];
	int nameSize = 2048;
	int valueType;
	int dataSize = 0;

	// get the data type and size
	int res = RegEnumValueW(key, index, nameChar, &nameSize, 0, &valueType, NULL, &dataSize);

	if (res == ERROR_SUCCESS) {
		CB_PREF(bah_registry_TRegistryValue__setName)(value, bbStringFromShorts(nameChar, nameSize));
		CB_PREF(bah_registry_TRegistryValue__setValueType)(value, valueType);
	
		switch (valueType) {
			case REG_DWORD:
				{
					int v;
					nameSize = 2048;
					dataSize = 4;
					res = RegEnumValueW(key, index, nameChar, &nameSize, 0, NULL, &v, &dataSize);
					
					CB_PREF(bah_registry_TRegistryValue__setInteger)(value, v);
					
					break;
				}
			case REG_QWORD:
				{
					BBInt64 v;
					nameSize = 2048;
					dataSize = 8;
					res = RegEnumValueW(key, index, nameChar, &nameSize, 0, NULL, &v, &dataSize);
					
					CB_PREF(bah_registry_TRegistryValue__setLong)(value, v);
					
					break;
				}
			case REG_SZ:
			case REG_EXPAND_SZ:
			case REG_LINK:
				{
					BBChar valueChar[8192];
					nameSize = 2048;
					if (dataSize > 8192) {
						dataSize = 8192;
					}
					res = RegEnumValueW(key, index, nameChar, &nameSize, 0, NULL, valueChar, &dataSize);
					
					CB_PREF(bah_registry_TRegistryValue__setString)(value, bbStringFromWString(valueChar));
					
					break;
				}
		}
	}
	
	return (res == ERROR_SUCCESS);
}


