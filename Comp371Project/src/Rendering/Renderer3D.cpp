#include "Renderer3D.h"
#include "../Core/Debug.h"
#include "../Core/Application.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLTexture2D.h"
#include "CameraController.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Mesh.h"

#include <array>

std::unordered_map<unsigned int, RenderingBatch> Renderer3D::s_renderingBatches;
Renderer3DStatistics Renderer3D::s_stats;

std::shared_ptr<OpenGLCubeMap> Renderer3D::s_defaultWhiteCubeMap;
std::shared_ptr<OpenGLTexture2D> Renderer3D::s_defaultWhiteTexture;
std::shared_ptr<OpenGLShader> Renderer3D::s_shader;
Material Renderer3D::s_defaultMaterial = Material(); //create the default material
bool Renderer3D::s_useShadows = false;
DirectionalLight* Renderer3D::s_directionalLightArr;
unsigned int Renderer3D::s_directionalLightIndex;
PointLight* Renderer3D::s_pointLightArr;
unsigned int Renderer3D::s_pointLightIndex;
bool Renderer3D::s_updateLights;

unsigned int RenderingBatch::s_maxVertices = 16000;
unsigned int RenderingBatch::s_maxIndices = 72000;
unsigned int RenderingBatch::s_maxTextureSlots = 32;
unsigned int RenderingBatch::s_maxTexture2DShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;
unsigned int RenderingBatch::s_maxTexture2DSlots = RenderingBatch::s_maxTextureSlots / 4;
unsigned int RenderingBatch::s_maxCubemapSlots = RenderingBatch::s_maxTextureSlots / 4;
unsigned int RenderingBatch::s_maxCubemapShadowMapSlots = RenderingBatch::s_maxTextureSlots / 4;



//rendering batch///////////////////////////////////////////////////////////////////

RenderingBatch::RenderingBatch()
{
	m_texture2DSlots = new std::shared_ptr<OpenGLTexture2D>[s_maxTexture2DSlots];
	m_texture2DShadowMapSlots = new std::shared_ptr<OpenGLTexture2D>[s_maxTexture2DShadowMapSlots];
	m_cubemapSlots = new std::shared_ptr<OpenGLCubeMap>[s_maxCubemapSlots];
	m_cubemapShadowMapSlots = new std::shared_ptr<OpenGLCubeMap>[s_maxCubemapShadowMapSlots];

	m_vao = std::make_shared<OpenGLVertexArray>();

	m_vbo = std::make_shared<OpenGLVertexBuffer>(sizeof(VertexData) * s_maxVertices);

	m_vbo->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float3, "a_textureCoords" },
		{ ShaderDataType::Float3, "a_normal" },
		{ ShaderDataType::Float4, "a_color" },
		{ ShaderDataType::Float, "a_texIndex" },
		{ ShaderDataType::Float, "a_tillingFactor" },
		{ ShaderDataType::Float, "a_uses3DTexture" },
		{ ShaderDataType::Float, "a_ambiantIntensity" },
		{ ShaderDataType::Float, "a_diffuseIntensity" },
		{ ShaderDataType::Float, "a_specularIntensity" },
		{ ShaderDataType::Float, "a_shininess" },
		{ ShaderDataType::Float, "a_ignoresLighting" }
	});

	m_vao->AddVertexBuffer(m_vbo);

	m_ibo = std::make_shared<OpenGLIndexBuffer>();
}


RenderingBatch::~RenderingBatch()
{
	delete[] m_texture2DSlots;
	delete[] m_cubemapSlots;
}

void RenderingBatch::Add(const VertexData* vertices, unsigned int numVertices, const unsigned int* indices,
	unsigned int numIndices, unsigned int renderTarget)
{
	for (unsigned int i = 0; i < numVertices; i++)
	{
		m_vertexDataArr.push_back(vertices[i]);
	}

	for (unsigned int i = 0; i < numIndices; i++)
	{
		m_indicesArr.push_back(m_vertexDataIndex + indices[i]);
	}
	m_vertexDataIndex += numVertices;
	m_indicesIndex += numIndices;

	Renderer3D::s_stats.numIndices += numIndices;
	Renderer3D::s_stats.numVertices += numVertices;
}

