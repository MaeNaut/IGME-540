#include "GameEntity.h"
#include "Graphics.h"
#include "BufferStructs.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> _mesh)
{
	transform = std::make_shared<Transform>();
	mesh = _mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	// Collect the data locally
	DirectX::XMFLOAT4 colorTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	VertexShaderData dataToCopy = {};
	dataToCopy.colorTint = colorTint;
	dataToCopy.world = transform->GetWorldMatrix();
	dataToCopy.view = camera->GetView();
	dataToCopy.projection = camera->GetProjection();

	// Draw the entity
	mesh->Draw();
}
