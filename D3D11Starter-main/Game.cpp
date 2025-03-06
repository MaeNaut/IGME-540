#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Transform.h"
#include "Material.h"

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
//Transform transform;

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
	
	// Create the camera for game
	camera = std::make_shared<Camera>(Window::AspectRatio(), 0, 0, -20, 45, true);
	cameras.push_back(camera);
	camera = std::make_shared<Camera>(Window::AspectRatio(), 2, 0, -2, 60, false);
	cameras.push_back(camera);
	camera = std::make_shared<Camera>(Window::AspectRatio(), -1, 1, -2, 90, false);
	cameras.push_back(camera);
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
	std::shared_ptr<SimplePixelShader> psUV = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> psNormal = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> psCustom = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"CustomPS.cso").c_str());


	// Create materials, MUST be done before creating entities
	colorTint = { 1.0f, 0.0f, 1.0f, 1.0f };
	std::shared_ptr<Material> mat1 = std::make_shared<Material>(colorTint, vs, ps);

	colorTint = { 0.3f, 0.3f, 0.3f, 1.0f };
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(colorTint, vs, ps);

	colorTint = { 0.0f, 1.0f, 0.0f, 1.0f };
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(colorTint, vs, ps);

	colorTint = { 1.0f, 0.0f, 1.0f, 1.0f };
	std::shared_ptr<Material> matUV = std::make_shared<Material>(colorTint, vs, psUV);
	std::shared_ptr<Material> matNormal = std::make_shared<Material>(colorTint, vs, psNormal);
	std::shared_ptr<Material> matCustom = std::make_shared<Material>(colorTint, vs, psCustom);


	// Load meshes from obj files
	sphere = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str());
	meshes.push_back(sphere);
	cube = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());
	meshes.push_back(cube);
	helix = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str());
	meshes.push_back(helix);
	

	// Create entities
	entity = std::make_shared<GameEntity>(sphere, mat1);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, mat1);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, mat1);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(sphere, mat2);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, mat2);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, mat2);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(sphere, mat3);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, mat3);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, mat3);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(sphere, matUV);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, matUV);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, matUV);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(sphere, matNormal);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, matNormal);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, matNormal);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(sphere, matCustom);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(cube, matCustom);
	entities.push_back(entity);
	entity = std::make_shared<GameEntity>(helix, matCustom);
	entities.push_back(entity);


	// Set appropriate positions for each entity
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->GetTransform()->MoveAbsolute((i / 3) * 3, -(i % 3) * 3, 0);
	}
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
	for (int i = 0; i < entities.size(); i++)
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
	}

	// Entities
	{
		for (std::shared_ptr cam : cameras)
		{
			if (cam->IsActive())
			{
				for (std::shared_ptr ge : entities)
				{
					ge->Draw(cam);
				}
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
	if (ImGui::CollapsingHeader("Camera details"))
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

	ImGui::End();
}