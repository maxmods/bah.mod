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
	irrklang::ISound* bmx_soundengine_play3dsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource* source, Maxvec3df * pos,
		bool  playLooped, bool  startPaused, bool  track, bool  enableSoundEffects);
	bool bmx_soundengine_iscurrentlyplaying(irrklang::ISoundEngine* engine, const char *name);
	bool bmx_soundengine_iscurrentlyplayingsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource * source);
	irrklang::ik_s32 bmx_soundengine_getsoundsourcecount(irrklang::ISoundEngine* engine);
	void bmx_soundengine_removesoundsource(irrklang::ISoundEngine* engine, const irrklang::ik_c8 * name);
	void bmx_soundengine_removesoundsourcesource(irrklang::ISoundEngine* engine, irrklang::ISoundSource * source);
	const char * bmx_soundengine_getdrivername(irrklang::ISoundEngine* engine);

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

	void bmx_audiostreamformat_delete(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getchannelcount(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getframecount(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getsamplerate(MaxSAudioStreamFormat * format);
	irrklang::ESampleFormat bmx_audiostreamformat_getsampleformat(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getsamplesize(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getframesize(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getsampledatasize(MaxSAudioStreamFormat * format);
	irrklang::ik_s32 bmx_audiostreamformat_getbytespersecond(MaxSAudioStreamFormat * format);

	void bmx_soundeffect_disablealleffects(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enablechorussoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fDepth, irrklang::ik_f32 fFeedback,
		irrklang::ik_f32 fFrequency, bool sinusWaveForm, irrklang::ik_f32 fDelay, irrklang::ik_s32 lPhase);
	void bmx_soundeffect_disablechorussoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_ischorussoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enablecompressorsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fGain, irrklang::ik_f32 fAttack, irrklang::ik_f32 fRelease,
		irrklang::ik_f32 fThreshold, irrklang::ik_f32 fRatio, irrklang::ik_f32 fPredelay);
	void bmx_soundeffect_disablecompressorsoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_iscompressorsoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enabledistortionsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fGain, irrklang::ik_f32 fEdge, irrklang::ik_f32 fPostEQCenterFrequency,
		irrklang::ik_f32 fPostEQBandwidth, irrklang::ik_f32 fPreLowpassCutoff);
	void bmx_soundeffect_disabledistortionsoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isdistortionsoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enableechosoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fFeedback, irrklang::ik_f32 fLeftDelay,
		irrklang::ik_f32 fRightDelay, irrklang::ik_s32 lPanDelay);
	void bmx_soundeffect_disableechosoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isechosoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enableflangersoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fDepth, irrklang::ik_f32 fFeedback,
		irrklang::ik_f32 fFrequency, bool triangleWaveForm, irrklang::ik_f32 fDelay, irrklang::ik_s32 lPhase);
	void bmx_soundeffect_disableflangersoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isflangersoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enablegarglesoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_s32 rateHz, bool sinusWaveForm);
	void bmx_soundeffect_disablegarglesoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isgarglesoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enablei3dl2reverbsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_s32 lRoom, irrklang::ik_s32 lRoomHF, irrklang::ik_f32 flRoomRolloffFactor,
		irrklang::ik_f32 flDecayTime, irrklang::ik_f32 flDecayHFRatio, irrklang::ik_s32 lReflections, irrklang::ik_f32 flReflectionsDelay,
		irrklang::ik_s32 lReverb, irrklang::ik_f32 flReverbDelay, irrklang::ik_f32 flDiffusion, irrklang::ik_f32 flDensity, irrklang::ik_f32 flHFReference);
	void bmx_soundeffect_disablei3dl2reverbsoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isi3dl2reverbsoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enableparameqsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fCenter, irrklang::ik_f32 fBandwidth, irrklang::ik_f32 fGain);
	void bmx_soundeffect_disableparameqsoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_isparameqsoundeffectenabled(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_enablewavesreverdsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fInGain, irrklang::ik_f32 fReverbMix, irrklang::ik_f32 fReverbTime, irrklang::ik_f32 fHighFreqRTRatio);
	void bmx_soundeffect_disablewavesreverbsoundeffect(irrklang::ISoundEffectControl * effect);
	bool bmx_soundeffect_iswavesreverbsoundeffectenabled(irrklang::ISoundEffectControl * effect);

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

