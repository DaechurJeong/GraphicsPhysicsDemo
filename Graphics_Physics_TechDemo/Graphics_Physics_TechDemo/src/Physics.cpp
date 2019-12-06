#include "Physics.h"
#include "Base.h"

void Physics::update(float dt)
{
	std::vector<SoftBodyPhysics*>::iterator it_soft = softbody_objs.begin();
	for (; it_soft != softbody_objs.end(); ++it_soft)
		(*it_soft)->Update(dt);

	for (it_soft = softbody_objs.begin(); it_soft != softbody_objs.end(); ++it_soft)
	{
		std::vector<SoftBodyPhysics*>::iterator it_soft2;
		for (it_soft2 = softbody_objs.begin(); it_soft2 != softbody_objs.end(); ++it_soft2)
		{
			if (it_soft == it_soft2)
				continue;
			(*it_soft)->CollisionResponseSoft(*it_soft2);
		}

		std::vector<Object*>::iterator it_rigid;
		for (it_rigid = physics_objs.begin(); it_rigid < physics_objs.end(); ++it_rigid)
		{
			if ((*it_rigid)->phy)
				(*it_rigid)->position += 0.3f * glm::vec3(0, GRAVITY, 0)*dt;
			(*it_soft)->CollisionResponseRigid(*it_rigid);
		}

	}
}



