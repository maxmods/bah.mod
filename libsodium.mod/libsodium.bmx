' Copyright (c) 2016-2017 Bruce A Henderson
' 
' Permission to use, copy, modify, and/or distribute this software for any purpose
' with or without fee is hereby granted, provided that the above copyright notice
' and this permission notice appear in all copies.
' 
' THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
' REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
' FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
' INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
' OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
' TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
' THIS SOFTWARE.
'
SuperStrict

Rem
bbdoc: Sodium Crypto Library
End Rem
Module bah.libsodium

ModuleInfo "Version: 1.01"
ModuleInfo "License: ISC"
ModuleInfo "Copyright: libsodium - 2013-2017 Frank Denis"
ModuleInfo "Copyright: Wrapper - 2016-2017 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to libsodium 1.0.15.46e2a46"
ModuleInfo "History: 1.00 Initial Release"

?bmxng


Import "common.bmx"


If sodium_init() = -1 Then
	Throw "Sodium initialisation error."
End If


Type TUtil

	Rem
	bbdoc: 
	End Rem
	Function Bin2Hex:String(Bin:Byte Ptr, binlen:Int)
		Local buf:Byte[binlen * 2 + 1]
		
		sodium_bin2hex(buf, Size_T(buf.length), Bin, Size_T(binlen))
		
		Return String.FromCString(buf)
	End Function

	Rem
	bbdoc: Adds padding data to a buffer buf whose original size is @unpaddedBufLen in order to extend its total length to a multiple of @blocksize.
	returns: -1 if the padded buffer length would exceed @maxBufLen, or if the block size is 0. It returns 0 on success.
	about: The new length is put into @paddedBufLen.
	Uses the ISO/IEC 7816-4 padding algorithm. It supports arbitrary block sizes, ensures that the
	padding data are checked for computing the unpadded length, and is more resistant to some classes
	of attacks than other standard padding algorithms.
	Padding should be applied prior to encryption, and removed after decryption.
	Usage of padding in order to hide the length of a password is not recommended. A client willing
	to send a password to a server should hash it instead (even with a single iteration of the hash
	function).
	This ensures that the length of the transmitted data is constant, and that the server doesn't
	effortlessly get a copy of the password.
	End Rem
	Function Pad:Int(paddedBufLen:Size_T Var, buf:Byte Ptr, unpaddedBufLen:Size_T, blockSize:Size_T, maxBufLen:Size_T)
		Return sodium_pad(Varptr paddedBufLen, buf, unpaddedBufLen, blockSize, maxBufLen)
	End Function
	
	Rem
	bbdoc: Computes the original, unpadded length of a message previously padded using Pad().
	about: The original length is put into @unpaddedBufLen.
	End Rem
	Function UnPad:Int(unpaddedBufLen:Size_T Var, buf:Byte Ptr, paddedBufLen:Size_T, blockSize:Size_T)
		Return sodium_unpad(Varptr unpaddedBufLen, buf, paddedBufLen, blockSize)
	End Function
	
End Type

