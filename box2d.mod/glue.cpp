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
class MaxContactFilter;
class MaxContactListener;
class MaxBoundaryListener;
class MaxFilterData;
class MaxDestructionListener;
class Maxb2EdgeChainDef;

extern "C" {

	BBArray * _bah_box2d_b2Vec2__newVecArray(int count);
	void _bah_box2d_b2Vec2__setVec(BBArray * array, int index, b2Vec2 * vec);
	b2Vec2 * _bah_box2d_b2Vec2__getVec(BBArray * array, int index);
	void _bah_box2d_b2World__setShape(BBArray * shapes, int index, b2Shape * shape);

	void _bah_box2d_b2DebugDraw__DrawPolygon(BBObject * maxHandle, BBArray * array, int r, int g, int b);
	void _bah_box2d_b2DebugDraw__DrawSolidPolygon(BBObject * maxHandle, BBArray * array, int r, int g, int b);
	void _bah_box2d_b2DebugDraw__DrawSegment(BBObject * maxHandle, b2Vec2 * p1, b2Vec2 * p2, int r, int g, int b);
	void _bah_box2d_b2DebugDraw__DrawSolidCircle(BBObject * maxHandle, b2Vec2 * center, float32 radius, b2Vec2 * axis, int r, int g, int b);
	BBObject * _bah_box2d_b2World__createJoint(b2JointType type);
	bool _bah_box2d_b2ContactFilter__ShouldCollide(BBObject * maxHandle, b2Shape * shape1, b2Shape * shape2);
	void _bah_box2d_b2ContactListener__Add(BBObject * maxHandle, const b2ContactPoint* point);
	void _bah_box2d_b2ContactListener__Persist(BBObject * maxHandle, const b2ContactPoint* point);
	void _bah_box2d_b2ContactListener__Remove(BBObject * maxHandle, const b2ContactPoint* point);
	void _bah_box2d_b2ContactListener__Result(BBObject * maxHandle, const b2ContactResult* result);
	void _bah_box2d_b2BoundaryListener__Violation(BBObject * maxHandle, b2Body * body);

	void _bah_box2d_b2DestructionListener__SayGoodbyeJoint(BBObject * maxHandle, b2Joint * joint);
	void _bah_box2d_b2DestructionListener__SayGoodbyeShape(BBObject * maxHandle, b2Shape * shape);
	BBObject * _bah_box2d_b2Body__createShape(b2ShapeType type);

	b2AABB * bmx_b2aabb_create(b2Vec2 * lowerBound, b2Vec2 * upperBound);
	void bmx_b2aabb_delete(b2AABB * aabb);
	void bmx_b2abb_setlowerbound(b2AABB * aabb, b2Vec2 * lowerBound);
	void bmx_b2abb_setupperbound(b2AABB * aabb, b2Vec2 * upperBound);
	bool bmx_b2abb_isvalid(b2AABB * aabb);

	b2Vec2 * bmx_b2vec2_create(float32 x, float32 y);
	void bmx_b2vec2_delete(b2Vec2 * vec);
	float32 bmx_b2vec2_getx(b2Vec2 * vec);
	float32 bmx_b2vec2_gety(b2Vec2 * vec);
	void bmx_b2vec2_add(b2Vec2 * vec, b2Vec2 * other);
	void bmx_b2vec2_copy(b2Vec2 * vec, b2Vec2 * other);
	void bmx_b2vec2_set(b2Vec2 * vec, float32 x, float32 y);
	b2Vec2 * bmx_b2vec2_subtract(b2Vec2 * vec, b2Vec2 * other);
	float32 bmx_b2vec2_length(b2Vec2 * vec);
	void bmx_b2vec2_multiply(b2Vec2 * vec, float32 value);
	b2Vec2 * bmx_b2vec2_plus(b2Vec2 * vec, b2Vec2 * other);
	float32 bmx_b2vec2_normalize(b2Vec2 * vec);
	float32 bmx_b2vec2_lengthsquared(b2Vec2 * vec);
	BBArray * bmx_b2vec2_getvertexarray(const b2Vec2* vertices, int32 vertexCount);

	b2Body * bmx_b2world_createbody(b2World * world, b2BodyDef * def, BBObject * body);
	void bmx_b2world_destroybody(b2World * world, b2Body * body);
	b2Body * bmx_b2world_getgroundbody(b2World * world);
	void bmx_b2world_setwarmstarting(b2World * world, bool flag);
	void bmx_b2world_setcontinuousphysics(b2World * world, bool flag);
	void bmx_b2world_validate(b2World * world);
	void bmx_b2world_setdebugDraw(b2World * world, b2DebugDraw * debugDraw);
	b2Joint * bmx_b2world_createjoint(b2World * world, b2JointDef * def);
	void bmx_b2world_destroyjoint(b2World * world, b2Joint * joint);
	b2Body * bmx_b2world_getbodylist(b2World * world);
	b2Joint * bmx_b2world_getjointlist(b2World * world);
	void bmx_b2world_setfilter(b2World * world, b2ContactFilter * filter);
	void bmx_b2world_setcontactlistener(b2World * world, b2ContactListener * listener);
	void bmx_b2world_setboundarylistener(b2World * world, b2BoundaryListener * listener);
	void bmx_b2world_setgravity(b2World * world, b2Vec2 * gravity);
	int32 bmx_b2world_getproxycount(b2World * world);
	int32 bmx_b2world_getpaircount(b2World * world);
	int32 bmx_b2world_getbodycount(b2World * world);
	int32 bmx_b2world_getjointcount(b2World * world);
	int32 bmx_b2world_query(b2World * world, b2AABB * aabb, BBArray * shapes);
	void bmx_b2world_free(b2World * world);
	void bmx_b2world_setdestructionlistener(b2World * world, b2DestructionListener * listener);
	void bmx_b2world_refilter(b2World * world, b2Shape * shape);
	int32 bmx_b2world_raycast(b2World * world, b2Segment * segment, BBArray * shapes, bool solidShapes);
	b2Shape * bmx_b2world_raycastone(b2World * world, b2Segment * segment, float32 * lambda, b2Vec2 * normal, bool solidShapes);
	bool bmx_b2world_inrange(b2World * world, b2AABB * aabb);

	b2BodyDef * bmx_b2bodydef_create();
	void bmx_b2bodydef_delete(b2BodyDef * def);
	void bmx_b2bodydef_setposition(b2BodyDef * def, b2Vec2 * position);
	void bmx_b2bodydef_setpositionxy(b2BodyDef * def, float32 x, float32 y);
	void bmx_b2bodydef_setangle(b2BodyDef * def, float32 angle);
	void bmx_b2bodydef_setmassdata(b2BodyDef * def, b2MassData * data);
	bool bmx_b2bodydef_issleeping(b2BodyDef * def);
	void bmx_b2bodydef_setissleeping(b2BodyDef * def, bool sleeping);
	void bmx_b2bodydef_setfixedrotation(b2BodyDef * def, bool fixed);
	bool bmx_b2bodydef_getfixedrotation(b2BodyDef * def);
	void bmx_b2bodydef_setisbullet(b2BodyDef * def, bool bullet);
	void bmx_b2bodydef_setlineardamping(b2BodyDef * def, float32 damping);
	float32 bmx_b2bodydef_getlineardamping(b2BodyDef * def);
	void bmx_b2bodydef_setangulardamping(b2BodyDef * def, float32 damping);
	float32 bmx_b2bodydef_getangulardamping(b2BodyDef * def);
	void bmx_b2bodydef_setallowsleep(b2BodyDef * def, bool allow);
	bool bmx_b2bodydef_getallowsleep(b2BodyDef * def);
	b2Vec2 * bmx_b2bodydef_getposition(b2BodyDef * def);

	b2World * bmx_b2world_create(b2AABB * worldAABB, b2Vec2 * gravity, bool doSleep);
	void bmx_b2world_dostep(b2World * world, float32 timeStep, int velocityIterations, int positionIterations);

	void bmx_b2shapedef_setfriction(b2ShapeDef * def, float32 friction);
	void bmx_b2shapedef_setrestitution(b2ShapeDef * def, float32 restitution);
	void bmx_b2shapedef_setdensity(b2ShapeDef * def, float32 density);
	void bmx_b2shapedef_setfilter(b2ShapeDef * def, MaxFilterData* filterData);
	MaxFilterData* bmx_b2shapedef_getfilter(b2ShapeDef * def);
	void bmx_b2shapedef_setissensor(b2ShapeDef * def, bool sensor);
	bool bmx_b2shapedef_issensor(b2ShapeDef * def);

	b2PolygonDef * bmx_b2polygondef_create();
	void bmx_b2polygondef_setasbox(b2PolygonDef * def, float32 hx, float32 hy);
	void bmx_b2polygondef_delete(b2PolygonDef * def);
	void bmx_b2polygondef_setasorientedbox(b2PolygonDef * def, float32 hx, float32 hy, b2Vec2 * center, float32 angle);
	void bmx_b2polygondef_setvertices(b2PolygonDef * def, BBArray * vertices);

	b2Shape * bmx_b2body_createshape(b2Body * body, b2ShapeDef * def);
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
	b2Vec2 * bmx_b2body_getworldcenter(b2Body * body);
	b2Vec2 * bmx_b2body_getlocalcenter(b2Body * body);
	void bmx_b2body_setlinearvelocity(b2Body * body, b2Vec2 * v);
	b2Vec2 * bmx_b2body_getlinearvelocity(b2Body * body);
	void bmx_b2body_setangularvelocity(b2Body * body, float32 omega);
	float32 bmx_b2body_getangularvelocity(b2Body * body);
	void bmx_b2body_applyforce(b2Body * body, b2Vec2 * force, b2Vec2 * point);
	void bmx_b2body_applytorque(b2Body * body, float32 torque);
	void bmx_b2body_applyimpulse(b2Body * body, b2Vec2 * impulse, b2Vec2 * point);
	float32 bmx_b2body_getmass(b2Body * body);
	float32 bmx_b2body_getinertia(b2Body * body);
	b2Vec2 * bmx_b2body_getworldpoint(b2Body * body, b2Vec2 * localPoint);
	b2Vec2 * bmx_b2body_getworldvector(b2Body * body, b2Vec2 * localVector);
	b2Vec2 * bmx_b2body_getlocalpoint(b2Body * body, b2Vec2 * worldPoint);
	b2Vec2 * bmx_b2body_getlocalvector(b2Body * body, b2Vec2 * worldVector);
	b2JointEdge * bmx_b2body_getjointlist(b2Body * body);
	const b2XForm * bmx_b2body_getxform(b2Body * body);
	bool bmx_b2body_setxform(b2Body * body, b2Vec2 * position, float32 angle);
	b2World * bmx_b2body_getworld(b2Body * body);

	MaxDebugDraw * bmx_b2debugdraw_create(BBObject * handle);
	void bmx_b2debugdraw_setflags(MaxDebugDraw * dbg, uint32 flags);
	uint32 bmx_b2debugdraw_getflags(MaxDebugDraw * dbg);
	void bmx_b2debugdraw_appendflags(MaxDebugDraw * dbg, uint32 flags);
	void bmx_b2debugdraw_clearflags(MaxDebugDraw * dbg, uint32 flags);

	b2CircleDef * bmx_b2circledef_create();
	void bmx_b2circledef_setradius(b2CircleDef * def, float32 radius);
	void bmx_b2circledef_setlocalposition(b2CircleDef * def, b2Vec2 * pos);
	void bmx_b2circledef_delete(b2CircleDef * def);
	float32 bmx_b2circledef_getradius(b2CircleDef * def);
	b2Vec2 * bmx_b2circledef_getlocalposition(b2CircleDef * def);

	bool bmx_b2shape_issensor(b2Shape * shape);
	b2Body * bmx_b2shape_getbody(b2Shape * shape);
	BBObject * bmx_b2shape_getmaxshape(b2Shape * shape);
	b2Shape * bmx_b2shape_getnext(b2Shape * shape);
	bool bmx_b2shape_testpoint(b2Shape * shape, b2XForm * xf, b2Vec2 * p);
	float32 bmx_b2shape_getsweepradius(b2Shape * shape);
	float32 bmx_b2shape_getfriction(b2Shape * shape);
	float32 bmx_b2shape_getrestitution(b2Shape * shape);
	void bmx_b2shape_computeaabb(b2Shape * shape, b2AABB * aabb, b2XForm * xf);
	void bmx_b2shape_computesweptaabb(b2Shape * shape, b2AABB * aabb, b2XForm * xf1, b2XForm * xf2);
	void bmx_b2shape_computemass(b2Shape * shape, b2MassData * data);
	MaxFilterData * bmx_b2shape_getfilterdata(b2Shape * shape);
	void bmx_b2shape_setfilterdata(b2Shape * shape, MaxFilterData * data);
	void bmx_b2shape_setfriction(b2Shape * shape, float32 friction);
	void bmx_b2shape_setrestitution(b2Shape * shape, float32 restitution);

	b2RevoluteJointDef * bmx_b2revolutejointdef_create();
	void bmx_b2revolutejointdef_initialize(b2RevoluteJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor);
	void bmx_b2revolutejointdef_delete(b2RevoluteJointDef * def);
	bool bmx_b2revolutejointdef_islimitenabled(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_enablelimit(b2RevoluteJointDef * def, bool limit);
	float32 bmx_b2revolutejointdef_getlowerangle(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setlowerangle(b2RevoluteJointDef * def, float32 angle);
	float32 bmx_b2revolutejointdef_getupperangle(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setupperangle(b2RevoluteJointDef * def, float32 angle);
	bool bmx_b2revolutejointdef_ismotorenabled(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_enablemotor(b2RevoluteJointDef * def, bool value);
	float32 bmx_b2revolutejointdef_getmotorspeed(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setmotorspeed(b2RevoluteJointDef * def, float32 speed);
	float32 bmx_b2revolutejointdef_getmaxmotortorque(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setmaxmotortorque(b2RevoluteJointDef * def, float32 torque);
	b2Vec2 * bmx_b2revolutejointdef_getlocalanchor1(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setlocalanchor1(b2RevoluteJointDef * def, b2Vec2 *anchor);
	b2Vec2 * bmx_b2revolutejointdef_getlocalanchor2(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setlocalanchor2(b2RevoluteJointDef * def, b2Vec2 *anchor);
	float32 bmx_b2revolutejointdef_getreferenceangle(b2RevoluteJointDef * def);
	void bmx_b2revolutejointdef_setreferenceangle(b2RevoluteJointDef * def, float32 angle);

	BBObject * bmx_b2joint_getmaxjoint(b2Joint * joint);
	void bmx_b2joint_attachobject(b2Joint * joint, BBObject *joint);
	b2Body * bmx_b2joint_getbody1(b2Joint * joint);
	b2Body * bmx_b2joint_getbody2(b2Joint * joint);
	b2Joint * bmx_b2joint_getnext(b2Joint * joint);

	b2MassData * bmx_b2massdata_new();
	void bmx_b2massdata_delete(b2MassData * data);
	void bmx_b2massdata_setmass(b2MassData * data, float32 mass);
	void bmx_b2massdata_setcenter(b2MassData * data, b2Vec2 * center);
	void bmx_b2massdata_seti(b2MassData * data, float32 i);

	b2Body * bmx_b2jointedge_getother(b2JointEdge * joint);
	b2Joint * bmx_b2jointedge_getjoint(b2JointEdge * joint);
	b2JointEdge * bmx_b2jointedge_getprev(b2JointEdge * joint);
	b2JointEdge * bmx_b2jointedge_getnext(b2JointEdge * joint);

	MaxContactFilter * bmx_b2contactfilter_new(BBObject * handle);
	void bmx_b2contactfilter_delete(MaxContactFilter * filter);

	MaxContactListener * bmx_b2contactlistener_new(BBObject * handle);
	void bmx_b2contactlistener_delete(MaxContactListener * filter);

	MaxBoundaryListener * bmx_b2boundarylistener_new(BBObject * handle);
	void bmx_b2boundarylistener_delete(MaxBoundaryListener * filter);

	void bmx_b2jointdef_setcollideconnected(b2JointDef * def, bool collideConnected);
	bool bmx_b2jointdef_getcollideconnected(b2JointDef * def);
	void bmx_b2jointdef_setbody1(b2JointDef * def, b2Body * body);
	b2Body * bmx_b2jointdef_getbody1(b2JointDef * def);
	void bmx_b2jointdef_setbody2(b2JointDef * def, b2Body * body);
	b2Body * bmx_b2jointdef_getbody2(b2JointDef * def);

	b2DistanceJointDef * bmx_b2distancejointdef_new();
	void bmx_b2distancejointdef_setlocalanchor1(b2DistanceJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2distancejointdef_getlocalanchor1(b2DistanceJointDef * def);
	void bmx_b2distancejointdef_setlocalanchor2(b2DistanceJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2distancejointdef_getlocalanchor2(b2DistanceJointDef * def);
	void bmx_b2distancejointdef_setlength(b2DistanceJointDef * def, float32 length);
	float32 bmx_b2distancejointdef_getlength(b2DistanceJointDef * def);
	void bmx_b2distancejointdef_delete(b2DistanceJointDef * def);
	void bmx_b2distancejointdef_setfrequencyhz(b2DistanceJointDef * def, float32 freq);
	void bmx_b2distancejointdef_setdampingratio(b2DistanceJointDef * def, float32 ratio);
	void bmx_b2distancejointdef_initialize(b2DistanceJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor1, b2Vec2 * anchor2);

	b2PrismaticJointDef * bmx_b2prismaticjointdef_create();
	void bmx_b2prismaticjointdef_enablelimit(b2PrismaticJointDef * def, bool value);
	bool bmx_b2prismaticjointdef_islimitenabled(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setlowertranslation(b2PrismaticJointDef * def, float32 translation);
	float32 bmx_b2prismaticjointdef_getlowertranslation(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setuppertranslation(b2PrismaticJointDef * def, float32 translation);
	float32 bmx_b2prismaticjointdef_getuppertranslation(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_enablemotor(b2PrismaticJointDef * def, bool value);
	bool bmx_b2prismaticjointdef_ismotorenabled(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setmaxmotorforce(b2PrismaticJointDef * def, float32 force);
	float32 bmx_b2prismaticjointdef_getmaxmotorforce(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setmotorspeed(b2PrismaticJointDef * def, float32 speed);
	float32 bmx_b2prismaticjointdef_getmotorspeed(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_delete(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_initialize(b2PrismaticJointDef * def, b2Body * body1, b2Body * body2,
		b2Vec2 * anchor, b2Vec2 * axis);
	void bmx_b2prismaticjointdef_setlocalanchor1(b2PrismaticJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2prismaticjointdef_getlocalanchor1(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setlocalanchor2(b2PrismaticJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2prismaticjointdef_getlocalanchor2(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setlocalaxis1(b2PrismaticJointDef * def, b2Vec2 * axis);
	b2Vec2 * bmx_b2prismaticjointdef_getlocalaxis1(b2PrismaticJointDef * def);
	void bmx_b2prismaticjointdef_setreferenceangle(b2PrismaticJointDef * def, float32 angle);
	float32 bmx_b2prismaticjointdef_getreferenceangle(b2PrismaticJointDef * def);


	float32 bmx_b2revolutejoint_getlowerlimit(b2RevoluteJoint * joint);
	float32 bmx_b2revolutejoint_getupperlimit(b2RevoluteJoint * joint);
	void bmx_b2revolutejoint_setlimits(b2RevoluteJoint * joint, float32 lowerLimit, float32 upperLimit);
	bool bmx_b2revolutejoint_ismotorenabled(b2RevoluteJoint * joint);
	void bmx_b2revolutejoint_enablemotor(b2RevoluteJoint * joint, bool flag);
	void bmx_b2revolutejoint_setmotorspeed(b2RevoluteJoint * joint, float32 speed);
	float32 bmx_b2revolutejoint_getmotorspeed(b2RevoluteJoint * joint);
	void bmx_b2revolutejoint_setmaxmotortorque(b2RevoluteJoint * joint, float32 torque);
	float32 bmx_b2revolutejoint_getmotortorque(b2RevoluteJoint * joint);
	bool bmx_b2revolutejoint_islimitenabled(b2RevoluteJoint * joint);
	void bmx_b2revolutejoint_enablelimit(b2RevoluteJoint * joint, bool flag);
	b2Vec2 * bmx_b2revolutejoint_getanchor1(b2RevoluteJoint * joint);
	b2Vec2 * bmx_b2revolutejoint_getanchor2(b2RevoluteJoint * joint);
	b2Vec2 * bmx_b2revolutejoint_getreactionforce(b2RevoluteJoint * joint, float32 inv_dt);
	float32 bmx_b2revolutejoint_getreactiontorque(b2RevoluteJoint * joint, float32 inv_dt);
	float32 bmx_b2revolutejoint_getjointangle(b2RevoluteJoint * joint);
	float32 bmx_b2revolutejoint_getjointspeed(b2RevoluteJoint * joint);

	float32 bmx_b2prismaticjoint_getjointspeed(b2PrismaticJoint * joint);
	bool bmx_b2prismaticjoint_islimitenabled(b2PrismaticJoint * joint);
	void bmx_b2prismaticjoint_enablelimit(b2PrismaticJoint * joint, bool flag);
	float32 bmx_b2prismaticjoint_getlowerlimit(b2PrismaticJoint * joint);
	float32 bmx_b2prismaticjoint_getupperlimit(b2PrismaticJoint * joint);
	void bmx_b2prismaticjoint_setlimits(b2PrismaticJoint * joint, float32 lowerLimit, float32 upperLimit);
	bool bmx_b2prismaticjoint_ismotorenabled(b2PrismaticJoint * joint);
	void bmx_b2prismaticjoint_enablemotor(b2PrismaticJoint * joint, bool flag);
	void bmx_b2prismaticjoint_setmotorspeed(b2PrismaticJoint * joint, float32 speed);
	float32 bmx_b2prismaticjoint_getmotorspeed(b2PrismaticJoint * joint);
	void bmx_b2prismaticjoint_setmaxmotorforce(b2PrismaticJoint * joint, float32 force);
	float32 bmx_b2prismaticjoint_getmotorforce(b2PrismaticJoint * joint);
	b2Vec2 * bmx_b2prismaticjoint_getanchor1(b2PrismaticJoint * joint);
	b2Vec2 * bmx_b2prismaticjoint_getanchor2(b2PrismaticJoint * joint);
	b2Vec2 * bmx_b2prismaticjoint_getreactionforce(b2PrismaticJoint * joint, float32 inv_dt);
	float32 bmx_b2prismaticjoint_getreactiontorque(b2PrismaticJoint * joint, float32 inv_dt);
	float32 bmx_b2prismaticjoint_getjointtranslation(b2PrismaticJoint * joint);

	b2Vec2 * bmx_b2cross(b2Vec2 * a, float32 s);
	b2Vec2 * bmx_b2crossf(float32 s, b2Vec2 * a);
	b2Vec2 * bmx_b2mul(b2Mat22 * A, b2Vec2 * v);
	b2Vec2 * bmx_b2mult(b2Mat22 * A, b2Vec2 * v);
	b2Vec2 * bmx_b2mulf(b2XForm * T, b2Vec2 * v);
	b2Vec2 * bmx_b2multf(b2XForm * T, b2Vec2 * v);
	float32 bmx_b2dot(b2Vec2 * a, b2Vec2 * b);

	b2XForm * bmx_b2xform_create();
	b2Vec2 * bmx_b2xform_getposition(b2XForm * form);
	void bmx_b2xform_setposition(b2XForm * form, b2Vec2 * pos);
	b2Mat22 * bmx_b2xform_getr(b2XForm * form);
	void bmx_b2xform_setr(b2XForm * form, b2Mat22 * r);
	void bmx_b2xform_delete(b2XForm * form);

	void bmx_b2mat22_setangle(b2Mat22 * mat, float32 angle);
	float32 bmx_b2mat22_getangle(b2Mat22 * mat);

	b2Shape * bmx_b2contact_getshape1(b2Contact * contact);
	b2Shape * bmx_b2contact_getshape2(b2Contact * contact);
	b2Contact * bmx_b2contact_getnext(b2Contact * contact);
	bool bmx_b2contact_issolid(b2Contact * contact);
	int32 bmx_b2contact_getmanifoldcount(b2Contact * contact);

	b2Shape * bmx_b2contactresult_getshape1(b2ContactResult * contactResult);
	b2Shape * bmx_b2contactresult_getshape2(b2ContactResult * contactResult);
	b2Vec2 * bmx_b2contactresult_getposition(b2ContactResult * contactResult);
	b2Vec2 * bmx_b2contactresult_getnormal(b2ContactResult * contactResult);
	float32 bmx_b2contactresult_getnormalimpulse(b2ContactResult * contactResult);
	float32 bmx_b2contactresult_gettangentimpulse(b2ContactResult * contactResult);

	b2Shape * bmx_b2contactpoint_getshape1(b2ContactPoint * contactPoint);
	b2Shape * bmx_b2contactpoint_getshape2(b2ContactPoint * contactPoint);
	b2Vec2 * bmx_b2contactpoint_getposition(b2ContactPoint * contactPoint);
	b2Vec2 * bmx_b2contactpoint_getvelocity(b2ContactPoint * contactPoint);
	b2Vec2 * bmx_b2contactpoint_getnormal(b2ContactPoint * contactPoint);
	float32 bmx_b2contactpoint_getseparation(b2ContactPoint * contactPoint);
	float32 bmx_b2contactpoint_getfriction(b2ContactPoint * contactPoint);
	float32 bmx_b2contactpoint_getrestitution(b2ContactPoint * contactPoint);

	MaxFilterData* bmx_b2filterdata_create();
	uint16  bmx_b2filterdata_getcategorybits(MaxFilterData * filterData);
	void bmx_b2filterdata_setcategorybits(MaxFilterData * filterData, uint16 categoryBits);
	uint16  bmx_b2filterdata_getmaskbits(MaxFilterData * filterData);
	void bmx_b2filterdata_setmaskbits(MaxFilterData * filterData, uint16 maskBits);
	int16 bmx_b2filterdata_getgroupindex(MaxFilterData * filterData);
	void bmx_b2filterdata_setgroupindex(MaxFilterData * filterData, int16 index);
	void bmx_b2filterdata_delete(MaxFilterData * filterData);

	b2GearJointDef * bmx_b2gearjointdef_new();
	void bmx_b2gearjointdef_setjoint1(b2GearJointDef * def, b2Joint * joint);
	void bmx_b2gearjointdef_setjoint2(b2GearJointDef * def, b2Joint * joint);
	void bmx_b2gearjointdef_setratio(b2GearJointDef * def, float32 ratio);
	void bmx_b2gearjointdef_delete(b2GearJointDef * def);

	b2Vec2 * bmx_b2gearjoint_getanchor1(b2GearJoint * joint);
	b2Vec2 * bmx_b2gearjoint_getanchor2(b2GearJoint * joint);
	b2Vec2 * bmx_b2gearjoint_getreactionforce(b2GearJoint * joint, float32 inv_dt);
	float32 bmx_b2gearjoint_getreactiontorque(b2GearJoint * joint, float32 inv_dt);
	float32 bmx_b2gearjoint_getratio(b2GearJoint * joint);

	b2Vec2 * bmx_b2mousejoint_getanchor1(b2MouseJoint * joint);
	b2Vec2 * bmx_b2mousejoint_getanchor2(b2MouseJoint * joint);
	b2Vec2 * bmx_b2mousejoint_getreactionforce(b2MouseJoint * joint, float32 inv_dt);
	float32 bmx_b2mousejoint_getreactiontorque(b2MouseJoint * joint, float32 inv_dt);
	void bmx_b2mousejoint_settarget(b2MouseJoint * joint, b2Vec2 * target);
	b2Vec2 * bmx_b2mousejoint_gettarget(b2MouseJoint * joint);
	b2Vec2 * bmx_b2mousejoint_getlocalanchor(b2MouseJoint * joint);

	b2Vec2 * bmx_b2pulleyjoint_getanchor1(b2PulleyJoint * joint);
	b2Vec2 * bmx_b2pulleyjoint_getanchor2(b2PulleyJoint * joint);
	b2Vec2 * bmx_b2pulleyjoint_getreactionforce(b2PulleyJoint * joint, float32 inv_dt);
	float32 bmx_b2pulleyjoint_getreactiontorque(b2PulleyJoint * joint, float32 inv_dt);
	b2Vec2 * bmx_b2pulleyjoint_getgroundanchor1(b2PulleyJoint * joint);
	b2Vec2 * bmx_b2pulleyjoint_getgroundanchor2(b2PulleyJoint * joint);
	float32 bmx_b2pulleyjoint_getlength1(b2PulleyJoint * joint);
	float32 bmx_b2pulleyjoint_getlength2(b2PulleyJoint * joint);
	float32 bmx_b2pulleyjoint_getratio(b2PulleyJoint * joint);

	b2Vec2 * bmx_b2distancejoint_getanchor1(b2DistanceJoint * joint);
	b2Vec2 * bmx_b2distancejoint_getanchor2(b2DistanceJoint * joint);
	b2Vec2 * bmx_b2distancejoint_getreactionforce(b2DistanceJoint * joint, float32 inv_dt);
	float32 bmx_b2distancejoint_getreactiontorque(b2DistanceJoint * joint, float32 inv_dt);

	b2MouseJointDef * bmx_b2mousejointdef_new();
	void bmx_b2mousejointdef_settarget(b2MouseJointDef * def, b2Vec2 * target);
	b2Vec2 * bmx_b2mousejointdef_gettarget(b2MouseJointDef * def);
	void bmx_b2mousejointdef_setmaxforce(b2MouseJointDef * def, float32 maxForce);
	float32 bmx_b2mousejointdef_getmaxforce(b2MouseJointDef * def);
	void bmx_b2mousejointdef_setfrequencyhz(b2MouseJointDef * def, float32 frequency);
	float32 bmx_b2mousejointdef_getfrequencyhz(b2MouseJointDef * def);
	void bmx_b2mousejointdef_setdampingration(b2MouseJointDef * def, float32 ratio);
	float32 bmx_b2mousejointdef_getdampingratio(b2MouseJointDef * def);
	void bmx_b2mousejointdef_delete(b2MouseJointDef * def);

	b2PulleyJointDef * bmx_b2pulleyjointdef_create();
	void bmx_b2pulleyjointdef_initialize(b2PulleyJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * groundAnchor1, 
		b2Vec2 * groundAnchor2, b2Vec2 * anchor1, b2Vec2 * anchor2, float32 ratio);
	void bmx_b2pulleyjointdef_setgroundanchor1(b2PulleyJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2pulleyjointdef_getgroundanchor1(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setgroundanchor2(b2PulleyJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2pulleyjointdef_getgroundanchor2(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setlocalanchor1(b2PulleyJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2pulleyjointdef_getlocalanchor1(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setlocalanchor2(b2PulleyJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2pulleyjointdef_getlocalanchor2(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setlength1(b2PulleyJointDef * def, float32 length);
	float32 bmx_b2pulleyjointdef_getlength1(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setmaxlength1(b2PulleyJointDef * def, float32 maxLength);
	float32 bmx_b2pulleyjointdef_getmaxlength1(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setlength2(b2PulleyJointDef * def, float32 length);
	float32 bmx_b2pulleyjointdef_getlength2(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setmaxlength2(b2PulleyJointDef * def, float32 maxLength);
	float32 bmx_b2pulleyjointdef_getmaxlength2(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_setratio(b2PulleyJointDef * def, float32 ratio);
	float32 bmx_b2pulleyjointdef_getratio(b2PulleyJointDef * def);
	void bmx_b2pulleyjointdef_delete(b2PulleyJointDef * def);

	MaxDestructionListener * bmx_b2destructionlistener_new(BBObject * handle);
	void bmx_b2destructionlistener_delete(MaxDestructionListener * listener);

	b2Mat22 * bmx_b2obb_getrotationmatrix(b2OBB * obb);
	b2Vec2 * bmx_b2obb_getcenter(b2OBB * obb);
	b2Vec2 * bmx_b2obb_getextents(b2OBB * obb);
	void bmx_b2obb_delete(b2OBB * obb);

	BBArray * bmx_b2polygonshape_getvertices(b2PolygonShape * shape);
	BBArray * bmx_b2polygonshape_getcorevertices(b2PolygonShape * shape);
	BBArray * bmx_b2polygonshape_getnormals(b2PolygonShape * shape);
	const b2OBB * bmx_b2polygonshape_getobb(b2PolygonShape * shape);
	b2Vec2 * bmx_b2polygonshape_getcentroid(b2PolygonShape * shape);
	int bmx_b2polygonshape_getvertexcount(b2PolygonShape * shape);
	b2Vec2 * bmx_b2polygonshape_getfirstvertex(b2PolygonShape * shape, b2XForm * xf);
	b2Vec2 * bmx_b2polygonshape_centroid(b2PolygonShape * shape, b2XForm * xf);
	b2Vec2 * bmx_b2polygonshape_support(b2PolygonShape * shape, b2XForm * xf, b2Vec2 * d);

	b2Vec2 * bmx_b2circleshape_getlocalposition(b2CircleShape * shape);
	float32 bmx_b2circleshape_getradius(b2CircleShape * shape);

	b2Segment * bmx_b2segment_createxy(float32 x1, float32 y1, float32 x2, float32 y2);
	b2Segment * bmx_b2segment_create(b2Vec2 * p1, b2Vec2 * p2);
	b2Vec2 * bmx_b2segment_getstartpoint(b2Segment * seg);
	b2Vec2 * bmx_b2segment_getendpoint(b2Segment * seg);
	void bmx_b2segment_setstartpoint(b2Segment * seg, b2Vec2 * point);
	void bmx_b2segment_setendpoint(b2Segment * seg, b2Vec2 * point);
	void bmx_b2segment_delete(b2Segment * seg);

	b2LineJointDef * bmx_b2linejointdef_create();
	void bmx_b2linejointdef_initialize(b2LineJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor, b2Vec2 * axis);
	void bmx_b2linejointdef_setlocalanchor1(b2LineJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2linejointdef_getlocalanchor1(b2LineJointDef * def);
	void bmx_b2linejointdef_setlocalanchor2(b2LineJointDef * def, b2Vec2 * anchor);
	b2Vec2 * bmx_b2linejointdef_getlocalanchor2(b2LineJointDef * def);
	void bmx_b2linejointdef_setlocalaxis1(b2LineJointDef * def, b2Vec2 * axis);
	b2Vec2 * bmx_b2linejointdef_getlocalaxis1(b2LineJointDef * def);
	void bmx_b2linejointdef_enablelimit(b2LineJointDef * def, bool limit);
	bool bmx_b2linejointdef_getlimit(b2LineJointDef * def);
	void bmx_b2linejointdef_setlowertranslation(b2LineJointDef * def, float32 translation);
	float32 bmx_b2linejointdef_getlowertranslation(b2LineJointDef * def);
	void bmx_b2linejointdef_setuppertranslation(b2LineJointDef * def, float32 translation);
	float32 bmx_b2linejointdef_getuppertranslation(b2LineJointDef * def);
	void bmx_b2linejointdef_enablemotor(b2LineJointDef * def, bool enable);
	bool bmx_b2linejointdef_ismotorenabled(b2LineJointDef * def);
	void bmx_b2linejointdef_setmaxmotorforce(b2LineJointDef * def, float32 maxForce);
	float32 bmx_b2linejointdef_getmaxmotorforce(b2LineJointDef * def);
	void bmx_b2linejointdef_setmotorspeed(b2LineJointDef * def, float32 speed);
	float32 bmx_b2linejointdef_getmotorspeed(b2LineJointDef * def);
	void bmx_b2linejointdef_delete(b2LineJointDef * def);
	
	b2Vec2 * bmx_b2linejoint_getanchor1(b2LineJoint * joint);
	b2Vec2 * bmx_b2linejoint_getanchor2(b2LineJoint * joint);
	b2Vec2 * bmx_b2linejoint_getreactionforce(b2LineJoint * joint, float32 inv_dt);
	float32 bmx_b2linejoint_getreactiontorque(b2LineJoint * joint, float32 inv_dt);
	float32 bmx_b2linejoint_getjointtranslation(b2LineJoint * joint);
	float32 bmx_b2linejoint_getjointspeed(b2LineJoint * joint);
	bool bmx_b2linejoint_islimitenabled(b2LineJoint * joint);
	void bmx_b2linejoint_enablelimit(b2LineJoint * joint, bool flag);
	float32 bmx_b2linejoint_getlowerlimit(b2LineJoint * joint);
	float32 bmx_b2linejoint_getupperlimit(b2LineJoint * joint);
	void bmx_b2linejoint_setlimits(b2LineJoint * joint, float32 _lower, float32 _upper);
	bool bmx_b2linejoint_ismotorenabled(b2LineJoint * joint);
	void bmx_b2linejoint_enablemotor(b2LineJoint * joint, bool flag);
	void bmx_b2linejoint_setmotorspeed(b2LineJoint * joint, float32 speed);
	float32 bmx_b2linejoint_getmotorspeed(b2LineJoint * joint);
	void bmx_b2linejoint_setmaxmotorforce(b2LineJoint * joint, float32 force);
	float32 bmx_b2linejoint_getmotorforce(b2LineJoint * joint);

	Maxb2EdgeChainDef * bmx_b2edgechaindef_create();
	b2EdgeChainDef * bmx_b2edgechaindef_getdef(Maxb2EdgeChainDef * def);
	bool bmx_b2edgechaindef_isaloop(Maxb2EdgeChainDef * def);
	void bmx_b2edgechaindef_setisaloop(Maxb2EdgeChainDef * def, bool value);
	void bmx_b2edgechaindef_delete(Maxb2EdgeChainDef * def);
	void bmx_b2edgechaindef_setvertices(Maxb2EdgeChainDef * def, BBArray * vertices);

	float32 bmx_b2edgeshape_getlength(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getvertex1(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getvertex2(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getcorevertex1(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getcorevertex2(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getnormalvector(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getdirectionvector(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getcorner1vector(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getcorner2vector(b2EdgeShape * shape);
	bool bmx_b2edgeshape_corner1isconvex(b2EdgeShape * shape);
	bool bmx_b2edgeshape_corner2isconvex(b2EdgeShape * shape);
	b2Vec2 * bmx_b2edgeshape_getfirstvertex(b2EdgeShape * shape, b2XForm * xf);
	b2Vec2 * bmx_b2edgeshape_support(b2EdgeShape * shape, b2XForm * xf, b2Vec2 * d);
	b2EdgeShape * bmx_b2edgeshape_getnextedge(b2EdgeShape * shape);
	b2EdgeShape * bmx_b2edgeshape_getprevedge(b2EdgeShape * shape);

}

class MaxFilterData
{
public:
	MaxFilterData(b2FilterData& fd)
	{
		data = &fd;
		owner = false;
	}

	MaxFilterData(const b2FilterData& fd)
	{
		data = const_cast<b2FilterData *>(&fd);
		owner = false;
	}

	MaxFilterData()
	{
		data = new b2FilterData;
		owner = true;
	}

	~MaxFilterData()
	{
		if (owner) {
			delete data;
		}
	}
	
	const b2FilterData& getData() {
		return *data;
	}
	
	void setCategoryBits(uint16 categoryBits) {
		data->categoryBits = categoryBits;
	}
	
	void setMaskBits(uint16 maskBits) {
		data->maskBits = maskBits;
	}
	
	void setGroupIndex(int16 index) {
		data->groupIndex = index;
	}
	
	int16 getGroupIndex() {
		return data->groupIndex;
	}
	
private:
	b2FilterData * data;
	bool owner;
};


// *****************************************************

class Maxb2EdgeChainDef
{
public:
	Maxb2EdgeChainDef()
	{
		def = new b2EdgeChainDef;
		vertices = NULL;
	}
	
	~Maxb2EdgeChainDef()
	{
		delete def;
		if (vertices) {
			delete [] vertices;
		}
	}

	void initVertices(int size) {
		if (vertices) {
			delete [] vertices;
			def->vertices = NULL;
		}
		
		vertices = new b2Vec2[size];
		def->vertexCount = size;
		def->vertices = vertices;
	}	
	
	b2EdgeChainDef * def;
	b2Vec2 * vertices;
};


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

bool bmx_b2abb_isvalid(b2AABB * aabb) {
	return aabb->IsValid();
}

// *****************************************************

b2Vec2 * bmx_b2vec2_new(b2Vec2 v) {
	b2Vec2 *vec = new b2Vec2(v.x, v.y);
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

void bmx_b2vec2_add(b2Vec2 * vec, b2Vec2 * other) {
	*vec += *other;
}

void bmx_b2vec2_copy(b2Vec2 * vec, b2Vec2 * other) {
	*vec = *other;
}

void bmx_b2vec2_set(b2Vec2 * vec, float32 x, float32 y) {
	vec->Set(x, y);
}

b2Vec2 * bmx_b2vec2_subtract(b2Vec2 * vec, b2Vec2 * other) {
	return bmx_b2vec2_new(*vec - *other);
}

float32 bmx_b2vec2_length(b2Vec2 * vec) {
	return vec->Length();
}

void bmx_b2vec2_multiply(b2Vec2 * vec, float32 value) {
	*vec *= value;
}

b2Vec2 * bmx_b2vec2_plus(b2Vec2 * vec, b2Vec2 * other) {
	return bmx_b2vec2_new(*vec + *other);
}

float32 bmx_b2vec2_normalize(b2Vec2 * vec) {
	return vec->Normalize();
}

float32 bmx_b2vec2_lengthsquared(b2Vec2 * vec) {
	return vec->LengthSquared();
}

BBArray * bmx_b2vec2_getvertexarray(const b2Vec2* vertices, int32 vertexCount) {
	BBArray * array = _bah_box2d_b2Vec2__newVecArray(vertexCount);
	for (int i = 0; i < vertexCount; i++) {
		_bah_box2d_b2Vec2__setVec(array, i, bmx_b2vec2_new(vertices[i]));
	}
	
	return array;
}

// *****************************************************

b2Body * bmx_b2world_createbody(b2World * world, b2BodyDef * def, BBObject * body) {
	def->userData = body;
	BBRETAIN(body);
	return world->CreateBody(def);
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

void bmx_b2world_setwarmstarting(b2World * world, bool flag) {
	world->SetWarmStarting(flag);
}

void bmx_b2world_setcontinuousphysics(b2World * world, bool flag) {
	world->SetContinuousPhysics(flag);
}

void bmx_b2world_validate(b2World * world) {
	world->Validate();
}

void bmx_b2world_setdebugDraw(b2World * world, b2DebugDraw * debugDraw) {
	world->SetDebugDraw(debugDraw);
}

b2Joint * bmx_b2world_createjoint(b2World * world, b2JointDef * def) {
	BBObject * joint = _bah_box2d_b2World__createJoint(def->type);
	def->userData = joint;
	BBRETAIN(joint);
	return world->CreateJoint(def);
}

void bmx_b2world_destroyjoint(b2World * world, b2Joint * joint) {
	void * data = joint->GetUserData();
	if (data && data != &bbNullObject) {
		BBRELEASE((BBObject*)data);
	}
	world->DestroyJoint(joint);
}

b2Body * bmx_b2world_getbodylist(b2World * world) {
	return world->GetBodyList();
}

b2Joint * bmx_b2world_getjointlist(b2World * world) {
	return world->GetJointList();
}

void bmx_b2world_setfilter(b2World * world, b2ContactFilter * filter) {
	world->SetContactFilter(filter);
}

void bmx_b2world_setcontactlistener(b2World * world, b2ContactListener * listener) {
	world->SetContactListener(listener);
}

void bmx_b2world_setboundarylistener(b2World * world, b2BoundaryListener * listener) {
	world->SetBoundaryListener(listener);
}

void bmx_b2world_setgravity(b2World * world, b2Vec2 * gravity) {
	world->SetGravity(*gravity);
}

int32 bmx_b2world_getproxycount(b2World * world) {
	return world->GetProxyCount();
}

int32 bmx_b2world_getpaircount(b2World * world) {
	return world->GetPairCount();
}

int32 bmx_b2world_getbodycount(b2World * world) {
	return world->GetBodyCount();
}

int32 bmx_b2world_getjointcount(b2World * world) {
	return world->GetJointCount();
}

int32 bmx_b2world_query(b2World * world, b2AABB * aabb, BBArray * shapes) {
	int32 n = shapes->scales[0];
	b2Shape* _shapes[n];
	
	int32 ret = world->Query(*aabb, _shapes, n);

	int32 count = (ret < n) ? ret : n;

	for (int i = 0; i < count; i++) {
		_bah_box2d_b2World__setShape(shapes, i, _shapes[i]);
	}

	return ret;
}

void bmx_b2world_free(b2World * world) {
	delete world;
}

void bmx_b2world_setdestructionlistener(b2World * world, b2DestructionListener * listener) {
	world->SetDestructionListener(listener);
}

void bmx_b2world_refilter(b2World * world, b2Shape * shape) {
	world->Refilter(shape);
}

int32 bmx_b2world_raycast(b2World * world, b2Segment * segment, BBArray * shapes, bool solidShapes) {
	int32 n = shapes->scales[0];
	b2Shape* _shapes[n];
	
	int32 ret = world->Raycast(*segment, _shapes, n, solidShapes, NULL);

	int32 count = (ret < n) ? ret : n;

	for (int i = 0; i < count; i++) {
		_bah_box2d_b2World__setShape(shapes, i, _shapes[i]);
	}

	return ret;
}

b2Shape * bmx_b2world_raycastone(b2World * world, b2Segment * segment, float32 * lambda, b2Vec2 * normal, bool solidShapes) {
	return world->RaycastOne(*segment, lambda, normal, solidShapes, NULL);
}

bool bmx_b2world_inrange(b2World * world, b2AABB * aabb) {
	return world->InRange(*aabb);
}

// *****************************************************

b2BodyDef * bmx_b2bodydef_create() {
	return new b2BodyDef;
}

void bmx_b2bodydef_delete(b2BodyDef * def) {
	delete def;
}

void bmx_b2bodydef_setposition(b2BodyDef * def, b2Vec2 * position) {
	def->position = *position;
}

void bmx_b2bodydef_setangle(b2BodyDef * def, float32 angle) {
	def->angle = angle / 57.2957795f;
}

void bmx_b2bodydef_setmassdata(b2BodyDef * def, b2MassData * data) {
	def->massData = *data;
}

bool bmx_b2bodydef_issleeping(b2BodyDef * def) {
	return def->isSleeping;
}

void bmx_b2bodydef_setissleeping(b2BodyDef * def, bool sleeping) {
	def->isSleeping = sleeping;
}

void bmx_b2bodydef_setfixedrotation(b2BodyDef * def, bool fixed) {
	def->fixedRotation = fixed;
}

bool bmx_b2bodydef_getfixedrotation(b2BodyDef * def) {
	return def->fixedRotation;
}

void bmx_b2bodydef_setisbullet(b2BodyDef * def, bool bullet) {
	def->isBullet = bullet;
}

void bmx_b2bodydef_setlineardamping(b2BodyDef * def, float32 damping) {
	def->linearDamping = damping;
}

float32 bmx_b2bodydef_getlineardamping(b2BodyDef * def) {
	return def->linearDamping;
}

void bmx_b2bodydef_setangulardamping(b2BodyDef * def, float32 damping) {
	def->angularDamping = damping;
}

float32 bmx_b2bodydef_getangulardamping(b2BodyDef * def) {
	return def->angularDamping;
}

void bmx_b2bodydef_setallowsleep(b2BodyDef * def, bool allow) {
	def->allowSleep = allow;
}

bool bmx_b2bodydef_getallowsleep(b2BodyDef * def) {
	return def->allowSleep;
}

b2Vec2 * bmx_b2bodydef_getposition(b2BodyDef * def) {
	return bmx_b2vec2_new(def->position);
}

void bmx_b2bodydef_setpositionxy(b2BodyDef * def, float32 x, float32 y) {
	def->position = b2Vec2(x, y);
}

// *****************************************************

b2World * bmx_b2world_create(b2AABB * worldAABB, b2Vec2 * gravity, bool doSleep) {
	return new b2World(*worldAABB, *gravity, doSleep);
}

void bmx_b2world_dostep(b2World * world, float32 timeStep, int velocityIterations, int positionIterations) {
	world->Step(timeStep, velocityIterations, positionIterations);
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

void bmx_b2shapedef_setfilter(b2ShapeDef * def, MaxFilterData * filterData) {
	def->filter = filterData->getData();
}

MaxFilterData* bmx_b2shapedef_getfilter(b2ShapeDef * def) {
	return new MaxFilterData(def->filter);
}

void bmx_b2shapedef_setissensor(b2ShapeDef * def, bool sensor) {
	def->isSensor = sensor;
}

bool bmx_b2shapedef_issensor(b2ShapeDef * def) {
	return def->isSensor;
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

void bmx_b2polygondef_setasorientedbox(b2PolygonDef * def, float32 hx, float32 hy, b2Vec2 * center, float32 angle) {
	def->SetAsBox(hx, hy, *center, angle * 0.0174533f);
}

void bmx_b2polygondef_setvertices(b2PolygonDef * def, BBArray * vertices) {
	int n = vertices->scales[0];
	def->vertexCount = n;
	
	for (int i = 0; i < n; i++) {
		def->vertices[i] = *_bah_box2d_b2Vec2__getVec(vertices, i);

	}
}

// *****************************************************

b2Shape * bmx_b2body_createshape(b2Body * body, b2ShapeDef * def) {
	BBObject * shape  = _bah_box2d_b2Body__createShape(def->type);
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

b2Vec2 * bmx_b2body_getworldcenter(b2Body * body) {
	return bmx_b2vec2_new(body->GetWorldCenter());
}

b2Vec2 * bmx_b2body_getlocalcenter(b2Body * body) {
	return bmx_b2vec2_new(body->GetLocalCenter());
}

void bmx_b2body_setlinearvelocity(b2Body * body, b2Vec2 * v) {
	body->SetLinearVelocity(*v);
}

b2Vec2 * bmx_b2body_getlinearvelocity(b2Body * body) {
	return bmx_b2vec2_new(body->GetLinearVelocity());
}

void bmx_b2body_setangularvelocity(b2Body * body, float32 omega) {
	body->SetAngularVelocity(omega * 0.0174533f);
}

float32 bmx_b2body_getangularvelocity(b2Body * body) {
	return body->GetAngularVelocity() * 57.2957795f;
}

void bmx_b2body_applyforce(b2Body * body, b2Vec2 * force, b2Vec2 * point) {
	body->ApplyForce(*force, *point);
}

void bmx_b2body_applytorque(b2Body * body, float32 torque) {
	body->ApplyTorque(torque);
}

void bmx_b2body_applyimpulse(b2Body * body, b2Vec2 * impulse, b2Vec2 * point) {
	body->ApplyImpulse(*impulse, *point);
}

float32 bmx_b2body_getmass(b2Body * body) {
	return body->GetMass();
}

float32 bmx_b2body_getinertia(b2Body * body) {
	return body->GetInertia();
}

b2Vec2 * bmx_b2body_getworldpoint(b2Body * body, b2Vec2 * localPoint) {
	return bmx_b2vec2_new(body->GetWorldPoint(*localPoint));
}

b2Vec2 * bmx_b2body_getworldvector(b2Body * body, b2Vec2 * localVector) {
	return bmx_b2vec2_new(body->GetWorldVector(*localVector));
}

b2Vec2 * bmx_b2body_getlocalpoint(b2Body * body, b2Vec2 * worldPoint) {
	return bmx_b2vec2_new(body->GetLocalPoint(*worldPoint));
}

b2Vec2 * bmx_b2body_getlocalvector(b2Body * body, b2Vec2 * worldVector) {
	return bmx_b2vec2_new(body->GetLocalVector(*worldVector));
}

b2JointEdge * bmx_b2body_getjointlist(b2Body * body) {
	return body->GetJointList();
}

const b2XForm * bmx_b2body_getxform(b2Body * body) {
	return &body->GetXForm();
}

bool bmx_b2body_setxform(b2Body * body, b2Vec2 * position, float32 angle) {
	body->SetXForm(*position, angle * 0.0174533f);
}

b2World * bmx_b2body_getworld(b2Body * body) {
	return body->GetWorld();
}

// *****************************************************

class MaxDebugDraw : public b2DebugDraw
{
public:

	MaxDebugDraw(BBObject * handle)
		: maxHandle(handle)
	{
	}

	~MaxDebugDraw() {}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		BBArray * array = bmx_b2vec2_getvertexarray(vertices, vertexCount);
		_bah_box2d_b2DebugDraw__DrawPolygon(maxHandle, array, 
			static_cast<int>(color.r * 255.0f),
			static_cast<int>(color.g * 255.0f),
			static_cast<int>(color.b * 255.0f));
	}

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		BBArray * array = bmx_b2vec2_getvertexarray(vertices, vertexCount);
		_bah_box2d_b2DebugDraw__DrawSolidPolygon(maxHandle, array, 
			static_cast<int>(color.r * 255.0f),
			static_cast<int>(color.g * 255.0f),
			static_cast<int>(color.b * 255.0f));
	}

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	}

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
		_bah_box2d_b2DebugDraw__DrawSolidCircle(maxHandle, bmx_b2vec2_new(center), radius, 
			bmx_b2vec2_new(axis), 
			static_cast<int>(color.r * 255.0f),
			static_cast<int>(color.g * 255.0f),
			static_cast<int>(color.b * 255.0f));
	}

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
		_bah_box2d_b2DebugDraw__DrawSegment(maxHandle, bmx_b2vec2_new(p1), bmx_b2vec2_new(p2), 
			static_cast<int>(color.r * 255.0f),
			static_cast<int>(color.g * 255.0f),
			static_cast<int>(color.b * 255.0f));
	}

	void DrawXForm(const b2XForm& xf) {
	
	}
	
	void SetFlags(uint32 flags) {
		b2DebugDraw::m_drawFlags = flags;
	}
	
private:
	BBObject * maxHandle;
};


MaxDebugDraw * bmx_b2debugdraw_create(BBObject * handle) {
	return new MaxDebugDraw(handle);
}

void bmx_b2debugdraw_setflags(MaxDebugDraw * dbg, uint32 flags) {
	dbg->SetFlags(flags);
}

uint32 bmx_b2debugdraw_getflags(MaxDebugDraw * dbg) {
	return dbg->GetFlags();
}

void bmx_b2debugdraw_appendflags(MaxDebugDraw * dbg, uint32 flags) {
	dbg->AppendFlags(flags);
}

void bmx_b2debugdraw_clearflags(MaxDebugDraw * dbg, uint32 flags) {
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

float32 bmx_b2circledef_getradius(b2CircleDef * def) {
	return def->radius;
}

b2Vec2 * bmx_b2circledef_getlocalposition(b2CircleDef * def) {
	return bmx_b2vec2_new(def->localPosition);
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

b2Shape * bmx_b2shape_getnext(b2Shape * shape) {
	return shape->GetNext();
}

bool bmx_b2shape_testpoint(b2Shape * shape, b2XForm * xf, b2Vec2 * p) {
	return shape->TestPoint(*xf, *p);
}

float32 bmx_b2shape_getsweepradius(b2Shape * shape) {
	return shape->GetSweepRadius();
}

float32 bmx_b2shape_getfriction(b2Shape * shape) {
	return shape->GetFriction();
}

float32 bmx_b2shape_getrestitution(b2Shape * shape) {
	return shape->GetRestitution();
}

void bmx_b2shape_computeaabb(b2Shape * shape, b2AABB * aabb, b2XForm * xf) {
	shape->ComputeAABB(aabb, *xf);
}

void bmx_b2shape_computesweptaabb(b2Shape * shape, b2AABB * aabb, b2XForm * xf1, b2XForm * xf2) {
	shape->ComputeSweptAABB(aabb, *xf1, *xf2);
}

void bmx_b2shape_computemass(b2Shape * shape, b2MassData * data) {
	shape->ComputeMass(data);
}

MaxFilterData * bmx_b2shape_getfilterdata(b2Shape * shape) {
	return new MaxFilterData(shape->GetFilterData());
}

void bmx_b2shape_setfilterdata(b2Shape * shape, MaxFilterData * data) {
	shape->SetFilterData(data->getData());
}

void bmx_b2shape_setfriction(b2Shape * shape, float32 friction) {
	shape->SetFriction(friction);
}

void bmx_b2shape_setrestitution(b2Shape * shape, float32 restitution) {
	shape->SetRestitution(restitution);
}

// *****************************************************

b2RevoluteJointDef * bmx_b2revolutejointdef_create() {
	return new b2RevoluteJointDef;
}

void bmx_b2revolutejointdef_initialize(b2RevoluteJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor) {
	def->Initialize(body1, body2, *anchor);
}

void bmx_b2revolutejointdef_delete(b2RevoluteJointDef * def) {
	delete def;
}

bool bmx_b2revolutejointdef_islimitenabled(b2RevoluteJointDef * def) {
    return def->enableLimit;
}

void bmx_b2revolutejointdef_enablelimit(b2RevoluteJointDef * def, bool limit) {
    def->enableLimit = limit;
}

float32 bmx_b2revolutejointdef_getlowerangle(b2RevoluteJointDef * def) {
    return def->lowerAngle * 57.2957795f;
}

void bmx_b2revolutejointdef_setlowerangle(b2RevoluteJointDef * def, float32 angle) {
    def->lowerAngle = angle * 0.0174533f;
}

float32 bmx_b2revolutejointdef_getupperangle(b2RevoluteJointDef * def) {
    return def->upperAngle * 57.2957795f;
}

void bmx_b2revolutejointdef_setupperangle(b2RevoluteJointDef * def, float32 angle) {
    def->upperAngle = angle * 0.0174533f;
}

bool bmx_b2revolutejointdef_ismotorenabled(b2RevoluteJointDef * def) {
    return def->enableMotor;
}

void bmx_b2revolutejointdef_enablemotor(b2RevoluteJointDef * def, bool value) {
    def->enableMotor = value;
}

float32 bmx_b2revolutejointdef_getmotorspeed(b2RevoluteJointDef * def) {
    return def->motorSpeed * 57.2957795f;
}

void bmx_b2revolutejointdef_setmotorspeed(b2RevoluteJointDef * def, float32 speed) {
    def->motorSpeed = speed * 0.0174533f;
}

float32 bmx_b2revolutejointdef_getmaxmotortorque(b2RevoluteJointDef * def) {
    return def->maxMotorTorque;
}

void bmx_b2revolutejointdef_setmaxmotortorque(b2RevoluteJointDef * def, float32 torque) {
    def->maxMotorTorque = torque;
}

b2Vec2 * bmx_b2revolutejointdef_getlocalanchor1(b2RevoluteJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor1);
}

void bmx_b2revolutejointdef_setlocalanchor1(b2RevoluteJointDef * def, b2Vec2 *anchor) {
	def->localAnchor1 = *anchor;
}

b2Vec2 * bmx_b2revolutejointdef_getlocalanchor2(b2RevoluteJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor2);
}

void bmx_b2revolutejointdef_setlocalanchor2(b2RevoluteJointDef * def, b2Vec2 *anchor) {
	def->localAnchor2 = *anchor;
}

float32 bmx_b2revolutejointdef_getreferenceangle(b2RevoluteJointDef * def) {
	return def->referenceAngle * 57.2957795f;
}

void bmx_b2revolutejointdef_setreferenceangle(b2RevoluteJointDef * def, float32 angle) {
	def->referenceAngle = angle * 0.0174533f;
}


// *****************************************************

BBObject * bmx_b2joint_getmaxjoint(b2Joint * joint) {
	void * obj = joint->GetUserData();
	if (obj) {
		return (BBObject *)obj;
	}
	return &bbNullObject;
}

b2Body * bmx_b2joint_getbody1(b2Joint * joint) {
	return joint->GetBody1();
}

b2Body * bmx_b2joint_getbody2(b2Joint * joint) {
	return joint->GetBody2();
}

b2Joint * bmx_b2joint_getnext(b2Joint * joint) {
	return joint->GetNext();
}

// *****************************************************

b2MassData * bmx_b2massdata_new() {
	return new b2MassData;
}

void bmx_b2massdata_delete(b2MassData * data) {
	delete data;
}

void bmx_b2massdata_setmass(b2MassData * data, float32 mass) {
	data->mass = mass;
}

void bmx_b2massdata_setcenter(b2MassData * data, b2Vec2 * center) {
	data->center = *center;
}

void bmx_b2massdata_seti(b2MassData * data, float32 i) {
	data->I = i;
}

// *****************************************************

b2Body * bmx_b2jointedge_getother(b2JointEdge * joint) {
	return joint->other;
}

b2Joint * bmx_b2jointedge_getjoint(b2JointEdge * joint) {
	return joint->joint;
}

b2JointEdge * bmx_b2jointedge_getprev(b2JointEdge * joint) {
	return joint->prev;
}

b2JointEdge * bmx_b2jointedge_getnext(b2JointEdge * joint) {
	return joint->next;
}


// *****************************************************

class MaxContactFilter : public b2ContactFilter
{
public:
	MaxContactFilter::MaxContactFilter(BBObject * handle)
		: maxHandle(handle)
	{
	}
	
	bool MaxContactFilter::ShouldCollide(b2Shape* shape1, b2Shape* shape2) {
		return _bah_box2d_b2ContactFilter__ShouldCollide(maxHandle, shape1, shape2);
	}


private:
	BBObject * maxHandle;
};

MaxContactFilter * bmx_b2contactfilter_new(BBObject * handle) {
	return new MaxContactFilter(handle);
}

void bmx_b2contactfilter_delete(MaxContactFilter * filter) {
	delete filter;
}


// *****************************************************

class MaxContactListener : public b2ContactListener
{
public:
	MaxContactListener::MaxContactListener(BBObject * handle)
		: maxHandle(handle)
	{
	}
	
	void MaxContactListener::Add(const b2ContactPoint* point) {
		_bah_box2d_b2ContactListener__Add(maxHandle, point);
	}

	void MaxContactListener::Persist(const b2ContactPoint* point) {
		_bah_box2d_b2ContactListener__Persist(maxHandle, point);
	}

	void MaxContactListener::Remove(const b2ContactPoint* point) {
		_bah_box2d_b2ContactListener__Remove(maxHandle, point);
	}

	void MaxContactListener::Result(const b2ContactResult* result) {
		_bah_box2d_b2ContactListener__Result(maxHandle, result);
	}

private:
	BBObject * maxHandle;
};

MaxContactListener * bmx_b2contactlistener_new(BBObject * handle) {
	return new MaxContactListener(handle);
}

void bmx_b2contactlistener_delete(MaxContactListener * listener) {
	delete listener;
}

// *****************************************************

class MaxBoundaryListener : public b2BoundaryListener
{
public:
	MaxBoundaryListener::MaxBoundaryListener(BBObject * handle)
		: maxHandle(handle)
	{
	}
	
	void MaxBoundaryListener::Violation(b2Body* body) {
		_bah_box2d_b2BoundaryListener__Violation(maxHandle, body);
	}
	
private:
	BBObject * maxHandle;
};


MaxBoundaryListener * bmx_b2boundarylistener_new(BBObject * handle) {
	return new MaxBoundaryListener(handle);
}

void bmx_b2boundarylistener_delete(MaxBoundaryListener * listener) {
	delete listener;
}

// *****************************************************

void bmx_b2jointdef_setcollideconnected(b2JointDef * def, bool collideConnected) {
	def->collideConnected = collideConnected;
}

bool bmx_b2jointdef_getcollideconnected(b2JointDef * def) {
	return def->collideConnected;
}

void bmx_b2jointdef_setbody1(b2JointDef * def, b2Body * body) {
	def->body1 = body;
}

b2Body * bmx_b2jointdef_getbody1(b2JointDef * def) {
	return def->body1;
}

void bmx_b2jointdef_setbody2(b2JointDef * def, b2Body * body) {
	def->body2 = body;
}

b2Body * bmx_b2jointdef_getbody2(b2JointDef * def) {
	return def->body2;
}

// *****************************************************

b2DistanceJointDef * bmx_b2distancejointdef_new() {
	return new b2DistanceJointDef;
}

void bmx_b2distancejointdef_setlocalanchor1(b2DistanceJointDef * def, b2Vec2 * anchor) {
	def->localAnchor1 = *anchor;
}

b2Vec2 * bmx_b2distancejointdef_getlocalanchor1(b2DistanceJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor1);
}

void bmx_b2distancejointdef_setlocalanchor2(b2DistanceJointDef * def, b2Vec2 * anchor) {
	def->localAnchor2 = *anchor;
}

b2Vec2 * bmx_b2distancejointdef_getlocalanchor2(b2DistanceJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor2);
}

void bmx_b2distancejointdef_setlength(b2DistanceJointDef * def, float32 length) {
	def->length = length;
}

float32 bmx_b2distancejointdef_getlength(b2DistanceJointDef * def) {
	return def->length;
}

void bmx_b2distancejointdef_delete(b2DistanceJointDef * def) {
	delete def;
}

void bmx_b2distancejointdef_setfrequencyhz(b2DistanceJointDef * def, float32 freq) {
	def->frequencyHz = freq;
}

void bmx_b2distancejointdef_setdampingratio(b2DistanceJointDef * def, float32 ratio) {
	def->dampingRatio = ratio;
}

void bmx_b2distancejointdef_initialize(b2DistanceJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor1, b2Vec2 * anchor2) {
	def->Initialize(body1, body2, *anchor1, *anchor2);
}


// *****************************************************

b2PrismaticJointDef * bmx_b2prismaticjointdef_create() {
    return new b2PrismaticJointDef;
}

void bmx_b2prismaticjointdef_enablelimit(b2PrismaticJointDef * def, bool value) {
    def->enableLimit = value;
}

bool bmx_b2prismaticjointdef_islimitenabled(b2PrismaticJointDef * def) {
    return def->enableLimit;
}

void bmx_b2prismaticjointdef_setlowertranslation(b2PrismaticJointDef * def, float32 translation) {
    def->lowerTranslation = translation;
}

float32 bmx_b2prismaticjointdef_getlowertranslation(b2PrismaticJointDef * def) {
    return def->lowerTranslation;
}

void bmx_b2prismaticjointdef_setuppertranslation(b2PrismaticJointDef * def, float32 translation) {
    def->upperTranslation = translation;
}

float32 bmx_b2prismaticjointdef_getuppertranslation(b2PrismaticJointDef * def) {
    return def->upperTranslation;
}

void bmx_b2prismaticjointdef_enablemotor(b2PrismaticJointDef * def, bool value) {
    def->enableMotor = value;
}

bool bmx_b2prismaticjointdef_ismotorenabled(b2PrismaticJointDef * def) {
    return def->enableMotor;
}

void bmx_b2prismaticjointdef_setmaxmotorforce(b2PrismaticJointDef * def, float32 force) {
    def->maxMotorForce = force;
}

float32 bmx_b2prismaticjointdef_getmaxmotorforce(b2PrismaticJointDef * def) {
    return def->maxMotorForce;
}

void bmx_b2prismaticjointdef_setmotorspeed(b2PrismaticJointDef * def, float32 speed) {
    def->motorSpeed = speed * 0.0174533f;
}

float32 bmx_b2prismaticjointdef_getmotorspeed(b2PrismaticJointDef * def) {
    return def->motorSpeed * 57.2957795f;
}

void bmx_b2prismaticjointdef_delete(b2PrismaticJointDef * def) {
    delete def;
}

void bmx_b2prismaticjointdef_initialize(b2PrismaticJointDef * def, b2Body * body1, b2Body * body2,
		b2Vec2 * anchor, b2Vec2 * axis) {
	def->Initialize(body1, body2, *anchor, *axis);
}

void bmx_b2prismaticjointdef_setlocalanchor1(b2PrismaticJointDef * def, b2Vec2 * anchor) {
	def->localAnchor1 = *anchor;
}

b2Vec2 * bmx_b2prismaticjointdef_getlocalanchor1(b2PrismaticJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor1);
}

void bmx_b2prismaticjointdef_setlocalanchor2(b2PrismaticJointDef * def, b2Vec2 * anchor) {
	def->localAnchor2 = *anchor;
}

b2Vec2 * bmx_b2prismaticjointdef_getlocalanchor2(b2PrismaticJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor2);
}

void bmx_b2prismaticjointdef_setlocalaxis1(b2PrismaticJointDef * def, b2Vec2 * axis) {
	def->localAxis1 = *axis;
}

b2Vec2 * bmx_b2prismaticjointdef_getlocalaxis1(b2PrismaticJointDef * def) {
	return bmx_b2vec2_new(def->localAxis1);
}

void bmx_b2prismaticjointdef_setreferenceangle(b2PrismaticJointDef * def, float32 angle) {
	def->referenceAngle = angle * 0.0174533f;
}

float32 bmx_b2prismaticjointdef_getreferenceangle(b2PrismaticJointDef * def) {
	return def->referenceAngle * 57.2957795f;
}


// *****************************************************

float32 bmx_b2revolutejoint_getlowerlimit(b2RevoluteJoint * joint) {
    return joint->GetLowerLimit() * 57.2957795f;
}

float32 bmx_b2revolutejoint_getupperlimit(b2RevoluteJoint * joint) {
    return joint->GetUpperLimit() * 57.2957795f;
}

void bmx_b2revolutejoint_setlimits(b2RevoluteJoint * joint, float32 lowerLimit, float32 upperLimit) {
    joint->SetLimits(lowerLimit * 0.0174533f, upperLimit * 0.0174533f);
}

bool bmx_b2revolutejoint_ismotorenabled(b2RevoluteJoint * joint) {
    return joint->IsMotorEnabled();
}

void bmx_b2revolutejoint_enablemotor(b2RevoluteJoint * joint, bool flag) {
    joint->EnableMotor(flag);
}

void bmx_b2revolutejoint_setmotorspeed(b2RevoluteJoint * joint, float32 speed) {
    joint->SetMotorSpeed(speed * 0.0174533f);
}

float32 bmx_b2revolutejoint_getmotorspeed(b2RevoluteJoint * joint) {
    return joint->GetMotorSpeed() * 57.2957795f;
}

void bmx_b2revolutejoint_setmaxmotortorque(b2RevoluteJoint * joint, float32 torque) {
    joint->SetMaxMotorTorque(torque);
}

float32 bmx_b2revolutejoint_getmotortorque(b2RevoluteJoint * joint) {
    return joint->GetMotorTorque();
}

bool bmx_b2revolutejoint_islimitenabled(b2RevoluteJoint * joint) {
	return joint->IsLimitEnabled();
}

void bmx_b2revolutejoint_enablelimit(b2RevoluteJoint * joint, bool flag) {
	joint->EnableLimit(flag);
}

b2Vec2 * bmx_b2revolutejoint_getanchor1(b2RevoluteJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2revolutejoint_getanchor2(b2RevoluteJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2revolutejoint_getreactionforce(b2RevoluteJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2revolutejoint_getreactiontorque(b2RevoluteJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

float32 bmx_b2revolutejoint_getjointangle(b2RevoluteJoint * joint) {
	return joint->GetJointAngle() * 57.2957795f;
}

float32 bmx_b2revolutejoint_getjointspeed(b2RevoluteJoint * joint) {
	return joint->GetJointSpeed() * 57.2957795f;
}


// *****************************************************

float32 bmx_b2prismaticjoint_getjointspeed(b2PrismaticJoint * joint) {
    return joint->GetJointSpeed();
}

bool bmx_b2prismaticjoint_islimitenabled(b2PrismaticJoint * joint) {
    return joint->IsLimitEnabled();
}

void bmx_b2prismaticjoint_enablelimit(b2PrismaticJoint * joint, bool flag) {
    joint->EnableLimit(flag);
}

float32 bmx_b2prismaticjoint_getlowerlimit(b2PrismaticJoint * joint) {
    return joint->GetLowerLimit();
}

float32 bmx_b2prismaticjoint_getupperlimit(b2PrismaticJoint * joint) {
    return joint->GetUpperLimit();
}

void bmx_b2prismaticjoint_setlimits(b2PrismaticJoint * joint, float32 lowerLimit, float32 upperLimit) {
    joint->SetLimits(lowerLimit, upperLimit);
}

bool bmx_b2prismaticjoint_ismotorenabled(b2PrismaticJoint * joint) {
    return joint->IsMotorEnabled();
}

void bmx_b2prismaticjoint_enablemotor(b2PrismaticJoint * joint, bool flag) {
    joint->EnableMotor(flag);
}

void bmx_b2prismaticjoint_setmotorspeed(b2PrismaticJoint * joint, float32 speed) {
    joint->SetMotorSpeed(speed);
}

float32 bmx_b2prismaticjoint_getmotorspeed(b2PrismaticJoint * joint) {
    return joint->GetMotorSpeed();
}

void bmx_b2prismaticjoint_setmaxmotorforce(b2PrismaticJoint * joint, float32 force) {
    joint->SetMaxMotorForce(force);
}

float32 bmx_b2prismaticjoint_getmotorforce(b2PrismaticJoint * joint) {
    return joint->GetMotorForce();
}

b2Vec2 * bmx_b2prismaticjoint_getanchor1(b2PrismaticJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2prismaticjoint_getanchor2(b2PrismaticJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2prismaticjoint_getreactionforce(b2PrismaticJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2prismaticjoint_getreactiontorque(b2PrismaticJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

float32 bmx_b2prismaticjoint_getjointtranslation(b2PrismaticJoint * joint) {
	return joint->GetJointTranslation();
}


// *****************************************************

b2Vec2 * bmx_b2cross(b2Vec2 * a, float32 s) {
	return bmx_b2vec2_new(b2Cross(*a, s));
}

b2Vec2 * bmx_b2crossf(float32 s, b2Vec2 * a) {
	return bmx_b2vec2_new(b2Cross(s, *a));
}

b2Vec2 * bmx_b2mul(b2Mat22 * A, b2Vec2 * v) {
	return bmx_b2vec2_new(b2Mul(*A, *v));
}

b2Vec2 * bmx_b2mult(b2Mat22 * A, b2Vec2 * v) {
	return bmx_b2vec2_new(b2MulT(*A, *v));
}

b2Vec2 * bmx_b2mulf(b2XForm * T, b2Vec2 * v) {
	return bmx_b2vec2_new(b2Mul(*T, *v));
}

b2Vec2 * bmx_b2multf(b2XForm * T, b2Vec2 * v) {
	return bmx_b2vec2_new(b2MulT(*T, *v));
}

float32 bmx_b2dot(b2Vec2 * a, b2Vec2 * b) {
	return b2Dot(*a, *b);
}

// *****************************************************

b2XForm * bmx_b2xform_create() {
	return new b2XForm;
}

b2Vec2 * bmx_b2xform_getposition(b2XForm * form) {
	return bmx_b2vec2_new(form->position);
}

void bmx_b2xform_setposition(b2XForm * form, b2Vec2 * pos) {
	form->position = *pos;
}

b2Mat22 * bmx_b2xform_getr(b2XForm * form) {
	return &form->R;
}

void bmx_b2xform_setr(b2XForm * form, b2Mat22 * r) {
	form->R = *r;
}

void bmx_b2xform_delete(b2XForm * form) {
	delete form;
}

// *****************************************************

void bmx_b2mat22_setangle(b2Mat22 * mat, float32 angle) {
	mat->Set(angle * 0.0174533f);
}

float32 bmx_b2mat22_getangle(b2Mat22 * mat) {
	return mat->GetAngle() * 57.2957795f;
}

// *****************************************************

b2Shape * bmx_b2contact_getshape1(b2Contact * contact) {
	return contact->GetShape1();
}

b2Shape * bmx_b2contact_getshape2(b2Contact * contact) {
	return contact->GetShape2();
}

b2Contact * bmx_b2contact_getnext(b2Contact * contact) {
	return contact->GetNext();
}

bool bmx_b2contact_issolid(b2Contact * contact) {
	return contact->IsSolid();
}

int32 bmx_b2contact_getmanifoldcount(b2Contact * contact) {
	return contact->GetManifoldCount();
}

// *****************************************************

b2Shape * bmx_b2contactresult_getshape1(b2ContactResult * contactResult) {
	return contactResult->shape1;
}

b2Shape * bmx_b2contactresult_getshape2(b2ContactResult * contactResult) {
	return contactResult->shape2;
}

b2Vec2 * bmx_b2contactresult_getposition(b2ContactResult * contactResult) {
	return bmx_b2vec2_new(contactResult->position);
}

b2Vec2 * bmx_b2contactresult_getnormal(b2ContactResult * contactResult) {
	return bmx_b2vec2_new(contactResult->normal);
}

float32 bmx_b2contactresult_getnormalimpulse(b2ContactResult * contactResult) {
	return contactResult->normalImpulse;
}

float32 bmx_b2contactresult_gettangentimpulse(b2ContactResult * contactResult) {
	return contactResult->tangentImpulse;
}

// *****************************************************

b2Shape * bmx_b2contactpoint_getshape1(b2ContactPoint * contactPoint) {
	return contactPoint->shape1;
}

b2Shape * bmx_b2contactpoint_getshape2(b2ContactPoint * contactPoint) {
	return contactPoint->shape2;
}

b2Vec2 * bmx_b2contactpoint_getposition(b2ContactPoint * contactPoint) {
	return bmx_b2vec2_new(contactPoint->position);
}

b2Vec2 * bmx_b2contactpoint_getvelocity(b2ContactPoint * contactPoint) {
	return bmx_b2vec2_new(contactPoint->velocity);
}

b2Vec2 * bmx_b2contactpoint_getnormal(b2ContactPoint * contactPoint) {
	return bmx_b2vec2_new(contactPoint->normal);
}

float32 bmx_b2contactpoint_getseparation(b2ContactPoint * contactPoint) {
	return contactPoint->separation;
}

float32 bmx_b2contactpoint_getfriction(b2ContactPoint * contactPoint) {
	return contactPoint->friction;
}

float32 bmx_b2contactpoint_getrestitution(b2ContactPoint * contactPoint) {
	return contactPoint->restitution;
}


// *****************************************************

MaxFilterData * bmx_b2filterdata_create() {
	return new MaxFilterData();
}

uint16  bmx_b2filterdata_getcategorybits(MaxFilterData * filterData) {
	return filterData->getData().categoryBits;
}

void bmx_b2filterdata_setcategorybits(MaxFilterData * filterData, uint16 categoryBits) {
	filterData->setCategoryBits(categoryBits);
}

uint16  bmx_b2filterdata_getmaskbits(MaxFilterData * filterData) {
	return filterData->getData().maskBits;
}

void bmx_b2filterdata_setmaskbits(MaxFilterData * filterData, uint16 maskBits) {
	filterData->setMaskBits(maskBits);
}

int16 bmx_b2filterdata_getgroupindex(MaxFilterData * filterData) {
	return filterData->getData().groupIndex;
}

void bmx_b2filterdata_setgroupindex(MaxFilterData * filterData, int16 index) {
	filterData->setGroupIndex(index);
}

void bmx_b2filterdata_delete(MaxFilterData * filterData) {
	delete filterData;
}

// *****************************************************

b2GearJointDef * bmx_b2gearjointdef_new() {
	return new b2GearJointDef;
}

void bmx_b2gearjointdef_setjoint1(b2GearJointDef * def, b2Joint * joint) {
	def->joint1 = joint;
}

void bmx_b2gearjointdef_setjoint2(b2GearJointDef * def, b2Joint * joint) {
	def->joint2 = joint;
}

void bmx_b2gearjointdef_setratio(b2GearJointDef * def, float32 ratio) {
	def->ratio = ratio;
}

void bmx_b2gearjointdef_delete(b2GearJointDef * def) {
	delete def;
}

// *****************************************************

b2Vec2 * bmx_b2gearjoint_getanchor1(b2GearJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2gearjoint_getanchor2(b2GearJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2gearjoint_getreactionforce(b2GearJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2gearjoint_getreactiontorque(b2GearJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

float32 bmx_b2gearjoint_getratio(b2GearJoint * joint) {
	return joint->GetRatio();
}

// *****************************************************

b2Vec2 * bmx_b2mousejoint_getanchor1(b2MouseJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2mousejoint_getanchor2(b2MouseJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2mousejoint_getreactionforce(b2MouseJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2mousejoint_getreactiontorque(b2MouseJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

void bmx_b2mousejoint_settarget(b2MouseJoint * joint, b2Vec2 * target) {
	joint->SetTarget(*target);
}

b2Vec2 * bmx_b2mousejoint_gettarget(b2MouseJoint * joint) {
	return bmx_b2vec2_new(joint->m_target);
}

b2Vec2 * bmx_b2mousejoint_getlocalanchor(b2MouseJoint * joint) {
	return bmx_b2vec2_new(joint->m_localAnchor);
}

// *****************************************************

b2Vec2 * bmx_b2pulleyjoint_getanchor1(b2PulleyJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2pulleyjoint_getanchor2(b2PulleyJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2pulleyjoint_getreactionforce(b2PulleyJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2pulleyjoint_getreactiontorque(b2PulleyJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

b2Vec2 * bmx_b2pulleyjoint_getgroundanchor1(b2PulleyJoint * joint) {
	return bmx_b2vec2_new(joint->GetGroundAnchor1());
}

b2Vec2 * bmx_b2pulleyjoint_getgroundanchor2(b2PulleyJoint * joint) {
	return bmx_b2vec2_new(joint->GetGroundAnchor2());
}

float32 bmx_b2pulleyjoint_getlength1(b2PulleyJoint * joint) {
	return joint->GetLength1();
}

float32 bmx_b2pulleyjoint_getlength2(b2PulleyJoint * joint) {
	return joint->GetLength2();
}

float32 bmx_b2pulleyjoint_getratio(b2PulleyJoint * joint) {
	return joint->GetRatio();
}

// *****************************************************

b2Vec2 * bmx_b2distancejoint_getanchor1(b2DistanceJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2distancejoint_getanchor2(b2DistanceJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2distancejoint_getreactionforce(b2DistanceJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2distancejoint_getreactiontorque(b2DistanceJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

// *****************************************************

b2MouseJointDef * bmx_b2mousejointdef_new() {
	return new b2MouseJointDef;
}

void bmx_b2mousejointdef_settarget(b2MouseJointDef * def, b2Vec2 * target) {
	def->target = *target;
}

b2Vec2 * bmx_b2mousejointdef_gettarget(b2MouseJointDef * def) {
	return bmx_b2vec2_new(def->target);
}

void bmx_b2mousejointdef_setmaxforce(b2MouseJointDef * def, float32 maxForce) {
	def->maxForce = maxForce;
}

float32 bmx_b2mousejointdef_getmaxforce(b2MouseJointDef * def) {
	return def->maxForce;
}

void bmx_b2mousejointdef_setfrequencyhz(b2MouseJointDef * def, float32 frequency) {
	def->frequencyHz = frequency;
}

float32 bmx_b2mousejointdef_getfrequencyhz(b2MouseJointDef * def) {
	return def->frequencyHz;
}

void bmx_b2mousejointdef_setdampingration(b2MouseJointDef * def, float32 ratio) {
	def->dampingRatio = ratio;
}

float32 bmx_b2mousejointdef_getdampingratio(b2MouseJointDef * def) {
	return def->dampingRatio;
}

void bmx_b2mousejointdef_delete(b2MouseJointDef * def) {
	delete def;
}


// *****************************************************

b2PulleyJointDef * bmx_b2pulleyjointdef_create() {
	return new b2PulleyJointDef;
}

void bmx_b2pulleyjointdef_initialize(b2PulleyJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * groundAnchor1, 
		b2Vec2 * groundAnchor2, b2Vec2 * anchor1, b2Vec2 * anchor2, float32 ratio) {
	def->Initialize(body1, body2, *groundAnchor1, *groundAnchor2, *anchor1, *anchor2, ratio);
}

void bmx_b2pulleyjointdef_setgroundanchor1(b2PulleyJointDef * def, b2Vec2 * anchor) {
	def->groundAnchor1 = *anchor;
}

b2Vec2 * bmx_b2pulleyjointdef_getgroundanchor1(b2PulleyJointDef * def) {
	return bmx_b2vec2_new(def->groundAnchor1);
}

void bmx_b2pulleyjointdef_setgroundanchor2(b2PulleyJointDef * def, b2Vec2 * anchor) {
	def->groundAnchor2 = *anchor;
}

b2Vec2 * bmx_b2pulleyjointdef_getgroundanchor2(b2PulleyJointDef * def) {
	return bmx_b2vec2_new(def->groundAnchor2);
}

void bmx_b2pulleyjointdef_setlocalanchor1(b2PulleyJointDef * def, b2Vec2 * anchor) {
	def->localAnchor1 = *anchor;
}

b2Vec2 * bmx_b2pulleyjointdef_getlocalanchor1(b2PulleyJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor1);
}

void bmx_b2pulleyjointdef_setlocalanchor2(b2PulleyJointDef * def, b2Vec2 * anchor) {
	def->localAnchor2 = *anchor;
}

b2Vec2 * bmx_b2pulleyjointdef_getlocalanchor2(b2PulleyJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor2);
}

void bmx_b2pulleyjointdef_setlength1(b2PulleyJointDef * def, float32 length) {
	def->length1 = length;
}

float32 bmx_b2pulleyjointdef_getlength1(b2PulleyJointDef * def) {
	return def->length1;
}

void bmx_b2pulleyjointdef_setmaxlength1(b2PulleyJointDef * def, float32 maxLength) {
	def->maxLength1 = maxLength;
}

float32 bmx_b2pulleyjointdef_getmaxlength1(b2PulleyJointDef * def) {
	return def->maxLength1;
}

void bmx_b2pulleyjointdef_setlength2(b2PulleyJointDef * def, float32 length) {
	def->length2 = length;
}

float32 bmx_b2pulleyjointdef_getlength2(b2PulleyJointDef * def) {
	return def->length2;
}

void bmx_b2pulleyjointdef_setmaxlength2(b2PulleyJointDef * def, float32 maxLength) {
	def->maxLength2 = maxLength;
}

float32 bmx_b2pulleyjointdef_getmaxlength2(b2PulleyJointDef * def) {
	return def->maxLength2;
}

void bmx_b2pulleyjointdef_setratio(b2PulleyJointDef * def, float32 ratio) {
	def->ratio = ratio;
}

float32 bmx_b2pulleyjointdef_getratio(b2PulleyJointDef * def) {
	return def->ratio;
}

void bmx_b2pulleyjointdef_delete(b2PulleyJointDef * def) {
	delete def;
}

// *****************************************************

class MaxDestructionListener : public b2DestructionListener
{
public:
	MaxDestructionListener::MaxDestructionListener(BBObject * handle)
		: maxHandle(handle)
	{
	}
	
	void MaxDestructionListener::SayGoodbye(b2Joint * joint) {
		_bah_box2d_b2DestructionListener__SayGoodbyeJoint(maxHandle, joint);
		void * data = joint->GetUserData();
		if (data && data != &bbNullObject) {
			BBRELEASE((BBObject*)data);
			joint->SetUserData(0);
		}
	}

	void MaxDestructionListener::SayGoodbye(b2Shape * shape) {
		_bah_box2d_b2DestructionListener__SayGoodbyeShape(maxHandle, shape);
		void * data = shape->GetUserData();
		if (data && data != &bbNullObject) {
			BBRELEASE((BBObject*)data);
			shape->SetUserData(0);
		}
	}
	
private:
	BBObject * maxHandle;
};

MaxDestructionListener * bmx_b2destructionlistener_new(BBObject * handle) {
	return new MaxDestructionListener(handle);
}

void bmx_b2destructionlistener_delete(MaxDestructionListener * listener) {
	delete listener;
}

// *****************************************************

b2Mat22 * bmx_b2obb_getrotationmatrix(b2OBB * obb) {
	return &obb->R;
}

b2Vec2 * bmx_b2obb_getcenter(b2OBB * obb) {
	return bmx_b2vec2_new(obb->center);
}

b2Vec2 * bmx_b2obb_getextents(b2OBB * obb) {
	return bmx_b2vec2_new(obb->extents);
}

// *****************************************************

BBArray * bmx_b2polygonshape_getvertices(b2PolygonShape * shape) {
	return bmx_b2vec2_getvertexarray(shape->GetVertices(), shape->GetVertexCount());
}

BBArray * bmx_b2polygonshape_getcorevertices(b2PolygonShape * shape) {
	return bmx_b2vec2_getvertexarray(shape->GetCoreVertices(), shape->GetVertexCount());
}

BBArray * bmx_b2polygonshape_getnormals(b2PolygonShape * shape) {
	return bmx_b2vec2_getvertexarray(shape->GetNormals(), shape->GetVertexCount());
}

const b2OBB * bmx_b2polygonshape_getobb(b2PolygonShape * shape) {
	return &shape->GetOBB();
}

b2Vec2 * bmx_b2polygonshape_getcentroid(b2PolygonShape * shape) {
	return bmx_b2vec2_new(shape->GetCentroid());
}

int bmx_b2polygonshape_getvertexcount(b2PolygonShape * shape) {
	return shape->GetVertexCount();
}

b2Vec2 * bmx_b2polygonshape_getfirstvertex(b2PolygonShape * shape, b2XForm * xf) {
	return bmx_b2vec2_new(shape->GetFirstVertex(*xf));
}

b2Vec2 * bmx_b2polygonshape_centroid(b2PolygonShape * shape, b2XForm * xf) {
	return bmx_b2vec2_new(shape->Centroid(*xf));
}

b2Vec2 * bmx_b2polygonshape_support(b2PolygonShape * shape, b2XForm * xf, b2Vec2 * d) {
	return bmx_b2vec2_new(shape->Support(*xf, *d));
}

// *****************************************************

b2Vec2 * bmx_b2circleshape_getlocalposition(b2CircleShape * shape) {
	return bmx_b2vec2_new(shape->GetLocalPosition());
}

float32 bmx_b2circleshape_getradius(b2CircleShape * shape) {
	return shape->GetRadius();
}

// *****************************************************

b2Segment * bmx_b2segment_createxy(float32 x1, float32 y1, float32 x2, float32 y2) {
	b2Segment * seg = new b2Segment;
	seg->p1 = b2Vec2(x1, y1);
	seg->p2 = b2Vec2(x2, y2);
	return seg;
}

b2Segment * bmx_b2segment_create(b2Vec2 * p1, b2Vec2 * p2) {
	b2Segment * seg = new b2Segment;
	if (p1) {
		seg->p1 = *p1;
	}
	if (p2) {
		seg->p2 = *p2;
	}
	return seg;
}

b2Vec2 * bmx_b2segment_getstartpoint(b2Segment * seg) {
	return bmx_b2vec2_new(seg->p1);
}

b2Vec2 * bmx_b2segment_getendpoint(b2Segment * seg) {
	return bmx_b2vec2_new(seg->p2);
}

void bmx_b2segment_setstartpoint(b2Segment * seg, b2Vec2 * point) {
	seg->p1 = *point;
}

void bmx_b2segment_setendpoint(b2Segment * seg, b2Vec2 * point) {
	seg->p2 = *point;
}

void bmx_b2segment_delete(b2Segment * seg) {
	delete seg;
}

// *****************************************************

b2LineJointDef * bmx_b2linejointdef_create() {
	return new b2LineJointDef();
}

void bmx_b2linejointdef_initialize(b2LineJointDef * def, b2Body * body1, b2Body * body2, b2Vec2 * anchor, b2Vec2 * axis) {
	def->Initialize(body1, body2, *anchor, *axis);
}

void bmx_b2linejointdef_setlocalanchor1(b2LineJointDef * def, b2Vec2 * anchor) {
	def->localAnchor1 = *anchor;
}

b2Vec2 * bmx_b2linejointdef_getlocalanchor1(b2LineJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor1);
}

void bmx_b2linejointdef_setlocalanchor2(b2LineJointDef * def, b2Vec2 * anchor) {
	def->localAnchor2 = *anchor;
}

b2Vec2 * bmx_b2linejointdef_getlocalanchor2(b2LineJointDef * def) {
	return bmx_b2vec2_new(def->localAnchor2);
}

void bmx_b2linejointdef_setlocalaxis1(b2LineJointDef * def, b2Vec2 * axis) {
	def->localAxis1 = *axis;
}

b2Vec2 * bmx_b2linejointdef_getlocalaxis1(b2LineJointDef * def) {
	return bmx_b2vec2_new(def->localAxis1);
}

void bmx_b2linejointdef_enablelimit(b2LineJointDef * def, bool limit) {
	def->enableLimit = limit;
}

bool bmx_b2linejointdef_getlimit(b2LineJointDef * def) {
	return def->enableLimit;
}

void bmx_b2linejointdef_setlowertranslation(b2LineJointDef * def, float32 translation) {
	def->lowerTranslation = translation;
}

float32 bmx_b2linejointdef_getlowertranslation(b2LineJointDef * def) {
	return def->lowerTranslation;
}

void bmx_b2linejointdef_setuppertranslation(b2LineJointDef * def, float32 translation) {
	def->upperTranslation = translation;
}

float32 bmx_b2linejointdef_getuppertranslation(b2LineJointDef * def) {
	return def->upperTranslation;
}

void bmx_b2linejointdef_enablemotor(b2LineJointDef * def, bool enable) {
	def->enableMotor = enable;
}

bool bmx_b2linejointdef_ismotorenabled(b2LineJointDef * def) {
	return def->enableMotor;
}

void bmx_b2linejointdef_setmaxmotorforce(b2LineJointDef * def, float32 maxForce) {
	def->maxMotorForce = maxForce;
}

float32 bmx_b2linejointdef_getmaxmotorforce(b2LineJointDef * def) {
	return def->maxMotorForce;
}

void bmx_b2linejointdef_setmotorspeed(b2LineJointDef * def, float32 speed) {
	def->motorSpeed = speed * 0.0174533f;
}

float32 bmx_b2linejointdef_getmotorspeed(b2LineJointDef * def) {
	return def->motorSpeed * 57.2957795f;
}

void bmx_b2linejointdef_delete(b2LineJointDef * def) {
	delete def;
}

// *****************************************************

b2Vec2 * bmx_b2linejoint_getanchor1(b2LineJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor1());
}

b2Vec2 * bmx_b2linejoint_getanchor2(b2LineJoint * joint) {
	return bmx_b2vec2_new(joint->GetAnchor2());
}

b2Vec2 * bmx_b2linejoint_getreactionforce(b2LineJoint * joint, float32 inv_dt) {
	return bmx_b2vec2_new(joint->GetReactionForce(inv_dt));
}

float32 bmx_b2linejoint_getreactiontorque(b2LineJoint * joint, float32 inv_dt) {
	return joint->GetReactionTorque(inv_dt);
}

float32 bmx_b2linejoint_getjointtranslation(b2LineJoint * joint) {
	return joint->GetJointTranslation();
}

float32 bmx_b2linejoint_getjointspeed(b2LineJoint * joint) {
	return joint->GetJointSpeed();
}

bool bmx_b2linejoint_islimitenabled(b2LineJoint * joint) {
	return joint->IsLimitEnabled();
}

void bmx_b2linejoint_enablelimit(b2LineJoint * joint, bool flag) {
	return joint->EnableLimit(flag);
}

float32 bmx_b2linejoint_getlowerlimit(b2LineJoint * joint) {
	return joint->GetLowerLimit();
}

float32 bmx_b2linejoint_getupperlimit(b2LineJoint * joint) {
	return joint->GetUpperLimit();
}

void bmx_b2linejoint_setlimits(b2LineJoint * joint, float32 _lower, float32 _upper) {
	joint->SetLimits(_lower, _upper);
}

bool bmx_b2linejoint_ismotorenabled(b2LineJoint * joint) {
	return joint->IsMotorEnabled();
}

void bmx_b2linejoint_enablemotor(b2LineJoint * joint, bool flag) {
	joint->EnableMotor(flag);
}

void bmx_b2linejoint_setmotorspeed(b2LineJoint * joint, float32 speed) {
	joint->SetMotorSpeed(speed);
}

float32 bmx_b2linejoint_getmotorspeed(b2LineJoint * joint) {
	return joint->GetMotorSpeed();
}

void bmx_b2linejoint_setmaxmotorforce(b2LineJoint * joint, float32 force) {
	joint->SetMaxMotorForce(force);
}

float32 bmx_b2linejoint_getmotorforce(b2LineJoint * joint) {
	return joint->GetMotorForce();
}


// *****************************************************

Maxb2EdgeChainDef * bmx_b2edgechaindef_create() {
	return new Maxb2EdgeChainDef;
}

b2EdgeChainDef * bmx_b2edgechaindef_getdef(Maxb2EdgeChainDef * def) {
	return def->def;
}

bool bmx_b2edgechaindef_isaloop(Maxb2EdgeChainDef * def) {
	return def->def->isALoop;
}

void bmx_b2edgechaindef_setisaloop(Maxb2EdgeChainDef * def, bool value) {
	def->def->isALoop = value;
}

void bmx_b2edgechaindef_delete(Maxb2EdgeChainDef * def) {
	delete def;
}


void bmx_b2edgechaindef_setvertices(Maxb2EdgeChainDef * def, BBArray * vertices) {

	int n = vertices->scales[0];

	def->initVertices(n);
	
	for (int i = 0; i < n; i++) {
		def->vertices[i] = *_bah_box2d_b2Vec2__getVec(vertices, i);
	}
}

// *****************************************************

float32 bmx_b2edgeshape_getlength(b2EdgeShape * shape) {
	return shape->GetLength();
}

b2Vec2 * bmx_b2edgeshape_getvertex1(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetVertex1());
}

b2Vec2 * bmx_b2edgeshape_getvertex2(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetVertex2());
}

b2Vec2 * bmx_b2edgeshape_getcorevertex1(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetCoreVertex1());
}

b2Vec2 * bmx_b2edgeshape_getcorevertex2(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetCoreVertex2());
}

b2Vec2 * bmx_b2edgeshape_getnormalvector(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetNormalVector());
}

b2Vec2 * bmx_b2edgeshape_getdirectionvector(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetDirectionVector());
}

b2Vec2 * bmx_b2edgeshape_getcorner1vector(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetCorner1Vector());
}

b2Vec2 * bmx_b2edgeshape_getcorner2vector(b2EdgeShape * shape) {
	return bmx_b2vec2_new(shape->GetCorner2Vector());
}

bool bmx_b2edgeshape_corner1isconvex(b2EdgeShape * shape) {
	return shape->Corner1IsConvex();
}

bool bmx_b2edgeshape_corner2isconvex(b2EdgeShape * shape) {
	return shape->Corner2IsConvex();
}

b2Vec2 * bmx_b2edgeshape_getfirstvertex(b2EdgeShape * shape, b2XForm * xf) {
	return bmx_b2vec2_new(shape->GetFirstVertex(*xf));
}

b2Vec2 * bmx_b2edgeshape_support(b2EdgeShape * shape, b2XForm * xf, b2Vec2 * d) {
	return bmx_b2vec2_new(shape->Support(*xf, *d));
}

b2EdgeShape * bmx_b2edgeshape_getnextedge(b2EdgeShape * shape) {
	return shape->GetNextEdge();
}

b2EdgeShape * bmx_b2edgeshape_getprevedge(b2EdgeShape * shape) {
	return shape->GetPrevEdge();
}


