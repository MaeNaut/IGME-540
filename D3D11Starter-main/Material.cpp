#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps)
{
	colorTint = _colorTint;
	vs = _vs;
	ps = _ps;

	scale = { 1.0f, 1.0f };
	offset = { 0.0f, 0.0f };
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

DirectX::XMFLOAT2 Material::GetScale()
{
	return scale;
}

DirectX::XMFLOAT2 Material::GetOffset()
{
	return offset;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetSRV()
{
	return textureSRVs;
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

void Material::SetScale(DirectX::XMFLOAT2 _scale)
{
	scale = _scale;
}

void Material::SetOffset(DirectX::XMFLOAT2 _offset)
{
	offset = _offset;
}

void Material::AddTextureSRV(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderVariableName, srv });
}

void Material::AddSampler(std::string shaderVariableName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ shaderVariableName, sampler });
}

void Material::BindResources()
{
	for (auto& t : textureSRVs)
	{
		ps->SetShaderResourceView(t.first.c_str(), t.second);
	}
	for (auto& s : samplers)
	{
		ps->SetSamplerState(s.first.c_str(), s.second);
	}
}
