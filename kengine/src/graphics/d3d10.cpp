#include <kengine/graphics/d3d10.hpp>
#include <kengine/logger.hpp>
#include <d3d10.h>

namespace kengine::graphics::d3d10 {

#define BACKBUFFER_COUNT 1

class RenderableD3D10 : public Renderable {
public:
	~RenderableD3D10() {
		if (vertexBuffer != nullptr) {
			vertexBuffer->Release();
		}

		if (indexBuffer != nullptr) {
			indexBuffer->Release();
		}
	}

	ID3D10Buffer* vertexBuffer;
	ID3D10Buffer* indexBuffer;
	UINT vertexCount;
	UINT indexCount;
	UINT renderableID;
};

class RendererD3D10 : public IRenderer {
public:
	virtual void init(Window& window, Camera& camera);
	virtual void render();
	virtual void destroy();

	virtual RendererSupport getSupport();

	virtual Renderable* createRenderable();
	virtual void renderableUploadMesh(Renderable* renderable, Mesh* mesh);
	virtual Mesh renderableDownloadMesh(Renderable* renderable);
	virtual void destroyRenderable(Renderable* renderable);

	virtual PostProcess* createPostProcess(const char* source, ShaderDescriptor shaderDescriptor);
	virtual void destroyPostProcess(PostProcess* postProcess);

	Window* window;
	ID3D10Device* device;
	IDXGISwapChain* swapChain;
	UINT frameIndex;
	ID3D10RenderTargetView* renderTargetViews[BACKBUFFER_COUNT];
	D3D10_VIEWPORT viewport;

	std::vector<RenderableD3D10*> renderables;
};

void getD3D10SupportedRenderers(std::vector<RendererSupport>& renderers) {
	renderers.push_back({ { RendererAPIType::D3D10, { 10, 0, 0 } }, false });
}

IRenderer* createD3D10Renderer() {
	RendererSupport support = {};
	support.api.api = RendererAPIType::D3D10;
	support.api.version.major = 10;
	support.api.version.minor = 0;
	support.api.version.patch = 0;

	return createD3D10RendererWithSupport(support);
}

IRenderer* createD3D10RendererWithSupport(RendererSupport renderer) {
	if (renderer.api.api != RendererAPIType::D3D10) {
		return nullptr;
	}

	if (renderer.api.version.major != 10) {
		return nullptr;
	}

	/* check d3d10 support */
	IDXGIFactory* factory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) {
		return nullptr;
	}

	IDXGIAdapter* adapter;
	ID3D10Device* device;
	for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		D3D10_DRIVER_TYPE driverTypes[] = { D3D10_DRIVER_TYPE_HARDWARE, D3D10_DRIVER_TYPE_WARP, D3D10_DRIVER_TYPE_REFERENCE };
		for (D3D10_DRIVER_TYPE driverType : driverTypes)
		{
			if (SUCCEEDED(D3D10CreateDevice(adapter, driverType, NULL, 0, D3D10_SDK_VERSION, &device))) {
				device->Release();
				adapter->Release();
				factory->Release();
				return new RendererD3D10();
			}
		}

		adapter->Release();
	}

	factory->Release();

	logger::printf(LogType::Error, "Failed to create RendererD3D10 (reason: D3D10 not supported)");
	return nullptr;
}

void destroyD3D10Renderer(IRenderer* renderer) {
	delete renderer;
}