//returns texture index for the texture
int RenderingBatch::AddTexture2D(std::shared_ptr<OpenGLTexture2D> texture, unsigned int renderTarget)
{
	int textureIndex = 0;
	bool found = false;
	for (int i = 0; i < m_texture2DIndex; i++)
	{
		if (*texture == *m_texture2DSlots[i])
		{
			textureIndex = i;
			found = true;
			break;
		}
	}

	if (textureIndex == 0 && !found)
	{
		if (m_texture2DIndex == s_maxTexture2DSlots)
		{
			Draw(renderTarget);
		}

		m_texture2DSlots[m_texture2DIndex] = texture;
		textureIndex = m_texture2DIndex;
		m_texture2DIndex++;
	}

	return textureIndex;
}

//returns texture index for the texture
int RenderingBatch::AddCubemap(std::shared_ptr<OpenGLCubeMap> cubemap, unsigned int renderTarget)
{
	int textureIndex = 0;
	bool found = false;
	for (int i = 0; i < m_cubemapIndex; i++)
	{
		if (*cubemap == *m_cubemapSlots[i])
		{
			textureIndex = i;
			found = true;
			break;
		}
	}

	if (textureIndex == 0 && !found)
	{
		if (m_cubemapIndex == s_maxCubemapSlots)
		{
			Draw(renderTarget);
		}

		m_cubemapSlots[m_cubemapIndex] = cubemap;
		textureIndex = m_cubemapIndex;
		m_cubemapIndex++;
	}

	return textureIndex;
}

void RenderingBatch::AddTexture2DShadowMap(std::shared_ptr<OpenGLTexture2D> shadowMap)
{
	int textureIndex = 0;
	bool found = false;
	for (int i = 0; i < m_texture2DShadowMapIndex; i++)
	{
		if (*shadowMap == *m_texture2DShadowMapSlots[i])
		{
			textureIndex = i;
			found = true;
			break;
		}
	}

	if (textureIndex == 0 && !found)
	{
		if (m_texture2DShadowMapIndex == s_maxTexture2DShadowMapSlots)
		{
			assert(false);
		}

		m_texture2DShadowMapSlots[m_texture2DShadowMapIndex] = shadowMap;
		m_texture2DShadowMapIndex++;
	}
}

void RenderingBatch::AddCubemapShadowMap(std::shared_ptr<OpenGLCubeMap> shadowMap)
{
	int textureIndex = 0;
	bool found = false;
	for (int i = 0; i < m_cubemapShadowMapIndex; i++)
	{
		if (*shadowMap == *m_cubemapShadowMapSlots[i])
		{
			textureIndex = i;
			found = true;
			break;
		}
	}

	if (textureIndex == 0 && !found)
	{
		if (m_cubemapShadowMapIndex == s_maxCubemapShadowMapSlots)
		{
			assert(false);
		}

		m_cubemapShadowMapSlots[m_cubemapShadowMapIndex] = shadowMap;
		m_cubemapShadowMapIndex++;
	}
}

