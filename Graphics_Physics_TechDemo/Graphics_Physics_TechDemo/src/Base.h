/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Base.h
Purpose: Prototype of Softbody class and constraint
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Nahye Park, nahye.park
Creation date: 9/20/2018
End Header --------------------------------------------------------*/
#pragma once

#ifndef BASE_H
#define BASE_H

#include "Object.h"
#include <set>

#define GRAVITY -9.8f


struct constraints {
	constraints() { p1 = 0; p2 = 0; restlen = 0; }
	int p1;
	int p2;
	float restlen;
	bool operator<(constraints const& op2) const{
		if (p1 == op2.p1)
			return p2 < op2.p2;
		return (p1 < op2.p1);
	}
};

class SoftBodyPhysics : public Object
{

public:
	SoftBodyPhysics(ObjectShape shape, glm::vec3 pos, glm::vec3 scale_, int dim):Object(shape, pos, scale_, dim) {
		Init();
	}
	void Init();
	void Update(float dt);
	void KeepConstraint(float dt);
	void CollisionResponseRigid(Object* _rhs);
	void CollisionResponseSoft(SoftBodyPhysics* _rhs);

	void SetInitConstraints() { m_cons = m_init_cons; }
	bool colliding() { return isCollided; }

	std::vector<glm::vec3> m_scaled_ver;
	glm::vec3 m_min;
	glm::vec3 m_max;

	float m_mass;
	float stiffness;
	float damping;
private:
	void Verlet(float dt);
	void Move(float dt);
	
	void Acceleration();

	bool IsCollided(glm::vec3& point, glm::vec3& center, float& radius);
	bool IsCollidedPlane(glm::vec3& point, glm::vec3& p_point0, glm::vec3& p_point1, float& radius, float& distance, glm::vec3& norm, float d
	, glm::vec3& movedpoint);

	std::vector<glm::vec3> m_old_ver;
	std::vector<constraints> m_init_cons;
	std::vector<constraints> m_cons;
	std::vector<constraints> m_in_cons;
	std::set<constraints> m_const;
	std::set<std::pair<constraints, constraints>> m_volume_cons;

	float m_gravity;
	std::vector<glm::vec3> m_acceleration;
	std::vector<glm::vec3> m_velocity;
	bool isCollided;

	std::vector <std::pair<unsigned, glm::vec3>> m_edge;
};

#endif


