#include "Base.h"
#include <iostream>

void SoftBodyPhysics::Init()
{
	m_gravity = GRAVITY;
	m_scaled_ver = obj_vertices;

	unsigned ver = static_cast<unsigned>(m_scaled_ver.size());
	for (unsigned i = 0; i < ver; ++i)
		m_scaled_ver[i] = position + m_scaled_ver[i]*scale;
	m_old_ver = m_scaled_ver;
	stiffness = 0.5f;
	damping = 1.f;

	isCollided = false;

	if(m_shape == ObjShape::O_PLANE)
	{ 
		m_mass = 0.3f;
		m_acceleration = std::vector<glm::vec3>(ver, glm::vec3(0, m_gravity * m_mass, 0));
		m_velocity = std::vector<glm::vec3>(ver, glm::vec3(0));

		//m_edge.push_back(std::make_pair(0, m_scaled_ver[0]));
		//m_edge.push_back(std::make_pair(dimension, m_scaled_ver[dimension]));
		//m_edge.push_back(std::make_pair(ver-dimension, m_scaled_ver[ver - dimension]));
		//m_edge.push_back(std::make_pair(ver-1, m_scaled_ver[ver - 1]));


		for(int i = 0; i <= dimension; ++i)
			m_edge.push_back(std::make_pair(i, m_scaled_ver[i]));

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

		m_init_cons = m_cons;

	}
	else if (m_shape == ObjShape::O_SPHERE)
	{
		m_mass = 0.4f;
		m_acceleration = std::vector<glm::vec3>(ver, glm::vec3(0, m_gravity * m_mass, 0));
		m_velocity = std::vector<glm::vec3>(ver, glm::vec3(0));

		int last_index = static_cast<int>(m_scaled_ver.size())-1;
		//set constraints
		
		for (int i = 0; i <= dimension; ++i)
		{//horizontal
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

				if (j != dimension)
				{
					constraints d_cons;
					d_cons.p1 = h_cons.p1;
					d_cons.p2 = h_cons.p2 + dimension + 1;
					d_cons.restlen = glm::distance(m_scaled_ver[d_cons.p1], m_scaled_ver[d_cons.p2]);

					m_cons.push_back(d_cons);
				}

				if (j != 0)
				{
					constraints d_cons;
					d_cons.p1 = h_cons.p1;
					d_cons.p2 = h_cons.p2 -( dimension + 1);
					d_cons.restlen = glm::distance(m_scaled_ver[d_cons.p1], m_scaled_ver[d_cons.p2]);

					m_cons.push_back(d_cons);
				}


			}


			//vertical
			for (int j = 0; j < dimension; ++j)
			{
				constraints v_cons;
				v_cons.p1 = j * (dimension + 1) + i;
				v_cons.p2 = v_cons.p1 + (dimension + 1);
				v_cons.restlen = glm::distance(m_scaled_ver[v_cons.p1], m_scaled_ver[v_cons.p2]);

				m_cons.push_back(v_cons);

			}

		}

		for (int i = 0; i <= m_scaled_ver.size()/2; ++i)
		{
			constraints d_cons;
			d_cons.p1 = i;
			d_cons.p2 = last_index-i;
			d_cons.restlen = glm::distance(m_scaled_ver[d_cons.p1], m_scaled_ver[d_cons.p2]);

			m_cons.push_back(d_cons);
		}

		m_init_cons = m_cons;
	}


}

void SoftBodyPhysics::Update(float dt)
{
	Acceleration();
	
	//Move(dt);
	if(!isCollided)
		KeepConstraint();
	Verlet(dt);

	for (int i = 0; i < m_scaled_ver.size(); ++i)
		obj_vertices[i] = (m_scaled_ver[i] - position) / scale;
	
}

void SoftBodyPhysics::Verlet(float dt)
{
	float f = 0.99f;
	for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
	{
		glm::vec3* _new = &m_scaled_ver[i];
		glm::vec3 temp = *_new;
		glm::vec3* old = &m_old_ver[i];
		

		(*_new) +=  f * temp - f * (*old) + m_acceleration[i] * dt * dt;
		(*old) = temp;
	}
}

void SoftBodyPhysics::Move(float dt)
{
	for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
	{
		m_old_ver[i] = m_scaled_ver[i];
		m_velocity[i] += m_acceleration[i] * dt;
		m_scaled_ver[i] += m_velocity[i] * dt;
	}
}

