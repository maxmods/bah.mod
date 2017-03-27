/*
  Copyright 2017 Bruce A Henderson
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
      http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include "guetzli/processor.h"
#include "guetzli/quality.h"
#include "guetzli/stats.h"

extern "C" {

	int bmx_guetzli_savejpg(void *stream, int (*writefunc)(const void*, int, void*), int width, int height, int pitch, char *pix, int quality);
	
}


int bmx_guetzli_savejpg(void *stream, int (*writefunc)(const void*, int, void*), int width, int height, int pitch, char *pix, int quality) {

	std::vector<uint8_t> rgb;
	rgb.resize(3 * width * height);

	for (int y = 0; y < height; ++y) {
		uint8_t * row_out = &rgb[3 * y * width];
		memcpy(row_out, pix, 3 * width);
		pix += pitch;
	}

	std::string outData;

	guetzli::Params params;
	params.butteraugli_target = guetzli::ButteraugliScoreForQuality(quality);
	
	guetzli::ProcessStats stats;
	
	if (!guetzli::Process(params, &stats, rgb, width, height, &outData)) {
		return 1;
	}
	
	writefunc(outData.data(), outData.size(), stream);
	
	return 0;
}

