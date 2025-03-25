#pragma once

#include "SimpleShader.h"
#include "Mesh.h"
#include "Transform.h"

#include <memory>
#include <unordered_map>

class Material
{
public:
	// Constructor
	Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps);

	// Getters
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColorTint();
	DirectX::XMFLOAT2 GetScale();
	DirectX::XMFLOAT2 GetOffset();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetSRV();

	// Setters
	void SetVS(std::shared_ptr<SimpleVertexShader> _vs);
	void SetPS(std::shared_ptr<SimplePixelShader> _ps);
	void SetColorTint(DirectX::XMFLOAT4 _colorTint);
	void SetScale(DirectX::XMFLOAT2 _scale);
	void SetOffset(DirectX::XMFLOAT2 _offset);

	// Methods
	void AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void BindResources();

private:
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 offset;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

};

