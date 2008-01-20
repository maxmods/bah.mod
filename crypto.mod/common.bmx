' Copyright (c) 2007 Bruce A Henderson
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

?macos
Import "-lcrypto"
?linux
Import "-lcrypto"
?win32
Import "-leay32"
Import "ssl/include/*.h"
?

Import "glue.cpp"

Extern

	Function bmx_EVP_CIPHER_CTX_create:Byte Ptr()
	Function bmx_EVP_EncryptInit_ex:Int(ctxPtr:Byte Ptr, cipherType:Byte Ptr, impl:Byte Ptr, key:Byte Ptr, kLen:Int, iv:Byte Ptr, vLen:Int)
	Function EVP_EncryptUpdate:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr, in:Byte Ptr, inl:Int)
	Function EVP_EncryptFinal_ex:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr)
	Function EVP_CIPHER_CTX_cleanup:Int(handle:Byte Ptr)

	Function bmx_EVP_DecryptInit_ex:Int(ctxPtr:Byte Ptr, cipherType:Byte Ptr, impl:Byte Ptr, key:Byte Ptr, kLen:Int, iv:Byte Ptr, vLen:Int)
	Function EVP_DecryptUpdate:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr, in:Byte Ptr, inl:Int)
	Function EVP_DecryptFinal_ex:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr)
	Function EVP_CIPHER_CTX_set_padding:Int(handle:Byte Ptr, padding:Int)
	Function EVP_CIPHER_CTX_set_key_length:Int(handle:Byte Ptr, keyLen:Int)
	Function EVP_CIPHER_CTX_ctrl:Int(handle:Byte Ptr, paramType:Int, arg:Int, ret:Int Ptr)

	Function bmx_EVP_CipherInit_ex:Int(ctxPtr:Byte Ptr, cipherType:Byte Ptr, impl:Byte Ptr, key:Byte Ptr, kLen:Int, iv:Byte Ptr, vLen:Int, enc:Int)
	Function EVP_CipherUpdate:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr, in:Byte Ptr, inl:Int)
	Function EVP_CipherFinal_ex:Int(handle:Byte Ptr, out:Byte Ptr, outl:Int Ptr)

	Function EVP_des_cbc:Byte Ptr()
	Function EVP_des_ecb:Byte Ptr()
	Function EVP_des_cfb:Byte Ptr()
	Function EVP_des_ofb:Byte Ptr()

	Function EVP_des_ede_cbc:Byte Ptr()
	Function EVP_des_ede:Byte Ptr()
	Function EVP_des_ede_cfb:Byte Ptr()
	Function EVP_des_ede_ofb:Byte Ptr()

	Function EVP_des_ede3_cbc:Byte Ptr()
	Function EVP_des_ede3:Byte Ptr()
	Function EVP_des_ede3_cfb:Byte Ptr()
	Function EVP_des_ede3_ofb:Byte Ptr()
	
	Function EVP_desx_cbc:Byte Ptr()

	Function EVP_bf_cbc:Byte Ptr()
	Function EVP_bf_ecb:Byte Ptr()
	Function EVP_bf_cfb:Byte Ptr()
	Function EVP_bf_ofb:Byte Ptr()
	
	Function EVP_rc4:Byte Ptr()

Rem
' Not included as standard in ssl because of patent issues
	Function EVP_idea_cbc:Byte Ptr()
	Function EVP_idea_ecb:Byte Ptr()
	Function EVP_idea_cfb:Byte Ptr()
	Function EVP_idea_ofb:Byte Ptr()
End Rem

	Function EVP_rc2_cbc:Byte Ptr()
	Function EVP_rc2_ecb:Byte Ptr()
	Function EVP_rc2_cfb:Byte Ptr()
	Function EVP_rc2_ofb:Byte Ptr()
	
	Function EVP_cast5_cbc:Byte Ptr()
	Function EVP_cast5_ecb:Byte Ptr()
	Function EVP_cast5_cfb:Byte Ptr()
	Function EVP_cast5_ofb:Byte Ptr()
	
	Function EVP_rc5_32_12_16_cbc:Byte Ptr()
	Function EVP_rc5_32_12_16_ecb:Byte Ptr()
	Function EVP_rc5_32_12_16_cfb:Byte Ptr()
	Function EVP_rc5_32_12_16_ofb:Byte Ptr()
	
End Extern

Rem
bbdoc: Helper function for converting an Int[] to a Byte[].
about: Obviously your ints should be in the range, 0-255.
End Rem
Function IntToByteArray:Byte[](ints:Int[])
	Local arr:Byte[] = New Byte[ints.length]
	For Local i:Int = 0 Until ints.length
		arr[i] = ints[i]
	Next
	Return arr
End Function

Rem
bbdoc: Helper function for converting a String to a Byte[].
End Rem
Function StringToByteArray:Byte[](text:String)
	Local arr:Byte[] = New Byte[text.length]
	For Local i:Int = 0 Until text.length
		arr[i] = text[i]
	Next
	Return arr
End Function

Const EVP_CTRL_INIT:Int = 0

Const EVP_CTRL_SET_KEY_LENGTH:Int = 1

Const EVP_CTRL_GET_RC2_KEY_BITS:Int = 2

Const EVP_CTRL_SET_RC2_KEY_BITS:Int = 3

Const EVP_CTRL_GET_RC5_ROUNDS:Int = 4

Const EVP_CTRL_SET_RC5_ROUNDS:Int = 5

Const EVP_CTRL_RAND_KEY:Int = 6


Const EVP_MAX_MD_SIZE:Int = 64	' longest known is SHA512

Const EVP_MAX_KEY_LENGTH:Int = 32

Const EVP_MAX_IV_LENGTH:Int = 16

Const EVP_MAX_BLOCK_LENGTH:Int = 32

