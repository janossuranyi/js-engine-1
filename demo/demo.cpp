#include <chrono>
#include <cmath>
#include <iterator>
#include <list>
#include <iostream>
#include <string>
#include "SDL.h"

#include "graphics/GraphicsTypes.hpp"
#include "graphics/GraphicsDriver.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/ShaderManager.hpp"
#include "impl/GraphicsDriverOGL.hpp"
#include "system/SystemTypes.hpp"
#include "system/Filesystem.hpp"
#include "system/Timer.hpp"
#include "system/Logger.hpp"
#include "scene/Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "system/Heap.hpp"
#include "impl/InputServiceSDL.hpp"

#include "cube.hpp"

#define NUM_FRAME_BUF 2

using namespace jse;
using namespace std::chrono;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_ASPECT (float(WINDOW_WIDTH)/float(WINDOW_HEIGHT))

void X_add_light_cube(Scene& aScene, const Vector3f& aPos, const Color3 aColor, int aLightNum)
{
	MeshQueryResult cubeMesh = aScene.GetMeshByName("Cube");
	Node3d* customNode = new Node3d("Custom" + std::to_string(aLightNum));

	if (cubeMesh.second > -1)
	{
		Mesh3d m = *cubeMesh.first;

		Material custMat;
		custMat.ambient = aColor;
		custMat.diffuse = Color3(0.f);
		custMat.specular = Color3(0.f);
		custMat.type = MaterialType_Specular;

		m.SetName("LightCube" + std::to_string(aLightNum));
		m.SetMaterial(custMat);
		int idx = aScene.AddMesh(m);

		customNode->AddMesh(idx);
		customNode->SetPosition(aPos);
		customNode->SetScale(.5f);
		customNode->SetVisible(true);

		aScene.AddNode(customNode);

		customNode->UpdateWorldTransform(true);
	}
}

