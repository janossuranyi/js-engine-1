#include <chrono>
#include <cmath>
#include <iterator>
#include <list>
#include <iostream>
#include <string>
#include <functional>
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
#include "scene/Node3d.hpp"
#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "scene/scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "system/Heap.hpp"
#include "impl/InputServiceSDL.hpp"
#include "math/Interp.hpp"

#include "cube.hpp"

#define NUM_FRAME_BUF 2

using namespace jse;
using namespace std::chrono;

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 900
#define WINDOW_ASPECT (float(WINDOW_WIDTH)/float(WINDOW_HEIGHT))
const float TicksPerFrame = 1000.f;


void X_add_light_cube(Scene& aScene, Node3d* aNode, const Color3 aColor, int aLightNum)
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
		custMat.specularIntesity = 100.0f;

		custMat.type = MaterialType_Specular;

		m.SetName("LightCube" + std::to_string(aLightNum));
		m.SetMaterial(custMat);
		int idx = aScene.AddMesh(m);

		customNode->AddRenderable(aScene.GetMeshByIndex(idx));
		customNode->AddScale(vec3(.2f));
		customNode->SetVisible(true);
		customNode->UpdateMatrix();

		aNode->AddChildNode(customNode);
	}
}

Quat createQuaternion(const float rX, const float rY, const float rZ)
{
	Quat qx = glm::angleAxis(glm::radians(rX), vec3(1.f, 0.f, 0.f));
	Quat qy = glm::angleAxis(glm::radians(rY), vec3(0.f, 1.f, 0.f));
	Quat qz = glm::angleAxis(glm::radians(rZ), vec3(0.f, 0.f, 1.f));

	return qz * qy * qx;
}


int main(int argc, char** argv)
{
	FileSystem fs;
#ifdef __APPLE__
    fs.SetWorkingDir("/Users/johnny/tmp/js-engine-1/assets");
#else
    fs.SetWorkingDir("d:/src/js-engine-1/assets");
#endif
	GraphicsDriver* gl = new GraphicsDriverOGL();
	
	const Vector2l pos(0);
	const Color clearColor(0.f, 0.f, 0.3f, 1.f);
	int n = 0;

	gl->Init(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 32, /*fullScreen*/0, /*MS*/4, GpuProgramFormat_GLSL, "Hello OpenGL", pos, true);

	ShaderManager sm(gl, &fs);

	if (!sm.Init()) {
        Info("Shader manager init failed !");
		return 255;
	}

	Scene* scene = new Scene("Scene1", &sm, gl, &fs);

	float Rl = .3f;
	scene->SetDefaultLightRadius(Rl);

	scene->LoadScene(fs.Resolve("test2.gltf"));
	//scene->LoadScene(fs.Resolve("test2.gltf"));
	scene->Compile();


	n = gl->GetCaps(GraphicsCaps_MaxTextureImageUnits);
	Info("Max texture units: %d", n);

	n = gl->GetCaps(GraphicCaps_AutoGenerateMipMaps);
	Info("AutoGenerateMipMaps: %d", n);

	gl->SetClearColor(clearColor);
	gl->SetVSyncEnabled(1, true);

	bool runOnce = false;

	float rotAngle = 0;

	uint64_t frametime = SDL_GetTicks64();

	
	//float lPower = 100.f;

	bool running = true;
	InputService* input = new InputServiceSDL();

	float vX = -20.0f, vY = 00.0f, vZ = 0.0f;

	input->SetRelativeMouseMode(true);

	scene->SetPerspectiveCameraLens(glm::radians(45.0f), WINDOW_ASPECT, 0.1f, 100.0f);
	
	Node3d* Icosphere = scene->GetNodeByName("Icosphere");
	Node3d* Torus02 = scene->GetNodeByName("Torus02");
	Node3d* Torus01 = scene->GetNodeByName("Torus01");

	int ln = 1;
	scene->WalkNodeHiearchy([&](Node3d* n) {

		for (auto it : n->GetRenderables())
		{
			if (it->GetType() == RenderableType::Light)
			{
				const PointLight* light = reinterpret_cast<const PointLight*>(it.get());
				const Vector3f color = light->GetDiffuse();

				X_add_light_cube(*scene, n, glm::normalize(color), ln++);
			}
		}
	});

	scene->Compile();

	float f = 0.f;

//	double clock = SimpleTimer::GetTime(true);
	float clock = float(SDL_GetTicks64()) / 1000.f;
	float lastf = clock;

	gl->SetCullFaceEnable(true);
	gl->SetFrontFace(FrontFace_CCW);
    gl->SetsRGBFrameBufferEnabled(true);

	scene->GetCamera().SetSpeed(35);

	float pitch = 0.f, yaw = -90.f;

	bool srgb = true;

	while (running && !runOnce)
	{
		float now = float(SDL_GetTicks64()) / 1000.f;
		float dt = now - clock;

		clock = now;


		scene->UpdateAnimation(dt);
		scene->UpdateCamera();

		scene->Draw();

		gl->SwapBuffers();

		input->Update();


		if (input->IsKeyDown(Key_Escape) || input->IsButtonDown(MB_Right))
		{
			running = false;
		}

		if (input->IsKeyDown(Key_Space))
		{
			srgb = !srgb;
			gl->SetsRGBFrameBufferEnabled(srgb);
		}
		else if (input->IsKeyDown(Key_I))
		{
			//Rl = std::fmaf(-0.2f, dt, Rl);
			Rl = -0.2f * dt + Rl;
			scene->SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}
		else if (input->IsKeyDown(Key_O))
		{
			Rl = 0.2f * dt + Rl;
			scene->SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}
		else if (input->IsKeyDown(Key_W))
		{
			scene->GetCamera().MoveForward(dt);
		}
		else if (input->IsKeyDown(Key_S))
		{
			scene->GetCamera().MoveBackward(dt);
		}
		else if (input->IsKeyDown(Key_A))
		{
			scene->GetCamera().MoveRight(dt);
		}
		else if (input->IsKeyDown(Key_D))
		{
			scene->GetCamera().MoveLeft(dt);
		}


		Vector2l mPos = input->GetRelativeMousePosition();
		scene->GetCamera().SetDirection(yaw, pitch);
		yaw += mPos.x * dt * 15.f;
		pitch -= mPos.y * dt * 15.f;

		yaw = std::fmodf(yaw, 360.f);
		pitch = std::fmodf(pitch, 360.f);

		if (now - lastf > 1)
		{
			lastf = now;
			//Info("DT = %.4f", dt);
			//timer.PrintElapsedTime("frame time");
		}

	}

	gl->UseShader(nullptr);

	delete scene;
	delete input;
	delete gl;
	return 0;
}