Rem
bbdoc: Securing memory allocations.
End Rem
Type TMemory

	Rem
	bbdoc: This function tries to effectively zero @length bytes starting at @pnt, even if optimizations are being applied to the code.
	about: After use, sensitive data should be overwritten, but memset() and hand-written code can be silently
	stripped out by an optimizing compiler or by the linker.+
	End Rem
	Function MemZero(pnt:Byte Ptr, length:Size_T)
		sodium_memzero(pnt, length)
	End Function
	
	Rem
	bbdoc: Locks at least @len bytes of memory starting at @addr.
	about: This can help avoid swapping sensitive data to disk.
	In addition, it is recommended to totally disable swap partitions on machines processing senstive
	data, or, as a second choice, use encrypted swap partitions.
	For similar reasons, on Unix systems, one should also disable core dumps when running
	crypto code outside a development environment. This can be achieved using a shell built-in such 
	as ulimit or programatically using setrlimit(RLIMIT_CORE, &(struct rlimit) {0, 0}). On 
	operating systems where this feature is implemented, kernel crash dumps should also be disabled.
	Note: Many systems place limits on the amount of memory that may be locked by a process. Care
	should be taken to raise those limits (e.g. Unix ulimits) where neccessary. sodium_lock()
	will return -1 when any limit is reached.
	End Rem
	Function MLock:Int(addr:Byte Ptr, length:Size_T)
		Return sodium_mlock(addr, length)
	End Function
	
	Rem
	bbdoc: Should be called after locked memory is not being used any more. 
	about: It will zero len bytes starting at addr before actually flagging the pages as swappable
	again. Calling Memzero() prior to MUnlock() is thus not required.
	On systems where it is supported, MLock() also wraps madvise() and advises the kernel not to
	include the locked memory in core dumps. sodium_unlock() also undoes this additional protection.
	End Rem
	Function MUnlock:Int(addr:Byte Ptr, length:Size_T)
		Return sodium_munlock(addr, length)
	End Function
	
	Rem
	bbdoc: Returns a pointer from which exactly size contiguous bytes of memory can be accessed.
	about: The allocated region is placed at the end of a page boundary, immediately followed by a
	guard page. As a result, accessing memory past the end of the region will immediately terminate
	the application.
	A canary is also placed right before the returned pointer. Modifications of this canary are
	detected when trying to free the allocated region with #Free(), and also cause the application to
	immediately terminate.
	An additional guard page is placed before this canary to make it less likely for sensitive data
	to be accessible when reading past the end of an unrelated region.
	The allocated region is filled with 0xdb bytes in order to help catch bugs due to uninitialized data.
	In addition, Mlock() is called on the region to help avoid it being swapped to disk. On
	operating systems supporting MAP_NOCORE or MADV_DONTDUMP, memory allocated this way will also not
	be part of core dumps.
	The returned address will not be aligned if the allocation size is not a multiple of the required
	alignment.
	For this reason, sodium_malloc() should not be used with packed or variable-length structures,
	unless the size given to sodium_malloc() is rounded up in order to ensure proper alignment.
	All the structures used by libsodium can safely be allocated using Malloc().
	Allocating 0 bytes is a valid operation. It returns a pointer that can be successfully passed
	to Free().
	End Rem
	Function Malloc:Byte Ptr(size:Size_T)
		Return sodium_malloc(size)
	End Function
	
	Rem
	bbdoc: Returns a pointer from which count objects that are size bytes of memory each can be accessed.
	about: It provides the same guarantees as #Malloc() but also protects against arithmetic
	overflows when @count * @size exceeds SIZE_MAX.
	End Rem
	Function AllocArray:Byte Ptr(count:Size_T, size:Size_T)
		Return sodium_allocarray(count, size)
	End Function
	
	Rem
	bbdoc: Unlocks and deallocates memory allocated using Malloc() or AllocArray().
	about: Prior to this, the canary is checked in order to detect possible buffer underflows
	and terminate the process if required.
	#Free also fills the memory region with zeros before the deallocation.
	This function can be called even if the region was previously protected using ProtectReadonly; the protection will automatically be changed as needed.
	@mem can be NULL, in which case no operation is performed.
	End Rem
	Function Free(mem:Byte Ptr)
		sodium_free(mem)
	End Function
	
	Rem
	bbdoc: Makes a region allocated using Malloc() or AllocArray() inaccessible.
	about: It cannot be read or written, but the data are preserved.
	This function can be used to make confidential data inaccessible except when actually needed for a specific operation.
	End Rem
	Function ProtectNoAccess:Int(mem:Byte Ptr)
		Return sodium_mprotect_noaccess(mem)
	End Function
	
	Rem
	bbdoc: Marks a region allocated using Malloc() or AllocArray() as read-only.
	about: Attempting to modify the data will cause the process to terminate.
	End Rem
	Function ProtectReadOnly:Int(mem:Byte Ptr)
		Return sodium_mprotect_readonly(mem)
	End Function
	
	Rem
	bbdoc: Marks a region allocated using Malloc() or AllocArray() as readable and writable, after having been protected using ProtectReadonly() or ProtectNoAccess()
	End Rem
	Function ProtectReadWrite:Int(mem:Byte Ptr)
		Return sodium_mprotect_readwrite(mem)
	End Function
	
End Type



