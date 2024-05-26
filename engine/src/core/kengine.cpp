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
	platform::IPlatform* platform = platform::Platform::get().create();

	platform->init();

	window::IWindow* window = window::Window::get().create("KEngine Window", 800, 600);
	window->show();

	graphics::IRenderer* renderer = graphics::Renderer::get().create();

	void* mem = platform::Memory::get().allocAligned(63, platform::AllocationTag::Engine);
	platform::Memory::get().printAllocations(Logger::get().getLogger(), LogSeverity::Info);

	std::shared_ptr<assets::TextAsset> textAsset = assets::Manager::get().load<assets::TextAsset>("test.txt");
	if (textAsset) {
		Logger::get().logf(LogSeverity::Info, "Loaded text asset with content: {}", textAsset->getText());
	} else {
		Logger::get().logf(LogSeverity::Error, "Failed to load text asset");
	}

	std::shared_ptr<assets::TextAsset> textAsset2 = assets::Manager::get().load<assets::TextAsset>("test2.txt");
	if (textAsset) {
		Logger::get().logf(LogSeverity::Info, "Loaded text asset with content: {}", textAsset2->getText());
	} else {
		Logger::get().logf(LogSeverity::Error, "Failed to load text asset");
	}

	std::shared_ptr<assets::ImageAsset> imageAsset = assets::Manager::get().load<assets::ImageAsset>("test.png");
	if (imageAsset) {
		Logger::get().logf(LogSeverity::Info, "Loaded image asset");
	} else {
		Logger::get().logf(LogSeverity::Error, "Failed to load image asset");
	}

	// log static and instance uuids
	Logger::get().logf(LogSeverity::Info, "TextAsset static UUID: {}", assets::TextAsset::getUUIDStatic());
	Logger::get().logf(LogSeverity::Info, "TextAsset instance UUID: {}", textAsset->getUUID());
	Logger::get().logf(LogSeverity::Info, "TextAsset2 instance UUID: {}", textAsset2->getUUID());
	Logger::get().logf(LogSeverity::Info, "ImageAsset static UUID: {}", assets::ImageAsset::getUUIDStatic());

	while (!window->isClosed()) {
		platform->update();
		renderer->render();
	}

	platform::Memory::get().dealloc(mem, 63);

	platform->shutdown();
}

} // namespace kengine::core
