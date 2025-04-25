#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Transform.h"
#include "Material.h"
#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Convert the name of the variable to string
#define GET_VARIABLE_NAME(Variable) (#Variable)

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();
	CreateLight();
	CreateShadow();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();
	}
	

	// Create the cameras for game
	camera = std::make_shared<Camera>(Window::AspectRatio(), 0.0f, 0.0f, -15.0f, 45.0f, true);
	cameras.push_back(camera);
	camera = std::make_shared<Camera>(Window::AspectRatio(), 2.0f, 0.0f, -2.0f, 60.0f, false);
	cameras.push_back(camera);
	camera = std::make_shared<Camera>(Window::AspectRatio(), -1.0f, 1.0f, -2.0f, 90.0f, false);
	cameras.push_back(camera);


	// Initialize some fields here
	ambientColor = { 0.1f, 0.1f, 0.25f };
	skyboxColor = { 1.0f, 1.0f, 1.0f };
	light = {};
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Load shaders, MUST be done before creating materials
	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());


	// Load textures
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/bronze_albedo.png").c_str(), nullptr, bronzeAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone_albedo.png").c_str(), nullptr, cobblestoneAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/floor_albedo.png").c_str(), nullptr, floorAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/paint_albedo.png").c_str(), nullptr, paintAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/rough_albedo.png").c_str(), nullptr, roughAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/scratched_albedo.png").c_str(), nullptr, scratchedAlbedo.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/wood_albedo.png").c_str(), nullptr, woodAlbedo.GetAddressOf());

	// Load normal maps
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/bronze_normals.png").c_str(), nullptr, bronzeNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str(), nullptr, cobblestoneNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/floor_normals.png").c_str(), nullptr, floorNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/paint_normals.png").c_str(), nullptr, paintNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/rough_normals.png").c_str(), nullptr, roughNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/scratched_normals.png").c_str(), nullptr, scratchedNormal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/wood_normals.png").c_str(), nullptr, woodNormal.GetAddressOf());

	// Load roughness maps
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/bronze_roughness.png").c_str(), nullptr, bronzeRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone_roughness.png").c_str(), nullptr, cobblestoneRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/floor_roughness.png").c_str(), nullptr, floorRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/paint_roughness.png").c_str(), nullptr, paintRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/rough_roughness.png").c_str(), nullptr, roughRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/scratched_roughness.png").c_str(), nullptr, scratchedRoughness.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/wood_roughness.png").c_str(), nullptr, woodRoughness.GetAddressOf());

	// Load metalness maps
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/bronze_metal.png").c_str(), nullptr, bronzeMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone_metal.png").c_str(), nullptr, cobblestoneMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/floor_metal.png").c_str(), nullptr, floorMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/paint_metal.png").c_str(), nullptr, paintMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/rough_metal.png").c_str(), nullptr, roughMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/scratched_metal.png").c_str(), nullptr, scratchedMetal.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/wood_metal.png").c_str(), nullptr, woodMetal.GetAddressOf());


	// Create a sampler
	D3D11_SAMPLER_DESC sampDesc{};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());


	// Create materials, MUST be done before creating entities
	colorTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::shared_ptr<Material> mat1 = std::make_shared<Material>(colorTint, vs, ps, 0.99f);
	mat1->AddTextureSRV("Albedo", bronzeAlbedo);
	mat1->AddTextureSRV("NormalMap", bronzeNormal);
	mat1->AddTextureSRV("RoughnessMap", bronzeRoughness);
	mat1->AddTextureSRV("MetalnessMap", bronzeMetal);
	mat1->AddSampler("BasicSampler", sampler);
	materials.push_back(mat1);

	std::shared_ptr<Material> mat2 = std::make_shared<Material>(colorTint, vs, ps, 0.1f);
	mat2->AddTextureSRV("Albedo", cobblestoneAlbedo);
	mat2->AddTextureSRV("NormalMap", cobblestoneNormal);
	mat2->AddTextureSRV("RoughnessMap", cobblestoneRoughness);
	mat2->AddTextureSRV("MetalnessMap", cobblestoneMetal);
	mat2->AddSampler("BasicSampler", sampler);
	materials.push_back(mat2);

	std::shared_ptr<Material> mat3 = std::make_shared<Material>(colorTint, vs, ps, 0.99f);
	mat3->AddTextureSRV("Albedo", floorAlbedo);
	mat3->AddTextureSRV("NormalMap", floorNormal);
	mat3->AddTextureSRV("RoughnessMap", floorRoughness);
	mat3->AddTextureSRV("MetalnessMap", floorMetal);
	mat3->AddSampler("BasicSampler", sampler);
	materials.push_back(mat3);

	std::shared_ptr<Material> mat4 = std::make_shared<Material>(colorTint, vs, ps, 0.1f);
	mat4->AddTextureSRV("Albedo", paintAlbedo);
	mat4->AddTextureSRV("NormalMap", paintNormal);
	mat4->AddTextureSRV("RoughnessMap", paintRoughness);
	mat4->AddTextureSRV("MetalnessMap", paintMetal);
	mat4->AddSampler("BasicSampler", sampler);
	materials.push_back(mat4);

	std::shared_ptr<Material> mat5 = std::make_shared<Material>(colorTint, vs, ps, 0.1f);
	mat5->AddTextureSRV("Albedo", roughAlbedo);
	mat5->AddTextureSRV("NormalMap", roughNormal);
	mat5->AddTextureSRV("RoughnessMap", roughRoughness);
	mat5->AddTextureSRV("MetalnessMap", roughMetal);
	mat5->AddSampler("BasicSampler", sampler);
	materials.push_back(mat5);

	std::shared_ptr<Material> mat6 = std::make_shared<Material>(colorTint, vs, ps, 0.99f);
	mat6->AddTextureSRV("Albedo", scratchedAlbedo);
	mat6->AddTextureSRV("NormalMap", scratchedNormal);
	mat6->AddTextureSRV("RoughnessMap", scratchedRoughness);
	mat6->AddTextureSRV("MetalnessMap", scratchedMetal);
	mat6->AddSampler("BasicSampler", sampler);
	materials.push_back(mat6);

	std::shared_ptr<Material> mat7 = std::make_shared<Material>(colorTint, vs, ps, 0.1f);
	mat7->AddTextureSRV("Albedo", woodAlbedo);
	mat7->AddTextureSRV("NormalMap", woodNormal);
	mat7->AddTextureSRV("RoughnessMap", woodRoughness);
	mat7->AddTextureSRV("MetalnessMap", woodMetal);
	mat7->AddSampler("BasicSampler", sampler);
	materials.push_back(mat7);


	// Load meshes from obj files
	cube = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());
	meshes.push_back(cube);
	cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Models/cylinder.obj").c_str());
	meshes.push_back(cylinder);
	helix = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str());
	meshes.push_back(helix);
	quad = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad.obj").c_str());
	meshes.push_back(quad);
	quad_double_sided = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad_double_sided.obj").c_str());
	meshes.push_back(quad_double_sided);
	sphere = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str());
	meshes.push_back(sphere);
	torus = std::make_shared<Mesh>(FixPath("../../Assets/Models/torus.obj").c_str());
	meshes.push_back(torus);

	// Create entities
	entity = std::make_shared<GameEntity>(cube, mat1);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cylinder, mat2);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, mat3);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(quad, mat4);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(quad_double_sided, mat5);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(sphere, mat6);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(torus, mat7);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(quad_double_sided, mat7);
	entities.push_back(entity);


	// Set appropriate positions for each entity
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->GetTransform()->MoveAbsolute(i * 3.0f - entities.size() + 1.5f, 0.0f, 0.0f);
	}

	entities[7]->GetTransform()->SetPosition(0.0f, -2.0f, 0.0f);
	entities[7]->GetTransform()->SetScale(20.0f, 20.0f, 20.0f);


	// Create Skybox
	skybox = std::make_shared<Sky>(cube, sampler, FixPath(L"../../Assets/Skybox/").c_str());
}


