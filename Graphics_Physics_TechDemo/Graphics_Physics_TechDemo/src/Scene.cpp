#include "Scene.h"
#include "input.h"

const float FRAME_LIMIT = 1.f / 59.f;

void Scene::Init(GLFWwindow* window, Camera* camera)
{
	InitAllPBRTexture();
	if (curr_scene == 0)
		Scene0Init(camera);
	else if (curr_scene == 1)
		Scene1Init(camera);
	else if (curr_scene == 2)
		Scene2Init(camera);

	pbr_texture_shader.CreateShader("ShaderCodes\\pbr_texture.vs", "ShaderCodes\\pbr_texture.fs", nullptr);
	equirectangularToCubmapShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\equirectangular_to_cubemap.fs", nullptr);
	irradianceShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\irradiance_convolution.fs", nullptr);
	backgroundShader.CreateShader("ShaderCodes\\background.vs", "ShaderCodes\\background.fs", nullptr);
	brdfShader.CreateShader("ShaderCodes\\brdf.vs", "ShaderCodes\\brdf.fs", nullptr);
	prefilterShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\prefilter.fs", nullptr);

	pbr_texture_shader.Use();
	pbr_texture_shader.SetInt("irradianceMap", 0);
	pbr_texture_shader.SetInt("prefilterMap", 1);
	pbr_texture_shader.SetInt("brdfLUT", 2);

	pbr_texture_shader.SetInt("albedoMap", 3);
	pbr_texture_shader.SetInt("normalMap", 4);
	pbr_texture_shader.SetInt("metallicMap", 5);
	pbr_texture_shader.SetInt("roughnessMap", 6);
	pbr_texture_shader.SetInt("aoMap", 7);

	backgroundShader.Use();
	backgroundShader.SetInt("environmentMap", 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// pbr: setup framebuffer
	InitFrameBuffer(&equirectangularToCubmapShader, &irradianceShader, &prefilterShader, &brdfShader,
		captureFBO, captureRBO, envCubemap, irradianceMap, prefilterMap, brdfLUTTexture);
	InitSkybox(&backgroundShader, &pbr_texture_shader, camera, (float)width, (float)height);

	ResizeFrameBuffer(window);
}
void Scene::Update(GLFWwindow* window, Camera* camera, float dt)
{
	float currFrame = (float)glfwGetTime();
	deltaTime = currFrame - lastFrame;
	lastFrame = currFrame;

	ImGuiUpdate(window, camera, deltaTime);

	ProcessInput(camera, window, deltaTime);

	if (curr_scene == 0)
		Scene0Draw(camera, deltaTime);
	else if (curr_scene == 1)
		Scene1Draw(camera, deltaTime);
	else if (curr_scene == 2)
		Scene2Draw(camera, deltaTime);

	ImGuirender();
}
void Scene::Scene0Init(Camera* camera)
{
	Object* rigid_plane = new Object(O_PLANE, glm::vec3(4.f, -4.f, 1.f), glm::vec3(7.f, 1.f, 0.5f), dimension_);
	rigid_plane->rotation = 1.f;
	m_physics.push_object(rigid_plane);
	pbr_obj.push_back(rigid_plane);

	Object* rigid_plane_4 = new Object(O_PLANE, glm::vec3(4.f, -4.f, 2.5f), glm::vec3(7.f, 1.f, 0.5f), dimension_);
	rigid_plane_4->rotation = 1.f;
	m_physics.push_object(rigid_plane_4);
	pbr_obj.push_back(rigid_plane_4);

	Object* rigid_plane_2 = new Object(O_PLANE, glm::vec3(1.3f, -7.f, 0.5f), glm::vec3(4.f, 1.f, 4.f), dimension_);
	rigid_plane_2->rotation = -1.f;
	m_physics.push_object(rigid_plane_2);
	pbr_obj.push_back(rigid_plane_2);

	Object* rigid_plane_3 = new Object(O_PLANE, glm::vec3(4.f, -10.3f, 0.5f), glm::vec3(4.f, 1.f, 4.f), dimension_);
	rigid_plane_3->rotation = 1.f;
	m_physics.push_object(rigid_plane_3);
	pbr_obj.push_back(rigid_plane_3);

	//SoftBodyPhysics* plane = new SoftBodyPhysics(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(4.f, 1.f, 7.f), dimension_);
	//m_physics.push_object(plane);
	//softbody_obj.push_back(plane);

	SoftBodyPhysics* sb_sphere = new SoftBodyPhysics(O_SPHERE, glm::vec3(6.5f, 0.f, 2.f), glm::vec3(1.f, 1.f, 1.f), dimension_);
	m_physics.push_object(sb_sphere);
	softbody_obj.push_back(sb_sphere);

	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		pbr_obj[i]->albedo = pbr_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_BaseColor.png");
		pbr_obj[i]->normal = pbr_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Normal.png");
		pbr_obj[i]->metallic = pbr_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Metallic.png");
		pbr_obj[i]->roughness = pbr_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Roughness.png");
		pbr_obj[i]->ao = pbr_obj[i]->loadTexture("models\\pbr\\Wood\\ao.png");
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		softbody_obj[i]->albedo = softbody_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_BaseColor.png");
		softbody_obj[i]->normal = softbody_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Normal.png");
		softbody_obj[i]->metallic = softbody_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Metallic.png");
		softbody_obj[i]->roughness = softbody_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Roughness.png");
		softbody_obj[i]->ao = softbody_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\ao.png");
	}

	// light properties
	for (int i = 0; i < 4; ++i)
	{
		Light m_light;
		m_light.color = glm::vec3(300.f, 300.f, 300.f);
		light.push_back(m_light);
	}
	light[0].position = glm::vec3(10.f, 10.f, 10.f);
	light[1].position = glm::vec3(-10.f, 10.f, 10.f);
	light[2].position = glm::vec3(-10.f, -10.f, 10.f);
	light[3].position = glm::vec3(10.f, -10.f, 10.f);
}
void Scene::Scene1Init(Camera* camera)
{
	// Generate objects for scene0
	Object* main_obj_texture = new Object(O_SPHERE, glm::vec3(0.9f, -2.5f, 2.f), glm::vec3(1.f, 1.f, 1.f), dimension_);
	m_physics.push_object(main_obj_texture);
	pbr_obj.push_back(main_obj_texture);

	SoftBodyPhysics* plane = new SoftBodyPhysics(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(4.f, 1.f, 7.f), dimension_);
	m_physics.push_object(plane);
	softbody_obj.push_back(plane);

	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		pbr_obj[i]->albedo = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_BaseColor.png");
		pbr_obj[i]->normal = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Normal.png");
		pbr_obj[i]->metallic = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Metallic.png");
		pbr_obj[i]->roughness = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Roughness.png");
		pbr_obj[i]->ao = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\ao.png");
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		softbody_obj[i]->albedo = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_BaseColor.png");
		softbody_obj[i]->normal = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Normal.png");
		softbody_obj[i]->metallic = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Metallic.png");
		softbody_obj[i]->roughness = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Roughness.png");
		softbody_obj[i]->ao = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\ao.png");
	}
	// light properties
	for (int i = 0; i < 2; ++i)
	{
		Light m_light;
		m_light.color = glm::vec3(300.f, 300.f, 300.f);
		light.push_back(m_light);
	}
	light[0].position = glm::vec3(10.f, 10.f, 10.f);
	light[1].position = glm::vec3(-10.f, 10.f, 10.f);
}
void Scene::Scene2Init(Camera* camera)
{

	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		pbr_obj[i]->albedo = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_BaseColor.png");
		pbr_obj[i]->normal = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Normal.png");
		pbr_obj[i]->metallic = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Metallic.png");
		pbr_obj[i]->roughness = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Roughness.png");
		pbr_obj[i]->ao = pbr_obj[i]->loadTexture("models\\pbr\\selfmade_plastic\\ao.png");
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		softbody_obj[i]->albedo = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_BaseColor.png");
		softbody_obj[i]->normal = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Normal.png");
		softbody_obj[i]->metallic = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Metallic.png");
		softbody_obj[i]->roughness = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Roughness.png");
		softbody_obj[i]->ao = softbody_obj[i]->loadTexture("models\\pbr\\Wood\\ao.png");
	}
	// light properties
	for (int i = 0; i < 2; ++i)
	{
		Light m_light;
		m_light.color = glm::vec3(300.f, 300.f, 300.f);
		light.push_back(m_light);
	}
	light[0].position = glm::vec3(10.f, 10.f, 10.f);
	light[1].position = glm::vec3(-10.f, 10.f, 10.f);
}
void Scene::Scene0Draw(Camera* camera, float dt)
{
	if (dt <= FRAME_LIMIT)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->albedo + 2);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->normal + 2);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->metallic + 2);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->roughness + 2);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->ao + 2);
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->albedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->normal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->metallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->roughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->ao);
	}

	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*p_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*p_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*p_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*p_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*p_obj)->ao);
		(*p_obj)->render_textured(camera, &pbr_texture_shader, (*p_obj)->position, aspect);
	}
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*s_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*s_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*s_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*s_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*s_obj)->ao);
		(*s_obj)->render_textured(camera, &pbr_texture_shader, (*s_obj)->position, aspect);
	}
	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::Scene1Draw(Camera* camera, float dt)
{
	if (dt <= FRAME_LIMIT)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->albedo + 2);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->normal + 2);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->metallic + 2);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->roughness + 2);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->ao + 2);
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->albedo + 2);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->normal + 2);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->metallic + 2);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->roughness + 2);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->ao + 2);
	}

	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*p_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*p_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*p_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*p_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*p_obj)->ao);
		(*p_obj)->render_textured(camera, &pbr_texture_shader, (*p_obj)->position, aspect);
	}
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*s_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*s_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*s_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*s_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*s_obj)->ao);
		(*s_obj)->render_textured(camera, &pbr_texture_shader, (*s_obj)->position, aspect);
	}
	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}

