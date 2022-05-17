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
#include "scene/Node3d.hpp"
#include "scene/Scene.hpp"
#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"

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

Quat createQuaternion(const float rX, const float rY, const float rZ)
{
	Quat qx = glm::angleAxis(glm::radians(rX), vec3(1.f, 0.f, 0.f));
	Quat qy = glm::angleAxis(glm::radians(rY), vec3(0.f, 1.f, 0.f));
	Quat qz = glm::angleAxis(glm::radians(rZ), vec3(0.f, 0.f, 1.f));

	return qz * qy * qx;
}

std::vector<Keyframe> keyframes = {
	{
		{-7.0f, -5.0f, 0.0f},
		createQuaternion(0.f, 0.f, 0.f),
		0.0f
	},
	{
		{-1.0f, 0.0f, -0.3f},
		createQuaternion(0.f, 180.f, 0.f),
		10.0f
	},
	{
		{1.0f, 0.5f, -0.6f},
		createQuaternion(0.f, 180.f, 180.f),
		20.0f
	},
	{
		{2.0f, 6.0f, -2.0f},
		createQuaternion(90.f, 180.f, 180.f),
		30.0f
	}
};

Animation anim("Anim1");

AnimationTrack* track1 = anim.CreateTrack("track1");

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

	Scene scene("Scene1", &sm, gl, &fs);

	//scene.LoadScene(fs.Resolve("test2.dae"), 0);
	scene.LoadScene(fs.Resolve("test2.gltf"));
	scene.Compile();


	GpuShader*		prog	= gl->CreateGpuShader();
	GpuShaderStage* s_vtx	= gl->CreateGpuShaderStage(ShaderStage_Vertex, "simple_vtx.glsl");
	GpuShaderStage* s_frg	= gl->CreateGpuShaderStage(ShaderStage_Fragment, "simple_frag.glsl");


	n = gl->GetCaps(GraphicsCaps_MaxTextureImageUnits);
	Info("Max texture units: %d", n);

	n = gl->GetCaps(GraphicCaps_AutoGenerateMipMaps);
	Info("AutoGenerateMipMaps: %d", n);

	gl->SetClearColor(clearColor);
	gl->SetVSyncEnabled(1,false);
    gl->SetCullFaceEnable(true);

	bool runOnce = false;

	float rotAngle = 0;

	uint64_t frametime = SDL_GetTicks64();

	
	//float lPower = 100.f;
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

	const LightVec& lights = scene.GetLights();

	int ln = 1;
	std::for_each(lights.begin(), lights.end(), [&](const Light* it) {

		const PointLight* light = reinterpret_cast<const PointLight*>(it);
		const Vector3f color = light->GetDiffuse();

		X_add_light_cube(scene, light->GetPosition(), glm::normalize(color), ln++);
	});

	scene.Compile();

	float f = 0.f;
		
	std::for_each(keyframes.begin(), keyframes.end(), [&](const Keyframe& k) {
		Keyframe* x = track1->CreateKeyframe(k.time);
		x->position = k.position;
		x->rotation = k.rotation;

		f = k.time;
	});

	anim.SetLength(f);
	f = 0.0f;


//	double clock = SimpleTimer::GetTime(true);
	float clock = float(SDL_GetTicks64()) / 1000.f;
	float lastf = clock;

	const Matrix mtx = Icosphere->GetWorldTransform();
	Info("Icosphere model pos: (%.2f, %.2f, %.2f)", mtx[3].x, mtx[3].y, mtx[3].z);

	while (running && !runOnce)
	{
		float now = double(SDL_GetTicks64()) / 1000.f;
		float DDT = now - clock;

		clock = now;

		vec3 viewPos(vX, vY, vZ);

		scene.SetCameraPos(viewPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

		//track1->ApplyOnNode(Icosphere, f, 0.5f);

		if (Icosphere->HasAnimation())
		{
			Animation* an = Icosphere->GetAnimation();
			Icosphere->SetPosition(Vector3f(0.0f));
			Icosphere->SetRotation(Quat(1,0,0,0));

			for (int t = 0; t < an->GetTrackNum(); t++)
			{
				AnimationTrack* track = an->GetTrack(t);
				track->ApplyOnNode(Icosphere, f, 1.0f);
			}

			f += 1000.f * DDT;

			if (f > an->GetLength()) {
				f -= an->GetLength();
			}

		}


		scene.Draw(prog);

		//gl->FlushCommandBuffers();


		input->Update();


		if (input->IsKeyDown(Key_Escape) || input->IsButtonDown(MB_Right))
		{
			running = false;
		}

		if (input->IsKeyDown(Key_I))
		{
			Rl -= 0.2 * DDT;
			scene.SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}
		else if (input->IsKeyDown(Key_O))
		{
			Rl += 0.2 * DDT;
			scene.SetDefaultLightRadius(Rl);
			Info("R.light: %.2f", Rl);
		}

		Vector2l mPos = input->GetRelativeMousePosition();

		if (input->IsWheelDownMoved())
		{
			vZ -= 10.f * DDT;
		}
		else if (input->IsWheelUpMoved())
		{
			vZ += 10.f * DDT;
		}

		vX += mPos.x * DDT;
		vY += mPos.y * DDT;

		if (now - lastf > 1)
		{
			lastf = now;
			Info("f = %.4f, DDT = %.4f", f, DDT);
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