irrklang::ISound* bmx_soundengine_play3dsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource* source, Maxvec3df * pos,
		bool  playLooped, bool  startPaused, bool  track, bool  enableSoundEffects) {
	return engine->play3D(source, pos->vec, playLooped, startPaused, track, enableSoundEffects);
}

bool bmx_soundengine_iscurrentlyplaying(irrklang::ISoundEngine* engine, const char *name) {
	return engine->isCurrentlyPlaying(name);
}

bool bmx_soundengine_iscurrentlyplayingsource(irrklang::ISoundEngine* engine, irrklang::ISoundSource * source) {
	return engine->isCurrentlyPlaying(source);
}

irrklang::ik_s32 bmx_soundengine_getsoundsourcecount(irrklang::ISoundEngine* engine) {
	return engine->getSoundSourceCount();
}

void bmx_soundengine_removesoundsource(irrklang::ISoundEngine* engine, const irrklang::ik_c8 * name) {
	engine->removeSoundSource(name);
}

void bmx_soundengine_removesoundsourcesource(irrklang::ISoundEngine* engine, irrklang::ISoundSource * source) {
	engine->removeSoundSource(source);
}

const char * bmx_soundengine_getdrivername(irrklang::ISoundEngine* engine) {
	return engine->getDriverName();
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

// *****************************************************


void bmx_audiostreamformat_delete(MaxSAudioStreamFormat * format) {
	delete format;
}

irrklang::ik_s32 bmx_audiostreamformat_getchannelcount(MaxSAudioStreamFormat * format) {
	return format->format.ChannelCount;
}

irrklang::ik_s32 bmx_audiostreamformat_getframecount(MaxSAudioStreamFormat * format) {
	return format->format.FrameCount;
}

irrklang::ik_s32 bmx_audiostreamformat_getsamplerate(MaxSAudioStreamFormat * format) {
	return format->format.SampleRate;
}

irrklang::ESampleFormat bmx_audiostreamformat_getsampleformat(MaxSAudioStreamFormat * format) {
	return format->format.SampleFormat;
}

irrklang::ik_s32 bmx_audiostreamformat_getsamplesize(MaxSAudioStreamFormat * format) {
	return format->format.getSampleSize();
}

irrklang::ik_s32 bmx_audiostreamformat_getframesize(MaxSAudioStreamFormat * format) {
	return format->format.getFrameSize();
}

irrklang::ik_s32 bmx_audiostreamformat_getsampledatasize(MaxSAudioStreamFormat * format) {
	return format->format.getSampleDataSize();
}

irrklang::ik_s32 bmx_audiostreamformat_getbytespersecond(MaxSAudioStreamFormat * format) {
	return format->format.getBytesPerSecond();
}


// *****************************************************


void bmx_soundeffect_disablealleffects(irrklang::ISoundEffectControl * effect) {
    effect->disableAllEffects();
}

bool bmx_soundeffect_enablechorussoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fDepth, irrklang::ik_f32 fFeedback,
        irrklang::ik_f32 fFrequency, bool sinusWaveForm, irrklang::ik_f32 fDelay, irrklang::ik_s32 lPhase) {
    return effect->enableChorusSoundEffect(fWetDryMix, fDepth, fFeedback, fFrequency, sinusWaveForm, fDelay, lPhase);
}

void bmx_soundeffect_disablechorussoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableChorusSoundEffect();
}

bool bmx_soundeffect_ischorussoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isChorusSoundEffectEnabled();
}

bool bmx_soundeffect_enablecompressorsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fGain, irrklang::ik_f32 fAttack, irrklang::ik_f32 fRelease,
        irrklang::ik_f32 fThreshold, irrklang::ik_f32 fRatio, irrklang::ik_f32 fPredelay) {
    return effect->enableCompressorSoundEffect(fGain, fAttack, fRelease, fThreshold, fRatio, fPredelay);
}

void bmx_soundeffect_disablecompressorsoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableCompressorSoundEffect();
}

bool bmx_soundeffect_iscompressorsoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isCompressorSoundEffectEnabled();
}

bool bmx_soundeffect_enabledistortionsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fGain, irrklang::ik_f32 fEdge, irrklang::ik_f32 fPostEQCenterFrequency,
        irrklang::ik_f32 fPostEQBandwidth, irrklang::ik_f32 fPreLowpassCutoff) {
    return effect->enableDistortionSoundEffect(fGain, fEdge, fPostEQCenterFrequency, fPostEQBandwidth, fPreLowpassCutoff);
}

