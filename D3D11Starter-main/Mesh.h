#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount;
	int vertexCount;

public:
	Mesh() = default;
	Mesh(Vertex vertices[], unsigned int indices[], int _vertexCount, int _indexCount);
	Mesh(const char* objFile);
	~Mesh();

	// Getters
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	int GetTriangleCount();

	// Helper function
	void CreateBuffer(Vertex vertices[], unsigned int indices[]);

	void Draw();
};

