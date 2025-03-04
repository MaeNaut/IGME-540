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
	camera = std::make_shared<Camera>(Window::AspectRatio(), 0, 0, -5, 45, true);
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
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	//// First Mesh
	//{
	//	Vertex vertices[] =
	//	{
	//		{ XMFLOAT3(+0.0f, +0.25f, +0.0f), red },
	//		{ XMFLOAT3(+0.5f, -0.25f, +0.0f), blue },
	//		{ XMFLOAT3(-0.5f, -0.25f, +0.0f), green },
	//	};

	//	unsigned int indices[] = { 0, 1, 2 };

	//	int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
	//	int indexCount = sizeof(indices) / sizeof(indices[0]);

	//	mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
	//	meshes.push_back(mesh);
	//}

	//// Second Mesh
	//{
	//	Vertex vertices[] =
	//	{
	//		{ XMFLOAT3(-0.3f, -0.1f, +0.0f), red },
	//		{ XMFLOAT3(-0.3f, +0.1f, +0.0f), red },
	//		{ XMFLOAT3(+0.0f, -0.1f, +0.0f), blue },
	//		{ XMFLOAT3(+0.0f, +0.1f, +0.0f), blue },
	//		{ XMFLOAT3(+0.3f, -0.1f, +0.0f), green },
	//		{ XMFLOAT3(+0.3f, +0.1f, +0.0f), green },
	//	};

	//	unsigned int indices[] = 
	//	{
	//		0, 1, 2,
	//		2, 1, 3,
	//		2, 3, 4,
	//		4, 3, 5
	//	};

	//	int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
	//	int indexCount = sizeof(indices) / sizeof(indices[0]);

	//	mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
	//	meshes.push_back(mesh);
	//}

	//// Third Mesh
	//{
	//	Vertex vertices[] =
	//	{
	//		{ XMFLOAT3(+0.0f, +0.3f, +0.0f), green },
	//		{ XMFLOAT3(-0.2f, +0.0f, +0.0f), black },
	//		{ XMFLOAT3(+0.2f, +0.0f, +0.0f), black },
	//		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), black },
	//		{ XMFLOAT3(-0.2f, -0.3f, +0.0f), red },
	//		{ XMFLOAT3(+0.2f, -0.3f, +0.0f), blue },

	//	};

	//	unsigned int indices[] =
	//	{
	//		1, 0, 3,
	//		3, 0, 2,
	//		4, 1, 3,
	//		5, 3, 2
	//	};

	//	int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
	//	int indexCount = sizeof(indices) / sizeof(indices[0]);

	//	mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
	//	meshes.push_back(mesh);
	//}

	// Load shaders
	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());


	// Create materials, MUST be done before creating entities
	DirectX::XMFLOAT4 colorTint(1.0f, 1.0f, 1.0f, 1.0f);
	std::shared_ptr<Material> mat = std::make_shared<Material>(colorTint, vs, ps);

	

	entity = std::make_shared<GameEntity>(std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str()), mat);
	entities.push_back(entity);

	//// Create, place, and push_back each entity
	//entity = std::make_shared<GameEntity>(meshes[0], mat);
	//entity->GetTransform()->SetPosition(-0.4f, -0.2f, 0.0f);
	//entities.push_back(entity);

	//entity = std::make_shared<GameEntity>(meshes[1], mat);
	//entity->GetTransform()->SetPosition(-0.1f, 0.5f, 0.0f);
	//entities.push_back(entity);

	//entity = std::make_shared<GameEntity>(meshes[2], mat);
	//entity->GetTransform()->SetPosition(0.3f, 0.1f, 0.0f);
	//entities.push_back(entity);
	//
	//entity = std::make_shared<GameEntity>(meshes[2], mat);
	//entity->GetTransform()->SetPosition(0.5f, -0.3f, 0.0f);
	//entities.push_back(entity);

	//entity = std::make_shared<GameEntity>(meshes[2], mat);
	//entity->GetTransform()->SetPosition(0.7f, -0.7f, 0.0f);
	//entities.push_back(entity);
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

	//// Transform
	//float t = sin(totalTime * 10.0f) / 100.0f;
	//entities[0]->GetTransform()->MoveAbsolute(t, 0.0f, 0.0f);

	//float s = sin(totalTime) * 0.5f + 1.0f;
	//entities[1]->GetTransform()->SetScale(s, s, s);

	//for (int i = 2; i < entities.size(); i++)
	//{
	//	entities[i]->GetTransform()->Rotate(0, 0, deltaTime);
	//}

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
					//std::shared_ptr<SimpleVertexShader> vs = ge->GetMaterial()->GetVS();
					//std::shared_ptr<SimplePixelShader> ps = ge->GetMaterial()->GetPS();
					//
					//// Set up the pipeline for this entity (and mesh)
					//vs->SetShader();
					//ps->SetShader();

					//// Collect data for the vertex shader
					//vs->SetMatrix4x4("world", ge->GetTransform()->GetWorldMatrix());
					//vs->SetMatrix4x4("view", cam->GetView());
					//vs->SetMatrix4x4("projection", cam->GetProjection());

					//// Copy data to constant buffers
					//vs->CopyAllBufferData();

					//// Same for the pixel shader
					//ps->SetFloat("Time", totalTime);
					//ps->CopyAllBufferData();

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

	//if (ImGui::CollapsingHeader("Constant Buffer"))
	//{
	//	ImGui::DragFloat3("Offset", &offset.x, 0.05f, 0.0f, 0.0f, "%.2f", 0);
	//	ImGui::ColorEdit4("Mesh Color Tint", &colorTint.x);
	//}

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