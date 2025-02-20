#pragma once

#include "SimpleShader.h"
#include "Mesh.h"
#include "Transform.h"

#include <memory>

class Material
{
public:
	Material(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps);

	// Getters
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();

private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
};