int main(int argc, char** argv)
{

	FileSystem fs;

    fs.SetWorkingDir("d:/src/js-engine-1/assets");

	GraphicsDriver* gl = new GraphicsDriverOGL();
	
	const Vector2l pos(0);
	const Color clearColor(0.f, 0.f, 0.3f, 1.f);
	int n = 0;

	gl->Init(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 32, 0, 4, GpuProgramFormat_GLSL, "Hello OpenGL", pos, true);

	ShaderManager sm(gl, &fs);

	if (!sm.Init()) {
		return 255;
	}

	Scene scene("Scene1", &sm, gl, &fs);

	//scene.LoadScene(fs.Resolve("test.dae"), true);
	scene.LoadScene(fs.Resolve("test2.gltf"));
	scene.Compile();


	GpuShader*		prog	= gl->CreateGpuShader();
	GpuShaderStage* s_vtx	= gl->CreateGpuShaderStage(ShaderStage_Vertex, "simple_vtx.glsl");
	GpuShaderStage* s_frg	= gl->CreateGpuShaderStage(ShaderStage_Fragment, "simple_frag.glsl");

	u32 frameNum = 0;

	n = gl->GetCaps(GraphicsCaps_MaxTextureImageUnits);
	Info("Max texture units: %d", n);

	n = gl->GetCaps(GraphicCaps_AutoGenerateMipMaps);
	Info("AutoGenerateMipMaps: %d", n);

	gl->SetClearColor(clearColor);
	gl->SetVSyncEnabled(true, false);

	bool runOnce = false;

	float rotAngle = 0;

	uint64_t frametime = SDL_GetTicks64();

	
	float lastf = 0.0f;
	float x = 0.0f;
	float z = 30.0f;
	float lPower = 100.f;
	float Rl = 0.2f;

	bool running = true;
	InputService* input = new InputServiceSDL();

	float vX = 0.0f, vY = 0.0f, vZ = 5.0f;

	input->SetRelativeMouseMode(true);

	scene.SetPerspectiveCameraLens(glm::radians(45.0f), WINDOW_ASPECT, 0.1f, 100.0f);
	scene.SetDefaultLightRadius(Rl);
	
	Node3d* Icosphere = scene.GetNodeByName("Icosphere");
	Node3d* Torus02 = scene.GetNodeByName("Torus02");
	Node3d* Torus01 = scene.GetNodeByName("Torus01");

	float torusRotX = 0.0f;
	float icoRotY = 0.0f;

	const Vector3f AxisX(1.f, 0.f, 0.f);
	const Vector3f AxisY(0.f, 1.f, 0.f);
	const Vector3f AxisZ(0.f, 0.f, 1.f);

	const LightVec& lights = scene.GetLights();

	int ln = 1;
	std::for_each(lights.begin(), lights.end(), [&](const Light* it) {

		const PointLight* light = reinterpret_cast<const PointLight*>(it);
		const Vector3f color = light->GetDiffuse();

		X_add_light_cube(scene, light->GetPosition(), glm::normalize(color), ln++);
	});

	scene.Compile();

	Quat q1 = glm::angleAxis(0.0f, AxisY);
	Quat q2 = glm::angleAxis(glm::radians(300.f), AxisY);

	float f = 0.0f;
	float fd = 0.02f;

	while (running && !runOnce)
	{
//		SimpleTimer timer;

		float fTime = SDL_GetTicks64() / 1000.0;
		
		float Dt = (SDL_GetTicks64() - frametime) / 60.0f;
		frametime = SDL_GetTicks64();


		Vector3f viewPos(vX, vY, vZ);

		scene.SetCameraPos(viewPos, Vector3f(0.0f), Vector3f(0.0f, 1.0f, 0.0f));


		Quat q = glm::slerp(q1, q2, f);
		Quat z = glm::angleAxis(glm::radians(2*icoRotY), AxisZ);
		Quat y = glm::angleAxis(glm::radians(icoRotY), AxisY);
		Quat x = glm::angleAxis(glm::radians(-icoRotY), AxisX);
		Icosphere->SetPosition(vec3(0.0f, 1.2f * glm::sin(glm::radians(icoRotY)), 0.0f));
		Icosphere->SetRotation(z*y*x);

//		Icosphere->UpdateWorldTransform(true);


		//Torus01->SetRotation(qRot);
		/*
		Torus01->UpdateMatrix(true);

		Torus02->SetRotation(-qX * qY);
		
		Torus02->SetRotation(qRot);
		Torus02->UpdateWorldTransform(true);

		*/
		frameNum++;

		scene.Draw(prog);

		torusRotX += Dt * 1.0f;
		torusRotX = fmod(torusRotX, 360.0f);
		icoRotY -= Dt * 1.0f;
		icoRotY = fmod(icoRotY, 360.0f);

		f += fd * Dt;
		if (f > 1.0f) {
			fd *= -1.f;
			f = 1.0f;
		};

		if (f < 0.f)
		{
			fd *= -1.f;
			f = 0.0f;
		}
		//gl->FlushCommandBuffers();


		input->Update();


		if (input->IsKeyDown(Key_Escape) || input->IsButtonDown(MB_Right))
		{
			running = false;
		}

		if (input->IsKeyDown(Key_I))
		{
			Rl -= 0.01;
			scene.SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}
		else if (input->IsKeyDown(Key_O))
		{
			Rl += 0.01;
			scene.SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}

		Vector2l mPos = input->GetRelativeMousePosition();

		if (input->IsWheelDownMoved())
		{
			vZ -= 2.5f * Dt;
		}
		else if (input->IsWheelUpMoved())
		{
			vZ += 2.5f * Dt;
		}

		vX += mPos.x / 100.0f * Dt;
		vY += mPos.y / 100.0f * Dt;

		if (fTime - lastf > 1)
		{
			lastf = fTime;
		
			//timer.PrintElapsedTime("frame time");
		}

	}

	gl->UseShader(nullptr);

	delete input;
	delete prog;
	delete s_vtx;
	delete s_frg;
	delete gl;
	return 0;
}
