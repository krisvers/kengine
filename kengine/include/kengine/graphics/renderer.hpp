#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

#include <kengine/types.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/mesh.hpp>
#include <kengine/graphics/postprocess.hpp>
#include <kengine/window.hpp>
#include <kengine/camera.hpp>
#include <kengine/module/module.hpp>

#include <vector>

namespace kengine::graphics {

enum class RendererAPIType {
	Metal,
	D3D12,
	Vulkan,
	D3D11,
	D3D10,
	OpenGL,
	Unknown,
};

struct RendererAPIVersion {
	u8 major;
	u8 minor;
	u8 patch;
};

struct RendererAPI {
	RendererAPIType api;
	RendererAPIVersion version;
};

struct RendererSupport {
	RendererAPI api;
	bool isModule;
};

struct RendererModule {
	RendererSupport support;
	kengine::module::ModuleSet* moduleSet;
};

class IRenderer {
public:
	virtual void init(Window& window, Camera& camera) = 0;
	virtual void render() = 0;
	virtual void destroy() = 0;

	virtual RendererSupport getSupport() = 0;

	virtual Renderable* createRenderable() = 0;
	virtual void renderableUploadMesh(Renderable* renderable, Mesh* mesh) = 0;
	virtual Mesh renderableDownloadMesh(Renderable* renderable) = 0;
	virtual void destroyRenderable(Renderable* renderable) = 0;

	virtual PostProcess* createPostProcess(const char* source, ShaderDescriptor shaderDescriptor) = 0;
	virtual void destroyPostProcess(PostProcess* postProcess) = 0;

	Window* window;
};

void getSupportedRenderers(std::vector<RendererSupport>& renderers);
IRenderer* createRenderer();
IRenderer* createRendererWithSupport(RendererSupport support);
void destroyRenderer(IRenderer* renderer);

bool loadRendererFromModuleSet(kengine::module::ModuleSet* set);

} // namespace kengine::graphics

#endif
