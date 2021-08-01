#pragma once
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "DirectionalLight.h"

#include <unordered_map>
#include <memory>

struct Material
{
	glm::vec3 ambiant;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

/*struch which contains the data to
  send to the gpu for each vertex

  the order of the fields is the layout to be sent to the gpu
*/
struct VertexData
{
	glm::vec3 position;
	glm::vec3 textureCoords;
	glm::vec3 normal;
	glm::vec4 color;
	float textureIndex;
	float tillingFactor;
	float uses3DTexture;
};

//struct which contains the statistics of the renderer
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

//one batch per render target (triangles, lines, points, etc.)
class RenderingBatch
{
	friend class Renderer3D;
public:
	RenderingBatch();
	~RenderingBatch();

	void Add(VertexData* vertices, unsigned int numVertices, unsigned int* indices,
		unsigned int numIndices, unsigned int renderTarget);

	//returns texture index for the texture
	int AddTexture2D(std::shared_ptr<OpenGLTexture2D> texture, unsigned int renderTarget);

	//returns texture index for the texture
	int AddCubemap(std::shared_ptr<OpenGLCubeMap> cubemap, unsigned int renderTarget);

	void AddTexture2DShadowMap(std::shared_ptr<OpenGLTexture2D> shadowMap);

	void AddCubemapShadowMap(std::shared_ptr<OpenGLCubeMap> shadowMap);


	//render target is a gl enum ex: GL_TRIANGLES
	void Draw(unsigned int renderTarget);

	bool IsEmpty() const;

	void ResetBatch();
private:

	static unsigned int s_maxVertices;
	static unsigned int s_maxIndices;
	static unsigned int s_maxTextureSlots;
	static unsigned int s_maxTexture2DSlots;
	static unsigned int s_maxTexture2DShadowMapSlots;
	static unsigned int s_maxCubemapSlots;
	static unsigned int s_maxCubemapShadowMapSlots;


	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;

	std::vector<VertexData> m_vertexDataArr;
	unsigned int m_vertexDataIndex = 0;

	std::vector<unsigned int> m_indicesArr;
	unsigned int m_indicesIndex = 0;

	std::shared_ptr<OpenGLTexture2D>* m_texture2DSlots;
	std::shared_ptr<OpenGLTexture2D>* m_texture2DShadowMapSlots;
	std::shared_ptr<OpenGLCubeMap>* m_cubemapSlots;
	std::shared_ptr<OpenGLCubeMap>* m_cubemapShadowMapSlots;
	unsigned int m_texture2DIndex = 0;
	unsigned int m_cubemapIndex = 0;
	unsigned int m_texture2DShadowMapIndex = 0;
	unsigned int m_cubemapShadowMapIndex = 0;
};

//batch renderer for 3D
class Renderer3D
{
	friend class RenderingBatch;
public:
	static void Init();
	static void Shutdown();

	static const Renderer3DStatistics& GetStats();
	static void ResetStats();

	//use to start renderering and stop rendering
	static void BeginScene();
	static void EndScene();

	static void UseShadows(bool value) { s_useShadows = value; }

	static std::shared_ptr<OpenGLCubeMap> GetDefaultWhiteCubeMap();
	static std::shared_ptr<OpenGLTexture2D> GetDefaultWhiteTexture();

	//primitives

	//voxels/cubes
	static void DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture, float tileFactor = 1.0f, 
		const glm::vec4& tintColor = { 1, 1, 1, 1 });

	static void DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, 
		std::shared_ptr<OpenGLCubeMap> texture, float tileFactor = 1.0f,
		const glm::vec4& tintColor = { 1, 1, 1, 1 });

	static void DrawVoxel(const glm::mat4& transform, 
		const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, 
		const glm::vec4& tintColor = { 1, 1, 1, 1 });

	//wire cube
	static void DrawWireCube(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawWireCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const glm::vec4& color = { 1, 1, 1, 1 });

	//point cube
	static void DrawPointCube(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawPointCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const glm::vec4& color = { 1, 1, 1, 1 });

	//squares
	static void DrawQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture,
		float tileFactor = 1.0f, const glm::vec4& tintColor = { 1, 1, 1, 1 });

	static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		std::shared_ptr<OpenGLTexture2D> texture, float tileFactor = 1.0f,
		const glm::vec4& tintColor = { 1, 1, 1, 1 });

	static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, 
		const glm::vec4& color = { 1, 1, 1, 1 });

	//wired squares
	static void DrawWireSquare(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawWireSquare(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const glm::vec4& color = { 1, 1, 1, 1 });

	//Lines
	static void DrawLine(const glm::mat4& transform, const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
		const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawLine(const glm::mat4& transform, const glm::vec3& point1, const glm::vec3& point2, 
		const glm::vec4& color = { 1, 1, 1, 1 });

	static void DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, 
		const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color = { 1, 1, 1, 1 });

	//draw generic Vertex Data
	static void DrawVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
		unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture2D> texture,
		const glm::vec3* textureCoords, float tileFactor, const glm::vec4& tintColor);

	static void DrawVertexData(unsigned int renderTarget, const glm::vec3& position, const glm::vec3& rotation, 
		const glm::vec3& scale, const glm::vec3* vertices, unsigned int numVertices, unsigned int* indices, 
		unsigned int indexCount, std::shared_ptr<OpenGLTexture2D> texture, const glm::vec3* textureCoords, 
		float tileFactor, const glm::vec4& tintColor);

	//lighting functions
	static void AddDirectionalLight(const glm::vec3& position, const glm::vec3& direction, 
		const glm::vec4& lightColor = { 1, 1, 1, 1 });


private:
	//draw to data passed to the renderer to the screen
	static void FlushBatch();

	static void ResetBatches();

	static void CleanUpAfterShadowMapGeneration();

	static void AddShadowMapToShaders(const DirectionalLight& light);

	static void GenerateShadowMaps();

	//helper function which loads a voxel into the data to pass to the gpu when the renderer flushes
	static void UploadVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,	
		float tileFactor, const glm::vec4& tintColor);

	static void UploadWireCube(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadPointCube(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
		float tileFactor, const glm::vec4& tintColor);

	//uploads a quad or filled in square into the renderer
	static void UploadQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadWireSquare(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadLine(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture,
		float tileFactor, const glm::vec4& tintColor);

	//helper function which allows to pass any vertex data with a 2D texture
	static void UploadVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
		unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture2D> texture, 
		const glm::vec3* textureCoords, float tileFactor, const glm::vec4& tintColor);

	static Renderer3DStatistics s_stats;

	static std::shared_ptr<OpenGLCubeMap> s_defaultWhiteCubeMap;
	static std::shared_ptr<OpenGLTexture2D> s_defaultWhiteTexture;
	static std::shared_ptr<OpenGLShader> s_shader;

	static std::unordered_map<unsigned int, RenderingBatch> s_renderingBatches;
	static bool s_useShadows;

	static DirectionalLight* s_directionalLightArr;
	static unsigned int s_directionalLightIndex;

	//temp
	static std::shared_ptr<DirectionalLight> s_light;
};