#ifndef KENGINE_CORE_GRAPHICS_MESH_HPP
#define KENGINE_CORE_GRAPHICS_MESH_HPP

#include <kengine/types.hpp>
#include <kengine/core/transform.hpp>
#include <kengine/util/math/vector.hpp>

#include <vector>

namespace kengine::core::graphics {

struct Vertex {
	util::math::Vector<float, 3> position;
	util::math::Vector<float, 3> normal;
	util::math::Vector<float, 2> texCoords;
};

class IMesh {
protected:
	IMesh() = default;
	IMesh(IMesh* mesh);
	virtual ~IMesh() = default;

public:
	virtual std::vector<Vertex> getVertices() = 0;
	virtual void setVertices(std::vector<Vertex> const& vertices) = 0;

	virtual std::vector<kengine::usize> getIndices() = 0;
	virtual void setIndices(std::vector<kengine::usize> const& indices) = 0;
};

class IObject {
public:
	IObject(IMesh* mesh) : _mesh(mesh) {}

	IMesh* getMesh() { return _mesh; }
	void setMesh(IMesh* mesh) { _mesh = mesh; }

	Transform transform;

private:
	IMesh* _mesh;
};

} // namespace kengine::core

#endif