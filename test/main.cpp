#include <kengine/kengine.hpp>
#include <kengine/graphics/renderable.hpp>
#include <kengine/graphics/mesh.hpp>
#include <kengine/util/time.hpp>
#include <kengine/input/input.hpp>
#include <kengine/input/gamepad.hpp>
#include <kengine/logger.hpp>

#include <iostream>

#define M_PI 3.14159265358979323846

using namespace kengine;

static KEngine engine = KEngine();

struct Transform {
	util::Vector<f32, 3> position;
	util::Vector<f32, 3> rotation;
	util::Vector<f32, 3> scale;

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
		engine.renderer->uploadRenderableMesh(renderable, &mesh);
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

	static int init() {
		engine.camera.isOrthographic = false;
		engine.camera.position[2] = 0;

		player.init();
		player.transform.scale = 0.05f;
		player.transform.position[2] = -1;

		graphics::Mesh mesh = graphics::Mesh();
		mesh.vertices = {
			{ util::Vector<f32, 3>{ -1, -1, 0 }, util::Vector<f32, 3>{ 0, 0, 1 }, util::Vector<f32, 3>{ 1, 1, 0 }  },
			{ util::Vector<f32, 3>{  1, -1, 0 }, util::Vector<f32, 3>{ 0, 0, 1 }, util::Vector<f32, 3>{ 1, 0, 1 }  },
			{ util::Vector<f32, 3>{ -1,  1, 0 }, util::Vector<f32, 3>{ 0, 0, 1 }, util::Vector<f32, 3>{ 0, 1, 1 }  },
		};

		player.mesh(mesh);

		gamepad = input::input::getGamepad();

		return 0;
	}

	static int update(f32 deltaTime) {
		util::Vector<f32, 3> movement = {0,0,0};
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
			engine.camera.position[2] += deltaTime;
		}
		if (input::input::isKey(input::InputKey::KEY_DOWN)) {
			engine.camera.position[2] -= deltaTime;
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
				logger::print(LogType::DEBUG, "A pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_B)) {
				logger::print(LogType::DEBUG, "B pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_X)) {
				logger::print(LogType::DEBUG, "X pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_Y)) {
				logger::print(LogType::DEBUG, "Y pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_UP)) {
				logger::print(LogType::DEBUG, "DPad up pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_DOWN)) {
				logger::print(LogType::DEBUG, "DPad down pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_LEFT)) {
				logger::print(LogType::DEBUG, "DPad left pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_DPAD_RIGHT)) {
				logger::print(LogType::DEBUG, "DPad right pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_BACK)) {
				logger::print(LogType::DEBUG, "back pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_GUIDE)) {
				logger::print(LogType::DEBUG, "guide pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_START)) {
				logger::print(LogType::DEBUG, "start pressed\n");
				return 1;
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_LEFT_SHOULDER)) {
				logger::print(LogType::DEBUG, "Left bumper pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_RIGHT_SHOULDER)) {
				logger::print(LogType::DEBUG, "Right bumper pressed\n");
			}

			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_LEFT_STICK)) {
				logger::print(LogType::DEBUG, "Left stick pressed\n");
			}
			if (gamepad->isButtonDown(input::GamepadButton::BUTTON_RIGHT_STICK)) {
				logger::print(LogType::DEBUG, "Right stick pressed\n");
			}

			if (!gamepad->connected) {
				gamepad = nullptr;
			}
		} else {
			gamepad = input::input::getGamepad();
			if (gamepad != nullptr) {
				logger::printf(LogType::DEBUG, "Gamepad \"%s\" chosen for input\n", gamepad->deviceName);
			}
		}

		movement.normalize() *= deltaTime;

		player.transform.position += movement;

		player.update();
		return 0;
	}

	static void destroy() {
		player.deinit();
	}
};

int main(int argc, char** argv) {
	engine.setGame(Game::init, Game::update, Game::destroy);
	return engine.main();
}
