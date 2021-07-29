#include "Renderer3D.h"
#include "../Core/Debug.h"
#include "../Core/Application.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"

#include <array>

std::unordered_map<unsigned int, RenderingBatch> Renderer3D::s_renderingBatches;
Renderer3DStatistics Renderer3D::s_stats;

std::shared_ptr<OpenGLCubeMap> Renderer3D::s_defaultWhiteCubeMap;
std::shared_ptr<OpenGLTexture2D> Renderer3D::s_defaultWhiteTexture;
std::shared_ptr<OpenGLShader> Renderer3D::s_shader;

unsigned int RenderingBatch::s_maxVertices = 16000;
unsigned int RenderingBatch::s_maxIndices = 72000;
unsigned int RenderingBatch::s_maxTextureSlots = 16;


//rendering batch///////////////////////////////////////////////////////////////////

RenderingBatch::RenderingBatch()
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


RenderingBatch::~RenderingBatch()
{
	delete[] m_vertexDataArr;
	delete[] m_indicesArr;
	delete[] m_textureSlots;
}

void RenderingBatch::Add(VertexData* vertices, unsigned int numVertices, unsigned int* indices,
	unsigned int numIndices, unsigned int renderTarget)
{
	CheckCapacity(vertices, numVertices, indices, numIndices, renderTarget);

	for (unsigned int i = 0; i < numVertices; i++)
	{
		VertexData& temp = vertices[i];
		m_vertexDataArr[m_vertexDataIndex + i] = vertices[i];
	}

	for (unsigned int i = 0; i < numIndices; i++)
	{
		m_indicesArr[m_indicesIndex + i] =  m_vertexDataIndex + indices[i];
	}
	m_vertexDataIndex += numVertices;
	m_indicesIndex += numIndices;

	Renderer3D::s_stats.numIndices += numIndices;
	Renderer3D::s_stats.numVertices += numVertices;
}

