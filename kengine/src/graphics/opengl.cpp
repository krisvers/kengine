#include <kengine/graphics/opengl.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/logger.hpp>

#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace kengine::graphics {

class RenderableOpenGL : public Renderable {
public:
	GLuint m_vertexArray;
	GLuint m_vertexBuffer;
	u32 m_verticesCount;
	u32 m_index;

	RenderableOpenGL() {
		glGenBuffers(1, &m_vertexBuffer);
		glGenVertexArrays(1, &m_vertexArray);

		m_verticesCount = 0;
		modelMatrix = util::Matrix<4, 4>().identity();
	}

	~RenderableOpenGL() {
		glDeleteBuffers(1, &m_vertexBuffer);
		glDeleteVertexArrays(1, &m_vertexArray);
	}

	void draw() {
		glBindVertexArray(m_vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
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
	GLuint m_shaderProgram;
	GLint m_shaderMVPUniform;
};

IRenderer* createOpenGLRenderer() {
	return reinterpret_cast<IRenderer*>(new RendererOpenGL());
}

void destroyOpenGLRenderer(IRenderer* renderer) {
	delete static_cast<RendererOpenGL*>(renderer);
}

void RendererOpenGL::init() {
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize OpenGL context");
	}

	const char* vshaderSource = R"(
		#version 410
		layout (location = 0) in vec3 in_pos;
		layout (location = 1) in vec3 in_normal;
		layout (location = 2) in vec3 in_color;
		layout (location = 3) in vec2 in_texcoord;

		layout (location = 0) out vec3 v_pos;

		uniform mat4 mvp;

		void main() {
			gl_Position = mvp * vec4(in_pos, 1.0);
			v_pos = in_pos;
		}
	)";

	const char* fshaderSource = R"(
		#version 410
		layout (location = 0) in vec3 v_pos;
		layout (location = 0) out vec4 out_color;
		void main() {
			out_color = vec4((v_pos + 1.0) / 2.0, 1.0);
		}
	)";

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vshader, 1, &vshaderSource, nullptr);
	glCompileShader(vshader);

	GLint success = GL_TRUE;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetShaderInfoLog(vshader, 512, NULL, msg);
		logger::printf(LogType::ERROR, "GLSL vertex shader compilation failure: %s\n", msg);
		throw std::runtime_error("GLSL vertex shader compilation failure");
	}

	glShaderSource(fshader, 1, &fshaderSource, nullptr);
	glCompileShader(fshader);

	success = GL_TRUE;
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetShaderInfoLog(fshader, 512, NULL, msg);
		logger::printf(LogType::ERROR, "GLSL fragment shader compilation failure: %s\n", msg);
		throw std::runtime_error("GLSL fragment shader compilation failure");
	}

	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vshader);
	glAttachShader(m_shaderProgram, fshader);
	glLinkProgram(m_shaderProgram);

	success = GL_TRUE;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, msg);
		logger::printf(LogType::ERROR, "GLSL shader program linker failure: %s\n", msg);
		throw std::runtime_error("GLSL shader program linker failure");
	}

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	glUseProgram(m_shaderProgram);
	m_shaderMVPUniform = glGetUniformLocation(m_shaderProgram, "mvp");
}

void RendererOpenGL::render() {
	glClear(GL_COLOR_BUFFER_BIT);
	for (RenderableOpenGL*& renderable : m_renderables) {
		glUniformMatrix4fv(m_shaderMVPUniform, 1, GL_FALSE, renderable->modelMatrix.data());
		renderable->draw();
	}
}

void RendererOpenGL::destroy() {
	for (RenderableOpenGL*& renderable : m_renderables) {
		delete renderable;
	}
	glDeleteProgram(m_shaderProgram);
}

Renderable* RendererOpenGL::createRenderable() {
	RenderableOpenGL* renderable = new RenderableOpenGL();
	renderable->m_index = m_renderables.size();
	m_renderables.push_back(renderable);

	return static_cast<Renderable*>(renderable);
}

void RendererOpenGL::uploadRenderableMesh(Renderable* renderable, Mesh* mesh) {
	RenderableOpenGL* r = static_cast<RenderableOpenGL*>(renderable);
	glBindVertexArray(r->m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, r->m_vertexBuffer);

	std::vector<f32> vertices(Vertex::count() * mesh->vertices.size());
	for (usize i = 0; i < mesh->vertices.size(); ++i) {
		vertices[i * Vertex::count() + 0] = mesh->vertices[i].position.x();
		vertices[i * Vertex::count() + 1] = mesh->vertices[i].position.y();
		vertices[i * Vertex::count() + 2] = mesh->vertices[i].position.z();
		vertices[i * Vertex::count() + 3] = mesh->vertices[i].normal.x();
		vertices[i * Vertex::count() + 4] = mesh->vertices[i].normal.y();
		vertices[i * Vertex::count() + 5] = mesh->vertices[i].normal.z();
		vertices[i * Vertex::count() + 6] = mesh->vertices[i].color.x();
		vertices[i * Vertex::count() + 7] = mesh->vertices[i].color.y();
		vertices[i * Vertex::count() + 8] = mesh->vertices[i].color.z();
		vertices[i * Vertex::count() + 9] = mesh->vertices[i].texcoord.x();
		vertices[i * Vertex::count() + 10] = mesh->vertices[i].texcoord.y();
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	constexpr GLsizei stride = Vertex::size();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size()));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size() * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(util::Vector<f32, 3>::size() * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	r->m_verticesCount = mesh->vertices.size();
}

Mesh RendererOpenGL::downloadRenderableMesh(Renderable* renderable) {
	return Mesh();
}

void RendererOpenGL::destroyRenderable(Renderable* renderable) {
	RenderableOpenGL* r = static_cast<RenderableOpenGL*>(renderable);
	m_renderables.erase(m_renderables.begin() + r->m_index);
	delete renderable;
}

} // namespace kengine::graphics
