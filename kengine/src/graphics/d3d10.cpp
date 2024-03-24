#include <kengine/graphics/d3d10.hpp>

namespace kengine::graphics::d3d10 {

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

	return new RendererD3D10();
}

void destroyD3D10Renderer(IRenderer* renderer) {
	delete renderer;
}

void RendererD3D10::init(Window& window, Camera& camera) {
	this->window = &window;
}

void RendererD3D10::render() {

}

void RendererD3D10::destroy() {

}

RendererSupport RendererD3D10::getSupport() {
	return { { RendererAPIType::D3D10, { 10, 0, 0 } } };
}

Renderable* RendererD3D10::createRenderable() {
	return new Renderable();
}

void RendererD3D10::renderableUploadMesh(Renderable* renderable, Mesh* mesh) {

}

Mesh RendererD3D10::renderableDownloadMesh(Renderable* renderable) {
	return Mesh();
}

void RendererD3D10::destroyRenderable(Renderable* renderable) {
	delete renderable;
}

PostProcess* RendererD3D10::createPostProcess(const char* source, ShaderDescriptor shaderDescriptor) {
	return nullptr;
}

void RendererD3D10::destroyPostProcess(PostProcess* postProcess) {

}

} // namespace kengine::graphics::d3d10