#include <kengine/module/module.hpp>

namespace kengine::module {

void* SynthesizedModuleSet::get(const char* name) {
	auto it = symbols.find(name);
	if (it == symbols.end()) {
		return nullptr;
	}

	return it->second;
}

bool SynthesizedModuleSet::load(const char* name) {
	return false;
}

void SynthesizedModuleSet::set(const char* name, void* symbol) {
	symbols[name] = symbol;
}

} // namespace kengine::module