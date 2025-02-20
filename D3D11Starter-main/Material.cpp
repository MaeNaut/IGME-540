#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps)
{
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
