/*
 Copyright (c) 2009 Bruce A Henderson
 
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

#include "glue.h"

// --------------------------------------------------------

MaxCLDevice::MaxCLDevice(BBObject * handle, cl_device_id i)
	: maxHandle(handle), id(i)
{
}

MaxCLDevice::MaxCLDevice(cl_device_id i)
	: id(i)
{
}

MaxCLDevice::~MaxCLDevice()
{
}

cl_device_id MaxCLDevice::Id() {
	return id;
}

void MaxCLDevice::SetHandle(BBObject * handle) {
	maxHandle = handle;
}

BBObject * MaxCLDevice::GetInfo() {
	cl_char vendorName[1024];
	cl_char deviceName[1024];
	cl_char driverVersion[1024];
	cl_char deviceVersion[1024];
	cl_uint maxComputeUnits;
	cl_uint maxWorkItemDimensions;
	size_t maxWorkItemSizes[32];
	size_t maxWorkGroupSize;
	cl_uint maxClockFrequency;
	cl_ulong maxMemAllocSize;
	cl_bool imageSupport;
	cl_uint maxReadImageArgs;
	cl_uint maxWriteImageArgs;
	size_t image2dMaxWidth;
	size_t image2dMaxHeight;
	size_t image3dMaxWidth;
	size_t image3dMaxHeight;
	size_t image3dMaxDepth;
	cl_uint maxSamplers;
	size_t maxParameterSize;
	cl_ulong globalMemCacheSize;
	cl_ulong globalMemSize;
	cl_ulong maxConstantBufferSize;
	cl_uint maxConstantArgs;
	cl_ulong localMemSize;
	cl_bool errorCorrectionSupport;
	size_t profilingTimerResolution;
	cl_bool endianLittle;
	cl_char profile[1024];
	cl_char extensions[1024];
	
	size_t	size;
	cl_int err = clGetDeviceInfo(id, CL_DEVICE_VENDOR, sizeof(vendorName), vendorName, &size);

	err = clGetDeviceInfo(id, CL_DEVICE_NAME, sizeof(deviceName), deviceName, &size);
	err = clGetDeviceInfo(id, CL_DRIVER_VERSION, sizeof(driverVersion), driverVersion, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_VERSION, sizeof(deviceVersion), deviceVersion, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxWorkItemDimensions), &maxWorkItemDimensions, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSizes), &maxWorkItemSizes, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(maxClockFrequency), &maxClockFrequency, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxMemAllocSize), &maxMemAllocSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE_SUPPORT, sizeof(imageSupport), &imageSupport, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(maxReadImageArgs), &maxReadImageArgs, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(maxWriteImageArgs), &maxWriteImageArgs, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(image2dMaxWidth), &image2dMaxWidth, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(image2dMaxHeight), &image2dMaxHeight, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(image3dMaxWidth), &image3dMaxWidth, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(image3dMaxHeight), &image3dMaxHeight, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(image3dMaxDepth), &image3dMaxDepth, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_SAMPLERS, sizeof(maxSamplers), &maxSamplers, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(maxParameterSize), &maxParameterSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(globalMemCacheSize), &globalMemCacheSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMemSize), &globalMemSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(maxConstantBufferSize), &maxConstantBufferSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(maxConstantArgs), &maxConstantArgs, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMemSize), &localMemSize, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(errorCorrectionSupport), &errorCorrectionSupport, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(profilingTimerResolution), &profilingTimerResolution, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_ENDIAN_LITTLE, sizeof(endianLittle), &endianLittle, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_PROFILE, sizeof(profile), profile, &size);
	err = clGetDeviceInfo(id, CL_DEVICE_EXTENSIONS, sizeof(extensions), extensions, &size);
	
	BBArray * sizes = bbArrayNew1D("i", maxWorkItemDimensions);
	int *s = (int*)BBARRAYDATA( sizes, sizes->dims );
	for (int i = 0; i < maxWorkItemDimensions; i++) {
		s[i] = static_cast<int>(maxWorkItemSizes[i]);
	}
	
	BBObject * device = _bah_opencl_TCLDeviceInfo__create(bbStringFromCString((char*)vendorName), bbStringFromCString((char*)deviceName), 
		bbStringFromCString((char*)driverVersion), bbStringFromCString((char*)deviceVersion), static_cast<int>(maxComputeUnits), static_cast<int>(maxWorkItemDimensions),
		sizes, static_cast<int>(maxWorkGroupSize), static_cast<int>(maxClockFrequency), static_cast<BBInt64>(maxMemAllocSize),
		static_cast<int>(imageSupport), static_cast<int>(maxReadImageArgs), static_cast<int>(maxWriteImageArgs), static_cast<int>(image2dMaxWidth),
		static_cast<int>(image2dMaxHeight), static_cast<int>(image3dMaxWidth), static_cast<int>(image3dMaxHeight), static_cast<int>(image3dMaxDepth),
		static_cast<int>(maxSamplers), static_cast<int>(maxParameterSize), static_cast<BBInt64>(globalMemCacheSize), static_cast<BBInt64>(globalMemSize),
		static_cast<BBInt64>(maxConstantBufferSize), static_cast<int>(maxConstantArgs), static_cast<int>(localMemSize), static_cast<int>(errorCorrectionSupport),
		static_cast<int>(profilingTimerResolution), static_cast<int>(endianLittle), bbStringFromCString((char*)profile), bbStringFromCString((char*)extensions));

}

// --------------------------------------------------------

MaxCLPlatform::MaxCLPlatform(BBObject * handle, MaxCLDevice * device)
	: maxHandle(handle), deviceId(device->Id())
{
	cl_int err;
	
	context = clCreateContext(0, 1, &deviceId, NULL, NULL, &err);
	if (context == NULL) {
		// THROW
	}
	
	queue = clCreateCommandQueue(context, deviceId, 0, NULL);
	if (queue == NULL) {
		// THROW
	}
	
}

MaxCLPlatform::~MaxCLPlatform()
{
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}

cl_context MaxCLPlatform::Context() {
	return context;
}

cl_command_queue MaxCLPlatform::Queue() {
	return queue;
}

// --------------------------------------------------------

MaxCLKernel::MaxCLKernel(BBObject * handle, MaxCLProgram * p, cl_kernel k)
	: maxHandle(handle), kernel(k), program(p)
{
}

MaxCLKernel::~MaxCLKernel()
{
	clReleaseKernel(kernel);
}

int MaxCLKernel::SetArg(int index, cl_mem mem) {
	return static_cast<int>(clSetKernelArg(kernel, index, sizeof(cl_mem), &mem));
}

int MaxCLKernel::Execute(int workDim, int globalWorkSize, int localWorkSize) {
	cl_int err;

	size_t globalSize = globalWorkSize;
	size_t localSize = localWorkSize;

	
	if (localSize) {
		err = clEnqueueNDRangeKernel(program->Platform()->Queue(), kernel, workDim, NULL, &globalSize, &localSize, 0, NULL, NULL);
	} else {
		err = clEnqueueNDRangeKernel(program->Platform()->Queue(), kernel, workDim, NULL, &globalSize, NULL, 0, NULL, NULL);
	}
	
	return err;

}

// --------------------------------------------------------

MaxCLProgram::MaxCLProgram(BBObject * handle, MaxCLPlatform * p, BBString * source)
	: maxHandle(handle), platform(p)
{

	cl_int err;
	char * s = bbStringToCString(source);
	
	program = clCreateProgramWithSource(platform->Context(), 1, (const char**)&s, NULL, &err);
	
	bbMemFree(s);
	
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	
	// TODO : handle error
}

MaxCLProgram::~MaxCLProgram()
{
}

cl_program MaxCLProgram::Program() {
	return program;
}

MaxCLPlatform * MaxCLProgram::Platform() {
	return platform;
}

// --------------------------------------------------------

MaxCLBuffer::MaxCLBuffer(BBObject * handle, MaxCLPlatform * p, int flags, int size, void * data)
	: maxHandle(handle), platform(p)
{
	cl_int err;
	buffer = clCreateBuffer(p->Context(), flags, size, data, &err);

	// TODO : THROW on NON-ZERO	
}

MaxCLBuffer::~MaxCLBuffer()
{
	clReleaseMemObject(buffer);
}

cl_mem MaxCLBuffer::Mem() {
	return buffer;
}

int MaxCLBuffer::EnqueueWrite(int blockingWrite, int size, void * data) {
	return clEnqueueWriteBuffer(platform->Queue(), buffer, blockingWrite, 0, size, data, 0, NULL, NULL);
}

int MaxCLBuffer::EnqueueRead(int blockingRead, int size, void * data) {
	return clEnqueueReadBuffer(platform->Queue(), buffer, blockingRead, 0, size, data, 0, NULL, NULL);
}

int MaxCLBuffer::EnqueueCopy(cl_mem dest, int offset, int destOffset, int size) {
	return clEnqueueCopyBuffer(platform->Queue(), buffer, dest, offset, destOffset, size, 0, NULL, NULL);
}

MaxCLImage::MaxCLImage(BBObject * handle, MaxCLPlatform * p, int flags, int channelOrder, int imageType, int width, int height, int rowPitch, void * hostPtr)
	: maxHandle(handle), platform(p)
{
	cl_int err;

	cl_image_format format;
	format.image_channel_order = channelOrder;
	format.image_channel_data_type = imageType;


	image = clCreateImage2D(p->Context(), flags, &format, width, height, rowPitch, hostPtr, &err);

	// TODO : THROW on NON-ZERO	
}

MaxCLImage::MaxCLImage(BBObject * handle, MaxCLPlatform * p, int flags, int channelOrder, int imageType, int width, int height, int depth, int rowPitch, int slicePitch, void * hostPtr)
	: maxHandle(handle), platform(p)
{
	cl_int err;

	cl_image_format format;
	format.image_channel_order = channelOrder;
	format.image_channel_data_type = imageType;


	image = clCreateImage3D(p->Context(), flags, &format, width, height, depth, rowPitch, slicePitch, hostPtr, &err);

	// TODO : THROW on NON-ZERO	
}

MaxCLImage::~MaxCLImage()
{
	clReleaseMemObject(image);
}

// --------------------------------------------------------

MaxCLPlatform * bmx_ocl_platform_init(BBObject * handle, MaxCLDevice * device) {
	return new MaxCLPlatform(handle, device);
}

BBArray * bmx_ocl_platform_getdevices(int deviceType) {

	cl_int err;
	cl_uint size;
	cl_device_id devices[10];
	
	err = clGetDeviceIDs(NULL, deviceType, 10, devices, &size);

	BBArray * list = _bah_opencl_TCLDevice__newDeviceList(size);
	
	for (int n = 0; n < size; n++) {
		cl_device_id device = devices[n];
		cl_device_type type;
		size_t s;
	
		err = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, &s);
	
		MaxCLDevice * dev = new MaxCLDevice(device);
		BBObject * obj = _bah_opencl_TCLDevice__setDevice(list, n, dev, type);
		dev->SetHandle(obj);
	}
	
	return list;
}

int bmx_ocl_platform_flush(MaxCLPlatform * platform) {
	return static_cast<int>(clFlush(platform->Queue()));
}

int bmx_ocl_platform_finish(MaxCLPlatform * platform) {
	return static_cast<int>(clFinish(platform->Queue()));
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBObject * bmx_ocl_device_getinfo(MaxCLDevice * device) {
	return device->GetInfo();
}

void bmx_ocl_device_free(MaxCLDevice * device) {
	delete device;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxCLProgram * bmx_ocl_program_create(BBObject * handle, MaxCLPlatform * platform, BBString * text) {
	return new MaxCLProgram(handle, platform, text);
}

void bmx_ocl_program_free(MaxCLProgram * program) {
	delete program;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxCLKernel * bmx_ocl_kernel_create(BBObject * handle, BBString * name, MaxCLProgram * program) {
	
	cl_int err;
	char * k = bbStringToCString(name);
	
	MaxCLKernel * kernel = new MaxCLKernel(handle, program, clCreateKernel(program->Program(), k, &err));
	
	bbMemFree(k);
	
	return kernel;
}

int bmx_ocl_kernel_setargbuffer(MaxCLKernel * kernel, int index, MaxCLBuffer * buffer) {
	return kernel->SetArg(index, buffer->Mem());
}

int bmx_ocl_kernel_execute(MaxCLKernel * kernel, int workDim, int globalWorkSize, int localWorkSize) {
	return kernel->Execute(workDim, globalWorkSize, localWorkSize);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxCLBuffer * bmx_ocl_membuff_create(BBObject * handle, MaxCLPlatform * platform, int flags, int size, void * data) {
	return new MaxCLBuffer(handle, platform, flags, size, data);
}

int bmx_ocl_membuff_enqueuewrite(MaxCLBuffer * mem, int blockingWrite, int size, void * data) {
	return mem->EnqueueWrite(blockingWrite, size, data);
}

int bmx_ocl_membuff_enqueueread(MaxCLBuffer * mem, int blockingRead, int size, void * data) {
	return mem->EnqueueRead(blockingRead, size, data);
}

int bmx_ocl_membuff_enqueuecopy(MaxCLBuffer * mem, MaxCLBuffer * dest, int offset, int destOffset, int size) {
	return mem->EnqueueCopy(dest->Mem(), offset, destOffset, size);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxCLImage * bmx_ocl_memimage_create2d(BBObject * handle, MaxCLPlatform * platform, int flags, int channelOrder, int imageType, int width, int height, int rowPitch, void * hostPtr) {
	return new MaxCLImage(handle, platform, flags, channelOrder, imageType, width, height, rowPitch, hostPtr);
}

MaxCLImage * bmx_ocl_memimage_create3d(BBObject * handle, MaxCLPlatform * platform, int flags, int channelOrder, int imageType, int width, int height, int depth, int rowPitch, int slicePitch, void * hostPtr) {
	return new MaxCLImage(handle, platform, flags, channelOrder, imageType, width, height, depth, rowPitch, slicePitch, hostPtr);
}


