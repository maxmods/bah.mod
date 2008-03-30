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

#include <blitz.h>
#include "Box2D.h"

class MaxDebugDraw;

extern "C" {

	BBArray * _bah_box2d_b2Vec2__newVecArray(int count);
	void _bah_box2d_b2Vec2__setVec(BBArray * array, int index, b2Vec2 * vec);

	void _bah_box2d_b2DebugDraw__DrawPolygon(BBObject * maxHandle, BBArray * array, int r, int g, int b);

	b2AABB * bmx_b2aabb_create(b2Vec2 * lowerBound, b2Vec2 * upperBound);
	void bmx_b2aabb_delete(b2AABB * aabb);
	void bmx_b2abb_setlowerbound(b2AABB * aabb, b2Vec2 * lowerBound);
	void bmx_b2abb_setupperbound(b2AABB * aabb, b2Vec2 * upperBound);

	b2Vec2 * bmx_b2vec2_create(float32 x, float32 y);
	void bmx_b2vec2_delete(b2Vec2 * vec);
	float32 bmx_b2vec2_getx(b2Vec2 * vec);
	float32 bmx_b2vec2_gety(b2Vec2 * vec);

	b2Body * bmx_b2world_createstaticbody(b2World * world, b2BodyDef * def, BBObject * body);
	b2Body * bmx_b2world_createdynamicbody(b2World * world, b2BodyDef * def, BBObject * body);
	void bmx_b2world_destroybody(b2World * world, b2Body * body);

	b2BodyDef * bmx_b2bodydef_create();
	void bmx_b2bodydef_delete(b2BodyDef * def);
	//void bmx_b2bodydef_settype(b2BodyDef * def, b2BodyDef::Type bodyType);
	void bmx_b2bodydef_setposition(b2BodyDef * def, b2Vec2 * position);

	b2World * bmx_b2world_create(b2AABB * worldAABB, b2Vec2 * gravity, bool doSleep);
	void bmx_b2world_dostep(b2World * world, float32 timeStep, int iterations);

	void bmx_b2shapedef_setfriction(b2ShapeDef * def, float32 friction);
	void bmx_b2shapedef_setrestitution(b2ShapeDef * def, float32 restitution);
	void bmx_b2shapedef_setdensity(b2ShapeDef * def, float32 density);

	b2PolygonDef * bmx_b2polygondef_create();
	void bmx_b2polygondef_setasbox(b2PolygonDef * def, float32 hx, float32 hy);

	b2Shape * bmx_b2body_createshape(b2Body * body, b2ShapeDef * def, BBObject * shape);
	void bmx_b2body_destroyshape(b2Body * body, b2Shape * shape);
	void bmx_b2body_setmassfromshapes(b2Body * body);
	b2Vec2 * bmx_b2body_getposition(b2Body * body);
	float32 bmx_b2body_getangle(b2Body * body);

	MaxDebugDraw * bmx_b2debugdraw_create(BBObject * handle);

}


// *****************************************************

b2AABB * bmx_b2aabb_create(b2Vec2 * lowerBound, b2Vec2 * upperBound) {
	b2AABB * aabb = new b2AABB;
	if (lowerBound) {
		aabb->lowerBound = *lowerBound;
	}
	if (upperBound) {
		aabb->upperBound = *upperBound;
	}
	return aabb; 
}

void bmx_b2aabb_delete(b2AABB * aabb) {
	delete aabb;
}

void bmx_b2abb_setlowerbound(b2AABB * aabb, b2Vec2 * lowerBound) {
	aabb->lowerBound = *lowerBound;
}

void bmx_b2abb_setupperbound(b2AABB * aabb, b2Vec2 * upperBound) {
	aabb->upperBound = *upperBound;
}

// *****************************************************

b2Vec2 * bmx_b2vec2_new(b2Vec2 v) {
	b2Vec2 *vec = new b2Vec2;
	vec->x = v.x;
	vec->y = v.y;
	return vec;
}

b2Vec2 * bmx_b2vec2_fromvect(b2Vec2 vec) {
	return bmx_b2vec2_new(vec);
}

b2Vec2 * bmx_b2vec2_create(float32 x, float32 y) {
	return new b2Vec2(x, y);
}

