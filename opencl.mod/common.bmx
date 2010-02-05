' Copyright (c) 2009-2010 Bruce A Henderson
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
Import BRL.Stream

?macos
Import "-framework OpenCL"
?Not macos
Import "include/*.h"
?

?win32
Import "-lopencl"
?

Import "*.h"
Import "glue.cpp"


Extern

	Function bmx_ocl_platform_init:Byte Ptr(handle:Object, device:Byte Ptr)
	Function bmx_ocl_platform_flush:Int(handle:Byte Ptr)
	Function bmx_ocl_platform_finish:Int(handle:Byte Ptr)

	Function bmx_ocl_device_getinfo:Object(handle:Byte Ptr)
	Function bmx_ocl_device_free(handle:Byte Ptr)

	Function bmx_ocl_program_create:Byte Ptr(handle:Object, platform:Byte Ptr, text:String)
	Function bmx_ocl_program_free(handle:Byte Ptr)

	Function bmx_ocl_kernel_create:Byte Ptr(handle:Object, name:String, program:Byte Ptr)
	Function bmx_ocl_kernel_setargbuffer:Int(handle:Byte Ptr, index:Int, buffer:Byte Ptr)
	Function bmx_ocl_kernel_execute:Int(handle:Byte Ptr, workDim:Int, globalWorkSize:Int, localWorkSize:Int)
	Function bmx_ocl_kernel_setargint:Int(handle:Byte Ptr, index:Int, value:Int)
	Function bmx_ocl_kernel_setargfloat:Int(handle:Byte Ptr, index:Int, value:Float)
	Function bmx_ocl_kernel_setarglong:Int(handle:Byte Ptr, index:Int, value:Long Ptr)

	Function bmx_ocl_membuff_create:Byte Ptr(handle:Object, platform:Byte Ptr, flags:Int, size:Int, data:Byte Ptr)
	Function bmx_ocl_membuff_enqueuewrite:Int(handle:Byte Ptr, blockingWrite:Int, size:Int, data:Byte Ptr)
	Function bmx_ocl_membuff_enqueueread:Int(handle:Byte Ptr, blockingRead:Int, size:Int, data:Byte Ptr)
	Function bmx_ocl_membuff_enqueuecopy:Int(handle:Byte Ptr, dest:Byte Ptr, offset:Int, destOffset:Int, size:Int)

	Function bmx_ocl_memimage_create2d:Byte Ptr(handle:Object, platform:Byte Ptr, flags:Int, channelOrder:Int, imageType:Int, width:Int, height:Int, rowPitch:Int, hostPtr:Byte Ptr)
	Function bmx_ocl_memimage_create3d:Byte Ptr(handle:Object, platform:Byte Ptr, flags:Int, channelOrder:Int, imageType:Int, width:Int, height:Int, depth:Int, rowPitch:Int, slicePitch:Int, hostPtr:Byte Ptr)
	Function bmx_ocl_memimage_enqueueread:Int(handle:Byte Ptr, blockingRead:Int, originX:Int, originY:Int, originZ:Int, regionX:Int, regionY:Int, regionZ:Int, rowPitch:Int, slicePitch:Int, data:Byte Ptr)
	Function bmx_ocl_memimage_enqueuewrite:Int(handle:Byte Ptr, blockingWrite:Int, originX:Int, originY:Int, originZ:Int, regionX:Int, regionY:Int, regionZ:Int, rowPitch:Int, slicePitch:Int, data:Byte Ptr)

End Extern

Rem
bbdoc: The default OpenCL device in the system.
End Rem
Const CL_DEVICE_TYPE_DEFAULT:Int = (1 Shl 0)
Rem
bbdoc: An OpenCL device that is the host processor.
about: The host processor runs the OpenCL implementations and is a single or multi-core CPU.
End Rem
Const CL_DEVICE_TYPE_CPU:Int = (1 Shl 1)
Rem
bbdoc: An OpenCL device that is a GPU.
about: By this we mean that the device can also be used to accelerate a 3D API such as OpenGL or DirectX.
End Rem
Const CL_DEVICE_TYPE_GPU:Int = (1 Shl 2)
Rem
bbdoc: Dedicated OpenCL accelerators (for example the IBM CELL Blade).
about: These devices communicate with the host processor using a peripheral interconnect such as PCIe.
End Rem
Const CL_DEVICE_TYPE_ACCELERATOR:Int = (1 Shl 3)
Rem
bbdoc: All OpenCL devices available in the system.
End Rem
Const CL_DEVICE_TYPE_ALL:Int = $FFFFFFFF

