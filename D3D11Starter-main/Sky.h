#pragma once

#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>


class Sky
{
public:
	Sky(std::shared_ptr<Mesh> _mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler, std::wstring filePath);
	void Draw(std::shared_ptr<Camera> camera);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyDepthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterState;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};

