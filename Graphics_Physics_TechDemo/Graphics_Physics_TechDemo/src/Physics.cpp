#include "Physics.h"
#include "Base.h"

void Physics::update(float dt)
{
	std::vector<SoftBodyPhysics*>::iterator it;
	for (it = softbody_objs.begin(); it < softbody_objs.end(); ++it)
	{
		(*it)->Update(dt);

		
	}


}