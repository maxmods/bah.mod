' Copyright (c) 2007-2009 Bruce A Henderson
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
bbdoc: Cryptography
End Rem
Module BaH.Crypto

ModuleInfo "Version: 1.02"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007-2009 Bruce A Henderson"

ModuleInfo "History: 1.02"
ModuleInfo "History: Added TStream support for the 'easy' digest API."
ModuleInfo "History: 1.01"
ModuleInfo "History: Added digest functions."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/ssl/lib"
?

Import "common.bmx"


Rem
bbdoc: A Cipher Context.
End Rem
Type EVP_CIPHER_CTX

	Field ctxPtr:Byte Ptr

	Rem
	bbdoc: Creates and initializes a Cipher Context.
	End Rem
	Function CreateContext:EVP_CIPHER_CTX()
		Return New EVP_CIPHER_CTX.Create()
	End Function
	
	Rem
	bbdoc: Creates and initializes a Cipher Context.
	End Rem
	Method Create:EVP_CIPHER_CTX()
		ctxPtr = bmx_EVP_CIPHER_CTX_create()
		Return Self
	End Method

	Rem
	bbdoc: Sets up cipher context ctx for encryption with @cipherType from ENGINE @impl.
	returns: True for success, False for failure.
	about: @cipherType is normally supplied by a function such as
       EVP_des_cbc(). If impl is NULL then the default implementation is used.
       key is the symmetric key to use and iv is the IV to use (if necessary),
       the actual number of bytes used for the key and IV depends on the
       cipher. It is possible to set all parameters to NULL except type in an
       initial call and supply the remaining parameters in subsequent calls,
       all of which have type set to NULL. This is done when the default
       cipher parameters are not appropriate.
	End Rem
	Method EncryptInit:Int(cipherType:EVP_CIPHER = Null, impl:ENGINE = Null, key:Byte[] = Null, iv:Byte[] = Null)
		Local kLen:Int, vLen:Int
		If key Then
			kLen = key.length
		End If
		If iv Then
			vLen = iv.length
		End If
		
		If cipherType Then
			If impl Then
				Return bmx_EVP_EncryptInit_ex(ctxPtr, cipherType.cipherPtr, impl.enginePtr, key, kLen, iv, vLen)		
			Else
				Return bmx_EVP_EncryptInit_ex(ctxPtr, cipherType.cipherPtr, Null, key, kLen, iv, vLen)		
			End If
		Else
			If impl Then
				Return bmx_EVP_EncryptInit_ex(ctxPtr, Null, impl.enginePtr, key, kLen, iv, vLen)		
			Else
				Return bmx_EVP_EncryptInit_ex(ctxPtr, Null, Null, key, kLen, iv, vLen)		
			End If
		End If
	End Method

	Rem
	bbdoc: Encrypts inl bytes from the buffer in and writes the encrypted version to out.
	returns: True for success, False for failure.
	about: This method can be called multiple
       times to encrypt successive blocks of data. The amount of data written
       depends on the block alignment of the encrypted data: as a result the
       amount of data written may be anything from zero bytes to (inl +
       cipher_block_size - 1) so outl should contain sufficient room. The
       actual number of bytes written is placed in outl.
	End Rem
	Method EncryptUpdate:Int(out:Byte Ptr, outl:Int Var, in:Byte Ptr, inl:Int)
		Return EVP_EncryptUpdate(ctxPtr, out, Varptr outl, in, inl)
	End Method
	
	Rem
	bbdoc: If padding is enabled (the default) then EncryptFinal() encrypts the "final" data, that is any data that remains in a partial block.
	returns: True for success, False for failure.
	about: It
       uses standard block padding (aka PKCS padding). The encrypted final
       data is written to out which should have sufficient space for one
       cipher block. The number of bytes written is placed in outl. After this
       function is called the encryption operation is finished and no further
       calls to EncryptUpdate() should be made.
	End Rem
	Method EncryptFinal:Int(out:Byte Ptr, outl:Int Var)
		Return EVP_EncryptFinal_ex(ctxPtr, out, Varptr outl)
	End Method
	
	Rem
	bbdoc: Clears all information from a cipher context and free up any allocated memory associate with it.
	returns: True for success and False for failure.
	about: It should be called
       after all operations using a cipher are complete so sensitive
       information does not remain in memory.
	End Rem
	Method Cleanup:Int()
		Return EVP_CIPHER_CTX_cleanup(ctxPtr)
	End Method

	Rem
	bbdoc: 
	returns: True for success, False for failure.
	End Rem
	Method DecryptInit:Int(cipherType:EVP_CIPHER = Null, impl:ENGINE = Null, key:Byte[] = Null, iv:Byte[] = Null)
		Local kLen:Int, vLen:Int
		If key Then
			kLen = key.length
		End If
		If iv Then
			vLen = iv.length
		End If
		
		If cipherType Then
			If impl Then
				Return bmx_EVP_DecryptInit_ex(ctxPtr, cipherType.cipherPtr, impl.enginePtr, key, kLen, iv, vLen)		
			Else
				Return bmx_EVP_DecryptInit_ex(ctxPtr, cipherType.cipherPtr, Null, key, kLen, iv, vLen)		
			End If
		Else
			If impl Then
				Return bmx_EVP_DecryptInit_ex(ctxPtr, Null, impl.enginePtr, key, kLen, iv, vLen)		
			Else
				Return bmx_EVP_DecryptInit_ex(ctxPtr, Null, Null, key, kLen, iv, vLen)		
			End If
		End If
	End Method

	Rem
	bbdoc: 
	returns: True for success, False for failure.
	End Rem
	Method DecryptUpdate:Int(out:Byte Ptr, outl:Int Var, in:Byte Ptr, inl:Int)
		Return EVP_DecryptUpdate(ctxPtr, out, Varptr outl, in, inl)
	End Method

	Rem
	bbdoc: 
	returns: False for a decryption failure or True for success.
	End Rem
	Method DecryptFinal:Int(outm:Byte Ptr, outl:Int Var)
		Return EVP_DecryptFinal_ex(ctxPtr, outm, Varptr outl)
	End Method
	
	Rem
	bbdoc: Enables or disables padding.
	about: By default
       encryption operations are padded using standard block padding and the
       padding is checked and removed when decrypting. If the pad parameter is
       zero then no padding is performed, the total amount of data encrypted
       or decrypted must then be a multiple of the block size or an error will
       occur.
	End Rem
	Method SetPadding:Int(padding:Int)
		Return EVP_CIPHER_CTX_set_padding(ctxPtr, padding)
	End Method
	
	Rem
	bbdoc: Sets the key length of the ciphe.
     about: If the cipher is a fixed length cipher then attempting to set the key
       length to any value other than the fixed value is an error.
	End Rem
	Method SetKeyLength:Int(keyLen:Int)
		Return EVP_CIPHER_CTX_set_key_length(ctxPtr, keyLen)
	End Method
	
	Rem
	bbdoc: Allows various cipher specific parameters to be set.
	about: Currently only the RC2 effective key length and the
       number of rounds of RC5 can be set :
	<ul>
	<li>EVP_CTRL_SET_KEY_LENGTH</li>
	<li>EVP_CTRL_SET_RC2_KEY_BITS</li>
	<li>EVP_CTRL_SET_RC5_ROUNDS</li>
	</ul>
	End Rem
	Method CtrlSet:Int(paramType:Int, arg:Int)
		Return EVP_CIPHER_CTX_ctrl(ctxPtr, paramType, arg, Null)
	End Method

	Rem
	bbdoc: Allows various cipher specific parameters to be determined.
	about: Valid paramTypes are :
	<ul>
	<li>EVP_CTRL_GET_RC2_KEY_BITS</li>
	<li>EVP_CTRL_GET_RC5_ROUNDS</li>
	</ul>
	End Rem
	Method CtrlGet:Int(paramType:Int, arg:Int Var)
		Return EVP_CIPHER_CTX_ctrl(ctxPtr, paramType, 0, Varptr arg)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CipherInit:Int(cipherType:EVP_CIPHER = Null, impl:ENGINE = Null, key:Byte[] = Null, iv:Byte[] = Null, enc:Int)
		Local kLen:Int, vLen:Int
		If key Then
			kLen = key.length
		End If
		If iv Then
			vLen = iv.length
		End If
		
		If cipherType Then
			If impl Then
				Return bmx_EVP_CipherInit_ex(ctxPtr, cipherType.cipherPtr, impl.enginePtr, key, kLen, iv, vLen, enc)		
			Else
				Return bmx_EVP_CipherInit_ex(ctxPtr, cipherType.cipherPtr, Null, key, kLen, iv, vLen, enc)		
			End If
		Else
			If impl Then
				Return bmx_EVP_CipherInit_ex(ctxPtr, Null, impl.enginePtr, key, kLen, iv, vLen, enc)		
			Else
				Return bmx_EVP_CipherInit_ex(ctxPtr, Null, Null, key, kLen, iv, vLen, enc)
			End If
		End If
	End Method

	Rem
	bbdoc: 
	returns: True for success, False for failure.
	End Rem
	Method CipherUpdate:Int(out:Byte Ptr, outl:Int Var, in:Byte Ptr, inl:Int)
		Return EVP_CipherUpdate(ctxPtr, out, Varptr outl, in, inl)
	End Method

	Rem
	bbdoc: 
	returns: False for a decryption failure or True for success.
	End Rem
	Method CipherFinal:Int(outm:Byte Ptr, outl:Int Var)
		Return EVP_CipherFinal_ex(ctxPtr, outm, Varptr outl)
	End Method

	Method Delete()
		If ctxPtr Then
			bmx_EVP_CIPHER_CTX_delete(ctxPtr)
			ctxPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Cipher Algorithms