void Scene::Scene2Draw(Camera* camera, float dt)
{
	if (dt <= FRAME_LIMIT)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	unsigned index = textIndex;
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3 + i * 5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->albedo);
		glActiveTexture(GL_TEXTURE4 + i * 5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->normal);
		glActiveTexture(GL_TEXTURE5 + i * 5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->metallic);
		glActiveTexture(GL_TEXTURE6 + i * 5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->roughness);
		glActiveTexture(GL_TEXTURE7 + i * 5);
		glBindTexture(GL_TEXTURE_2D, pbr_obj[i]->ao);
		index += 5;
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE3 + i * 5 + index);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->albedo);
		glActiveTexture(GL_TEXTURE4 + i * 5 + index);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->normal);
		glActiveTexture(GL_TEXTURE5 + i * 5 + index);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->metallic);
		glActiveTexture(GL_TEXTURE6 + i * 5 + index);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->roughness);
		glActiveTexture(GL_TEXTURE7 + i * 5 + index);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->ao);
		index += 5;
	}
	textIndex = index;

	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*p_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*p_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*p_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*p_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*p_obj)->ao);
		(*p_obj)->render_textured(camera, &pbr_texture_shader, (*p_obj)->position, aspect);
	}
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*s_obj)->albedo);
		pbr_texture_shader.SetInt("normalMap", (*s_obj)->normal);
		pbr_texture_shader.SetInt("metallicMap", (*s_obj)->metallic);
		pbr_texture_shader.SetInt("roughnessMap", (*s_obj)->roughness);
		pbr_texture_shader.SetInt("aoMap", (*s_obj)->ao);
		(*s_obj)->render_textured(camera, &pbr_texture_shader, (*s_obj)->position, aspect);
	}

	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// lighting
	for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
	{
		glm::vec3 newPos = light[i].position + glm::vec3(sin(dt * 5.0) * 5.0, 0.0, 0.0);
		newPos = light[i].position;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
	}

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::ImGuiUpdate(GLFWwindow* window, Camera* camera, float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	bool show_demo_window = false;

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	{
		ImGui::Begin("GUI interface");
		ImGui::Text("Object controller");
		ImGui::Text("Frame Per Second : %d ms", static_cast<int>(1.f / dt));
		ImGui::SliderFloat("metallic", &met, 0.f, 1.f);
		ImGui::SliderFloat("roughness", &rou, 0.f, 1.f);

		if (ImGui::Button("Default roughness"))
			roughness_status = false;
		if (ImGui::Button("Control roughness"))
			roughness_status = true;
		if (ImGui::Button("Default metallic"))
			metallic_status = false;
		if (ImGui::Button("Control metallic"))
			metallic_status = true;
		ImGui::End();
	}
	if (second_imgui)
	{
		ImGui::Begin("Scene selector");
		if (ImGui::Button("Scene0"))
		{
			if (curr_scene != 0)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 0;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene1"))
		{
			if (curr_scene != 1)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 1;
				Init(window, camera);
			}
		}
		ImGui::End();
	}
	if (third_imgui)
	{
		ImGui::Begin("Select PBR texture");
		if (ImGui::Button("Plastic"))
		{
			ChangePBRTexture(PLASTIC);
		}
		if (ImGui::Button("Steel"))
		{
			ChangePBRTexture(STEEL);
		}
		if (ImGui::Button("Wood"))
		{
			ChangePBRTexture(WOOD);
		}
		if (ImGui::Button("Rusted-Iron"))
		{
			ChangePBRTexture(RUSTED_IRON);
		}
		if (ImGui::Button("Fabric"))
		{
			ChangePBRTexture(FABRIC);
		}
		if (ImGui::Button("TornFabric"))
		{
			ChangePBRTexture(TORN_FABRIC);
		}
		if (ImGui::Button("Aluminium"))
		{
			ChangePBRTexture(ALUMINIUM);
		}
		ImGui::End();
	}
	if (forth_imgui)
	{
		ImGui::Begin("Reload");
		if (ImGui::Button("Reload"))
		{
			Reload(camera);
		}
		ImGui::End();
	}
}
void Scene::ImGuirender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Scene::ImGuiShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void Scene::Reload(Camera* camera)
{
	ShutDown();
	if (curr_scene == 0)
		Scene0Init(camera);
	else if (curr_scene == 1)
		Scene1Init(camera);
}
void Scene::ShutDown()
{
	for (auto p_obj : pbr_obj)
	{
		delete p_obj;
		p_obj = NULL;
	}
	if(!pbr_obj.empty())
		pbr_obj.clear();
	for (auto s_obj : softbody_obj)
	{
		delete s_obj;
		s_obj = NULL;
	}
	if(!softbody_obj.empty())
		softbody_obj.clear();
	m_physics.clear_objects();
	if(!light.empty())
		light.clear();
}
void Scene::ResizeFrameBuffer(GLFWwindow* window)
{
	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}
