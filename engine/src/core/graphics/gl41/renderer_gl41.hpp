#ifndef KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP

#include <kengine/core/graphics/renderer.hpp>

namespace kengine::core::graphics::gl41 {

class RendererExceptionGL41 : public RendererException {
public:
	RendererExceptionGL41(std::string const& message) : RendererException("(DX10 exception) " + message) {}
};

class RendererGL41 : public IRenderer {
public:
	RendererGL41();
	~RendererGL41() override;

	void render() override;

private:
	class Implementation;
	Implementation* impl;
};

} // namespace kengine::core::graphics::gl41

#endif