End Rem
Type EVP_CIPHER

	Field cipherPtr:Byte Ptr

	Rem
	bbdoc: DES, using CBC (Cipher-block chaining) mode.
	End Rem
	Function des_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_cbc()
		Return this
	End Function
	
	Rem
	bbdoc: DES, using ECB (Electronic codebook) mode.
	End Rem
	Function des_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ecb()
		Return this
	End Function
	
	Rem
	bbdoc: DES, using CFB (Cipher feedback) mode.
	End Rem
	Function des_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_cfb()
		Return this
	End Function
	
	Rem
	bbdoc: DES, using OFB (Output feedback) mode.
	End Rem
	Function des_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ofb()
		Return this
	End Function

	Rem
	bbdoc: Two key triple DES, using CBC (Cipher-block chaining) mode.
	End Rem
	Function des_ede_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede_cbc()
		Return this
	End Function

	Rem
	bbdoc: Two key triple DES, using ECB (Electronic codebook) mode.
	End Rem
	Function des_ede:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede()
		Return this
	End Function

	Rem
	bbdoc: Two key triple DES, using OFB (Output feedback) mode.
	End Rem
	Function des_ede_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede_ofb()
		Return this
	End Function

	Rem
	bbdoc: Two key triple DES, using CFB (Cipher feedback) mode.
	End Rem
	Function des_ede_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede_cfb()
		Return this
	End Function

	Rem
	bbdoc: Three key triple DES, using CBC (Cipher-block chaining) mode.
	End Rem
	Function des_ede3_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede3_cbc()
		Return this
	End Function

	Rem
	bbdoc: Three key triple DES, using ECB (Electronic codebook) mode.
	End Rem
	Function des_ede3:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede3()
		Return this
	End Function

	Rem
	bbdoc: Three key triple DES, using OFB (Output feedback) mode.
	End Rem
	Function des_ede3_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede3_ofb()
		Return this
	End Function

	Rem
	bbdoc: Three key triple DES, using CFB (Cipher feedback) mode.
	End Rem
	Function des_ede3_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_des_ede3_cfb()
		Return this
	End Function

	Rem
	bbdoc: DESX algorithm, using CFB (Cipher feedback) mode.
	End Rem
	Function desx_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_desx_cbc()
		Return this
	End Function

	Rem
	bbdoc: Blowfish, using CBC (Cipher-block chaining) mode.
	End Rem
	Function bf_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_bf_cbc()
		Return this
	End Function
	
	Rem
	bbdoc: Blowfish, using ECB (Electronic codebook) mode.
	End Rem
	Function bf_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_bf_ecb()
		Return this
	End Function
	
	Rem
	bbdoc: Blowfish, using CFB (Cipher feedback) mode.
	End Rem
	Function bf_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_bf_cfb()
		Return this
	End Function
	
	Rem
	bbdoc: Blowfish, using OFB (Output feedback) mode.
	End Rem
	Function bf_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_bf_ofb()
		Return this
	End Function

	Rem
	bbdoc: RC4 stream cipher.
	about: This is a variable key length cipher with default key length 128 bits.
	End Rem
	Function rc4:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc4()
		Return this
	End Function

