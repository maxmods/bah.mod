/*
  Copyright (c) 2007 Bruce A Henderson
 
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

#include <map>

#include "chipmunk.h"
#include "blitz.h"

extern "C" {
	cpVect * _bah_chipmunk_CPVect__getVectForIndex(BBArray * verts, int index);
	BBObject *cpfind( void *obj );
	void cpbind( void *obj, BBObject *peer );
	void _bah_chipmunk_CPPolyShape__setVert(BBArray * verts, int index, cpVect * vec);
	void _bah_chipmunk_CPContact__setContact(BBArray * conts, int index, cpContact * contact);

	cpBody * bmx_cpbody_create(BBObject * handle, cpFloat mass, cpFloat inertia);
	cpFloat bmx_cpbody_getmass(cpBody * body);
	cpFloat bmx_cpbody_getinertia(cpBody * body);
	cpFloat bmx_cpbody_getangle(cpBody * body);
	cpFloat bmx_cpbody_getangularvelocity(cpBody * body);
	cpFloat bmx_cpbody_gettorque(cpBody * body);
	void bmx_cpbody_setposition(cpBody * body, cpVect * vec);
	cpVect * bmx_cpbody_getposition(cpBody * body);
	cpVect * bmx_cpbody_getrot(cpBody * body);
	void bmx_cpbody_setvelocity(cpBody * body, cpVect * velocity);
	void bmx_cpbody_setangularvelocity(cpBody * body, cpFloat av);
	void bmx_cpbody_setangle(cpBody * body, cpFloat angle);
	void bmx_cpbody_updatevelocity(cpBody * body, cpVect * gravity, cpFloat damping, cpFloat dt);
	void bmx_cpbody_applyforce(cpBody * body, cpVect * force, cpVect * offset);
	cpVect * bmx_cpbody_local2world(cpBody * body, cpVect * vec);
	cpVect * bmx_cpbody_world2local(cpBody * body, cpVect * vec);
	cpVect * bmx_cpbody_getvelocity(cpBody * body);
	void bmx_body_applyimpulse(cpBody * body, cpVect * impulse, cpVect * offset);
	void bmx_body_slew(cpBody * body, cpVect * pos, cpFloat dt);

	cpSpace * bmx_cpspace_create(BBObject * handle);
	void bmx_cpspace_setgravity(cpSpace * space, cpVect * vec);
	void bmx_cpspace_addstaticshape(cpSpace * space, cpShape * shape);
	void bmx_cpspace_addbody(cpSpace * space, cpBody * body);
	void bmx_cpspace_addshape(cpSpace * space, cpShape * shape);
	cpSpaceHash * bmx_cpspace_getactiveshapes(cpSpace * space);
	cpSpaceHash * bmx_cpspace_getstaticshapes(cpSpace * space);
	void bmx_cpspace_setiterations(cpSpace * space, int num);
	void bmx_cpspace_addjoint(cpSpace * space, cpJoint * joint);
	void bmx_cpspace_addcollisionpairfunc(cpSpace * space, unsigned long a, unsigned long b, cpCollFunc func, void *data);
	void bmx_cpspace_removecollisionpairfunc(cpSpace *space, unsigned long a, unsigned long b);
	void bmx_cpspace_setdefaultcollisionpairfunc(cpSpace * space, cpCollFunc func, void *data);
	void bmx_cpspace_setdamping(cpSpace * space, cpFloat damping);

	cpVect * bmx_cpvect_create(cpFloat x, cpFloat y);
	void bmx_cpvect_delete(cpVect * vec);
	cpVect * bmx_cpvect_new(cpVect v);
	cpFloat bmx_cpvect_getx(cpVect * vec);
	cpFloat bmx_cpvect_gety(cpVect * vec);
	cpVect * bmx_cpvect_add(cpVect * vec, cpVect * vec2);
	cpVect * bmx_cpvect_sub(cpVect * vec, cpVect * vec2);
	cpVect * bmx_cpvect_rotate(cpVect * vec, cpVect * vec2);
	void bmx_cpvect_getxy(cpVect * vec, cpFloat * x, cpFloat * y);
	cpVect * bmx_cpvect_negate(cpVect * vec);
	cpVect * bmx_cpvect_mult(cpVect * vec, cpFloat scalar);
	cpFloat bmx_cpvect_dot(cpVect * vec, cpVect * vec1);
	cpFloat bmx_cpvect_cross(cpVect * vec, cpVect * vec1);
	cpVect * bmx_cpvect_perp(cpVect * vec);
	cpVect * bmx_cpvect_project(cpVect * vec, cpVect * vec1);
	cpVect * bmx_cpvect_unrotate(cpVect * vec, cpVect * vec1);
	cpFloat bmx_cpvect_length(cpVect * vec);
	cpFloat bmx_cpvect_lengthsq(cpVect * vec);
	cpVect * bmx_cpvect_normalize(cpVect * vec);
	cpFloat bmx_cpvect_toangle(cpVect * vec);

	cpShape * bmx_cpsegmentshape_create(BBObject * handle, cpBody * body, cpVect * a, cpVect * b, cpFloat radius);
	cpVect * bmx_cpsegmentshape_getendpointa(cpSegmentShape * shape);
	cpVect * bmx_cpsegmentshape_getendpointb(cpSegmentShape * shape);
	cpVect * bmx_cpsegmentshape_getnormal(cpSegmentShape * shape);

	void bmx_cpshape_setelasticity(cpShape * shape, cpFloat e);
	void bmx_cpshape_setfriction(cpShape * shape, cpFloat u);
	void bmx_cpshape_setcollisiontype(cpShape * shape, unsigned long type);
	cpBody * bmx_cpshape_getbody(cpShape * shape);
	void bmx_cpshape_setgroup(cpShape * shape, unsigned long group);
	void bmx_cpshape_setlayers(cpShape * shape, unsigned long layers);
	void bmx_cpshape_setsurfacevelocity(cpShape * shape, cpVect * velocity);
	cpVect * bmx_cpshape_getsurfacevelocity(cpShape * shape);
	cpFloat bmx_cpshape_getelasticity(cpShape * shape);
	cpFloat bmx_cpshape_getfriction(cpShape * shape);
	cpBB * bmx_shape_cachebb(cpShape * shape);

	cpFloat bmx_momentforpoly(cpFloat m, BBArray * verts, int count, cpVect * offset);
	cpFloat bmx_momentforcircle(cpFloat m, cpFloat r1, cpFloat r2, cpVect * offset);
	void bmx_cpdampedspring(cpBody * a, cpBody * b, cpVect * anchor1, cpVect * anchor2, cpFloat rlen, cpFloat k, cpFloat dmp, cpFloat dt);

	BBArray * bmx_cppolyshape_getvertsascoords(cpPolyShape * shape);
	int bmx_cppolyshape_numverts(cpPolyShape * shape);
	void bmx_cppolyshape_getverts(cpPolyShape * shape, BBArray * verts);

	cpShape * bmx_cppolyshape_create(BBObject * handle, cpBody * body, BBArray *verts, int count, cpVect * offset);

	cpShape * bmx_cpcircleshape_create(BBObject * handle, cpBody * body, cpFloat radius, cpVect * offset);
	cpFloat bmx_cpcircleshape_getradius(cpCircleShape * shape);
	cpVect * bmx_cpcircleshape_getcenter(cpCircleShape * shape);
	cpVect * bmx_cpcircleshape_gettransformedcenter(cpCircleShape * shape);
	
	cpJoint * bmx_cppinjoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * anchor1, cpVect * anchor2);
	cpJoint * bmx_cpslidejoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * anchor1, cpVect * anchor2, cpFloat minDist, cpFloat maxDist);
	cpJoint * bmx_cppivotjoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * pivot);
	cpJoint * bmx_cpgroovejoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * grooveA, cpVect * grooveB, cpVect * anchor);

	unsigned int bmx_CP_HASH_PAIR(int collTypeA, int collTypeB);

	cpVect * bmx_cpcontact_getposition(cpContact * contact);
	cpVect * bmx_cpcontact_getnormal(cpContact * contact);
	cpFloat bmx_cpcontact_getdistance(cpContact * contact);
	cpFloat bmx_cpcontact_getjnacc(cpContact * contact);
	cpFloat bmx_cpcontact_getjtacc(cpContact * contact);
	void bmx_cpcontact_fill(BBArray * conts, cpContact * contacts, int numContacts);

	cpBB * bmx_cpbb_create(cpFloat l, cpFloat b, cpFloat r, cpFloat t);
	int bmx_cpbb_intersects(cpBB * bb, cpBB * other);
	int bmx_cpbb_containsbb(cpBB * bb, cpBB * other);
	int bmx_cpbb_containsvect(cpBB * bb, cpVect * v);
	cpVect * bmx_cpbb_clampvect(cpBB * bb, cpVect * v);
	cpVect * bmx_cpbb_wrapvect(cpBB * bb, cpVect * v);
	void bmx_cpbb_delete(cpBB * bb);

}

unsigned int bmx_CP_HASH_PAIR(int collTypeA, int collTypeB) {
	return CP_HASH_PAIR(collTypeA, collTypeB);
}


typedef std::map<void*, BBObject*> PeerMap;

static PeerMap peerMap;

void cpbind( void *obj, BBObject *peer ) {
	if( !obj || peer==&bbNullObject ) return;
	peerMap.insert( std::make_pair( obj,peer ) );
	BBRETAIN( peer );
}

BBObject *cpfind( void *obj ){
	if( !obj ) return &bbNullObject;
	PeerMap::iterator it = peerMap.find( obj );
	if( it != peerMap.end() ) return it->second;
	return &bbNullObject;
}

void cpunbind(void *obj) {
	BBObject * peer = cpfind(obj);
	if (peer != &bbNullObject) {
		peerMap.erase(obj);
		BBRELEASE(peer);
	}
}

BBArray *vertsToBBFloatArray( int num, cpVect *verts ){
	BBArray *p=bbArrayNew1D( "f", num * 2 );
	cpFloat *s=(cpFloat*)BBARRAYDATA( p,p->dims );
	for( int i=0;i < num;++i ){
		s[i * 2] = verts[i].x;
		s[i * 2 + 1] = verts[i].y;
	}
	return p;
}

// -------------------------------------------------

cpBody * bmx_cpbody_create(BBObject * handle, cpFloat mass, cpFloat inertia) {
	cpBody * body = cpBodyNew(mass, inertia);
	cpbind(body, handle);
	return body;
}

cpFloat bmx_cpbody_getmass(cpBody * body) {
	return body->m;
}

cpFloat bmx_cpbody_getinertia(cpBody * body) {
	return body->i;
}

cpFloat bmx_cpbody_getangle(cpBody * body) {
	return body->a * 57.2957795f;
}

void bmx_cpbody_setangle(cpBody * body, cpFloat angle) {
	cpBodySetAngle(body, angle / 57.2957795f);
}

cpFloat bmx_cpbody_getangularvelocity(cpBody * body) {
	return body->w;
}

cpFloat bmx_cpbody_gettorque(cpBody * body) {
	return body->t;
}

void bmx_cpbody_setposition(cpBody * body, cpVect * vec) {
	body->p = *vec;
}

cpVect * bmx_cpbody_getposition(cpBody * body) {
	return bmx_cpvect_new(body->p);
}

cpVect * bmx_cpbody_getrot(cpBody * body) {
	return bmx_cpvect_new(body->rot);
}

void bmx_cpbody_setvelocity(cpBody * body, cpVect * velocity) {
	body->v = *velocity;
}

void bmx_cpbody_setangularvelocity(cpBody * body, cpFloat av) {
	body->w = av;
}

void bmx_cpbody_updatevelocity(cpBody * body, cpVect * gravity, cpFloat damping, cpFloat dt) {
	cpBodyUpdateVelocity(body, *gravity, damping, dt);
}

void bmx_cpbody_applyforce(cpBody * body, cpVect * force, cpVect * offset) {
	cpBodyApplyForce(body, *force, *offset);
}

cpVect * bmx_cpbody_local2world(cpBody * body, cpVect * vec) {
	return bmx_cpvect_new(cpBodyLocal2World(body, *vec));
}

cpVect * bmx_cpbody_world2local(cpBody * body, cpVect * vec) {
	return bmx_cpvect_new(cpBodyWorld2Local(body, *vec));
}

cpVect * bmx_cpbody_getvelocity(cpBody * body) {
	return bmx_cpvect_new(body->v);
}

void bmx_body_applyimpulse(cpBody * body, cpVect * impulse, cpVect * offset) {
	cpBodyApplyImpulse(body, *impulse, *offset);
}

void bmx_body_slew(cpBody * body, cpVect * pos, cpFloat dt) {
	cpBodySlew(body, *pos, dt);
}

// -------------------------------------------------

cpSpace * bmx_cpspace_create(BBObject * handle) {
	cpSpace * space = cpSpaceNew();
	cpbind(space, handle);
	return space;
}

void bmx_cpspace_setgravity(cpSpace * space, cpVect * vec) {
	space->gravity = *vec;
}

void bmx_cpspace_addstaticshape(cpSpace * space, cpShape * shape) {
	cpSpaceAddStaticShape(space, shape);
}

void bmx_cpspace_addbody(cpSpace * space, cpBody * body) {
	cpSpaceAddBody(space, body);
}

void bmx_cpspace_addshape(cpSpace * space, cpShape * shape) {
	cpSpaceAddShape(space, shape);
}

cpSpaceHash * bmx_cpspace_getactiveshapes(cpSpace * space) {
	return space->activeShapes;
}

cpSpaceHash * bmx_cpspace_getstaticshapes(cpSpace * space) {
	return space->staticShapes;
}

void bmx_cpspace_setiterations(cpSpace * space, int num) {
	space->iterations = num;
}

void bmx_cpspace_addjoint(cpSpace * space, cpJoint * joint) {
	cpSpaceAddJoint(space, joint);
}

void bmx_cpspace_addcollisionpairfunc(cpSpace * space, unsigned long a, unsigned long b, cpCollFunc func, void *data) {
	cpSpaceAddCollisionPairFunc(space, a, b, func, data);
}

void bmx_cpspace_removecollisionpairfunc(cpSpace *space, unsigned long a, unsigned long b) {
	cpSpaceRemoveCollisionPairFunc(space, a, b);
}

void bmx_cpspace_setdefaultcollisionpairfunc(cpSpace * space, cpCollFunc func, void *data) {
	cpSpaceSetDefaultCollisionPairFunc(space, func, data);
}

void bmx_cpspace_setdamping(cpSpace * space, cpFloat damping) {
	space->damping = damping;
}

// -------------------------------------------------

cpVect * bmx_cpvect_create(cpFloat x, cpFloat y) {

	cpVect *vec = new cpVect;
	vec->x = x;
	vec->y = y;
	return vec;
}

cpVect * bmx_cpvect_new(cpVect v) {
	cpVect *vec = new cpVect;
	vec->x = v.x;
	vec->y = v.y;
	return vec;
}

void bmx_cpvect_delete(cpVect * vec) {
	delete vec;
}

cpFloat bmx_cpvect_getx(cpVect * vec) {
	return vec->x;
}

cpFloat bmx_cpvect_gety(cpVect * vec) {
	return vec->y;
}

cpVect * bmx_cpvect_add(cpVect * vec, cpVect * vec2) {
	return bmx_cpvect_new(cpvadd(*vec, *vec2));
}

cpVect * bmx_cpvect_sub(cpVect * vec, cpVect * vec2) {
	return bmx_cpvect_new(cpvsub(*vec, *vec2));
}

cpVect * bmx_cpvect_rotate(cpVect * vec, cpVect * vec2) {
	return bmx_cpvect_new(cpvrotate(*vec, *vec2));
}

void bmx_cpvect_getxy(cpVect * vec, cpFloat * x, cpFloat * y) {
	*x = vec->x;
	*y = vec->y;
}

cpVect * bmx_cpvect_negate(cpVect * vec) {
	return bmx_cpvect_new(cpvneg(*vec));
}

cpVect * bmx_cpvect_mult(cpVect * vec, cpFloat scalar) {
	return bmx_cpvect_new(cpvmult(*vec, scalar));
}

cpFloat bmx_cpvect_dot(cpVect * vec, cpVect * vec1) {
	return cpvdot(*vec, *vec1);
}

cpFloat bmx_cpvect_cross(cpVect * vec, cpVect * vec1) {
	return cpvcross(*vec, *vec1);
}

cpVect * bmx_cpvect_perp(cpVect * vec) {
	return bmx_cpvect_new(cpvperp(*vec));
}

cpVect * bmx_cpvect_project(cpVect * vec, cpVect * vec1) {
	return bmx_cpvect_new(cpvproject(*vec, *vec1));
}

cpVect * bmx_cpvect_unrotate(cpVect * vec, cpVect * vec1) {
	return bmx_cpvect_new(cpvunrotate(*vec, *vec1));
}

cpFloat bmx_cpvect_length(cpVect * vec) {
	return cpvlength(*vec);
}

cpFloat bmx_cpvect_lengthsq(cpVect * vec) {
	return cpvlengthsq(*vec);
}

cpVect * bmx_cpvect_normalize(cpVect * vec) {
	return bmx_cpvect_new(cpvnormalize(*vec));
}

cpFloat bmx_cpvect_toangle(cpVect * vec) {
	return cpvtoangle(*vec);
}


// -------------------------------------------------

cpShape * bmx_cpsegmentshape_create(BBObject * handle, cpBody * body, cpVect * a, cpVect * b, cpFloat radius) {
	cpShape * shape = cpSegmentShapeNew(body, *a, *b, radius);
	cpbind(shape, handle);
	return shape;
}

cpVect * bmx_cpsegmentshape_getendpointa(cpSegmentShape * shape) {
	return bmx_cpvect_new(shape->a);
}

cpVect * bmx_cpsegmentshape_getendpointb(cpSegmentShape * shape) {
	return bmx_cpvect_new(shape->b);
}

cpVect * bmx_cpsegmentshape_getnormal(cpSegmentShape * shape) {
	return bmx_cpvect_new(shape->n);
}

// -------------------------------------------------

cpBB * bmx_cpbb_new(cpBB bb) {
	cpBB *bbox = new cpBB;
	bbox->l = bb.l;
	bbox->t = bb.t;
	bbox->r = bb.r;
	bbox->b = bb.b;
	return bbox;
}

void bmx_cpbb_delete(cpBB * bb) {
	delete bb;
}


cpBB * bmx_cpbb_create(cpFloat l, cpFloat b, cpFloat r, cpFloat t) {
	cpBB *bbox = new cpBB;
	bbox->l = l;
	bbox->t = t;
	bbox->r = r;
	bbox->b = b;
	return bbox;
}

int bmx_cpbb_intersects(cpBB * bb, cpBB * other) {
	return cpBBintersects(*bb, *other);
}

int bmx_cpbb_containsbb(cpBB * bb, cpBB * other) {
	return cpBBcontainsBB(*bb, *other);
}

int bmx_cpbb_containsvect(cpBB * bb, cpVect * v) {
	return cpBBcontainsVect(*bb, *v);
}

cpVect * bmx_cpbb_clampvect(cpBB * bb, cpVect * v) {
	return bmx_cpvect_new(cpBBClampVect(*bb, *v));
}

cpVect * bmx_cpbb_wrapvect(cpBB * bb, cpVect * v) {
	return bmx_cpvect_new(cpBBWrapVect(*bb, *v));
}

// -------------------------------------------------

void bmx_cpshape_setelasticity(cpShape * shape, cpFloat e) {
	shape->e = e;
}

void bmx_cpshape_setfriction(cpShape * shape, cpFloat u) {
	shape->u = u;
}

void bmx_cpshape_setcollisiontype(cpShape * shape, unsigned long type) {
	shape->collision_type = type;
}

cpBody * bmx_cpshape_getbody(cpShape * shape) {
	return shape->body;
}

void bmx_cpshape_setgroup(cpShape * shape, unsigned long group) {
	shape->group = group;
}

void bmx_cpshape_setlayers(cpShape * shape, unsigned long layers) {
	shape->layers = layers;
}

void bmx_cpshape_setsurfacevelocity(cpShape * shape, cpVect * velocity) {
	shape->surface_v = *velocity;
}

cpVect * bmx_cpshape_getsurfacevelocity(cpShape * shape) {
	return bmx_cpvect_new(shape->surface_v);
}

cpFloat bmx_cpshape_getelasticity(cpShape * shape) {
	return shape->e;
}

cpFloat bmx_cpshape_getfriction(cpShape * shape) {
	return shape->u;
}

cpBB * bmx_shape_cachebb(cpShape * shape) {
	return bmx_cpbb_new(cpShapeCacheBB(shape));
}

// -------------------------------------------------

cpFloat bmx_momentforpoly(cpFloat m, BBArray * verts, int count, cpVect * offset) {
	cpVect tVerts[count];
	for (int i = 0; i<count; i++) {
		tVerts[i] = *_bah_chipmunk_CPVect__getVectForIndex(verts, i);
	}
	return cpMomentForPoly(m, count, tVerts, *offset);
}

cpFloat bmx_momentforcircle(cpFloat m, cpFloat r1, cpFloat r2, cpVect * offset) {
	return cpMomentForCircle(m, r1, r2, *offset);
}

void bmx_cpdampedspring(cpBody * a, cpBody * b, cpVect * anchor1, cpVect * anchor2, cpFloat rlen, cpFloat k, cpFloat dmp, cpFloat dt) {
	cpDampedSpring(a, b, *anchor1, *anchor2, rlen, k, dmp, dt);
}

// -------------------------------------------------

cpShape * bmx_cppolyshape_create(BBObject * handle, cpBody * body, BBArray *verts, int count, cpVect * offset) {
	cpVect tVerts[count];
	for (int i = 0; i<count; i++) {
		tVerts[i] = *_bah_chipmunk_CPVect__getVectForIndex(verts, i);
	}
	cpShape * shape = cpPolyShapeNew(body, count, tVerts, *offset);
	cpbind(shape, handle);
	return shape;
}

BBArray * bmx_cppolyshape_getvertsascoords(cpPolyShape * shape) {
	return vertsToBBFloatArray(shape->numVerts, shape->verts);
}

int bmx_cppolyshape_numverts(cpPolyShape * shape) {
	return shape->numVerts;
}

void bmx_cppolyshape_getverts(cpPolyShape * shape, BBArray * verts) {
	for (int i = 0; i< shape->numVerts; i++) {
		_bah_chipmunk_CPPolyShape__setVert(verts, i, bmx_cpvect_new(shape->verts[i]));
	}
}

// -------------------------------------------------

cpShape * bmx_cpcircleshape_create(BBObject * handle, cpBody * body, cpFloat radius, cpVect * offset) {
	cpShape * shape = cpCircleShapeNew(body, radius, *offset);
	cpbind(shape, handle);
	return shape;	
}

cpFloat bmx_cpcircleshape_getradius(cpCircleShape * shape) {
	return shape->r;
}

cpVect * bmx_cpcircleshape_getcenter(cpCircleShape * shape) {
	return bmx_cpvect_new(shape->c);
}

cpVect * bmx_cpcircleshape_gettransformedcenter(cpCircleShape * shape) {
	return bmx_cpvect_new(shape->tc);
}


// -------------------------------------------------

cpJoint * bmx_cppinjoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * anchor1, cpVect * anchor2) {
	cpJoint * joint = cpPinJointNew(bodyA, bodyB, *anchor1, *anchor2);
	cpbind(joint, handle);
	return joint;
}

// -------------------------------------------------

cpJoint * bmx_cpslidejoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * anchor1, cpVect * anchor2, cpFloat minDist, cpFloat maxDist) {
	cpJoint * joint = cpSlideJointNew(bodyA, bodyB, *anchor1, *anchor2, minDist, maxDist);
	cpbind(joint, handle);
	return joint;
}


// -------------------------------------------------

cpJoint * bmx_cppivotjoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * pivot) {
	cpJoint * joint = cpPivotJointNew(bodyA, bodyB, *pivot);
	cpbind(joint, handle);
	return joint;
}


// -------------------------------------------------

cpJoint * bmx_cpgroovejoint_create(BBObject * handle, cpBody * bodyA, cpBody * bodyB, cpVect * grooveA, cpVect * grooveB, cpVect * anchor) {
	cpJoint * joint = cpGrooveJointNew(bodyA, bodyB, *grooveA, *grooveB, *anchor);
	cpbind(joint, handle);
	return joint;
}

// -------------------------------------------------

cpVect * bmx_cpcontact_getposition(cpContact * contact) {
	return bmx_cpvect_new(contact->p);
}

cpVect * bmx_cpcontact_getnormal(cpContact * contact) {
	return bmx_cpvect_new(contact->n);
}

cpFloat bmx_cpcontact_getdistance(cpContact * contact) {
	return contact->dist;
}

cpFloat bmx_cpcontact_getjnacc(cpContact * contact) {
	return contact->jnAcc;
}

cpFloat bmx_cpcontact_getjtacc(cpContact * contact) {
	return contact->jtAcc;
}

void bmx_cpcontact_fill(BBArray * conts, cpContact * contacts, int numContacts) {
	for (int i = 0; i< numContacts; i++) {
		_bah_chipmunk_CPContact__setContact(conts, i, &contacts[i]);
	}
}


