#include "renderer_dx10.hpp"
#include <kengine/core/graphics/renderer.hpp>
#include <d3d10.h>
#include <stdexcept>

namespace kengine::core::graphics::dx10 {

struct Vertex {
	float x, y, z;
};

class RendererDX10::Implementation {
private:
	ID3D10Device* device;
	// hello triangle temp stuff
	ID3D10Buffer* vertexBuffer;
	ID3D10Buffer* indexBuffer;
	ID3D10InputLayout* inputLayout;
	ID3D10VertexShader* vertexShader;
	ID3D10PixelShader* pixelShader;

public:
	Implementation() {
		if (D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, 0, D3D10_SDK_VERSION, &device) != S_OK) {
			throw RendererExceptionDX10("Failed to create D3D10 device");
		}

		// hello triangle temp stuff

		Vertex vertices[] = {
			{ 0.0f, 0.5f, 0.5f },
			{ 0.5f, -0.5f, 0.5f },
			{ -0.5f, -0.5f, 0.5f }
		};

		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D10_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA initData;
		initData.pSysMem = vertices;

		if (device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer) != S_OK) {
			throw RendererExceptionDX10("Failed to create vertex buffer");
		}

		DWORD indices[] = { 0, 1, 2 };
		bufferDesc.Usage = D3D10_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(indices);
		bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		initData.pSysMem = indices;

		if (device->CreateBuffer(&bufferDesc, &initData, &indexBuffer) != S_OK) {
			throw RendererExceptionDX10("Failed to create index buffer");
		}

		const char* shaderSource = R"(
			float4 vsmain(float4 position : POSITION) : SV_POSITION {
				return position;
			}

			float4 psmain(float4 position : SV_POSITION) : SV_TARGET {
				return float4(1.0f, 0.0f, 0.0f, 1.0f);
			}
		)";

		ID3D10Blob* shaderBlob;
		ID3D10Blob* errorBlob;
		if (D3D10CompileShader(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "vsmain", "vs_4_0", 0, &shaderBlob, &errorBlob) != S_OK) {
			std::string s = (char*) errorBlob->GetBufferPointer();
			errorBlob->Release();
			throw RendererExceptionDX10("Failed to compile vertex shader (error message: " + s + ")");
		}

		if (device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &vertexShader) != S_OK) {
			throw RendererExceptionDX10("Failed to create vertex shader");
		}

		D3D10_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (device->CreateInputLayout(layout, 1, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &inputLayout) != S_OK) {
			throw RendererExceptionDX10("Failed to create input layout");
		}
		shaderBlob->Release();

		if (D3D10CompileShader(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "psmain", "ps_4_0", 0, &shaderBlob, &errorBlob) != S_OK) {
			std::string s = (char*) errorBlob->GetBufferPointer();
			errorBlob->Release();
			throw RendererExceptionDX10("Failed to compile pixel shader (error message: " + s + ")");
		}

		if (device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &pixelShader) != S_OK) {
			throw RendererExceptionDX10("Failed to create pixel shader");
		}
		shaderBlob->Release();
	}

	~Implementation() {
		device->Release();
	}

	void render() {
		device->IASetInputLayout(inputLayout);
		device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		device->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		device->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		device->VSSetShader(vertexShader);
		device->PSSetShader(pixelShader);

		device->DrawIndexed(3, 0, 0);
	}
};

RendererDX10::RendererDX10() {
	impl = new RendererDX10::Implementation();
}

RendererDX10::~RendererDX10() {
	delete impl;
}

void RendererDX10::render() {
	impl->render();
}

} // namespace kengine::core::graphics::dx10