Rem
' Not included as standard in ssl because of patent issues
'	Rem
'	bbdoc: IDEA encryption algorithm, using CBC (Cipher-block chaining) mode.
'	End Rem
	Function idea_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_idea_cbc()
		Return this
	End Function

'	Rem
'	bbdoc: IDEA encryption algorithm, using ECB (Electronic codebook) mode.
'	End Rem
	Function idea_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_idea_ecb()
		Return this
	End Function

'	Rem
'	bbdoc: IDEA encryption algorithm, using CFB (Cipher feedback) mode.
'	End Rem
	Function idea_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_idea_cfb()
		Return this
	End Function

'	Rem
'	bbdoc: IDEA encryption algorithm, using OFB (Output feedback) mode.
'	End Rem
	Function idea_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_idea_ofb()
		Return this
	End Function
End Rem

	Rem
	bbdoc: RC2 encryption algorithm, using CBC (Cipher-block chaining) mode.
	about: This is a variable key length cipher with an
           additional parameter called "effective key bits" Or "effective key
           length".  By default both are set to 128 bits.
	End Rem
	Function rc2_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc2_cbc()
		Return this
	End Function

	Rem
	bbdoc: RC2 encryption algorithm, using ECB (Electronic codebook) mode.
	about: This is a variable key length cipher with an
           additional parameter called "effective key bits" Or "effective key
           length".  By default both are set to 128 bits.
	End Rem
	Function rc2_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc2_ecb()
		Return this
	End Function

	Rem
	bbdoc: RC2 encryption algorithm, using CFB (Cipher feedback) mode.
	about: This is a variable key length cipher with an
           additional parameter called "effective key bits" Or "effective key
           length".  By default both are set to 128 bits.
	End Rem
	Function rc2_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc2_cfb()
		Return this
	End Function

	Rem
	bbdoc: RC2 encryption algorithm, using OFB (Output feedback) mode.
	about: This is a variable key length cipher with an
           additional parameter called "effective key bits" Or "effective key
           length".  By default both are set to 128 bits.
	End Rem
	Function rc2_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc2_ofb()
		Return this
	End Function

	Rem
	bbdoc: CAST encryption algorithm, using CBC (Cipher-block chaining) mode.
	about: This is a variable key length cipher.
	End Rem
	Function cast5_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_cast5_cbc()
		Return this
	End Function

	Rem
	bbdoc: CAST encryption algorithm, using ECB (Electronic codebook) mode.
	about: This is a variable key length cipher.
	End Rem
	Function cast5_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_cast5_ecb()
		Return this
	End Function

	Rem
	bbdoc: CAST encryption algorithm, using CFB (Cipher feedback) mode.
	about: This is a variable key length cipher.
	End Rem
	Function cast5_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_cast5_cfb()
		Return this
	End Function

	Rem
	bbdoc: CAST encryption algorithm, using OFB (Output feedback) mode.
	about: This is a variable key length cipher.
	End Rem
	Function cast5_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_cast5_ofb()
		Return this
	End Function

