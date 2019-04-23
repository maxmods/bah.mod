SuperStrict

Extern "c"

Function fopen64_:Byte Ptr( file$,Mode$ )

Function fclose64_:Int( c_stream:Byte Ptr )="fclose"
Function fread64_:Int( buf:Byte Ptr,size:Int,count:Int,c_stream:Byte Ptr )="fread"
Function fwrite64_:Int( buf:Byte Ptr,size:Int,count:Int,c_stream:Byte Ptr )="fwrite"
Function fflush64_:Int( c_stream:Byte Ptr )="fflush"

?win32
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="fseeko64"
Function ftell64_( c_stream:Byte Ptr, pos:Long Ptr )
?linux
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="fseeko64"
?macos
Function fseek64_:Int( c_stream:Byte Ptr,offset:Long,origin:Int )="fseeko"
?Not win32
Function ftell64_( c_stream:Byte Ptr, pos:Long Ptr )
?
Function fputs64_:Int( str$,c_stream:Byte Ptr )

Function stat_:Int( path$,st_mode:Int Var,st_size:Int Var,st_mtime:Int Var,st_ctime:Int Var )

End Extern