//returns texture index for the texture
int RenderingBatch::AddTexture(std::shared_ptr<OpenGLTexture> texture, unsigned int renderTarget)
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
void RenderingBatch::Draw(unsigned int renderTarget)
{
	m_vao->Bind();

	m_vbo->SetData(m_vertexDataArr, sizeof(VertexData) * m_vertexDataIndex);

	m_ibo->SetData(m_indicesArr, m_indicesIndex);

	for (unsigned int i = 0; i < m_textureIndex; i++)
	{
		m_textureSlots[i]->Bind(i);
	}

	glDrawElements(renderTarget, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
	Debug::CheckOpenGLError();

	Renderer3D::s_stats.numDrawCalls++;
	ResetBatch();
}

bool RenderingBatch::IsEmpty() const
{
	return m_indicesArr == 0 || m_vertexDataIndex == 0;
}

void RenderingBatch::CheckCapacity(VertexData* vertices, unsigned int numVertices, unsigned int* indices,
	unsigned int numIndices, unsigned int renderTarget)
{
	if (s_maxVertices - m_vertexDataIndex < numVertices
		|| s_maxIndices - m_indicesIndex < numIndices)
	{
		Draw(renderTarget);
	}
}

void RenderingBatch::ResetBatch()
{
	m_vertexDataIndex = 0;
	m_indicesIndex = 0;
	m_textureIndex = 0;
}

//Renderer3D///////////////////////////////////////////////////

void Renderer3D::Init()
{
	unsigned int whiteTextureData = 0xffffffff;
	s_defaultWhiteCubeMap = std::make_shared<OpenGLCubeMap>(1, 1, &whiteTextureData);
	s_defaultWhiteTexture = std::make_shared<OpenGLTexture2D>(1, 1, &whiteTextureData);

	int* samplers = new int[RenderingBatch::s_maxTextureSlots];
	for (int i = 0; i < RenderingBatch::s_maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	s_shader = std::make_shared<OpenGLShader>("/Users/madelainemareschal/Desktop/comp371project/comp371project/Resources/Shaders/Shader3D.glsl");
	s_shader->Bind();
	s_shader->SetIntArray("u_texture", samplers, RenderingBatch::s_maxTextureSlots);

	delete[] samplers;
}

void Renderer3D::Shutdown()
{
	//delete[] s_data.quadVertexBufferBase;
}

void Renderer3D::BeginScene()
{
	std::shared_ptr<Camera> camera = Application::GetCamera();
	glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetTransform();

	s_shader->Bind();
	s_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
}

void Renderer3D::EndScene()
{
	FlushBatch();
}

std::shared_ptr<OpenGLCubeMap> Renderer3D::GetDefaultWhiteCubeMap() { return s_defaultWhiteCubeMap; }
std::shared_ptr<OpenGLTexture2D> Renderer3D::GetDefaultWhiteTexture() { return s_defaultWhiteTexture; }

void Renderer3D::FlushBatch()
{
	s_shader->Bind();
	for (auto& pair : s_renderingBatches)
	{
		if (!pair.second.IsEmpty())
		{
			pair.second.Draw(pair.first);
		}
	}
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	UploadVoxel(transform, texture, tileFactor, tintColor);
}

void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	std::shared_ptr<OpenGLCubeMap> texture, float tileFactor, const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawVoxel(t.GetTransformMatrix(), texture, tileFactor, tintColor);
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, const glm::vec4& color)
{
	DrawVoxel(transform, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, 
	const glm::vec3& scale, const glm::vec4& color)
{
	DrawVoxel(position, rotation, scale, GetDefaultWhiteCubeMap(), 1.0f, color);
}

void Renderer3D::DrawWireCube(const glm::mat4& transform, const glm::vec4& color)
{
	UploadWireCube(transform, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawWireCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireCube(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawPointCube(const glm::mat4& transform, const glm::vec4& color) 
{
	UploadPointCube(transform, GetDefaultWhiteCubeMap(), 1.0f, color);
}

void Renderer3D::DrawPointCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawPointCube(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture, 
	float tileFactor, const glm::vec4& tintColor)
{
	UploadQuad(transform, texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	std::shared_ptr<OpenGLTexture2D> texture, float tileFactor,
	const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawQuad(t.GetTransformMatrix(), texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
{
	DrawQuad(transform, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, 
	const glm::vec3& scale, const glm::vec4& color)
{
	DrawQuad(position, rotation, scale, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawWireSquare(const glm::mat4& transform, const glm::vec4& color)
{
	UploadWireSquare(transform, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawWireSquare(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireSquare(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const glm::vec4& color)
{
	UploadLine(transform, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const glm::vec3& point1, const glm::vec3& point2,
	const glm::vec4& color)
{
	glm::vec3 points[] = { point1, point2 };
	unsigned int indices[] = { 0, 1 };

	UploadVertexData(GL_LINES, transform, points, 2, indices, 2, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), point1, point2, color);
}

void Renderer3D::DrawVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
	unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	UploadVertexData(renderTarget, transform, vertices, numVertices, indices, indexCount, texture, tileFactor, tintColor);
}

void Renderer3D::DrawVertexData(unsigned int renderTarget, const glm::vec3& position, const glm::vec3& rotation,
	const glm::vec3& scale, const glm::vec3* vertices, unsigned int numVertices, unsigned int* indices,
	unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture, float tileFactor, const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawVertexData(renderTarget, t.GetTransformMatrix(), vertices, numVertices, 
		indices, indexCount, texture, tileFactor, tintColor);
}

const Renderer3DStatistics& Renderer3D::GetStats()
{
	return s_stats;
}

void Renderer3D::ResetStats()
{
	s_stats.Reset();
}

void Renderer3D::UploadVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_TRIANGLES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = {
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }
	};

	VertexData cubeVertices[8];
	
	for (int i = 0; i < sizeof(position)/sizeof(glm::vec3); i++)
	{
		cubeVertices[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		cubeVertices[i].color = tintColor;
		cubeVertices[i].normal = position[i];
		cubeVertices[i].textureIndex = textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = {
		//back face
		1, 0, 2,
		2, 3, 1,

		//left face
		0, 4, 6,
		6, 2, 0,

		//front face
		4, 5, 7, 
		7, 6, 4,

		//right face
		5, 1, 3,
		3, 7, 5,

		//top face
		6, 7, 3, 
		3, 2, 6,

		//bottom face
		0, 1, 5,
		5, 4, 0
	};

	s_renderingBatches[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices, 
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadWireCube(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = {
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }
	};

	VertexData cubeVertices[8];

	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		cubeVertices[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		cubeVertices[i].color = tintColor;
		cubeVertices[i].normal = position[i];
		cubeVertices[i].textureIndex = textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = {
		0, 1,
		0, 2,
		1, 3,
		2, 3,

		4, 5,
		4, 6,
		5, 7,
		6, 7,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	s_renderingBatches[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadPointCube(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_POINTS;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = {
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }
	};

	VertexData cubeVertices[8];

	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		cubeVertices[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		cubeVertices[i].color = tintColor;
		cubeVertices[i].normal = position[i];
		cubeVertices[i].textureIndex = textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = {
		0, 1, 2, 3, 4, 5, 6, 7
	};

	s_renderingBatches[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}


void Renderer3D::UploadQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_TRIANGLES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = {
		{ -0.5f, -0.5f,  0.0f },
		{  0.5f, -0.5f,  0.0f },
		{  0.5f,  0.5f,  0.0f },
		{ -0.5f,  0.5f,  0.0f }
	};

	VertexData quadVertices[4];
	
	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		quadVertices[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		quadVertices[i].color = tintColor;
		quadVertices[i].normal = position[i];
		quadVertices[i].textureIndex = textureIndex;
		quadVertices[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	s_renderingBatches[renderTarget].Add(quadVertices, sizeof(quadVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadWireSquare(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = {
		{ -0.5f, -0.5f,  0.0f },
		{  0.5f, -0.5f,  0.0f },
		{  0.5f,  0.5f,  0.0f },
		{ -0.5f,  0.5f,  0.0f }
	};

	VertexData quadVertices[4];

	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		quadVertices[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		quadVertices[i].color = tintColor;
		quadVertices[i].normal = position[i];
		quadVertices[i].textureIndex = textureIndex;
		quadVertices[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = {
		0, 1,
		1, 2, 
		2, 3, 
		3, 0
	};

	s_renderingBatches[renderTarget].Add(quadVertices, sizeof(quadVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadLine(const glm::mat4& transform, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	glm::vec3 position[] = { 
		{  0.5f, 0.0f, 0.0f },
		{ -0.5f, 0.0f, 0.0f }
	};

	VertexData vertexData[2];

	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		vertexData[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		vertexData[i].color = tintColor;
		vertexData[i].normal = position[i];
		vertexData[i].textureIndex = textureIndex;
		vertexData[i].tillingFactor = tileFactor;
	}

	unsigned int indices[] = { 0, 1 };

	s_renderingBatches[renderTarget].Add(vertexData, sizeof(vertexData) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
	unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	int textureIndex = s_renderingBatches[renderTarget].AddTexture(texture, renderTarget);

	VertexData* vertexData = new VertexData[numVertices];

	for (int i = 0; i < numVertices; i++)
	{
		vertexData[i].position = (glm::vec3) (transform * glm::vec4(vertices[i], 1));
		vertexData[i].color = tintColor;
		vertexData[i].normal = vertices[i];
		vertexData[i].textureIndex = textureIndex;
		vertexData[i].tillingFactor = tileFactor;
	}

	s_renderingBatches[renderTarget].Add(vertexData, numVertices, indices,
		indexCount, renderTarget);

	delete[] vertexData;
}