Rem
bbdoc: This flag specifies that the memory object will be read and written by a kernel.
about: This is the default.
End Rem
Const CL_MEM_READ_WRITE:Int = (1 Shl 0)
Rem
bbdoc: This flags specifies that the memory object will be written but not read by a kernel.
about: Reading from a buffer or image object created with CL_MEM_WRITE_ONLY inside a kernel is undefined.
End Rem
Const CL_MEM_WRITE_ONLY:Int = (1 Shl 1)
Rem
bbdoc: This flag specifies that the memory object is a read-only memory object when used inside a kernel.
about: Writing to a buffer or image object created with CL_MEM_READ_ONLY inside a kernel is undefined.
End Rem
Const CL_MEM_READ_ONLY:Int = (1 Shl 2)
Rem
bbdoc: This flag is valid only if hostPtr is not NULL.
about: If specified, it indicates that the application wants the OpenCL implementation to use memory referenced by hostPtr as the storage bits for the memory object.
<p>
OpenCL implementations are allowed to cache the buffer contents pointed to by hostPtr in device memory.
This cached copy can be used when kernels are executed on a device.
</p>
<p>
The result of OpenCL commands that operate on multiple buffer objects created with the same hostPtr or overlapping host regions is considered to be undefined.
</p>
End Rem
Const CL_MEM_USE_HOST_PTR:Int = (1 Shl 3)
Rem
bbdoc: This flag specifies that the application wants the OpenCL implementation to allocate memory from host accessible memory.
about: CL_MEM_ALLOC_HOST_PTR and CL_MEM_USE_HOST_PTR are mutually exclusive.
End Rem
Const CL_MEM_ALLOC_HOST_PTR:Int = (1 Shl 4)
Rem
bbdoc: This flag is valid only if hostPtr is not NULL.
about: If specified, it indicates that the application wants the OpenCL implementation to allocate memory for the memory object
and copy the data from memory referenced by hostPtr.
<p>
CL_MEM_COPY_HOST_PTR and CL_MEM_USE_HOST_PTR are mutually exclusive.

</p>
<p>
CL_MEM_COPY_HOST_PTR can be used with CL_MEM_ALLOC_HOST_PTR to initialize the contents of the TCLMem object allocated using host-accessible (e.g. PCIe) memory.
</p>
End Rem
Const CL_MEM_COPY_HOST_PTR:Int = (1 Shl 5)


Const CL_SUCCESS:Int = 0
Const CL_DEVICE_NOT_FOUND:Int = -1
Const CL_DEVICE_NOT_AVAILABLE:Int = -2
Const CL_COMPILER_NOT_AVAILABLE:Int = -3
Const CL_MEM_OBJECT_ALLOCATION_FAILURE:Int = -4
Const CL_OUT_OF_RESOURCES:Int = -5
Const CL_OUT_OF_HOST_MEMORY:Int = -6
Const CL_PROFILING_INFO_NOT_AVAILABLE:Int = -7
Const CL_MEM_COPY_OVERLAP:Int = -8
Const CL_IMAGE_FORMAT_MISMATCH:Int = -9
Const CL_IMAGE_FORMAT_NOT_SUPPORTED:Int = -10
Const CL_BUILD_PROGRAM_FAILURE:Int = -11
Const CL_MAP_FAILURE:Int = -12

Const CL_INVALID_VALUE:Int = -30
Const CL_INVALID_DEVICE_TYPE:Int = -31
Const CL_INVALID_PLATFORM:Int = -32
Const CL_INVALID_DEVICE:Int = -33
Const CL_INVALID_CONTEXT:Int = -34
Const CL_INVALID_QUEUE_PROPERTIES:Int = -35
Const CL_INVALID_COMMAND_QUEUE:Int = -36
Const CL_INVALID_HOST_PTR:Int = -37
Const CL_INVALID_MEM_OBJECT:Int = -38
Const CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:Int = -39
Const CL_INVALID_IMAGE_SIZE:Int = -40
Const CL_INVALID_SAMPLER:Int = -41
Const CL_INVALID_BINARY:Int = -42
Const CL_INVALID_BUILD_OPTIONS:Int = -43
Const CL_INVALID_PROGRAM:Int = -44
Const CL_INVALID_PROGRAM_EXECUTABLE:Int = -45
Const CL_INVALID_KERNEL_NAME:Int = -46
Const CL_INVALID_KERNEL_DEFINITION:Int = -47
Const CL_INVALID_KERNEL:Int = -48
Const CL_INVALID_ARG_INDEX:Int = -49
Const CL_INVALID_ARG_VALUE:Int = -50
Const CL_INVALID_ARG_SIZE:Int = -51
Const CL_INVALID_KERNEL_ARGS:Int = -52
Const CL_INVALID_WORK_DIMENSION:Int = -53
Const CL_INVALID_WORK_GROUP_SIZE:Int = -54
Const CL_INVALID_WORK_ITEM_SIZE:Int = -55
Const CL_INVALID_GLOBAL_OFFSET:Int = -56
Const CL_INVALID_EVENT_WAIT_LIST:Int = -57
Const CL_INVALID_EVENT:Int = -58
Const CL_INVALID_OPERATION:Int = -59
Const CL_INVALID_GL_OBJECT:Int = -60
Const CL_INVALID_BUFFER_SIZE:Int = -61
Const CL_INVALID_MIP_LEVEL:Int = -62


