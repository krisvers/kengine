#ifndef KRISVERS_KENGINE_GRAPHICS_MESH_HPP
#define KRISVERS_KENGINE_GRAPHICS_MESH_HPP

#include <kengine/types.hpp>
#include <kengine/math/vector.hpp>

#include <vector>

namespace kengine::graphics {

struct Vertex {
	math::Vector<f32, 3> position{ 0, 0, 0 };
	math::Vector<f32, 3> normal{ 0, 0, 0 };
	math::Vector<f32, 3> color{ 1, 1, 1 };
	math::Vector<f32, 2> texcoord{ 0, 0 };

	Vertex(math::Vector<f32, 3> pos) : position(pos) {}
	Vertex(math::Vector<f32, 3> pos, math::Vector<f32, 3> norm) : position(pos), normal(norm) {}
	Vertex(math::Vector<f32, 3> pos, math::Vector<f32, 3> norm, math::Vector<f32, 3> col) : position(pos), normal(norm), color(col) {}
	Vertex(math::Vector<f32, 3> pos, math::Vector<f32, 3> norm, math::Vector<f32, 3> col, math::Vector<f32, 2> tex) : position(pos), normal(norm), color(col), texcoord(tex) {}
	Vertex(math::Vector<f32, 2> tex) : texcoord(tex) {}

	static constexpr kengine::usize size() { return math::Vector<f32, 3>::size() * 3 + math::Vector<f32, 2>::size(); }
	static constexpr kengine::usize count() { return math::Vector<f32, 3>::count() * 3 + math::Vector<f32, 2>::count(); }
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<u32> indices;
};

} // namespace kengine::graphics

#endif
