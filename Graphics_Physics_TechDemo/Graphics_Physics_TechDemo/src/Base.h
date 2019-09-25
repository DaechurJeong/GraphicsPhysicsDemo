#pragma once

#ifndef BASE_H
#define BASE_H

#include "Object.h"
#include "Vector3.h"
//#include <vector>

#define GRAVITY -4.0f

struct constraints {
	constraints() { p1 = 0; p2 = 0; restlen = 0; }
	int p1;
	int p2;
	float restlen;
};

class SoftBodyPhysics : public Object
{

public:
	SoftBodyPhysics() {}
	void Init();
	void Update(float dt);

private:
	void Verlet(float dt);
	void KeepConstraint();
	void Acceleration();

	std::vector<glm::vec3> m_scaled_ver;
	std::vector<glm::vec3> m_old_ver;
	std::vector<constraints> m_cons;
	float m_gravity;
	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;

	int m_dimension;

	//temp
	std::vector <glm::vec3> m_edge;

};

#endif


