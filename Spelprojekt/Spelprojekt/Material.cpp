#include "Material.h"
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <assert.h>

Material::Material(Shader* shader, DX11Handler& dx11) : shader(shader)
{
	this->id = material_id_counter++;
	this->buffer = dx11.CreateBuffer<MaterialData>(materialData);

	this->materialData.ambient = { 0.8f, 0.8f, 0.8f ,1 };
	this->materialData.diffuse = { 0.8f, 0.8f, 0.8f ,1 };
	this->materialData.specular = { 0.8f, 0.8f, 0.8f ,1 };
	this->materialData.shininess = 32;

	this->materialData.hasAlbedoTexture = false;
	this->materialData.hasNormalTexture = false;
}

Material::~Material()
{
}

void Material::SetTexture(size_t index, Texture* texture, SHADER_BIND_TYPE type)
{
	auto& slot = slots[(int)type];
	auto gg = slot.find(index);

	if (gg == slot.end())
	{
		MaterialSlot newSlot;
		newSlot.texture = texture;
		newSlot.sampler = nullptr;
		slot.insert({ index, newSlot });
	}
	else
	{
		(*gg).second.texture = texture;
	}
}

void Material::SetSampler(size_t index, ID3D11SamplerState* sampler, SHADER_BIND_TYPE type)
{
	auto& slot = slots[(int)type];
	auto gg = slot.find(index);

	if (gg == slot.end())
	{
		MaterialSlot newSlot;
		newSlot.texture = nullptr;
		newSlot.sampler = sampler;
		slot.insert({ index, newSlot });
	}
	else
	{
		(*gg).second.sampler = sampler;
	}
}

void Material::Unbind(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* pSRV[1] = { NULL };

	SHADER_BIND_TYPE bindType;
	MaterialSlot slot;

	for (size_t j = 0; j < SHADER_BIND_TYPE_COUNT; j++)
	{
		for (auto slotKV : slots[j])
		{
			size_t index = slotKV.first;

			switch ((SHADER_BIND_TYPE)j)
			{
			case SHADER_BIND_TYPE::VERTEX:
				if (slotKV.second.texture != nullptr)
					context->VSSetShaderResources(index, 1, pSRV);
				break;

			case SHADER_BIND_TYPE::PIXEL:
				if (slotKV.second.texture != nullptr)
					context->PSSetShaderResources(index, 1, pSRV);
				break;
			}
		}
	}
}

void Material::Bind(ID3D11DeviceContext* context)
{
	context->UpdateSubresource(buffer, 0, 0, &materialData, 0, 0);
	context->PSSetConstantBuffers(MATERIAL_CONSTANT_BUFFER_SLOT, 1, &buffer);

	SHADER_BIND_TYPE bindType;
	MaterialSlot slot;

	for (size_t j = 0; j < SHADER_BIND_TYPE_COUNT; j++)
	{
		for (auto slotKV : slots[j])
		{
			size_t index = slotKV.first;

			switch ((SHADER_BIND_TYPE)j)
			{
			case SHADER_BIND_TYPE::VERTEX:
				if (slotKV.second.sampler != nullptr)
					context->VSSetSamplers(index, 1, &slotKV.second.sampler);

				if (slotKV.second.texture != nullptr)
				{
					ID3D11ShaderResourceView* srv = slotKV.second.texture->GetSRV();
					context->VSSetShaderResources(index, 1, &srv);
				}

				break;
			case SHADER_BIND_TYPE::PIXEL:
				if (slotKV.second.sampler != nullptr)
					context->PSSetSamplers(index, 1, &slotKV.second.sampler);

				if (slotKV.second.texture != nullptr)
				{
					ID3D11ShaderResourceView* srv = slotKV.second.texture->GetSRV();
					context->PSSetShaderResources(index, 1, &srv);
				}
				break;
			}
		}
	}
}
