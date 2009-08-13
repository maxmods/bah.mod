/*
* libtcod 1.5.0
* Copyright (c) 2008,2009 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

typedef void *TCOD_dungeon_t;

TCODLIB_API TCOD_dungeon_t TCOD_dungeon_new(int width, int height);

// high level API
TCODLIB_API void TCOD_dungeon_set_variable(TCOD_dungeon_t dungeon, const char *name, const char *value);
TCODLIB_API void TCOD_dungeon_load(TCOD_dungeon_t dungeon, const char *filename);
TCODLIB_API void TCOD_dungeon_generate(TCOD_dungeon_t dungeon, TCOD_random_t rng, TCOD_map_t map);

// nodes internal API
// constructor nodes
TCODLIB_API void TCOD_dungeon_bsp(TCOD_dungeon_t dungeon, int depth, int minHSize, int minVSize, float minHRatio, float minVRatio, bool roomWalls, bool randomRooms);
TCODLIB_API void TCOD_dungeon_copy(TCOD_dungeon_t dungeon, const TCOD_dungeon_t *source);
TCODLIB_API void TCOD_dungeon_random(TCOD_dungeon_t dungeon, int wallPercentage);
TCODLIB_API void TCOD_dungeon_morph(TCOD_dungeon_t dungeon, const TCOD_dungeon_t *source1, const TCOD_dungeon_t *source2, float coef);
TCODLIB_API void TCOD_dungeon_add(TCOD_dungeon_t dungeon, const TCOD_dungeon_t *source1, const TCOD_dungeon_t *source2);
TCODLIB_API void TCOD_dungeon_multiply(TCOD_dungeon_t dungeon, const TCOD_dungeon_t *source1, const TCOD_dungeon_t *source2);

// image nodes
TCODLIB_API void TCOD_dungeon_invert(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_rotate(TCOD_dungeon_t dungeon, float angleInRadians);
TCODLIB_API void TCOD_dungeon_scale (TCOD_dungeon_t dungeon, float coefx, float coefy);
TCODLIB_API void TCOD_dungeon_translate(TCOD_dungeon_t dungeon, int dx, int dy);

// heightmap nodes
TCODLIB_API void TCOD_dungeon_noise(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_fbm(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_turbulence(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_voronoi(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_filter(TCOD_dungeon_t dungeon);
// ...

// cellular automata nodes
TCODLIB_API void TCOD_dungeon_cave(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_cave2(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_dig(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_roundCorners(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_removeInnerWalls(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_clearIsolatedWalls(TCOD_dungeon_t dungeon, int loops);
TCODLIB_API void TCOD_dungeon_seal(TCOD_dungeon_t dungeon);
TCODLIB_API void TCOD_dungeon_connect(TCOD_dungeon_t dungeon);

// custom CA functions
TCODLIB_API void TCOD_dungeon_customFunc(TCOD_dungeon_t dungeon,  bool (*cafunc)(TCOD_dungeon_t *dungeon, int x, int y, void *userData), int loops, void *userData );
TCODLIB_API bool TCOD_dungeon_isWall(TCOD_dungeon_t dungeon, int x, int y);
TCODLIB_API void TCOD_dungeon_setWall(TCOD_dungeon_t dungeon, int x,int y, bool wall);
TCODLIB_API int TCOD_dungeon_countWalls(TCOD_dungeon_t dungeon, int x, int y, int range);
