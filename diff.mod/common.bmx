SuperStrict

Import BRL.Stream

Import "source.bmx"


Extern
	
	Function bmx_diff_screate:Byte Ptr(a:String, b:String)
	Function bmx_diff_sfree(handle:Byte Ptr)
	Function bmx_diff_scompose(handle:Byte Ptr)
	Function bmx_diff_scomposeUnifiedHunks(handle:Byte Ptr)	
	Function bmx_diff_sprintUnifiedFormat(handle:Byte Ptr)

	Function bmx_diff_lcreate:Byte Ptr(a:String[], b:String[])
	Function bmx_diff_lfree(handle:Byte Ptr)
	Function bmx_diff_lcompose(handle:Byte Ptr)
	Function bmx_diff_lcomposeUnifiedHunks(handle:Byte Ptr)	
	Function bmx_diff_lprintUnifiedFormat(handle:Byte Ptr)

End Extern

