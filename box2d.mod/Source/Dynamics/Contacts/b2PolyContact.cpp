/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "b2PolyContact.h"
#include "../b2Body.h"
#include "../b2WorldCallbacks.h"
#include "../../Common/b2BlockAllocator.h"

#include <memory>
#include <new>

b2Contact* b2PolygonContact::Create(b2Shape* shape1, b2Shape* shape2, b2BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(b2PolygonContact));
	return new (mem) b2PolygonContact(shape1, shape2);
}

void b2PolygonContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
	((b2PolygonContact*)contact)->~b2PolygonContact();
	allocator->Free(contact, sizeof(b2PolygonContact));
}

b2PolygonContact::b2PolygonContact(b2Shape* s1, b2Shape* s2)
	: b2Contact(s1, s2)
{
	b2Assert(m_shape1->m_type == e_polygonShape);
	b2Assert(m_shape2->m_type == e_polygonShape);
	m_manifold.pointCount = 0;
}

void b2PolygonContact::Evaluate(b2ContactListener* listener)
{
	b2Body* b1 = m_shape1->GetBody();
	b2Body* b2 = m_shape2->GetBody();

	b2Manifold m0;
	memcpy(&m0, &m_manifold, sizeof(b2Manifold));

	b2CollidePolygons(&m_manifold, (b2PolygonShape*)m_shape1, b1->GetXForm(), (b2PolygonShape*)m_shape2, b2->GetXForm());

	bool match[b2_maxManifoldPoints] = {false, false};

	// Match contact ids to facilitate warm starting.
	if (m_manifold.pointCount > 0)
	{
		// Match old contact ids to new contact ids and copy the
		// stored impulses to warm start the solver.
		for (int32 i = 0; i < m_manifold.pointCount; ++i)
		{
			b2ManifoldPoint* cp = m_manifold.points + i;
			cp->normalForce = 0.0f;
			cp->tangentForce = 0.0f;
			bool matched = false;
			b2ContactID id = cp->id;

			for (int32 j = 0; j < m0.pointCount; ++j)
			{
				if (match[j] == true)
					continue;

				b2ManifoldPoint* cp0 = m0.points + j;
				b2ContactID id0 = cp0->id;
				id0.features.flip &= ~b2_newPoint;

				if (id0.key == id.key)
				{
					match[j] = true;
					cp->normalForce = cp0->normalForce;
					cp->tangentForce = cp0->tangentForce;

					// Not a new point.
					matched = true;
					break;
				}
			}

			if (matched == false)
			{
				cp->id.features.flip |= b2_newPoint;
			}
		}

		m_manifoldCount = 1;
	}
	else
	{
		m_manifoldCount = 0;
	}

	// Report removed points.
	if (listener && m0.pointCount > 0)
	{
		b2ContactPoint cp;
		cp.shape1 = m_shape1;
		cp.shape2 = m_shape2;
		cp.normal = m0.normal;
		for (int32 i = 0; i < m0.pointCount; ++i)
		{
			if (match[i])
			{
				continue;
			}

			b2ManifoldPoint* mp0 = m0.points + i;
			cp.position = b2Mul(b1->GetXForm(), mp0->localPoint1);
			cp.separation = mp0->separation;
			cp.normalForce = mp0->normalForce;
			cp.tangentForce = mp0->tangentForce;
			cp.id = mp0->id;
			listener->Remove(&cp);
		}
	}
}