//render target is a gl enum ex: GL_TRIANGLES
void RenderingBatch::Draw(unsigned int renderTarget)
{
	m_vao->Bind();

	m_vbo->SetData(&m_vertexDataArr[0], sizeof(VertexData) * m_vertexDataIndex);

	m_ibo->SetData(&m_indicesArr[0], m_indicesIndex);

	for (unsigned int i = 0; i < m_texture2DIndex; i++)
	{
		m_texture2DSlots[i]->Bind(i);
	}

	unsigned int offset = s_maxTexture2DSlots;

	for (unsigned int i = 0; i < m_cubemapIndex; i++)
	{
		m_cubemapSlots[i]->Bind(i + offset);
	}

	offset += s_maxCubemapSlots;

	for (unsigned int i = 0; i < m_texture2DShadowMapIndex; i++)
	{
		m_texture2DShadowMapSlots[i]->Bind(i + offset);
	}

	offset += s_maxTexture2DShadowMapSlots;

	for (unsigned int i = 0; i < m_cubemapShadowMapIndex; i++)
	{
		m_cubemapShadowMapSlots[i]->Bind(i + offset);
	}
	
	glDrawElements(renderTarget, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

	Renderer3D::s_stats.numDrawCalls++;
}

bool RenderingBatch::IsEmpty() const
{
	return m_indicesIndex == 0 || m_vertexDataIndex == 0;
}

void RenderingBatch::ResetBatch()
{
	m_vertexDataIndex = 0;
	m_indicesIndex = 0;
	m_texture2DIndex = 0;
	m_texture2DShadowMapIndex = 0;
	m_cubemapIndex = 0;
	m_cubemapShadowMapIndex = 0;
	m_vertexDataArr.clear();
	m_indicesArr.clear();
}

//Renderer3D///////////////////////////////////////////////////

void Renderer3D::Init()
{
	unsigned int whiteTextureData = 0xffffffff;
	s_defaultWhiteCubeMap = std::make_shared<OpenGLCubeMap>(1, &whiteTextureData);
	s_defaultWhiteTexture = std::make_shared<OpenGLTexture2D>(1, 1, &whiteTextureData);

	int* samplers = new int[RenderingBatch::s_maxTexture2DSlots];
	for (int i = 0; i < RenderingBatch::s_maxTexture2DSlots; i++)
	{
		samplers[i] = i;
	}

	unsigned int offset = RenderingBatch::s_maxTexture2DSlots;

	int* samplersCubemap = new int[RenderingBatch::s_maxCubemapSlots];
	for (int i = 0; i < RenderingBatch::s_maxCubemapSlots; i++)
	{
		samplersCubemap[i] = i + offset;
	}
	
	offset += RenderingBatch::s_maxCubemapSlots;

	int* samplers2DShadowMap = new int[RenderingBatch::s_maxTexture2DShadowMapSlots];
	for (int i = 0; i < RenderingBatch::s_maxTexture2DShadowMapSlots; i++)
	{
		samplers2DShadowMap[i] = i + offset;
	}

	offset += RenderingBatch::s_maxTexture2DShadowMapSlots;

	int* samplersCubemapShadowMap = new int[RenderingBatch::s_maxCubemapShadowMapSlots];
	for (int i = 0; i < RenderingBatch::s_maxCubemapShadowMapSlots; i++)
	{
		samplersCubemapShadowMap[i] = i + offset;
	}

	s_shader = std::make_shared<OpenGLShader>("Resources/Shaders/Shader3D.glsl");
	s_shader->Bind();
	s_shader->SetIntArray("u_texture", samplers, RenderingBatch::s_maxTexture2DSlots);
	s_shader->SetIntArray("u_cubeMap", samplersCubemap, RenderingBatch::s_maxCubemapSlots);
	s_shader->SetIntArray("u_shadow2D", samplers2DShadowMap, RenderingBatch::s_maxTexture2DShadowMapSlots);
	s_shader->SetIntArray("u_cubeMapShadowMap", samplersCubemapShadowMap, RenderingBatch::s_maxCubemapShadowMapSlots);

	delete[] samplers;
	delete[] samplersCubemap;
	delete[] samplers2DShadowMap;
	delete[] samplersCubemapShadowMap;

	s_directionalLightArr = new DirectionalLight[RenderingBatch::s_maxTexture2DShadowMapSlots];
	s_pointLightArr = new PointLight[RenderingBatch::s_maxCubemapShadowMapSlots];
}

void Renderer3D::Shutdown()
{
	delete[] s_directionalLightArr;
	delete[] s_pointLightArr;
}

void Renderer3D::BeginScene()
{
	std::shared_ptr<Camera> camera = Application::GetCamera();
	glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetTransform();

	s_shader->Bind();
	s_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
	s_shader->SetFloat3("u_camPos", Application::GetCameraController()->GetCamPos());

	s_directionalLightIndex = 0;
	s_pointLightIndex = 0;
}

void Renderer3D::EndScene()
{
	if (s_useShadows && (s_directionalLightIndex != 0 || s_pointLightIndex != 0))
	{
		if (s_updateLights)
		{
			GenerateShadowMaps();
			s_updateLights = false;
		}

		DrawLights();
		s_shader->Bind();
		s_shader->SetInt("u_useShadows", 1);
		s_shader->SetInt("u_numDirLights", s_directionalLightIndex);
		s_shader->SetInt("u_numPointLights", s_pointLightIndex);
		AddShadowMapToShaders();
		FlushBatch();
	}
	else
	{
		s_shader->Bind();
		s_shader->SetInt("u_useShadows", 0);
		FlushBatch();
	}
	
	ResetBatches();
}

//requests that all the light's shadow maps be recalculated for this frame
void Renderer3D::UpdateLights()
{
	s_updateLights = true;
}

std::shared_ptr<OpenGLCubeMap> Renderer3D::GetDefaultWhiteCubeMap() { return s_defaultWhiteCubeMap; }
std::shared_ptr<OpenGLTexture2D> Renderer3D::GetDefaultWhiteTexture() { return s_defaultWhiteTexture; }

void Renderer3D::FlushBatch()
{
	for (auto& pair : s_renderingBatches)
	{
		if (!pair.second.IsEmpty())
		{
			pair.second.Draw(pair.first);
		}
	}
}

void Renderer3D::ResetBatches()
{
	for (auto& pair : s_renderingBatches)
	{
		pair.second.ResetBatch();
	}
}

void Renderer3D::CleanUpAfterShadowMapGeneration()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	int windowWidth, windowHeight;
	glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer3D::AddShadowMapToShaders()
{
	for (int i = 0; i < s_directionalLightIndex; i++)
	{
		auto shadowMap = s_directionalLightArr[i].GetShadowMap();
		for (auto& pair : s_renderingBatches)
		{
			pair.second.AddTexture2DShadowMap(shadowMap);
		}
	}

	for (int i = 0; i < s_pointLightIndex; i++)
	{
		auto shadowMap = s_pointLightArr[i].GetShadowMap();
		for (auto& pair : s_renderingBatches)
		{
			pair.second.AddCubemapShadowMap(shadowMap);
		}
	}
}

void Renderer3D::GenerateShadowMaps()
{
	for (unsigned int i = 0; i < s_directionalLightIndex; i++)
	{
		s_directionalLightArr[i].PrepareForShadowMapGeneration();
		FlushBatch();
	}

	for (unsigned int i = 0; i < s_pointLightIndex; i++)
	{
		s_pointLightArr[i].PrepareForShadowMapGeneration();
		//Cannot call FlushBatch directly, because the point light uses a geometry shader which expects only triangles
		//FlushBatch also draws lines, which causes an error with the geometry shader when glDrawElements is eventually called

		s_renderingBatches[GL_TRIANGLES].Draw(GL_TRIANGLES);
	}

	CleanUpAfterShadowMapGeneration();
}

void Renderer3D::DrawLights()
{
	for (unsigned int i = 0; i < s_directionalLightIndex; i++)
	{
		s_directionalLightArr[i].DrawLightCube();
	}

	for (unsigned int i = 0; i < s_pointLightIndex; i++)
	{
		s_pointLightArr[i].DrawLightCube();
	}
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	UploadVoxel(transform, mat, texture, tileFactor, tintColor);
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	DrawVoxel(transform, s_defaultMaterial, texture, tileFactor, tintColor);
}

void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, std::shared_ptr<OpenGLCubeMap> texture, float tileFactor, const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawVoxel(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor);
}


