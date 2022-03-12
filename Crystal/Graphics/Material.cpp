#include "Material.h"
#include "../RHI/Texture.h"
#include "../Core/Logging/Logger.h"

using namespace Crystal;
using namespace Crystal::Math;

Material::Material(const MaterialProperties& materialProperties) 
    :
    m_materialProperties(std::make_unique<MaterialProperties>(materialProperties))
{}

Texture* Crystal::Material::GetTexture(TextureID id) const noexcept {
    if (m_textures.contains(id)) {
        return m_textures.at(id).get();
    }
    else {
        cryfmtlog_warning("Material did not contain a {} texture, returning nullptr", detail::TextuserIDToString(id));
        return nullptr;
    }
}

void Material::SetTexture(TextureID id, std::unique_ptr<Texture>&& texture) noexcept {
    if (texture) {
        m_textures[id] = std::move(texture);

        using enum Material::TextureID;
        switch (id)
        {
        case Ambient:
            m_materialProperties->HasAmbientTexture = true;
            break;
        case Emissive:
            m_materialProperties->HasEmissiveTexture = true;
            break;
        case Diffuse:
            m_materialProperties->HasDiffuseTexture = true;
            break;
        case Specular:
            m_materialProperties->HasSpecularTexture = true;
            break;
        case SpecularPower:
            m_materialProperties->HasSpecularPowerTexture = true;
            break;
        case Normal:
            m_materialProperties->HasNormalTexture = true;
            break;
        case Bump:
            m_materialProperties->HasBumpTexture = true;
            break;
        case Opacity:
            m_materialProperties->HasOpacityTexture = true;
            break;
        }
    }
    else {
        crylog_warning("Texture is null");
    }
}

const Vector4& Material::GetAmbientColor() const noexcept {
    return m_materialProperties->Ambient;
}

void Material::SetAmbientColor(const Vector4& ambient) noexcept {
    m_materialProperties->Ambient = ambient;
}

const Vector4& Material::GetDiffuseColor() const noexcept {
    return m_materialProperties->Diffuse;
}

void Material::SetDiffuseColor(const Vector4& diffuse) noexcept {
    m_materialProperties->Diffuse = diffuse;
}

const Vector4& Material::GetSpecularColor() const noexcept {
    return m_materialProperties->Specular;
}

void Material::SetSpecularColor(const Vector4& specular) noexcept {
    m_materialProperties->Specular = specular;
}

const Vector4& Material::GetEmissiveColor() const noexcept {
    return m_materialProperties->Emissive;
}

void Material::SetEmissiveColor(const Vector4& emissive) noexcept {
    m_materialProperties->Emissive = emissive;
}

float Material::GetSpecularPower() const noexcept {
    return m_materialProperties->SpecularPower;
}

void Material::SetSpecularPower(float specularPower) noexcept {
    m_materialProperties->SpecularPower = specularPower;
}

const Vector4& Material::GetReflectance() const noexcept {
    return m_materialProperties->Reflectance;
}

void Material::SetReflectance(const Vector4& reflectance) noexcept {
    m_materialProperties->Reflectance = reflectance;
}

float Material::GetOpacity() const noexcept {
    return m_materialProperties->Opacity;
}

void Material::SetOpacity(float opacity) noexcept {
    m_materialProperties->Opacity = opacity;
}

float Material::GetIndexOfRefraction() const noexcept {
    return m_materialProperties->IndexOfRefraction;
}

void Material::SetIndexOfRefraction(float indexOfRefraction) noexcept {
    m_materialProperties->IndexOfRefraction = indexOfRefraction;
}

float Material::GetBumbIntensity() const {
    return m_materialProperties->BumpIntensity;
}

void Material::SetBumpIntensity(float bumbIntensity) {
    m_materialProperties->BumpIntensity = bumbIntensity;
}

const MaterialProperties& Material::GetMaterialProperties() const noexcept {
    return *m_materialProperties;
}

void Material::SetMaterialProperties(const MaterialProperties& materialProperties) noexcept {
    *m_materialProperties = materialProperties;
}

bool Material::IsTransparent() const noexcept {
    return m_materialProperties->Opacity < 1.0f || m_materialProperties->HasOpacityTexture;
}
