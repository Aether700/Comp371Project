#ifndef _RENDERER_3D_H_
#define _RENDERER_3D_H_

#include <GL/glew.h>
#include "Camera.h"
#include <glm/glm.hpp>
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLShader.h"


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
	glm::vec3 normal;
	glm::vec4 color;
	float textureIndex;
	float tillingFactor;
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
	int AddTexture(std::shared_ptr<OpenGLTexture> texture, unsigned int renderTarget);

	//render target is a gl enum ex: GL_TRIANGLES
	void Draw(unsigned int renderTarget);

	bool IsEmpty() const;

private:
	void CheckCapacity(VertexData* vertices, unsigned int numVertices, unsigned int* indices,
		unsigned int numIndices, unsigned int renderTarget);

	void ResetBatch();

	static unsigned int s_maxVertices;
	static unsigned int s_maxIndices;
	static unsigned int s_maxTextureSlots;


	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;

	VertexData* m_vertexDataArr;
	unsigned int m_vertexDataIndex = 0;

	unsigned int* m_indicesArr;
	unsigned int m_indicesIndex = 0;

	std::shared_ptr<OpenGLTexture>* m_textureSlots;
	unsigned int m_textureIndex = 0;
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
		unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void DrawVertexData(unsigned int renderTarget, const glm::vec3& position, const glm::vec3& rotation, 
		const glm::vec3& scale, const glm::vec3* vertices, unsigned int numVertices, unsigned int* indices, 
		unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture, float tileFactor, const glm::vec4& tintColor);

private:
	//draw to data passed to the renderer to the screen
	static void FlushBatch();

	//checks the current capacity of the batch renderer 
	//and will flush and start a new batch if needed
	static void CheckBatchCapacity();

	//helper function which loads a voxel into the data to pass to the gpu when the renderer flushes
	static void UploadVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,	
		float tileFactor, const glm::vec4& tintColor);

	static void UploadWireCube(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadPointCube(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	//uploads a quad or filled in square into the renderer
	static void UploadQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadWireSquare(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	static void UploadLine(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
		float tileFactor, const glm::vec4& tintColor);

	//helper function which allows to pass any
	static void UploadVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
		unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture, 
		float tileFactor, const glm::vec4& tintColor);

	static Renderer3DStatistics s_stats;

	static std::shared_ptr<OpenGLCubeMap> s_defaultWhiteCubeMap;
	static std::shared_ptr<OpenGLTexture2D> s_defaultWhiteTexture;
	static std::shared_ptr<OpenGLShader> s_shader;

	static std::unordered_map<unsigned int, RenderingBatch> s_renderingBatches;
};


#endif
