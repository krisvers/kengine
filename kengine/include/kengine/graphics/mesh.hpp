#ifndef KRISVERS_KENGINE_GRAPHICS_MESH_HPP
#define KRISVERS_KENGINE_GRAPHICS_MESH_HPP

#include <kengine/types.hpp>
#include <kengine/util/vector.hpp>

#include <vector>

namespace kengine::graphics {

struct Vertex {
	util::Vector<f32, 3> position;
	util::Vector<f32, 3> normal;
	util::Vector<f32, 3> color;
	util::Vector<f32, 2> texcoord;

	static constexpr kengine::usize size() { return util::Vector<f32, 3>::size() * 3 + util::Vector<f32, 2>::size(); }
	static constexpr kengine::usize count() { return util::Vector<f32, 3>::count() * 3 + util::Vector<f32, 2>::count(); }
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<u32> indices;
};

} // namespace kengine::graphics

#endif
