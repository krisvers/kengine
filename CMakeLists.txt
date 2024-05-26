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

file(GLOB_RECURSE SOURCES "engine/src/*/*/*/*/*.cpp" "engine/src/*/*/*/*.cpp" "engine/src/*/*/*.cpp" "engine/src/*/*.cpp" "game/src/*.cpp")

include_directories("engine/include")

set(LIBRARIES)
set(INCLUDES)

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")

if (WIN32)
	find_library(D3D10_LIBRARY d3d10)
	find_library(DXGI_LIBRARY dxgi)
	message(STATUS "Found D3D10: ${D3D10_LIBRARY}")
	message(STATUS "Found DXGI: ${DXGI_LIBRARY}")
	list(APPEND LIBRARIES ${D3D10_LIBRARY} ${DXGI_LIBRARY})
else()
	set(OpenGL_GL_PREFERENCE "LEGACY")
	find_package(OpenGL REQUIRED)
	list(APPEND LIBRARIES ${OPENGL_LIBRARIES})
	list(APPEND INCLUDES ${OPENGL_INCLUDE_DIRS})
endif()

# Add source to this project's executable.
add_executable(kengine ${SOURCES})

message(STATUS "Found libraries: ${LIBRARIES}")

include_directories(${INCLUDES})
target_link_libraries(kengine ${LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET kengine PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
