#pragma once
#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"

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

	static std::shared_ptr<OpenGLCubeMap> GetDefaultWhiteCubeMap();

	//primitives
	static void DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
		float tileFactor = 1.0f, const glm::vec4& tintColor = { 1, 1, 1, 1 });

private:
	//draw to data passed to the renderer to the screen
	static void FlushBatch();

	//prepares the renderer to start a new batch
	static void StartBatch();

	//checks the current capacity of the batch renderer 
	//and will flush and start a new batch if needed
	static void CheckBatchCapacity();

	/*gets the index of the texture provided in the renderer, if the texture 
	  has not been assigned an index yet it will be assigned an index and this 
	  index will be returned. if there is no more texture slots the renderer 
	  will be flushed and a new batch will be started with an index reserved 
	  for the provided texture
	*/
	static int GetTextureIndex(const std::shared_ptr<OpenGLCubeMap>& texture);

	//helper function which loads a voxel into the data to pass to the gpu when the renderer flushes
	static void UploadVoxel(const glm::mat4& transform, float textureIndex,	float tileFactor, const glm::vec4& tintColor);
};