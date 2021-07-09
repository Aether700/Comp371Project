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
	glm::vec3 texCoord;
	glm::vec4 color;
	float textureIndex;
	float tillingFactor;
};

struct Renderer3DData
{
	static const unsigned int maxQuads = 20000;//10000;
	static const unsigned int maxVertices = maxQuads * 8;
	static const unsigned int maxIndices = maxQuads * 6 * 6; //6 faces with 6 indices each
	static const unsigned int maxTextureSlots = 32; 

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

	Renderer3DStatistics stats;
};

static Renderer3DData s_data = Renderer3DData();



void Renderer3D::Init()
{
	s_data.quadVertexArray = std::make_shared<OpenGLVertexArray>();

	s_data.quadVertexBuffer = std::make_shared<OpenGLVertexArray>(s_data.maxVertices * sizeof(VertexData));
	s_data.quadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float3, "a_texCoord" },
		{ ShaderDataType::Float4, "a_color" },
		{ ShaderDataType::Float, "a_texIndex" },
		{ ShaderDataType::Float, "a_tillingFactor" }
		});
	s_data.quadVertexArray->AddVertexBuffer(s_data.quadVertexBuffer);

	s_data.quadVertexBufferBase = new VertexData[s_data.maxVertices];


	//the index data used by the index buffer to reduce the number of vertices used
	unsigned int* quadIndices = new unsigned int[s_data.maxIndices];

	need to fix offsets and such
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

	s_data.shader2D = std::make_shared<OpenGLShader>("Resources/Shaders/Shader3D.glsl");
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

void Renderer3D::DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture, 
	float tileFactor, const glm::vec4& tintColor)
{
	CheckBatchCapacity();

	const glm::vec3 texCoord[] = {
		{  1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },
		{  0.0f,  1.0f,  0.0f },
		{  0.0f, -1.0f,  0.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f, -1.0f },
	};

	float textureIndex = GetTextureIndex(texture);

	for (size_t i = 0; i < 8; i++)
	{
		s_data.quadVertexBufferPtr->position = transform * s_data.voxelVertexPositions[i];
		s_data.quadVertexBufferPtr->color = tintColor;
		s_data.quadVertexBufferPtr->texCoord = texCoord[i ];
		s_data.quadVertexBufferPtr->textureIndex = textureIndex;
		s_data.quadVertexBufferPtr->tillingFactor = tileFactor;
		s_data.quadVertexBufferPtr++;
	}

	s_data.quadIndexCount += 6 * 6;

	s_data.stats.numIndices += 6 * 6;
	s_data.stats.numVertices += 8;
}

void Renderer3D::FlushAndReset()
{
	EndScene();

	s_data.quadIndexCount = 0;
	s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	s_data.textureSlotIndex = 1;
}

const Renderer3DStatistics& Renderer3D::GetStats()
{
	return s_data.stats;
}

void Renderer3D::ResetStats()
{
	s_data.stats.Reset();
}

void Renderer3D::UploadVoxel(const glm::vec3& position, const glm::vec3 rotation, const glm::vec3& size, float textureIndex,
	float tileFactor, const glm::vec4& tintColor, const glm::vec3* textureCoords)
{
	//position and normal data of a basic cube
	float positions[] = {
	};

	float cubeVertices[] = {
		     // positions          // normals

		//back face 
		//index order 0, 1, 2, 2, 3, 0
		-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
		 //0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f, //duplicated
		-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
		//-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f, //duplicated

		//front face
		-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	 0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f, 1.0f,
		 //0.5f,  0.5f,  0.5f,	 0.0f,  0.0f, 1.0f, //duplicated
		-0.5f,  0.5f,  0.5f,	 0.0f,  0.0f, 1.0f,
		//-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f, 1.0f, //duplicated

		//left face
		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
		//-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f, //duplicated
		-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
		//-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f, //duplicated

		//right face
		 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,
		 //0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f, //duplicated
		 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,
		 //0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f, //duplicated

		 //down face
		-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,
		 //0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f, //duplicated
		-0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,
		//-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f, //duplicated

		//up face
		-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,
		 //0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f, //duplicated
		 0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,
		//-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f //duplicated
	};


}