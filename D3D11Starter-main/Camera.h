#pragma once

#include "Transform.h"

#include <DirectXMath.h>
#include <memory>

class Camera
{
public:
	Camera() = default;
	Camera(float aspectRatio, float x, float y, float z, float _fov, bool _isActive);

	// Update Matrix
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float deltaTime);

	// Getters
	std::shared_ptr<Transform> GetTransform();
	float GetFov();
	float GetNearZ();
	float GetFarZ();
	bool IsActive();

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

	// Setters
	void IsActive(bool _isActive);

private:
	std::shared_ptr<Transform> transform;
	float fov;
	float nearZ;
	float farZ;
	bool isActive;

	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

