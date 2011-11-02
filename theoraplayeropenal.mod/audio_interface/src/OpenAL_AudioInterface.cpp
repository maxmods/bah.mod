/************************************************************************************
This source file is part of the Theora Video Playback Library
For latest info, see http://libtheoraplayer.sourceforge.net/
*************************************************************************************
Copyright (c) 2008-2010 Kresimir Spes (kreso@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
*************************************************************************************/
#include <stdio.h>
#include "OpenAL_AudioInterface.h"

ALCdevice* gDevice=0;
ALCcontext* gContext=0;



extern "C" {


#if _WIN32

#include <windows.h>

static HMODULE openAL;

int _LoadOpenAL(){

	openAL=LoadLibrary( "OpenAL32.dll" );
	
	return openAL!=0;
}

void *_GetOpenALFunction( const char *fname ){

	if( !openAL ) return 0;
	
	return (int(*))GetProcAddress( openAL,fname );
}

#endif

#if __APPLE__

#import <CoreFoundation/CoreFoundation.h>

static CFBundleRef openAL;

static CFBundleRef OpenBundle( const char *path ){
	CFURLRef url;
	CFStringRef str;
	CFBundleRef bundle;
	str=CFStringCreateWithCString( kCFAllocatorDefault,path,kCFStringEncodingASCII );
	url=CFURLCreateWithFileSystemPath( kCFAllocatorDefault,str,kCFURLPOSIXPathStyle,true );
	bundle=CFBundleCreate( kCFAllocatorDefault,url );
	CFRelease( url );
	CFRelease( str );
	return bundle;
}

int _LoadOpenAL(){

	openAL=OpenBundle( "/System/Library/Frameworks/OpenAL.framework" );
	if( !openAL ) openAL=OpenBundle( "/Library/Frameworks/OpenAL.framework" );
	
	return openAL!=0;
}

void *_GetOpenALFunction( const char *fname ){
	void *p;
	CFStringRef str;
	
	if( !openAL ) return 0;
	
	str=CFStringCreateWithCString( kCFAllocatorDefault,fname,kCFStringEncodingASCII );
	
	p=CFBundleGetFunctionPointerForName( openAL,str );
	
	CFRelease( str );
	
	return p;
}

#endif

#if __linux

#include <dlfcn.h>

static void *openAL;

int _LoadOpenAL(){

	openAL=dlopen( "libopenal.so",RTLD_NOW );
	if( !openAL ) openAL=dlopen( "libopenal.so.0",RTLD_NOW );
	if( !openAL ) openAL=dlopen( "libopenal.so.0.0.8",RTLD_NOW );

	return openAL!=0;
}

void *_GetOpenALFunction( const char *fname ){

	if( !openAL ) return 0;

	return dlsym( openAL,fname );
}

#endif

	LPALCOPENDEVICE openal_alcOpenDevice;
	LPALCCREATECONTEXT openal_alcCreateContext;
	LPALCDESTROYCONTEXT openal_alcDestroyContext;
	LPALCCLOSEDEVICE openal_alcCloseDevice;
	LPALCGETERROR openal_alcGetError;
	LPALCMAKECONTEXTCURRENT openal_alcMakeContextCurrent;
	
	LPALGENSOURCES openal_alGenSources; 
	LPALGENBUFFERS openal_alGenBuffers;
	LPALBUFFERDATA openal_alBufferData;
	LPALSOURCEQUEUEBUFFERS openal_alSourceQueueBuffers;
	LPALSOURCEF openal_alSourcef; 
	LPALGETSOURCEI openal_alGetSourcei;
	LPALGETSOURCEF openal_alGetSourcef;
	LPALSOURCEPLAY openal_alSourcePlay;
	LPALSOURCESTOP openal_alSourceStop;
	LPALSOURCEPAUSE openal_alSourcePause;
	LPALSOURCEUNQUEUEBUFFERS openal_alSourceUnqueueBuffers;
	LPALDELETEBUFFERS openal_alDeleteBuffers;
	
}



