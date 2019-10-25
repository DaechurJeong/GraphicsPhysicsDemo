#include "Scene.h"
#include "input.h"

void Scene::Init(GLFWwindow* window, Camera* camera)
{
	if (curr_scene == 0)
		Scene0Init(camera);
	else if (curr_scene == 1)
		Scene1Init(camera);

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
	{
		Scene0Draw(camera, deltaTime);
	}
	else if (curr_scene == 1)
	{
		Scene1Draw(camera, deltaTime);
	}
	ImGuirender();
}
void Scene::Scene0Init(Camera* camera)
{
	// Generate objects for scene0
	Object* main_obj = new Object(O_SPHERE, glm::vec3(0.f, 0.f, -2.f), glm::vec3(1.f, 1.f, 1.f), dimension_);
	pbr_obj.push_back(main_obj);

	//////////////////////////PHYSICS TEST//////////////////
	Object* main_obj_texture = new Object(O_SPHERE, glm::vec3(0.9f, -2.5f, 2.f), glm::vec3(1.f, 1.f, 1.f), dimension_);
	m_physics.push_object(main_obj_texture);
	pbr_obj.push_back(main_obj_texture);

	Object* rigid_plane = new Object(O_PLANE, glm::vec3(3.5f, -5.f, 0.f), glm::vec3(4.f, 1.f, 4.f), dimension_);
	rigid_plane->rotation = 1.f;
	m_physics.push_object(rigid_plane);
	pbr_obj.push_back(rigid_plane);

	//SoftBodyPhysics* plane = new SoftBodyPhysics(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(4.f, 1.f, 7.f), dimension_);
	//m_physics.push_object(plane);
	//softbody_obj.push_back(plane);

	SoftBodyPhysics* sb_sphere = new SoftBodyPhysics(O_SPHERE, glm::vec3(4.5f, 1.f, 2.5f), glm::vec3(1.f, 1.f, 1.f), dimension_);
	m_physics.push_object(sb_sphere);
	softbody_obj.push_back(sb_sphere);

	// load PBR material textures
	albedo = main_obj_texture->loadTexture("models\\pbr\\rusted_iron\\albedo.png");
	normal = main_obj_texture->loadTexture("models\\pbr\\rusted_iron\\normal.png");
	metallic = main_obj_texture->loadTexture("models\\pbr\\rusted_iron\\metallic.png");
	roughness = main_obj_texture->loadTexture("models\\pbr\\rusted_iron\\roughness.png");
	ao = main_obj_texture->loadTexture("models\\pbr\\rusted_iron\\ao.png");

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
	albedo = main_obj_texture->loadTexture("models\\pbr\\self_made\\albedo.png");
	normal = main_obj_texture->loadTexture("models\\pbr\\self_made\\Sphere3D_1_defaultMat_Normal.png");
	metallic = main_obj_texture->loadTexture("models\\pbr\\self_made\\Sphere3D_1_defaultMat_Metallic.png");
	roughness = main_obj_texture->loadTexture("models\\pbr\\self_made\\Sphere3D_1_defaultMat_Roughness.png");
	ao = main_obj_texture->loadTexture("models\\pbr\\self_made\\ao.png");

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
	if (dt < 0.2f)
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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, roughness);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ao);

	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
		(*p_obj)->render_textured(camera, &pbr_texture_shader, (*p_obj)->position, aspect);
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
		(*s_obj)->render_textured(camera, &pbr_texture_shader, (*s_obj)->position, aspect);

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
	if (dt < 0.2f)
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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, roughness);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ao);

	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
		(*p_obj)->render_textured(camera, &pbr_texture_shader, (*p_obj)->position, aspect);
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
		(*s_obj)->render_line(camera, &pbr_texture_shader, (*s_obj)->position, aspect);

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
