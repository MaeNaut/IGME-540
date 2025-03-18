#pragma once

#include "SimpleShader.h"
#include "Mesh.h"
#include "Transform.h"

#include <memory>
#include <unordered_map>

class Material
{
public:
	Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps);

	// Getters
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColorTint();

	// Setters
	void SetVS(std::shared_ptr<SimpleVertexShader> _vs);
	void SetPS(std::shared_ptr<SimplePixelShader> _ps);
	void SetColorTint(DirectX::XMFLOAT4 _colorTint);

	// Adders
	void AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};

