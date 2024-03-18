#ifndef KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP
#define KRISVERS_KENGINE_GRAPHICS_RENDERER_HPP

#include <kengine/types.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/mesh.hpp>
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
	virtual void uploadRenderableMesh(Renderable* renderable, Mesh* mesh) = 0;
	virtual Mesh downloadRenderableMesh(Renderable* renderable) = 0;
	virtual void destroyRenderable(Renderable* renderable) = 0;

	Window* window;
};

} // namespace kengine::graphics

#endif
