SuperStrict

Framework BaH.OpenCL
Import BRL.StandardIO

' initialise a platform for a device type
Local platform:TCLPlatform = TCLPlatform.InitDevice(CL_DEVICE_TYPE_ALL)

' create,load and compile a program
Local program:TCLProgram = platform.LoadProgram(LoadString("example_02.cl"))

' create some kernels
Local kernelSquare:TCLKernel = program.LoadKernel("square")
Local kernelInverse:TCLKernel = program.LoadKernel("inverse")

' our data arrays
Const SIZE:Int = 1024 * 1024
Local data:Float[][] = [New Float[SIZE], New Float[SIZE]]
Local testBuf:Float[SIZE]

' initialize Input data
For Local i:Int = 0 Until SIZE
	data[0][i] = i + 1
Next

Local buf:TCLBuffer[2]

' create an input buffer, copying the data into the buffer
buf[0] = platform.CreateBuffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, SIZE * 4, data[0])
' copy data to buffer
'buf[0].EnqueueWrite(SIZE * 4, data[0])

' create an output buffer
buf[1] = platform.CreateBuffer(CL_MEM_READ_WRITE, SIZE * 4)


kernelSquare.SetArg(0, buf[0])
kernelSquare.SetArg(1, buf[1])
	
kernelInverse.SetArg(0, buf[1])
kernelInverse.SetArg(1, buf[0])


Local startTime:Int = MilliSecs()

For Local i:Int = 0 Until 200
	kernelSquare.Execute(1, SIZE)
	'kernelInverse.Execute(1, SIZE)
Next

' copy data from buffer - this waits for the queues to complete.
buf[1].Read(SIZE * 4, data[1])

Local endTime:Int = MilliSecs()
Print "Time elapsed for OpenCL run : " + (endTime - startTime)



startTime = MilliSecs()

Local value:Float
For Local i:Int = 0 Until 200
	For Local n:Int = 0 Until SIZE
		value = data[0][n]
		testBuf[n] = value * value
	Next
Next

endTime = MilliSecs()
Print "Time elapsed for BlitzMax run : " + (endTime - startTime)


For Local i:Int = 0 Until 10
	Print data[1][i] + " : " + testBuf[i]
Next
