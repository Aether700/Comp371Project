#pragma once
#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"

//batch renderer for 3D
class Renderer3D
{
public:
	static void Init();
	static void Shutdown();

	//use to start renderering and stop rendering
	static void BeginScene();
	static void EndScene();

	//draw to data passed to the renderer to the screen
	static void Flush();

	//primitives
	static void DrawVoxel(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color);

	//stats of the batch renderer
	struct Statistic
	{
		unsigned int numDrawCalls = 0;
		unsigned int voxelCount = 0;

		unsigned int GetTotalVertexCount() { return voxelCount * 8; }
		unsigned int GetTotalIndexCount() { return voxelCount * 6 * 6; }
	};

	Statistic GetStats();

private:
	static void FlushAndReset();
};