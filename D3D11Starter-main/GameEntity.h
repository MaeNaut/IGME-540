#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

#include <memory>

class GameEntity
{
public:
	GameEntity() = default;
	GameEntity(std::shared_ptr<Mesh> _mesh);

	// Getters
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();

	// Draw Method
	void Draw(std::shared_ptr<Camera> camera);

private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

