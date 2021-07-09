#pragma once
#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../GraphicsAPI/OpenGLCubeMap.h"

#include <memory>


struct Renderer3DStatistics
{
	unsigned int numDrawCalls = 0;
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	void Reset()
	{
		numDrawCalls = 0;
		numVertices = 0;
		numIndices = 0;
	}
};

//batch renderer for 3D
class Renderer3D
{
public:
	static void Init();
	static void Shutdown();

	static const Renderer3DStatistics& GetStats();
	static void ResetStats();

	//use to start renderering and stop rendering
	static void BeginScene();
	static void EndScene();

	//draw to data passed to the renderer to the screen
	static void Flush();

	//primitives
	static void DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
		float tileFactor = 1.0f, const glm::vec4& tintColor = { 1, 1, 1, 1 });

private:
	static void FlushAndReset();

	//helper function which loads a voxel into the data to pass to the gpu when the renderer flushes
	static void UploadVoxel(const glm::vec3& position, const glm::vec3 rotation, const glm::vec3& size, float textureIndex,
		float tileFactor, const glm::vec4& tintColor, const glm::vec3* textureCoords);
};