void bmx_soundeffect_disabledistortionsoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableDistortionSoundEffect();
}

bool bmx_soundeffect_isdistortionsoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isDistortionSoundEffectEnabled();
}

bool bmx_soundeffect_enableechosoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fFeedback, irrklang::ik_f32 fLeftDelay,
        irrklang::ik_f32 fRightDelay, irrklang::ik_s32 lPanDelay) {
    return effect->enableEchoSoundEffect(fWetDryMix, fFeedback, fLeftDelay, fRightDelay, lPanDelay);
}

void bmx_soundeffect_disableechosoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableEchoSoundEffect();
}

bool bmx_soundeffect_isechosoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isEchoSoundEffectEnabled();
}

bool bmx_soundeffect_enableflangersoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fWetDryMix, irrklang::ik_f32 fDepth, irrklang::ik_f32 fFeedback,
        irrklang::ik_f32 fFrequency, bool triangleWaveForm, irrklang::ik_f32 fDelay, irrklang::ik_s32 lPhase) {
    return effect->enableFlangerSoundEffect(fWetDryMix, fDepth, fFeedback, fFrequency, triangleWaveForm, fDelay, lPhase);
}

void bmx_soundeffect_disableflangersoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableFlangerSoundEffect();
}

bool bmx_soundeffect_isflangersoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isFlangerSoundEffectEnabled();
}

bool bmx_soundeffect_enablegarglesoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_s32 rateHz, bool sinusWaveForm) {
    return effect->enableGargleSoundEffect(rateHz, sinusWaveForm);
}

void bmx_soundeffect_disablegarglesoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableGargleSoundEffect();
}

bool bmx_soundeffect_isgarglesoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isGargleSoundEffectEnabled();
}

bool bmx_soundeffect_enablei3dl2reverbsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_s32 lRoom, irrklang::ik_s32 lRoomHF, irrklang::ik_f32 flRoomRolloffFactor,
        irrklang::ik_f32 flDecayTime, irrklang::ik_f32 flDecayHFRatio, irrklang::ik_s32 lReflections, irrklang::ik_f32 flReflectionsDelay,
        irrklang::ik_s32 lReverb, irrklang::ik_f32 flReverbDelay, irrklang::ik_f32 flDiffusion, irrklang::ik_f32 flDensity, irrklang::ik_f32 flHFReference) {
    return effect->enableI3DL2ReverbSoundEffect(lRoom, lRoomHF, flRoomRolloffFactor,
        flDecayTime, flDecayHFRatio, lReflections, flReflectionsDelay, lReverb, flReverbDelay, flDiffusion, flDensity, flHFReference);
}

void bmx_soundeffect_disablei3dl2reverbsoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableI3DL2ReverbSoundEffect();
}

bool bmx_soundeffect_isi3dl2reverbsoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isI3DL2ReverbSoundEffectEnabled();
}

bool bmx_soundeffect_enableparameqsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fCenter, irrklang::ik_f32 fBandwidth, irrklang::ik_f32 fGain) {
    return effect->enableParamEqSoundEffect(fCenter, fBandwidth, fGain);
}

void bmx_soundeffect_disableparameqsoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableParamEqSoundEffect();
}

bool bmx_soundeffect_isparameqsoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isParamEqSoundEffectEnabled();
}

bool bmx_soundeffect_enablewavesreverdsoundeffect(irrklang::ISoundEffectControl * effect, irrklang::ik_f32 fInGain, irrklang::ik_f32 fReverbMix, irrklang::ik_f32 fReverbTime, irrklang::ik_f32 fHighFreqRTRatio) {
    return effect->enableWavesReverbSoundEffect(fInGain, fReverbMix, fReverbTime, fHighFreqRTRatio);
}

void bmx_soundeffect_disablewavesreverbsoundeffect(irrklang::ISoundEffectControl * effect) {
    effect->disableWavesReverbSoundEffect();
}

bool bmx_soundeffect_iswavesreverbsoundeffectenabled(irrklang::ISoundEffectControl * effect) {
    return effect->isWavesReverbSoundEffectEnabled();
}

