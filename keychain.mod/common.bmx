' Copyright (c) 2010 Bruce A Henderson
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
Import Pub.Macos

Import "-framework Security"
Import "glue.m"

Extern

	Function bmx_keychain_SecCopyErrorMessageString:String(status:Int)

	Function bmx_keychain_open:Byte Ptr(path:String)
	Function bmx_keychain_free(handle:Byte Ptr)
	Function bmx_keychain_create:Byte Ptr(path:String, password:String, promptUser:Int)

	Function bmx_keychain_SecKeychainSetUserInteractionAllowed(allowed:Int)
	Function bmx_keychain_SecKeychainGetUserInteractionAllowed:Int()
	Function bmx_keychain_SecKeychainFindInternetPassword:String(handle:Byte Ptr, protocol:Int, serverName:String, accountName:String, path:String, ..
			authenticationType:Int, securityDomain:String, port:Int, item:Byte Ptr)
	Function bmx_keychain_SecKeychainAddInternetPassword(handle:Byte Ptr, password:String, protocol:Int, serverName:String, accountName:String, ..
		path:String, authenticationType:Int, securityDomain:String, port:Int, item:Byte Ptr)
	Function bmx_keychain_SecKeychainFindGenericPassword:String(handle:Byte Ptr, serviceName:String, accountName:String, item:Byte Ptr)
	Function bmx_keychain_SecKeychainAddGenericPassword(handle:Byte Ptr, password:String, serviceName:String, accountName:String, item:Byte Ptr)
	Function bmx_keychain_SecKeychainUnlock(handle:Byte Ptr, password:String, usePassword:Int)
	Function bmx_keychain_SecKeychainSetDefault(handle:Byte Ptr)
	Function bmx_keychain_SecKeychainGetStatus:Int(handle:Byte Ptr)
	Function bmx_keychain_SecKeychainGetPath:String(handle:Byte Ptr)
	Function bmx_keychain_SecKeychainDelete(handle:Byte Ptr)

	Function bmx_keychainitem_create:Byte Ptr()
	Function bmx_keychainitem_free(handle:Byte Ptr)

	Function bmx_keychainitem_SecKeychainItemModifyAttributesAndData(handle:Byte Ptr, password:String)
	Function bmx_keychainitem_SecKeychainItemDelete(handle:Byte Ptr)

End Extern



