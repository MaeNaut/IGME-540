#pragma once

#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"

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
	void CreateLight();
	void CreateShadow();

	// Fields for UI
	bool show_demo_window = false;
	int zipCode = 00501;
	DirectX::XMFLOAT4 bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 colorTint;

	// Mesh fields
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> quad_double_sided;
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;
	std::vector<std::shared_ptr<Mesh>> meshes;

	// Entity fields
	std::shared_ptr<GameEntity> entity;
	std::vector<std::shared_ptr<GameEntity>> entities;

	// Camera fields
	std::shared_ptr<Camera> camera;
	std::vector<std::shared_ptr<Camera>> cameras;

	// Fields for textures
	std::vector<std::shared_ptr<Material>> materials;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	// Albedos
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodAlbedo;

	// Normal maps
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodNormal;

	// Roughness maps
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneRoughness ;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodRoughness;

	// Metal maps
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodMetal;

	// Fields for lighting
	DirectX::XMFLOAT3 ambientColor;
	DirectX::XMFLOAT3 skyboxColor;
	Light light;
	std::vector<Light> lights;
	DirectX::XMFLOAT3 directionalLightDirection;

	// Fields for skybox
	std::shared_ptr<Sky> skybox;

	// Shadow mapping resources
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	DirectX::XMVECTOR lightDirection;
	float lightProjectionSize;
	int shadowMapResolution;
};
