/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Base.cpp
Purpose: Implementation of Softbody Physics
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Nahye Park, nahye.park
Creation date: 9/20/2018
End Header --------------------------------------------------------*/
#include "Base.h"
#include <iostream>

#define KEEP_CONS_SPEED 35.f

void SoftBodyPhysics::Init()
{
	m_gravity = GRAVITY;
	m_scaled_ver = obj_vertices;

	m_scaled_ver.push_back(position);

	unsigned ver = static_cast<unsigned>(m_scaled_ver.size());
	for (unsigned i = 0; i < ver-1; ++i)
		m_scaled_ver[i] = position + m_scaled_ver[i]*scale;
	m_old_ver = m_scaled_ver;
	stiffness = 0.3f;
	damping = 0.5f;

	isCollided = false;

	if(m_shape == ObjShape::O_PLANE)
	{ 
		m_mass = 0.3f;
		m_acceleration = std::vector<glm::vec3>(ver, glm::vec3(0, m_gravity * m_mass, 0));
		m_velocity = std::vector<glm::vec3>(ver, glm::vec3(0));

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

				m_const.insert(h_cons);
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

				m_const.insert(v_cons);
			}
		}
	}
	else if (m_shape == ObjShape::O_SPHERE)
	{
		m_mass = 0.4f;
		m_acceleration = std::vector<glm::vec3>(ver, glm::vec3(0, m_gravity * m_mass, 0));
		m_velocity = std::vector<glm::vec3>(ver, glm::vec3(0));

		//up & down side
		for (int i = 1; i <= dimension; ++i)
		{
			constraints up_cons;
			up_cons.p1 = 0;
			up_cons.p2 = i;
			up_cons.restlen = 0;
			m_const.insert(up_cons);

			constraints down_cons;
			down_cons.p1 = ver - 2;
			down_cons.p2 = down_cons.p1 - i;
			down_cons.restlen = 0;
			m_const.insert(down_cons);
		}

		//face
		int curr = 0;
		for (int i = 1; i < obj_indices.size(); ++i)
		{
			constraints cons;
			cons.p1 = obj_indices[curr];
			cons.p2 = obj_indices[i];
			cons.restlen = glm::distance(m_scaled_ver[cons.p1], m_scaled_ver[cons.p2]);
			m_const.insert(cons);

			curr = i;
		}

		//volume constraints
		unsigned unit = dimension / 2;
		//vertical
		for (unsigned i = 0; i < unit; ++i)
		{
			//horizontal
			for (unsigned j = 0; j < unit; ++j)
			{
				constraints vol_cons1;
				constraints vol_cons2;

				vol_cons1.p1 = i * (dimension + 1) + j;
				vol_cons2.p1 = vol_cons1.p1 + unit;

				vol_cons2.p2 = (dimension + 1) * (unit + i) + j;
				vol_cons1.p2 = vol_cons2.p2 + unit;

				vol_cons1.restlen = glm::distance(m_scaled_ver[vol_cons1.p1], m_scaled_ver[vol_cons1.p2]);
				vol_cons2.restlen = glm::distance(m_scaled_ver[vol_cons2.p1], m_scaled_ver[vol_cons2.p2]);

				m_volume_cons.insert(std::make_pair(vol_cons1, vol_cons2));
			}
		}
	}
}

