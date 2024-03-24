#include <kengine/graphics/opengl.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/postprocess.hpp>
#include <kengine/logger.hpp>

#include <glad/glad.h>

#include <iostream>
#include <vector>

namespace kengine::graphics::opengl {

class PostProcessOpenGL : public PostProcess {
public:
	GLuint m_program;
	u32 m_index;
};

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
		modelMatrix = math::Matrix<4, 4>().identity();
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

class RendererOpenGL4_1 : public IRenderer {
public:
	virtual void init(Window& window, Camera& camera);
	virtual void render();
	virtual void destroy();

	virtual RendererSupport getSupport() {
		return { { RendererAPIType::OpenGL, { 4, 1, 0 } }, false };
	}

	virtual Renderable* createRenderable();
	virtual void renderableUploadMesh(Renderable* renderable, Mesh* mesh);
	virtual Mesh renderableDownloadMesh(Renderable* renderable);
	virtual void destroyRenderable(Renderable* renderable);

	virtual PostProcess* createPostProcess(const char* source, ShaderDescriptor shaderDescriptor);
	virtual void destroyPostProcess(PostProcess* postProcess);

	u8 createShaderGL(GLuint shader, const char* source);
	GLuint createShaderProgramGL(std::vector<GLuint> shaders);

	Camera* m_camera;
	Window* m_window;

	GLuint m_quadVertexShader;

	std::vector<RenderableOpenGL*> m_renderables;
	std::vector<PostProcessOpenGL*> m_postProcesses;

	GLuint m_lightingPassProgram;

	GLuint m_geometryPassProgram;
	GLuint m_geometryPassTexture;
	GLuint m_geometryPassNormalTexture;
	GLuint m_geometryPassColorTexture;
	GLuint m_geometryPassBatchIDTexture;
	GLuint m_geometryPassFramebuffer;
	GLint m_geometryPassMVPUniform;
	GLint m_geometryPassModelToWorldUniform;
	GLint m_geometryPassBatchID;

