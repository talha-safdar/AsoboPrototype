#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include "Model.h"
#include "Shader.h"
#include "Vertex.h"

#include <glm/gtc/type_ptr.hpp>

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

	m_parts.clear();

	std::cout << "Node count: " << data->nodes_count << std::endl;

	for (cgltf_size nodeIndex = 0; nodeIndex < data->nodes_count; ++nodeIndex)
	{
		// Go through every node in the GLB. If this node actually contains/references a mesh, load it.
		cgltf_node& node = data->nodes[nodeIndex];

		std::cout << "Node " << nodeIndex;

		if (node.name)
		{
			std::cout << ": " << node.name;
		}

		std::cout << std::endl;

		if (!node.mesh)
		{
			continue;
		}

		cgltf_float worldTransform[16];
		cgltf_node_transform_world(&node, worldTransform);

		glm::mat4 nodeTransform = glm::make_mat4(worldTransform);

		cgltf_mesh& gltfMesh = *node.mesh;

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

			ModelPart part;
			part.name = node.name ? node.name : "Unnamed";
			part.mesh = std::make_unique<Mesh>();
			part.transform = nodeTransform;

			if (!part.mesh->Create(vertices, indices))
			{
				std::cerr << "Failed to create mesh from model: " << path << std::endl;
				cgltf_free(data);
				return false;
			}

			m_parts.push_back(std::move(part));
		}
	}

	cgltf_free(data);

	std::cout << "Loaded model: " << path << " | Parts: " << m_parts.size() << std::endl;

	return !m_parts.empty();
}

void Model::Draw(Shader& shader, const glm::mat4& modelMatrix) const
{
	for (const ModelPart& part : m_parts)
	{
		// glm::mat4 finalModel = modelMatrix * part.transform;
		glm::mat4 finalModel = modelMatrix * part.transform * part.localTransform;

		shader.SetMat4("uModel", finalModel);
		part.mesh->Draw();
	}
}

void Model::RotatePart(const std::string& name, float angle, const glm::vec3& axis)
{
	for (ModelPart& part : m_parts)
	{
		if (part.name == name)
		{
			part.localTransform = glm::rotate(glm::mat4(1.0f), angle, axis);
			return;
		}
	}

	std::cerr << "Model part not found: " << name << std::endl;
}

void Model::ResetPartTransform(const std::string & name)
{
	// usage: m_testModel->ResetPartTransform("RightArm");
	for (ModelPart& part : m_parts)
	{
		if (part.name == name)
		{
			part.localTransform = glm::mat4(1.0f);
			return;
		}
	}
}

void Model::PrintParts() const
{
	std::cout << "Model parts:" << std::endl;

	for (const ModelPart& part : m_parts)
	{
		std::cout << " - " << part.name << std::endl;
	}
}