void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	std::shared_ptr<OpenGLCubeMap> texture, float tileFactor, const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawVoxel(t.GetTransformMatrix(), texture, tileFactor, tintColor);
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	DrawVoxel(transform, mat, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawVoxel(const glm::mat4& transform, const glm::vec4& color)
{
	DrawVoxel(transform, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation,
	const glm::vec3& scale, const Material& mat, const glm::vec4& color)
{
	DrawVoxel(position, rotation, scale, mat, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawVoxel(const glm::vec3& position, const glm::vec3& rotation, 
	const glm::vec3& scale, const glm::vec4& color)
{
	DrawVoxel(position, rotation, scale, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawWireCube(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	UploadWireCube(transform, mat, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawWireCube(const glm::mat4& transform, const glm::vec4& color)
{
	UploadWireCube(transform, s_defaultMaterial, GetDefaultWhiteCubeMap(), 1, color);
}

void Renderer3D::DrawWireCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireCube(t.GetTransformMatrix(), mat, color);
}

void Renderer3D::DrawWireCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireCube(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawPointCube(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	UploadPointCube(transform, mat, GetDefaultWhiteCubeMap(), 1.0f, color);
}

void Renderer3D::DrawPointCube(const glm::mat4& transform, const glm::vec4& color) 
{
	UploadPointCube(transform, s_defaultMaterial, GetDefaultWhiteCubeMap(), 1.0f, color);
}

void Renderer3D::DrawPointCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawPointCube(t.GetTransformMatrix(), mat, color);
}

void Renderer3D::DrawPointCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawPointCube(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLTexture2D> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	UploadQuad(transform, mat, texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, std::shared_ptr<OpenGLTexture2D> texture, 
	float tileFactor, const glm::vec4& tintColor)
{
	UploadQuad(transform, s_defaultMaterial, texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, std::shared_ptr<OpenGLTexture2D> texture, float tileFactor,
	const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawQuad(t.GetTransformMatrix(), mat, texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	std::shared_ptr<OpenGLTexture2D> texture, float tileFactor,
	const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawQuad(t.GetTransformMatrix(), texture, tileFactor, tintColor);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	DrawQuad(transform, mat, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
{
	DrawQuad(transform, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation,
	const glm::vec3& scale, const Material& mat, const glm::vec4& color)
{
	DrawQuad(position, rotation, scale, mat, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, 
	const glm::vec3& scale, const glm::vec4& color)
{
	DrawQuad(position, rotation, scale, GetDefaultWhiteTexture(), 1.0f, color);
}

void Renderer3D::DrawWireSquare(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	UploadWireSquare(transform, mat, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawWireSquare(const glm::mat4& transform, const glm::vec4& color)
{
	UploadWireSquare(transform, s_defaultMaterial, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawWireSquare(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireSquare(t.GetTransformMatrix(), mat, color);
}

void Renderer3D::DrawWireSquare(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawWireSquare(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const Material& mat, const glm::vec4& color)
{
	UploadLine(transform, mat, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const glm::vec4& color)
{
	UploadLine(transform, s_defaultMaterial, GetDefaultWhiteTexture(), 1, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), mat, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const Material& mat, 
	const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color)
{
	constexpr glm::vec3 textureCoords[] = {
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
	};

	glm::vec3 points[] = { point1, point2 };
	unsigned int indices[] = { 0, 1 };

	UploadVertexData(GL_LINES, transform, mat, points, 2, indices, 
		2, GetDefaultWhiteTexture(), textureCoords, 1, color);
}

void Renderer3D::DrawLine(const glm::mat4& transform, const glm::vec3& point1, const glm::vec3& point2,
	const glm::vec4& color)
{
	constexpr glm::vec3 textureCoords[] = {
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
	};

	glm::vec3 points[] = { point1, point2 };
	unsigned int indices[] = { 0, 1 };

	UploadVertexData(GL_LINES, transform, s_defaultMaterial, points, 2, indices, 2, GetDefaultWhiteTexture(), textureCoords, 1, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const Material& mat, const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), mat, point1, point2, color);
}

void Renderer3D::DrawLine(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
	const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color)
{
	Transform t = Transform(position, rotation, scale);
	DrawLine(t.GetTransformMatrix(), point1, point2, color);
}

void Renderer3D::DrawVertexData(unsigned int renderTarget, const glm::mat4& transform, const glm::vec3* vertices,
	unsigned int numVertices, unsigned int* indices, unsigned int indexCount, std::shared_ptr<OpenGLTexture2D> texture,
	const glm::vec3* textureCoords, float tileFactor, const glm::vec4& tintColor)
{
	UploadVertexData(renderTarget, transform, s_defaultMaterial, vertices, numVertices, 
		indices, indexCount, texture, textureCoords, tileFactor, tintColor);
}

void Renderer3D::DrawVertexData(unsigned int renderTarget, const glm::vec3& position, const glm::vec3& rotation,
	const glm::vec3& scale, const glm::vec3* vertices, unsigned int numVertices, unsigned int* indices,
	unsigned int indexCount, std::shared_ptr<OpenGLTexture2D> texture, const glm::vec3* textureCoords, 
	float tileFactor, const glm::vec4& tintColor)
{
	Transform t = Transform(position, rotation, scale);
	DrawVertexData(renderTarget, t.GetTransformMatrix(), vertices, numVertices, 
		indices, indexCount, texture, textureCoords, tileFactor, tintColor);
}

void Renderer3D::DrawMesh(const glm::mat4& transform, std::shared_ptr<Mesh>& mesh, std::shared_ptr<OpenGLTexture2D> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	UploadMesh(transform, s_defaultMaterial, mesh, texture, tileFactor, tintColor);
}

void Renderer3D::AddDirectionalLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec4& lightColor)
{
	assert(s_directionalLightIndex < RenderingBatch::s_maxTexture2DShadowMapSlots);

	DirectionalLight currLight(position, direction, lightColor);

	s_shader->Bind();
	s_shader->SetFloat4("u_dirLightColors[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetColor());
	s_shader->SetFloat3("u_dirLightPos[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetPosition());
	s_shader->SetFloat("u_lightRadius[" + std::to_string(s_directionalLightIndex) + "]", currLight.GetRadius());
	s_shader->SetMat4("u_dirLightSpaceMatrices[" + std::to_string(s_directionalLightIndex) + "]", 
		currLight.GetLightSpaceMatrix());
	
	if (s_directionalLightArr[s_directionalLightIndex] != currLight)
	{
		s_updateLights = true;
		s_directionalLightArr[s_directionalLightIndex] = std::move(currLight);
	}

	s_directionalLightIndex++;
}

void Renderer3D::AddPointLight(const glm::vec3& position, const glm::vec4& lightColor)
{
	assert(s_pointLightIndex < RenderingBatch::s_maxCubemapShadowMapSlots);

	PointLight currLight(position, lightColor);

	s_shader->Bind();
	s_shader->SetFloat4("u_pointLightColors[" + std::to_string(s_pointLightIndex) + "]", currLight.GetColor());
	s_shader->SetFloat3("u_pointLightPos[" + std::to_string(s_pointLightIndex) + "]", currLight.GetPosition());
	s_shader->SetFloat("u_pointLightFarPlanes[" + std::to_string(s_pointLightIndex) + "]", currLight.GetFarPlane());

	if (s_pointLightArr[s_pointLightIndex] != currLight)
	{
		s_updateLights = true;
		s_pointLightArr[s_pointLightIndex] = std::move(currLight);
	}
	s_pointLightIndex++;
}

const Renderer3DStatistics& Renderer3D::GetStats()
{
	return s_stats;
}

void Renderer3D::ResetStats()
{
	s_stats.Reset();
}

void Renderer3D::UploadVoxel(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_TRIANGLES;
	int textureIndex = s_renderingBatches[renderTarget].AddCubemap(texture, renderTarget);

	glm::vec3 posAndNormals[] = {
		//back face
		{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
		{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
		{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
		{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },

		//left face
		{ -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
		{ -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
		{ -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },

		//front face
		{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
		{  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },

		//right face
		{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
		{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
		{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },

		//top face
		{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
		{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
		{  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },
		{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },

		//bottom face
		{ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
		{  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }

		/* actual vertices
		{ -0.5f, -0.5f, -0.5f }, //0
		{  0.5f, -0.5f, -0.5f }, //1
		{ -0.5f,  0.5f, -0.5f }, //2
		{  0.5f,  0.5f, -0.5f }, //3
		{ -0.5f, -0.5f,  0.5f }, //4
		{  0.5f, -0.5f,  0.5f }, //5
		{ -0.5f,  0.5f,  0.5f }, //6
		{  0.5f,  0.5f,  0.5f }, //7
		*/
	};

	constexpr unsigned int numVertices = sizeof(posAndNormals) / (sizeof(glm::vec3) * 2);
	VertexData cubeVertices[numVertices];

	unsigned int index = 0;

	for (int i = 0; i < 2 * numVertices; i += 2)
	{
		cubeVertices[index].position = (glm::vec3)(transform * glm::vec4(posAndNormals[i], 1));
		cubeVertices[index].textureCoords = posAndNormals[i];
		cubeVertices[index].color = tintColor;
		cubeVertices[index].normal = (glm::vec3)(transform * glm::vec4(posAndNormals[i + 1], 0));
		cubeVertices[index].textureIndex = (float)textureIndex;
		cubeVertices[index].tillingFactor = tileFactor;
		cubeVertices[index].uses3DTexture = 1; //set uses3DTexture to true
		cubeVertices[index].mat = mat; 
		index++;
	}

	unsigned int indices[] = {
		//back face
		0, 1, 2,
		2, 3, 0,

		//left face
		4, 5, 6,
		6, 7, 4,

		//front face
		8, 9, 10,
		10, 11, 8,

		//right face
		12, 13, 14,
		14, 15, 12,

		//top face
		16, 17, 18,
		18, 19, 16,

		//bottom face
		20, 21, 22,
		22, 23, 20
	};

	/*
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
		cubeVertices[i].textureIndex = (float)textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
		cubeVertices[i].uses3DTexture = 1; //set uses3DTexture to true
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
	*/

	s_renderingBatches[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices, 
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadWireCube(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddCubemap(texture, renderTarget);

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
		cubeVertices[i].textureCoords = position[i];
		cubeVertices[i].color = tintColor;
		cubeVertices[i].normal = position[i];
		cubeVertices[i].textureIndex = textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
		cubeVertices[i].uses3DTexture = 1;
		cubeVertices[i].mat = mat;
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

void Renderer3D::UploadPointCube(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLCubeMap> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_POINTS;
	int textureIndex = s_renderingBatches[renderTarget].AddCubemap(texture, renderTarget);

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
		cubeVertices[i].textureCoords = position[i];
		cubeVertices[i].color = tintColor;
		cubeVertices[i].normal = (glm::vec3)(transform * glm::vec4(0, 0, 1, 0));
		cubeVertices[i].textureIndex = textureIndex;
		cubeVertices[i].tillingFactor = tileFactor;
		cubeVertices[i].uses3DTexture = 1;
		cubeVertices[i].mat = mat;
	}

	unsigned int indices[] = {
		0, 1, 2, 3, 4, 5, 6, 7
	};

	s_renderingBatches[renderTarget].Add(cubeVertices, sizeof(cubeVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}


void Renderer3D::UploadQuad(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLTexture2D> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_TRIANGLES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture2D(texture, renderTarget);

	constexpr glm::vec3 textureCoords[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	};

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
		quadVertices[i].textureCoords = textureCoords[i];
		quadVertices[i].color = tintColor;
		quadVertices[i].normal = (glm::vec3)(transform * glm::vec4(0, 0, 1, 0));
		quadVertices[i].textureIndex = textureIndex;
		quadVertices[i].tillingFactor = tileFactor;
		quadVertices[i].uses3DTexture = 0;
		quadVertices[i].mat = mat;
	}


	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	s_renderingBatches[renderTarget].Add(quadVertices, sizeof(quadVertices) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadWireSquare(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLTexture2D> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture2D(texture, renderTarget);

	constexpr glm::vec3 textureCoords[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	};

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
		quadVertices[i].textureCoords = textureCoords[i];
		quadVertices[i].color = tintColor;
		quadVertices[i].normal = position[i];
		quadVertices[i].textureIndex = textureIndex;
		quadVertices[i].tillingFactor = tileFactor;
		quadVertices[i].uses3DTexture = 0;
		quadVertices[i].mat = mat;
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

void Renderer3D::UploadLine(const glm::mat4& transform, const Material& mat, std::shared_ptr<OpenGLTexture2D> texture,
	float tileFactor, const glm::vec4& tintColor)
{
	unsigned int renderTarget = GL_LINES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture2D(texture, renderTarget);

	constexpr glm::vec3 textureCoords[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f }
	};

	glm::vec3 position[] = { 
		{  0.5f, 0.0f, 0.0f },
		{ -0.5f, 0.0f, 0.0f }
	};

	VertexData vertexData[2];

	for (int i = 0; i < sizeof(position) / sizeof(glm::vec3); i++)
	{
		vertexData[i].position = (glm::vec3)(transform * glm::vec4(position[i], 1));
		vertexData[i].textureCoords = textureCoords[i];
		vertexData[i].color = tintColor;
		vertexData[i].normal = position[i];
		vertexData[i].textureIndex = textureIndex;
		vertexData[i].tillingFactor = tileFactor;
		vertexData[i].uses3DTexture = 0;
		vertexData[i].mat = mat;
	}

	unsigned int indices[] = { 0, 1 };

	s_renderingBatches[renderTarget].Add(vertexData, sizeof(vertexData) / sizeof(VertexData), indices,
		sizeof(indices) / sizeof(unsigned int), renderTarget);
}

void Renderer3D::UploadVertexData(unsigned int renderTarget, const glm::mat4& transform, const Material& mat, 
	const glm::vec3* vertices, unsigned int numVertices, unsigned int* indices, unsigned int indexCount, 
	std::shared_ptr<OpenGLTexture2D> texture, const glm::vec3* textureCoords, float tileFactor, const glm::vec4& tintColor)
{
	int textureIndex = s_renderingBatches[renderTarget].AddTexture2D(texture, renderTarget);

	VertexData* vertexData = new VertexData[numVertices];

	for (int i = 0; i < numVertices; i++)
	{
		vertexData[i].position = (glm::vec3) (transform * glm::vec4(vertices[i], 1));
		vertexData[i].textureCoords = textureCoords[i];
		vertexData[i].color = tintColor;
		vertexData[i].normal = vertices[i];
		vertexData[i].textureIndex = textureIndex;
		vertexData[i].tillingFactor = tileFactor;
		vertexData[i].uses3DTexture = 0;
		vertexData[i].mat = mat;
	}

	s_renderingBatches[renderTarget].Add(vertexData, numVertices, indices,
		indexCount, renderTarget);

	delete[] vertexData;
}

void Renderer3D::UploadMesh(const glm::mat4& transform, const Material& mat, std::shared_ptr<Mesh>& mesh,
	std::shared_ptr<OpenGLTexture2D> texture, float tileFactor, const glm::vec4& tintColor)
{
	constexpr unsigned int renderTarget = GL_TRIANGLES;
	int textureIndex = s_renderingBatches[renderTarget].AddTexture2D(texture, renderTarget);

	std::vector<VertexData> vertexData;
	vertexData.reserve(mesh->GetPositions().size());

	for (unsigned int i = 0; i < mesh->GetPositions().size(); i++)
	{
		VertexData currVertex;
		currVertex.position = mesh->GetPositions()[i];
		currVertex.textureCoords = glm::vec3(mesh->GetTextureCoords()[i], 0);
		currVertex.color = tintColor;
		currVertex.normal = mesh->GetNormals()[i];
		currVertex.textureIndex = textureIndex;
		currVertex.tillingFactor = tileFactor;
		currVertex.uses3DTexture = 0;
		currVertex.mat = mat;
		vertexData.push_back(currVertex);
	}

	s_renderingBatches[renderTarget].Add(&vertexData[0], vertexData.size(), &mesh->GetIndices()[0],
		mesh->GetIndices().size(), renderTarget);
}
