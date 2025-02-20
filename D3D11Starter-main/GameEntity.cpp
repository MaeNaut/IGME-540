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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> &constantBuffer, std::shared_ptr<Camera> camera)
{
	// Actually bind the buffer
	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());


	// Collect the data locally
	DirectX::XMFLOAT4 colorTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	VertexShaderData dataToCopy = {};
	dataToCopy.colorTint = colorTint;
	dataToCopy.world = transform->GetWorldMatrix();
	dataToCopy.view = camera->GetView();
	dataToCopy.projection = camera->GetProjection();

	// Map() the buffer first
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Graphics::Context->Map(
		constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped);

	// Copy to GPU using memcpy
	memcpy(mapped.pData, &dataToCopy, sizeof(VertexShaderData));

	// Unmap when done
	Graphics::Context->Unmap(constantBuffer.Get(), 0);


	// Draw the entity
	mesh->Draw();
}