void Scene::InitAllPBRTexture()
{
	Object* rigid_plane = new Object(O_PLANE, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), dimension_);

	// plastic
	albedo[0] = rigid_plane->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[0] = rigid_plane->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Normal.png");
	metallic[0] = rigid_plane->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[0] = rigid_plane->loadTexture("models\\pbr\\selfmade_plastic\\Sphere3D_1_defaultMat_Roughness.png");
	ao[0] = rigid_plane->loadTexture("models\\pbr\\selfmade_plastic\\ao.png");

	// steel
	albedo[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Normal.png");
	metallic[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Roughness.png");
	ao[1] = rigid_plane->loadTexture("models\\pbr\\steel\\ao.png");

	// wood
	albedo[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Normal.png");
	metallic[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Roughness.png");
	ao[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\ao.png");

	// rusted_iron
	albedo[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Normal.png");
	metallic[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Roughness.png");
	ao[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\ao.png");

	// fabric
	albedo[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Normal.png");
	metallic[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Roughness.png");
	ao[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\ao.png");

	// torn-fabric
	albedo[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Normal.png");
	metallic[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Roughness.png");
	ao[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\ao.png");

	// aluminium
	albedo[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Normal.png");
	metallic[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Roughness.png");
	ao[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\ao.png");
}

void Scene::ChangePBRTexture(TextureType type)
{
	//m_textype = type;
	if (type == PLASTIC)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[0];
			pbr_obj[i]->normal = normal[0];
			pbr_obj[i]->metallic = metallic[0];
			pbr_obj[i]->roughness = roughness[0];
			pbr_obj[i]->ao = ao[0];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[0];
			softbody_obj[i]->normal = normal[0];
			softbody_obj[i]->metallic = metallic[0];
			softbody_obj[i]->roughness = roughness[0];
			softbody_obj[i]->ao = ao[0];
		}
	}
	if (type == STEEL)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[1];
			pbr_obj[i]->normal = normal[1];
			pbr_obj[i]->metallic = metallic[1];
			pbr_obj[i]->roughness = roughness[1];
			pbr_obj[i]->ao = ao[1];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[1];
			softbody_obj[i]->normal = normal[1];
			softbody_obj[i]->metallic = metallic[1];
			softbody_obj[i]->roughness = roughness[1];
			softbody_obj[i]->ao = ao[1];
		}
	}
	if (type == WOOD)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[2];
			pbr_obj[i]->normal = normal[2];
			pbr_obj[i]->metallic = metallic[2];
			pbr_obj[i]->roughness = roughness[2];
			pbr_obj[i]->ao = ao[2];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[2];
			softbody_obj[i]->normal = normal[2];
			softbody_obj[i]->metallic = metallic[2];
			softbody_obj[i]->roughness = roughness[2];
			softbody_obj[i]->ao = ao[2];
		}
	}
	if (type == RUSTED_IRON)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[3];
			pbr_obj[i]->normal = normal[3];
			pbr_obj[i]->metallic = metallic[3];
			pbr_obj[i]->roughness = roughness[3];
			pbr_obj[i]->ao = ao[3];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[3];
			softbody_obj[i]->normal = normal[3];
			softbody_obj[i]->metallic = metallic[3];
			softbody_obj[i]->roughness = roughness[3];
			softbody_obj[i]->ao = ao[3];
		}
	}
	else if (type == FABRIC)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[4];
			pbr_obj[i]->normal = normal[4];
			pbr_obj[i]->metallic = metallic[4];
			pbr_obj[i]->roughness = roughness[4];
			pbr_obj[i]->ao = ao[4];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[4];
			softbody_obj[i]->normal = normal[4];
			softbody_obj[i]->metallic = metallic[4];
			softbody_obj[i]->roughness = roughness[4];
			softbody_obj[i]->ao = ao[4];
		}
	}
	else if (type == TORN_FABRIC)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[5];
			pbr_obj[i]->normal = normal[5];
			pbr_obj[i]->metallic = metallic[5];
			pbr_obj[i]->roughness = roughness[5];
			pbr_obj[i]->ao = ao[5];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[5];
			softbody_obj[i]->normal = normal[5];
			softbody_obj[i]->metallic = metallic[5];
			softbody_obj[i]->roughness = roughness[5];
			softbody_obj[i]->ao = ao[5];
		}
	}
	else if (type == ALUMINIUM)
	{
		for (unsigned i = 0; i < pbr_obj.size(); ++i)
		{
			pbr_obj[i]->albedo = albedo[6];
			pbr_obj[i]->normal = normal[6];
			pbr_obj[i]->metallic = metallic[6];
			pbr_obj[i]->roughness = roughness[6];
			pbr_obj[i]->ao = ao[6];
		}
		for (unsigned i = 0; i < softbody_obj.size(); ++i)
		{
			softbody_obj[i]->albedo = albedo[6];
			softbody_obj[i]->normal = normal[6];
			softbody_obj[i]->metallic = metallic[6];
			softbody_obj[i]->roughness = roughness[6];
			softbody_obj[i]->ao = ao[6];
		}
	}
}