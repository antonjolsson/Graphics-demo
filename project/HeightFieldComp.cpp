
#include "HeightFieldComp.h"

#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "gameObject.h"

using namespace glm;
using std::string;

HeightFieldComp::HeightFieldComp(const std::string& _heightfieldPath, const std::string& _terrainPhotoPath, GameObject* _go)
    : m_meshResolution(0)
    , m_texid_hf(UINT32_MAX)
    , m_texid_diffuse(UINT32_MAX)
    , m_vao(UINT32_MAX)
    , m_positionBuffer(UINT32_MAX)
    , m_uvBuffer(UINT32_MAX)
    , m_indexBuffer(UINT32_MAX)
    , m_numIndices(0)
    , m_heightFieldPath("")
    , m_diffuseTexturePath("")
{
	go = _go;
	generateMesh(tesselation);
	loadHeightField(_heightfieldPath);
	loadDiffuseTexture(_terrainPhotoPath);
	shaderProgram = HEIGHTFIELD_PROGRAM;
}

void HeightFieldComp::loadHeightField(const std::string& _heightFieldPath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(_heightFieldPath.c_str(), &width, &height, &components, 1);
	if(data == nullptr)
	{
		std::cout << "Failed to load image: " << _heightFieldPath << ".\n";
		return;
	}

	if(m_texid_hf == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_hf);
	}
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
	             data); // just one component (float)

	m_heightFieldPath = _heightFieldPath;
	std::cout << "Successfully loaded height field texture: " << _heightFieldPath << ".\n";
}

void HeightFieldComp::loadDiffuseTexture(const std::string& _diffusePath)
{
	int width, height, components;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* data = stbi_load(_diffusePath.c_str(), &width, &height, &components, 3);
	if(data == nullptr)
	{
		std::cout << "Failed to load image: " << _diffusePath << ".\n";
		return;
	}

	if(m_texid_diffuse == UINT32_MAX)
	{
		glGenTextures(1, &m_texid_diffuse);
	}

	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // plain RGB
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Successfully loaded diffuse texture: " << _diffusePath << ".\n";
}


void HeightFieldComp::createVBOs(const int _tesselation)
{
	m_meshResolution = _tesselation;

	std::vector<vec3> positions;
	std::vector<vec2> textureCoords;
	std::vector<std::array<unsigned int, 3>> indices;
	
	for (int i = 0; i <= _tesselation; ++i)
	{
		for (int j = 0; j <= _tesselation; ++j)
		{
			positions.emplace_back(vec3{ j * 2.f / _tesselation - 1, 0, i * 2.f / _tesselation - 1 });
			textureCoords.emplace_back(vec2{ j / static_cast<float>(_tesselation), i / static_cast<float>(_tesselation), });
		}
	}

	for (int i = 0; i < _tesselation; ++i)
	{
		for (int j = 0; j < _tesselation; ++j)
		{
			std::array<unsigned int, 3 > arr1 { j + (i * (_tesselation + 1)), j + 1 + (i * (_tesselation + 1)), j + ((_tesselation + 1) * (i + 1)) };
			indices.emplace_back(arr1);
			std::array<unsigned int, 3> arr2 { j + 1 + (i * (_tesselation + 1)), j + ((_tesselation + 1) * (i + 1)),  j + 1 + ((_tesselation + 1) * (i + 1)) };
			indices.emplace_back(arr2);
		}
	}

	glGenBuffers(1, &m_positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(vec2), &textureCoords[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(std::array<int, 3>), &indices[0], GL_STATIC_DRAW);
}

void HeightFieldComp::generateMesh(const int _tesselation)
{
	// generate a mesh in range -1 to 1 in x and z
	// (y is 0 but will be altered in height field vertex shader)
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_numIndices = _tesselation * _tesselation * 6;
	createVBOs(_tesselation);
}

void HeightFieldComp::render(const GLuint _compShaderProgram)
{
	//glUniform1i(glGetUniformLocation(HEIGHTFIELD_PROGRAM, "has_diffuse_texture"), 1);
	glUniform1i(glGetUniformLocation(_compShaderProgram, "has_diffuse_texture"), 1);
	
	if(m_vao == UINT32_MAX)
	{
		std::cout << "No vertex array is generated, cannot draw anything.\n";
		return;
	}
	glBindVertexArray(m_vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texid_hf);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texid_diffuse);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void HeightFieldComp::update(float _dt) {
	modelMatrix = mat4 ({terrainScaling});
	modelMatrix[3] = vec4{ 0, 0, 0, 1.0 };
}