// --------------------------------------------------------
// Creates the lights for the world
// --------------------------------------------------------
void Game::CreateLight()
{
	// Initialize the directional light's direction
	// Used for shadows later
	directionalLightDirection = { 1.0f, -1.0f, 0.0f };

	// Create the lights
	light.Type = 0;
	light.Direction = directionalLightDirection;
	light.Color = { 1.0f, 1.0f, 1.0f };
	light.Intensity = 1.0f;
	lights.push_back(light);

	light.Type = 1;
	light.Range = 20.0f;
	light.Position = { 0.0f, 3.0f, -3.0f };
	light.Color = { 1.0f, 1.0f, 1.0f };
	light.Intensity = 1.0f;
	lights.push_back(light);
}


// --------------------------------------------------------
// Creates the shadows that will be
// casted using the lights and meshes
// --------------------------------------------------------
void Game::CreateShadow()
{
	// Initialize some fields
	lightDirection = DirectX::XMLoadFloat3(&directionalLightDirection);
	lightProjectionSize = 15.0f;
	shadowMapResolution = 2048;


	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution;		// Ideally a power of 2 (like 1024)
	shadowDesc.Height = shadowMapResolution;	// Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	// Create the shadow rasterizer state for depth biasing
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// Create the shadow sampler state
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);


	// Create the light view matrix
	XMMATRIX lightView = XMMatrixLookToLH(
		-lightDirection * 20,		// Position: "Backing up" 20 units from origin
		lightDirection,				// Direction: light's direction
		XMVectorSet(0, 1, 0, 0));	// Up: World up vector (Y axis)
	XMStoreFloat4x4(&lightViewMatrix, lightView);

	// Create the light projection matrix
	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);
	XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (std::shared_ptr cam : cameras)
	{
		if (camera != nullptr) { camera->UpdateProjectionMatrix(Window::AspectRatio()); }
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	ImGuiUpdate(deltaTime);
	CustomUI();

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	// Rotate the entities every frame
	//for (std::shared_ptr<GameEntity> ge : entities)
	//{
	//	ge->GetTransform()->Rotate(0, deltaTime, 0);
	//}
	for (int i = 0; i < entities.size() - 1; i++)
	{
		entities[i]->GetTransform()->Rotate(0, deltaTime, 0);
	}

	// Camera
	for (std::shared_ptr cam : cameras)
	{
		if (cam->IsActive())
		{
			cam->Update(deltaTime);
		}
	}
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		float color[4] = { bgColor.x, bgColor.y, bgColor.z, bgColor.w };
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);


		// Clear the shadow map
		Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		
		// Set up the output merger stage
		ID3D11RenderTargetView* nullRTV{};
		Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());

		// Enable the shadow rasterizer state
		Graphics::Context->RSSetState(shadowRasterizer.Get());

		// Deactivate pixel shader
		Graphics::Context->PSSetShader(0, 0, 0);

		// Change viewpoint
		D3D11_VIEWPORT viewport = {};
		viewport.Width = (float)shadowMapResolution;
		viewport.Height = (float)shadowMapResolution;
		viewport.MaxDepth = 1.0f;
		Graphics::Context->RSSetViewports(1, &viewport);

		// Define shadow vertex shader
		std::shared_ptr<SimpleVertexShader> shadowVS = std::make_shared<SimpleVertexShader>(
			Graphics::Device, Graphics::Context, FixPath(L"ShadowVS.cso").c_str());
		shadowVS->SetShader();
		shadowVS->SetMatrix4x4("view", lightViewMatrix);
		shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);

		// Loop and draw all entities
		for (auto& e : entities)
		{
			shadowVS->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
			shadowVS->CopyAllBufferData();
			// Draw the mesh directly to avoid the entity's material
			// Note: Your code may differ significantly here!
			e->GetMesh()->Draw();
		}

		// Reset the pipeline
		viewport.Width = (float)Window::Width();
		viewport.Height = (float)Window::Height();
		Graphics::Context->RSSetViewports(1, &viewport);
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());

		// Disable the shadow rasterizer state
		Graphics::Context->RSSetState(0);
	}

	// Draw Geometry
	{
		// Entities & Skybox
		for (std::shared_ptr cam : cameras)
		{
			if (cam->IsActive())
			{
				for (std::shared_ptr ge : entities)
				{
					ge->GetMaterial()->GetPS()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
					ge->GetMaterial()->GetPS()->SetInt("lightCount", (int)lights.size());
					ge->GetMaterial()->GetVS()->SetMatrix4x4("lightView", lightViewMatrix);
					ge->GetMaterial()->GetVS()->SetMatrix4x4("lightProjection", lightProjectionMatrix);
					ge->GetMaterial()->AddTextureSRV("ShadowMap", shadowSRV);
					ge->GetMaterial()->AddSampler("ShadowSampler", shadowSampler);
					ge->GetMaterial()->BindResources();
					ge->Draw(cam);
				}

				skybox->Draw(cam);
			}
		}
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());

		// Unbind the SRVs
		ID3D11ShaderResourceView* nullSRVs[128] = {};
		Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);
	}
}