Rem
bbdoc: Indicates FTP.
End Rem
Const kSecProtocolTypeFTP:Int = Asc("f") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates HTTP.
End Rem
Const kSecProtocolTypeHTTP:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "t") Shl 8 | Asc("p")
Rem
bbdoc: Indicates IRC.
End Rem
Const kSecProtocolTypeIRC:Int = Asc("i") Shl 24 | Asc("r") Shl 16 | Asc( "c") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates NNTP.
End Rem
Const kSecProtocolTypeNNTP:Int = Asc("n") Shl 24 | Asc("n") Shl 16 | Asc( "t") Shl 8 | Asc("p")
Rem
bbdoc: Indicates POP3.
End Rem
Const kSecProtocolTypePOP3:Int = Asc("p") Shl 24 | Asc("o") Shl 16 | Asc( "p") Shl 8 | Asc("3")
Rem
bbdoc: Indicates SMTP.
End Rem
Const kSecProtocolTypeSMTP:Int = Asc("s") Shl 24 | Asc("m") Shl 16 | Asc( "t") Shl 8 | Asc("p")
Rem
bbdoc: Indicates SOCKS.
End Rem
Const kSecProtocolTypeSOCKS:Int = Asc("s") Shl 24 | Asc("o") Shl 16 | Asc( "x") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates IMAP.
End Rem
Const kSecProtocolTypeIMAP:Int = Asc("i") Shl 24 | Asc("m") Shl 16 | Asc( "a") Shl 8 | Asc("p")
Rem
bbdoc: Indicates LDAP.
End Rem
Const kSecProtocolTypeLDAP:Int = Asc("l") Shl 24 | Asc("d") Shl 16 | Asc( "a") Shl 8 | Asc("p")
Rem
bbdoc: Indicates AFP over AppleTalk.
End Rem
Const kSecProtocolTypeAppleTalk:Int = Asc("a") Shl 24 | Asc("t") Shl 16 | Asc( "l") Shl 8 | Asc("k")
Rem
bbdoc: Indicates AFP over TCP.
End Rem
Const kSecProtocolTypeAFP:Int = Asc("a") Shl 24 | Asc("f") Shl 16 | Asc( "p") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates Telnet.
End Rem
Const kSecProtocolTypeTelnet:Int = Asc("t") Shl 24 | Asc("e") Shl 16 | Asc( "l") Shl 8 | Asc("n")
Rem
bbdoc: Indicates SSH.
End Rem
Const kSecProtocolTypeSSH:Int = Asc("s") Shl 24 | Asc("s") Shl 16 | Asc( "h") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates FTP over TLS/SSL.
End Rem
Const kSecProtocolTypeFTPS:Int = Asc("f") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates HTTP over TLS/SSL.
End Rem
Const kSecProtocolTypeHTTPS:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates HTTP proxy.
End Rem
Const kSecProtocolTypeHTTPProxy:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc("x")
Rem
bbdoc: Indicates HTTPS proxy.
End Rem
Const kSecProtocolTypeHTTPSProx:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "s") Shl 8 | Asc("x")
Rem
bbdoc: Indicates FTP proxy.
End Rem
Const kSecProtocolTypeFTPProxy:Int = Asc("f") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc("x")
Rem
bbdoc: Indicates CIFS.
End Rem
Const kSecProtocolTypeCIFS:Int = Asc("c") Shl 24 | Asc("i") Shl 16 | Asc( "f") Shl 8 | Asc("s")
Rem
bbdoc: Indicates SMB.
End Rem
Const kSecProtocolTypeSMB:Int = Asc("s") Shl 24 | Asc("m") Shl 16 | Asc( "b") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates RTSP.
End Rem
Const kSecProtocolTypeRTSP:Int = Asc("r") Shl 24 | Asc("t") Shl 16 | Asc( "s") Shl 8 | Asc("p")
Rem
bbdoc: Indicates RTSP proxy.
End Rem
Const kSecProtocolTypeRTSPProxy:Int = Asc("r") Shl 24 | Asc("t") Shl 16 | Asc( "s") Shl 8 | Asc("x")
Rem
bbdoc: Indicates DAAP.
End Rem
Const kSecProtocolTypeDAAP:Int = Asc("d") Shl 24 | Asc("a") Shl 16 | Asc( "a") Shl 8 | Asc("p")
Rem
bbdoc: Indicates Remote Apple Events.
End Rem
Const kSecProtocolTypeEPPC:Int = Asc("e") Shl 24 | Asc("p") Shl 16 | Asc( "p") Shl 8 | Asc("c")
Rem
bbdoc: Indicates IPP.
End Rem
Const kSecProtocolTypeIPP:Int = Asc("i") Shl 24 | Asc("p") Shl 16 | Asc( "p") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates NNTP over TLS/SSL.
End Rem
Const kSecProtocolTypeNNTPS:Int = Asc("n") Shl 24 | Asc("t") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates LDAP over TLS/SSL.
End Rem
Const kSecProtocolTypeLDAPS:Int = Asc("l") Shl 24 | Asc("d") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates Telnet over TLS/SSL.
End Rem
Const kSecProtocolTypeTelnetS:Int = Asc("t") Shl 24 | Asc("e") Shl 16 | Asc( "l") Shl 8 | Asc("s")
Rem
bbdoc: Indicates IMAP4 over TLS/SSL.
End Rem
Const kSecProtocolTypeIMAPS:Int = Asc("i") Shl 24 | Asc("m") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates IRC over TLS/SSL.
End Rem
Const kSecProtocolTypeIRCS:Int = Asc("i") Shl 24 | Asc("r") Shl 16 | Asc( "c") Shl 8 | Asc("s")
Rem
bbdoc: Indicates POP3 over TLS/SSL.
End Rem
Const kSecProtocolTypePOP3S:Int = Asc("p") Shl 24 | Asc("o") Shl 16 | Asc( "p") Shl 8 | Asc("s")
Rem
bbdoc: Indicates CVS pserver.
End Rem
Const kSecProtocolTypeCVSpserver:Int = Asc("c") Shl 24 | Asc("v") Shl 16 | Asc( "s") Shl 8 | Asc("p")
Rem
bbdoc: Indicates Subversion.
End Rem
Const kSecProtocolTypeSVN:Int = Asc("s") Shl 24 | Asc("v") Shl 16 | Asc( "n") Shl 8 | Asc(" ")
Rem
bbdoc: Indicates that any protocol is acceptable.
about: When performing a search, use this constant to avoid constraining your search results to a particular protocol.
End Rem
Const kSecProtocolTypeAny:Int = 0

