#pragma once
#include <string>
#include <unordered_map>
#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"

class OpenGLShader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~OpenGLShader();

	void Bind() const;
	void Unbind() const;

	inline const std::string& GetName() const { return m_name; }

	void SetInt(const std::string& name, int value);
	void SetIntArray(const std::string& name, int* values, unsigned int count);
	void SetFloat(const std::string& name, const float value);
	void SetFloat3(const std::string& name, const glm::vec3& value);
	void SetFloat4(const std::string& name, const glm::vec4& value);
	void SetMat4(const std::string& name, const glm::mat4& value);

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformIntArray(const std::string& name, int* values, unsigned int count);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<unsigned int, std::string> PreProcess(const std::string& src);
	void Compile(const std::unordered_map<unsigned int, std::string>& shaderSrc);
	int GetUniformLocation(const std::string& name);

	unsigned int rendererID;
	std::unordered_map<std::string, int> m_uniformLocations;
	std::string m_name;
};

