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
	b2Body * bmx_b2world_getgroundbody(b2World * world);

	b2BodyDef * bmx_b2bodydef_create();
	void bmx_b2bodydef_delete(b2BodyDef * def);
	//void bmx_b2bodydef_settype(b2BodyDef * def, b2BodyDef::Type bodyType);
	void bmx_b2bodydef_setposition(b2BodyDef * def, b2Vec2 * position);
	void bmx_b2bodydef_setangle(b2BodyDef * def, float32 angle);

	b2World * bmx_b2world_create(b2AABB * worldAABB, b2Vec2 * gravity, bool doSleep);
	void bmx_b2world_dostep(b2World * world, float32 timeStep, int iterations);

	void bmx_b2shapedef_setfriction(b2ShapeDef * def, float32 friction);
	void bmx_b2shapedef_setrestitution(b2ShapeDef * def, float32 restitution);
	void bmx_b2shapedef_setdensity(b2ShapeDef * def, float32 density);

	b2PolygonDef * bmx_b2polygondef_create();
	void bmx_b2polygondef_setasbox(b2PolygonDef * def, float32 hx, float32 hy);
	void bmx_b2polygondef_delete(b2PolygonDef * def);

	b2Shape * bmx_b2body_createshape(b2Body * body, b2ShapeDef * def, BBObject * shape);
	void bmx_b2body_destroyshape(b2Body * body, b2Shape * shape);
	void bmx_b2body_setmassfromshapes(b2Body * body);
	b2Vec2 * bmx_b2body_getposition(b2Body * body);
	float32 bmx_b2body_getangle(b2Body * body);
	BBObject * bmx_b2body_getmaxbody(b2Body * body);
	b2Body * bmx_b2body_getnext(b2Body * body);
	b2Shape * bmx_b2body_getshapelist(b2Body * body);
	bool bmx_b2body_isstatic(b2Body * body);
	bool bmx_b2body_isdynamic(b2Body * body);
	bool bmx_b2body_isfrozen(b2Body * body);
	bool bmx_b2body_issleeping(b2Body * body);
	void bmx_b2body_allowsleeping(b2Body * body, bool flag);
	void bmx_b2body_wakeup(b2Body * body);
	void bmx_b2body_puttosleep(b2Body * body);
	bool bmx_b2body_isbullet(b2Body * body);
	void bmx_b2body_setbullet(b2Body * body, bool flag);

	MaxDebugDraw * bmx_b2debugdraw_create(BBObject * handle);
	void bmx_b2debugdraw_setflags(MaxDebugDraw * dbg, int flags);
	int bmx_b2debugdraw_getflags(MaxDebugDraw * dbg);
	void bmx_b2debugdraw_appendflags(MaxDebugDraw * dbg, int flags);
	void bmx_b2debugdraw_clearflags(MaxDebugDraw * dbg, int flags);

	b2CircleDef * bmx_b2circledef_create();
	void bmx_b2circledef_setradius(b2CircleDef * def, float32 radius);
	void bmx_b2circledef_setlocalposition(b2CircleDef * def, b2Vec2 * pos);
	void bmx_b2circledef_delete(b2CircleDef * def);

	bool bmx_b2shape_issensor(b2Shape * shape);
	b2Body * bmx_b2shape_getbody(b2Shape * shape);
	BBObject * bmx_b2shape_getmaxshape(b2Shape * shape);

	b2RevoluteJointDef * bmx_b2revolutejointdef_create();
	void bmx_b2revolutejointdef_initialize(b2RevoluteJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor);

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

b2Body * bmx_b2world_getgroundbody(b2World * world) {
	return world->GetGroundBody();
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

void bmx_b2bodydef_setangle(b2BodyDef * def, float32 angle) {
	def->angle = angle / 57.2957795f;
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

void bmx_b2polygondef_delete(b2PolygonDef * def) {
	delete def;
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
	return body->GetAngle() * 57.2957795f;
}

BBObject * bmx_b2body_getmaxbody(b2Body * body) {
	void * obj = body->GetUserData();
	if (obj) {
		return (BBObject *)obj;
	}
	return &bbNullObject;
}

b2Body * bmx_b2body_getnext(b2Body * body) {
	return body->GetNext();
}

b2Shape * bmx_b2body_getshapelist(b2Body * body) {
	return body->GetShapeList();
}

bool bmx_b2body_isstatic(b2Body * body) {
	return body->IsStatic();
}

bool bmx_b2body_isdynamic(b2Body * body) {
	return body->IsDynamic();
}

bool bmx_b2body_isfrozen(b2Body * body) {
	return body->IsFrozen();
}

bool bmx_b2body_issleeping(b2Body * body) {
	return body->IsSleeping();
}

void bmx_b2body_allowsleeping(b2Body * body, bool flag) {
	body->AllowSleeping(flag);
}

void bmx_b2body_wakeup(b2Body * body) {
	body->WakeUp();
}

void bmx_b2body_puttosleep(b2Body * body) {
	body->PutToSleep();
}

bool bmx_b2body_isbullet(b2Body * body) {
	return body->IsBullet();
}

void bmx_b2body_setbullet(b2Body * body, bool flag) {
	body->SetBullet(flag);
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

void bmx_b2debugdraw_setflags(MaxDebugDraw * dbg, int flags) {
	dbg->SetFlags(flags);
}

int bmx_b2debugdraw_getflags(MaxDebugDraw * dbg) {
	return dbg->GetFlags();
}

void bmx_b2debugdraw_appendflags(MaxDebugDraw * dbg, int flags) {
	dbg->AppendFlags(flags);
}

void bmx_b2debugdraw_clearflags(MaxDebugDraw * dbg, int flags) {
	dbg->ClearFlags(flags);
}

// *****************************************************

b2CircleDef * bmx_b2circledef_create() {
	return new b2CircleDef;
}

void bmx_b2circledef_setradius(b2CircleDef * def, float32 radius) {
	def->radius = radius;
}

void bmx_b2circledef_setlocalposition(b2CircleDef * def, b2Vec2 * pos) {
	def->localPosition = *pos;
}

void bmx_b2circledef_delete(b2CircleDef * def) {
	delete def;
}


// *****************************************************

bool bmx_b2shape_issensor(b2Shape * shape) {
	return shape->IsSensor();
}

b2Body * bmx_b2shape_getbody(b2Shape * shape) {
	return shape->GetBody();
}

BBObject * bmx_b2shape_getmaxshape(b2Shape * shape) {
	void * obj = shape->GetUserData();
	if (obj) {
		return (BBObject *)obj;
	}
	return &bbNullObject;
}

// *****************************************************

b2RevoluteJointDef * bmx_b2revolutejointdef_create() {
	return new b2RevoluteJointDef;
}

void bmx_b2revolutejointdef_initialize(b2RevoluteJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor) {
	def->Initialize(body1, body2, *anchor);
}

