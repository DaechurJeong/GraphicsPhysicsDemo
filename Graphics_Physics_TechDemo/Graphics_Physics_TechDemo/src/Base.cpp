#include "Base.h"

void SoftBodyPhysics::Init()
{
	m_gravity = GRAVITY;
	m_old_ver = obj_vertices;
	m_dimension = 5;

	//set constraints
	for (int i = 0; i < m_dimension; ++i)
	{
		m_edge.push_back(obj_vertices[i]);
		for (int j = 0; j < m_dimension - 1 ; ++j)
		{
			constraints h_cons;
			h_cons.p1 = j * m_dimension + i;
			h_cons.p2 = h_cons.p1 + 1;
			h_cons.restlen = (obj_vertices[h_cons.p1] - obj_vertices[h_cons.p2]).length();

			m_cons.push_back(h_cons);

			constraints v_cons;
			v_cons.p1 = h_cons.p1;
			v_cons.p2 = v_cons.p1 + m_dimension;
			v_cons.restlen = (obj_vertices[v_cons.p1] - obj_vertices[v_cons.p2]).length();

			m_cons.push_back(v_cons);
		}
	}


}

void SoftBodyPhysics::Update(float dt)
{


	Acceleration();
	Verlet(dt);
	KeepConstraint();
}

void SoftBodyPhysics::Verlet(float dt)
{
	float f = 0.99f;
	for (int i = 0; i < obj_vertices.size(); ++i)
	{
		glm::vec3* temp = &obj_vertices[i];

		obj_vertices[i] += f * obj_vertices[i] - f * m_old_ver[i] + m_acceleration * dt * dt;
		m_old_ver[i] = *temp;
	}
}

void SoftBodyPhysics::KeepConstraint()
{
	for (int i = 0; i < 8; ++i)
	{
		//staying edge
		for (int j = 0; j < m_edge.size(); ++j)
			obj_vertices[j] = m_edge[j];


		for (int j = 0; j < m_cons.size(); ++j)
		{
			constraints& c = m_cons[j];
			glm::vec3& point1 = obj_vertices[c.p1];
			glm::vec3& point2 = obj_vertices[c.p2];

			glm::vec3 delta = point2 - point1;
			float len = delta.length();
			float diff = (len - c.restlen) / len;

			point1 += delta * 0.5f * diff;
			point2 -= delta * 0.5f * diff;
		}
	}

}

void SoftBodyPhysics::Acceleration()
{
	m_acceleration = glm::vec3(0, m_gravity, 0);
}
