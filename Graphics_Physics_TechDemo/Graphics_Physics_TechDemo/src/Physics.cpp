#include "Physics.h"
#include "Base.h"

void Physics::update(float dt)
{
	std::vector<glm::vec3> points;

	std::vector<std::pair<unsigned, std::vector<unsigned>>> index;
	std::vector<std::vector<glm::vec3>> polygon;
	glm::vec3 forMin = softbody_objs[0]->m_min;
	glm::vec3 forMax = softbody_objs[0]->m_max;

	unsigned num_index = 0;
	std::vector<SoftBodyPhysics*>::iterator it_soft = softbody_objs.begin();
	for (; it_soft != softbody_objs.end(); ++it_soft)
	{
		(*it_soft)->Update(dt);
		//points.insert(points.end(), (*it_soft)->m_scaled_ver.begin(), (*it_soft)->m_scaled_ver.end());
		//
		//forMin = glm::min(forMin, (*it_soft)->m_min);
		//forMax = glm::max(forMax, (*it_soft)->m_max);

		//std::vector<unsigned> push_index;
		//std::vector<glm::vec3> push_poly;
		////bsp
		//for (size_t j = 0; j < (*it_soft)->obj_indices.size(); ++j)
		//{
		//	push_index.push_back((*it_soft)->obj_indices[j]);
		//	push_poly.push_back((*it_soft)->m_scaled_ver[(*it_soft)->obj_indices[j]]);

		//	if (j % 3 == 2)
		//	{
		//		index.push_back(std::make_pair(num_index, push_index));
		//		polygon.push_back(push_poly);

		//		push_index.clear();
		//		push_poly.clear();
		//	}
		//}
		//++num_index;
	}

	//glm::vec3 forCenter = (forMin + forMax) * 0.5f;
	//glm::vec3 length = forMax - forMin;
	//float maxLength = glm::max(length.x, glm::max(length.y, length.z));

	//OcTree = buildOctree(forCenter, maxLength * 0.5f, 1, polygon, index, softbody_objs);
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
			//if (!(*it_soft)->colliding())
				//(*it_soft)->SetInitConstraints();
		}

	}

	//for (it_soft = softbody_objs.begin(); it_soft != softbody_objs.end(); ++it_soft)
	//	(*it_soft)->KeepConstraint();

}

OcNode* Physics::buildOctree(glm::vec3 center, float halfwidth, int depth, std::vector<std::vector<glm::vec3>>& triangles, std::vector<std::pair<unsigned, std::vector<unsigned>>>& indices, std::vector<SoftBodyPhysics*>& objs)
{
	std::vector<std::vector<glm::vec3>> insideTriangle;
	std::vector<std::pair<unsigned, std::vector<unsigned>>> insideIndex;

	for (size_t i = 0; i < triangles.size(); ++i)
	{
		if (isInside(triangles[i], center, halfwidth))
		{
			insideTriangle.push_back(triangles[i]);
			insideIndex.push_back(indices[i]);
		}
	}

	OcNode* pNode = new OcNode(depth, insideIndex, insideTriangle);
	pNode->center = center;
	pNode->halfwidth = halfwidth;
	pNode->type = TreeType::NODE;

	if (insideTriangle.size() <= MIN_LEAF_SIZE)
	{
		for (int i = 0; i < 8; ++i)
			pNode->pChild[i] = 0;

		if (insideTriangle.size() != 0)
			pNode->type = TreeType::LEAF;
		return pNode;
	}

	glm::vec3 offset;
	float step = halfwidth * 0.5f;
	for (int i = 0; i < 8; ++i)
	{
		offset.x = ((i & 1) ? step : -step);
		offset.y = ((i & 2) ? step : -step);
		offset.z = ((i & 4) ? step : -step);
		pNode->pChild[i] = buildOctree(center + offset, step, depth + 1, pNode->triangles, pNode->indices, objs);
	}

	return pNode;

}

bool Physics::isInside(std::vector<glm::vec3> polygon, glm::vec3 center, float half)
{
	glm::vec3 l_min = center - half;
	glm::vec3 l_max = center + half;
	int numInside = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (polygon[i].x > l_min.x && polygon[i].x < l_max.x
			&& polygon[i].y > l_min.y && polygon[i].y < l_max.y
			&& polygon[i].z > l_min.z && polygon[i].z < l_max.z)
			++numInside;
	}

	if (numInside)
		return true;

	return false;
}

void OcNode::Draw(Camera* camera, Shader* shader, Object* line)
{
	if (depth != 0)
	{

		glm::vec3 l_scale = glm::vec3(halfwidth);
		line->scale = l_scale;
		line->render_objs(camera, shader, center, 1200.f / 800.f, true);

	}
	else
	{

		if (triangles.size() == 0)
			return;


		glm::vec3 l_scale = glm::vec3(halfwidth);
		line->scale = l_scale;
		line->render_objs(camera, shader, center, 1200.f / 800.f, true);


		for (int i = 0; i < 8; ++i)
		{
			if (pChild[i] != 0)
				pChild[i]->Draw(camera, shader, line);

		}
	}
}