Rem
bbdoc: Random data generation.
about: A set of functions to generate unpredictable data, suitable for creating secret keys.
End Rem
Type TRandomBytes

	Rem
	bbdoc: Returns an unpredictable value between 0 and $ffffffff
	End Rem
	Function Random:UInt()
		Return randombytes_random()
	End Function
	
	Rem
	bbdoc: Returns an unpreditable value between 0 upperBound (excluded).
	about: 
	End Rem
	Function Uniform:UInt(upperBound:UInt)
		Return randombytes_uniform(upperBound)
	End Function
	
	Rem
	bbdoc: Fills the Byte array @buf with an unpredictable sequence of bytes.
	End Rem
	Function Buffer(buf:Byte[])
		randombytes_buf(buf, Size_T(buf.length))
	End Function
	
	Rem
	bbdoc: Fills @size bytes starting at @buf with an unpredictable sequence of bytes.
	End Rem
	Function Buffer(buf:Byte Ptr, size:Int)
		randombytes_buf(buf, Size_T(size))
	End Function
	

End Type


Rem
bbdoc: Computes a fixed-length fingerprint for an arbitrary long message.
about: Sample use cases :
File integrity checking.
Creating unique identifiers to index aribtrary long data.
NOTE : This algorithm is not suitable for hashing passwords.
End Rem
Type TGenericHash
	
	Rem
	bbdoc: Returns the minimum recommended output size.
	about: This size makes it practically impossible for two messages to produce the same fingerprint.
	End Rem
	Function Bytes:Size_T()
		Return crypto_generichash_bytes()
	EndFunction
	
	Rem
	bbdoc: 
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_generichash_keybytes()
	End Function
	
	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:Byte Ptr, inlen:Int, key:Byte Ptr = Null, keylen:Int = 0)
		If key Then
			Return crypto_generichash(out, Size_T(outlen), in, ULong(inlen), key, Size_T(keylen))
		Else
			Return crypto_generichash(out, Size_T(outlen), in, ULong(inlen), Null, 0)
		End If
	End Function

	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:String, key:String = Null)
		Local res:Int
	
		Local i:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(i)
	
		If key Then
			Local k:Byte Ptr = key.ToUTF8String()
			Local keylen:Int = strlen_(k)
			
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), k, Size_T(keylen))
			
			MemFree(k)
		Else
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), Null, 0)
		End If
		
		MemFree(i)
		
		Return res
	End Function
	
	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:String, key:Byte Ptr, keylen:Int)
		Local res:Int
	
		Local i:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(i)
	
		If key Then
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), key, Size_T(keylen))
		Else
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), Null, 0)
		End If
		
		MemFree(i)
		
		Return res
	End Function
	
End Type

Rem
bbdoc: Computes a fixed-length fingerprint for an arbitrary long message.
about: This multipart streaming API is especially useful to process very large files and datastreams.
NOTE : This algorithm is not suitable for hashing passwords.
End Rem
Type TGenericMultipartHash

	Field statePtr:Byte Ptr
	
	Rem
	bbdoc: Initializes a hash with @key (may be Null) in order to eventually produce @outlen bytes of output.
	End Rem
	Method New(key:String, outlen:Int)
		statePtr = MemAlloc(crypto_generichash_statebytes())
		
		If key Then
			Local k:Byte Ptr = key.ToUTF8String()
			Local keylen:Int = strlen_(k)
		
			crypto_generichash_init(statePtr, k, Size_T(keylen), Size_T(outlen))
			
			MemFree(k)
		Else
			crypto_generichash_init(statePtr, Null, 0, Size_T(outlen))
		End If
	End Method

	Rem
	bbdoc: Initializes a hash with @key (may be Null), of size @keylen in order to eventually produce @outlen bytes of output.
	End Rem
	Method New(key:Byte Ptr, keylen:Int, outlen:Int)
		statePtr = MemAlloc(crypto_generichash_statebytes())
		
		If key And keylen > 0 Then
			crypto_generichash_init(statePtr, key, Size_T(keylen), Size_T(outlen))
		Else
			crypto_generichash_init(statePtr, Null, 0, Size_T(outlen))
		End If
	End Method
	
	Rem
	bbdoc: Processes a chunk of @in bytes of the complete message, @inlen in length.
	End Rem
	Method HashUpdate:Int(in:Byte Ptr, inlen:Int)
		Return crypto_generichash_update(statePtr, in, ULong(inlen))
	End Method

	Rem
	bbdoc: Processes a chunk @in of the complete message.
	End Rem
	Method HashUpdate:Int(in:String)
		Local k:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(k)

		Local res:Int = crypto_generichash_update(statePtr, k, ULong(inlen))
		
		MemFree(k)
		Return res
	End Method
	
	Rem
	bbdoc: Finalises the hash into @out.
	about: Once this has been called, this hash should no longer be used.
	End Rem
	Method HashFinal:Int(out:Byte Ptr, outlen:Int)
		Return crypto_generichash_final(statePtr, out, Size_T(outlen))
	End Method
	
	Method Delete()
		If statePtr Then
			MemFree statePtr
			statePtr = Null
		End If
	End Method