?MacOS
	Rem
	bbdoc: RC5 encryption algorithm, using CBC (Cipher-block chaining) mode.
	about: This is a variable key length cipher with an
           additional "number of rounds" parameter. By default the key length
           is set to 128 bits and 12 rounds.
	End Rem
	Function rc5_32_12_16_cbc:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc5_32_12_16_cbc()
		Return this
	End Function

	Rem
	bbdoc: RC5 encryption algorithm, using ECB (Electronic codebook) mode.
	about: This is a variable key length cipher with an
           additional "number of rounds" parameter. By default the key length
           is set to 128 bits and 12 rounds.
	End Rem
	Function rc5_32_12_16_ecb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc5_32_12_16_ecb()
		Return this
	End Function

	Rem
	bbdoc: RC5 encryption algorithm, using CFB (Cipher feedback) mode.
	about: This is a variable key length cipher with an
           additional "number of rounds" parameter. By default the key length
           is set to 128 bits and 12 rounds.
	End Rem
	Function rc5_32_12_16_cfb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc5_32_12_16_cfb()
		Return this
	End Function

	Rem
	bbdoc: RC5 encryption algorithm, using OFB (Output feedback) mode.
	about: This is a variable key length cipher with an
           additional "number of rounds" parameter. By default the key length
           is set to 128 bits and 12 rounds.
	End Rem
	Function rc5_32_12_16_ofb:EVP_CIPHER()
		Local this:EVP_CIPHER = New EVP_CIPHER
		this.cipherPtr = EVP_rc5_32_12_16_ofb()
		Return this
	End Function
