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

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_ASPECT (float(WINDOW_WIDTH)/float(WINDOW_HEIGHT))

/*
   Tension: 1 is high, 0 normal, -1 is low
   Bias: 0 is even,
		 positive is towards first segment,
		 negative towards the other
*/
vec3 HermiteInterpolate(
	const vec3& y0, const vec3& y1,
	const vec3& y2, const vec3& y3,
	const float mu,
	const float tension,
	const float bias)
{
	float mu2, mu3, a0, a1, a2, a3;
	vec3 m1, m0;

	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1 - y0) * (1.0f + bias) * (1.0f - tension) / 2.f;
	m0 += (y2 - y1) * (1.0f - bias) * (1.0f - tension) / 2.f;
	m1 = (y2 - y1) * (1.0f + bias) * (1.0f - tension) / 2.f;
	m1 += (y3 - y2) * (1.0f - bias) * (1.0f - tension) / 2.f;
	a0 = 2.0f * mu3 - 3.0f * mu2 + 1.0f;
	a1 = mu3 - 2.0f * mu2 + mu;
	a2 = mu3 - mu2;
	a3 = -2.0f * mu3 + 3.0f * mu2;

	return(a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
}

inline vec3 lerp(const vec3& p1, const vec3& p2, const float f)
{
	return (1.f - f) * p1 + f * p2;
}

inline vec3 cerp(const vec3& p1, const vec3& p2, const float f)
{
	const float f2 = (1 - cosf(f * M_PI)) / 2;
	return lerp(p1, p2, f2);
}


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

int main(int argc, char** argv)
{

	FileSystem fs;

    fs.SetWorkingDir("d:/src/js-engine-1/assets");

	GraphicsDriver* gl = new GraphicsDriverOGL();
	
	const Vector2l pos(0);
	const Color clearColor(0.f, 0.f, 0.3f, 1.f);
	int n = 0;

	gl->Init(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 32, 1, 0, GpuProgramFormat_GLSL, "Hello OpenGL", pos, true);

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

	int f = 0;
	int kf = 1;
	const int keyframeNum = 512;

	vec3 path[] = {
		{-14.0f, -10.0f, 0.0f},
		{-7.0f, -5.0f, 0.0f},
		{-1.0f, 0.0f, -0.3f},
		{1.0f, 0.5f, -0.6f},
		{2.0f, 6.0f, -2.0f},
		{4.0f, 12.0f, -4.0f}
	};

	Quat orients[] = {
		createQuaternion(0.f, 0.f, 0.f),
		createQuaternion(0.f, 180.f, 0.f),
		createQuaternion(0.f, 180.f, 180.f),
		createQuaternion(90.f, 180.f, 180.f)
	};

	const int pathLen = (sizeof(path) / sizeof(vec3));

	while (running && !runOnce)
	{
//		SimpleTimer timer;

		float fTime = SDL_GetTicks64() / 1000.0;
		const float kT = f / float(keyframeNum);
		
		float Dt = (SDL_GetTicks64() - frametime) / 60.0f;
		frametime = SDL_GetTicks64();


		vec3 viewPos(vX, vY, vZ);

		scene.SetCameraPos(viewPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

		vec3 pt = HermiteInterpolate(path[kf - 1], path[kf], path[kf + 1], path[kf + 2], kT, -0.3f, 0.4f);
		Quat r = glm::mix(orients[kf - 1], orients[kf], kT);
		Icosphere->SetPosition(pt);
		Icosphere->SetRotation(r);

//		Icosphere->UpdateWorldTransform(true);


		//Torus01->SetRotation(qRot);
		/*
		Torus01->UpdateMatrix(true);

		Torus02->SetRotation(-qX * qY);
		
		Torus02->SetRotation(qRot);
		Torus02->UpdateWorldTransform(true);

		*/

		f += 1;

		if (f > keyframeNum) {
			f = f - keyframeNum;
			kf++;
			if (kf > pathLen - 3)
				kf = 1;
		}

		scene.Draw(prog);

		torusRotX += Dt * 1.0f;
		torusRotX = fmod(torusRotX, 360.0f);
		icoRotY -= Dt * 1.0f;
		icoRotY = fmod(icoRotY, 360.0f);

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
