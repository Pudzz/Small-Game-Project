#pragma once
#include "Texture.h"
#include "Shader.h"
#include <unordered_map>
#include "ConstantBuffers.h"

enum class SHADER_BIND_TYPE
{
	VERTEX,
	PIXEL
};

const size_t SHADER_BIND_TYPE_COUNT = 2;
constexpr size_t ALBEDO_MATERIAL_TYPE = 0;
constexpr size_t NORMAL_MATERIAL_TYPE = 1;

class Material
{
	struct MaterialSlot
	{
		Texture* texture;
		ID3D11SamplerState* sampler;
	};

public:
	
	Material(Shader* shader, DX11Handler& dx11);
	virtual ~Material();

	size_t GetID() const { return this->id; }

	void SetMaterialData(MaterialData materialData) { this->materialData = materialData; }
	MaterialData& GetMaterialData() { return this->materialData; }

	void SetTexture(size_t index, Texture* texture, SHADER_BIND_TYPE type = SHADER_BIND_TYPE::PIXEL);
	void SetSampler(size_t index, ID3D11SamplerState* sampler, SHADER_BIND_TYPE type = SHADER_BIND_TYPE::PIXEL);

	Shader* GetShader() const { return this->shader; }
	void Bind(ID3D11DeviceContext*);
	void Unbind(ID3D11DeviceContext*);

private:

	size_t id;
	Shader* shader;
	ID3D11Buffer* buffer;
	MaterialData materialData;
	// 2 = items in SHADER_BIND_TYPE 
	std::unordered_map<size_t, MaterialSlot> slots [SHADER_BIND_TYPE_COUNT];
};

static size_t material_id_counter = 0;