Const CL_R:Int = $10B0
Const CL_A:Int = $10B1
Const CL_RG:Int = $10B2
Const CL_RA:Int = $10B3
Const CL_RGB:Int = $10B4
Const CL_RGBA:Int = $10B5
Const CL_BGRA:Int = $10B6
Const CL_ARGB:Int = $10B7
Const CL_INTENSITY:Int = $10B8
Const CL_LUMINANCE:Int = $10B9

Rem
bbdoc: Each channel component is a normalized signed 8-bit integer value.
End Rem
Const CL_SNORM_INT8:Int = $10D0
Rem
bbdoc: Each channel component is a normalized signed 16-bit integer value.
End Rem
Const CL_SNORM_INT16:Int = $10D1
Rem
bbdoc: Each channel component is a normalized unsigned 8-bit integer value.
End Rem
Const CL_UNORM_INT8:Int = $10D2
Rem
bbdoc: Each channel component is a normalized unsigned 16- bit integer value.
End Rem
Const CL_UNORM_INT16:Int = $10D3
Rem
bbdoc: Represents a normalized 5-6-5 3-channel RGB image.
about: The channel order must be CL_RGB.
End Rem
Const CL_UNORM_SHORT_565:Int = $10D4
Rem
bbdoc: Represents a normalized x-5-5-5 4-channel xRGB image.
about: The channel order must be CL_RGB.
End Rem
Const CL_UNORM_SHORT_555:Int = $10D5
Rem
bbdoc: Represents a normalized x-10-10-10 4-channel xRGB image.
about: The channel order must be CL_RGB.
End Rem
Const CL_UNORM_INT_101010:Int = $10D6
Rem
bbdoc: Each channel component is an unnormalized signed 8-bit integer value.
End Rem
Const CL_SIGNED_INT8:Int = $10D7
Rem
bbdoc: Each channel component is an unnormalized signed 16-bit integer value.
End Rem
Const CL_SIGNED_INT16:Int = $10D8
Rem
bbdoc: Each channel component is an unnormalized signed 32-bit integer value.
End Rem
Const CL_SIGNED_INT32:Int = $10D9
Rem
bbdoc: Each channel component is an unnormalized unsigned 8-bit integer value.
End Rem
Const CL_UNSIGNED_INT8:Int = $10DA
Rem
bbdoc: Each channel component is an unnormalized unsigned 16-bit integer value.
End Rem
Const CL_UNSIGNED_INT16:Int = $10DB
Rem
bbdoc: Each channel component is an unnormalized unsigned 32-bit integer value.
End Rem
Const CL_UNSIGNED_INT32:Int = $10DC
Rem
bbdoc: Each channel component is a 16-bit half-float value.
End Rem
Const CL_HALF_FLOAT:Int = $10DD
Rem
bbdoc: Each channel component is a single precision floating-point value.
End Rem
Const CL_FLOAT:Int = $10DE

Rem
bbdoc: Denorms are supported.
End Rem
Const CL_FP_DENORM:Int = 1 Shl 0
Rem
bbdoc: INF and NaNs are supported.
End Rem
Const CL_FP_INF_NAN:Int = 1 Shl 1
Rem
bbdoc: Round to nearest even rounding mode supported.
End Rem
Const CL_FP_ROUND_TO_NEAREST:Int = 1 Shl 2
Rem
bbdoc: Round to zero rounding mode supported.
End Rem
Const CL_FP_ROUND_TO_ZERO:Int = 1 Shl 3
Rem
bbdoc: Round to +ve and -ve infinity rounding modes supported.
End Rem
Const CL_FP_ROUND_TO_INF:Int = 1 Shl 4
Rem
bbdoc: IEEE754-2008 fused multiply-add is supported.
End Rem
Const CL_FP_FMA:Int = 1 Shl 5


