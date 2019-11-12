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
			if ((*it_rigid)->phy)
				(*it_rigid)->position += 0.3f * glm::vec3(0, GRAVITY, 0)*dt;

			(*it_soft)->CollisionResponseRigid(*it_rigid);
			if (!(*it_soft)->colliding())
				(*it_soft)->SetInitConstraints();
		}
	}
}