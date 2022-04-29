#pragma once
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Material.h"
#include "assimp/scene.h"

namespace Assimp {
	class Importer;
}

namespace Crystal {
	namespace detail {
		enum class MaterialUnit { Vec4, Float };

		struct AssimpMaterialSpecification {
			std::string_view Key;
			uint32_t Type;
			uint32_t Id;
			MaterialUnit Unit;
		};
	}

	class CommandContext;
	class Mesh;
	class Scene {
	public:
		bool LoadSceneFromFile(CommandContext& ctx, std::string_view fileName);
	private:
		void ImportScene(CommandContext& ctx, const aiScene& scene, std::string_view parentPath);
		void ImportMesh(CommandContext& ctx, const aiMesh& assimpMesh);
		void ImportMaterial(CommandContext& ctx, const aiMaterial& aiMat, std::string_view parentPath) noexcept;

		void SetMaterials(Material& material, const aiMaterial& assimpMaterial) const noexcept;
		void LoadTextures(CommandContext& ctx, Material& material, const aiMaterial& assimpMaterial, std::string_view parentPath) const noexcept;

		void ProcessVertices(CommandContext& ctx, Mesh& mesh, const aiMesh& aiMesh) const noexcept;
		void ProcessIndices(CommandContext& ctx, Mesh& mesh, const aiMesh& aiMesh) const noexcept;

		static std::optional<const aiScene*> PreProcess(std::string_view fileName) noexcept;

		std::vector<std::unique_ptr<Mesh>> m_meshes;
		std::vector<std::unique_ptr<Material>> m_materials;

		using MaterialColorFuncPtr = void(Material::*)(const Math::Vector4&);
		using MaterialValueFuncPtr = void(Material::*)(float);

		union MaterialFunction {
			MaterialFunction(MaterialColorFuncPtr ptr) : ColorFuncPtr(ptr) {}
			MaterialFunction(MaterialValueFuncPtr ptr) : ValueFuncPtr(ptr) {}

			MaterialColorFuncPtr ColorFuncPtr;
			MaterialValueFuncPtr ValueFuncPtr;
		}; 

		std::unordered_map<detail::AssimpMaterialSpecification, MaterialFunction> m_materialFunctionMap =
		{
			{ { AI_MATKEY_COLOR_AMBIENT , detail::MaterialUnit::Vec4  }, &Material::SetAmbientColor      },
			{ { AI_MATKEY_COLOR_EMISSIVE, detail::MaterialUnit::Vec4  }, &Material::SetEmissiveColor     },
			{ { AI_MATKEY_COLOR_DIFFUSE , detail::MaterialUnit::Vec4  }, &Material::SetDiffuseColor      },
			{ { AI_MATKEY_COLOR_SPECULAR, detail::MaterialUnit::Vec4  }, &Material::SetAmbientColor      },
			{ { AI_MATKEY_SHININESS     , detail::MaterialUnit::Float }, &Material::SetSpecularPower     },
			{ { AI_MATKEY_OPACITY       , detail::MaterialUnit::Float }, &Material::SetOpacity           },
			{ { AI_MATKEY_REFRACTI      , detail::MaterialUnit::Float }, &Material::SetIndexOfRefraction },
			{ { AI_MATKEY_REFLECTIVITY  , detail::MaterialUnit::Float }, &Material::SetReflectance       },
			{ { AI_MATKEY_BUMPSCALING   , detail::MaterialUnit::Float }, &Material::SetBumpIntensity     }
		};

		static constexpr std::array<std::pair<aiTextureType, bool>, 8> AiTextureTypes = {
			{
				{ aiTextureType_AMBIENT,   true  },
				{ aiTextureType_EMISSIVE,  true  },
				{ aiTextureType_DIFFUSE,   true  },
				{ aiTextureType_SPECULAR,  true  },
				{ aiTextureType_SHININESS, false },
				{ aiTextureType_OPACITY,   false },
				{ aiTextureType_NORMALS,   false },
				{ aiTextureType_HEIGHT,    false }
			}
		};
	};
}


