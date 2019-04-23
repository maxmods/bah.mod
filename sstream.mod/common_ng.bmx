SuperStrict

Extern "c"

Function fopen64_:Byte Ptr( file$,Mode$ )
?bmxng
Function fclose64_:Int( c_stream:Byte Ptr )="int fclose(FILE*)!"
Function fread64_:Size_T( buf:Byte Ptr,size:Size_T,count:Size_T,c_stream:Byte Ptr )="size_t fread(void *, size_t, size_t, FILE*)!"
Function fwrite64_:Size_T( buf:Byte Ptr,size:Size_T,count:Size_T,c_stream:Byte Ptr )="size_t fwrite(const void*, size_t, size_t, FILE*)!"
Function fflush64_:Int( c_stream:Byte Ptr )="int fflush(FILE*)!"

?win32
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="int fseeko64(FILE*, _off64_t, int)!"
Function ftell64_( c_stream:Byte Ptr, pos:Long Ptr )
?linux
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="fseeko64"
?macos
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="fseeko"
?Not win32
Function ftell64_( c_stream:Byte Ptr, pos:Long Ptr )
?

End Extern
