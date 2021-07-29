#include "OpenGLShader.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <assert.h>
#include <fstream>
#include <array>
#include <vector>

//helper function which returns the opengl macro value of the shader type matching the string provided
static unsigned int ShaderTypeFromString(const std::string& type)
{
	if (type == "vertex")
	{
		return GL_VERTEX_SHADER;
	}
	else if (type == "fragment" || type == "pixel")
	{
		return GL_FRAGMENT_SHADER;
	}

	std::cout << "Unknown shader type\n";
	assert(false);
	return 0;
}

//creates a shader object from the file found at the filepath
OpenGLShader::OpenGLShader(const std::string& filepath)
{
	std::string src = ReadFile(filepath);
	auto shaderSrcs = PreProcess(src);
	Compile(shaderSrcs);


	int lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	int lastDot = filepath.rfind('.');

	int count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_name = filepath.substr(lastSlash, count);
}

//creates a shader object with the name provided from the vertex and fragment shader source code provided
OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	: m_name(name)
{
	std::unordered_map<unsigned int, std::string> shaderSrcs;
	shaderSrcs[GL_VERTEX_SHADER] = vertexSrc;
	shaderSrcs[GL_FRAGMENT_SHADER] = fragmentSrc;

	Compile(shaderSrcs);
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_rendererID);
}

//reads the file at the filepath provided and returns it in string form
std::string OpenGLShader::ReadFile(const std::string& filepath)
{
	std::string result;
	std::ifstream file(filepath, std::ios::in | std::ios::binary);

	if (file)
	{
		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();
	}
	else
	{
		std::cout << "Could not open file '" << filepath << "'\n";
		assert(false);
	}

	return result;
}

/*preprocesses the file before compilation. this API allows multiple
  shaders to be written in the same file.

  To define each shader type in the file, just type "#type vertex\n" at the top of
  your vertex shader code and type "#type fragment\n" or "#type pixel\n" at the
  top of your fragment/pixel shader code

  This class expects this kind of format and will fail to process any files
  which does not follow this structure (the order of the vertex and fragment
  shader code does not matter as long as the right "headers" are written above it)
*/
std::unordered_map<unsigned int, std::string> OpenGLShader::PreProcess(const std::string& src)
{
	std::unordered_map<unsigned int, std::string> shaderSrcs;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t position = src.find(typeToken, 0);

	while (position != std::string::npos)
	{
		size_t eol = src.find_first_of("\r\n", position);
		assert(eol != std::string::npos);

		size_t begin = position + typeTokenLength + 1;

		std::string type = src.substr(begin, eol - begin);
		assert(ShaderTypeFromString(type));

		size_t nextLinePos = src.find_first_not_of("\r\n", eol);
		position = src.find(typeToken, nextLinePos);
		shaderSrcs[ShaderTypeFromString(type)] = src.substr(nextLinePos,
			position - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
	}

	return shaderSrcs;
}

void OpenGLShader::Bind() const
{
	glUseProgram(m_rendererID);
}

void OpenGLShader::Unbind() const
{
	glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string& name, int value)
{
	UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string& name, int* values, unsigned int count)
{
	UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string& name, const float value)
{
	UploadUniformFloat(name, value);
}
void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
	UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
	int location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, unsigned int count)
{
	int location = GetUniformLocation(name);
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
	int location = GetUniformLocation(name);
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
	int location = GetUniformLocation(name);
	glUniform3f(location, value.r, value.g, value.b);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
	int location = GetUniformLocation(name);
	glUniform4f(location, value.r, value.g, value.b, value.a);
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	int location = GetUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	int location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

//compiles and links all the shaders into an opengl shader program
void OpenGLShader::Compile(const std::unordered_map<unsigned int, std::string>& shaderSrcs)
{
	unsigned int program = glCreateProgram();
	assert(shaderSrcs.size() <= 2);
	std::array<unsigned int, 2> glShaderIDs;
	int index = 0;
	for (auto& kv : shaderSrcs)
	{
		GLenum type = kv.first;
		const std::string& source = kv.second;

		GLuint shader = glCreateShader(type);

		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			std::cout << infoLog.data() << "\n";
			assert(false);
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[index] = shader;
		index++;
	}

	m_rendererID = program;

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);

		for (auto id : glShaderIDs)
			glDeleteShader(id);

		std::cout << infoLog.data() << "\n";
		return;
	}

	for (auto id : glShaderIDs)
	{
		glDetachShader(program, id);
	}
}

/*retrieves the uniform location for the uniform name provided
  or -1 if the uniform was not found in this shader.

  if the uniform is not found a warning message will be written to the console indicating so
*/
int OpenGLShader::GetUniformLocation(const std::string& name)
{
	if (m_uniformLocations.find(name) != m_uniformLocations.end())
	{
		return m_uniformLocations[name];
	}

	int location = glGetUniformLocation(m_rendererID, name.c_str());

	if (location == -1)
	{
		std::cout << "Unknown uniform \"" << name << "\" in shader " << m_name << "\n";
	}

	m_uniformLocations[name] = location;
	return location;
}
