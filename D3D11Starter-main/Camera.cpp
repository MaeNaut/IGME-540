#include "Camera.h"
#include "Input.h"

#include <numbers>
#include <algorithm>

using namespace std::numbers;

Camera::Camera(float aspectRatio, float x, float y, float z, float _fov, bool _isActive)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(x, y, z);
	fov = _fov;	// in degree; converted to radians when being passed into param
	isActive = _isActive;

	nearZ = 0.01f;
	farZ = 100.0f;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projection,
		DirectX::XMMatrixPerspectiveFovLH(fov / 180 * pi_v<float>, aspectRatio, nearZ, farZ));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 eyePos = transform->GetPosition();
	DirectX::XMFLOAT3 eyeDir = transform->GetForward();

	DirectX::XMStoreFloat4x4(&view,
		DirectX::XMMatrixLookToLH(
			DirectX::XMLoadFloat3(&eyePos),
			DirectX::XMLoadFloat3(&eyeDir),
			DirectX::XMVectorSet(0, 1, 0, 0)));
}

std::shared_ptr<Transform> Camera::GetTransform()
{
	return transform;
}

float Camera::GetFov()
{
	return fov;
}

float Camera::GetNearZ()
{
	return nearZ;
}

float Camera::GetFarZ()
{
	return farZ;
}

bool Camera::IsActive()
{
	return isActive;
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
	return view;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
	return projection;
}

void Camera::IsActive(bool _isActive)
{
	isActive = _isActive;
}

void Camera::Update(float deltaTime)
{
	// Keyboard Input
	if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, deltaTime * 5); }		// Forward
	if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -deltaTime * 5); }		// Backward
	if (Input::KeyDown('A')) { transform->MoveRelative(-deltaTime * 5, 0, 0); }		// Left
	if (Input::KeyDown('D')) { transform->MoveRelative(deltaTime * 5, 0, 0); }		// Right
	if (Input::KeyDown(VK_SPACE)) { transform->MoveAbsolute(0, deltaTime * 5, 0); }	// Up
	if (Input::KeyDown('X')) { transform->MoveAbsolute(0, -deltaTime * 5, 0); }		// Down

	// Mouse Input
	if (Input::MouseLeftDown())
	{
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();

		transform->Rotate(cursorMovementY / 200.0f, cursorMovementX / 200.0f, 0);
		
		float threshold = pi_v<float> / 2;
		if (transform->GetRotation().x > threshold || 
			transform->GetRotation().x < -threshold)
		{
			transform->SetRotation(
				std::clamp(transform->GetRotation().x, -threshold, threshold),
				transform->GetRotation().y,
				transform->GetRotation().z);
		}
	}

	UpdateViewMatrix();
}
