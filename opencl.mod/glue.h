/*
 Copyright (c) 2009-2010 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifdef __APPLE__
#include <OpenCL/Opencl.h>
#else
#define _MSC_VER
#include <windows.h>
#include <stdint.h>
#include <CL/cl.h>
#endif

class MaxCLKernel;
class MaxCLProgram;
class MaxCLPlatform;
class MaxCLDevice;
class MaxCLBuffer;
class MaxCLImage;

extern "C" {

#include "blitz.h"

	BBArray * _bah_opencl_TCLDevice__newDeviceList(int size);
	BBObject * _bah_opencl_TCLDevice__setDevice(BBArray * list, int index, MaxCLDevice * device, int deviceType);

	BBObject * _bah_opencl_TCLDeviceInfo__create(BBString * vendorName, BBString * deviceName, BBString * driverVersion, BBString * deviceVersion,
		int maxComputeUnits, int maxWorkItemDimensions, BBArray * maxWorkItemSizes, int maxWorkGroupSize, int maxClockFrequency,
		BBInt64 maxMemAllocSize, int imageSupport, int maxReadImageArgs, int maxWriteImageArgs, int image2dMaxWidth,
		int image2dMaxHeight, int image3dMaxWidth, int image3dMaxHeight, int image3dMaxDepth, int maxSamplers, int maxParameterSize,
		BBInt64 globalMemCacheSize, BBInt64 globalMemSize, BBInt64 maxConstantBufferSize, int maxConstantArgs, int localMemSize,
		int errorCorrectionSupport, int profilingTimerResolution, int endianLittle, BBString * profile, BBString * extensions);

	MaxCLPlatform * bmx_ocl_platform_init(BBObject * handle, MaxCLDevice * device);
	BBArray * bmx_ocl_platform_getdevices(int deviceType);
	int bmx_ocl_platform_flush(MaxCLPlatform * platform);
	int bmx_ocl_platform_finish(MaxCLPlatform * platform);

	BBObject * bmx_ocl_device_getinfo(MaxCLDevice * device);
	void bmx_ocl_device_free(MaxCLDevice * device);

	MaxCLProgram * bmx_ocl_program_create(BBObject * handle, MaxCLPlatform * platform, BBString * text);
	void bmx_ocl_program_free(MaxCLProgram * program);

	MaxCLKernel * bmx_ocl_kernel_create(BBObject * handle, BBString * name, MaxCLProgram * program);
	int bmx_ocl_kernel_setargbuffer(MaxCLKernel * kernel, int index, MaxCLBuffer * buffer);
	int bmx_ocl_kernel_execute(MaxCLKernel * kernel, int workDim, int globalWorkSize, int localWorkSize);

	MaxCLBuffer * bmx_ocl_membuff_create(BBObject * handle, MaxCLPlatform * platform, int flags, int size, void * data);
	int bmx_ocl_membuff_enqueuewrite(MaxCLBuffer * mem, int blockingWrite, int size, void * data);
	int bmx_ocl_membuff_enqueueread(MaxCLBuffer * mem, int blockingRead, int size, void * data);
	int bmx_ocl_membuff_enqueuecopy(MaxCLBuffer * mem, MaxCLBuffer * dest, int offset, int destOffset, int size);

	MaxCLImage * bmx_ocl_memimage_create2d(BBObject * handle, MaxCLPlatform * platform, int flags, int channelOrder, int imageType, int width, int height, int rowPitch, void * hostPtr);
	MaxCLImage * bmx_ocl_memimage_create3d(BBObject * handle, MaxCLPlatform * platform, int flags, int channelOrder, int imageType, int width, int height, int depth, int rowPitch, int slicePitch, void * hostPtr);
	int bmx_ocl_memimage_enqueueread(MaxCLImage * image, int blockingRead, int originX, int originY, int originZ, int regionX, int regionY, int regionZ, int rowPitch, int slicePitch, void * data);
	int bmx_ocl_memimage_enqueuewrite(MaxCLImage * image, int blockingWrite, int originX, int originY, int originZ, int regionX, int regionY, int regionZ, int rowPitch, int slicePitch, void * data);

}

class MaxCLDevice
{
public:
	MaxCLDevice(BBObject * handle, cl_device_id i);
	MaxCLDevice(cl_device_id i);
	~MaxCLDevice();
	
	void SetHandle(BBObject * handle);
	cl_device_id Id();
	BBObject * GetInfo();

private:
	cl_device_id id;
	BBObject * maxHandle;
};

class MaxCLPlatform
{
public:

	MaxCLPlatform(BBObject * handle, MaxCLDevice * device);
	~MaxCLPlatform();

	cl_context Context();
	cl_command_queue Queue();
	
private:
	cl_context context;
	cl_device_id deviceId;
	cl_command_queue queue;
	BBObject * maxHandle;
};

class MaxCLKernel
{
public:
	MaxCLKernel(BBObject * handle, MaxCLProgram * p, cl_kernel k);
	~MaxCLKernel();

	int SetArg(int index, cl_mem mem);
	int Execute(int workDim, int globalWorkSize, int localWorkSize);
	
private:
	cl_kernel	 kernel;
	MaxCLProgram * program;
	BBObject * maxHandle;
};

class MaxCLProgram
{
public:
	MaxCLProgram(BBObject * handle, MaxCLPlatform * p, BBString * source);
	~MaxCLProgram();

	cl_program Program();
	MaxCLPlatform * Platform();
	
private:
	cl_program program;
	MaxCLPlatform * platform;
	BBObject * maxHandle;
};

class MaxCLBuffer
{
public:
	MaxCLBuffer(BBObject * handle, MaxCLPlatform * p, int flags, int size, void * data);
	~MaxCLBuffer();
	
	cl_mem Mem();
	int EnqueueWrite(int blockingWrite, int size, void * data);
	int EnqueueRead(int blockingRead, int size, void * data);
	int EnqueueCopy(cl_mem dest, int offset, int destOffset, int size);
	
private:
	cl_mem buffer;
	MaxCLPlatform * platform;
	BBObject * maxHandle;
};

class MaxCLImage
{
public:
	MaxCLImage(BBObject * handle, MaxCLPlatform * p, int flags, int channelOrder, int imageType, int width, int height, int rowPitch, void * hostPtr);
	MaxCLImage(BBObject * handle, MaxCLPlatform * p, int flags, int channelOrder, int imageType, int width, int height, int depth, int rowPitch, int slicePitch, void * hostPtr);
	~MaxCLImage();
	
	cl_mem Mem();
	int EnqueueRead(int blockingRead, int originX, int originY, int originZ, int regionX, int regionY, int regionZ, int rowPitch, int slicePitch, void * data);
	int EnqueueWrite(int blockingWrite, int originX, int originY, int originZ, int regionX, int regionY, int regionZ, int rowPitch, int slicePitch, void * data);
	
private:
	cl_mem image;
	MaxCLPlatform * platform;
	BBObject * maxHandle;
};

