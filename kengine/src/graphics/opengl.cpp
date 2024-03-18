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
	virtual void init(Window& window, Camera& camera);
	virtual void render();
	virtual void destroy();

	virtual Renderable* createRenderable();
	virtual void uploadRenderableMesh(Renderable* renderable, Mesh* mesh);
	virtual Mesh downloadRenderableMesh(Renderable* renderable);
	virtual void destroyRenderable(Renderable* renderable);

	void createShaderGL(GLuint shader, const char* source);
	GLuint createShaderProgramGL(std::vector<GLuint> shaders);

	Camera* m_camera;
	Window* m_window;

	std::vector<RenderableOpenGL*> m_renderables;
	GLuint m_lightingPassProgram;

	GLuint m_geometryPassProgram;
	GLuint m_geometryPassTexture;
	GLuint m_geometryPassNormalTexture;
	GLuint m_geometryPassColorTexture;
	GLuint m_geometryPassFramebuffer;
	GLint m_geometryPassMVPUniform;
};

IRenderer* createOpenGLRenderer() {
	return reinterpret_cast<IRenderer*>(new RendererOpenGL());
}

void destroyOpenGLRenderer(IRenderer* renderer) {
	delete static_cast<RendererOpenGL*>(renderer);
}

void RendererOpenGL::init(Window& window, Camera& camera) {
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize OpenGL context");
	}

	m_window = &window;
	m_camera = &camera;

	const char* vgeometryShaderSource = R"(
		#version 410
		layout (location = 0) in vec3 in_pos;
		layout (location = 1) in vec3 in_normal;
		layout (location = 2) in vec3 in_color;
		layout (location = 3) in vec2 in_texcoord;
		
		layout (location = 0) out vec3 v_pos;
		layout (location = 1) out vec3 v_normal;
		layout (location = 2) out vec3 v_color;

		uniform mat4 mvp;

		void main() {
			gl_Position = mvp * vec4(in_pos, 1.0);
			v_pos = gl_Position.xyz;
			v_normal = in_normal;
			v_color = in_color;
		}
	)";

	const char* fgeometryShaderSource = R"(
		#version 410
		
		layout (location = 0) in vec3 v_pos;
		layout (location = 1) in vec3 v_normal;
		layout (location = 2) in vec3 v_color;
		
		layout (location = 0) out vec3 out_pos;
		layout (location = 1) out vec3 out_normal;
		layout (location = 2) out vec3 out_color;

		void main() {
			out_pos = v_pos;
			out_normal = v_normal;
			out_color = v_color;
		}
	)";

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);

	createShaderGL(vshader, vgeometryShaderSource);
	createShaderGL(fshader, fgeometryShaderSource);

	m_geometryPassProgram = createShaderProgramGL({ vshader, fshader });

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	const char* vlightingShaderSource = R"(
		#version 410

		const vec2 quadVertices[4] = vec2[](
			vec2(-1.0, -1.0),
			vec2(-1.0, 1.0),
			vec2(1.0, -1.0),
			vec2(1.0, 1.0)
		);

		layout (location = 0) out vec2 v_texcoord;

		void main() {
			gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
			v_texcoord = (quadVertices[gl_VertexID] + 1.0) / 2.0;
		}
	)";

	const char* flightingShaderSource = R"(
		#version 410

		uniform sampler2D sampler_position;
		uniform sampler2D sampler_normal;
		uniform sampler2D sampler_color;

		layout (location = 0) in vec2 v_texcoord;

		layout (location = 0) out vec4 out_color;

		void main() {
			out_color = texture(sampler_color, v_texcoord);
		}
	)";

	vshader = glCreateShader(GL_VERTEX_SHADER);
	fshader = glCreateShader(GL_FRAGMENT_SHADER);

	createShaderGL(vshader, vlightingShaderSource);
	createShaderGL(fshader, flightingShaderSource);

	m_lightingPassProgram = createShaderProgramGL({ vshader, fshader });

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	m_geometryPassMVPUniform = glGetUniformLocation(m_geometryPassProgram, "mvp");

	glGenFramebuffers(1, &m_geometryPassFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_geometryPassFramebuffer);

	glGenTextures(1, &m_geometryPassTexture);
	glGenTextures(1, &m_geometryPassNormalTexture);
	glGenTextures(1, &m_geometryPassColorTexture);

	glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_window->width, m_window->height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_geometryPassTexture, 0);

	glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_window->width, m_window->height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_geometryPassNormalTexture, 0);

	glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window->width, m_window->height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_geometryPassColorTexture, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, buffers);
}

void RendererOpenGL::render() {
	m_camera->calculateViewMatrix();
	m_camera->calculateProjectionMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, m_geometryPassFramebuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_geometryPassProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);

	util::Matrix<4, 4> vp = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();
	for (RenderableOpenGL*& renderable : m_renderables) {
		glUniformMatrix4fv(m_geometryPassMVPUniform, 1, GL_FALSE, (vp * renderable->modelMatrix).data());
		renderable->draw();
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_geometryPassFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_lightingPassProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "sampler_position"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "sampler_normal"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "sampler_color"), 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RendererOpenGL::destroy() {
	for (RenderableOpenGL*& renderable : m_renderables) {
		delete renderable;
	}
	glDeleteProgram(m_geometryPassProgram);
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

void RendererOpenGL::createShaderGL(GLuint shader, const char* source) {
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetShaderInfoLog(shader, 512, nullptr, msg);
		logger::printf(LogType::ERROR, "GLSL vertex shader compilation failure: %s\n", msg);
		throw std::runtime_error("GLSL vertex shader compilation failure");
	}
}

GLuint RendererOpenGL::createShaderProgramGL(std::vector<GLuint> shaders) {
	GLuint program = glCreateProgram();
	for (GLuint shader : shaders) {
		glAttachShader(program, shader);
	}
	glLinkProgram(program);

	GLint success = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetProgramInfoLog(program, 512, nullptr, msg);
		logger::printf(LogType::ERROR, "GLSL shader program linker failure: %s\n", msg);
		throw std::runtime_error("GLSL shader program linker failure");
	}

	return program;
}

} // namespace kengine::graphics
