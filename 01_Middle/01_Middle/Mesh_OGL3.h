#pragma once

#include <GL/glew.h>

#include <vector>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>

class Mesh
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	Mesh(void);
	~Mesh(void);

	void initBuffers();
	void draw();

	void addVertex(const Vertex& vertex) {
		vertices.push_back(vertex);
	}

	void addIndex(unsigned int index) {
		indices.push_back(index);
	}

	void addTriangleIndices(unsigned int a, unsigned int b, unsigned int c)
	{
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	void addQuad(glm::vec3 posA, glm::vec3 posB, glm::vec3 posC, glm::vec3 posD, glm::vec3 normal, glm::vec2 tex)
	{
		auto i = vertices.size();

		vertices.push_back({ posA, normal, tex }); // i
		vertices.push_back({ posB, normal, tex }); // i + 1
		vertices.push_back({ posC, normal, tex }); // i + 2
		vertices.push_back({ posD, normal, tex }); // i + 3

		addTriangleIndices(i, i + 1, i + 3);
		addTriangleIndices(i + 1, i + 2, i + 3);
	}

	void addQuadTex(glm::vec3 posA, glm::vec3 posB, glm::vec3 posC, glm::vec3 posD, glm::vec3 normal)
	{
		auto i = vertices.size();

		vertices.push_back({ posA, normal, glm::vec2(0.0f, 0.0f) }); // i
		vertices.push_back({ posB, normal, glm::vec2(0.0f, 1.0f) }); // i + 1
		vertices.push_back({ posC, normal, glm::vec2(1.0f, 1.0f) }); // i + 2
		vertices.push_back({ posD, normal, glm::vec2(1.0f, 0.0f) }); // i + 3

		addTriangleIndices(i, i + 1, i + 3);
		addTriangleIndices(i + 1, i + 2, i + 3);
	}

	void addTriangle(glm::vec3 posA, glm::vec3 posB, glm::vec3 posC, glm::vec3 normal, glm::vec2 tex)
	{
		auto i = vertices.size();

		vertices.push_back({ posA, normal, tex }); // i
		vertices.push_back({ posB, normal, tex }); // i + 1
		vertices.push_back({ posC, normal, tex }); // i + 2

		addTriangleIndices(i, i + 1, i + 2);
	}

	void addTriangleTex(glm::vec3 posA, glm::vec3 posB, glm::vec3 posC, glm::vec3 normal)
	{
		auto i = vertices.size();

		vertices.push_back({ posA, normal, glm::vec2(0.0f, 0.0f) }); // i
		vertices.push_back({ posB, normal, glm::vec2(1.0f, 0.0f) }); // i + 1
		vertices.push_back({ posC, normal, glm::vec2(0.0f, 1.0f) }); // i + 2

		addTriangleIndices(i, i + 1, i + 2);
	}

	void merge(Mesh *other)
	{
		auto i = vertices.size();
		vertices.insert(vertices.end(), other->vertices.begin(), other->vertices.end());

		std::for_each(other->indices.begin(), other->indices.end(), [this, i](auto index)
		{
			indices.push_back(index + i);
		});
	}

private:
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint indexBuffer;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
