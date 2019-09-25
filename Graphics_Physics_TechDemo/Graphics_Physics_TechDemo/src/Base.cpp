#include "Base.h"

void SoftBodyPhysics::Init()
{
	m_gravity = GRAVITY;
	m_scaled_ver = obj_vertices;
	for (int i = 0; i < m_scaled_ver.size(); ++i)
		m_scaled_ver[i] = position + m_scaled_ver[i]*scale;
	m_old_ver = m_scaled_ver;
	m_dimension = 33;

	for(int i = 0; i < m_dimension; ++i)
		m_edge.push_back(m_scaled_ver[i]);

	//set constraints
	//horizontal
	for (int i = 0; i < m_dimension-1; ++i)
	{
		
		for (int j = 0; j < m_dimension ; ++j)
		{
			constraints h_cons;
			h_cons.p1 = j * m_dimension + i;
			h_cons.p2 = h_cons.p1 + 1;

			h_cons.restlen = scale.x / (m_dimension - 1);
			//(m_scaled_ver[h_cons.p1] - m_scaled_ver[h_cons.p2]).length();

			m_cons.push_back(h_cons);

		}
	}

	//vertical
	for (int i = 0; i < m_dimension; ++i)
	{
		for (int j = 0; j < m_dimension -1 ; ++j)
		{
			constraints v_cons;
			v_cons.p1 = j * m_dimension + i;
			v_cons.p2 = v_cons.p1 + m_dimension;
			v_cons.restlen = scale.z / (m_dimension - 1);
				//(m_scaled_ver[v_cons.p1] - m_scaled_ver[v_cons.p2]).length();

			m_cons.push_back(v_cons);
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
		glm::vec3* temp = &m_scaled_ver[i];

		m_scaled_ver[i] += f * m_scaled_ver[i] - f * m_old_ver[i] + m_acceleration * dt * dt;
		m_old_ver[i] = *temp;
	}
}

void SoftBodyPhysics::KeepConstraint()
{
	for (int i = 0; i < 8; ++i)
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
			float len = glm::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
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
