#include "Model.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include "Model.h"
#include "Vertex.h"

#include <iostream>


bool Model::Load(const std::string& path)
{
	cgltf_options options{};
	cgltf_data* data = nullptr;

	cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);

	if (result != cgltf_result_success)
	{
		std::cerr << "Failed to parse model: " << path << std::endl;
		return false;
	}

	result = cgltf_load_buffers(&options, data, path.c_str());

	if (result != cgltf_result_success)
	{
		std::cerr << "Failed to load model buffers: " << path << std::endl;
		cgltf_free(data);
		return false;
	}

	for (cgltf_size meshIndex = 0; meshIndex < data->meshes_count; ++meshIndex)
	{
		cgltf_mesh& gltfMesh = data->meshes[meshIndex];

		for (cgltf_size primitiveIndex = 0; primitiveIndex < gltfMesh.primitives_count; ++primitiveIndex)
		{
			cgltf_primitive& primitive = gltfMesh.primitives[primitiveIndex];

			if (primitive.type != cgltf_primitive_type_triangles)
				continue;

			const cgltf_accessor* positionAccessor = nullptr;
			const cgltf_accessor* normalAccessor = nullptr;
			const cgltf_accessor* texCoordAccessor = nullptr;

			for (cgltf_size attributeIndex = 0; attributeIndex < primitive.attributes_count; ++attributeIndex)
			{
				const cgltf_attribute& attribute = primitive.attributes[attributeIndex];

				if (attribute.type == cgltf_attribute_type_position)
					positionAccessor = attribute.data;

				if (attribute.type == cgltf_attribute_type_normal)
					normalAccessor = attribute.data;

				if (attribute.type == cgltf_attribute_type_texcoord && attribute.index == 0)
					texCoordAccessor = attribute.data;
			}

			if (!positionAccessor)
				continue;

			std::vector<Vertex> vertices;
			vertices.reserve(positionAccessor->count);

			for (cgltf_size vertexIndex = 0; vertexIndex < positionAccessor->count; ++vertexIndex)
			{
				Vertex vertex{};
				cgltf_float values[4]{};

				cgltf_accessor_read_float(positionAccessor, vertexIndex, values, 3);
				vertex.position = glm::vec3(values[0], values[1], values[2]);

				if (normalAccessor)
				{
					cgltf_accessor_read_float(normalAccessor, vertexIndex, values, 3);
					vertex.normal = glm::vec3(values[0], values[1], values[2]);
				}
				else
				{
					vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
				}

				if (texCoordAccessor)
				{
					cgltf_accessor_read_float(texCoordAccessor, vertexIndex, values, 2);
					vertex.texCoord = glm::vec2(values[0], values[1]);
				}
				else
				{
					vertex.texCoord = glm::vec2(0.0f);
				}

				vertices.push_back(vertex);
			}

			std::vector<unsigned int> indices;

			if (primitive.indices)
			{
				indices.resize(primitive.indices->count);

				for (cgltf_size index = 0; index < primitive.indices->count; ++index)
					indices[index] = static_cast<unsigned int>(cgltf_accessor_read_index(primitive.indices, index));
			}
			else
			{
				indices.resize(vertices.size());

				for (unsigned int index = 0; index < indices.size(); ++index)
					indices[index] = index;
			}

			auto mesh = std::make_unique<Mesh>();

			if (!mesh->Create(vertices, indices))
			{
				std::cerr << "Failed to create mesh from model: " << path << std::endl;
				cgltf_free(data);
				return false;
			}

			m_meshes.push_back(std::move(mesh));
		}
	}

	cgltf_free(data);

	std::cout << "Loaded model: " << path << " | Meshes: " << m_meshes.size() << std::endl;

	return !m_meshes.empty();
}

void Model::Draw() const
{
	for (const std::unique_ptr<Mesh>& mesh : m_meshes)
		mesh->Draw();
}