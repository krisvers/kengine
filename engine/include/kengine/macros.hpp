#ifndef KENGINE_MACROS_HPP
#define KENGINE_MACROS_HPP

#ifdef _WIN32
#define KENGINE_PLATFORM_WINDOWS
#define KENGINE_DEBUG_BREAK() __debugbreak()
#elif defined(__linux__)
#define KENGINE_PLATFORM_LINUX
#define KENGINE_DEBUG_BREAK() __builtin_trap()
#elif defined(__APPLE__)
#define KENGINE_PLATFORM_MACOS
#define KENGINE_DEBUG_BREAK() __builtin_trap()
#else
#error "Unsupported platform"
#endif

#endif