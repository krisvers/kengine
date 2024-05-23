#include <kengine/core/window.hpp>
#include <kengine/macros.hpp>

namespace kengine::core {

class Window::Implementation {
public:
	Implementation(std::string const& title, kengine::u32 width, kengine::u32 height) {

	}
};

Window::Window(std::string const& title) {
	Window(title, 800, 600);
}

Window::Window(kengine::u32 width, kengine::u32 height) {
	Window("game window", width, height);
}

Window::Window(std::string const& title, kengine::u32 width, kengine::u32 height) {
	impl = new Implementation(title, width, height);
}

Window::~Window() {

}

} // namespace kengine::core