void RendererD3D10::init(Window& window, Camera& camera) {
	this->window = &window;

	IDXGIFactory1* factory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) {
		throw std::runtime_error("Failed to create DXGI factory");
	}

	IDXGIAdapter1* adapter;
	UINT best = UINT_MAX;
	SIZE_T bestScore = 0;
	for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		SIZE_T score = 0;
		if (desc.VendorId == 0x10DE || desc.VendorId == 0x1002)	score += 1000000000;
		if (desc.VendorId == 0x8086)							score += 1000000;
		if (desc.VendorId == 0x1414)							score += 1000;

		score += desc.DedicatedVideoMemory / 1024;

		if (score > bestScore) {
			best = i;
			bestScore = score;
		}

		adapter->Release();
	}

	factory->EnumAdapters1(best, &adapter);

	UINT flags = 0;
	#ifdef _DEBUG
	flags |= D3D10_CREATE_DEVICE_DEBUG;
	#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = window.width;
	swapChainDesc.BufferDesc.Height = window.height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BACKBUFFER_COUNT;
	swapChainDesc.OutputWindow = static_cast<HWND>(window.getHandleWin32());
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	device = nullptr;
	D3D10_DRIVER_TYPE driverTypes[] = { D3D10_DRIVER_TYPE_HARDWARE, D3D10_DRIVER_TYPE_WARP, D3D10_DRIVER_TYPE_REFERENCE };
	for (D3D10_DRIVER_TYPE type : driverTypes) {
		if (SUCCEEDED(D3D10CreateDeviceAndSwapChain(adapter, type, NULL, flags, D3D10_SDK_VERSION, &swapChainDesc, &swapChain, &device))) {
			break;
		}
	}

	if (device == nullptr) {
		throw std::runtime_error("Failed to create D3D10 device");
	}

	adapter->Release();
	factory->Release();

	ID3D10Texture2D* backBuffer;
	for (UINT i = 0; i < BACKBUFFER_COUNT; ++i) {
		swapChain->GetBuffer(i, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		device->CreateRenderTargetView(backBuffer, NULL, &renderTargetViews[i]);
		backBuffer->Release();
	}

	viewport.Width = window.width;
	viewport.Height = window.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	device->RSSetViewports(1, &viewport);
}

void RendererD3D10::render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	device->OMSetRenderTargets(1, &renderTargetViews[frameIndex], NULL);
	device->ClearRenderTargetView(renderTargetViews[frameIndex], clearColor);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (RenderableD3D10* const& renderable : renderables) {
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		device->IASetVertexBuffers(0, 1, &renderable->vertexBuffer, &stride, &offset);
		if (renderable->indexBuffer != nullptr) {
			device->IASetIndexBuffer(renderable->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			device->DrawIndexed(renderable->indexCount, 0, 0);
		} else {
			device->Draw(renderable->vertexCount, 0);
		}
	}

	swapChain->Present(0, 0);
	frameIndex = (frameIndex + 1) % BACKBUFFER_COUNT;
}

void RendererD3D10::destroy() {
	for (RenderableD3D10* renderable : renderables) {
		delete renderable;
	}
    for (ID3D10RenderTargetView* renderTargetView : renderTargetViews) {
		if (renderTargetView != nullptr) {
			renderTargetView->Release();
		}
	}
	swapChain->Release();
	device->Release();
}

RendererSupport RendererD3D10::getSupport() {
	return { { RendererAPIType::D3D10, { 10, 0, 0 } } };
}

Renderable* RendererD3D10::createRenderable() {
	RenderableD3D10* renderable = new RenderableD3D10();
	renderable->renderableID = renderables.size();
	renderables.push_back(renderable);
	return renderable;
}

void RendererD3D10::renderableUploadMesh(Renderable* renderable, Mesh* mesh) {
	if (renderable == nullptr || mesh == nullptr) {
		return;
	}

	RenderableD3D10* renderableD3D10 = static_cast<RenderableD3D10*>(renderable);

	if (renderableD3D10->vertexBuffer != nullptr) {
		renderableD3D10->vertexBuffer->Release();
		renderableD3D10->vertexBuffer = nullptr;
	}

	if (renderableD3D10->indexBuffer != nullptr) {
		renderableD3D10->indexBuffer->Release();
		renderableD3D10->indexBuffer = nullptr;
	}

	D3D10_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * mesh->vertices.size();
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D10_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = mesh->vertices.data();

	device->CreateBuffer(&bufferDesc, &initData, &renderableD3D10->vertexBuffer);
	renderableD3D10->vertexCount = mesh->vertices.size();

	if (!mesh->indices.empty()) {
		bufferDesc.ByteWidth = sizeof(UINT) * mesh->indices.size();
		bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;

		initData.pSysMem = mesh->indices.data();

		device->CreateBuffer(&bufferDesc, &initData, &renderableD3D10->indexBuffer);
		renderableD3D10->indexCount = mesh->indices.size();
	}
}

Mesh RendererD3D10::renderableDownloadMesh(Renderable* renderable) {
	return Mesh();
}

void RendererD3D10::destroyRenderable(Renderable* renderable) {
	if (renderable == nullptr) {
		return;
	}

	RenderableD3D10* r = static_cast<RenderableD3D10*>(renderable);
	renderables.erase(renderables.begin() + r->renderableID);

	delete r;
}

PostProcess* RendererD3D10::createPostProcess(const char* source, ShaderDescriptor shaderDescriptor) {
	return nullptr;
}

void RendererD3D10::destroyPostProcess(PostProcess* postProcess) {

}

} // namespace kengine::graphics::d3d10