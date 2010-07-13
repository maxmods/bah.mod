/*

  Copyright (c) 2007 Tomas Pettersson
  Copyright (c) 2010 Bruce A Henderson
 
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

#include <stdlib.h>
#include <time.h>
#include <math.h>


#define rnd(n) (rand()%(n+1))

#define PI 3.14159265f


class Sfxr;

extern "C" {

#include "blitz.h"

	BBObject * _bah_sfxr_TSfxrSettings__create(BBObject * settings, int wave_type, float sound_vol, float p_base_freq, float p_freq_limit, float p_freq_ramp, float p_freq_dramp, 
		float p_duty, float p_duty_ramp, float p_vib_strength, float p_vib_speed, float p_vib_delay, float p_env_attack, float p_env_sustain, 
		float p_env_decay, float p_env_punch, int filter_on, float p_lpf_resonance, float p_lpf_freq, float p_lpf_ramp, float p_hpf_freq, 
		float p_hpf_ramp, float p_pha_offset, float p_pha_ramp, float p_repeat_speed, float p_arp_speed, float p_arp_mod);


	Sfxr * bmx_sfxr_new();
	void bmx_sfxr_resetparams(Sfxr * sfx);
	BBObject * bmx_sfxr_getsettings(Sfxr * sfx, BBObject * settings);
	void bmx_sfxr_setsettings(Sfxr * sfx, int wave_type, float sound_vol, float p_base_freq, float p_freq_limit, float p_freq_ramp, float p_freq_dramp, 
		float p_duty, float p_duty_ramp, float p_vib_strength, float p_vib_speed, float p_vib_delay, float p_env_attack, float p_env_sustain, 
		float p_env_decay, float p_env_punch, int filter_on, float p_lpf_resonance, float p_lpf_freq, float p_lpf_ramp, float p_hpf_freq, 
		float p_hpf_ramp, float p_pha_offset, float p_pha_ramp, float p_repeat_speed, float p_arp_speed, float p_arp_mod);

	int bmx_sfxr_calculatesize(Sfxr * sfx, int freq);
	void bmx_sfxr_generatesound(Sfxr * sfx, float * data, int length, int dataType, int freq);
	void bmx_sfxr_randomize(Sfxr * sfx);
	void bmx_sfxr_mutate(Sfxr * sfx);

}


class Sfxr
{

	int wave_type;
	
	float p_base_freq;
	float p_freq_limit;
	float p_freq_ramp;
	float p_freq_dramp;
	float p_duty;
	float p_duty_ramp;
	
	float p_vib_strength;
	float p_vib_speed;
	float p_vib_delay;
	
	float p_env_attack;
	float p_env_sustain;
	float p_env_decay;
	float p_env_punch;
	
	bool filter_on;
	float p_lpf_resonance;
	float p_lpf_freq;
	float p_lpf_ramp;
	float p_hpf_freq;
	float p_hpf_ramp;
	
	float p_pha_offset;
	float p_pha_ramp;
	
	float p_repeat_speed;
	
	float p_arp_speed;
	float p_arp_mod;
	
	float master_vol;//=0.05f;
	
	float sound_vol;//=0.5f;
	
	
	bool playing_sample;//=false;
	int phase;
	double fperiod;
	double fmaxperiod;
	double fslide;
	double fdslide;
	int period;
	float square_duty;
	float square_slide;
	int env_stage;
	int env_time;
	int env_length[3];
	float env_vol;
	float fphase;
	float fdphase;
	int iphase;
	float phaser_buffer[1024];
	int ipp;
	float noise_buffer[32];
	float fltp;
	float fltdp;
	float fltw;
	float fltw_d;
	float fltdmp;
	float fltphp;
	float flthp;
	float flthp_d;
	float vib_phase;
	float vib_speed;
	float vib_amp;
	int rep_time;
	int rep_limit;
	int arp_time;
	int arp_limit;
	double arp_mod;
	
	float* vselected;
	int vcurbutton;//=-1;
	
	int wav_bits;//=16;
	int wav_freq;//=44100;
	
	int file_sampleswritten;
	float filesample;//=0.0f;
	int fileacc;//=0;

public:
	Sfxr()
		: master_vol(0.5f), sound_vol(0.5f), playing_sample(false), vselected(0), vcurbutton(-1), wav_bits(16), wav_freq(44100), filesample(0.0f), fileacc(0)
	{
	
	}

	float frnd(float range)
	{
		return (float)rnd(10000)/10000*range;
	}


	void ResetSample(bool restart)
	{
		if(!restart)
			phase=0;
		fperiod=100.0/(p_base_freq*p_base_freq+0.001);
		period=(int)fperiod;
		fmaxperiod=100.0/(p_freq_limit*p_freq_limit+0.001);
		fslide=1.0-pow((double)p_freq_ramp, 3.0)*0.01;
		fdslide=-pow((double)p_freq_dramp, 3.0)*0.000001;
		square_duty=0.5f-p_duty*0.5f;
		square_slide=-p_duty_ramp*0.00005f;
		if(p_arp_mod>=0.0f)
			arp_mod=1.0-pow((double)p_arp_mod, 2.0)*0.9;
		else
			arp_mod=1.0+pow((double)p_arp_mod, 2.0)*10.0;
		arp_time=0;
		arp_limit=(int)(pow(1.0f-p_arp_speed, 2.0f)*20000+32);
		if(p_arp_speed==1.0f)
			arp_limit=0;
		if(!restart)
		{
			// reset filter
			fltp=0.0f;
			fltdp=0.0f;
			fltw=pow(p_lpf_freq, 3.0f)*0.1f;
			fltw_d=1.0f+p_lpf_ramp*0.0001f;
			fltdmp=5.0f/(1.0f+pow(p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
			if(fltdmp>0.8f) fltdmp=0.8f;
			fltphp=0.0f;
			flthp=pow(p_hpf_freq, 2.0f)*0.1f;
			flthp_d=1.0+p_hpf_ramp*0.0003f;
			// reset vibrato
			vib_phase=0.0f;
			vib_speed=pow(p_vib_speed, 2.0f)*0.01f;
			vib_amp=p_vib_strength*0.5f;
			// reset envelope
			env_vol=0.0f;
			env_stage=0;
			env_time=0;
			env_length[0]=(int)(p_env_attack*p_env_attack*100000.0f);
			env_length[1]=(int)(p_env_sustain*p_env_sustain*100000.0f);
			env_length[2]=(int)(p_env_decay*p_env_decay*100000.0f);
	
			fphase=pow(p_pha_offset, 2.0f)*1020.0f;
			if(p_pha_offset<0.0f) fphase=-fphase;
			fdphase=pow(p_pha_ramp, 2.0f)*1.0f;
			if(p_pha_ramp<0.0f) fdphase=-fdphase;
			iphase=abs((int)fphase);
			ipp=0;
			for(int i=0;i<1024;i++)
				phaser_buffer[i]=0.0f;
	
			for(int i=0;i<32;i++)
				noise_buffer[i]=frnd(2.0f)-1.0f;
	
			rep_time=0;
			rep_limit=(int)(pow(1.0f-p_repeat_speed, 2.0f)*20000+32);
			if(p_repeat_speed==0.0f)
				rep_limit=0;
		}
	}


	int SynthSample(int length, float* buffer) {
		for(int i=0;i<length;i++)
		{

			if(!playing_sample)
				return i;
	
			rep_time++;
			if(rep_limit!=0 && rep_time>=rep_limit)
			{
				rep_time=0;
				ResetSample(true);
			}
	
			// frequency envelopes/arpeggios
			arp_time++;
			if(arp_limit!=0 && arp_time>=arp_limit)
			{
				arp_limit=0;
				fperiod*=arp_mod;
			}
			fslide+=fdslide;
			fperiod*=fslide;
			if(fperiod>fmaxperiod)
			{
				fperiod=fmaxperiod;
				if(p_freq_limit>0.0f) {
					playing_sample=false;
				}
			}
			float rfperiod=fperiod;
			if(vib_amp>0.0f)
			{
				vib_phase+=vib_speed;
				rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
			}
			period=(int)rfperiod;
			if(period<8) period=8;
			square_duty+=square_slide;
			if(square_duty<0.0f) square_duty=0.0f;
			if(square_duty>0.5f) square_duty=0.5f;		
			// volume envelope
			env_time++;
			if(env_time>env_length[env_stage])
			{
				env_time=0;
				env_stage++;
				if(env_stage==3) {
					playing_sample=false;
				}
			}
			if(env_stage==0)
				env_vol=(float)env_time/env_length[0];
			if(env_stage==1)
				env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*p_env_punch;
			if(env_stage==2)
				env_vol=1.0f-(float)env_time/env_length[2];
	
			// phaser step
			fphase+=fdphase;
			iphase=abs((int)fphase);
			if(iphase>1023) iphase=1023;
	
			if(flthp_d!=0.0f)
			{
				flthp*=flthp_d;
				if(flthp<0.00001f) flthp=0.00001f;
				if(flthp>0.1f) flthp=0.1f;
			}
	
			float ssample=0.0f;
			for(int si=0;si<8;si++) // 8x supersampling
			{
				float sample=0.0f;
				phase++;
				if(phase>=period)
				{
					phase%=period;
					if(wave_type==3)
						for(int i=0;i<32;i++)
							noise_buffer[i]=frnd(2.0f)-1.0f;
				}
				// base waveform
				float fp=(float)phase/period;
				switch(wave_type)
				{
				case 0: // square
					if(fp<square_duty)
						sample=0.5f;
					else
						sample=-0.5f;
					break;
				case 1: // sawtooth
					sample=1.0f-fp*2;
					break;
				case 2: // sine
					sample=(float)sin(fp*2*PI);
					break;
				case 3: // noise
					sample=noise_buffer[phase*32/period];
					break;
				}
				// lp filter
				float pp=fltp;
				fltw*=fltw_d;
				if(fltw<0.0f) fltw=0.0f;
				if(fltw>0.1f) fltw=0.1f;
				if(p_lpf_freq!=1.0f)
				{
					fltdp+=(sample-fltp)*fltw;
					fltdp-=fltdp*fltdmp;
				}
				else
				{
					fltp=sample;
					fltdp=0.0f;
				}
				fltp+=fltdp;
				// hp filter
				fltphp+=fltp-pp;
				fltphp-=fltphp*flthp;
				sample=fltphp;
				// phaser
				phaser_buffer[ipp&1023]=sample;
				sample+=phaser_buffer[(ipp-iphase+1024)&1023];
				ipp=(ipp+1)&1023;
				// final accumulation and envelope application
				ssample+=sample*env_vol;
			}
			ssample=ssample/8*master_vol;
	
			ssample*=2.0f*sound_vol;
	
			if(buffer!=NULL)
			{
				if(ssample>1.0f) ssample=1.0f;
				if(ssample<-1.0f) ssample=-1.0f;
				*buffer++=ssample;
			}
		}
		
		return length;
	}


	void ResetParams()
	{
		wave_type=0;
	
		p_base_freq=0.3f;
		p_freq_limit=0.0f;
		p_freq_ramp=0.0f;
		p_freq_dramp=0.0f;
		p_duty=0.0f;
		p_duty_ramp=0.0f;
	
		p_vib_strength=0.0f;
		p_vib_speed=0.0f;
		p_vib_delay=0.0f;
	
		p_env_attack=0.0f;
		p_env_sustain=0.3f;
		p_env_decay=0.4f;
		p_env_punch=0.0f;
	
		filter_on=false;
		p_lpf_resonance=0.0f;
		p_lpf_freq=1.0f;
		p_lpf_ramp=0.0f;
		p_hpf_freq=0.0f;
		p_hpf_ramp=0.0f;
		
		p_pha_offset=0.0f;
		p_pha_ramp=0.0f;
	
		p_repeat_speed=0.0f;
	
		p_arp_speed=0.0f;
		p_arp_mod=0.0f;
	}

	BBObject * GetSettings(BBObject * settings) {
	
		return _bah_sfxr_TSfxrSettings__create(settings, wave_type, sound_vol, p_base_freq, p_freq_limit, p_freq_ramp, p_freq_dramp, 
			p_duty, p_duty_ramp, p_vib_strength, p_vib_speed, p_vib_delay, p_env_attack, p_env_sustain, 
			p_env_decay, p_env_punch, filter_on, p_lpf_resonance, p_lpf_freq, p_lpf_ramp, p_hpf_freq, 
			p_hpf_ramp, p_pha_offset, p_pha_ramp, p_repeat_speed, p_arp_speed, p_arp_mod);

	}
	
	void SetSettings(int wave_typei, float sound_voli, float p_base_freqi, float p_freq_limiti, float p_freq_rampi, float p_freq_drampi, 
			float p_dutyi, float p_duty_rampi, float p_vib_strengthi, float p_vib_speedi, float p_vib_delayi, float p_env_attacki, float p_env_sustaini, 
			float p_env_decayi, float p_env_punchi, int filter_oni, float p_lpf_resonancei, float p_lpf_freqi, float p_lpf_rampi, float p_hpf_freqi, 
			float p_hpf_rampi, float p_pha_offseti, float p_pha_rampi, float p_repeat_speedi, float p_arp_speedi, float p_arp_modi) {
		
		wave_type = wave_typei;
		sound_vol = sound_voli;
		p_base_freq = p_base_freqi;
		p_freq_limit = p_freq_limiti;
		p_freq_ramp = p_freq_rampi;
		p_freq_dramp = p_freq_drampi;
		p_duty = p_dutyi;
		p_duty_ramp = p_duty_rampi;
		p_vib_strength = p_vib_strengthi;
		p_vib_speed = p_vib_speedi;
		p_vib_delay = p_vib_delayi;
		p_env_attack = p_env_attacki;
		p_env_sustain = p_env_sustaini;
		p_env_decay = p_env_decayi;
		p_env_punch = p_env_punchi;
		filter_on = filter_oni;
		p_lpf_resonance = p_lpf_resonancei;
		p_lpf_freq = p_lpf_freqi;
		p_lpf_ramp = p_lpf_rampi;
		p_hpf_freq = p_hpf_freqi;
		p_hpf_ramp = p_hpf_rampi;
		p_pha_offset = p_pha_offseti;
		p_pha_ramp = p_pha_rampi;
		p_repeat_speed = p_repeat_speedi;
		p_arp_speed = p_arp_speedi;
		p_arp_mod = p_arp_modi;

	
	}

	void Randomize() {
		p_base_freq=pow(frnd(2.0f)-1.0f, 2.0f);
		if(rnd(1))
			p_base_freq=pow(frnd(2.0f)-1.0f, 3.0f)+0.5f;
		p_freq_limit=0.0f;
		p_freq_ramp=pow(frnd(2.0f)-1.0f, 5.0f);
		if(p_base_freq>0.7f && p_freq_ramp>0.2f)
			p_freq_ramp=-p_freq_ramp;
		if(p_base_freq<0.2f && p_freq_ramp<-0.05f)
			p_freq_ramp=-p_freq_ramp;
		p_freq_dramp=pow(frnd(2.0f)-1.0f, 3.0f);
		p_duty=frnd(2.0f)-1.0f;
		p_duty_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
		p_vib_strength=pow(frnd(2.0f)-1.0f, 3.0f);
		p_vib_speed=frnd(2.0f)-1.0f;
		p_vib_delay=frnd(2.0f)-1.0f;
		p_env_attack=pow(frnd(2.0f)-1.0f, 3.0f);
		p_env_sustain=pow(frnd(2.0f)-1.0f, 2.0f);
		p_env_decay=frnd(2.0f)-1.0f;
		p_env_punch=pow(frnd(0.8f), 2.0f);
		if(p_env_attack+p_env_sustain+p_env_decay<0.2f)
		{
			p_env_sustain+=0.2f+frnd(0.3f);
			p_env_decay+=0.2f+frnd(0.3f);
		}
		p_lpf_resonance=frnd(2.0f)-1.0f;
		p_lpf_freq=1.0f-pow(frnd(1.0f), 3.0f);
		p_lpf_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
		if(p_lpf_freq<0.1f && p_lpf_ramp<-0.05f)
			p_lpf_ramp=-p_lpf_ramp;
		p_hpf_freq=pow(frnd(1.0f), 5.0f);
		p_hpf_ramp=pow(frnd(2.0f)-1.0f, 5.0f);
		p_pha_offset=pow(frnd(2.0f)-1.0f, 3.0f);
		p_pha_ramp=pow(frnd(2.0f)-1.0f, 3.0f);
		p_repeat_speed=frnd(2.0f)-1.0f;
		p_arp_speed=frnd(2.0f)-1.0f;
		p_arp_mod=frnd(2.0f)-1.0f;
	}

	void Mutate() {
		if(rnd(1)) p_base_freq+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_freq_ramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_freq_dramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_duty+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_duty_ramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_vib_strength+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_vib_speed+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_vib_delay+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_env_attack+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_env_sustain+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_env_decay+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_env_punch+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_lpf_resonance+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_lpf_freq+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_lpf_ramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_hpf_freq+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_hpf_ramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_pha_offset+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_pha_ramp+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_repeat_speed+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_arp_speed+=frnd(0.1f)-0.05f;
		if(rnd(1)) p_arp_mod+=frnd(0.1f)-0.05f;
	}

	void PlaySample()
	{
		ResetSample(false);
		playing_sample=true;
	}
	

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Sfxr * bmx_sfxr_new() {
	return new Sfxr();
}

BBObject * bmx_sfxr_getsettings(Sfxr * sfx, BBObject * settings) {
	return sfx->GetSettings(settings);
}

void bmx_sfxr_setsettings(Sfxr * sfx, int wave_type, float sound_vol, float p_base_freq, float p_freq_limit, float p_freq_ramp, float p_freq_dramp, 
		float p_duty, float p_duty_ramp, float p_vib_strength, float p_vib_speed, float p_vib_delay, float p_env_attack, float p_env_sustain, 
		float p_env_decay, float p_env_punch, int filter_on, float p_lpf_resonance, float p_lpf_freq, float p_lpf_ramp, float p_hpf_freq, 
		float p_hpf_ramp, float p_pha_offset, float p_pha_ramp, float p_repeat_speed, float p_arp_speed, float p_arp_mod) {

	sfx->SetSettings(wave_type, sound_vol, p_base_freq, p_freq_limit, p_freq_ramp, p_freq_dramp, 
			p_duty, p_duty_ramp, p_vib_strength, p_vib_speed, p_vib_delay, p_env_attack, p_env_sustain, 
			p_env_decay, p_env_punch, filter_on, p_lpf_resonance, p_lpf_freq, p_lpf_ramp, p_hpf_freq, 
			p_hpf_ramp, p_pha_offset, p_pha_ramp, p_repeat_speed, p_arp_speed, p_arp_mod);

}

int bmx_sfxr_calculatesize(Sfxr * sfx, int freq) {
	int size;
	float buffer[512];
	
	sfx->ResetSample(true);
	sfx->PlaySample();
	
	int block = sfx->SynthSample(512, buffer);
	size = block;
	
	while (block > 0) {
		block = sfx->SynthSample(512, buffer);
		size += block;
	}

	return size;
}

void bmx_sfxr_generatesound(Sfxr * sfx, float * data, int length, int soundType, int freq) {
	float buffer[length];

	sfx->ResetSample(true);
	sfx->PlaySample();
	int i = sfx->SynthSample(length, buffer);
	
	int l = length;
	int div = 1;
	
	switch (freq) {
		case 22050:
			div = 2;
			break;
		default:
			div = 1;
			break;
	}
	
	float sample = 0.0f;
	int acc = 0;
	
	while (l--) {
	
		float f = buffer[l];
		if (f < -1.0) f = -1.0;
		if (f > 1.0) f = 1.0;

		sample += f;
		
		if (div > 1 && ++acc < div) {
			continue;
		}
		
		sample /= div;
		acc = 0;
	
		switch (soundType) {
			case 0:
				data[l / div] = sample;
				break;
			case 1:
				((short*)data)[l / div] = (short)(sample * 32767);
				break;
			case 2:
				((unsigned char*)data)[l / div] = (unsigned char)(sample*127+128);
				break;
		}
		
		sample = 0.0f;
	}


}

void bmx_sfxr_randomize(Sfxr * sfx) {
	sfx->Randomize();
}

void bmx_sfxr_mutate(Sfxr * sfx) {
	sfx->Mutate();
}

void bmx_sfxr_resetparams(Sfxr * sfx) {
	sfx->ResetParams();
}

