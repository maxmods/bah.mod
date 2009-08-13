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

// TODO :
// TCODMap conversion to/from TCODImage and TCODHeightMap
// variables support in TCODParser

class TCODLIB_API TCODDungeon {
public :
	TCODDungeon(int width, int height);
	
	// high level API
	void setVariable(const char *name, const char *value);
	void load(const char *filename);
	void generate(TCODRandom *rng, TCODMap *map);

    // nodes internal API
	// constructor nodes
	void bsp(int depth, int minHSize, int minVSize, float minHRatio, float minVRatio, bool roomWalls, bool randomRooms);
	void copy(const TCODDungeon *source);
	void random(int wallPercentage);
	void morph(const TCODDungeon *source1, const TCODDungeon *source2, float coef);
	void add(const TCODDungeon *source1, const TCODDungeon *source2);
	void multiply(const TCODDungeon *source1, const TCODDungeon *source2);
	
	// image nodes
	void invert();
	void rotate(float angleInRadians);
	void scale (float coefx, float coefy);
	void translate(int dx, int dy);
	
	// heightmap nodes
	void noise();
	void fbm();
	void turbulence();
	void voronoi();
	void filter();
	// ...
	
	// cellular automata nodes
	void cave(int loops);
	void cave2(int loops);
	void dig(int loops);
	void roundCorners(int loops);
	void removeInnerWalls(int loops);
	void clearIsolatedWalls(int loops);
	void seal();
	void connect();
	
	// custom CA functions
	void customFunc( bool (*cafunc)(TCODDungeon *dungeon, int x, int y, void *userData), int loops, void *userData );
	bool isWall(int x, int y);
	void setWall(int x,int y, bool wall);
	int countWalls(int x, int y, int range);
protected :
	TCODParser parser;
};
