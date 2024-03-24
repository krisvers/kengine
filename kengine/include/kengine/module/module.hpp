#ifndef KRISVERS_KENGINE_MODULE_MODULE_HPP
#define KRISVERS_KENGINE_MODULE_MODULE_HPP

#include <kengine/types.hpp>

#include <unordered_map>
#include <string>

namespace kengine::module {

struct ModuleSet {
	std::unordered_map<std::string, void*> symbols;

	virtual void* get(const char* name) = 0;
	virtual bool load(const char* name) = 0;
};

struct SynthesizedModuleSet : public ModuleSet {
	virtual void* get(const char* name);
	virtual bool load(const char* name);
	void set(const char* name, void* symbol);
};

/*
	path is path/to/module without extension (it is decided depending on the platform)
	i.e. path/to/module -> path/to/module.dll on Windows, path/to/module.so on Linux, path/to/module.dylib on MacOS
*/
ModuleSet* loadModuleSet(const char* path);
SynthesizedModuleSet* synthesizeModuleSet();
void unloadModuleSet(ModuleSet* moduleSet);

} // namespace kengine::module

#endif