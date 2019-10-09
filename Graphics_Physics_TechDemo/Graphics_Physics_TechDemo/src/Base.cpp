#include "Base.h"

void SoftBodyPhysics::Init()
{
	m_gravity = GRAVITY;
	m_scaled_ver = obj_vertices;
	for (int i = 0; i < m_scaled_ver.size(); ++i)
		m_scaled_ver[i] = position + m_scaled_ver[i]*scale;
	m_old_ver = m_scaled_ver;

	if(m_shape == ObjShape::O_PLANE)
	{ 
		m_mass = 0.3f;
		for(int i = 0; i <= dimension; ++i)
			m_edge.push_back(m_scaled_ver[i]);

		//set constraints
		//horizontal
		for (int i = 0; i < dimension; ++i)
		{
		
			for (int j = 0; j <= dimension; ++j)
			{
				constraints h_cons;
				h_cons.p1 = j * (dimension +1) + i;
				h_cons.p2 = h_cons.p1 + 1;

				h_cons.restlen = scale.x / dimension;

				m_cons.push_back(h_cons);
			}
		}

		//vertical
		for (int i = 0; i <= dimension; ++i)
		{
			for (int j = 0; j < dimension; ++j)
			{
				constraints v_cons;
				v_cons.p1 = j * (dimension +1) + i;
				v_cons.p2 = v_cons.p1 + (dimension +1);
				v_cons.restlen = scale.z / (dimension);

				m_cons.push_back(v_cons);
			}
		}

	}
	else if (m_shape == ObjShape::O_SPHERE)
	{
		m_mass = 0.6f;
		int last_index = (dimension) * (dimension + 1);
		//set constraints
		//horizontal
		for (int i = 0; i <= dimension; ++i)
		{
			for (int j = 0; j <= dimension; ++j)
			{
				constraints h_cons;
				h_cons.p1 = j * (dimension + 1) + i;
				h_cons.p2 = h_cons.p1 + 1;

				if (i == dimension)
					h_cons.p2 = j * (dimension + 1);

				if (j == 0)
					h_cons.restlen = 0;
				else
					h_cons.restlen = glm::distance(m_scaled_ver[h_cons.p1], m_scaled_ver[h_cons.p2]);

				m_cons.push_back(h_cons);

				//if (i < dimension/2)
				//{
				//	constraints d_cons;
				//	d_cons.p1 = h_cons.p1;
				//	d_cons.p2 = h_cons.p1 + dimension/2;
				//	d_cons.restlen = glm::distance(m_scaled_ver[d_cons.p1], m_scaled_ver[d_cons.p2]);

				//	m_cons.push_back(d_cons);
				//}
			}


			//vertical
			for (int j = 0; j < dimension; ++j)
			{
				constraints v_cons;
				v_cons.p1 = j * (dimension + 1) + i;
				v_cons.p2 = v_cons.p1 + (dimension + 1);
				v_cons.restlen = glm::distance(m_scaled_ver[v_cons.p1], m_scaled_ver[v_cons.p2]);

				m_cons.push_back(v_cons);

				/*if (j < dimension/2)
				{
					constraints d_cons;
					d_cons.p1 = v_cons.p1;
					d_cons.p2 = v_cons.p2 = v_cons.p1 + (dimension*2 + 1);
					d_cons.restlen = glm::distance(m_scaled_ver[d_cons.p1], m_scaled_ver[d_cons.p2]);

					m_cons.push_back(d_cons);
				}*/
			}
		}


	}


}

void SoftBodyPhysics::Update(float dt)
{
	Acceleration();
	Verlet(dt);
	KeepConstraint();

	for (int i = 0; i < m_scaled_ver.size(); ++i)
		obj_vertices[i] = (m_scaled_ver[i] - position) / scale;
	
}

void SoftBodyPhysics::Verlet(float dt)
{
	float f = 0.99f;
	for (int i = 0; i < m_scaled_ver.size(); ++i)
	{
		glm::vec3* _new = &m_scaled_ver[i];
		glm::vec3 temp = *_new;
		glm::vec3* old = &m_old_ver[i];
		

		(*_new) +=  f * temp - f * (*old) + m_acceleration * dt * dt;
		(*old) = temp;
	}
}

void SoftBodyPhysics::KeepConstraint()
{
	for (int i = 0; i < 16; ++i)
	{
		//staying edge
		for (int j = 0; j < m_edge.size(); ++j)
			m_scaled_ver[j] = m_edge[j];


		for (int j = 0; j < m_cons.size(); ++j)
		{
			constraints& c = m_cons[j];
			glm::vec3& point1 = m_scaled_ver[c.p1];
			glm::vec3& point2 = m_scaled_ver[c.p2];

			glm::vec3 delta = point2 - point1;
			if (delta == glm::vec3(0, 0, 0))
				continue;

			float len = glm::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
			float diff = (len - c.restlen) / len;

			point1 += delta * 0.5f * diff;
			point2 -= delta * 0.5f * diff;
		}
	}

}

void SoftBodyPhysics::Acceleration()
{
	m_acceleration = glm::vec3(0, m_gravity*m_mass, 0);
}

void SoftBodyPhysics::CollisionResponseRigid(Object* _rhs)
{
	if (_rhs->m_shape == ObjShape::O_SPHERE)
	{
		glm::vec3 center = _rhs->position;
		float radius = _rhs->scale.x + 0.01f;
		float radius_sqr = radius * radius;
		for (int i = 0; i < m_scaled_ver.size(); ++i)
		{
			glm::vec3& point = m_scaled_ver[i];
			if (IsCollided(point, center, radius_sqr))
			{
				glm::vec3 normal = point - center;
				normal = glm::normalize(normal);

				m_scaled_ver[i] = center + normal * radius;

			}
		}
	}
	else if (_rhs->m_shape == ObjShape::O_PLANE)
	{
		glm::vec3 center = _rhs->position;
		float radius = 0.01f;
		//float radius_sqr = radius * radius;
		for (int i = 0; i < m_scaled_ver.size(); ++i)
		{
			glm::vec3& point = m_scaled_ver[i];
			if (IsCollidedPlane(point, center, radius))
			{
				//glm::vec3 normal = point - center;
				//normal = glm::normalize(normal);
				//glm::vec3 normal = glm::vec3(0, 1, 0);

				m_scaled_ver[i].y = center.y + radius;

			}
		}
	}

}

void SoftBodyPhysics::CollisionResponseSoft(SoftBodyPhysics* _rhs)
{
}

bool SoftBodyPhysics::IsCollided(glm::vec3& point, glm::vec3& center, float& radius)
{
	float distance = (center.x - point.x) * (center.x - point.x) +
		(center.y - point.y) * (center.y - point.y) +
		(center.z - point.z) * (center.z - point.z);

	if (distance < radius)
		return true;
	else
		return false;

}

bool SoftBodyPhysics::IsCollidedPlane(glm::vec3& point, glm::vec3& center, float& radius)
{
	if (glm::abs(point.y - center.y) < radius)
		return true;
	else
		return false;

}
