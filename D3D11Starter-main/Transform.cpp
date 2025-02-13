#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1, 1, 1),
	dirty(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix,
		XMMatrixInverse(0, XMMatrixTranspose(XMMatrixIdentity())));
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 _position)
{
	position = _position;
	dirty = true;
}

void Transform::SetRotation(float p, float y, float r)
{
	pitchYawRoll.x = p;
	pitchYawRoll.y = y;
	pitchYawRoll.z = r;
	dirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 _rotation)
{
	pitchYawRoll = _rotation;
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 _scale)
{
	scale = _scale;
	dirty = true;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMStoreFloat3(&position,
		XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0));
	dirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 _offset)
{
	XMStoreFloat3(&position,
		XMLoadFloat3(&position) + XMLoadFloat3(&_offset));
	dirty = true;
}

void Transform::Rotate(float p, float y, float r)
{
	pitchYawRoll.x += p;
	pitchYawRoll.y += y;
	pitchYawRoll.z += r;
	dirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 _rotation)
{
	pitchYawRoll.x += _rotation.x;
	pitchYawRoll.y += _rotation.y;
	pitchYawRoll.z += _rotation.z;
	dirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	dirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 _scale)
{
	scale.x *= _scale.x;
	scale.y *= _scale.y;
	scale.z *= _scale.z;
	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty)
	{
		// Make translation, rotation, and scale matrices
		// XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		XMMATRIX trMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX scMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

		// World matrix is a combination of translation, rotation, and scale matrices
		XMMATRIX world = scMatrix * rotMatrix * trMatrix;
		XMStoreFloat4x4(&worldMatrix, world);
		dirty = false;
	}

	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (dirty)
	{
		// Make translation, rotation, and scale matrices
		// XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		XMMATRIX trMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX scMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

		// World matrix is a combination of translation, rotation, and scale matrices
		XMMATRIX world = scMatrix * rotMatrix * trMatrix;
		XMStoreFloat4x4(&worldInverseTransposeMatrix,
			XMMatrixInverse(0, XMMatrixTranspose(world)));
		dirty = false;
	}

	return worldInverseTransposeMatrix;
}
