/* Copyright (c) 2016 Bruce A Henderson

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

     1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

     2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

     3. This notice may not be removed or altered from any source
     distribution.
*/

#include "emb-format.h"
#include "emb-thread.h"
#include "emb-point.h"
#include "emb-stitch.h"
#include "emb-pattern.h"
#include "emb-flag.h"

EmbFormatList * bmx_libembroidery_EmbFormatList_next(EmbFormatList * list) {
	return list->next;
}


void bmx_libembroidery_EmbThreadList_thread(EmbThreadList * list, EmbThread * data) {
	*data = list->thread;
}

void bmx_libembroidery_EmbThreadList_getat(EmbThreadList * list, int index, EmbThread * data) {
	*data = embThreadList_getAt(list, index);
}

EmbThreadList * bmx_libembroidery_EmbThreadList_next(EmbThreadList * list) {
	return list->next;
}

void bmx_libembroidery_EmbPointList_point(EmbPointList * list, EmbPoint * data) {
	*data = list->point;
}

EmbPointList * bmx_libembroidery_EmbPointList_next(EmbPointList * list) {
	return list->next;
}

void bmx_libembroidery_EmbStitchList_stitch(EmbStitchList * list, EmbStitch * data) {
	*data = list->stitch;
}

EmbStitchList * bmx_libembroidery_EmbStitchList_next(EmbStitchList * list) {
	return list->next;
}

EmbStitchList * bmx_libembroidery_EmbPattern_stitchlist(EmbPattern * pattern) {
	return pattern->stitchList;
}

EmbThreadList * bmx_libembroidery_EmbPattern_threadlist(EmbPattern * pattern) {
	return pattern->threadList;
}

int bmx_libembroidery_EmbFlagList_flag(EmbFlagList * list) {
	return list->flag;
}

EmbFlagList * bmx_libembroidery_EmbFlagList_next(EmbFlagList * list) {
	return list->next;
}

int bmx_libembroidery_EmbPattern_currentcolorindex(EmbPattern * pattern) {
	return pattern->currentColorIndex;
}

