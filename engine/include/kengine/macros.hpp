#ifndef KENGINE_MACROS_HPP
#define KENGINE_MACROS_HPP

#ifdef _WIN32
#define KENGINE_PLATFORM_WINDOWS
#elif defined(__linux__)
#define KENGINE_PLATFORM_LINUX
#elif defined(__APPLE__)
#define KENGINE_PLATFORM_MACOS
#else
#error "Unsupported platform"
#endif

#endif