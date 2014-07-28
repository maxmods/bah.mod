/*
 Copyright (c) 2014 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "dtl.hpp"

extern "C" {

#include "blitz.h"
	
	dtl::Diff<char, std::string> * bmx_diff_screate(BBString * a, BBString * b);
	void bmx_diff_sfree(dtl::Diff<char, std::string> * diff);
	void bmx_diff_scompose(dtl::Diff<char, std::string> * diff);
	void bmx_diff_scomposeUnifiedHunks(dtl::Diff<char, std::string> * diff);
	void bmx_diff_sprintUnifiedFormat(dtl::Diff<char, std::string> * diff);

	dtl::Diff<std::string, std::vector<std::string> > * bmx_diff_lcreate(BBArray * a, BBArray * b);
	void bmx_diff_lfree(dtl::Diff<std::string, std::vector<std::string> > * diff);
	void bmx_diff_lcompose(dtl::Diff<std::string, std::vector<std::string> > * diff);
	void bmx_diff_lcomposeUnifiedHunks(dtl::Diff<std::string, std::vector<std::string> > * diff);
	void bmx_diff_lprintUnifiedFormat(dtl::Diff<std::string, std::vector<std::string> > * diff);
}


dtl::Diff<char, std::string> * bmx_diff_screate(BBString * a, BBString * b) {
	char * as = bbStringToUTF8String(a);
	char * bs = bbStringToUTF8String(b);
	
	dtl::Diff<char, std::string> * diff = new dtl::Diff<char, std::string>(as, bs);
	
	bbMemFree(as);
	bbMemFree(bs);
	
	return diff;
}

void bmx_diff_sfree(dtl::Diff<char, std::string> * diff) {
	delete diff;
}

void bmx_diff_scompose(dtl::Diff<char, std::string> * diff) {
	diff->compose();
}

void bmx_diff_scomposeUnifiedHunks(dtl::Diff<char, std::string> * diff) {
	diff->composeUnifiedHunks();
}

void bmx_diff_sprintUnifiedFormat(dtl::Diff<char, std::string> * diff) {
	diff->printUnifiedFormat();
}

std::vector<std::string> bmx_diff_array_to_vector(BBArray * a) {
	int n=a->scales[0];
	BBString **s=(BBString**)BBARRAYDATA( a,a->dims );
	std::vector<std::string> v;

	for( int i=0;i<n;++i ){
		char * as = bbStringToUTF8String(s[i]);
		v.push_back(as);
		bbMemFree(as);
	}
	
	return v;
}

dtl::Diff<std::string, std::vector<std::string> > * bmx_diff_lcreate(BBArray * a, BBArray * b) {

	std::vector<std::string> va = bmx_diff_array_to_vector(a);
	std::vector<std::string> vb = bmx_diff_array_to_vector(b);
	
	return new dtl::Diff<std::string, std::vector<std::string> >(va, vb);
}

void bmx_diff_lfree(dtl::Diff<std::string, std::vector<std::string> > * diff) {
	delete diff;
}

void bmx_diff_lcompose(dtl::Diff<std::string, std::vector<std::string> > * diff) {
	diff->compose();
}

void bmx_diff_lcomposeUnifiedHunks(dtl::Diff<std::string, std::vector<std::string> > * diff) {
	diff->composeUnifiedHunks();
}

void bmx_diff_lprintUnifiedFormat(dtl::Diff<std::string, std::vector<std::string> > * diff) {
	diff->printUnifiedFormat();
}
