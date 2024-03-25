#include <kengine/kengine.hpp>
#include <kengine/math/vector.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/mesh.hpp>
#include <kengine/graphics/postprocess.hpp>
#include <kengine/util/time.hpp>
#include <kengine/input/input.hpp>
#include <kengine/input/gamepad.hpp>
#include <kengine/logger.hpp>

#include <iostream>

#define M_PI 3.14159265358979323846f

using namespace kengine;

static KEngine engine = KEngine();

struct Transform {
	math::Vector<f32, 3> position;
	math::Vector<f32, 3> rotation;
	math::Vector<f32, 3> scale;

	Transform() {
		position = { 0, 0, 0 };
		rotation = { 0, 0, 0 };
		scale = { 1, 1, 1 };
	}
};

struct GameObject {
	graphics::Renderable* renderable;
	Transform transform;

	void init() {
		renderable = engine.renderer->createRenderable();
	}

	void deinit() {
		engine.renderer->destroyRenderable(renderable);
	}

	void mesh(graphics::Mesh& mesh) {
		engine.renderer->renderableUploadMesh(renderable, &mesh);
	}

	void update() {
		renderable->modelMatrix
			.identity()
			.translate(transform.position)
			.rotate(
				transform.rotation[0] * M_PI / 180.0f,
				transform.rotation[1] * M_PI / 180.0f,
				transform.rotation[2] * M_PI / 180.0f
			).scale(transform.scale);
	}
};

struct Game {
	inline static GameObject player;
	inline static input::Gamepad* gamepad;
	inline static graphics::PostProcess* postprocess;

	static int init() {
		engine.camera.isOrthographic = false;
		engine.camera.position[2] = 2;

		player.init();
		player.transform.scale = 0.05f;

		postprocess = engine.renderer->createPostProcess(
			R"(
				#version 410

				uniform sampler2D gbufferPosition;
				uniform sampler2D gbufferNormal;
				uniform sampler2D gbufferColor;
				uniform sampler2D gbufferBatchID;

				uniform sampler2D screenTexture;

				layout (location = 0) in vec2 gbufferTexcoord;

				layout (location = 0) out vec4 out_color;

				void main() {
					out_color = vec4(texture(gbufferBatchID, gbufferTexcoord).r, 0.0, 0.0, 1.0);
				}
			)",
			{
				graphics::ShaderType::Postprocess,
				graphics::ShaderMedium::GLSL,
				{ 4, 1, 0 },
			}
		);

		graphics::Mesh mesh = graphics::Mesh();
		mesh.vertices = {
			{ math::Vector<f32, 3>{ -1, -1, 0 }, math::Vector<f32, 3>{ 0, 0, 1 }, math::Vector<f32, 3>{ 1, 1, 0 }  },
			{ math::Vector<f32, 3>{  1, -1, 0 }, math::Vector<f32, 3>{ 0, 0, 1 }, math::Vector<f32, 3>{ 1, 0, 1 }  },
			{ math::Vector<f32, 3>{ -1,  1, 0 }, math::Vector<f32, 3>{ 0, 0, 1 }, math::Vector<f32, 3>{ 0, 1, 1 }  },
		};

		player.mesh(mesh);

		gamepad = input::input::getGamepad();

		return 0;
	}

	static int update(f32 deltaTime) {
		math::Vector<f32, 3> movement = {0,0,0};
		if (input::input::isKey(input::InputKey::KEY_W)) {
			movement[1] += 1;
		}
		if (input::input::isKey(input::InputKey::KEY_S)) {
			movement[1] -= 1;
		}
		if (input::input::isKey(input::InputKey::KEY_D)) {
			movement[0] += 1;
		}
		if (input::input::isKey(input::InputKey::KEY_A)) {
			movement[0] -= 1;
		}

		if (input::input::isKey(input::InputKey::KEY_UP)) {
			engine.camera.position[2] -= deltaTime;
		}
		if (input::input::isKey(input::InputKey::KEY_DOWN)) {
			engine.camera.position[2] += deltaTime;
		}
		if (input::input::isKey(input::InputKey::KEY_RIGHT)) {
			engine.camera.position[0] += deltaTime;
		}
		if (input::input::isKey(input::InputKey::KEY_LEFT)) {
			engine.camera.position[0] -= deltaTime;
		}

		if (input::input::isKey(input::InputKey::KEY_ESCAPE)) {
			return 1;
		}

		if (gamepad != nullptr) {
			movement[0] += gamepad->getAxis(input::GamepadAxis::AXIS_LEFT_X);
			movement[1] += gamepad->getAxis(input::GamepadAxis::AXIS_LEFT_Y);

			player.transform.rotation[1] += gamepad->getAxis(input::GamepadAxis::AXIS_RIGHT_Y) * 10;
			player.transform.rotation[2] += gamepad->getAxis(input::GamepadAxis::AXIS_RIGHT_X) * 10;

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_A)) {
				logger::print(LogType::Debug, "A pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_B)) {
				logger::print(LogType::Debug, "B pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_X)) {
				logger::print(LogType::Debug, "X pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_Y)) {
				logger::print(LogType::Debug, "Y pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_UP)) {
				logger::print(LogType::Debug, "DPad up pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_DOWN)) {
				logger::print(LogType::Debug, "DPad down pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_LEFT)) {
				logger::print(LogType::Debug, "DPad left pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_RIGHT)) {
				logger::print(LogType::Debug, "DPad right pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_BACK)) {
				logger::print(LogType::Debug, "back pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_GUIDE)) {
				logger::print(LogType::Debug, "guide pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_START)) {
				logger::print(LogType::Debug, "start pressed\n");
				return 1;
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_LEFT_SHOULDER)) {
				logger::print(LogType::Debug, "Left bumper pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_RIGHT_SHOULDER)) {
				logger::print(LogType::Debug, "Right bumper pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_LEFT_STICK)) {
				logger::print(LogType::Debug, "Left stick pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_RIGHT_STICK)) {
				logger::print(LogType::Debug, "Right stick pressed\n");
			}

			if (!gamepad->connected) {
				gamepad = nullptr;
			}
		} else {
			gamepad = input::input::getGamepad();
			if (gamepad != nullptr) {
				logger::printf(LogType::Debug, "Gamepad \"%s\" chosen for input\n", gamepad->deviceName);
			}
		}

		movement.normalize() *= deltaTime;

		player.transform.position += movement;

		player.update();
		return 0;
	}

	static void destroy() {
		player.deinit();
		engine.renderer->destroyPostProcess(postprocess);
	}
};

int main(int argc, char** argv) {
	engine.setGame(Game::init, Game::update, Game::destroy);
	return engine.main();
}
