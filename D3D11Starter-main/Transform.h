#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	// Setters
	void SetPosition(float x, float y, float z);
	// void SetPosition(DirectX::XMFLOAT3 pos);
	void SetRotation(float p, float y, float r);
	void SetScale(float x, float y, float z);

	// Transformers
	void MoveAbsolute(float x, float y, float z);
	// void MoveRelative(float x, float y, float z);
	void Rotate(float p, float y, float r);
	void Scale(float x, float y, float z);

	// Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();

	DirectX::XMFLOAT4X4 GetWorldMatrix();

private:
	// Transform data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	// Matrix
	bool dirty;
	DirectX::XMFLOAT4X4 worldMatrix;
};