	GLuint m_intermediateTextures[2];
	GLuint m_intermediateFramebuffers[2];
};

void getOpenGLSupportedRenderers(std::vector<RendererSupport>& renderers) {
	RendererSupport renderer = {};
	renderer.api.api = RendererAPIType::OpenGL;
	renderer.api.version = { 4, 1, 0 };
	renderer.isModule = false;
	renderers.push_back(renderer);
}

IRenderer* createOpenGLRenderer() {
	return new RendererOpenGL4_1();
}

IRenderer* createOpenGLRendererWithSupport(RendererSupport renderer) {
	if (renderer.api.api != RendererAPIType::OpenGL) {
		return nullptr;
	}

	if (renderer.api.version.major != 4 || renderer.api.version.minor != 1) {
		return nullptr;
	}

	return new RendererOpenGL4_1();
}

void destroyOpenGLRenderer(IRenderer* renderer) {
	delete static_cast<RendererOpenGL4_1*>(renderer);
}

void RendererOpenGL4_1::init(Window& window, Camera& camera) {
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
		uniform mat4 modelToWorld;

		const vec2 quadVertices[4] = vec2[](
			vec2(-1.0, -1.0),
			vec2(-1.0, 1.0),
			vec2(1.0, -1.0),
			vec2(1.0, 1.0)
		);

		void main() {
			gl_Position = mvp * vec4(in_pos, 1.0);
			v_pos = vec3(modelToWorld * vec4(in_pos, 1.0));
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
		layout (location = 2) out vec4 out_color;
		layout (location = 3) out uint out_batchID;

		uniform uint batchID;

		void main() {
			out_pos = v_pos;
			out_normal = v_normal;
			out_color = vec4(v_color, 1.0);
			out_batchID = 99999;
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

		layout (location = 0) out vec2 gbufferTexcoord;

		void main() {
			gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
			gbufferTexcoord = (quadVertices[gl_VertexID] + 1.0) / 2.0;
		}
	)";

	const char* flightingShaderSource = R"(
		#version 410

		uniform sampler2D gbufferPosition;
		uniform sampler2D gbufferNormal;
		uniform sampler2D gbufferColor;
		uniform sampler2D gbufferBatchID;

		layout (location = 0) in vec2 gbufferTexcoord;

		layout (location = 0) out vec4 out_color;

		void main() {
			out_color = texture(gbufferColor, gbufferTexcoord);
		}
	)";

	m_quadVertexShader = glCreateShader(GL_VERTEX_SHADER);
	fshader = glCreateShader(GL_FRAGMENT_SHADER);

	if (createShaderGL(m_quadVertexShader, vlightingShaderSource) != 0) {
		throw std::runtime_error("Failed to compile post-process vertex shader");
	}
	if (createShaderGL(fshader, flightingShaderSource) != 0) {
		throw std::runtime_error("Failed to compile lighting fragment shader");
	}

	m_lightingPassProgram = createShaderProgramGL({ m_quadVertexShader, fshader });

	glDeleteShader(fshader);

	m_geometryPassMVPUniform = glGetUniformLocation(m_geometryPassProgram, "mvp");
	m_geometryPassModelToWorldUniform = glGetUniformLocation(m_geometryPassProgram, "modelToWorld");
	m_geometryPassBatchID = glGetUniformLocation(m_geometryPassProgram, "batchID");

	glGenFramebuffers(1, &m_geometryPassFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_geometryPassFramebuffer);

	glGenTextures(1, &m_geometryPassTexture);
	glGenTextures(1, &m_geometryPassNormalTexture);
	glGenTextures(1, &m_geometryPassColorTexture);
	glGenTextures(1, &m_geometryPassBatchIDTexture);

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

	glBindTexture(GL_TEXTURE_2D, m_geometryPassBatchIDTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, m_window->width, m_window->height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_geometryPassBatchIDTexture, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, buffers);

	glGenFramebuffers(2, m_intermediateFramebuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFramebuffers[0]);

	glGenTextures(2, m_intermediateTextures);
	glBindTexture(GL_TEXTURE_2D, m_intermediateTextures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window->width, m_window->height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_intermediateTextures[0], 0);

	glDrawBuffers(1, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFramebuffers[1]);
	glBindTexture(GL_TEXTURE_2D, m_intermediateTextures[1]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window->width, m_window->height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_intermediateTextures[1], 0);

	glDrawBuffers(1, buffers);
}

void RendererOpenGL4_1::render() {
	m_camera->calculateViewMatrix();
	m_camera->calculateProjectionMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, m_geometryPassFramebuffer);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_geometryPassProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassBatchIDTexture);

	math::Matrix<4, 4> vp = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();
	u32 batchID = 0;
	for (RenderableOpenGL*& renderable : m_renderables) {
		glUniformMatrix4fv(m_geometryPassModelToWorldUniform, 1, GL_FALSE, renderable->modelMatrix.data());
		glUniformMatrix4fv(m_geometryPassMVPUniform, 1, GL_FALSE, (vp * renderable->modelMatrix).data());
		glUniform1ui(m_geometryPassBatchID, batchID);
		renderable->draw();
		++batchID;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFramebuffers[1]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_lightingPassProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "gbufferPosition"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "gbufferNormal"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "gbufferColor"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_geometryPassBatchIDTexture);
	glUniform1i(glGetUniformLocation(m_lightingPassProgram, "gbufferBatchID"), 3);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	usize outputFB = 0;
	for (PostProcessOpenGL*& postprocess : m_postProcesses) {
		glUseProgram(postprocess->m_program);

		glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFramebuffers[outputFB]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_geometryPassTexture);
		glUniform1i(glGetUniformLocation(postprocess->m_program, "gbufferPosition"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_geometryPassNormalTexture);
		glUniform1i(glGetUniformLocation(postprocess->m_program, "gbufferNormal"), 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_geometryPassColorTexture);
		glUniform1i(glGetUniformLocation(postprocess->m_program, "gbufferColor"), 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_geometryPassBatchIDTexture);
		glUniform1i(glGetUniformLocation(postprocess->m_program, "gbufferBatchID"), 3);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_intermediateFramebuffers[1 - outputFB]);
		glUniform1i(glGetUniformLocation(postprocess->m_program, "screenTexture"), 4);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		outputFB = 1 - outputFB;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_intermediateFramebuffers[1 - outputFB]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_window->width, m_window->height, 0, 0, m_window->width, m_window->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RendererOpenGL4_1::destroy() {
	for (PostProcessOpenGL*& postprocess : m_postProcesses) {
		delete postprocess;
	}

	for (RenderableOpenGL*& renderable : m_renderables) {
		delete renderable;
	}

	glDeleteShader(m_quadVertexShader);

	glDeleteProgram(m_geometryPassProgram);
	glDeleteProgram(m_lightingPassProgram);

	glDeleteTextures(1, &m_geometryPassTexture);
	glDeleteTextures(1, &m_geometryPassNormalTexture);
	glDeleteTextures(1, &m_geometryPassColorTexture);
	glDeleteFramebuffers(1, &m_geometryPassFramebuffer);

	glDeleteTextures(2, m_intermediateTextures);
	glDeleteFramebuffers(2, m_intermediateFramebuffers);
}

