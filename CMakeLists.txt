# CMakeList.txt : CMake project for kengine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("kengine")

file(GLOB_RECURSE SOURCES "engine/src/*/*/*/*.cpp" "engine/src/*/*/*.cpp" "engine/src/*/*.cpp" "game/src/*.cpp")

include_directories("engine/include")

set(LIBRARIES "")
set(INCLUDES "")

if (WIN32)
	find_library(D3D10_LIBRARY d3d10)
	find_library(DXGI_LIBRARY dxgi)
	string(APPEND LIBRARIES ${D3D10_LIBRARY} ${DXGI_LIBRARY})
else()
	set(OpenGL_GL_PREFERENCE "LEGACY")
	find_package(OpenGL REQUIRED)
	string(APPEND LIBRARIES ${OPENGL_LIBRARIES})
	string(APPEND INCLUDES ${OPENGL_INCLUDE_DIRS})
endif()

# Add source to this project's executable.
add_executable(kengine ${SOURCES})

include_directories(${INCLUDES})
target_link_libraries(kengine ${LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET kengine PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
