#include <kengine/module/module.hpp>
#include <windows.h>

namespace kengine::module {

struct ModuleSetWin32 : public ModuleSet {
	virtual void* get(const char* name);
	virtual bool load(const char* name);

	HMODULE module = nullptr;
};

void* ModuleSetWin32::get(const char* name) {
	auto it = symbols.find(name);
	if (it == symbols.end()) {
		return nullptr;
	}

	return it->second;
}

bool ModuleSetWin32::load(const char* name) {
	void* symbol = GetProcAddress(module, name);
	if (symbol == NULL) {
		return false;
	}

	symbols[name] = symbol;
	return true;
}

ModuleSet* loadModuleSet(const char* path) {
	std::string pathStr = path;
	pathStr += ".dll";

	HMODULE module = LoadLibraryA(pathStr.c_str());
	if (module == NULL) {
		return nullptr;
	}

	ModuleSetWin32* moduleSet = new ModuleSetWin32();
	moduleSet->module = module;
	return moduleSet;
}

void unloadModuleSet(ModuleSet* moduleSet) {
	delete moduleSet;
}

} // namespace kengine::module