Rem
bbdoc: Specifies Windows NT LAN Manager authentication.
End Rem
Const kSecAuthenticationTypeNTLM:Int = Asc("n") Shl 24 | Asc("t") Shl 16 | Asc( "l") Shl 8 | Asc("m")
Rem
bbdoc: Specifies Microsoft Network default authentication.
End Rem
Const kSecAuthenticationTypeMSN:Int = Asc("m") Shl 24 | Asc("s") Shl 16 | Asc( "n") Shl 8 | Asc("a")
Rem
bbdoc: Specifies Distributed Password authentication.
End Rem
Const kSecAuthenticationTypeDPA:Int = Asc("d") Shl 24 | Asc("p") Shl 16 | Asc( "a") Shl 8 | Asc("a")
Rem
bbdoc: Specifies Remote Password authentication. 
End Rem
Const kSecAuthenticationTypeRPA:Int = Asc("r") Shl 24 | Asc("p") Shl 16 | Asc( "a") Shl 8 | Asc("a")
Rem
bbdoc: Specifies HTTP Basic authentication.
End Rem
Const kSecAuthenticationTypeHTTPBasic:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "t") Shl 8 | Asc("p")
Rem
bbdoc: Specifies HTTP Digest Access authentication.
End Rem
Const kSecAuthenticationTypeHTTPDigest:Int = Asc("h") Shl 24 | Asc("t") Shl 16 | Asc( "t") Shl 8 | Asc("d")
Rem
bbdoc: Specifies HTML form based authentication.
End Rem
Const kSecAuthenticationTypeHTMLForm:Int = Asc("f") Shl 24 | Asc("o") Shl 16 | Asc( "r") Shl 8 | Asc("m")
Rem
bbdoc: Specifies the default authentication type.
End Rem
Const kSecAuthenticationTypeDefault:Int = Asc("d") Shl 24 | Asc("f") Shl 16 | Asc( "l") Shl 8 | Asc("t")
Rem
bbdoc: Specifies that any authentication type is acceptable.
about: When performing a search, use this constant to avoid constraining your search results to a particular authentication type.
End Rem
Const kSecAuthenticationTypeAny:Int = 0


Rem
bbdoc: Indicates a keychain was locked.
End Rem
Const kSecLockEvent:Int = 1
Rem
bbdoc: Indicates a keychain was unlocked.
End Rem
Const kSecUnlockEvent:Int = 2
Rem
bbdoc: Indicates an item was added to a keychain.
End Rem
Const kSecAddEvent:Int = 3
Rem
bbdoc: Indicates an item was deleted from a keychain.
End Rem
Const kSecDeleteEvent:Int = 4
Rem
bbdoc: Indicates a keychain item was updated.
End Rem
Const kSecUpdateEvent:Int = 5
Rem
bbdoc: Indicates the keychain password was changed.
End Rem
Const kSecPasswordChangedEvent:Int = 6
Rem
bbdoc: Indicates that a different keychain was specified as the default.
End Rem
Const kSecDefaultChangedEvent:Int = 9
Rem
bbdoc: Indicates a process has accessed a keychain item's data.
End Rem
Const kSecDataAccessEvent:Int = 10
Rem
bbdoc: Indicates the list of keychains has changed.
End Rem
Const kSecKeychainListChangedEvent:Int = 11
Rem
bbdoc: Indicates Trust Settings changed.
End Rem
Const kSecTrustSettingsChangedEvent:Int = 12

Rem
bbdoc: Indicates the keychain is unlocked.
about: You can use this mask in combination. For example, a keychain may be both readable and writable.
End Rem
Const kSecUnlockStateStatus:Int = 1
Rem
bbdoc: Indicates the keychain is readable.
about: You can use this mask in combination. For example, a keychain may be both readable and writable.
End Rem
Const kSecReadPermStatus:Int = 2
Rem
bbdoc: Indicates the keychain is writable.
about: You can use this mask in combination. For example, a keychain may be both readable and writable.
End Rem
Const kSecWritePermStatus:Int = 4

