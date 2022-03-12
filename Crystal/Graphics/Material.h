#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include "../Core/Math/Vector4.h"

namespace Crystal {
	struct MaterialProperties {
		Math::Vector4 Diffuse    { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::Vector4 Specular   { 1.0f, 1.0f, 1.0f, 1.0f };
		Math::Vector4 Emissive   { 0.0f, 0.0f, 0.0f, 1.0f };
		Math::Vector4 Ambient    { 0.0f, 0.0f, 0.0f, 1.0f };
		Math::Vector4 Reflectance{ 0.0f, 0.0f, 0.0f, 0.0f };

		float Opacity           { 1.0f   };
		float SpecularPower     { 128.0f };
		float IndexOfRefraction { 0      };
		float BumpIntensity     { 1      };

		bool HasAmbientTexture      { false };
		bool HasEmissiveTexture     { false };
		bool HasDiffuseTexture      { false };
		bool HasSpecularTexture     { false };
		bool HasSpecularPowerTexture{ false };
		bool HasNormalTexture       { false };
		bool HasBumpTexture         { false };
		bool HasOpacityTexture      { false };
	};

	class Texture;
	class Material {
	public:
		enum class TextureID {
			Ambient,
			Emissive,
			Diffuse,
			Specular,
			SpecularPower,
			Normal,
			Bump,
			Opacity
		};
		explicit Material(const MaterialProperties& materialProperties = {});

		[[nodiscard]] Texture* GetTexture(TextureID id) const noexcept;
		void SetTexture(TextureID id, std::unique_ptr<Texture>&& texture) noexcept ;

		[[nodiscard]] const Math::Vector4& GetAmbientColor() const noexcept;
		void SetAmbientColor(const Math::Vector4& ambient) noexcept;

		[[nodiscard]] const Math::Vector4& GetDiffuseColor() const noexcept;
		void SetDiffuseColor(const Math::Vector4& diffuse) noexcept;

		[[nodiscard]] const Math::Vector4& GetSpecularColor() const noexcept;
		void SetSpecularColor(const Math::Vector4& specular) noexcept;

		[[nodiscard]] const Math::Vector4& GetEmissiveColor() const noexcept;
		void SetEmissiveColor(const Math::Vector4& emissive) noexcept;

		[[nodiscard]] float GetSpecularPower() const noexcept;
		void SetSpecularPower(float specularPower) noexcept;

		[[nodiscard]] const Math::Vector4& GetReflectance() const noexcept;
		void SetReflectance(const Math::Vector4& reflectance) noexcept;

		[[nodiscard]] float GetOpacity() const noexcept;
		void SetOpacity(float opacity) noexcept;

		[[nodiscard]] float GetIndexOfRefraction() const noexcept;
		void SetIndexOfRefraction(float indexOfRefraction) noexcept;

		[[nodiscard]] float GetBumbIntensity() const;
		void SetBumpIntensity(float bumbIntensity);

		[[nodiscard]] const MaterialProperties& GetMaterialProperties() const noexcept;
		void SetMaterialProperties(const MaterialProperties& materialProperties) noexcept;

		[[nodiscard]] bool IsTransparent() const noexcept;
	private:
		std::unique_ptr<MaterialProperties> m_materialProperties;
		std::unordered_map<TextureID, std::unique_ptr<Texture>> m_textures;
	};

	namespace detail {
		constexpr std::string TextuserIDToString(Material::TextureID id) noexcept {
			constexpr std::array TextureIDStrings = {
				"ambient",
				"emissive",
				"ambient",
				"specular",
				"specularPower",
				"normal",
				"bump",
				"opacity"
			};

			return TextureIDStrings[static_cast<int>(id)];
		}
	}


