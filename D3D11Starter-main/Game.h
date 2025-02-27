#pragma once

#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();
	void ImGuiUpdate(float deltaTime);
	void CustomUI();

private:
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr


	// fields for UI
	bool show_demo_window = false;
	int zipCode = 00501;
	DirectX::XMFLOAT4 bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 offset = { 0.2f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 colorTint = { 1.0f, 0.5f, 0.5f, 1.0f };

	// Mesh fields
	std::shared_ptr<Mesh> mesh;
	std::vector<std::shared_ptr<Mesh>> meshes;

	// Entity fields
	std::shared_ptr<GameEntity> entity;
	std::vector<std::shared_ptr<GameEntity>> entities;

	// Camera fields
	std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<Camera>> cameras;
};
