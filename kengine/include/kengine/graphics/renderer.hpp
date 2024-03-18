#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

#include <kengine/types.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/mesh.hpp>
#include <kengine/graphics/postprocess.hpp>
#include <kengine/window.hpp>
#include <kengine/camera.hpp>

#include <vector>

namespace kengine::graphics {

class IRenderer {
public:
	virtual void init(Window& window, Camera& camera) = 0;
	virtual void render() = 0;
	virtual void destroy() = 0;

	virtual Renderable* createRenderable() = 0;
	virtual void renderableUploadMesh(Renderable* renderable, Mesh* mesh) = 0;
	virtual Mesh renderableDownloadMesh(Renderable* renderable) = 0;
	virtual void destroyRenderable(Renderable* renderable) = 0;

	virtual PostProcess* createPostProcess(const char* source, ShaderMedium medium) = 0;
	virtual void destroyPostProcess(PostProcess* postProcess) = 0;

	Window* window;
};

} // namespace kengine::graphics

#endif