	static constexpr MaterialProperties Zero{
		.Diffuse       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.Specular      = { 0.0f, 0.0f, 0.0f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 0
	};

	static constexpr MaterialProperties Red{
		.Diffuse = Math::Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		.Specular      = {1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = {0.1f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties Green{
		.Diffuse       = { 0.0f, 1.0f, 0.0f, 1.0f },
		.Specular      = {1.0f,  1.0f, 1.0f, 1.0f },
		.Ambient       = {0.0f,  0.1f, 0.0f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties Blue{
		.Diffuse       = { 0.0f, 0.0f, 1.0f, 1.0f },
		.Specular      = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.1f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties Cyan{
		.Diffuse       = { 0.0f, 1.0f, 1.0f, 1.0f },
		.Specular      = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = { 0.0f, 0.1f, 0.1f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties Magenta{
		.Diffuse       = { 1.0f, 0.0f, 1.0f, 1.0f },
		.Specular      = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = { 0.1f, 0.0f, 0.1f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties Yellow{
		.Diffuse       = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Specular      = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = { 0.1f, 0.1f, 0.1f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties White{
		.Diffuse       = { 1.0f, 0.0f, 0.0f, 1.0f },
		.Specular      = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Ambient       = { 0.1f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 128.0f
	};

	static constexpr MaterialProperties WhiteDiffuse{
		.Diffuse       = { 1.0f, 1.0f, 1.0f, 1.0f },
		.Specular      = { 0.0f, 0.0f, 0.0f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 0.0f
	};

	static constexpr MaterialProperties Black{
		.Diffuse       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.Specular      = { 0.0f, 0.0f, 0.0f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 0.0f
	};

	static constexpr MaterialProperties Emerald{
		.Diffuse       = { 0.07568f, 0.61424f,  0.07568f, 1.0f },
		.Specular      = { 0.633f,   0.727811f, 0.633f,   1.0f },
		.Ambient       = { 0.0215f,  0.1745f,   0.0215f,  1.0f },
		.SpecularPower = 76.8f,
	};

	static constexpr MaterialProperties Jade{
		.Diffuse       = { 0.54f,     0.89f,     0.63f,     1.0f },
		.Specular      = { 0.316228f, 0.316228f, 0.316228f, 1.0f },
		.Ambient       = { 0.135f,    0.2225f,   0.1575f,   1.0f },
		.SpecularPower = 12.8f
	};

	static constexpr MaterialProperties Obsidian{
		.Diffuse       = { 0.18275f,  0.17f,     0.22525f,  1.0f },
		.Specular      = { 0.332741f, 0.328634f, 0.346435f, 1.0f },
		.Ambient       = { 0.05375f,  0.05f,     0.06625f,  1.0f },
		.SpecularPower = 38.4f
	};

	static constexpr MaterialProperties Pearl{
		.Diffuse       = { 1.0f,      0.829f,    0.829f,    1.0f },
		.Specular      = {0.296648f, 0.296648f, 0.296648f, 1.0f },
		.Ambient       = {0.25f, 0.20725f, 0.20725f, 1.0f },
		.SpecularPower = 11.264f,
	};

	static constexpr MaterialProperties Ruby{
		.Diffuse       = {0.61424f,  0.04136f,  0.04136f,  1.0f },
		.Specular      = { 0.727811f, 0.626959f, 0.626959f, 1.0f},
		.Ambient       = {0.1745f, 0.01175f, 0.01175f, 1.0f },
		.SpecularPower = 76.8f
	};

	static constexpr MaterialProperties Turquoise{
		.Diffuse       = {0.396f,    0.74151f, 0.69102f,  1.0f },
		.Specular      = {0.297254f, 0.30829f, 0.306678f, 1.0f },
		.Ambient       = {0.1f, 0.18725f, 0.1745f, 1.0f },
		.SpecularPower = 12.8f
	};

	static constexpr MaterialProperties Brass{
		.Diffuse       = { 0.780392f, 0.568627f, 0.113725f, 1.0f },
		.Specular      = { 0.992157f, 0.941176f, 0.807843f, 1.0f },
		.Ambient       = { 0.329412f, 0.223529f, 0.027451f, 1.0f },
		.SpecularPower = 27.9f
	};

	static constexpr MaterialProperties Bronze{
		.Diffuse       = { 0.714f,    0.4284f,   0.18144f,  1.0f},
		.Specular      = { 0.393548f, 0.271906f, 0.166721f, 1.0f},
		.Ambient       = { 0.2125f,   0.1275f,   0.054f,    1.0f},
		.SpecularPower = 25.6f
	};

	static constexpr MaterialProperties Chrome{
		.Diffuse       = { 0.4f,      0.4f,      0.4f,      1.0f },
		.Specular      = { 0.774597f, 0.774597f, 0.774597f, 1.0f },
		.Ambient       = { 0.25f,     0.25f,     0.25f,     1.0f },
		.SpecularPower = 76.8f
	};

	static constexpr MaterialProperties Copper{
		.Diffuse       = { 0.7038f,   0.27048f,  0.0828f,   1.0f },
		.Specular      = { 0.256777f, 0.137622f, 0.086014f, 1.0f},
		.Ambient       = { 0.19125f,  0.0735f,   0.0225f,   1.0f},
		.SpecularPower = 12.8f
	};

	static constexpr MaterialProperties Gold{
		.Diffuse       = { 0.75164f,  0.60648f,  0.22648f,  1.0f },
		.Specular      = { 0.628281f, 0.555802f, 0.366065f, 1.0f },
		.Ambient       = { 0.24725f,  0.1995f,   0.0745f,   1.0f },
		.SpecularPower = 51.2f
	};

	static constexpr MaterialProperties Silver{
		.Diffuse       = { 0.50754f,  0.50754f,  0.50754f,  1.0f },
		.Specular      = { 0.508273f, 0.508273f, 0.508273f, 1.0f },
		.Ambient       = { 0.19225f,  0.19225f,  0.19225f,  1.0f },
		.SpecularPower = 51.2f
	};

	static constexpr MaterialProperties BlackPlastic{
		.Diffuse       = { 0.01f, 0.01f, 0.01f, 1.0f },
		.Specular      = { 0.5f,  0.5f,  0.5f,  1.0f },
		.Ambient       = { 0.0f,  0.0f,  0.0f,  1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties CyanPlastic{
		.Diffuse       = { 0.0f,        0.50980392f, 0.50980392f, 1.0f },
		.Specular      = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },
		.Ambient       = { 0.0f,        0.1f,        0.06f,       1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties GreenPlastic{
		.Diffuse       = { 0.1f,  0.35f, 0.1f,  1.0f},
		.Specular      = { 0.45f, 0.55f, 0.45f, 1.0f },
		.Ambient       = { 0.0f,  0.0f,  0.0f,  1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties RedPlastic{
		.Diffuse       = { 0.5f, 0.0f, 0.0f, 1.0f },
		.Specular      = { 0.7f, 0.6f, 0.6f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties WhitePlastic{
		.Diffuse       = { 0.55f, 0.55f, 0.55f, 1.0f },
		.Specular      = { 0.7f,  0.7f,  0.7f,  1.0f },
		.Ambient       = { 0.0f,  0.0f,  0.0f,  1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties YellowPlastic{
		.Diffuse       = { 0.5f, 0.5f, 0.0f, 1.0f },
		.Specular      = { 0.6f, 0.6f, 0.5f, 1.0f },
		.Ambient       = { 0.0f, 0.0f, 0.0f, 1.0f },
		.SpecularPower = 32.0f
	};

	static constexpr MaterialProperties BlackRubber{
		.Diffuse       = { 0.01f, 0.01f, 0.01f, 1.0f },
		.Specular      = { 0.4f,  0.4f,  0.4f,  1.0f },
		.Ambient       = { 0.02f, 0.02f, 0.02f, 1.0f },
		.SpecularPower = 10.0f
	};

	static constexpr MaterialProperties CyanRubber{
		.Diffuse       = { 0.4f,  0.5f,  0.5f,  1.0f },
		.Specular      = { 0.04f, 0.7f,  0.7f,  1.0f },
		.Ambient       = { 0.0f,  0.05f, 0.05f, 1.0f },
		.SpecularPower = 10.0f
	};

	static constexpr MaterialProperties GreenRubber{
		.Diffuse       = { 0.4f,  0.5f,  0.4f,  1.0f},
		.Specular      = { 0.04f, 0.7f,  0.04f, 1.0f},
		.Ambient       = { 0.0f,  0.05f, 0.0f,  1.0f},
		.SpecularPower = 10.0f
	};

	static constexpr MaterialProperties RedRubber{
		.Diffuse       = { 0.5f,  0.4f,  0.4f,  1.0f },
		.Specular      = { 0.7f,  0.04f, 0.04f, 1.0f},
		.Ambient       = { 0.05f, 0.0f,  0.0f,  1.0f},
		.SpecularPower = 10.0f
	};

	static constexpr MaterialProperties WhiteRubber{
		.Diffuse       = { 0.5f,  0.5f,  0.5f,  1.0f },
		.Specular      = { 0.7f,  0.7f,  0.7f,  1.0f },
		.Ambient       = { 0.05f, 0.05f, 0.05f, 1.0f },
		.SpecularPower = 10.0f
	};

	static constexpr MaterialProperties YellowRubber{
		.Diffuse       = { 0.5f,  0.5f,   0.4f,   1.0  },
		.Specular      = { 0.7f,  0.7f,   0.04f,  1.0f },
		.Ambient       = { 0.05f, 0.05f,  0.0f,   1.0f },
		.SpecularPower = 10.0f
	};
}
