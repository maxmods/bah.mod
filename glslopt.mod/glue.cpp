/*
  Copyright (c) 2014 Bruce A Henderson
 
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
#include "glsl_optimizer.h"

extern "C" {

#include "blitz.h"

	glslopt_ctx * bmx_glslopt_initialize(int target);
	void bmx_glslopt_cleanup(glslopt_ctx * ctx);
	void bmx_glslopt_set_max_unroll_iterations(glslopt_ctx * ctx, int iterations);
	glslopt_shader * bmx_glslopt_optimize(glslopt_ctx * ctx, int shaderType, BBString * source, int options);

	int bmx_glslopt_get_status(glslopt_shader * shader);
	BBString * bmx_glslopt_get_output(glslopt_shader * shader);
	BBString * bmx_glslopt_get_raw_output(glslopt_shader * shader);
	BBString * bmx_glslopt_get_log(glslopt_shader * shader);
	int bmx_glslopt_shader_get_input_count(glslopt_shader * shader);
	BBString * bmx_glslopt_shader_get_input_name(glslopt_shader * shader, int index);
	void bmx_glslopt_shader_get_stats(glslopt_shader * shader, int * approxMath, int * approxTex, int * approxFlow);
	void bmx_glslopt_shader_delete(glslopt_shader * shader);

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

glslopt_ctx * bmx_glslopt_initialize(int target) {
	return glslopt_initialize(static_cast<glslopt_target>(target));
}

void bmx_glslopt_cleanup(glslopt_ctx * ctx) {
	glslopt_cleanup(ctx);
}

void bmx_glslopt_set_max_unroll_iterations(glslopt_ctx * ctx, int iterations) {
	glslopt_set_max_unroll_iterations(ctx, iterations);
}

glslopt_shader * bmx_glslopt_optimize(glslopt_ctx * ctx, int shaderType, BBString * source, int options) {
	char * s = bbStringToCString(source);
	glslopt_shader * shader = glslopt_optimize(ctx, static_cast<glslopt_shader_type>(shaderType), s, options);
	bbMemFree(s);
	return shader;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_glslopt_get_status(glslopt_shader * shader) {
	return static_cast<int>(glslopt_get_status(shader));
}

BBString * bmx_glslopt_get_output(glslopt_shader * shader) {
	return bbStringFromCString(glslopt_get_output(shader));
}

BBString * bmx_glslopt_get_raw_output(glslopt_shader * shader) {
	return bbStringFromCString(glslopt_get_raw_output(shader));
}

BBString * bmx_glslopt_get_log(glslopt_shader * shader) {
	return bbStringFromCString(glslopt_get_log(shader));
}

int bmx_glslopt_shader_get_input_count(glslopt_shader * shader) {
	return glslopt_shader_get_input_count(shader);
}

BBString * bmx_glslopt_shader_get_input_name(glslopt_shader * shader, int index) {
	return bbStringFromCString(glslopt_shader_get_input_name(shader, index));
}

void bmx_glslopt_shader_get_stats(glslopt_shader * shader, int * approxMath, int * approxTex, int * approxFlow) {
	glslopt_shader_get_stats(shader, approxMath, approxTex, approxFlow);
}

void bmx_glslopt_shader_delete(glslopt_shader * shader) {
	glslopt_shader_delete(shader);
}

