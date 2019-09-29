#include "Physics.h"
#include "Base.h"

void Physics::update(float dt)
{
	std::vector<SoftBodyPhysics*>::iterator it_soft;
	for (it_soft = softbody_objs.begin(); it_soft < softbody_objs.end(); ++it_soft)
	{
		(*it_soft)->Update(dt);
		std::vector<Object*>::iterator it_rigid;
		for (it_rigid = physics_objs.begin(); it_rigid < physics_objs.end(); ++it_rigid)
		{
			(*it_soft)->CollisionResponseRigid(*it_rigid);
		}
	}
}