#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "Transform.h"


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
	LoadShaders();
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

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);


		// Create a constant buffer
		unsigned int bufferSize = sizeof(VertexShaderData);
		bufferSize = (bufferSize + 15) / 16 * 16;

		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = bufferSize;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());


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
	//transform = Transform();
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
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
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

	// First Mesh
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3(+0.0f, +0.25f, +0.0f), red },
			{ XMFLOAT3(+0.5f, -0.25f, +0.0f), blue },
			{ XMFLOAT3(-0.5f, -0.25f, +0.0f), green },
		};

		unsigned int indices[] = { 0, 1, 2 };

		int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
		int indexCount = sizeof(indices) / sizeof(indices[0]);

		mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
		meshes.push_back(mesh);
	}

	// Second Mesh
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3(-0.3f, -0.1f, +0.0f), red },
			{ XMFLOAT3(-0.3f, +0.1f, +0.0f), red },
			{ XMFLOAT3(+0.0f, -0.1f, +0.0f), blue },
			{ XMFLOAT3(+0.0f, +0.1f, +0.0f), blue },
			{ XMFLOAT3(+0.3f, -0.1f, +0.0f), green },
			{ XMFLOAT3(+0.3f, +0.1f, +0.0f), green },
		};

		unsigned int indices[] = 
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5
		};

		int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
		int indexCount = sizeof(indices) / sizeof(indices[0]);

		mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
		meshes.push_back(mesh);
	}

	// Third Mesh
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3(+0.0f, +0.3f, +0.0f), green },
			{ XMFLOAT3(-0.2f, +0.0f, +0.0f), black },
			{ XMFLOAT3(+0.2f, +0.0f, +0.0f), black },
			{ XMFLOAT3(+0.0f, +0.0f, +0.0f), black },
			{ XMFLOAT3(-0.2f, -0.3f, +0.0f), red },
			{ XMFLOAT3(+0.2f, -0.3f, +0.0f), blue },

		};

		unsigned int indices[] =
		{
			1, 0, 3,
			3, 0, 2,
			4, 1, 3,
			5, 3, 2
		};

		int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
		int indexCount = sizeof(indices) / sizeof(indices[0]);

		mesh = std::make_shared<Mesh>(vertices, indices, vertexCount, indexCount);
		meshes.push_back(mesh);
	}

	// Create, place, and push_back each entity
	entity = std::make_shared<GameEntity>(meshes[0]);
	entity->GetTransform()->SetPosition(-0.4f, -0.2f, 0.0f);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(meshes[1]);
	entity->GetTransform()->SetPosition(-0.1f, 0.5f, 0.0f);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(meshes[2]);
	entity->GetTransform()->SetPosition(0.3f, 0.1f, 0.0f);
	entities.push_back(entity);
	
	entity = std::make_shared<GameEntity>(meshes[2]);
	entity->GetTransform()->SetPosition(0.5f, -0.3f, 0.0f);
	entities.push_back(entity);

	entity = std::make_shared<GameEntity>(meshes[2]);
	entity->GetTransform()->SetPosition(0.7f, -0.7f, 0.0f);
	entities.push_back(entity);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
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

	// Transform
	float t = sin(totalTime * 10.0f) / 100.0f;
	entities[0]->GetTransform()->MoveAbsolute(t, 0.0f, 0.0f);

	float s = sin(totalTime) * 0.5f + 1.0f;
	entities[1]->GetTransform()->SetScale(s, s, s);

	for (int i = 2; i < entities.size(); i++)
	{
		entities[i]->GetTransform()->Rotate(0, 0, deltaTime);
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
		for (std::shared_ptr ge : entities)
		{
			ge->Draw(constantBuffer);
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

	if (ImGui::CollapsingHeader("Meshes"))
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			if (ImGui::TreeNode("", "Mesh %d", i + 1))
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

	if (ImGui::CollapsingHeader("Scene Entities"))
	{
		for (int i = 0; i < entities.size(); i++)
		{
			ImGui::PushID(id);
			id++;
			if (ImGui::TreeNode("", "Entity %d", i + 1))
			{
				float a = entities[i]->GetTransform()->GetPosition().x;
				ImGui::Text("position.x: %f", a);

				// -----* Error note *-----
				// Could not figure out how to convert XMFLOAT3 to float*
				// without lvalue error and data corruption
				
				//ImGui::DragFloat3("Position", &entities[i]->GetTransform()->GetPosition().x);
				//ImGui::DragFloat3("Rotation (Radians)", &entities[i]->GetTransform()->GetRotation().x);
				//ImGui::DragFloat3("Scale##3", &entities[i]->GetTransform()->GetScale().x);
				ImGui::Text("Mesh Index Count: %d", entities[i]->GetMesh()->GetIndexCount());
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	ImGui::End();
}