short float2short(float f)
{
	if      (f >  1) f= 1;
	else if (f < -1) f=-1;
	return (short) (f*32767);
}

OpenAL_AudioInterface::OpenAL_AudioInterface(TheoraVideoClip* owner,int nChannels,int freq) :
	TheoraAudioInterface(owner,nChannels,freq), TheoraTimer()
{
	mMaxBuffSize=freq*mNumChannels*2;
	mBuffSize=0;
	mNumProcessedSamples=0;
	mTimeOffset=0;

	mTempBuffer=new short[mMaxBuffSize];
	openal_alGenSources(1,&mSource);
	owner->setTimer(this);
	mNumPlayedSamples=0;
}

void OpenAL_AudioInterface::destroy()
{
	// todo
}

OpenAL_AudioInterface::~OpenAL_AudioInterface()
{
	// todo: delete buffers and source
	if (mTempBuffer) delete mTempBuffer;
}

void OpenAL_AudioInterface::insertData(float** data,int nSamples)
{
	//printf("got %d bytes, %d buffers queued\n",nSamples,(int)mBufferQueue.size());
	for (int i=0;i<nSamples;i++)
	{
		if (mBuffSize < mMaxBuffSize)
		{
			//mTempBuffer[mBuffSize++]=rand(); debug
			mTempBuffer[mBuffSize++]=float2short(data[0][i]);
			if (mNumChannels == 2)
				mTempBuffer[mBuffSize++]=float2short(data[1][i]);
		}
		if (mBuffSize == mFreq*mNumChannels/4)
		{
			OpenAL_Buffer buff;
			openal_alGenBuffers(1,&buff.id);


			ALuint format = (mNumChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			openal_alBufferData(buff.id,format,mTempBuffer,mBuffSize*2,mFreq);
			openal_alSourceQueueBuffers(mSource, 1, &buff.id);
			buff.nSamples=mBuffSize/mNumChannels;
			mNumProcessedSamples+=mBuffSize/mNumChannels;
			mBufferQueue.push(buff);

			mBuffSize=0;

			int state;
			openal_alGetSourcei(mSource,AL_SOURCE_STATE,&state);
			if (state != AL_PLAYING)
			{
				//alSourcef(mSource,AL_PITCH,0.5); // debug
				openal_alSourcef(mSource,AL_SAMPLE_OFFSET,(float) mNumProcessedSamples-mFreq/4);
				openal_alSourcePlay(mSource);
			}

		}
	}
}

void OpenAL_AudioInterface::update(float time_increase)
{
	int i,state,nProcessed;
	OpenAL_Buffer buff;

	// process played buffers

	openal_alGetSourcei(mSource,AL_BUFFERS_PROCESSED,&nProcessed);
	for (i=0;i<nProcessed;i++)
	{
		buff=mBufferQueue.front();
		mBufferQueue.pop();
		mNumPlayedSamples+=buff.nSamples;
		openal_alSourceUnqueueBuffers(mSource,1,&buff.id);
		openal_alDeleteBuffers(1,&buff.id);
	}

	// control playback and return time position
	openal_alGetSourcei(mSource,AL_SOURCE_STATE,&state);
	if (state == AL_PLAYING)
	{
		openal_alGetSourcef(mSource,AL_SEC_OFFSET,&mTime);
		mTime+=(float) mNumPlayedSamples/mFreq;
		mTimeOffset=0;
	}
	else
	{
		mTime=(float) mNumProcessedSamples/mFreq+mTimeOffset;
		mTimeOffset+=time_increase;
	}

	//if (mTimeOffset > 0) printf("%.2f\n",mTimeOffset);

	float duration=mClip->getDuration();
	if (mTime > duration) mTime=duration;
}

void OpenAL_AudioInterface::pause()
{
	openal_alSourcePause(mSource);
	TheoraTimer::pause();
}

void OpenAL_AudioInterface::play()
{
	openal_alSourcePlay(mSource);
	TheoraTimer::play();
}

void OpenAL_AudioInterface::seek(float time)
{
	OpenAL_Buffer buff;

	openal_alSourceStop(mSource);
	while (!mBufferQueue.empty())
	{
		buff=mBufferQueue.front();
		mBufferQueue.pop();
		openal_alSourceUnqueueBuffers(mSource,1,&buff.id);
		openal_alDeleteBuffers(1,&buff.id);
	}
//		int nProcessed;
//		alGetSourcei(mSource,AL_BUFFERS_PROCESSED,&nProcessed);
//		if (nProcessed != 0)
//			nProcessed=nProcessed;
	mBuffSize=0;
	mTimeOffset=0;

	mNumPlayedSamples=mNumProcessedSamples=(int) time*mFreq;
}

OpenAL_AudioInterfaceFactory::OpenAL_AudioInterfaceFactory()
{
	_LoadOpenAL();
	openal_alcCreateContext = (LPALCCREATECONTEXT)_GetOpenALFunction("alcCreateContext");
	openal_alcOpenDevice = (LPALCOPENDEVICE)_GetOpenALFunction("alcOpenDevice");
	openal_alcDestroyContext = (LPALCDESTROYCONTEXT)_GetOpenALFunction("alcDestroyContext");
	openal_alcCloseDevice = (LPALCCLOSEDEVICE)_GetOpenALFunction("alcCloseDevice");
	openal_alcGetError = (LPALCGETERROR)_GetOpenALFunction("alcGetError");
	openal_alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)_GetOpenALFunction("alcMakeContextCurrent");
	
	openal_alGenSources = (LPALGENSOURCES)_GetOpenALFunction("alGenSources"); 
	openal_alGenBuffers = (LPALGENBUFFERS)_GetOpenALFunction("alGenBuffers");
	openal_alBufferData = (LPALBUFFERDATA)_GetOpenALFunction("alBufferData");
	openal_alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)_GetOpenALFunction("alSourceQueueBuffers");
	openal_alSourcef = (LPALSOURCEF)_GetOpenALFunction("alSourcef"); 
	openal_alGetSourcei = (LPALGETSOURCEI)_GetOpenALFunction("alGetSourcei");
	openal_alGetSourcef = (LPALGETSOURCEF)_GetOpenALFunction("alGetSourcef");
	openal_alSourcePlay = (LPALSOURCEPLAY)_GetOpenALFunction("alSourcePlay");
	openal_alSourceStop = (LPALSOURCESTOP)_GetOpenALFunction("alSourceStop");
	openal_alSourcePause = (LPALSOURCEPAUSE)_GetOpenALFunction("alSourcePause");
	openal_alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)_GetOpenALFunction("alSourceUnqueueBuffers");
	openal_alDeleteBuffers = (LPALDELETEBUFFERS)_GetOpenALFunction("alDeleteBuffers");


	// openal init is here used only to simplify samples for this plugin
	// if you want to use this interface in your own program, you'll
	// probably want to remove the openal init/destory lines
	gDevice = openal_alcOpenDevice("");
	if (openal_alcGetError(gDevice) != ALC_NO_ERROR) goto Fail;
	gContext = openal_alcCreateContext(gDevice, NULL);
	if (openal_alcGetError(gDevice) != ALC_NO_ERROR) goto Fail;
	openal_alcMakeContextCurrent(gContext);
	if (openal_alcGetError(gDevice) != ALC_NO_ERROR) goto Fail;

	return;
Fail:
	gDevice=NULL;
	gContext=NULL;
}

OpenAL_AudioInterfaceFactory::~OpenAL_AudioInterfaceFactory()
{
	if (gDevice)
	{
		openal_alcMakeContextCurrent(NULL);
		openal_alcDestroyContext(gContext);
		openal_alcCloseDevice(gDevice);
	}
}

OpenAL_AudioInterface* OpenAL_AudioInterfaceFactory::createInstance(TheoraVideoClip* owner,int nChannels,int freq)
{
	return new OpenAL_AudioInterface(owner,nChannels,freq);
}