// --------------------------------------------------------
// Update the UI with a fresh data
// --------------------------------------------------------
void Game::ImGuiUpdate(float deltaTime)
{
	// Put this all in a helper method that is called from Game::Update()
	
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Show the demo window
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}

// --------------------------------------------------------
// My Custom UI for Dear ImGui
// --------------------------------------------------------
void Game::CustomUI()
{
	// Open the Custom window with a name of Inspector
	ImGui::Begin("Inspector");
	int id = 0;

	// Display app details
	if (ImGui::CollapsingHeader("App Details"))
	{
		// Show the framerate and window dimensions
		ImGui::Text("Frame rate: %f fps", ImGui::GetIO().Framerate);
		ImGui::Text("Window Client Size: %dx%d", Window::Width(), Window::Height());

		// Background color editor
		ImGui::ColorEdit4("Background Color", &bgColor.x);

		// Open or close the demo window
		if (ImGui::Button("Show ImGui Demo Window"))
		{
			if (show_demo_window == false)
			{
				show_demo_window = true;
			}
			else
			{
				show_demo_window = false;
			}
		}

		// ZIP Code slider
		ImGui::SliderInt("ZIP Code", &zipCode, 00501, 99734);
	}

	// Mesh UI
	if (ImGui::CollapsingHeader("Meshes"))
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			if (ImGui::TreeNode("", "Mesh %d", i))
			{
				ImGui::Text("Triangles: %d", meshes[i]->GetTriangleCount());
				ImGui::Text("Vertices:  %d", meshes[i]->GetVertexCount());
				ImGui::Text("Indices:   %d", meshes[i]->GetIndexCount());
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	// Entity UI
	if (ImGui::CollapsingHeader("Scene Entities"))
	{
		for (int i = 0; i < entities.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			if (ImGui::TreeNode("", "Entity %d", i))
			{
				XMFLOAT3 position = entities[i]->GetTransform()->GetPosition();
				XMFLOAT3 rotation = entities[i]->GetTransform()->GetRotation();
				XMFLOAT3 scale = entities[i]->GetTransform()->GetScale();

				if (ImGui::DragFloat3("Position", &position.x, 0.005f))
				{
					entities[i]->GetTransform()->SetPosition(position);
				}
				if (ImGui::DragFloat3("Rotation (Radians)", &rotation.x, 0.005f))
				{
					entities[i]->GetTransform()->SetRotation(rotation);
				}
				if (ImGui::DragFloat3("Scale", &scale.x, 0.005f))
				{
					entities[i]->GetTransform()->SetScale(scale);
				}

				ImGui::Text("Mesh Index Count: %d", entities[i]->GetMesh()->GetIndexCount());
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	// Camera UI
	if (ImGui::CollapsingHeader("Camera Details"))
	{
		// Choose which camera to be activated
		static int e = 0;
		if (ImGui::RadioButton("Camera 0", &e, 0))
		{
			cameras[0]->IsActive(true);
			cameras[1]->IsActive(false);
			cameras[2]->IsActive(false);
		}	ImGui::SameLine();
		if (ImGui::RadioButton("Camera 1", &e, 1))
		{
			cameras[0]->IsActive(false);
			cameras[1]->IsActive(true);
			cameras[2]->IsActive(false);
		}	ImGui::SameLine();
		if (ImGui::RadioButton("Camera 2", &e, 2))
		{
			cameras[0]->IsActive(false);
			cameras[1]->IsActive(false);
			cameras[2]->IsActive(true);
		}

		// Display the information of activated camera
		for (std::shared_ptr cam : cameras)
		{
			if (cam->IsActive())
			{
				XMFLOAT3 position = cam->GetTransform()->GetPosition();
				XMFLOAT3 rotation = cam->GetTransform()->GetRotation();

				ImGui::Text("Position:     %.3f, %.3f, %.3f", position.x, position.y, position.z);
				ImGui::Text("Direction:    %.3f, %.3f, %.3f", rotation.x, rotation.y, rotation.z);
				ImGui::Text("FOV (Degree): %.3f", cam->GetFov());
				ImGui::Text("Near Plane:   %.3f", cam->GetNearZ());
				ImGui::Text("Far Plane:    %.3f", cam->GetFarZ());
			}
		}
	}

	// Material UI
	if (ImGui::CollapsingHeader("Material Details"))
	{
		// Iterate through material vector
		for (int i = 0; i < materials.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			// Create tree node for each material
			if (ImGui::TreeNode("", "Material %d", i))
			{
				// Show an image of the texture
				// (using for loop for materials with multiple textures)
				for (auto& t : materials[i]->GetSRV())
				{
					ImGui::Image((ImTextureID)t.second.Get(), ImVec2(128, 128));
				}

				XMFLOAT4 colorTint = materials[i]->GetColorTint();
				XMFLOAT2 scale = materials[i]->GetScale();
				XMFLOAT2 offset = materials[i]->GetOffset();
				
				// Drag Widgets to modify material's color tint, scale, and offset
				ImGui::ColorEdit4("Color Tint", &colorTint.x);
				materials[i]->SetColorTint(colorTint);
				if (ImGui::DragFloat2("Scale", &scale.x, 0.005f))
				{
					materials[i]->SetScale(scale);
				}
				if (ImGui::DragFloat2("Offset", &offset.x, 0.005f))
				{
					materials[i]->SetOffset(offset);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	// Light UI
	if (ImGui::CollapsingHeader("Light Details"))
	{
		// Allow the user to adjust the color of ambient
		if (ImGui::DragFloat3("Ambient", &ambientColor.x, 0.001f, 0.0f, 1.0f)) {}
		// Iterate through light vector
		for (int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			// Create tree node for each light
			if (ImGui::TreeNode("", "Light %d", i))
			{
				// Display the type of light
				switch (lights[i].Type)
				{
					case 0:
						ImGui::Text("Light Type: Directional");
						break;
					case 1:
						ImGui::Text("Light Type: Point");
						break;
					case 2:
						ImGui::Text("Light Type: Spot");
						break;
				}

				// Modify the data of each light
				ImGui::ColorEdit3("Color", &lights[i].Color.x);
				if (ImGui::DragFloat3("Position", &lights[i].Position.x, 0.005f)) {}
				if (ImGui::DragFloat("Range", &lights[i].Range, 0.005f)) {}
				if (ImGui::DragFloat("Intensity", &lights[i].Intensity, 0.005f)) {}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	// Shadow Map UI
	if (ImGui::CollapsingHeader("Shadow Map"))
	{
		ImGui::Image((ImTextureID)shadowSRV.Get(), ImVec2(512, 512));
	}

	ImGui::End();
}