End Type


Rem
bbdoc: 
End Rem
Type TPWHash

	Rem
	bbdoc: 
	End Rem
	Function SaltBytes:Size_T()
		Return crypto_pwhash_saltbytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function AlgDefault:Int()
		Return crypto_pwhash_alg_default()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function StrBytes:Int()
		Return crypto_pwhash_strbytes()
	End Function

	Rem
	bbdoc: Derives an @outlen bytes long key from a password with a salt whose fixed length is #SaltBytes() bytes.
	about: @outlen should be at least 16 (128 bits).
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, password:Byte Ptr, passwordlen:ULong, salt:Byte Ptr, opslimit:ULong, memlimit:Size_T, alg:Int)
		Return crypto_pwhash(out, ULong(outlen), password, passwordlen, salt, ULong(opslimit), Size_T(memlimit), alg)
	End Function

	Rem
	bbdoc: Derives an @out.length bytes long key from a password with a salt whose fixed length is #SaltBytes() bytes.
	about: @out should be at least 16 (128 bits).
	End Rem
	Function Hash:Int(out:Byte[], password:String, salt:Byte Ptr, opslimit:ULong, memlimit:Size_T, alg:Int)
		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)
		
		Local res:Int = crypto_pwhash(out, ULong(out.length), p, ULong(plen), salt, ULong(opslimit), Size_T(memlimit), alg)
		
		MemFree(p)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStr:Int(out:Byte Ptr, password:Byte Ptr, passwordlen:ULong, opslimit:ULong, memlimit:Size_T)
		Return crypto_pwhash_str(out, password, passwordlen, ULong(opslimit), Size_T(memlimit))
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStr:Int(out:Byte Ptr, password:String, opslimit:ULong, memlimit:Size_T)
		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)

		Local res:Int = crypto_pwhash_str(out, p, ULong(plen), ULong(opslimit), Size_T(memlimit))
		
		MemFree(p)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStrVerify:Int(str:String, password:String)
		Local s:Byte Ptr = str.ToCString()

		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)
		
		Local res:Int = crypto_pwhash_str_verify(str, password, ULong(plen))
		
		MemFree(p)
		MemFree(s)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStrVerify:Int(str:Byte Ptr, password:Byte Ptr, passwordlen:ULong)
		Return crypto_pwhash_str_verify(str, password, passwordlen)
	End Function

	Rem
	bbdoc: Memory limit suitable for interactive, online operations.
	about: Requires 32MB of dedicated RAM.
	End Rem
	Function MemLimitInteractive:Size_T()
		Return crypto_pwhash_memlimit_interactive()
	End Function

	Rem
	bbdoc: Moderate memory limit.
	about: Requires 128MB of dedicated RAM.
	End Rem
	Function MemLimitModerate:Size_T()
		Return crypto_pwhash_memlimit_moderate()
	End Function

	Rem
	bbdoc: Memory limit suitable for sensitive data and non-interactive operations.
	about: Requires 512MB of dedicated RAM.
	End Rem
	Function MemLimitSensitive:Size_T()
		Return crypto_pwhash_memlimit_sensitive()
	End Function

	Rem
	bbdoc: Number of computations suitable for interactive, online operations.
	End Rem
	Function OpsLimitInteractive:Size_T()
		Return crypto_pwhash_opslimit_interactive()
	End Function

	Rem
	bbdoc: A moderate number of computations.
	about: Higher values requires more CPU cycles to compute a key.
	End Rem
	Function OpsLimitModerate:Size_T()
		Return crypto_pwhash_opslimit_moderate()
	End Function

	Rem
	bbdoc: Number of computations suitable for sensitive data and non-interactive operations.
	about: Higher values requires more CPU cycles to compute a key.
	End Rem
	Function OpsLimitSensitive:Size_T()
		Return crypto_pwhash_opslimit_sensitive()
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TShortHash

	Rem
	bbdoc: 
	End Rem
	Function Bytes:Size_T()
		Return crypto_shorthash_bytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_shorthash_keybytes()
	End Function

	Rem
	bbdoc: Populates key @key for use with #Hash.
	about: Returns True on succes, or False if @key was not large enough.
	End Rem
	Function KeygenKey:Int(key:Byte[])
		If Not key Or key.length < KeyBytes() Then
			Return False
		Else
			crypto_shorthash_keygen(key)
			Return True
		End If
	End Function
	
	Rem
	bbdoc: Generates a key for use with #Hash.
	End Rem
	Function KeyGen:Byte[]()
		Local key:Byte[] = New Byte[KeyBytes()]
		crypto_shorthash_keygen(key)
		Return key
	End Function
	
	Rem
	bbdoc: 
	about: The key has to remain secret. This function will not provide any mitigations against DoS attacks if the key is known from attackers
	End Rem
	Function Hash:Int()
	
		' crypto_shorthash:Int(out:Byte Ptr, in:Byte Ptr, inlen:ULong, k:Byte Ptr)

	End Function

