#pragma once

#include "../Dependencies/glm-0.9.9.8/glm/glm.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <iostream>

/* represents the mesh obtained when loading a model from a file
*/
class Mesh
{
public:
	Mesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& textureCoords, 
		const std::vector<glm::vec3>& normals, const std::vector<unsigned int>& indices) 
		: m_positions(positions), m_normals(normals), m_indices(indices)
	{
		m_textureCoords.reserve(textureCoords.size());

		for (const glm::vec2& coords : textureCoords)
		{
			m_textureCoords.push_back(glm::vec3(coords, 0));
		}
	}

	const std::vector<glm::vec3>& GetPositions() const { return m_positions; }
	const std::vector<glm::vec2>& GetTextureCoords() const { return m_textureCoords; }
	const std::vector<glm::vec3>& GetNormals() const { return m_normals; }
	const std::vector<unsigned int>& GetIndices() const { return m_indices; }

	//loads a model from a file and creates a Mesh object. returns nullptr if an error occurs
	static std::shared_ptr<Mesh> LoadFromFile(const std::string& filepath)
	{
		std::string extension = GetFileExtension(filepath);

		if (extension == "obj")
		{
			return LoadFromOBJ(filepath);
		}
		
		std::cout << "file format of \"" << filepath << "\" not supported\n";
		return nullptr;
	}

private:
	static std::string GetFileExtension(const std::string& filepath)
	{
		int lastDot = filepath.rfind('.');

		//no extension
		if (lastDot == std::string::npos)
		{
			return "";
		}

		return filepath.substr(lastDot + 1, filepath.size() - lastDot + 1);
	}

	static std::shared_ptr<Mesh> LoadFromOBJ(const std::string& filepath)
	{
		std::ifstream file(filepath);

		if (!file)
		{
			std::cout << "could not load file \"" << filepath << "\"\n";
			return nullptr;
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> textureCoords;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> posIndices;
		std::vector<unsigned int> texCoordIndices;
		std::vector<unsigned int> normalIndices;

		while (true)
		{
			if (file.eof())
			{
				break;
			}

			char buffer[200];
			file.getline(buffer, 200);//read line


			if (buffer[0] == 'v' && buffer[1] == ' ') //defining a postion
			{
				glm::vec3 position = glm::vec3(0, 0, 0);
				int result = sscanf_s(buffer, "v %f %f %f\n", &position.x, &position.y, &position.z);

				if (result != 3)
				{
					std::cout << "unexpected internal format\n";
					return nullptr;
				}
				positions.push_back(position);
			}
			else if (buffer[0] == 'v' && buffer[1] == 't' && buffer[2] == ' ') //defining a texture coordinate
			{
				glm::vec2 texCoord = glm::vec2(0, 0);
				int result = sscanf_s(buffer, "vt %f %f\n", &texCoord.x, &texCoord.y);

				if (result != 2)
				{
					std::cout << "unexpected internal format\n";
					return nullptr;
				}
				textureCoords.push_back(texCoord);
			}
			else if (buffer[0] == 'v' && buffer[1] == 'n' && buffer[2] == ' ') //defining a normal
			{
				glm::vec3 normal = glm::vec3(0, 0, 0);
				int result = sscanf_s(buffer, "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);

				if (result != 3)
				{
					std::cout << "unexpected internal format\n";
					return nullptr;
				}
				normals.push_back(normal);
			}
			else if (buffer[0] == 'f' && buffer[1] == ' ') //defining a face
			{
				int posIndex[3];
				int textureCoordsIndex[3];
				int normalIndex[3];

				bool hasTexCoords = true;
				bool hasNormals = true;

				//try format pos/tex/normal
				int result = sscanf_s(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &posIndex[0], &textureCoordsIndex[0],
					&normalIndex[0], &posIndex[1], &textureCoordsIndex[1], &normalIndex[1], &posIndex[2],
					&textureCoordsIndex[2], &normalIndex[2]);

				if (result != 9)
				{
					//try format pos//normal
					result = sscanf_s(buffer, "f %d//%d %d//%d %d//%d\n", &posIndex[0], &normalIndex[0], &posIndex[1],
						&normalIndex[1], &posIndex[2], &normalIndex[2]);

					if (result != 6)
					{
						//try format pos/tex
						int result = sscanf_s(buffer, "f %d/%d %d/%d %d/%d\n", &posIndex[0], &textureCoordsIndex[0],
							&posIndex[1], &textureCoordsIndex[1], &posIndex[2], &textureCoordsIndex[2]);

						if (result != 6)
						{
							std::cout << "format not supported\n";
							return nullptr;
						}

						hasNormals = false;
					}
					else
					{
						hasTexCoords = false;
					}
				}

				posIndices.push_back(posIndex[0] - 1);
				posIndices.push_back(posIndex[1] - 1);
				posIndices.push_back(posIndex[2] - 1);

				if (hasNormals)
				{
					normalIndices.push_back(normalIndex[0] - 1);
					normalIndices.push_back(normalIndex[1] - 1);
					normalIndices.push_back(normalIndex[2] - 1);
				}

				if (hasTexCoords)
				{
					texCoordIndices.push_back(textureCoordsIndex[0] - 1);
					texCoordIndices.push_back(textureCoordsIndex[1] - 1);
					texCoordIndices.push_back(textureCoordsIndex[2] - 1);
				}
			}
		}

		std::vector<glm::vec3> outPositions;
		std::vector<glm::vec2> outTextureCoords;
		std::vector<glm::vec3> outNormals;
		std::vector<unsigned int> outIndices;
		
		
		int index = 0;
		for (unsigned int i = 0; i < posIndices.size(); i++)
		{
			glm::vec2 texCoords;
			if (textureCoords.size() != 0)
			{
				texCoords = textureCoords[texCoordIndices[i]];
			}

			glm::vec3 normal;
			if (normals.size() != 0)
			{
				normal = normals[normalIndices[i]];
			}

			int currIndex = FindVertexData(outPositions, outTextureCoords, outNormals, 
				positions[posIndices[i]], texCoords, normal);

			if (currIndex == -1)
			{
				currIndex = index;
				index++;

				//add a vertex point
				outPositions.push_back(positions[posIndices[i]]);
				outTextureCoords.push_back(texCoords);
				outNormals.push_back(normal);
			}

			outIndices.push_back((unsigned int)currIndex);
		}

		return std::make_shared<Mesh>(outPositions, outTextureCoords, outNormals, outIndices);
	}

	//helper function which tries to find the VertexData and returns it's index or returns -1 if it couldn't be found
	static int FindVertexData(const std::vector<glm::vec3>& outPositions, 
		const std::vector<glm::vec2>& outTextureCoords, 
		const std::vector<glm::vec3>& outNormals, const glm::vec3& pos, const glm::vec2& textureCoords, const glm::vec3& normal)
	{
		for (unsigned int i = 0; i < outPositions.size(); i++)
		{
			if (outPositions[i] == pos && outTextureCoords[i] == textureCoords && outNormals[i] == normal)
			{
				return i;
			}
		}
		return -1;
	}

	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec2> m_textureCoords;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned int> m_indices;
};