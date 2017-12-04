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

Import "source.bmx"


Extern

	Function strlen_:Size_T(s:Byte Ptr) = "strlen"

	Function sodium_init:Int()
	
	Function randombytes_random:UInt()
	Function randombytes_uniform:UInt(upperBound:UInt)
	Function randombytes_buf(buf:Byte Ptr, size:Size_T)
	
	Function sodium_bin2hex:Byte Ptr(buf:Byte Ptr, buflen:Size_T, Bin:Byte Ptr, binlen:Size_T)
	Function sodium_memzero(pnt:Byte Ptr, length:Size_T)
	Function sodium_mlock:Int(addr:Byte Ptr, length:Size_T)
	Function sodium_munlock:Int(addr:Byte Ptr, length:Size_T)
	Function sodium_malloc:Byte Ptr(size:Size_T)
	Function sodium_allocarray:Byte Ptr(count:Size_T, size:Size_T)
	Function sodium_free(buf:Byte Ptr)
	Function sodium_mprotect_noaccess:Int(buf:Byte Ptr)
	Function sodium_mprotect_readonly:Int(buf:Byte Ptr)
	Function sodium_mprotect_readwrite:Int(buf:Byte Ptr)
	Function sodium_pad:Int(paddedBufLen:Size_T Ptr, buf:Byte Ptr, unpaddedBufLen:Size_T, blockSize:Size_T, maxBufLen:Size_T)
	Function sodium_unpad:Int(unpaddedBufLen:Size_T Ptr, buf:Byte Ptr, paddedBufLen:Size_T, blockSize:Size_T)
	
	Function crypto_generichash_bytes:Size_T()
	Function crypto_generichash_bytes_min:Size_T()
	Function crypto_generichash_bytes_max:Size_T()
	Function crypto_generichash_keybytes:Size_T()
	Function crypto_generichash_keybytes_min:Size_T()
	Function crypto_generichash_keybytes_max:Size_T()
	Function crypto_generichash:Int(out:Byte Ptr, outlen:Size_T, in:Byte Ptr, inlen:ULong, key:Byte Ptr, keylen:Size_T)
	Function crypto_generichash_statebytes:Size_T()
	Function crypto_generichash_init:Int(state:Byte Ptr, key:Byte Ptr, keylen:Size_T, outlen:Size_T)
	Function crypto_generichash_update:Int(state:Byte Ptr, in:Byte Ptr, inlen:ULong)
	Function crypto_generichash_final:Int(state:Byte Ptr, out:Byte Ptr, outlen:Size_T)
	
	Function crypto_pwhash_saltbytes:Size_T()
	Function crypto_pwhash_memlimit_interactive:Size_T()
	Function crypto_pwhash_memlimit_moderate:Size_T()
	Function crypto_pwhash_memlimit_sensitive:Size_T()
	Function crypto_pwhash_opslimit_interactive:Size_T()
	Function crypto_pwhash_opslimit_moderate:Size_T()
	Function crypto_pwhash_opslimit_sensitive:Size_T()
	Function crypto_pwhash_alg_default:Int()
	Function crypto_pwhash:Int(out:Byte Ptr, outlen:ULong, password:Byte Ptr, passwordlen:ULong, salt:Byte Ptr, opslimit:ULong, memlimit:Size_T, alg:Int)
	Function crypto_pwhash_str:Int(out:Byte Ptr, password:Byte Ptr, passwordlen:ULong, opslimit:ULong, memlimit:Size_T)
	Function crypto_pwhash_strbytes:Size_T()
	Function crypto_pwhash_str_verify:Int(str:Byte Ptr, password:Byte Ptr, passworldlen:ULong)
	
	Function crypto_box_seedbytes:Size_T()
	
	Function crypto_secretbox_easy:Int(c:Byte Ptr, m:Byte Ptr, mlen:ULong, n:Byte Ptr, k:Byte Ptr)
	Function crypto_secretbox_open_easy:Int(m:Byte Ptr, c:Byte Ptr, clen:ULong, n:Byte Ptr, k:Byte Ptr)
	Function crypto_secretbox_keybytes:Size_T()
	Function crypto_secretbox_noncebytes:Size_T()
	Function crypto_secretbox_macbytes:Size_T()
	
	Function crypto_shorthash_bytes:Size_T()
	Function crypto_shorthash_keybytes:Size_T()
	Function crypto_shorthash:Int(out:Byte Ptr, in:Byte Ptr, inlen:ULong, k:Byte Ptr)
	Function crypto_shorthash_keygen(k:Byte Ptr)
	
	Function crypto_auth_bytes:Size_T()
	Function crypto_auth_keybytes:Size_T()
	Function crypto_auth_primitive:Byte Ptr()
	Function crypto_auth_keygen(key:Byte Ptr)
	Function crypto_auth:Int(out:Byte Ptr, in:Byte Ptr, inlen:ULong, key:Byte Ptr)
	Function crypto_auth_verify:Int(h:Byte Ptr, in:Byte Ptr, inlen:ULong, key:Byte Ptr)
	
End Extern


