#ifndef KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP
#define KENGINE_CORE_GRAPHICS_GL41_RENDERER_HPP

#include <Windows.h>
#include <glad/glad.h>

#include <vector>

#include <kengine/macros.hpp>
#include <kengine/util/math/vector.hpp>
#include <kengine/core/graphics/renderer.hpp>

namespace kengine::core::graphics::gl41 {

// count template parameter
template<kengine::usize TextureCount>
struct FramebufferGL {
	GLuint fbo = 0;
	GLuint textures[TextureCount] = { 0 };
	GLenum attachmentTypes[TextureCount] = { 0 };

	~FramebufferGL() {
		glDeleteFramebuffers(1, &fbo);
		for (kengine::usize i = 0; i < TextureCount; ++i) {
			glDeleteTextures(1, &textures[i]);
		}
	}

	FramebufferGL& startSetup() {
		glGenFramebuffers(1, &fbo);
		for (kengine::usize i = 0; i < TextureCount; ++i) {
			glGenTextures(1, &textures[i]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		return *this;
	}

	FramebufferGL& setupTexture(kengine::usize which, GLsizei width, GLsizei height, GLenum format, GLenum internalFormat, GLenum type, GLenum sample, GLenum baseAttachmentType) {
		if (which >= TextureCount) {
			throw Exception("Framebuffer texture index out of bounds");
		}

		glBindTexture(GL_TEXTURE_2D, textures[which]);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sample);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sample);
		glBindTexture(GL_TEXTURE_2D, 0);

		attachmentTypes[which] = static_cast<GLenum>(baseAttachmentType + which);
		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(baseAttachmentType + which), GL_TEXTURE_2D, textures[which], 0);
		return *this;
	}

	FramebufferGL& endSetup() {
		glDrawBuffers(TextureCount, attachmentTypes);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return *this;
	}

	void bindForDrawing() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void bindForReading() {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	}
};

class ShaderGL {
public:
	bool compile(const char* vertexSource, const char* fragmentSource) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		glCompileShader(vertexShader);

		GLint success;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
			Logger::get().logf(LogSeverity::Error, "Vertex shader compilation failed: {}", infoLog);
			return false;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
			Logger::get().logf(LogSeverity::Error, "Fragment shader compilation failed: {}", infoLog);
			glDeleteShader(vertexShader);
			return false;
		}

		_program = glCreateProgram();
		glAttachShader(_program, vertexShader);
		glAttachShader(_program, fragmentShader);
		glLinkProgram(_program);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glGetProgramiv(_program, GL_LINK_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetProgramInfoLog(_program, sizeof(infoLog), nullptr, infoLog);
			Logger::get().logf(LogSeverity::Error, "Shader program linking failed: {}", infoLog);
			glDeleteProgram(_program);
			return false;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return true;
	}

	~ShaderGL() {
		glDeleteProgram(_program);
	}

	void bind() const {
		glUseProgram(_program);
	}

	void setUniform(const char* name, const GLint& value) const {
		glUniform1i(glGetUniformLocation(_program, name), value);
	}

	void setUniform(const char* name, const GLuint& value) const {
		glUniform1ui(glGetUniformLocation(_program, name), value);
	}

	void setUniform(const char* name, const GLfloat& value) const {
		glUniform1f(glGetUniformLocation(_program, name), value);
	}

	void setUniform(const char* name, const util::math::Vector<f32, 2>& vec) const {
		glUniform2fv(glGetUniformLocation(_program, name), 1, vec.data());
	}

	void setUniform(const char* name, const util::math::Vector<f32, 3>& vec) const {
		glUniform3fv(glGetUniformLocation(_program, name), 1, vec.data());
	}

	void setUniform(const char* name, const util::math::Vector<f32, 4>& vec) const {
		glUniform4fv(glGetUniformLocation(_program, name), 1, vec.data());
	}

private:
	GLuint _program;
};

class RendererGL41 : public IRenderer {
public:
	RendererGL41(window::IWindow& window);
	~RendererGL41();

	void render() override;
	IMesh* createMesh() override;
	IObject* createObject(IMesh* mesh) override;

private:
	void _initGL();
	void _deinitGL();
	void _swapBuffers();

	window::IWindow& _window;
	FramebufferGL<3> _intermediateFramebuffers[2];

	ShaderGL _litShader;

#ifdef KENGINE_PLATFORM_WINDOWS
	HDC _hdc = nullptr;
	HGLRC _hglrc = nullptr;
#endif // KENGINE_PLATFORM_WINDOWS
};

} // namespace kengine::core::graphics::gl41

#endif
