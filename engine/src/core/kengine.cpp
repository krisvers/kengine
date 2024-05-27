#include <kengine/core/kengine.hpp>
#include <kengine/core/logging.hpp>
#include <kengine/core/window/window.hpp>
#include <kengine/core/graphics/renderer.hpp>
#include <kengine/core/platform/platform.hpp>
#include <kengine/core/platform/memory.hpp>
#include <kengine/core/assets/asset.hpp>
#include <kengine/core/assets/manager.hpp>

namespace kengine::core {

void KEngine::run() {
	Logger::get().logf(LogSeverity::Verbose, "hello world {}", "\"this is the zaza man speaking\"");
	platform::IPlatform& platform = platform::Platform::get().create();

	platform.init();

	window::IWindow& window = window::Window::get().create("KEngine Window", 800, 600);
	window.show();

	graphics::IRenderer& renderer = graphics::Renderer::get().create(window);

	void* mem = platform::Memory::get().allocAligned(63, platform::AllocationTag::Engine);
	platform::Memory::get().printAllocations(Logger::get().getLogger(), LogSeverity::Info);

	assets::AssetReference<assets::TextAsset> textAssetReference = assets::Manager::get().load<assets::TextAsset>("test.txt");
	assets::TextAsset& textAsset = textAssetReference.get();

	assets::AssetReference<assets::ImageAsset> imageAssetReference = assets::Manager::get().load<assets::ImageAsset>("test.png");
	assets::ImageAsset& imageAsset = assets::Manager::get().getReference<assets::ImageAsset>(imageAssetReference);

	Logger::get().logf(LogSeverity::Info, "ImageAsset UUID: {}", imageAsset.getUUID());

	Logger::get().logf(LogSeverity::Info, "TextAsset static UUID: {}", assets::TextAsset::getUUIDStatic());
	Logger::get().logf(LogSeverity::Info, "ImageAsset static UUID: {}", assets::ImageAsset::getUUIDStatic());

	while (!window.isClosed()) {
		platform.update();
		renderer.render();
	}

	assets::Manager::get().unloadAll();

	platform::Memory::get().dealloc(mem, 63);

	graphics::Renderer::get().destroy(renderer);
	window::Window::get().destroy(window);

	platform.shutdown();
}

} // namespace kengine::core
