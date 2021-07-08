#include "Renderer3D.h"
#include "../Core/Application.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"

#include <array>

//struch which contains the data to send to the gpu for each vertex
struct VertexData
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float textureIndex;
	float tillingFactor;
};

struct Renderer3DData
{
	static const unsigned int maxQuads = 20000;//10000;
	static const unsigned int maxVertices = maxQuads * 4;
	static const unsigned int maxIndices = maxQuads * 6;
	static const unsigned int maxTextureSlots = 32; //to do: query the gpu to get capabilities

	std::shared_ptr<OpenGLVertexArray> quadVertexArray;
	std::shared_ptr<OpenGLVertexBuffer> quadVertexBuffer;
	std::shared_ptr<OpenGLShader> shader2D;
	std::shared_ptr<OpenGLTexture2D> whiteTexture;

	unsigned int quadIndexCount = 0;
	VertexData* quadVertexBufferBase = nullptr;
	VertexData* quadVertexBufferPtr = nullptr;

	std::array<std::shared_ptr<OpenGLTexture2D>, maxTextureSlots> textureSlots;
	unsigned int textureSlotIndex = 1; //index 0 is the white texture

	glm::vec4 voxelVertexPositions[8];

	Renderer3D::Statistic stats;
};

static Renderer3DData s_data = Renderer3DData();

void Renderer3D::Init()
{
	s_data.quadVertexArray = std::make_shared<OpenGLVertexArray>();

	s_data.quadVertexBuffer = std::make_shared<OpenGLVertexArray>(s_data.maxVertices * sizeof(VertexData));
	s_data.quadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float4, "a_color" },
		{ ShaderDataType::Float2, "a_texCoord" },
		{ ShaderDataType::Float, "a_texIndex" },
		{ ShaderDataType::Float, "a_tillingFactor" }
		});
	s_data.quadVertexArray->AddVertexBuffer(s_data.quadVertexBuffer);

	s_data.quadVertexBufferBase = new VertexData[s_data.maxVertices];

	unsigned int* quadIndices = new unsigned int[s_data.maxIndices];

	int offset = 0;
	for (int i = 0; i < s_data.maxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	std::shared_ptr<OpenGLIndexBuffer> quadIB = std::make_shared<OpenGLIndexBuffer>(quadIndices, s_data.maxIndices);
	s_data.quadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	s_data.whiteTexture = std::make_shared<OpenGLTexture2D>(1, 1);
	unsigned int whiteTextureData = 0xffffffff;
	s_data.whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));

	int samplers[s_data.maxTextureSlots];
	for (int i = 0; i < s_data.maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	s_data.shader2D = std::make_shared<OpenGLShader>("assets/shaders/Shader2D.glsl");
	s_data.shader2D->Bind();
	s_data.shader2D->SetIntArray("u_texture", samplers, s_data.maxTextureSlots);

	s_data.textureSlots[0] = s_data.whiteTexture;

	s_data.voxelVertexPositions[0] = { -0.5f, -0.5f, 0.5f, 1.0f };
	s_data.voxelVertexPositions[1] = { 0.5f, -0.5f, 0.5f, 1.0f };
	s_data.voxelVertexPositions[2] = { 0.5f,  0.5f, 0.5f, 1.0f };
	s_data.voxelVertexPositions[3] = { -0.5f,  0.5f, 0.5f, 1.0f };
	s_data.voxelVertexPositions[4] = { -0.5f, -0.5f, -0.5f, 1.0f };
	s_data.voxelVertexPositions[5] = { 0.5f, -0.5f, -0.5f, 1.0f };
	s_data.voxelVertexPositions[6] = { 0.5f,  0.5f, -0.5f, 1.0f };
	s_data.voxelVertexPositions[7] = { -0.5f,  0.5f, -0.5f, 1.0f };
}

void Renderer3D::Shutdown()
{
	delete[] s_data.quadVertexBufferBase;
}

void Renderer3D::BeginScene()
{
	std::shared_ptr<Camera> camera = Application::GetCamera();
	glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * glm::inverse(camera->GetTransform().GetTransformMatrix());

	s_data.shader2D->Bind();
	s_data.shader2D->SetMat4("u_viewProjMatrix", viewProjectionMatrix);

	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

	//start at index 1 since index 0 is our default white texture
	s_data.textureSlotIndex = 1;
}

void Renderer3D::EndScene()
{
	unsigned int dataSize = (unsigned char*)s_data.quadVertexBufferPtr - (unsigned char*)s_data.quadVertexBufferBase;
	s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

	Flush();
}

void Renderer3D::Flush()
{
	for (unsigned int i = 0; i < s_data.textureSlotIndex; i++)
	{
		s_data.textureSlots[i]->Bind(i);
	}

	glDrawElements(GL_TRIANGLES, s_data.quadIndexCount, GL_UNSIGNED_INT, nullptr);
	s_data.stats.numDrawCalls++;
}

void Renderer3D::DrawVoxel(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
{

}

void Renderer3D::FlushAndReset()
{
	EndScene();

	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.textureSlotIndex = 1;
}

Renderer3D::Statistic Renderer3D::GetStats()
{
	return s_data.stats;
}