?
End Type

Rem
bbdoc: 
End Rem
Type ENGINE

	Field enginePtr:Byte Ptr

End Type

Rem
bbdoc: A message digest context.
End Rem
Type EVP_MD_CTX

	Field ctxPtr:Byte Ptr

	Rem
	bbdoc: Creates and initializes a Message Digest Context.
	End Rem
	Function CreateContext:EVP_MD_CTX()
		Return New EVP_MD_CTX.Create()
	End Function
	
	Rem
	bbdoc: Creates and initializes a Message Digest Context.
	End Rem
	Method Create:EVP_MD_CTX()
		ctxPtr = EVP_MD_CTX_create()
		Return Self
	End Method

	Rem
	bbdoc: Sets up digest context ctx to use a digest type from ENGINE impl.
	about: If @impl is NULL then the default implementation of digest type is used.
	End Rem
	Method DigestInit:Int(digestType:EVP_MD, impl:ENGINE = Null)
		If impl Then
			Return EVP_DigestInit_ex(ctxPtr, digestType.digestPtr, impl.enginePtr)
		Else
			Return EVP_DigestInit_ex(ctxPtr, digestType.digestPtr, Null)
		End If
	End Method

	Rem
	bbdoc: Hashes @length bytes of @data into the digest context.
	about: This method can be called several times to hash additional data.
	End Rem
	Method DigestUpdate:Int(data:Byte Ptr, length:Int)
		Return EVP_DigestUpdate(ctxPtr, data, length)
	End Method

	Rem
	bbdoc: Retrieves the digest value from the context and places it in @out.
	about: The number of bytes of data written (i.e. the length of the digest) will be written to the integer @outl, at most
	EVP_MAX_MD_SIZE bytes will be written. After calling #DigestFinal no additional calls to #DigestUpdate can be made, but
	#DigestInit can be called to initialize a new digest operation.
	End Rem
	Method DigestFinal:Int(out:Byte Ptr, outl:Int Var)
		Return EVP_DigestFinal_ex(ctxPtr, out, Varptr outl)
	End Method
	
	Rem
	bbdoc: Clears all information from a cipher context and free up any allocated memory associate with it.
	returns: True for success and False for failure.
	about: It should be called
       after all operations using a cipher are complete so sensitive
       information does not remain in memory.
	End Rem
	Method Cleanup:Int()
		Return EVP_MD_CTX_cleanup(ctxPtr)
	End Method
	
	Method Delete()
		If ctxPtr Then
			EVP_MD_CTX_destroy(ctxPtr)
			ctxPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Message Digests
