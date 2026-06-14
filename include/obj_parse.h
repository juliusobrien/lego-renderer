#pragma once

#include <glm/glm.hpp>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

struct vertex
{
	glm::vec4 pos;
	glm::vec3 col;
	glm::vec3 nor;
};

struct triangle
{
public:
	vertex v1, v2, v3;
	bool reflect;
	int primID;
};

int obj_parse(const char* filename, std::vector<float>* io_vert)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename))
	{
		throw std::runtime_error(warn + err);
	}

	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			vertex vert{};
			vert.pos =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2],
				1.f
			};
			vert.nor =
			{
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			vert.col = { 1, 1, 1 };

			vertices.push_back(vert);
			indices.push_back(indices.size());
		}
	}

	std::vector<triangle> io_tris;

	for (int i = 0; i < indices.size() / 3; i++)
	{
		triangle tri;
		tri.v1 = vertices[indices[(i * 3) + 0]];
		tri.v2 = vertices[indices[(i * 3) + 1]];
		tri.v3 = vertices[indices[(i * 3) + 2]];

		tri.reflect = false;
		tri.primID = i;

		//if (strcmp(filename, "objs/cornell2/cornell-box.obj") == 0)
		//{

		//	tri.v1.col = glm::vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);
		//	tri.v2.col = glm::vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);
		//	tri.v3.col = glm::vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);

		//	if (tri.primID == 6 || tri.primID == 7)
		//	{
		//		tri.v1.col = glm::vec3(1, 0, 0);
		//		tri.v2.col = glm::vec3(1, 0, 0);
		//		tri.v3.col = glm::vec3(1, 0, 0);
		//	}
		//	if (tri.primID == 8 || tri.primID == 9)
		//	{
		//		tri.v1.col = glm::vec3(0, 1, 0);
		//		tri.v2.col = glm::vec3(0, 1, 0);
		//		tri.v3.col = glm::vec3(0, 1, 0);
		//	}
		//	if (tri.primID == 10 || tri.primID == 11)
		//	{
		//		tri.v1.col = glm::vec3(0, 0, 1);
		//		tri.v2.col = glm::vec3(0, 0, 1);
		//		tri.v3.col = glm::vec3(0, 0, 1);
		//	}
		//}

		//if (strcmp(filename, "objs/quad/quad.obj") == 0)
		//{
		//	tri.v1.col = glm::vec3(1, 0, 0);
		//	tri.v2.col = glm::vec3(0, 1, 0);
		//	tri.v3.col = glm::vec3(0, 0, 1);
		//}
		io_tris.push_back(tri);
	}

	io_vert->reserve(io_tris.size() * 3 * 9);
	for (const auto& tri : io_tris) {
		const vertex verts[3] = { tri.v1, tri.v2, tri.v3 };
		for (const auto& v : verts)
		{
			// vertex
			io_vert->push_back(v.pos.x);
			io_vert->push_back(v.pos.y);
			io_vert->push_back(v.pos.z);

			// color
			io_vert->push_back(v.col.r);
			io_vert->push_back(v.col.g);
			io_vert->push_back(v.col.b);

			// normal
			io_vert->push_back(v.nor.x);
			io_vert->push_back(v.nor.y);
			io_vert->push_back(v.nor.z);
		}
	}
	printf("successfully parsed %s and read %d triangles \n", filename, io_vert->size());
}
