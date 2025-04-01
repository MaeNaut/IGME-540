#pragma once

#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"

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

	// Fields for UI
	bool show_demo_window = false;
	int zipCode = 00501;
	DirectX::XMFLOAT4 bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 colorTint;

	// Mesh fields
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> torus;
	std::shared_ptr<Mesh> quad;
	std::vector<std::shared_ptr<Mesh>> meshes;

	// Entity fields
	std::shared_ptr<GameEntity> entity;
	std::vector<std::shared_ptr<GameEntity>> entities;

	// Camera fields
	std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<Camera>> cameras;

	// Fields for textures
	std::vector<std::shared_ptr<Material>> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> onyxTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	// Fields for lighting
	DirectX::XMFLOAT3 ambientColor;
	Light light;
	std::vector<Light> lights;
};
