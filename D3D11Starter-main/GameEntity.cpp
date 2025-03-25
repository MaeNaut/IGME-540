#include "GameEntity.h"
#include "Graphics.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> _mesh, std::shared_ptr<Material> _material)
{
	transform = std::make_shared<Transform>();
	mesh = _mesh;
	material = _material;
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

void GameEntity::SetMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
}

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	// Collect the data locally
	DirectX::XMFLOAT4 colorTint = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Activate the shader for entity
	material->GetVS()->SetShader();
	material->GetPS()->SetShader();

	// Prepare the data for GPU
	std::shared_ptr<SimpleVertexShader> vs = material->GetVS();
	std::shared_ptr<SimplePixelShader> ps = material->GetPS();

	vs->SetMatrix4x4("world", transform->GetWorldMatrix());
	vs->SetMatrix4x4("view", camera->GetView());
	vs->SetMatrix4x4("projection", camera->GetProjection());

	vs->CopyAllBufferData(); // Adjust the “vs” variable here as necessary

	// Same for the pixel shader
	ps->SetFloat4("colorTint", material->GetColorTint());
	ps->SetFloat2("scale", material->GetScale());
	ps->SetFloat2("offset", material->GetOffset());
	ps->CopyAllBufferData();

	// Draw the entity
	mesh->Draw();
}
