#include "Mesh.h"
#include "Graphics.h"

#include <DirectXMath.h>

using namespace DirectX;

Mesh::Mesh(Vertex vertices[], unsigned int indices[], int _vertexCount, int _indexCount)
{
	vertexCount = _vertexCount;
	indexCount = _indexCount;

	// Create a VERTEX BUFFER
	{
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices;

		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices;

		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

int Mesh::GetVertexCount()
{
	return vertexCount;
}

int Mesh::GetTriangleCount()
{
	return indexCount / 3;
}

void Mesh::Draw()
{
}