Rem
bbdoc: No error.
End Rem
Const errSecSuccess:Int = 0
Rem
bbdoc: Function or operation not implemented.
End Rem
Const errSecUnimplemented:Int = -4
Rem
bbdoc: One or more parameters passed to the function were not valid.
End Rem
Const errSecParam:Int = -50
Rem
bbdoc: Failed to allocate memory.
End Rem
Const errSecAllocate:Int = -108
Rem
bbdoc: No trust results are available.
End Rem
Const errSecNotAvailable:Int = -25291
Rem
bbdoc: Read only error.
End Rem
Const errSecReadOnly:Int = -25292
Rem
bbdoc: Authorization/Authentication failed.
End Rem
Const errSecAuthFailed:Int = -25293
Rem
bbdoc: The keychain does not exist.
End Rem
Const errSecNoSuchKeychain:Int = -25294
Rem
bbdoc: The keychain is not valid.
End Rem
Const errSecInvalidKeychain:Int = -25295
Rem
bbdoc: A keychain with the same name already exists.
End Rem
Const errSecDuplicateKeychain:Int = -25296
Rem
bbdoc: More than one callback of the same name exists.
End Rem
Const errSecDuplicateCallback:Int = -25297
Rem
bbdoc: The callback is not valid.
End Rem
Const errSecInvalidCallback:Int = -25298
Rem
bbdoc: The item already exists.
End Rem
Const errSecDuplicateItem:Int = -25299
Rem
bbdoc: The item cannot be found.
End Rem
Const errSecItemNotFound:Int = -25300
Rem
bbdoc: The buffer is too small.
End Rem
Const errSecBufferTooSmall:Int = -25301
Rem
bbdoc: The data is too large for the particular data type.
End Rem
Const errSecDataTooLarge:Int = -25302
Rem
bbdoc: The attribute does not exist.
End Rem
Const errSecNoSuchAttr:Int = -25303
Rem
bbdoc: The item reference is invalid.
End Rem
Const errSecInvalidItemRef:Int = -25304
Rem
bbdoc: The search reference is invalid.
End Rem
Const errSecInvalidSearchRef:Int = -25305
Rem
bbdoc: The keychain item class does not exist.
End Rem
Const errSecNoSuchClass:Int = -25306
Rem
bbdoc: A default keychain does not exist.
End Rem
Const errSecNoDefaultKeychain:Int = -25307
Rem
bbdoc: Interaction with the Security Server is not allowed.
End Rem
Const errSecInteractionNotAllowed:Int = -25308
Rem
bbdoc: The attribute is read only.
End Rem
Const errSecReadOnlyAttr:Int = -25309
Rem
bbdoc: The version is incorrect.
End Rem
Const errSecWrongSecVersion:Int = -25310
Rem
bbdoc: The key size is not allowed.
End Rem
Const errSecKeySizeNotAllowed:Int = -25311
Rem
bbdoc: There is no storage module available.
End Rem
Const errSecNoStorageModule:Int = -25312
Rem
bbdoc: There is no certificate module available.
End Rem
Const errSecNoCertificateModule:Int = -25313
Rem
bbdoc: There is no policy module available.
End Rem
Const errSecNoPolicyModule:Int = -25314
Rem
bbdoc: User interaction is required.
End Rem
Const errSecInteractionRequired:Int = -25315
Rem
bbdoc: The data is not available.
End Rem
Const errSecDataNotAvailable:Int = -25316
Rem
bbdoc: The data is not modifiable.
End Rem
Const errSecDataNotModifiable:Int = -25317
Rem
bbdoc: The attempt to create a certificate chain failed.
End Rem
Const errSecCreateChainFailed:Int = -25318
Rem
bbdoc: The preference domain specified is invalid. This error is 
End Rem
Const errSecInvalidPrefsDomain:Int = -25319
Rem
bbdoc: The access control list is not in standard simple form.
End Rem
Const errSecACLNotSimple:Int = -25240
Rem
bbdoc: The policy specified cannot be found.
End Rem
Const errSecPolicyNotFound:Int = -25241
Rem
bbdoc: The trust setting is invalid.
End Rem
Const errSecInvalidTrustSetting:Int = -25242
Rem
bbdoc: The specified item has no access control.
End Rem
Const errSecNoAccessForItem:Int = -25243
Rem
bbdoc: An invalid attempt to change the owner of an item.
End Rem
Const errSecInvalidOwnerEdit:Int = -25244
Rem
bbdoc: No trust results are available.
End Rem
Const errSecTrustNotAvailable:Int = -25245
Rem
bbdoc: The specified import or export format is not supported.
End Rem
Const errSecUnsupportedFormat:Int = -25256
Rem
bbdoc: The item you are trying to import has an unknown format.
End Rem
Const errSecUnknownFormat:Int = -25257
Rem
bbdoc: The key must be wrapped to be exported.
End Rem
Const errSecKeyIsSensitive:Int = -25258
Rem
bbdoc: An attempt was made to import multiple private keys.
End Rem
Const errSecMultiplePrivKeys:Int = -25259
Rem
bbdoc: A password is required for import or export.
End Rem
Const errSecPassphraseRequired:Int = -25260
Rem
bbdoc: The password reference was invalid.
End Rem
Const errSecInvalidPasswordRef:Int = -25261
Rem
bbdoc: The trust settings record was corrupted.
End Rem
Const errSecInvalidTrustSettings:Int = -25262
Rem
bbdoc: No trust settings were found.
End Rem
Const errSecNoTrustSettings:Int = -25263
Rem
bbdoc: MAC verification failed during PKCS12 Import.
End Rem
Const errSecPkcs12VerifyFailure:Int = -25264
Rem
bbdoc: Unable to decode the provided data.
End Rem
Const errSecDecode:Int = -26275