void SoftBodyPhysics::KeepConstraint()
{
	for (int i = 0; i < 14; ++i)
	{
		//staying edge
		//for (unsigned j = 0; j < m_edge.size(); ++j)
		//	m_scaled_ver[j] = m_edge[j];

		for (unsigned j = 0; j < m_edge.size(); ++j)
			m_scaled_ver[m_edge[j].first] = m_edge[j].second;



		for (unsigned j = 0; j < m_cons.size(); ++j)
		{
			constraints& c = m_cons[j];
			glm::vec3& point1 = m_scaled_ver[c.p1];
			glm::vec3& point2 = m_scaled_ver[c.p2];
			glm::vec3& old_1 = m_old_ver[c.p1];
			glm::vec3& old_2 = m_old_ver[c.p2];

			glm::vec3 delta = point2 - point1;
			if (delta == glm::vec3(0, 0, 0))
				continue;

			float len = glm::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
			float diff = (len - c.restlen) / len;

			glm::vec3 force = stiffness * delta * diff;// -damping * glm::dot(point2 - old_2, point1 - old_1) * diff;
			//m_velocity[c.p1] -= force;
			//m_velocity[c.p2] += force;

			//point1 += delta * 0.5f * diff;
			//point2 -= delta * 0.5f * diff;
			
			//m_acceleration[c.p1] -= force;
			//m_acceleration[c.p2] += force;
			
			point1 += force;
			point2 -= force;
		}
	}

}

void SoftBodyPhysics::Acceleration()
{
	for(unsigned i = 0; i < m_acceleration.size(); ++i)
		m_acceleration[i] = glm::vec3(0, m_gravity* m_mass, 0);
}


void SoftBodyPhysics::CollisionResponseRigid(Object* _rhs)
{
	if (_rhs->m_shape == ObjShape::O_SPHERE)
	{
		glm::vec3 center = _rhs->position;
		float radius = _rhs->scale.x + 0.01f;
		float radius_sqr = radius * radius;
		for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
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
		int last = static_cast<int>(_rhs->obj_vertices.size()) - 1;
			
		glm::vec3 point0 = _rhs->m_model * glm::vec4(_rhs->obj_vertices[0], 1.f);
		glm::vec3 point1 = _rhs->m_model * glm::vec4(_rhs->obj_vertices[last], 1.f);
		glm::vec3 point2 = _rhs->m_model * glm::vec4(_rhs->obj_vertices[1], 1.f);

		glm::vec3 v = point1 - point0;
		glm::vec3 w = point2 - point0;

		//check distance
		_rhs->normalVec = glm::cross(v, w);
		_rhs->normalVec = glm::normalize(_rhs->normalVec);

		_rhs->d = -glm::dot(_rhs->normalVec, point0);
			
		float radius = 0.f;
		//float radius_sqr = radius * radius;
		bool collision = false;
		for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
		{
			glm::vec3& point = m_scaled_ver[i];
			float distance = 0;
			glm::vec3 moved = m_scaled_ver[i];

			glm::vec3 l_norm = _rhs->normalVec;
			//if (glm::dot(m_scaled_ver[i] - m_old_ver[i], _rhs->normalVec) > 0.f)
			//	l_norm = (-1.f) * l_norm;

			collision = IsCollidedPlane(point, point0, point1, center, radius, distance, l_norm, _rhs->d, moved);
			if (collision)
			{
				m_scaled_ver[i] = moved + (radius - distance) * l_norm;
				isCollided = true;
			}
		}

		if (!collision)
			isCollided = false;

	
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

bool SoftBodyPhysics::IsCollidedPlane(glm::vec3& point, glm::vec3& p_point0, glm::vec3& p_point1, glm::vec3& center, float& radius, float& distance, glm::vec3& norm, float d
,glm::vec3& movedpoint)
{
	//distance = std::abs(glm::dot(point, norm) + d);
	
	distance = glm::dot(point, norm) + d;

	if (std::abs(distance) < 0.2f && distance < 0.f)
	{
		movedpoint = point - (distance * norm);

		glm::vec3 max = glm::max(p_point0, p_point1);
		glm::vec3 min = glm::min(p_point0, p_point1);

		if (movedpoint.x >= min.x && movedpoint.x <= max.x
			&& movedpoint.y >= min.y && movedpoint.y <= max.y
			&& movedpoint.z >= min.z && movedpoint.z <= max.z)
			return true;
		else
			return false;
	}
	else
		return false;

}