void SoftBodyPhysics::Update(float dt)
{
	Acceleration();

	Verlet(dt);

	KeepConstraint(dt);

	position = m_scaled_ver[m_scaled_ver.size() - 1];
	glm::vec3 center = glm::vec3(0);
	m_min = m_scaled_ver[0];
	m_max = m_scaled_ver[0];
	
	center += m_scaled_ver[right];
	center += m_scaled_ver[left];
	center += m_scaled_ver[front];
	center += m_scaled_ver[back];
	center += m_scaled_ver[up];
	center += m_scaled_ver[bottom];

	position = center;
	position.x /= 6;
	position.y /= 6;
	position.z /= 6;

	for (unsigned i = 0; i < m_scaled_ver.size() - 1; ++i)
	{
		if (m_min.x > m_scaled_ver[i].x)
			m_min.x = m_scaled_ver[i].x;
		if (m_max.x < m_scaled_ver[i].x)
			m_max.x = m_scaled_ver[i].x;
		if (m_min.y > m_scaled_ver[i].y)
			m_min.y = m_scaled_ver[i].y;
		if (m_max.y < m_scaled_ver[i].y)
			m_max.y = m_scaled_ver[i].y;
		if (m_min.z > m_scaled_ver[i].z)
			m_min.z = m_scaled_ver[i].z;
		if (m_max.z < m_scaled_ver[i].z)
			m_max.z = m_scaled_ver[i].z;

		obj_vertices[i] = (m_scaled_ver[i] - position) / scale;
	}
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

void SoftBodyPhysics::KeepConstraint(float dt)
{
	for (int i = 0; i < 7; ++i)
	{
		//staying edge
		for (unsigned j = 0; j < m_edge.size(); ++j)
			m_scaled_ver[m_edge[j].first] = m_edge[j].second;

		for (auto& j : m_const)
		{
			glm::vec3& point1 = m_scaled_ver[j.p1];
			glm::vec3& point2 = m_scaled_ver[j.p2];
			glm::vec3& old_1 = m_old_ver[j.p1];
			glm::vec3& old_2 = m_old_ver[j.p2];

			glm::vec3 delta = point2 - point1;
			if (delta == glm::vec3(0, 0, 0))
				continue;

			float len = glm::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
			float diff = (len - j.restlen) / len;

			glm::vec3 force = stiffness * delta * diff * dt * KEEP_CONS_SPEED;
	
			point1 += force;
			point2 -= force;
		}

		for (auto& j : m_volume_cons)
		{
			glm::vec3& point1 = m_scaled_ver[j.first.p1];
			glm::vec3& point2 = m_scaled_ver[j.first.p2];
			glm::vec3& old_1 = m_old_ver[j.first.p1];
			glm::vec3& old_2 = m_old_ver[j.first.p2];

			glm::vec3& point3 = m_scaled_ver[j.second.p1];
			glm::vec3& point4 = m_scaled_ver[j.second.p2];
			glm::vec3& old_3 = m_old_ver[j.second.p1];
			glm::vec3& old_4 = m_old_ver[j.second.p2];

			glm::vec3 delta1 = point2 - point1;
			glm::vec3 delta2 = point4 - point3;

			float len = glm::distance(point2, point1) + glm::distance(point4, point3);
			float diff = (len - j.first.restlen - j.second.restlen) / len;

			glm::vec3 force1 = stiffness * delta1 * diff * 0.5f * dt * KEEP_CONS_SPEED;
			point1 += force1;
			point2 -= force1;

			glm::vec3 force2 = stiffness * delta2 * diff * 0.5f * dt * KEEP_CONS_SPEED;
			point3 += force2;
			point4 -= force2;

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
		bool collision = false;
		for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
		{
			glm::vec3& point = m_scaled_ver[i];
			float distance = 0;
			glm::vec3 moved = m_scaled_ver[i];

			glm::vec3 l_norm = _rhs->normalVec;

			collision = IsCollidedPlane(point, point0, point1, radius, distance, l_norm, _rhs->d, moved);
			if (collision)
			{
				m_scaled_ver[i] = moved;
				isCollided = true;
			}
		}
		if (!collision)
			isCollided = false;
	}
}

void SoftBodyPhysics::CollisionResponseSoft(SoftBodyPhysics* _rhs)
{
	float radius = 0.0f;
	bool collision = false;

	if (glm::distance(position, _rhs->position) > scale[0] + _rhs->scale[0])
		return;

	glm::vec3 direction = _rhs->position - position;
	for (unsigned j = 0; j < _rhs->obj_indices.size() - 3; j += 3)
	{
		glm::vec3 point0 = _rhs->m_scaled_ver[_rhs->obj_indices[j]];
		glm::vec3 point1 = _rhs->m_scaled_ver[_rhs->obj_indices[j+2]];
		glm::vec3 point2 = _rhs->m_scaled_ver[_rhs->obj_indices[j+1]];
		
		if (glm::dot(-direction, point0 - _rhs->position) < 0)
			continue;

		glm::vec3 v = point1 - point0;
		glm::vec3 w = point2 - point0;

		//check distance
		glm::vec3 normalvec = glm::cross(v, w);
		normalvec = glm::normalize(normalvec);

		float d = -glm::dot(normalvec, point0);
		
		for (unsigned i = 0; i < m_scaled_ver.size(); ++i)
		{
			glm::vec3& point = m_scaled_ver[i];
			
			if (glm::dot(direction, point - position) < 0)
				continue;

			float distance = 0;
			glm::vec3 moved = m_scaled_ver[i];

			glm::vec3 l_norm = normalvec;

			collision = IsCollidedPlane(point, point0, point1, radius, distance, l_norm, d, moved);
			if (collision)
			{
				m_scaled_ver[i] = moved;
				isCollided = true;
			}
		}
	}
	if (!collision)
		isCollided = false;
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

bool SoftBodyPhysics::IsCollidedPlane(glm::vec3& point, glm::vec3& p_point0, glm::vec3& p_point1, float& radius, float& distance, glm::vec3& norm, float d
,glm::vec3& movedpoint)
{
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