End Type

Rem
bbdoc: Encrypts a message with a key and a nonce to keep it confidential, and computes an authentication tag.
about: This tag is used to make sure that the message hasn't been tampered with before decrypting it.
A single key is used both to encrypt/sign and verify/decrypt messages. For this reason, it is critical to keep
the key confidential.
The nonce doesn't have to be confidential, but it should never ever be reused with the same key.
End Rem
Type TSecretBox

	Rem
	bbdoc: 
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:String, nonce:String, key:String)
		Local m:Byte Ptr = message.ToUTF8String()
		Local mlen:Int = strlen_(m)
		Local n:Byte Ptr = nonce.ToCString()
		Local k:Byte Ptr = key.ToCString()
		
		Local res:Int = crypto_secretbox_easy(ciphertext, m, ULong(mlen), n, k)
		
		MemFree(k)
		MemFree(n)
		MemFree(m)
		Return res
	End Function

	Rem
	bbdoc: Encrypts a @message with a @nonce and @key.
	about: @key should be KeyBytes() long
	@nonce should be NonceBytes() long.
	@ciphertext should be at least MacBytes() + message length long.
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:String, nonce:Byte[], key:Byte[])
		Local m:Byte Ptr = message.ToUTF8String()
		Local mlen:Int = strlen_(m)
		
		Local res:Int = crypto_secretbox_easy(ciphertext, m, ULong(mlen), nonce, key)
		
		MemFree(m)
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:Byte Ptr, mlen:ULong, nonce:Byte[], key:Byte[])
		Return crypto_secretbox_easy(ciphertext, message, mlen, nonce, key)
	End Function
	
	Rem
	bbdoc: 
	returns: -1 if the verification fails, and 0 on success.
	End Rem
	Function OpenEasy:Int(decrypted:Byte[], ciphertext:Byte[], nonce:Byte[], key:Byte[])
		Return crypto_secretbox_open_easy(decrypted, ciphertext, ULong(ciphertext.length), nonce, key)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function OpenEasy:Int(decrypted:Byte[], ciphertext:Byte[], nonce:String, key:String)
		Local n:Byte Ptr = nonce.ToCString()
		Local k:Byte Ptr = key.ToCString()

		Local res:Int = crypto_secretbox_open_easy(decrypted, ciphertext, ULong(ciphertext.length), n, k)
		
		MemFree(k)
		MemFree(n)
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_secretbox_keybytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function NonceBytes:Size_T()
		Return crypto_secretbox_noncebytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function MacBytes:Size_T()
		Return crypto_secretbox_macbytes()
	End Function
	
End Type

Rem
bbdoc: Computes an authentication tag for a message and a secret key
about: Provides a way to verify that a given tag is valid for a given message and key.
Does not encrypt the message. It only computes and verifies an authentication tag.

