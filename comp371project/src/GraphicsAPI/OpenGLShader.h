#ifndef _OPENGL_SHADER_H_
#define _OPENGL_SHADER_H_

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class OpenGLShader
{
public:
	//creates a shader object from the file found at the filepath
	OpenGLShader(const std::string& filepath);

	//creates a shader object with the name provided from the vertex and fragment shader source code provided
	OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~OpenGLShader();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetRendererID() const { return m_rendererID; }
	inline const std::string& GetName() const { return m_name; }

	//sets different uniforms with the provided name and values
	void SetInt(const std::string& name, int value);
	void SetIntArray(const std::string& name, int* values, unsigned int count);
	void SetFloat(const std::string& name, const float value);
	void SetFloat3(const std::string& name, const glm::vec3& value);
	void SetFloat4(const std::string& name, const glm::vec4& value);
	void SetMat4(const std::string& name, const glm::mat4& value);


private:
	//helper functions which retrieve the uniform location and sets it to the value provided
	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformIntArray(const std::string& name, int* values, unsigned int count);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	
	//reads the file at the filepath provided and returns it in string form
	std::string ReadFile(const std::string& filepath);

	/*preprocesses the file before compilation. this API allows multiple 
	  shaders to be written in the same file. 
	  
	  To define each shader type in the file, just type "#type vertex\n" at the top of 
	  your vertex shader code and type "#type fragment\n" or "#type pixel\n" at the 
	  top of your fragment/pixel shader code

	  This class expects this kind of format and will fail to process any files 
	  which does not follow this structure (the order of the vertex and fragment 
	  shader code does not matter as long as the right "headers" are written above it)
	*/
	std::unordered_map<unsigned int, std::string> PreProcess(const std::string& src);

	//compiles and links all the shaders into an opengl shader program
	void Compile(const std::unordered_map<unsigned int, std::string>& shaderSrc);

	/*retrieves the uniform location for the uniform name provided 
	  or -1 if the uniform was not found in this shader.

	  if the uniform is not found a warning message will be written to the console indicating so
	*/
	int GetUniformLocation(const std::string& name);

	unsigned int m_rendererID;
	std::unordered_map<std::string, int> m_uniformLocations;
	std::string m_name;
};

#endif
