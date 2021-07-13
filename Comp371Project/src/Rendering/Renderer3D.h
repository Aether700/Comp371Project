#pragma once
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLBuffer.h"

#include <unordered_map>
#include <memory>

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
public:
	RenderingBatch() 
	{
		m_vertexDataArr = new VertexData[s_maxVertices];
		m_indicesArr = new unsigned int[s_maxIndices];
		m_textureSlots = new std::shared_ptr<OpenGLTexture>[s_maxTextureSlots];

		m_vao = std::make_shared<OpenGLVertexArray>();

		m_vbo = std::make_shared<OpenGLVertexBuffer>(sizeof(VertexData) * s_maxVertices);
		
		m_vbo->SetLayout({
			{ ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float3, "a_texCoord" },
			{ ShaderDataType::Float4, "a_color" },
			{ ShaderDataType::Float, "a_texIndex" },
			{ ShaderDataType::Float, "a_tillingFactor" }
		});

		m_vao->AddVertexBuffer(m_vbo);

		m_ibo = std::make_shared<OpenGLIndexBuffer>();
	}

	~RenderingBatch()
	{
		delete[] m_vertexDataArr;
		delete[] m_indicesArr;
		delete[] m_textureSlots;
	}

	void Add(VertexData* vertices, unsigned int numVertices, unsigned int* indices, 
		unsigned int numIndices, unsigned int renderTarget)
	{
		CheckCapacity(vertices, numVertices, indices, numIndices, renderTarget);

		for (unsigned int i = 0; i < numVertices; i++)
		{
			m_vertexDataArr[m_vertexDataIndex + i] = vertices[i];
		}
		m_vertexDataIndex += numVertices;

		for (unsigned int i = 0; i < numIndices; i++)
		{
			m_indicesArr[m_indexOffset + i] = m_indexOffset + indices[i];
		}
		m_indexOffset += numIndices;
	}

	//returns texture index for the texture
	int AddTexture(std::shared_ptr<OpenGLTexture> texture, unsigned int renderTarget)
	{
		int textureIndex = 0;

		for (int i = 1; i < m_textureIndex; i++)
		{
			if (*texture == *m_textureSlots[i])
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			if (m_textureIndex == s_maxTextureSlots)
			{
				Draw(renderTarget);
			}

			m_textureSlots[m_textureIndex] = texture;
			textureIndex = m_textureIndex;
			m_textureIndex++;
		}

		return textureIndex;
	}

	//render target is a gl enum ex: GL_TRIANGLES
	void Draw(unsigned int renderTarget)
	{
		m_vao->Bind();

		m_vbo->SetData(m_vertexDataArr, sizeof(VertexData) * m_vertexDataIndex);

		m_ibo->SetData(m_indicesArr, m_indexOffset);

		for (unsigned int i = 0; i < m_textureIndex; i++)
		{
			m_textureSlots[i]->Bind(i);
		}

		glDrawElements(renderTarget, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		ResetBatch();
	}

	bool IsEmpty() const
	{
		return m_indicesArr == 0 || m_vertexDataIndex == 0;
	}

private:
	void CheckCapacity(VertexData* vertices, unsigned int numVertices, unsigned int* indices,
		unsigned int numIndices, unsigned int renderTarget)
	{
		if (s_maxVertices - m_vertexDataIndex < numVertices 
			|| s_maxIndices - m_indexOffset < numIndices)
		{
			Draw(renderTarget);
		}
	}

	void ResetBatch()
	{
		m_vertexDataIndex = 0;
		m_indexOffset = 0;
		m_textureIndex = 0;
	}

	static unsigned int s_maxVertices;
	static unsigned int s_maxIndices;
	static unsigned int s_maxTextureSlots;


	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;

	VertexData* m_vertexDataArr;
	unsigned int m_vertexDataIndex = 0;

	unsigned int* m_indicesArr;
	unsigned int m_indexOffset = 0;

	std::shared_ptr<OpenGLTexture>* m_textureSlots;
	unsigned int m_textureIndex = 0;
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
		unsigned int renderTraget = GL_TRIANGLES, float tileFactor = 1.0f, 
		const glm::vec4& tintColor = { 1, 1, 1, 1 });

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
	static void UploadVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,	
		float tileFactor, const glm::vec4& tintColor, unsigned int renderTarget);

	static Renderer3DStatistics s_stats;

	static std::unordered_map<unsigned int, RenderingBatch> s_renderingBatches;
};