Computing the tag is deterministic: the same (message, key) tuple will always produce the same output.
However, even if the message is public, knowing the key is required in order to be able to compute a
valid tag. Therefore, the key should remain confidential. The tag, however, can be public.
A typical use case is:
A prepares a message, add an authentication tag, sends it to B
A doesn't store the message
Later on, B sends the message and the authentication tag to A
A uses the authentication tag to verify that it created this message.
End Rem
Type TAuth

	Rem
	bbdoc: Computes a tag for the message @in, whose length is @inlen bytes, and the key @key.
	about: @key should be #KeyBytes bytes. The function puts the tag into @out.
	The tag is #Bytes() bytes long.
	End Rem
	Function Auth:Int(out:Byte Ptr, in:Byte Ptr, inlen:ULong, key:Byte Ptr)
		Return crypto_auth(out, in, inlen, key)
	End Function

	Rem
	bbdoc: Computes a tag for the message @in, and the key @key.
	about: @key should be #KeyBytes bytes. The function puts the tag into @out.
	The tag is #Bytes() bytes long.
	End Rem
	Function Auth:Int(out:Byte Ptr, in:Byte[], key:Byte Ptr)
		Return crypto_auth(out, in, ULong(in.Length), key)
	End Function

	Rem
	bbdoc: Computes a tag for the message @in, and the key @key.
	about: @key should be #KeyBytes bytes. The function puts the tag into @out.
	The tag is #Bytes() bytes long.
	End Rem
	Function Auth:Int(out:Byte Ptr, in:Byte[], key:Byte[])
		If key.Length < KeyBytes() Then
			Return -1
		End If
		Return crypto_auth(out, in, ULong(in.Length), key)
	End Function

	Rem
	bbdoc: Computes a tag for the message @in, and the key @key.
	about: @key should be #KeyBytes bytes. The function puts the tag into @out.
	The tag is #Bytes() bytes long.
	End Rem
	Function Auth:Int(out:Byte[], in:Byte[], key:Byte[])
		If out.length < Bytes() Then
			Return -1
		End If
		If key.length < KeyBytes() Then
			Return -1
		End If
		Return crypto_auth(out, in, ULong(in.length), key)
	End Function
	
	Rem
	bbdoc: Verifies that the tag stored at @h is a valid tag for the message @in whose length is @inlen bytes, and the key @key.
	returns: -1 if the verification fails, and 0 if it passes.
	End Rem
	Function Verify:Int(h:Byte Ptr, in:Byte Ptr, inlen:ULong, key:Byte Ptr)
		Return crypto_auth_verify(h, in, inlen, key)
	End Function

	Rem
	bbdoc: Verifies that the tag stored at @h is a valid tag for the message @in whose length is @inlen bytes, and the key @key.
	returns: -1 if the verification fails, and 0 if it passes.
	End Rem
	Function Verify:Int(h:Byte Ptr, in:Byte[], key:Byte Ptr)
		Return crypto_auth_verify(h, in, ULong(in.length), key)
	End Function

	Rem
	bbdoc: Verifies that the tag stored at @h is a valid tag for the message @in whose length is @inlen bytes, and the key @key.
	returns: -1 if the verification fails, and 0 if it passes.
	End Rem
	Function Verify:Int(h:Byte Ptr, in:Byte[], key:Byte[])
		If key.length < KeyBytes() Then
			Return -1
		End If
		Return crypto_auth_verify(h, in, ULong(in.length), key)
	End Function

	Rem
	bbdoc: Verifies that the tag stored at @h is a valid tag for the message @in whose length is @inlen bytes, and the key @key.
	returns: -1 if the verification fails, and 0 if it passes.
	End Rem
	Function Verify:Int(h:Byte[], in:Byte[], key:Byte[])
		If h.length < Bytes() Then
			Return -1
		End If
		If key.length < KeyBytes() Then
			Return -1
		End If
		Return crypto_auth_verify(h, in, ULong(in.length), key)
	End Function
	
	Rem
	bbdoc: This helper function creates a random key @key.
	about: It is equivalent to calling TRandomBytes.Buffer() but improves code clarity and can prevent
	misuse by ensuring that the provided key length is always be correct.
	End Rem
	Function KeyGen:Int(key:Byte[])
		If key.length < KeyBytes() Then
			Return -1
		End If	
		crypto_auth_keygen(key)
	End Function
	
	Rem
	bbdoc: The size in bytes of the authentication tag.
	End Rem
	Function Bytes:Size_T()
		Return crypto_auth_bytes()
	End Function
	
	Rem
	bbdoc: The size in bytes of the key.
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_auth_keybytes()
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TBox

	Function SeedBytes:Size_T()
		Return crypto_box_seedbytes()
	End Function

End Type

?
