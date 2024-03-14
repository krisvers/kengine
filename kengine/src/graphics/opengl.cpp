#include <kengine/graphics/opengl.hpp>
#include <kengine/graphics/renderable.hpp>

#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace kengine::graphics {

class RenderableOpenGL : public Renderable {
public:
	GLuint m_vertexArray;
	GLuint m_vertexBuffer;
	u32 m_verticesCount;

	RenderableOpenGL() {
		glGenBuffers(1, &m_vertexBuffer);
		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);

		constexpr GLsizei stride = util::Vector<f32, 3>::size() * 3 + util::Vector<f32, 2>::size();
		glVertexAttribPointer(0, util::Vector<f32, 3>::size(), GL_FALSE, stride, nullptr);
		glVertexAttribPointer(1, util::Vector<f32, 3>::size(), GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size()));
		glVertexAttribPointer(2, util::Vector<f32, 3>::size(), GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size() * 2));
		glVertexAttribPointer(3, util::Vector<f32, 2>::size(), GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size() * 3));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	void draw() {
		glBindVertexArray(m_vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
	}
};

class RendererOpenGL : public IRenderer {
public:
	virtual void init();
	virtual void render();
	virtual void destroy();

	virtual Renderable* createRenderable();
	virtual void uploadRenderableMesh(Renderable* renderable, Mesh* mesh);
	virtual Mesh downloadRenderableMesh(Renderable* renderable);
	virtual void destroyRenderable(Renderable* renderable);

	std::vector<RenderableOpenGL*> m_renderables;
};

IRenderer* createOpenGLRenderer() {
	return reinterpret_cast<IRenderer*>(new RendererOpenGL());
}

void destroyOpenGLRenderer(IRenderer* renderer) {
	delete reinterpret_cast<RendererOpenGL*>(renderer);
}

void RendererOpenGL::init() {
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize OpenGL context");
	}
}

void RendererOpenGL::render() {
	for (RenderableOpenGL*& renderable : m_renderables) {
		renderable->draw();
	}
}

void RendererOpenGL::destroy() {
	std::cout << "opengl destroy\n";
}

Renderable* RendererOpenGL::createRenderable() {
	RenderableOpenGL* renderable = new RenderableOpenGL();
	m_renderables.push_back(renderable);

	return static_cast<Renderable*>(renderable);
}

void RendererOpenGL::uploadRenderableMesh(Renderable* renderable, Mesh* mesh) {
	RenderableOpenGL* r = reinterpret_cast<RenderableOpenGL*>(renderable);
	glBindVertexArray(r->m_vertexArray);
	glBindBuffer(GL_VERTEX_ARRAY, r->m_vertexBuffer);

	std::vector<f32> vertices(Vertex::count() * mesh->vertices.size());
	for (usize i = 0; i < mesh->vertices.size(); ++i) {
		
	}
	glBufferData(GL_VERTEX_ARRAY, Vertex::size() * mesh->vertices.size(), vertices.data(), GL_ARRAY_BUFFER);
}

Mesh RendererOpenGL::downloadRenderableMesh(Renderable* renderable) {
	return Mesh();
}

void RendererOpenGL::destroyRenderable(Renderable* renderable) {
	delete renderable;
}

} // namespace kengine::graphics
