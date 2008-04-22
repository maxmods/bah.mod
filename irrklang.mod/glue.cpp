/*
 Copyright (c) 2008 Bruce A Henderson
 
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

#include <irrKlang.h>

class MaxSAudioStreamFormat;
class Maxvec3df;

extern "C" {

	irrklang::ISoundEngine* bmx_createIrrKlangDevice(irrklang::E_SOUND_OUTPUT_DRIVER driver, int options, const char * deviceID);

	irrklang::ISound* bmx_soundengine_play2d(irrklang::ISoundEngine* engine, const char *  soundFileName,
		bool  playLooped, bool  startPaused, bool  track, irrklang::E_STREAM_MODE  streamMode, bool  enableSoundEffects);
	bool bmx_soundengine_drop(irrklang::ISoundEngine* engine);
	void bmx_soundengine_stopallsounds(irrklang::ISoundEngine* engine);
	void bmx_soundengine_setallsoundspaused(irrklang::ISoundEngine* engine, bool paused);
	void bmx_soundengine_removeallsoundsources(irrklang::ISoundEngine* engine);
	void bmx_soundengine_setsoundvolume(irrklang::ISoundEngine* engine, irrklang::ik_f32 volume);
	irrklang::ik_f32 bmx_soundengine_getsoundvolume(irrklang::ISoundEngine* engine);
	void bmx_soundengine_update(irrklang::ISoundEngine* engine);
	void bmx_soundengine_setdefault3dsoundmindistance(irrklang::ISoundEngine* engine, irrklang::ik_f32 minDistance);
	irrklang::ik_f32 bmx_soundengine_getdefault3dsoundmindistance(irrklang::ISoundEngine* engine);
	void bmx_soundengine_setdefault3dsoundmaxdistance(irrklang::ISoundEngine* engine, irrklang::ik_f32 minDistance);
	irrklang::ik_f32 bmx_soundengine_getdefault3dsoundmaxdistance(irrklang::ISoundEngine* engine);
	void bmx_soundengine_setrollofffactor(irrklang::ISoundEngine* engine, irrklang::ik_f32 rolloff);
	void bmx_soundengine_setdopplereffectparameters(irrklang::ISoundEngine* engine, irrklang::ik_f32 dopplerFactor, irrklang::ik_f32 distanceFactor);
	irrklang::ISoundSource* bmx_soundengine_addsoundsourcefromfile(irrklang::ISoundEngine* engine, const irrklang::ik_c8 * filename, irrklang::E_STREAM_MODE mode, bool preload);
	irrklang::ISoundSource* bmx_soundengine_addsoundsourcefrommemory(irrklang::ISoundEngine* engine, void * memory, irrklang::ik_s32 sizeInBytes, const irrklang::ik_c8 * name, bool copyMemory);
	irrklang::ISound* bmx_soundengine_play2dsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource* source,
		bool  playLooped, bool  startPaused, bool  track, bool  enableSoundEffects);
	irrklang::ISound* bmx_soundengine_play3d(irrklang::ISoundEngine* engine, const char *  soundFileName, Maxvec3df * pos,
		bool  playLooped, bool  startPaused, bool  track, irrklang::E_STREAM_MODE  streamMode, bool  enableSoundEffects);
	void bmx_soundengine_setlistenerposition(irrklang::ISoundEngine* engine, Maxvec3df * pos, Maxvec3df * lookDir, Maxvec3df * velPerSecond, Maxvec3df * upVector);

	void bmx_sound_setispaused(irrklang::ISound * sound, bool paused);
	bool bmx_sound_getispaused(irrklang::ISound * sound);
	void bmx_sound_stop(irrklang::ISound * sound);
	irrklang::ik_f32 bmx_sound_getvolume(irrklang::ISound * sound);
	void bmx_sound_setvolume(irrklang::ISound * sound, irrklang::ik_f32 volume);
	void bmx_sound_setpan(irrklang::ISound * sound, irrklang::ik_f32 pan);
	irrklang::ik_f32 bmx_sound_getpan(irrklang::ISound * sound);
	bool bmx_sound_islooped(irrklang::ISound * sound);
	void bmx_sound_setislooped(irrklang::ISound * sound, bool looped);
	bool bmx_sound_isfinished(irrklang::ISound * sound);
	void bmx_sound_setmindistance(irrklang::ISound * sound, irrklang::ik_f32 minDistance);
	irrklang::ik_f32 bmx_sound_getmindistance(irrklang::ISound * sound);
	void bmx_sound_setmaxdistance(irrklang::ISound * sound, irrklang::ik_f32 maxDistance);
	irrklang::ik_f32 bmx_sound_getmaxdistance(irrklang::ISound * sound);
	irrklang::ik_u32 bmx_sound_getplayposition(irrklang::ISound * sound);
	bool bmx_sound_setplayposition(irrklang::ISound * sound, irrklang::ik_u32 pos);
	bool bmx_sound_setplaybackspeed(irrklang::ISound * sound, irrklang::ik_f32 speed);
	irrklang::ik_f32 bmx_sound_getplaybackspeed(irrklang::ISound * sound);
	irrklang::ik_u32 bmx_sound_getplaylength(irrklang::ISound * sound);
	irrklang::ISoundEffectControl * bmx_sound_getsoundeffectcontrol(irrklang::ISound * sound);
	void bmx_sound_drop(irrklang::ISound * sound);
	void bmx_sound_setposition(irrklang::ISound * sound, Maxvec3df * position);

	const irrklang::ik_c8 * bmx_soundsource_getname(irrklang::ISoundSource * source);
	void bmx_soundsource_setstreammode(irrklang::ISoundSource * source, irrklang::E_STREAM_MODE mode);
	irrklang::E_STREAM_MODE bmx_soundsource_getstreammode(irrklang::ISoundSource * source);
	irrklang::ik_u32 bmx_soundsource_getplaylength(irrklang::ISoundSource * source);
	MaxSAudioStreamFormat * bmx_soundsource_getaudioformat(irrklang::ISoundSource * source);
	bool bmx_soundsource_isseekingsupported(irrklang::ISoundSource * source);
	void bmx_soundsource_setdefaultvolume(irrklang::ISoundSource * source, irrklang::ik_f32 volume);
	irrklang::ik_f32 bmx_soundsource_getdefaultvolume(irrklang::ISoundSource * source);
	void bmx_soundsource_setdefaultmindistance(irrklang::ISoundSource * source, irrklang::ik_f32 minDistance);
	irrklang::ik_f32 bmx_soundsource_getdefaultmindistance(irrklang::ISoundSource * source);
	void bmx_soundsource_setdefaultmaxdistance(irrklang::ISoundSource * source, irrklang::ik_f32 maxDistance);
	irrklang::ik_f32 bmx_soundsource_getdefaultmaxdistance(irrklang::ISoundSource * source);
	void bmx_soundsource_forcereloadatnextuse(irrklang::ISoundSource * source);
	void bmx_soundsource_setforcedstreamingthreshold(irrklang::ISoundSource * source, irrklang::ik_s32 thresholdBytes);
	irrklang::ik_s32 bmx_soundsource_getforcedstreamingthreshold(irrklang::ISoundSource * source);

	Maxvec3df * bmx_vec3df_create(irrklang::ik_f32 x, irrklang::ik_f32 y, irrklang::ik_f32 z);
	void bmx_vec3df_delete(Maxvec3df * vec);
	irrklang::ik_f32 bmx_vec3df_x(Maxvec3df * vec);
	irrklang::ik_f32 bmx_vec3df_y(Maxvec3df * vec);
	irrklang::ik_f32 bmx_vec3df_z(Maxvec3df * vec);

}


class MaxSAudioStreamFormat
{
public:
	MaxSAudioStreamFormat(irrklang::SAudioStreamFormat f)
		: format(f)
	{
	}
	
	irrklang::SAudioStreamFormat format;
};

class Maxvec3df
{
public:
	Maxvec3df(irrklang::vec3df v)
		: vec(v)
	{
	}
	
	irrklang::vec3df vec;
};

// *****************************************************


irrklang::ISoundEngine* bmx_createIrrKlangDevice(irrklang::E_SOUND_OUTPUT_DRIVER driver, int options, const char * deviceID) {
	return irrklang::createIrrKlangDevice(driver, options, deviceID);
}

// *****************************************************


irrklang::ISound* bmx_soundengine_play2d(irrklang::ISoundEngine* engine, const char *  soundFileName, bool playLooped,
		bool  startPaused, bool track, irrklang::E_STREAM_MODE  streamMode, bool  enableSoundEffects) {

	return engine->play2D(soundFileName, playLooped, startPaused, track, streamMode, enableSoundEffects);
}

bool bmx_soundengine_drop(irrklang::ISoundEngine* engine) {
	return engine->drop();
}

void bmx_soundengine_stopallsounds(irrklang::ISoundEngine* engine) {
	engine->stopAllSounds();
}

void bmx_soundengine_setallsoundspaused(irrklang::ISoundEngine* engine, bool paused) {
	engine->setAllSoundsPaused(paused);
}

void bmx_soundengine_removeallsoundsources(irrklang::ISoundEngine* engine) {
	engine->removeAllSoundSources();
}

void bmx_soundengine_setsoundvolume(irrklang::ISoundEngine* engine, irrklang::ik_f32 volume) {
	engine->setSoundVolume(volume);
}

irrklang::ik_f32 bmx_soundengine_getsoundvolume(irrklang::ISoundEngine* engine) {
	return engine->getSoundVolume();
}

void bmx_soundengine_update(irrklang::ISoundEngine* engine) {
	engine->update();
}

void bmx_soundengine_setdefault3dsoundmindistance(irrklang::ISoundEngine* engine, irrklang::ik_f32 minDistance) {
	engine->setDefault3DSoundMinDistance(minDistance);
}

irrklang::ik_f32 bmx_soundengine_getdefault3dsoundmindistance(irrklang::ISoundEngine* engine) {
	return engine->getDefault3DSoundMinDistance();
}

void bmx_soundengine_setdefault3dsoundmaxdistance(irrklang::ISoundEngine* engine, irrklang::ik_f32 maxDistance) {
	engine->setDefault3DSoundMaxDistance(maxDistance);
}

irrklang::ik_f32 bmx_soundengine_getdefault3dsoundmaxdistance(irrklang::ISoundEngine* engine) {
	return engine->getDefault3DSoundMaxDistance();
}

void bmx_soundengine_setrollofffactor(irrklang::ISoundEngine* engine, irrklang::ik_f32 rolloff) {
	engine->setRolloffFactor(rolloff);
}

void bmx_soundengine_setdopplereffectparameters(irrklang::ISoundEngine* engine, irrklang::ik_f32 dopplerFactor, irrklang::ik_f32 distanceFactor) {
	engine->setDopplerEffectParameters(dopplerFactor, distanceFactor);
}

irrklang::ISoundSource* bmx_soundengine_addsoundsourcefromfile(irrklang::ISoundEngine* engine, const irrklang::ik_c8 * filename, irrklang::E_STREAM_MODE mode, bool preload) {
	return engine->addSoundSourceFromFile(filename, mode, preload);
}

irrklang::ISoundSource* bmx_soundengine_addsoundsourcefrommemory(irrklang::ISoundEngine* engine, void * memory, irrklang::ik_s32 sizeInBytes, const irrklang::ik_c8 * name, bool copyMemory) {
	return engine->addSoundSourceFromMemory(memory, sizeInBytes, name, copyMemory);
}

irrklang::ISound* bmx_soundengine_play2dsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource* source,
		bool  playLooped, bool  startPaused, bool  track, bool  enableSoundEffects) {
	return engine->play2D(source, playLooped, startPaused, track, enableSoundEffects);
}

irrklang::ISound* bmx_soundengine_play3d(irrklang::ISoundEngine* engine, const char *  soundFileName, Maxvec3df * pos, bool playLooped,
		bool  startPaused, bool track, irrklang::E_STREAM_MODE  streamMode, bool  enableSoundEffects) {

	return engine->play3D(soundFileName, pos->vec, playLooped, startPaused, track, streamMode, enableSoundEffects);
}

void bmx_soundengine_setlistenerposition(irrklang::ISoundEngine* engine, Maxvec3df * pos, Maxvec3df * lookDir, Maxvec3df * velPerSecond, Maxvec3df * upVector) {
	engine->setListenerPosition(pos->vec, lookDir->vec, (velPerSecond) ? velPerSecond->vec : irrklang::vec3df(0,0,0),
		(upVector) ? upVector->vec : irrklang::vec3df(0,1,0));
}

// *****************************************************

void bmx_sound_setispaused(irrklang::ISound * sound, bool paused) {
    sound->setIsPaused(paused);
}

bool bmx_sound_getispaused(irrklang::ISound * sound) {
    return sound->getIsPaused();
}

void bmx_sound_stop(irrklang::ISound * sound) {
    sound->stop();
}

irrklang::ik_f32 bmx_sound_getvolume(irrklang::ISound * sound) {
    return sound->getVolume();
}

void bmx_sound_setvolume(irrklang::ISound * sound, irrklang::ik_f32 volume) {
    sound->setVolume(volume);
}

void bmx_sound_setpan(irrklang::ISound * sound, irrklang::ik_f32 pan) {
    sound->setPan(pan);
}

irrklang::ik_f32 bmx_sound_getpan(irrklang::ISound * sound) {
    return sound->getPan();
}

bool bmx_sound_islooped(irrklang::ISound * sound) {
    return sound->isLooped();
}

void bmx_sound_setislooped(irrklang::ISound * sound, bool looped) {
    sound->setIsLooped(looped);
}

bool bmx_sound_isfinished(irrklang::ISound * sound) {
    return sound->isFinished();
}

void bmx_sound_setmindistance(irrklang::ISound * sound, irrklang::ik_f32 minDistance) {
    sound->setMinDistance(minDistance);
}

irrklang::ik_f32 bmx_sound_getmindistance(irrklang::ISound * sound) {
    return sound->getMinDistance();
}

void bmx_sound_setmaxdistance(irrklang::ISound * sound, irrklang::ik_f32 maxDistance) {
    sound->setMaxDistance(maxDistance);
}

irrklang::ik_f32 bmx_sound_getmaxdistance(irrklang::ISound * sound) {
    return sound->getMaxDistance();
}

irrklang::ik_u32 bmx_sound_getplayposition(irrklang::ISound * sound) {
	return sound->getPlayPosition();
}

bool bmx_sound_setplayposition(irrklang::ISound * sound, irrklang::ik_u32 pos) {
	return sound->setPlayPosition(pos);
}

bool bmx_sound_setplaybackspeed(irrklang::ISound * sound, irrklang::ik_f32 speed) {
	return sound->setPlaybackSpeed(speed);
}

irrklang::ik_f32 bmx_sound_getplaybackspeed(irrklang::ISound * sound) {
	return sound->getPlaybackSpeed();
}

irrklang::ik_u32 bmx_sound_getplaylength(irrklang::ISound * sound) {
	return sound->getPlayLength();
}

irrklang::ISoundEffectControl * bmx_sound_getsoundeffectcontrol(irrklang::ISound * sound) {
	return sound->getSoundEffectControl();
}

void bmx_sound_drop(irrklang::ISound * sound) {
	sound->drop();
}

void bmx_sound_setposition(irrklang::ISound * sound, Maxvec3df * position) {
	sound->setPosition(position->vec);
}

// *****************************************************

const irrklang::ik_c8 * bmx_soundsource_getname(irrklang::ISoundSource * source) {
	return source->getName();
}

void bmx_soundsource_setstreammode(irrklang::ISoundSource * source, irrklang::E_STREAM_MODE mode) {
	source->setStreamMode(mode);
}

irrklang::E_STREAM_MODE bmx_soundsource_getstreammode(irrklang::ISoundSource * source) {
	return source->getStreamMode();
}

irrklang::ik_u32 bmx_soundsource_getplaylength(irrklang::ISoundSource * source) {
	return source->getPlayLength();
}

MaxSAudioStreamFormat * bmx_soundsource_getaudioformat(irrklang::ISoundSource * source) {
	return new MaxSAudioStreamFormat(source->getAudioFormat());
}

bool bmx_soundsource_isseekingsupported(irrklang::ISoundSource * source) {
	return source->getIsSeekingSupported();
}

void bmx_soundsource_setdefaultvolume(irrklang::ISoundSource * source, irrklang::ik_f32 volume) {
	source->setDefaultVolume(volume);
}

irrklang::ik_f32 bmx_soundsource_getdefaultvolume(irrklang::ISoundSource * source) {
	return source->getDefaultVolume();
}

void bmx_soundsource_setdefaultmindistance(irrklang::ISoundSource * source, irrklang::ik_f32 minDistance) {
	source->setDefaultMinDistance(minDistance);
}

irrklang::ik_f32 bmx_soundsource_getdefaultmindistance(irrklang::ISoundSource * source) {
	return source->getDefaultMinDistance();
}

void bmx_soundsource_setdefaultmaxdistance(irrklang::ISoundSource * source, irrklang::ik_f32 maxDistance) {
	source->setDefaultMaxDistance(maxDistance);
}

irrklang::ik_f32 bmx_soundsource_getdefaultmaxdistance(irrklang::ISoundSource * source) {
	return source->getDefaultMaxDistance();
}

void bmx_soundsource_forcereloadatnextuse(irrklang::ISoundSource * source) {
	source->forceReloadAtNextUse();
}

void bmx_soundsource_setforcedstreamingthreshold(irrklang::ISoundSource * source, irrklang::ik_s32 thresholdBytes) {
	source->setForcedStreamingThreshold(thresholdBytes);
}

irrklang::ik_s32 bmx_soundsource_getforcedstreamingthreshold(irrklang::ISoundSource * source) {
	return source->getForcedStreamingForceThreshold ();
}

// *****************************************************


Maxvec3df * bmx_vec3df_create(irrklang::ik_f32 x, irrklang::ik_f32 y, irrklang::ik_f32 z) {
	irrklang::vec3df pos(x, y, z);
	return new Maxvec3df(pos);
}

void bmx_vec3df_delete(Maxvec3df * vec) {
	delete vec;
}

irrklang::ik_f32 bmx_vec3df_x(Maxvec3df * vec) {
	return vec->vec.X;
}

irrklang::ik_f32 bmx_vec3df_y(Maxvec3df * vec) {
	return vec->vec.Y;
}

irrklang::ik_f32 bmx_vec3df_z(Maxvec3df * vec) {
	return vec->vec.Z;
}