Renderable* RendererOpenGL4_1::createRenderable() {
	RenderableOpenGL* renderable = new RenderableOpenGL();
	renderable->m_index = m_renderables.size();
	m_renderables.push_back(renderable);

	return static_cast<Renderable*>(renderable);
}

void RendererOpenGL4_1::renderableUploadMesh(Renderable* renderable, Mesh* mesh) {
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(math::Vector<f32, 3>::size()));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(math::Vector<f32, 3>::size() * 2));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(math::Vector<f32, 3>::size() * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	r->m_verticesCount = mesh->vertices.size();
}

Mesh RendererOpenGL4_1::renderableDownloadMesh(Renderable* renderable) {
	return Mesh();
}

void RendererOpenGL4_1::destroyRenderable(Renderable* renderable) {
	RenderableOpenGL* r = static_cast<RenderableOpenGL*>(renderable);
	m_renderables.erase(m_renderables.begin() + r->m_index);
	delete renderable;
}

PostProcess* RendererOpenGL4_1::createPostProcess(const char* source, ShaderDescriptor shaderDescriptor) {
	if (shaderDescriptor.medium != ShaderMedium::GLSL) {
		throw std::runtime_error("Invalid shader medium");
	}

	if (shaderDescriptor.type != ShaderType::Postprocess) {
		throw std::runtime_error("Invalid shader type");
	}

	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (createShaderGL(shader, source) != 0) {
		return nullptr;
	}

	GLuint program = createShaderProgramGL({ m_quadVertexShader, shader });

	glDeleteShader(shader);

	PostProcessOpenGL* postprocess = new PostProcessOpenGL();
	postprocess->m_index = m_postProcesses.size();
	postprocess->m_program = program;
	m_postProcesses.push_back(postprocess);
	return static_cast<PostProcess*>(postprocess);
}

void RendererOpenGL4_1::destroyPostProcess(PostProcess* postProcess) {
	if (postProcess == nullptr) {
		return;
	}

	PostProcessOpenGL* p = static_cast<PostProcessOpenGL*>(postProcess);
	m_postProcesses.erase(m_postProcesses.begin() + p->m_index);
	glDeleteProgram(p->m_program);
	delete postProcess;
}

u8 RendererOpenGL4_1::createShaderGL(GLuint shader, const char* source) {
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		char msg[512];
		glGetShaderInfoLog(shader, 512, nullptr, msg);
		logger::printf(LogType::ERROR, "GLSL vertex shader compilation failure: %s\n", msg);
		return 1;
	}

	return 0;
}

GLuint RendererOpenGL4_1::createShaderProgramGL(std::vector<GLuint> shaders) {
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
		return 0;
	}

	return program;
}

} // namespace kengine::graphics::opengl
