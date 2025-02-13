#pragma once

#include "Transform.h"
#include "Mesh.h"

#include <memory>

class GameEntity
{
public:
	GameEntity() = default;
	GameEntity(std::shared_ptr<Mesh> _mesh);

	// Getters
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Mesh> GetMesh();

	// Draw Method
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> &constantBuffer);

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
};

