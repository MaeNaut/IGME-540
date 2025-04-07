#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	// Setters
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 _position);
	void SetRotation(float p, float y, float r);
	void SetRotation(DirectX::XMFLOAT3 _rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 _scale);

	// Transformers
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 _offset);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(DirectX::XMFLOAT3 _offset);
	void Rotate(float p, float y, float r);
	void Rotate(DirectX::XMFLOAT3 _rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 _scale);

	// Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	void CalculateWorldMatrix();

	// Directions
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

private:
	// Transform data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	// Matrix
	bool dirty;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
};

