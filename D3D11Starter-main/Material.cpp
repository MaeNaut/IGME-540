#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps)
{
	colorTint = _colorTint;
	vs = _vs;
	ps = _ps;
}

std::shared_ptr<SimpleVertexShader> Material::GetVS()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPS()
{
	return ps;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

void Material::SetVS(std::shared_ptr<SimpleVertexShader> _vs)
{
	vs = _vs;
}

void Material::SetPS(std::shared_ptr<SimplePixelShader> _ps)
{
	ps = _ps;
}

void Material::SetColorTint(DirectX::XMFLOAT4 _colorTint)
{
	colorTint = _colorTint;
}

void Material::AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderVariableName, srv });
}

void Material::AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ shaderVariableName, sampler });
}
