
#include <stdio.h>
#include <string.h>

#include "defines.h"


#define USE_FILTER

#define FLAG_N 128
#define FLAG_V 64
#define FLAG_B 16
#define FLAG_D 8
#define FLAG_I 4
#define FLAG_Z 2
#define FLAG_C 1


extern "C" {


enum { 
  adc, _and, asl, bcc, bcs, beq, bit, bmi, bne, bpl, brk, bvc, bvs, clc,
  cld, cli, clv, cmp, cpx, cpy, dec, dex, dey, eor, inc, inx, iny, jmp,
  jsr, lda, ldx, ldy, lsr, nop, ora, pha, php, pla, plp, rol, ror, rti,
  rts, sbc, sec, sed, sei, sta, stx, sty, tax, tay, tsx, txa, txs, tya,
  xxx};


// SID register definition

struct s6581 {
                struct sidvoice {
                         word freq;
                         word pulse;
                         byte wave;
                         byte ad;
                         byte sr;
                       } v[3];
                byte ffreqlo;
                byte ffreqhi;
                byte res_ftv;
                byte ftp_vol;
              };

// internal oscillator def

struct sidosc {
                dword freq;
                dword pulse;
                byte wave;
                byte filter;
                dword attack;
                dword decay;
                dword sustain;
                dword release;
                dword       counter;  // Variablen
                signed int envval;
                byte        envphase;
                dword       noisepos;
                dword       noiseval;
                byte        noiseout;
              };

// internal filter def

struct sidflt {
                int freq;
                byte  l_ena;
                byte  b_ena;
                byte  h_ena;
                byte  v3ena;
                int vol;
                int rez;
                int h;
                int b;
                int l;
               };


// ------------------------------------------------------------- constants

static float attackTimes[16] =
{
  0.0022528606, 0.0080099577, 0.0157696042, 0.0237795619, 0.0372963655,
  0.0550684591,0.0668330845, 0.0783473987, 0.0981219818, 0.244554021,
  0.489108042, 0.782472742, 0.977715461, 2.93364701, 4.88907793, 7.82272493
};
static float decayReleaseTimes[16] =
{
  0.00891777693, 0.024594051, 0.0484185907, 0.0730116639, 0.114512475,
  0.169078356, 0.205199432, 0.240551975, 0.301266125, 0.750858245,
  1.50171551, 2.40243682, 3.00189298, 9.00721405, 15.010998, 24.0182111
};

#define imp 0
#define imm 1
#define abs 2
#define absx 3
#define absy 4
#define zp 6
#define zpx 7
#define zpy 8
#define ind 9
#define indx 10
#define indy 11
#define acc 12
#define rel 13


}


#include <math.h>




class SIDEngine
{
public:

	// --------------------------- pseudo-constants (depending on mixing freq)
	
	int  mixing_frequency;
	int  freqmul;
	int  filtmul;
	int  attacks [16];
	int  releases[16];
	
	// --------------------------------------------------------------- globals
	struct s6581 sid;
	struct sidosc osc[3];
	struct sidflt filter;

	byte memory[65536];  /* The C64 memory */

	int sample_active;
	int sample_position, sample_start, sample_end, sample_repeat_start;
	int fracPos;		/* Fractal position of sample */
	int sample_period;
	int sample_repeats;
	int sample_order;
	int sample_nibble;

	int internal_period, internal_order, internal_start, internal_end,
	internal_add, internal_repeat_times, internal_repeat_start;

	int cycles;
	byte bval;
	word wval;
	
	byte a,x,y,s,p;
	word pc;
	float filterl1,filterb1,freq,filterrez;

	SIDEngine()
		: filterl1(0), filterb1(0), freq(0.8), filterrez(0.1), fracPos(0)
	{
	}


	inline int GenerateDigi(int sIn);

	// initialize SID and frequency dependant values
	
	void synth_init(dword mixfrq);
	
	// render a buffer of n samples with the actual register contents
	void synth_render (word *buffer, dword len);

	void sidPoke(int reg, unsigned char val);
	
	void sidReset(void)
	{
	}
	
	byte getmem(word addr);

	void setmem(word addr, byte value);

	byte getaddr(int mode);
	
	void setaddr(int mode, byte val);
	
	void putaddr(int mode, byte val);
		
	inline void setflags(int flag, int cond);
	
	inline void push(byte val);
		
	inline byte pop();
	
	void branch(int flag);

	void cpuReset();
	
	void cpuResetTo(word npc);
		
	inline void cpuParse();
	
	int cpuJSR(word npc, byte na);
	
	void c64Init(void);
	
	unsigned short LoadSIDFromMemory(void *pSidData, unsigned short *load_addr,
	                                 unsigned short *init_addr, unsigned short *play_addr, unsigned char *subsongs, unsigned char *startsong, unsigned char *speed, unsigned short size);
	
	word c64SidLoad(unsigned char *data, int size, word *init_addr, word *play_addr, byte *sub_song_start, byte *max_sub_songs, byte *speed, char *name, char *author, char *copyright);

};