void bmx_b2vec2_delete(b2Vec2 * vec) {
	delete vec;
}

float32 bmx_b2vec2_getx(b2Vec2 * vec) {
	return vec->x;
}

float32 bmx_b2vec2_gety(b2Vec2 * vec) {
	return vec->y;
}


// *****************************************************

b2Body * bmx_b2world_createstaticbody(b2World * world, b2BodyDef * def, BBObject * body) {
	def->userData = body;
	BBRETAIN(body);
	return world->CreateStaticBody(def);
}

b2Body * bmx_b2world_createdynamicbody(b2World * world, b2BodyDef * def, BBObject * body) {
	def->userData = body;
	BBRETAIN(body);
	return world->CreateDynamicBody(def);
}

void bmx_b2world_destroybody(b2World * world, b2Body * body) {
	void * data = body->GetUserData();
	if (data && data != &bbNullObject) {
		BBRELEASE((BBObject*)data);
	}
	world->DestroyBody(body);
}

// *****************************************************

b2BodyDef * bmx_b2bodydef_create() {
	return new b2BodyDef;
}

void bmx_b2bodydef_delete(b2BodyDef * def) {
	delete def;
}

//void bmx_b2bodydef_settype(b2BodyDef * def, b2BodyDef::Type bodyType) {
//	def->type = bodyType;
//}

void bmx_b2bodydef_setposition(b2BodyDef * def, b2Vec2 * position) {
	def->position = *position;
}

// *****************************************************

b2World * bmx_b2world_create(b2AABB * worldAABB, b2Vec2 * gravity, bool doSleep) {
	return new b2World(*worldAABB, *gravity, doSleep);
}

void bmx_b2world_dostep(b2World * world, float32 timeStep, int iterations) {
	world->Step(timeStep, iterations);
}


// *****************************************************

void bmx_b2shapedef_setfriction(b2ShapeDef * def, float32 friction) {
	def->friction = friction;
}

void bmx_b2shapedef_setrestitution(b2ShapeDef * def, float32 restitution) {
	def->restitution = restitution;
}

void bmx_b2shapedef_setdensity(b2ShapeDef * def, float32 density) {
	def->density = density;
}

// *****************************************************

b2PolygonDef * bmx_b2polygondef_create() {
	return new b2PolygonDef;
}

void bmx_b2polygondef_setasbox(b2PolygonDef * def, float32 hx, float32 hy) {
	def->SetAsBox(hx, hy);
}

// *****************************************************

b2Shape * bmx_b2body_createshape(b2Body * body, b2ShapeDef * def, BBObject * shape) {
	def->userData = shape;
	BBRETAIN(shape);
	return body->CreateShape(def);
}

void bmx_b2body_destroyshape(b2Body * body, b2Shape * shape) {
	void * data = shape->GetUserData();
	if (data && data != &bbNullObject) {
		BBRELEASE((BBObject*)data);
	}
	body->DestroyShape(shape);
}

void bmx_b2body_setmassfromshapes(b2Body * body) {
	body->SetMassFromShapes();
}

b2Vec2 * bmx_b2body_getposition(b2Body * body) {
	return bmx_b2vec2_new(body->GetPosition());
}

float32 bmx_b2body_getangle(b2Body * body) {
	return body->GetAngle();
}

// *****************************************************

class MaxDebugDraw : public b2DebugDraw
{
public:

	MaxDebugDraw(BBObject * handle)
		: maxHandle(handle)
	{
	}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		BBArray * array = _bah_box2d_b2Vec2__newVecArray(vertexCount);
		for (int i = 0; i < vertexCount; i++) {
			_bah_box2d_b2Vec2__setVec(array, i, bmx_b2vec2_new(vertices[i]));
		}
		_bah_box2d_b2DebugDraw__DrawPolygon(maxHandle, array, 
			static_cast<int>(color.r * 255.0f),
			static_cast<int>(color.g * 255.0f),
			static_cast<int>(color.b * 255.0f));
	}

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	
	}

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	
	}

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	
	}

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	
	}

	void DrawXForm(const b2XForm& xf) {
	
	}
	
private:
	BBObject * maxHandle;
};


MaxDebugDraw * bmx_b2debugdraw_create(BBObject * handle) {
	return new MaxDebugDraw(handle);
}