End Rem
Type EVP_MD

	Field digestPtr:Byte Ptr

	Rem
	bbdoc: A cryptographic hash function with a 128 bit output.
	End Rem
	Function MD2:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_md2()
		Return this
	End Function

	Rem
	bbdoc: A cryptographic hash function with a 128 bit output.
	End Rem
	Function MD5:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_md5()
		Return this
	End Function

	Rem
	bbdoc: A cryptographic hash function with a 160 bit output.
	End Rem
	Function SHA:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_sha()
		Return this
	End Function

	Rem
	bbdoc: A cryptographic hash function with a 160 bit output.
	End Rem
	Function SHA1:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_sha1()
		Return this
	End Function

	Rem
	bbdoc: An SHA hash function, but using DSS (DSA) for the signature algorithm.
	End Rem
	Function DSS:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_dss()
		Return this
	End Function

	Rem
	bbdoc: An SHA1 hash function, but using DSS (DSA) for the signature algorithm.
	End Rem
	Function DSS1:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_dss1()
		Return this
	End Function
?MacOS
	Rem
	bbdoc: A method to construct hash functions with 128 bit output from block ciphers.
	about: These functions are an implementation of MDC2 with DES.
	End Rem
	Function MDC2:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_mdc2()
		Return this
	End Function
?
	Rem
	bbdoc: A cryptographic hash function with a 160 bit output.
	End Rem
	Function RIPEMD160:EVP_MD()
		Local this:EVP_MD = New EVP_MD
		this.digestPtr = EVP_ripemd160()
		Return this
	End Function

End Type

Rem
bbdoc: Creates an MD2 hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function MD2:String(data:Object)
	Return _processDigest(EVP_MD.MD2(), data)
End Function

Rem
bbdoc: Creates an MD5 hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function MD5:String(data:Object)
	Return _processDigest(EVP_MD.MD5(), data)
End Function

Rem
bbdoc: Creates an SHA hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function SHA:String(data:Object)
	Return _processDigest(EVP_MD.SHA(), data)
End Function

Rem
bbdoc: Creates an SHA1 hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function SHA1:String(data:Object)
	Return _processDigest(EVP_MD.SHA1(), data)
End Function

Rem
bbdoc: Creates a DSS hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function DSS:String(data:Object)
	Return _processDigest(EVP_MD.DSS(), data)
End Function

Rem
bbdoc: Creates a DSS1 hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function DSS1:String(data:Object)
	Return _processDigest(EVP_MD.DSS1(), data)
End Function
?MacOS
Rem
bbdoc: Creates an MDC2 hash for @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function MDC2:String(data:Object)
	Return _processDigest(EVP_MD.MDC2(), data)
End Function
?
Rem
bbdoc: Creates an RIPEMD-160 hash on @data.
about: @data can be a String or TStream.
returns: The hashed text or Null on error.
End Rem
Function RIPEMD160:String(data:Object)
	Return _processDigest(EVP_MD.RIPEMD160(), data)
End Function

Function _processDigest:String(digestType:EVP_MD, text:Object)
	Local ctx:EVP_MD_CTX = New EVP_MD_CTX.Create()
	ctx.DigestInit(digestType)

	If String(text) Then
	
		If Not ctx.DigestUpdate(String(text), String(text).length) Then
			Return Null
		End If
		
	ElseIf TStream(text) Then
	
		Local stream:TStream = TStream(text)
	
		Local data:Byte[2048]
		Local length:Int
		Local count:Int
		
		While True
			count = stream.Read(data, 2048)

			If count = 0 Then
				Exit
			End If
			
			If Not ctx.DigestUpdate(data, count) Then
				Return Null
			End If

		Wend
		
	Else
		Return Null
	End If

	Local buf:Byte[EVP_MAX_MD_SIZE]
	Local length:Int
	If Not ctx.DigestFinal(buf, length) Then
		Return Null
	End If
	ctx.Cleanup()
	Return BytesToHex(buf, length)
End Function
