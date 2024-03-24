#include <kengine/graphics/renderer.hpp>
#include <kengine/graphics/opengl.hpp>
#include <kengine/graphics/d3d10.hpp>

#include <vector>
#include <functional>

namespace kengine::graphics {

static std::vector<kengine::module::ModuleSet*> g_rendererModuleSets;

void getSupportedRenderers(std::vector<RendererSupport>& renderers) {
	renderers.clear();
	opengl::getOpenGLSupportedRenderers(renderers);
	d3d10::getD3D10SupportedRenderers(renderers);

	for (usize i = 0; i < g_rendererModuleSets.size(); ++i) {
		kengine::module::ModuleSet*& set = g_rendererModuleSets[i];
		void* ptr = set->get("getSupportedRenderers");
		reinterpret_cast<void(*)(std::vector<RendererSupport>&)>(ptr)(renderers);
	}
}

IRenderer* createRenderer() {
	std::vector<RendererSupport> renderers;
	getSupportedRenderers(renderers);
	if (renderers.empty()) {
		return nullptr;
	}

	std::function<bool(void*& outBest, RendererSupport& support, RendererSupport& bestSupport, void* value)> decideFunc =
	[](void*& outBest, RendererSupport& support, RendererSupport& bestSupport, void* value) -> bool {
		if (support.api.api == RendererAPIType::D3D12 || support.api.api == RendererAPIType::Metal) {
			outBest = value;
			bestSupport = support;
			return true;
		}

		if (support.api.api < bestSupport.api.api) {
			outBest = value;
			bestSupport = support;
		}
		return false;
	};

	RendererSupport unknown = { { RendererAPIType::Unknown, { 0, 0, 0 } }, false };
	RendererSupport* best = &unknown;
	for (RendererSupport& renderer : renderers) {
		if (renderer.isModule) {
			continue;
		}

		if (decideFunc(reinterpret_cast<void*&>(best), renderer, *best, &renderer)) {
			return createRendererWithSupport(*best);
		}
	}

	if (best != &unknown) {
		return createRendererWithSupport(*best);
	}

	RendererModule unknownModule = { { RendererAPIType::Unknown, { 0, 0, 0 } }, nullptr };
	RendererModule* bestModule = &unknownModule;
	std::vector<RendererSupport> supports;
	for (usize i = 0; i < g_rendererModuleSets.size(); ++i) {
		kengine::module::ModuleSet*& set = g_rendererModuleSets[i];

		supports.clear();
		reinterpret_cast<void(*)(std::vector<RendererSupport>)>(set->get("getSupportedRenderers"))(supports);

		if (supports.empty()) {
			continue;
		}

		for (RendererSupport& support : supports) {
			if (support.isModule) {
				continue;
			}
			
			RendererModule module = { support, set };
			if (decideFunc(reinterpret_cast<void*&>(bestModule), support, bestModule->support, &module)) {
				return reinterpret_cast<IRenderer*(*)(RendererSupport)>(set->get("createRendererWithSupport"))(*best);
			}
		}
	}

	if (bestModule == &unknownModule) {
		return nullptr;
	}

	return reinterpret_cast<IRenderer*(*)(RendererSupport)>(bestModule->moduleSet->get("createRendererWithSupport"))(bestModule->support);
}

IRenderer* createRendererWithSupport(RendererSupport support) {
	switch (support.api.api) {
		case RendererAPIType::OpenGL:
			return opengl::createOpenGLRendererWithSupport(support);
		case RendererAPIType::D3D10:
			return d3d10::createD3D10RendererWithSupport(support);
	}

	throw std::runtime_error("Unsupported renderer api");
}

void destroyRenderer(IRenderer* renderer) {
	opengl::destroyOpenGLRenderer(renderer);
}

bool loadRendererFromModuleSet(kengine::module::ModuleSet* set) {
	if (set == nullptr) {
		return false;
	}

	if (!set->load("getSupportedRenderers")) {
		return false;
	}

	if (!set->load("createRenderer")) {
		return false;
	}

	if (!set->load("createRendererWithSupport")) {
		return false;
	}

	if (!set->load("destroyRenderer")) {
		return false;
	}

	g_rendererModuleSets.push_back(set);
	return true;
}

} // namespace kengine::graphics