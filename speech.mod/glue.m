/*
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


#import <AppKit/AppKit.h>

#include <blitz.h>


// --------------------------------------------------------


NSSpeechSynthesizer * bmx_speech_new() {
	return [[NSSpeechSynthesizer alloc] init];

}

int bmx_speech_speak(NSSpeechSynthesizer *synth, BBString * text) {
	return [synth startSpeakingString:NSStringFromBBString(text)];
}

int bmx_speech_isSpeaking(NSSpeechSynthesizer *synth) {
	return [synth isSpeaking];
}

void bmx_speech_free(NSSpeechSynthesizer *synth) {
	[synth release];
}

BBArray * bmx_speech_availableVoices() {
	NSArray * arr = [NSSpeechSynthesizer availableVoices];

	int n = [arr count];

	BBArray *p = bbArrayNew1D( "$", n);
	BBString **s = (BBString**)BBARRAYDATA(p, p->dims);
	
	NSEnumerator * ae = [arr objectEnumerator];
	NSString * text;
	int i = 0;
	while (text = [ae nextObject]) {
		s[i] = bbStringFromNSString(text);
		BBRETAIN( s[i] );
		i++;
	}
	return p;
}

void bmx_speech_setVoice(NSSpeechSynthesizer *synth, BBString * voice) {
	[synth setVoice:NSStringFromBBString(voice)];
}

void bmx_speech_pause(NSSpeechSynthesizer *synth) {
	[synth pauseSpeakingAtBoundary:NSSpeechWordBoundary];
}

void bmx_speech_resume(NSSpeechSynthesizer *synth) {
	[synth continueSpeaking];
}

void bmx_speech_setVolume(NSSpeechSynthesizer *synth, float v) {
	[synth setVolume:v];
}

float bmx_speech_getVolume(NSSpeechSynthesizer *synth) {
	return [synth volume];
}

