#include "Scene.h"

#include <cassert>

#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Core/FileSystem/FileSystem.h"
#include "RHI/CommandContext.h"

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/ProgressHandler.hpp"
#include "RHI/RHICore.h"
#include "RHI/VertexTypes.h"
#include "RHI/D3D12/Managers/TextureManager.h"

using namespace Crystal;

bool Scene::LoadSceneFromFile(CommandContext& ctx, std::string_view fileName) {
	const auto parentPath = FileSystem::HasParentPath(fileName)
							? FileSystem::GetParentDirectory(fileName)
							: FileSystem::GetWorkingDirectory();

	if(const auto scene = PreProcess(fileName)){
		ImportScene(ctx, *scene.value(), parentPath);
		return true;
	}
	return false;
}

void Scene::ImportScene(CommandContext& ctx, const aiScene& scene, std::string_view parentPath) {
	m_materials.clear();
	m_meshes.clear();

	for (auto i = 0; i < scene.mNumMaterials; i++) {
		ImportMaterial(ctx, *(scene.mMaterials[i]), parentPath);
	}

	for(auto i = 0; i < scene.mNumMeshes; i++) {
		ImportMesh(ctx, *(scene.mMeshes[i]));
	}
}

void Scene::ImportMesh(CommandContext& ctx, const aiMesh& assimpMesh) {
	m_meshes.clear();
	Mesh mesh;

	ProcessVertices(ctx, mesh, assimpMesh);
	ProcessIndices(ctx, mesh, assimpMesh);

	m_meshes.emplace_back(mesh);
}

void Scene::ImportMaterial(CommandContext& ctx, const aiMaterial& aiMat, std::string_view parentPath) noexcept {
	Material material;

	SetMaterials(material, aiMat);
	LoadTextures(ctx, material, aiMat, parentPath);

	m_materials.emplace_back(material);
}

void Scene::SetMaterials(Material& material, const aiMaterial& assimpMaterial) const noexcept {
	aiColor4D color{};
	float value{};

	//Set materials
	for (const auto& [spec, func] : m_materialFunctionMap) {
		if (spec.Unit == detail::MaterialUnit::Vec4) {
			if (assimpMaterial.Get(spec.Key.data(), spec.Type, spec.Id, color) == aiReturn_SUCCESS) {
				(material.*func.ColorFuncPtr)(reinterpret_cast<Math::Vector4&>(color));
			}
		}
		else if (spec.Unit == detail::MaterialUnit::Float) {
			if (assimpMaterial.Get(spec.Key.data(), spec.Type, spec.Id, value) == aiReturn_SUCCESS) {
				//Special case when MaterialType is Reflectivity
				if (spec.Key.compare("$mat.reflectivity") == 0) [[unlikely]] {
					(material.*func.ColorFuncPtr)({ value });
				}
				else {
					(material.*func.ValueFuncPtr)(value);
				}
			}
		}
	}
}

void Scene::LoadTextures(CommandContext& ctx, Material& material, const aiMaterial& assimpMaterial, std::string_view parentPath) const noexcept {
	aiString aiTexturePath;
	aiTextureOp aiBlendOperation;
	float blendFactor;

	const auto hasTexture = [&](aiTextureType textureType) {
		return assimpMaterial.GetTextureCount(textureType) > 0 &&
			   assimpMaterial.GetTexture(
			       textureType,
			       0, &aiTexturePath,
			       nullptr, nullptr,
			       &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS;
	};

	for(const auto& [textureType, make_sRGB] : AiTextureTypes) {
		if (hasTexture(textureType)) {
			const auto filePath = FileSystem::Append(parentPath, aiTexturePath.C_Str());
			auto texture = TextureManager::LoadTextureFromFile(ctx, filePath, make_sRGB);
			material.SetTexture(static_cast<Material::TextureID>(textureType), std::move(texture));
		}
	}
}

std::optional<const aiScene*> Scene::PreProcess(std::string_view fileName) noexcept {
	const auto exportPath = FileSystem::ReplaceExtension(fileName, "assBin");

	Assimp::Importer importer;

	//Check if preprocessed file exists
	if (FileSystem::IsFile(exportPath)) {
		if(const auto scene = importer.ReadFile(exportPath, aiProcess_GenBoundingBoxes)) {
			return scene;
		}
		return {};
	}

	//File has not been preprocessed yet. Import and process the file.
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

	constexpr auto preProcessFlag =
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_OptimizeGraph |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenBoundingBoxes;

	if (const auto scene = importer.ReadFile(fileName.data(), preProcessFlag)) {
		//Export the preprocessed scene file for faster loading next time.
		Assimp::Exporter{}.Export(scene, "assBin", exportPath, 0);
		return scene;
	}
	return {};
}

void Scene::ProcessVertices(CommandContext& ctx, Mesh& mesh, const aiMesh& aiMesh) const noexcept {
	std::vector<Vertex> vertices;
	vertices.reserve(aiMesh.mNumVertices);

	assert(aiMesh.mMaterialIndex < m_materials.size());

	if(aiMesh.HasPositions()) [[likely]] {
		for(uint32_t i = 0; i < aiMesh.mNumVertices; i++) {
			vertices[i].Position = reinterpret_cast<Math::Vector3&>(aiMesh.mVertices[i]);
		}
	}

	if(aiMesh.HasNormals()) {
		for (uint32_t i = 0; i < aiMesh.mNumVertices; i++) {
			vertices[i].Normal = reinterpret_cast<Math::Vector3&>(aiMesh.mNormals[i]);
		}
	}

	if (aiMesh.HasTangentsAndBitangents()) {
		for (uint32_t i = 0; i < aiMesh.mNumVertices; i++) {
			vertices[i].Tangent   = reinterpret_cast<Math::Vector3&>(aiMesh.mTangents[i]);
			vertices[i].Bitangent = reinterpret_cast<Math::Vector3&>(aiMesh.mBitangents[i]);
		}
	}

	if (aiMesh.HasTextureCoords(0)) {
		for (uint32_t i = 0; i < aiMesh.mNumVertices; i++) {
			vertices[i].TexCoord = reinterpret_cast<Math::Vector3&>(aiMesh.mTextureCoords[0][i]);
		}
	}

	BufferDescription vbd = {
		.Count  = static_cast<uint32_t>(vertices.size()),
		.Stride = sizeof(std::decay_t<decltype(vertices)>)
	};

	mesh.SetVertexBuffer(0, std::make_unique<Buffer>(vbd));
}

void Scene::ProcessIndices(CommandContext& ctx, Mesh& mesh, const aiMesh& aiMesh) const noexcept {
	if (aiMesh.HasFaces()) [[likely]] {
		std::vector<uint32_t> indices;

		for(uint32_t i = 0; i < aiMesh.mNumFaces; i++) {
			const auto& face = aiMesh.mFaces[i];

			// Only extract triangular faces
			if(face.mNumIndices == 3) {
				indices.emplace_back(face.mIndices[0]);
				indices.emplace_back(face.mIndices[1]);
				indices.emplace_back(face.mIndices[2]);
			}
		}

		if (!indices.empty()) [[likely]] {
			BufferDescription ibd = {
				.Count  = static_cast<uint32_t>(indices.size()),
				.Stride = sizeof(std::decay_t<decltype(indices)>),
				.Format = IndexFormat_t::uint_32
			};

			mesh.SetIndexBuffer(std::make_unique<Buffer>(ibd));
		}
	}
}

 