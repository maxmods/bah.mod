SuperStrict

Import "stdprocessor.bmx"
Import "features.bmx"
Import "state.bmx"


Type TBlitzMaxDebugManager

	Field processor:TBlitzMaxStdDebugProcessor
	Field ideProcessor:TBlitzMaxIDEDebugProcessor
	
	Field finish:Int = False
	Field needCommand:Int = False
	Field transactionId:String
	
	Field state:Int = DebugState.STARTING
	
	Field features:TBlitzMaxFeatures


End Type
